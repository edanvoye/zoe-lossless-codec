// Copyright (c) 2014, Activision
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification, are permitted provided 
// that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this list of conditions and 
//    the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and 
// the following disclaimer in the documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or 
// promote products derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED 
// WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A 
// PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR 
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR 
// TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
// ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Revision History:
//  2014-03-28 E. Danvoye    Initial Release
//

#include "stdafx.h"

#include "huffman.h"

#include <algorithm>
#include <assert.h>

// Manual instantiation of template
template class ZoeHuffmanCodec<char, 8, 1>;
template class ZoeHuffmanCodec<char, 8, 2>;
template class ZoeHuffmanCodec<char, 8, 3>;
template class ZoeHuffmanCodec<char, 8, 4>;
template class ZoeHuffmanCodec<short, 10, 1>;
template class ZoeHuffmanCodec<short, 12, 1>;

struct HuffNode {
	unsigned freq;
	unsigned left;
	unsigned right;
};

struct StoredTreeNode
{
    StoredTreeNode() : left(-1), right(-1) {}

    unsigned short left;
    unsigned short right;
};

static void buildHuffFromNode(unsigned* huff_bits, unsigned* huff_length, const HuffNode& node, const HuffNode* nodes, unsigned depth, unsigned code)
{
	// Left
	unsigned left_code = (code << 1);
	if (node.left>=0x8000)
	{
		buildHuffFromNode(huff_bits, huff_length, nodes[node.left-0x8000], nodes, depth+1, left_code);
	}
	else
	{
		// logMessage("L Symbol:%d Depth:%d Len:%d Code:%s\n",node.left,depth,depth+1,binstr(left_code,depth+1));
		huff_bits[node.left] = left_code;
		huff_length[node.left] = depth+1;
	}

	// Right
	unsigned right_code = (code << 1) | 1;
	if (node.right>=0x8000)
	{
		buildHuffFromNode(huff_bits, huff_length, nodes[node.right-0x8000], nodes, depth+1, right_code);
	}
	else
	{
		// logMessage("R Symbol:%d Depth:%d Len:%d Code:%s\n",node.right,depth,depth+1,binstr(right_code,depth+1));
		huff_bits[node.right] = right_code;
		huff_length[node.right] = depth+1;
	}
}

template <typename T, int UsedBits>
void buildHuffmanTables(std::pair<int, unsigned>* char_count, int& char_count_used, unsigned* huff_bits, unsigned* huff_length, int& storedTreeUsed, StoredTreeNode* storedTree)
{
	// Build Huffmann tree
	HuffNode huffNodes[(1<<UsedBits) * 2]; // TODO verify if we could use less... maybe the limit is 1<<UsedBits ?
	int usedHuffNodes = 0;
	while (char_count_used>1)
	{
		int rootCount = char_count_used;
		unsigned newNodeIndex = usedHuffNodes++;

		// Create node from two smallest frequencies
		HuffNode * newNode = &huffNodes[newNodeIndex];
		newNode->freq = char_count[rootCount-2].second + char_count[rootCount-1].second;
		newNode->left = char_count[rootCount-2].first;
		newNode->right =char_count[rootCount-1].first; 

		// remove two items that have just been parented under the node
		char_count_used-=2;

		// add this new node to the tree
		char_count[char_count_used++] = std::make_pair(0x8000+newNodeIndex,newNode->freq);

		// Make sure the order is still preserved after this insertion
		for (size_t i=char_count_used-1;i>0;i--)
		{ 
			if (char_count[i].second<=char_count[i-1].second)
				break;
			std::swap(char_count[i], char_count[i-1]);
		}
	}

    // Build tree to be included in stream
    {
        storedTreeUsed = usedHuffNodes;
        for (int i=0;i<usedHuffNodes;i++)
        {
            storedTree[i].left = huffNodes[i].left;
            storedTree[i].right = huffNodes[i].right;
        }
    }

	// Build huffman tables (length+bits) from HuffMann tree
	{
		int rootHuffNodeIndex = char_count[0].first - 0x8000;
		buildHuffFromNode(huff_bits, huff_length, huffNodes[rootHuffNodeIndex], huffNodes, 0, 0);
	}
}

template <typename T, int UsedBits, int Channels>
ZoeHuffmanCodec<T, UsedBits, Channels>::ZoeHuffmanCodec(int width, int height)
	: image_width(width),
	  image_height(height)
{
}

template <typename T, int UsedBits, int Channels>
template <typename ReaderT>
unsigned ZoeHuffmanCodec<T, UsedBits, Channels>::encode(const T * image_src, char * image_dest)
{
	// Character usage count
    for (int c=0;c<Channels;c++)
	    for (int i=0;i<(1<<UsedBits);i++)
		    encoder_data[c].char_count[i] = std::make_pair(i,0);

    ReaderT reader(image_src);
		
	// Run predictor + accumulate usage stats
	for (int y=0;y<image_height;y++)
	{
        T prev[Channels] = {0};
		for (int i=0;i<image_width*Channels;i++)
		{
            const int c = i%Channels;
			const T b = reader.next();
			const T d = (b-prev[c]); // Simple left-predictor
            
			std::make_unsigned<T>::type du = ((std::make_unsigned<T>::type)d)&BitMask;

			encoder_data[c].char_count[du].second++;
			prev[c] = b;
		}
	}

    size_t compressed_size = 0;

    for (int c=0;c<Channels;c++)
    {
        // Sort symbol frequency
        std::sort(&encoder_data[c].char_count[0], &encoder_data[c].char_count[encoder_data[c].char_count_used], 
            [](std::pair<int, unsigned>& a, std::pair<int, unsigned>& b){return a.second > b.second;});

        // remove symbols with zero occurrences
        while (encoder_data[c].char_count[encoder_data[c].char_count_used-1].second==0)
            encoder_data[c].char_count_used--;

        // Store huffman tables in the compressed stream
        StoredTreeNode storedTree[(1<<UsedBits) * 2]; // TODO check if we use all of them, maybe the size should be 1<<UsedBits
        int storedTreeUsed = 0;

        // Build Huffman tables from stats
    	buildHuffmanTables<T, UsedBits>(encoder_data[c].char_count, encoder_data[c].char_count_used, encoder_data[c].huff_bits, encoder_data[c].huff_length, storedTreeUsed, storedTree);

        // Store huffman tables in the compressed stream
        *((unsigned int *)&image_dest[compressed_size]) = storedTreeUsed;
        compressed_size+= 4;
        *((unsigned int *)&image_dest[compressed_size]) = encoder_data[c].char_count[0].first - 0x8000; // root index
        compressed_size+= 4;
        memcpy(&image_dest[compressed_size], storedTree, sizeof(StoredTreeNode)*storedTreeUsed);
        compressed_size += sizeof(StoredTreeNode)*storedTreeUsed;
    }

	// For each line, build compressed stream by concatenating bits
	BitPacker<unsigned> bitPacker(&image_dest[compressed_size]);

    reader.reset();

	for (int y=0;y<image_height;y++)
	{
        T prev[Channels] = {0};
		for (int x=0;x<image_width*Channels;x++)
		{
            const int c = x%Channels;
            const T b = reader.next();
            const T d = (b-prev[c]); // Simple left-predictor
            unsigned int du = ((unsigned int)(std::make_unsigned<T>::type)d)&BitMask;

            bitPacker.pack(encoder_data[c].huff_length[du], encoder_data[c].huff_bits[du]);
            prev[c] = b;
		}
	}
	compressed_size += bitPacker.flush();

	return (unsigned)compressed_size;
}

template <typename T>
class BitReader
{
public:
    BitReader(const char * src_ptr) : ptr((const T *)src_ptr), pos(0)
    {
        current = *ptr++;
    }
    bool next()
    {
        if (pos==sizeof(T)*8)
        {
            current = *ptr++;
            pos = 0;
        }

        bool r = (current & ((T)1<<(sizeof(T)*8-1)))!=0;
        current <<= 1;
        pos++;

        return r;
    }
private:
    const T * ptr;
    int pos;
    T current;
};

class HuffmanTree
{
public:
    HuffmanTree(int rootIndex, const StoredTreeNode * storedTree) : m_rootIndex(rootIndex), m_storedTree(storedTree), m_currentIndex(rootIndex)
    {
    }
    HuffmanTree() : m_rootIndex(0), m_storedTree(0), m_currentIndex(0)
    {
    }
    void init(int rootIndex, const StoredTreeNode * storedTree)
    {
        m_rootIndex = rootIndex; 
        m_storedTree = storedTree; 
        m_currentIndex = rootIndex;
    }
    unsigned int next(bool bit)
    {
        // return 0xFFFFFFFF if we have to continue searching
        // return char value if we read a leaf

        int side = bit?m_storedTree[m_currentIndex].right:m_storedTree[m_currentIndex].left;
        if (side<0x8000)
        {
            m_currentIndex = m_rootIndex;
            return side;
        }
        else
        {
            m_currentIndex = side-0x8000;
            return 0xFFFFFFFF;
        }
    }

private:
    int m_currentIndex;
    int m_rootIndex;
    const StoredTreeNode * m_storedTree;
};

unsigned char Clip(int clr)
{
    return (unsigned char)(clr < 0 ? 0 : ( clr > 255 ? 255 : clr ));
}

template <typename T, int UsedBits, int Channels>
template <typename To, int op>
bool ZoeHuffmanCodec<T, UsedBits, Channels>::decode(const char * image_src, To * image_dest)
{
    HuffmanTree tree[Channels];

    for (int c=0;c<Channels;c++)
    {
        // Read Huffman tables
        int storedTreeUsed = *((const unsigned int*)image_src);
        image_src += 4;
        int storedTreeRootIndex = *((const unsigned int*)image_src);
        image_src += 4;
        const StoredTreeNode * storedTree = (const StoredTreeNode *)image_src;
        image_src += sizeof(StoredTreeNode)*storedTreeUsed;

        tree[c].init(storedTreeRootIndex, storedTree);
    }

    const char * src_ptr = image_src;
    BitReader<unsigned> reader(src_ptr);

    for (int y=0;y<image_height;y++)
    {
        int output_mult = 1;
        if ((op==OutputProcessing::rgb24_to_rgb32 || op==OutputProcessing::rgb24_to_rgb32_revY || op==OutputProcessing::gray_to_rgb32 || op==OutputProcessing::uyvy_to_rgb32)&&sizeof(To)==1)
            output_mult = 4;
        if ((op==OutputProcessing::gray_to_rgb24 || op==OutputProcessing::uyvy_to_rgb24)&&sizeof(To)==1)
            output_mult = 3;
        else if (op==OutputProcessing::interleave_yuyv&&sizeof(To)==1)
            output_mult = 2;

        To * dest_ptr;

        if (op==OutputProcessing::rgb24_to_rgb32)
            dest_ptr = image_dest + y * image_width * output_mult; // no reverse-y, but 4 bytes instead of 3
        else if (op==OutputProcessing::rgb24_to_rgb32_revY)
            dest_ptr = image_dest + (image_height-y-1) * image_width * output_mult; // no reverse-y, but 4 bytes instead of 3
        else if (op==OutputProcessing::gray_to_rgb24 || op==OutputProcessing::uyvy_to_rgb24 || op==OutputProcessing::gray_to_rgb32 || op==OutputProcessing::uyvy_to_rgb32)
            dest_ptr = image_dest + (image_height-y-1) * image_width * output_mult; // reverse Y for rgb formats
        else
            dest_ptr = image_dest + y * image_width * Channels * output_mult;

        int nb_read = 0;
        T prev[Channels] = {0};
        unsigned int x;
        while (nb_read<image_width*Channels)
        {
            const int chan = nb_read%Channels;

            // advance in tree bit by bit, until leaf
            while ((x=tree[chan].next(reader.next()))==0xFFFFFFFF) {}

            prev[chan] = (T)x + prev[chan];

            std::make_unsigned<T>::type du = ((std::make_unsigned<T>::type)prev[chan])&BitMask;

            if (op==OutputProcessing::interleave_yuyv && sizeof(To)==1) 
                *dest_ptr++ = (To)0x80;
            if (op==OutputProcessing::interleave_yuyv && sizeof(To)==2) 
                du = ((du<<BitShift)&0xFF00) | 0x0080; // interleave for 10 bit 

            if (op==OutputProcessing::uyvy_to_rgb24 || op==OutputProcessing::uyvy_to_rgb32)
            {
                // Convert UYVY to RGB24 while decompressing
                *dest_ptr++ = static_cast<To>(du);

                if (nb_read%4==3) // when we have decoded UYVY, convert two RGB24 pixels
                {
                    // dest_ptr is currently pointing to the second G in RGBRGB
                    dest_ptr -= 4; // return to beginning of this pixel

                    // Convert two pixels of UYVY to two RGB24
                    const int y0 = ((unsigned char*)dest_ptr)[1] - 16;
                    const int y1 = ((unsigned char*)dest_ptr)[3] - 16;
                    const int cb = ((unsigned char*)dest_ptr)[0] - 128;
                    const int cr = ((unsigned char*)dest_ptr)[2] - 128;
                    *dest_ptr++ = Clip(( 298 * y0 + 516 * cb            + 128) >> 8); // B0
                    *dest_ptr++ = Clip(( 298 * y0 - 100 * cb - 208 * cr + 128) >> 8); // G0
                    *dest_ptr++ = Clip(( 298 * y0            + 409 * cr + 128) >> 8); // R0
                    if (op==OutputProcessing::uyvy_to_rgb32)
                        *dest_ptr++ = 0xFF;
                    *dest_ptr++ = Clip(( 298 * y1 + 516 * cb            + 128) >> 8); // B1
                    *dest_ptr++ = Clip(( 298 * y1 - 100 * cb - 208 * cr + 128) >> 8); // G1
                    *dest_ptr++ = Clip(( 298 * y1            + 409 * cr + 128) >> 8); // R1
                    if (op==OutputProcessing::uyvy_to_rgb32)
                        *dest_ptr++ = 0xFF;
                }
            }
            else
            {
                for (int c=0;c<((op==OutputProcessing::gray_to_rgb32 || op==OutputProcessing::gray_to_rgb24)?3:1);c++)
                {
                    if (sizeof(To)*8<UsedBits)
                        *dest_ptr++ = static_cast<To>((du>>(UsedBits-sizeof(To)*8))&0xFF);
                    else
                        *dest_ptr++ = static_cast<To>(du);
                }
                if (op==OutputProcessing::gray_to_rgb32)
                    *dest_ptr++ = 0xFF;
                if ((op==OutputProcessing::rgb24_to_rgb32 || op==OutputProcessing::rgb24_to_rgb32_revY) && chan==2)
                    *dest_ptr++ = 0xFF;
            }

            nb_read++;
        }
    }

    return true;
}

// Manual instantiation of template function
template bool ZoeHuffmanCodec<char, 8, 1>::decode<char, OutputProcessing::interleave_yuyv>(const char * image_src, char * image_dest);
template bool ZoeHuffmanCodec<char, 8, 1>::decode<char, OutputProcessing::Default>(const char * image_src, char * image_dest);
template bool ZoeHuffmanCodec<short, 10, 1>::decode<short, OutputProcessing::interleave_yuyv>(const char * image_src, short * image_dest);
template bool ZoeHuffmanCodec<short, 10, 1>::decode<short, OutputProcessing::Default>(const char * image_src, short * image_dest);
template bool ZoeHuffmanCodec<short, 10, 1>::decode<char, OutputProcessing::Default>(const char * image_src, char * image_dest);
template bool ZoeHuffmanCodec<char, 8, 1>::decode<char, OutputProcessing::gray_to_rgb24>(const char * image_src, char * image_dest); // Y8 decoded directly to RGB24
template bool ZoeHuffmanCodec<short, 10, 1>::decode<char, OutputProcessing::gray_to_rgb24>(const char * image_src, char * image_dest); // Y10 decoded directly to RGB24
template bool ZoeHuffmanCodec<char, 8, 2>::decode<char, OutputProcessing::Default>(const char * image_src, char * image_dest);
template bool ZoeHuffmanCodec<char, 8, 3>::decode<char, OutputProcessing::Default>(const char * image_src, char * image_dest);
template bool ZoeHuffmanCodec<char, 8, 4>::decode<char, OutputProcessing::Default>(const char * image_src, char * image_dest);
template bool ZoeHuffmanCodec<char, 8, 2>::decode<char, OutputProcessing::uyvy_to_rgb24>(const char * image_src, char * image_dest); // UYVY decoded directly to RGB24
template bool ZoeHuffmanCodec<char, 8, 3>::decode<char, OutputProcessing::rgb24_to_rgb32>(const char * image_src, char * image_dest); // RGB24 converted to RGB32
template bool ZoeHuffmanCodec<char, 8, 3>::decode<char, OutputProcessing::rgb24_to_rgb32_revY>(const char * image_src, char * image_dest); // RGB24 converted to RGB32, reverse Y
template bool ZoeHuffmanCodec<char, 8, 1>::decode<char, OutputProcessing::gray_to_rgb32>(const char * image_src, char * image_dest); // Y8 decoded directly to RGB32
template bool ZoeHuffmanCodec<short, 10, 1>::decode<char, OutputProcessing::gray_to_rgb32>(const char * image_src, char * image_dest); // Y10 decoded directly to RGB32
template bool ZoeHuffmanCodec<char, 8, 2>::decode<char, OutputProcessing::uyvy_to_rgb32>(const char * image_src, char * image_dest); // UYVY decoded directly to RGB32

template unsigned int ZoeHuffmanCodec<char,8,1>::encode<TrivialBitReader<char> >(char const *,char *);
template unsigned int ZoeHuffmanCodec<short,10,1>::encode<TrivialBitReader<short> >(short const *,char *);
template unsigned int ZoeHuffmanCodec<short,10,1>::encode<UnpackBitReader<10,short> >(short const *,char *);
template unsigned int ZoeHuffmanCodec<char,8,3>::encode<TrivialBitReader<char> >(char const *,char *);
template unsigned int ZoeHuffmanCodec<char,8,4>::encode<TrivialBitReader<char> >(char const *,char *);
template unsigned int ZoeHuffmanCodec<char,8,2>::encode<TrivialBitReader<char> >(char const *,char *);
template unsigned int ZoeHuffmanCodec<short,12,1>::encode<TrivialBitReader<short> >(short const *,char *);
template unsigned int ZoeHuffmanCodec<short,12,1>::encode<UnpackBitReader<12,short> >(short const *,char *);

template bool ZoeHuffmanCodec<short, 12, 1>::decode<short, OutputProcessing::interleave_yuyv>(const char * image_src, short * image_dest);
template bool ZoeHuffmanCodec<short, 12, 1>::decode<short, OutputProcessing::Default>(const char * image_src, short * image_dest);
template bool ZoeHuffmanCodec<short, 12, 1>::decode<char, OutputProcessing::Default>(const char * image_src, char * image_dest);
template bool ZoeHuffmanCodec<short, 12, 1>::decode<char, OutputProcessing::gray_to_rgb24>(const char * image_src, char * image_dest); // Y12 decoded directly to RGB24
template bool ZoeHuffmanCodec<short, 12, 1>::decode<char, OutputProcessing::gray_to_rgb32>(const char * image_src, char * image_dest); // Y12 decoded directly to RGB32

