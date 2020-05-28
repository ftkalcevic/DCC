// C++ and Stream conversion of upng C code.

/*
uPNG -- derived from LodePNG version 20100808

Copyright (c) 2005-2010 Lode Vandevenne
Copyright (c) 2010 Sean Middleditch

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

		1. The origin of this software must not be misrepresented; you must not
		claim that you wrote the original software. If you use this software
		in a product, an acknowledgment in the product documentation would be
		appreciated but is not required.

		2. Altered source versions must be plainly marked as such, and must not be
		misrepresented as being the original software.

		3. This notice may not be removed or altered from any source
		distribution.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <assert.h>

#include "upng.h"

#define MIN(x,y)		((x)<(y)?(x):(y))
#define MAKE_BYTE(b) ((b) & 0xFF)
#define MAKE_DWORD(a,b,c,d) ((MAKE_BYTE(a) << 24) | (MAKE_BYTE(b) << 16) | (MAKE_BYTE(c) << 8) | MAKE_BYTE(d))
#define MAKE_DWORD_PTR(p) MAKE_DWORD((p)[0], (p)[1], (p)[2], (p)[3])

#define CHUNK_IHDR MAKE_DWORD('I','H','D','R')
#define CHUNK_IDAT MAKE_DWORD('I','D','A','T')
#define CHUNK_IEND MAKE_DWORD('I','E','N','D')

#define FIRST_LENGTH_CODE_INDEX 257
#define LAST_LENGTH_CODE_INDEX 285

#define NUM_DEFLATE_CODE_SYMBOLS 288	/*256 literals, the end code, some length codes, and 2 unused codes */
#define NUM_DISTANCE_SYMBOLS 32	/*the distance codes have their own symbols, 30 used, 2 unused */
#define NUM_CODE_LENGTH_CODES 19	/*the code length codes. 0-15: code lengths, 16: copy previous 3-6 times, 17: 3-10 zeros, 18: 11-138 zeros */
#define MAX_SYMBOLS 288 /* largest number of symbols used by any tree type */

#define DEFLATE_CODE_BITLEN 15
#define DISTANCE_BITLEN 15
#define CODE_LENGTH_BITLEN 7
#define MAX_BIT_LENGTH 15 /* largest bitlen used by any tree type */

#define DEFLATE_CODE_BUFFER_SIZE (NUM_DEFLATE_CODE_SYMBOLS * 2)
#define DISTANCE_BUFFER_SIZE (NUM_DISTANCE_SYMBOLS * 2)
#define CODE_LENGTH_BUFFER_SIZE (NUM_DISTANCE_SYMBOLS * 2)

#define SET_ERROR(code) do { error = (code); error_line = __LINE__; } while (0)

#define upng_chunk_length(chunk) MAKE_DWORD_PTR(chunk)
#define upng_chunk_type(chunk) MAKE_DWORD_PTR((chunk) + 4)
#define upng_chunk_critical(chunk) (((chunk)[4] & 32) == 0)



static const unsigned LENGTH_BASE[29] = {	/*the base lengths represented by codes 257-285 */
	3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31, 35, 43, 51, 59,
	67, 83, 99, 115, 131, 163, 195, 227, 258
};

static const unsigned LENGTH_EXTRA[29] = {	/*the extra bits used by codes 257-285 (added to base length) */
	0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5,
	5, 5, 5, 0
};

static const unsigned DISTANCE_BASE[30] = {	/*the base backwards distances (the bits of distance codes appear after length codes and use their own huffman tree) */
	1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193, 257, 385, 513,
	769, 1025, 1537, 2049, 3073, 4097, 6145, 8193, 12289, 16385, 24577
};

static const unsigned DISTANCE_EXTRA[30] = {	/*the extra bits of backwards distances (added to base) */
	0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10,
	11, 11, 12, 12, 13, 13
};

static const unsigned CLCL[NUM_CODE_LENGTH_CODES]	/*the order in which "code length alphabet code lengths" are stored, out of this the huffman tree of the dynamic huffman tree lengths is generated */
= { 16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15 };

static const unsigned FIXED_DEFLATE_CODE_TREE[NUM_DEFLATE_CODE_SYMBOLS * 2] = {
	289, 370, 290, 307, 546, 291, 561, 292, 293, 300, 294, 297, 295, 296, 0, 1,
	2, 3, 298, 299, 4, 5, 6, 7, 301, 304, 302, 303, 8, 9, 10, 11, 305, 306, 12,
	13, 14, 15, 308, 339, 309, 324, 310, 317, 311, 314, 312, 313, 16, 17, 18,
	19, 315, 316, 20, 21, 22, 23, 318, 321, 319, 320, 24, 25, 26, 27, 322, 323,
	28, 29, 30, 31, 325, 332, 326, 329, 327, 328, 32, 33, 34, 35, 330, 331, 36,
	37, 38, 39, 333, 336, 334, 335, 40, 41, 42, 43, 337, 338, 44, 45, 46, 47,
	340, 355, 341, 348, 342, 345, 343, 344, 48, 49, 50, 51, 346, 347, 52, 53,
	54, 55, 349, 352, 350, 351, 56, 57, 58, 59, 353, 354, 60, 61, 62, 63, 356,
	363, 357, 360, 358, 359, 64, 65, 66, 67, 361, 362, 68, 69, 70, 71, 364,
	367, 365, 366, 72, 73, 74, 75, 368, 369, 76, 77, 78, 79, 371, 434, 372,
	403, 373, 388, 374, 381, 375, 378, 376, 377, 80, 81, 82, 83, 379, 380, 84,
	85, 86, 87, 382, 385, 383, 384, 88, 89, 90, 91, 386, 387, 92, 93, 94, 95,
	389, 396, 390, 393, 391, 392, 96, 97, 98, 99, 394, 395, 100, 101, 102, 103,
	397, 400, 398, 399, 104, 105, 106, 107, 401, 402, 108, 109, 110, 111, 404,
	419, 405, 412, 406, 409, 407, 408, 112, 113, 114, 115, 410, 411, 116, 117,
	118, 119, 413, 416, 414, 415, 120, 121, 122, 123, 417, 418, 124, 125, 126,
	127, 420, 427, 421, 424, 422, 423, 128, 129, 130, 131, 425, 426, 132, 133,
	134, 135, 428, 431, 429, 430, 136, 137, 138, 139, 432, 433, 140, 141, 142,
	143, 435, 483, 436, 452, 568, 437, 438, 445, 439, 442, 440, 441, 144, 145,
	146, 147, 443, 444, 148, 149, 150, 151, 446, 449, 447, 448, 152, 153, 154,
	155, 450, 451, 156, 157, 158, 159, 453, 468, 454, 461, 455, 458, 456, 457,
	160, 161, 162, 163, 459, 460, 164, 165, 166, 167, 462, 465, 463, 464, 168,
	169, 170, 171, 466, 467, 172, 173, 174, 175, 469, 476, 470, 473, 471, 472,
	176, 177, 178, 179, 474, 475, 180, 181, 182, 183, 477, 480, 478, 479, 184,
	185, 186, 187, 481, 482, 188, 189, 190, 191, 484, 515, 485, 500, 486, 493,
	487, 490, 488, 489, 192, 193, 194, 195, 491, 492, 196, 197, 198, 199, 494,
	497, 495, 496, 200, 201, 202, 203, 498, 499, 204, 205, 206, 207, 501, 508,
	502, 505, 503, 504, 208, 209, 210, 211, 506, 507, 212, 213, 214, 215, 509,
	512, 510, 511, 216, 217, 218, 219, 513, 514, 220, 221, 222, 223, 516, 531,
	517, 524, 518, 521, 519, 520, 224, 225, 226, 227, 522, 523, 228, 229, 230,
	231, 525, 528, 526, 527, 232, 233, 234, 235, 529, 530, 236, 237, 238, 239,
	532, 539, 533, 536, 534, 535, 240, 241, 242, 243, 537, 538, 244, 245, 246,
	247, 540, 543, 541, 542, 248, 249, 250, 251, 544, 545, 252, 253, 254, 255,
	547, 554, 548, 551, 549, 550, 256, 257, 258, 259, 552, 553, 260, 261, 262,
	263, 555, 558, 556, 557, 264, 265, 266, 267, 559, 560, 268, 269, 270, 271,
	562, 565, 563, 564, 272, 273, 274, 275, 566, 567, 276, 277, 278, 279, 569,
	572, 570, 571, 280, 281, 282, 283, 573, 574, 284, 285, 286, 287, 0, 0
};

static const unsigned FIXED_DISTANCE_TREE[NUM_DISTANCE_SYMBOLS * 2] = {
	33, 48, 34, 41, 35, 38, 36, 37, 0, 1, 2, 3, 39, 40, 4, 5, 6, 7, 42, 45, 43,
	44, 8, 9, 10, 11, 46, 47, 12, 13, 14, 15, 49, 56, 50, 53, 51, 52, 16, 17,
	18, 19, 54, 55, 20, 21, 22, 23, 57, 60, 58, 59, 24, 25, 26, 27, 61, 62, 28,
	29, 30, 31, 0, 0
};

 upng_error upng::read_bit(unsigned long *bitpointer, unsigned char &bit)
{
	if (*bitpointer == 0)
	{
		uint16_t bytesRead;
		ReadBytes(&currentByte,1,bytesRead);
		if (bytesRead != 1)
		{
			SET_ERROR(UPNG_EMALFORMED);
			return UPNG_EMALFORMED;
		}
	}

	bit = (unsigned char)((currentByte >> *bitpointer) & 1);
	(*bitpointer)++;
	if (*bitpointer == 8)
		*bitpointer = 0;

	return UPNG_EOK;
}

 upng_error upng::read_bits(unsigned long *bitpointer, unsigned long nbits, unsigned &bits)
{
	unsigned i;
	bits = 0;
	for (i = 0; i < nbits; i++)
	{
		uint8_t bit;
		upng_error error = read_bit(bitpointer,bit);
		if (error != UPNG_EOK)
			return error;

		bits |= bit << i;
	}
	return UPNG_EOK;
}

/* the buffer must be numcodes*2 in size! */
static void huffman_tree_init(huffman_tree* tree, unsigned* buffer, unsigned numcodes, unsigned maxbitlen)
{
	tree->tree2d = buffer;

	tree->numcodes = numcodes;
	tree->maxbitlen = maxbitlen;
}

/*given the code lengths (as stored in the PNG file), generate the tree as defined by Deflate. maxbitlen is the maximum bits that a code in the tree can have. return value is error.*/
void upng::huffman_tree_create_lengths(huffman_tree* tree, const unsigned *bitlen)
{
	unsigned tree1d[MAX_SYMBOLS];
	unsigned blcount[MAX_BIT_LENGTH];
	unsigned nextcode[MAX_BIT_LENGTH+1];
	unsigned bits, n, i;
	unsigned nodefilled = 0;	/*up to which node it is filled */
	unsigned treepos = 0;	/*position in the tree (1 of the numcodes columns) */

	/* initialize local vectors */
	memset(blcount, 0, sizeof(blcount));
	memset(nextcode, 0, sizeof(nextcode));

	/*step 1: count number of instances of each code length */
	for (bits = 0; bits < tree->numcodes; bits++) {
		blcount[bitlen[bits]]++;
	}

	/*step 2: generate the nextcode values */
	for (bits = 1; bits <= tree->maxbitlen; bits++) {
		nextcode[bits] = (nextcode[bits - 1] + blcount[bits - 1]) << 1;
	}

	/*step 3: generate all the codes */
	for (n = 0; n < tree->numcodes; n++) {
		if (bitlen[n] != 0) {
			tree1d[n] = nextcode[bitlen[n]]++;
		}
	}

	/*convert tree1d[] to tree2d[][]. In the 2D array, a value of 32767 means uninited, a value >= numcodes is an address to another bit, a value < numcodes is a code. The 2 rows are the 2 possible bit values (0 or 1), there are as many columns as codes - 1
	   a good huffmann tree has N * 2 - 1 nodes, of which N - 1 are internal nodes. Here, the internal nodes are stored (what their 0 and 1 option point to). There is only memory for such good tree currently, if there are more nodes (due to too long length codes), error 55 will happen */
	for (n = 0; n < tree->numcodes * 2; n++) {
		tree->tree2d[n] = 32767;	/*32767 here means the tree2d isn't filled there yet */
	}

	for (n = 0; n < tree->numcodes; n++) {	/*the codes */
		for (i = 0; i < bitlen[n]; i++) {	/*the bits for this code */
			unsigned char bit = (unsigned char)((tree1d[n] >> (bitlen[n] - i - 1)) & 1);
			/* check if oversubscribed */
			if (treepos > tree->numcodes - 2) {
				SET_ERROR(UPNG_EMALFORMED);
				return;
			}

			if (tree->tree2d[2 * treepos + bit] == 32767) {	/*not yet filled in */
				if (i + 1 == bitlen[n]) {	/*last bit */
					tree->tree2d[2 * treepos + bit] = n;	/*put the current code in it */
					treepos = 0;
				} else {	/*put address of the next step in here, first that address has to be found of course (it's just nodefilled + 1)... */
					nodefilled++;
					tree->tree2d[2 * treepos + bit] = nodefilled + tree->numcodes;	/*addresses encoded with numcodes added to it */
					treepos = nodefilled;
				}
			} else {
				treepos = tree->tree2d[2 * treepos + bit] - tree->numcodes;
			}
		}
	}

	for (n = 0; n < tree->numcodes * 2; n++) {
		if (tree->tree2d[n] == 32767) {
			tree->tree2d[n] = 0;	/*remove possible remaining 32767's */
		}
	}
}

unsigned upng::huffman_decode_symbol(unsigned long *bp, const huffman_tree* codetree)
{
	unsigned treepos = 0, ct;
	unsigned char bit;
	for (;;) {
		if (read_bit(bp, bit) != UPNG_EOK)
			return 0;

		ct = codetree->tree2d[(treepos << 1) | bit];
		if (ct < codetree->numcodes) {
			return ct;
		}

		treepos = ct - codetree->numcodes;
		if (treepos >= codetree->numcodes) {
			SET_ERROR(UPNG_EMALFORMED);
			return 0;
		}
	}
}

/* get the tree of a deflated block with dynamic tree, the tree itself is also Huffman compressed with a known tree*/
void upng::get_tree_inflate_dynamic(huffman_tree* codetree, huffman_tree* codetreeD, huffman_tree* codelengthcodetree, unsigned long *bp)
{
	unsigned codelengthcode[NUM_CODE_LENGTH_CODES];
	unsigned bitlen[NUM_DEFLATE_CODE_SYMBOLS];
	unsigned bitlenD[NUM_DISTANCE_SYMBOLS];
	unsigned n, hlit, hdist, hclen, i;

	/*make sure that length values that aren't filled in will be 0, or a wrong tree will be generated */
	/*C-code note: use no "return" between ctor and dtor of an uivector! */
	/* clear bitlen arrays */
	memset(bitlen, 0, sizeof(bitlen));
	memset(bitlenD, 0, sizeof(bitlenD));

	/*the bit pointer is or will go past the memory */
	unsigned bits;
	if (read_bits(bp, 5, bits) != UPNG_EOK) return;
	hlit = bits + 257;	/*number of literal/length codes + 257. Unlike the spec, the value 257 is added to it here already */
	if (read_bits(bp, 5, bits) != UPNG_EOK) return;
	hdist = bits + 1;	/*number of distance codes. Unlike the spec, the value 1 is added to it here already */
	if (read_bits(bp, 4, bits) != UPNG_EOK) return;
	hclen = bits + 4;	/*number of code length codes. Unlike the spec, the value 4 is added to it here already */

	for (i = 0; i < NUM_CODE_LENGTH_CODES; i++) {
		if (i < hclen) {
			if (read_bits(bp, 3, bits) != UPNG_EOK) return;
			codelengthcode[CLCL[i]] = bits;
		} else {
			codelengthcode[CLCL[i]] = 0;	/*if not, it must stay 0 */
		}
	}

	huffman_tree_create_lengths(codelengthcodetree, codelengthcode);

	/* bail now if we encountered an error earlier */
	if (error != UPNG_EOK) {
		return;
	}

	/*now we can use this tree to read the lengths for the tree that this function will return */
	i = 0;
	while (i < hlit + hdist) {	/*i is the current symbol we're reading in the part that contains the code lengths of lit/len codes and dist codes */
		unsigned code = huffman_decode_symbol(bp, codelengthcodetree);
		if (error != UPNG_EOK) {
			break;
		}

		if (code <= 15) {	/*a length code */
			if (i < hlit) {
				bitlen[i] = code;
			} else {
				bitlenD[i - hlit] = code;
			}
			i++;
		} else if (code == 16) {	/*repeat previous */
			unsigned replength = 3;	/*read in the 2 bits that indicate repeat length (3-6) */
			unsigned value;	/*set value to the previous code */

			/*error, bit pointer jumps past memory */
			if (read_bits(bp, 2, bits) != UPNG_EOK) return;
			replength += bits;

			if ((i - 1) < hlit) {
				value = bitlen[i - 1];
			} else {
				value = bitlenD[i - hlit - 1];
			}

			/*repeat this value in the next lengths */
			for (n = 0; n < replength; n++) {
				/* i is larger than the amount of codes */
				if (i >= hlit + hdist) {
					SET_ERROR(UPNG_EMALFORMED);
					break;
				}

				if (i < hlit) {
					bitlen[i] = value;
				} else {
					bitlenD[i - hlit] = value;
				}
				i++;
			}
		} else if (code == 17) {	/*repeat "0" 3-10 times */
			unsigned replength = 3;	/*read in the bits that indicate repeat length */

			/*error, bit pointer jumps past memory */
			if (read_bits(bp, 3, bits) != UPNG_EOK) return;
			replength += bits;

			/*repeat this value in the next lengths */
			for (n = 0; n < replength; n++) {
				/* error: i is larger than the amount of codes */
				if (i >= hlit + hdist) {
					SET_ERROR(UPNG_EMALFORMED);
					break;
				}

				if (i < hlit) {
					bitlen[i] = 0;
				} else {
					bitlenD[i - hlit] = 0;
				}
				i++;
			}
		} else if (code == 18) {	/*repeat "0" 11-138 times */
			unsigned replength = 11;	/*read in the bits that indicate repeat length */
			/* error, bit pointer jumps past memory */

			if (read_bits(bp, 7, bits) != UPNG_EOK) return;
			replength += bits;

			/*repeat this value in the next lengths */
			for (n = 0; n < replength; n++) {
				/* i is larger than the amount of codes */
				if (i >= hlit + hdist) {
					SET_ERROR(UPNG_EMALFORMED);
					break;
				}
				if (i < hlit)
					bitlen[i] = 0;
				else
					bitlenD[i - hlit] = 0;
				i++;
			}
		} else {
			/* somehow an unexisting code appeared. This can never happen. */
			SET_ERROR(UPNG_EMALFORMED);
			break;
		}
	}

	if (error == UPNG_EOK && bitlen[256] == 0) {
		SET_ERROR(UPNG_EMALFORMED);
	}

	/*the length of the end code 256 must be larger than 0 */
	/*now we've finally got hlit and hdist, so generate the code trees, and the function is done */
	if (error == UPNG_EOK) {
		huffman_tree_create_lengths(codetree, bitlen);
	}
	if (error == UPNG_EOK) {
		huffman_tree_create_lengths(codetreeD, bitlenD);
	}
}

/*inflate a block with dynamic of fixed Huffman tree*/
void upng::inflate_huffman(unsigned char* out, unsigned long outsize, unsigned long *bp, unsigned long *pos, unsigned btype)
{
	unsigned codetree_buffer[DEFLATE_CODE_BUFFER_SIZE];
	unsigned codetreeD_buffer[DISTANCE_BUFFER_SIZE];
	unsigned done = 0;

	huffman_tree codetree;
	huffman_tree codetreeD;

	if (btype == 1) {
		/* fixed trees */
		huffman_tree_init(&codetree, (unsigned*)FIXED_DEFLATE_CODE_TREE, NUM_DEFLATE_CODE_SYMBOLS, DEFLATE_CODE_BITLEN);
		huffman_tree_init(&codetreeD, (unsigned*)FIXED_DISTANCE_TREE, NUM_DISTANCE_SYMBOLS, DISTANCE_BITLEN);
	} else if (btype == 2) {
		/* dynamic trees */
		unsigned codelengthcodetree_buffer[CODE_LENGTH_BUFFER_SIZE];
		huffman_tree codelengthcodetree;

		huffman_tree_init(&codetree, codetree_buffer, NUM_DEFLATE_CODE_SYMBOLS, DEFLATE_CODE_BITLEN);
		huffman_tree_init(&codetreeD, codetreeD_buffer, NUM_DISTANCE_SYMBOLS, DISTANCE_BITLEN);
		huffman_tree_init(&codelengthcodetree, codelengthcodetree_buffer, NUM_CODE_LENGTH_CODES, CODE_LENGTH_BITLEN);
		get_tree_inflate_dynamic(&codetree, &codetreeD, &codelengthcodetree, bp);
	}

	while (done == 0) {
		unsigned code = huffman_decode_symbol(bp, &codetree);
		if (error != UPNG_EOK) {
			return;
		}

		if (code == 256) {
			/* end code */
			done = 1;
		} else if (code <= 255) {
			/* literal symbol */
			if ((*pos) >= outsize) {
				SET_ERROR(UPNG_EMALFORMED);
				return;
			}

			/* store output */
			out[(*pos)++] = (unsigned char)(code);
		} else if (code >= FIRST_LENGTH_CODE_INDEX && code <= LAST_LENGTH_CODE_INDEX) {	/*length code */
			/* part 1: get length base */
			unsigned long length = LENGTH_BASE[code - FIRST_LENGTH_CODE_INDEX];
			unsigned codeD, distance, numextrabitsD;
			unsigned long start, forward, backward, numextrabits;

			/* part 2: get extra bits and add the value of that to length */
			numextrabits = LENGTH_EXTRA[code - FIRST_LENGTH_CODE_INDEX];

			/* error, bit pointer will jump past memory */
			unsigned bits;
			if (read_bits(bp, numextrabits, bits) != UPNG_EOK) return;
			length += bits;

			/*part 3: get distance code */
			codeD = huffman_decode_symbol(bp, &codetreeD);
			if (error != UPNG_EOK) {
				return;
			}

			/* invalid distance code (30-31 are never used) */
			if (codeD > 29) {
				SET_ERROR(UPNG_EMALFORMED);
				return;
			}

			distance = DISTANCE_BASE[codeD];

			/*part 4: get extra bits from distance */
			numextrabitsD = DISTANCE_EXTRA[codeD];

			/* error, bit pointer will jump past memory */

			if (read_bits(bp, numextrabitsD, bits) != UPNG_EOK) return;
			distance += bits;

			/*part 5: fill in all the out[n] values based on the length and dist */
			start = (*pos);
			backward = start - distance;

			if ((*pos) + length > outsize) {
				SET_ERROR(UPNG_EMALFORMED);
				return;
			}

			for (forward = 0; forward < length; forward++) {
				out[(*pos)++] = out[backward];
				backward++;

				if (backward >= start) {
					backward = start - distance;
				}
			}
		}
	}
}

void upng::inflate_uncompressed(unsigned char* out, unsigned long outsize, unsigned long *bp, unsigned long *pos)
{
	unsigned len, nlen;

	/* go to first boundary of byte */
	*bp = 0;

	///* read len (2 bytes) and nlen (2 bytes) */
	unsigned bits;
	if (read_bits(bp, 16, bits) != UPNG_EOK) return;
	len = bits;
	if (read_bits(bp, 16, bits) != UPNG_EOK) return;
	nlen = bits;

	/* check if 16-bit nlen is really the one's complement of len */
	if (len + nlen != 65535) {
		SET_ERROR(UPNG_EMALFORMED);
		return;
	}

	uint16_t bytesRead;
	stream->ReadBytes(out + *pos, len, bytesRead);
	(*pos) += len; 

	/* read the literal data: len bytes are now stored in the out buffer */
	if (bytesRead != len ) {
		SET_ERROR(UPNG_EMALFORMED);
		return;
	}
}

/*inflate the deflated data (cfr. deflate spec); return value is the error*/
upng_error upng::uz_inflate_data(unsigned char* out, unsigned long outsize)
{
	unsigned long bp = 0;	/*bit pointer in the "in" data, current byte is bp >> 3, current bit is bp & 0x7 (from lsb to msb of the byte) */
	unsigned long pos = 0;	/*byte position in the out buffer */

	uint8_t done = 0;

	while (done == 0) {
		unsigned btype;

		/* read block control bits */
		if (read_bit(&bp, done) != UPNG_EOK) return error;
		uint8_t b1, b2;
		if (read_bit(&bp, b1) != UPNG_EOK) return error;
		if (read_bit(&bp, b2) != UPNG_EOK) return error;
		btype = b1 | (b2 << 1);

		/* process control type appropriateyly */
		if (btype == 3) {
			SET_ERROR(UPNG_EMALFORMED);
			return error;
		} else if (btype == 0) {
			inflate_uncompressed(out, outsize, &bp, &pos );	/*no compression */
		} else {
			inflate_huffman(out, outsize, &bp, &pos, btype);	/*compression, btype 01 or 10 */
		}

		/* stop if an error has occured */
		if (error != UPNG_EOK) {
			return error;
		}
	}

	return error;
}

upng_error upng::ReadBytes(uint8_t* buffer, uint16_t len, uint16_t& actualBytesRead)
{
	/* scan through the chunks, finding the size of all IDAT chunks, and also
	 * verify general well-formed-ness */
	actualBytesRead = 0;
	while (actualBytesRead < len)
	{
		if (chunkLen == 0)
		{
			for (;; )
			{
				/* get length; sanity check it */
				uint8_t chunkHeader[8];
				uint16_t bytesRead;
				stream->ReadBytes(chunkHeader, sizeof(chunkHeader), bytesRead);
				chunkLen = upng_chunk_length(chunkHeader);
				if (chunkLen > INT_MAX) {
					SET_ERROR(UPNG_EMALFORMED);
					return error;
				}

				/* parse chunks */
				uint32_t chunkType = upng_chunk_type(chunkHeader);
				if (chunkType == CHUNK_IDAT) {
					break;
				}
				else if (chunkType == CHUNK_IEND) {
					SET_ERROR(UPNG_EMALFORMED);
					return error;
				}
				else if (upng_chunk_critical(chunkHeader)) {
					SET_ERROR(UPNG_EUNSUPPORTED);
					return error;
				}
				else
				{
					// Skip bytes
					stream->Seek(chunkLen, SEEK_CUR);
					chunkLen = 0;
					break;
				}
			}
		}

		if (chunkLen != 0)
		{
			// We are only ever asked for 1 or 2 bytes at a time
			uint16_t bytesRead;
			stream->ReadBytes(buffer, 1, bytesRead);
			if ( bytesRead != 1) {
				SET_ERROR(UPNG_EMALFORMED);
				return error;
			}
			chunkLen--;
			buffer++;
			actualBytesRead++;
		}
		if (chunkLen == 0)
		{
			uint32_t crc;
			uint16_t bytesRead;
			stream->ReadBytes((uint8_t*)&crc, sizeof(crc), bytesRead);
		}
	}
	return UPNG_EOK;
}

upng_error upng::uz_inflate(unsigned char *out, unsigned long outsize)
{
	uint8_t in[2];
	uint16_t bytesRead;
	ReadBytes(in, sizeof(in), bytesRead);

	/* we require two bytes for the zlib data header */
	if (bytesRead !=2) {
		SET_ERROR(UPNG_EMALFORMED);
		return error;
	}

	/* 256 * in[0] + in[1] must be a multiple of 31, the FCHECK value is supposed to be made that way */
	if ((in[0] * 256 + in[1]) % 31 != 0) {
		SET_ERROR(UPNG_EMALFORMED);
		return error;
	}

	/*error: only compression method 8: inflate with sliding window of 32k is supported by the PNG spec */
	if ((in[0] & 15) != 8 || ((in[0] >> 4) & 15) > 7) {
		SET_ERROR(UPNG_EMALFORMED);
		return error;
	}

	/* the specification of PNG says about the zlib stream: "The additional flags shall not specify a preset dictionary." */
	if (((in[1] >> 5) & 1) != 0) {
		SET_ERROR(UPNG_EMALFORMED);
		return error;
	}

	/* create output buffer */
	uz_inflate_data(out, outsize);

	return error;
}

/*Paeth predicter, used by PNG filter type 4*/
static int paeth_predictor(int a, int b, int c)
{
	int p = a + b - c;
	int pa = p > a ? p - a : a - p;
	int pb = p > b ? p - b : b - p;
	int pc = p > c ? p - c : c - p;

	if (pa <= pb && pa <= pc)
		return a;
	else if (pb <= pc)
		return b;
	else
		return c;
}

void upng::unfilter_scanline(unsigned char *recon, const unsigned char *scanline, const unsigned char *precon, unsigned long bytewidth, unsigned char filterType, unsigned long length)
{
	/*
	   For PNG filter method 0
	   unfilter a PNG image scanline by scanline. when the pixels are smaller than 1 byte, the filter works byte per byte (bytewidth = 1)
	   precon is the previous unfiltered scanline, recon the result, scanline the current one
	   the incoming scanlines do NOT include the filtertype byte, that one is given in the parameter filterType instead
	   recon and scanline MAY be the same memory address! precon must be disjoint.
	 */

	unsigned long i;
	switch (filterType) {
	case 0:
		for (i = 0; i < length; i++)
			recon[i] = scanline[i];
		break;
	case 1:
		for (i = 0; i < bytewidth; i++)
			recon[i] = scanline[i];
		for (i = bytewidth; i < length; i++)
			recon[i] = scanline[i] + recon[i - bytewidth];
		break;
	case 2:
		if (precon)
			for (i = 0; i < length; i++)
				recon[i] = scanline[i] + precon[i];
		else
			for (i = 0; i < length; i++)
				recon[i] = scanline[i];
		break;
	case 3:
		if (precon) {
			for (i = 0; i < bytewidth; i++)
				recon[i] = scanline[i] + precon[i] / 2;
			for (i = bytewidth; i < length; i++)
				recon[i] = scanline[i] + ((recon[i - bytewidth] + precon[i]) / 2);
		} else {
			for (i = 0; i < bytewidth; i++)
				recon[i] = scanline[i];
			for (i = bytewidth; i < length; i++)
				recon[i] = scanline[i] + recon[i - bytewidth] / 2;
		}
		break;
	case 4:
		if (precon) {
			for (i = 0; i < bytewidth; i++)
				recon[i] = (unsigned char)(scanline[i] + paeth_predictor(0, precon[i], 0));
			for (i = bytewidth; i < length; i++)
				recon[i] = (unsigned char)(scanline[i] + paeth_predictor(recon[i - bytewidth], precon[i], precon[i - bytewidth]));
		} else {
			for (i = 0; i < bytewidth; i++)
				recon[i] = scanline[i];
			for (i = bytewidth; i < length; i++)
				recon[i] = (unsigned char)(scanline[i] + paeth_predictor(recon[i - bytewidth], 0, 0));
		}
		break;
	default:
		SET_ERROR(UPNG_EMALFORMED);
		break;
	}
}

void upng::unfilter(unsigned char *out, const unsigned char *in, unsigned w, unsigned h, unsigned bpp)
{
	/*
	   For PNG filter method 0
	   this function unfilters a single image (e.g. without interlacing this is called once, with Adam7 it's called 7 times)
	   out must have enough bytes allocated already, in must have the scanlines + 1 filtertype byte per scanline
	   w and h are image dimensions or dimensions of reduced image, bpp is bpp per pixel
	   in and out are allowed to be the same memory address!
	 */

	unsigned y;
	unsigned char *prevline = 0;

	unsigned long bytewidth = (bpp + 7) / 8;	/*bytewidth is used for filtering, is 1 when bpp < 8, number of bytes per pixel otherwise */
	unsigned long linebytes = (w * bpp + 7) / 8;

	for (y = 0; y < h; y++) {
		unsigned long outindex = linebytes * y;
		unsigned long inindex = (1 + linebytes) * y;	/*the extra filterbyte added to each row */
		unsigned char filterType = in[inindex];

		unfilter_scanline(&out[outindex], &in[inindex + 1], prevline, bytewidth, filterType, linebytes);
		if (error != UPNG_EOK) {
			return;
		}

		prevline = &out[outindex];
	}
}

static void remove_padding_bits(unsigned char *out, const unsigned char *in, unsigned long olinebits, unsigned long ilinebits, unsigned h)
{
	/*
	   After filtering there are still padding bpp if scanlines have non multiple of 8 bit amounts. They need to be removed (except at last scanline of (Adam7-reduced) image) before working with pure image buffers for the Adam7 code, the color convert code and the output to the user.
	   in and out are allowed to be the same buffer, in may also be higher but still overlapping; in must have >= ilinebits*h bpp, out must have >= olinebits*h bpp, olinebits must be <= ilinebits
	   also used to move bpp after earlier such operations happened, e.g. in a sequence of reduced images from Adam7
	   only useful if (ilinebits - olinebits) is a value in the range 1..7
	 */
	unsigned y;
	unsigned long diff = ilinebits - olinebits;
	unsigned long obp = 0, ibp = 0;	/*bit pointers */
	for (y = 0; y < h; y++) {
		unsigned long x;
		for (x = 0; x < olinebits; x++) {
			unsigned char bit = (unsigned char)((in[(ibp) >> 3] >> (7 - ((ibp) & 0x7))) & 1);
			ibp++;

			if (bit == 0)
				out[(obp) >> 3] &= (unsigned char)(~(1 << (7 - ((obp) & 0x7))));
			else
				out[(obp) >> 3] |= (1 << (7 - ((obp) & 0x7)));
			++obp;
		}
		ibp += diff;
	}
}

/*out must be buffer big enough to contain full image, and in must contain the full decompressed data from the IDAT chunks*/
void upng::post_process_scanlines(unsigned char *out, unsigned char *in)
{
	unsigned bpp = get_bpp();
	unsigned w = width;
	unsigned h = height;

	if (bpp == 0) {
		SET_ERROR(UPNG_EMALFORMED);
		return;
	}

	if (bpp < 8 && w * bpp != ((w * bpp + 7) / 8) * 8) {
		unfilter(in, in, w, h, bpp);
		if (error != UPNG_EOK) {
			return;
		}
		remove_padding_bits(out, in, w * bpp, ((w * bpp + 7) / 8) * 8, h);
	} else {
		unfilter(out, in, w, h, bpp);	/*we can immediatly filter into the out buffer, no other steps needed */
	}
}

upng_format upng::determine_format() {
	switch (color_type) {
	case UPNG_LUM:
		switch (color_depth) {
		case 1:
			return UPNG_LUMINANCE1;
		case 2:
			return UPNG_LUMINANCE2;
		case 4:
			return UPNG_LUMINANCE4;
		case 8:
			return UPNG_LUMINANCE8;
		default:
			return UPNG_BADFORMAT;
		}
	case UPNG_RGB:
		switch (color_depth) {
		case 8:
			return UPNG_RGB8;
		case 16:
			return UPNG_RGB16;
		default:
			return UPNG_BADFORMAT;
		}
	case UPNG_LUMA:
		switch (color_depth) {
		case 1:
			return UPNG_LUMINANCE_ALPHA1;
		case 2:
			return UPNG_LUMINANCE_ALPHA2;
		case 4:
			return UPNG_LUMINANCE_ALPHA4;
		case 8:
			return UPNG_LUMINANCE_ALPHA8;
		default:
			return UPNG_BADFORMAT;
		}
	case UPNG_RGBA:
		switch (color_depth) {
		case 8:
			return UPNG_RGBA8;
		case 16:
			return UPNG_RGBA16;
		default:
			return UPNG_BADFORMAT;
		}
	default:
		return UPNG_BADFORMAT;
	}
}



/*read the information from the header and store it in the upng_Info. return value is error*/
upng_error upng::header()
{
	/* if we have an error state, bail now */
	if (error != UPNG_EOK) {
		return error;
	}

	/* if the state is not NEW (meaning we are ready to parse the header), stop now */
	if (state != UPNG_NEW) {
		return error;
	}

	uint8_t header[29];
	stream->Seek(0,SEEK_SET);
	uint16_t bytesRead;
	stream->ReadBytes(header, sizeof(header), bytesRead);

	/* minimum length of a valid PNG file is 29 bytes
	 * FIXME: verify this against the specification, or
	 * better against the actual code below */
	if (bytesRead != 29) {
		SET_ERROR(UPNG_ENOTPNG);
		return error;
	}
	
	/* check that PNG header matches expected value */
	if (header[0] != 137 || header[1] != 80 || header[2] != 78 || header[3] != 71 || header[4] != 13 || header[5] != 10 || header[6] != 26 || header[7] != 10) {
		SET_ERROR(UPNG_ENOTPNG);
		return error;
	}

	/* check that the first chunk is the IHDR chunk */
	if (MAKE_DWORD_PTR(header + 12) != CHUNK_IHDR) {
		SET_ERROR(UPNG_EMALFORMED);
		return error;
	}

	/* read the values given in the header */
	width = MAKE_DWORD_PTR(header + 16);
	height = MAKE_DWORD_PTR(header + 20);
	color_depth = header[24];
	color_type = (upng_color)header[25];

	/* determine our color format */
	format = determine_format();
	if (format == UPNG_BADFORMAT) {
		SET_ERROR(UPNG_EUNFORMAT);
		return error;
	}

	/* check that the compression method (byte 27) is 0 (only allowed value in spec) */
	if (header[26] != 0) {
		SET_ERROR(UPNG_EMALFORMED);
		return error;
	}

	/* check that the compression method (byte 27) is 0 (only allowed value in spec) */
	if (header[27] != 0) {
		SET_ERROR(UPNG_EMALFORMED);
		return error;
	}

	/* check that the compression method (byte 27) is 0 (spec allows 1, but uPNG does not support it) */
	if (header[28] != 0) {
		SET_ERROR(UPNG_EUNINTERLACED);
		return error;
	}

	state = UPNG_HEADER;
	return error;
}

/*read a PNG, the result will be in the same color type as the PNG (hence "generic")*/
upng_error upng::decode(uint8_t *outputBuffer, uint32_t bufferLen)
{
	/* if we have an error state, bail now */
	if (error != UPNG_EOK) {
		return error;
	}

	/* parse the main header, if necessary */
	header();
	if (error != UPNG_EOK) {
		return error;
	}

	/* if the state is not HEADER (meaning we are ready to decode the image), stop now */
	if (state != UPNG_HEADER) {
		return error;
	}

	/* first byte of the first chunk after the header */
	stream->Seek(33,SEEK_SET);


	/* decompress image data - scan through chunks as we read bits */
	error = uz_inflate(outputBuffer, bufferLen);
	if (error != UPNG_EOK) {
		return error;
	}

	/* unfilter scanlines */
	post_process_scanlines(outputBuffer, outputBuffer);

	if (error == UPNG_EOK) {
		state = UPNG_DECODED;
	}

	/* we are done with our input buffer; free it if we own it */
	return error;
}


void upng::Init()
{
	width = height = 0;

	color_type = UPNG_RGBA;
	color_depth = 8;
	format = UPNG_RGBA8;

	state = UPNG_NEW;

	error = UPNG_EOK;
	error_line = 0;

	source.size = 0;
	chunkLen = 0;
	currentByte = 0;
}

upng::upng(Stream *str)
{
	Init();
	stream = str;
	source.size = stream->GetSize();
}



upng_error upng::get_error() const
{
	return error;
}

unsigned upng::get_error_line() const
{
	return error_line;
}

unsigned upng::get_width() const
{
	return width;
}

unsigned upng::get_height() const
{
	return height;
}

unsigned upng::get_bpp() const
{
	return get_bitdepth() * get_components();
}

unsigned upng::get_components() const
{
	switch (color_type) {
	case UPNG_LUM:
		return 1;
	case UPNG_RGB:
		return 3;
	case UPNG_LUMA:
		return 2;
	case UPNG_RGBA:
		return 4;
	default:
		return 0;
	}
}

unsigned upng::get_bitdepth() const
{
	return color_depth;
}

unsigned upng::get_pixelsize() const
{
	unsigned bits = get_bitdepth() * get_components();
	bits += bits % 8;
	return bits;
}

upng_format upng::get_format() const
{
	return format;
}
