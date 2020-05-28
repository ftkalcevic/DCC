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

#if !defined(UPNG_H)
#define UPNG_H

#include <stdint.h>
#include "Stream.h"

enum upng_error {
	UPNG_EOK			= 0, /* success (no error) */
	UPNG_ENOMEM			= 1, /* memory allocation failed */
	UPNG_ENOTFOUND		= 2, /* resource not found (file missing) */
	UPNG_ENOTPNG		= 3, /* image data does not have a PNG header */
	UPNG_EMALFORMED		= 4, /* image data is not a valid PNG image */
	UPNG_EUNSUPPORTED	= 5, /* critical PNG chunk type is not supported */
	UPNG_EUNINTERLACED	= 6, /* image interlacing is not supported */
	UPNG_EUNFORMAT		= 7, /* image color format is not supported */
	UPNG_EPARAM			= 8  /* invalid parameter to method call */
};

enum upng_format {
	UPNG_BADFORMAT,
	UPNG_RGB8,
	UPNG_RGB16,
	UPNG_RGBA8,
	UPNG_RGBA16,
	UPNG_LUMINANCE1,
	UPNG_LUMINANCE2,
	UPNG_LUMINANCE4,
	UPNG_LUMINANCE8,
	UPNG_LUMINANCE_ALPHA1,
	UPNG_LUMINANCE_ALPHA2,
	UPNG_LUMINANCE_ALPHA4,
	UPNG_LUMINANCE_ALPHA8
};

enum upng_state {
	UPNG_ERROR		= -1,
	UPNG_DECODED	= 0,
	UPNG_HEADER		= 1,
	UPNG_NEW		= 2
};

enum upng_color {
	UPNG_LUM		= 0,
	UPNG_RGB		= 2,
	UPNG_LUMA		= 4,
	UPNG_RGBA		= 6
};

struct upng_source {
	unsigned long			size;			// input file size
};

struct huffman_tree {
	unsigned* tree2d;
	unsigned maxbitlen;	/*maximum number of bits a single code can get */
	unsigned numcodes;	/*number of symbols in the alphabet = number of codes */
};


class upng
{
	unsigned		width;
	unsigned		height;

	upng_color		color_type;
	unsigned		color_depth;
	upng_format		format;

	Stream *		stream;

	upng_error		error;
	unsigned		error_line;

	upng_state		state;
	upng_source		source;
	uint8_t			currentByte;
	uint32_t		chunkLen;


	upng_error ReadBytes(uint8_t* buffer, uint16_t len, uint16_t& bytesRead);
	unsigned huffman_decode_symbol(unsigned long* bp, const huffman_tree* codetree);
	void huffman_tree_create_lengths(huffman_tree* tree, const unsigned *bitlen);
	void get_tree_inflate_dynamic(huffman_tree* codetree, huffman_tree* codetreeD, huffman_tree* codelengthcodetree, unsigned long* bp);
	void inflate_huffman(unsigned char* out, unsigned long outsize, unsigned long* bp, unsigned long* pos, unsigned btype);
	void inflate_uncompressed(unsigned char* out, unsigned long outsize, unsigned long* bp, unsigned long* pos);
	upng_error uz_inflate_data(unsigned char* out, unsigned long outsize);
	upng_error uz_inflate(unsigned char* out, unsigned long outsize);
	void unfilter_scanline(unsigned char *recon, const unsigned char *scanline, const unsigned char *precon, unsigned long bytewidth, unsigned char filterType, unsigned long length);
	void unfilter(unsigned char *out, const unsigned char *in, unsigned w, unsigned h, unsigned bpp);
	void post_process_scanlines(unsigned char *out, unsigned char *in);
	upng_format determine_format();
	upng_error read_bit(unsigned long* bitpointer, unsigned char& bit);
	upng_error read_bits(unsigned long* bitpointer, unsigned long nbits, unsigned &bits);
	void Init();
	
	
public:
	upng(Stream *str);

	upng_error	header();
	upng_error	decode(uint8_t *outputBuffer, uint32_t bufferLen);

	upng_error	get_error() const;
	unsigned	get_error_line() const;

	unsigned	get_width() const;
	unsigned	get_height() const;
	unsigned	get_bpp() const;
	unsigned	get_bitdepth() const;
	unsigned	get_components() const;
	unsigned	get_pixelsize() const;
	upng_format	get_format() const;
};

#endif /*defined(UPNG_H)*/
