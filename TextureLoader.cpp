#include "TextureLoader.h"
#include <png.h>
#include <cassert>

static bool is_png_file(FILE *fp);
static bool load_png(const char *file, unsigned char **buffer, unsigned *w, unsigned *h, unsigned *format); // sz = w*h*format
static bool load_bmp(const char *file, unsigned char **buffer, unsigned *w, unsigned *h, unsigned *format);
static bool load_tga(const char *file, unsigned char **buffer, unsigned *w, unsigned *h, unsigned *format);

static int chromaRed = 255;
static int chromaGrn = 0;
static int chromaBlu = 255;

//static void RgbToRgba(unsigned char *data

void TextureLoader::Swap2Channels(unsigned char *&data, unsigned &w, unsigned &h, unsigned &fmt)
{
	if (fmt >= 1 && fmt < 3) {
		// create alpha channel if necessary
		if (fmt == 1) {
			fmt = 2;
			unsigned char *ndata = (unsigned char*)malloc(w*h*fmt);
			for (size_t i=0; i<w*h; i++) {
				ndata[i*2] = 255;
				ndata[i*2+1] = data[i];
			}
			free(data); data = ndata;
			return;
		}
		// swap
		for (size_t i=0; i<w*h; i++) {
			data[i*2] ^= data[i*2+1];
			data[i*2+1] ^= data[i*2];
			data[i*2] ^= data[i*2+1];
		}
		return;
	}
}

void TextureLoader::SetColorKey(int r, int g, int b)
{
	chromaRed = r;
	chromaGrn = g;
	chromaBlu = b;
}
void TextureLoader::RemoveColorKey(unsigned char *&data, unsigned &w, unsigned &h, unsigned &fmt)
{
	// TODO:	Can we convert RGB channels to RGBA (in the load_png function?)
	//			On second thought: It's possible to do it in this function, by adding an extra byte!

	// grayscale
#if 0
	if (fmt >= 1 && fmt < 3) {
		// create alpha channel if necessary
		if (fmt == 1) {
			fmt = 2;
			unsigned char *ndata = (unsigned char*)malloc(w*h*fmt);
			for (size_t i=0; i<w*h; i++) {
				ndata[i*2] = data[i];
				ndata[i*2+1] = 255;
			}
			free(data); data = ndata;
		}
		// remove key
		for (size_t i=0; i<w*h; i++) {
			if (data[i*fmt] == chromaRed) {
				data[i*fmt+1] = 0;
			}
		}
		return;
	}
#endif

	// rgb(a)

	if (fmt < 4) // RGBA channels only
		return;

	for (unsigned y=0; y<h; y++) { // rows
		for (unsigned x=0; x<w; x++) { // columns
			// modify pixel data
			unsigned char &r = data[w*y*fmt + x*fmt];
			unsigned char &g = data[w*y*fmt + x*fmt + 1];
			unsigned char &b = data[w*y*fmt + x*fmt + 2];
			unsigned char &a = data[w*y*fmt + x*fmt + 3];
			
			if (r == (unsigned char)chromaRed && 
				g == (unsigned char)chromaGrn && 
				b == (unsigned char)chromaBlu)
			{
				a = 0;
			}
		}
	}
}

Texture *TextureLoader::LoadTextureEx(const char *file, manipulation_func_t customFunction)
{
	Texture *ret = NULL;
	unsigned char *data	= NULL;
	unsigned format	= 0;
	unsigned width	= 0;
	unsigned height	= 0;

	FILE *fp = fopen(file, "rb");
	if (!fp)
	{
		fprintf(stderr, "%s: Failed to open file!\n", file);
		goto out;
	}

	// read file header
	int signature;
	if (fread(&signature, sizeof(signature), 1, fp) != 1)
		goto out;
	fclose(fp); fp = NULL;

	if (signature == 0x474E5089)
	{
		if ( !load_png(file, &data, &width, &height, &format) )
			goto out; /* if we fail... */
	}
	else if ((signature & 0xFFFF) == 0x4D42)
	{
		if ( !load_bmp(file, &data, &width, &height, &format) )
			goto out;
	}
	else {
		size_t len = strlen(file);
		if (len >= 4 && !strcmp(file+len-4, ".tga")) {
			if (!load_tga(file, &data, &width, &height, &format))
				goto out;
		}
	}
	
	// run pixel data through custom function (may modify parameters)
	if (customFunction)
		customFunction(data, width, height, format);

	// load texture
	if (data != NULL && format != 0 && width != 0 && height != 0)
		ret = LoadTextureMem(data, width, height, format);
out:
	if (fp)
		fclose(fp);
	free(data);
	return ret;
}

Texture *TextureLoader::LoadTexture(const char *file)
{
	return LoadTextureEx(file, NULL);
}

static bool is_png_file(FILE *fp) {
	unsigned char png_sig[8];

	fseek(fp, 0, SEEK_SET);
	if ( fread(png_sig, 1, sizeof(png_sig), fp) ) {
		if (!png_sig_cmp(png_sig, 0, 8))
			return 1; /* it's a PNG file */
	}

	/* rewind in case it isn't */
	fseek(fp, 0, SEEK_SET);
	return 0;
}

static bool load_png(const char *file, unsigned char **buffer, unsigned *w, unsigned *h, unsigned *format) { // sz = w*h*format
	png_bytep *row_pointers = NULL;

	/* confirm it's a PNG file */
	FILE *fp;
	if( (fp = fopen(file, "rb")) == NULL )
		return 0;

	if ( !is_png_file(fp) ) {
		fclose(fp);
		return 0;
	}

	/* load info */
	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if(!png_ptr) {
		fclose(fp);
		return 0;
	}
	png_infop info_ptr = png_create_info_struct(png_ptr);
	if(!info_ptr){
		png_destroy_read_struct(&png_ptr, NULL, NULL);
		fclose(fp);
		return 0;
	}
	if(setjmp(png_jmpbuf(png_ptr))) {
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		fclose(fp);
		free(*buffer);
		free(row_pointers);
		return 0;
	}

	/* detect format */
	png_init_io(png_ptr,fp);
	png_set_sig_bytes(png_ptr,8);

	png_read_info(png_ptr,info_ptr);

	int bit_depth, color_type;

	color_type = png_get_color_type(png_ptr, info_ptr);
	bit_depth = png_get_bit_depth(png_ptr, info_ptr);

	/* convert palettes to rgb and grayscale to 8-bits
	 * create full alpha channel */
	if(color_type == PNG_COLOR_TYPE_PALETTE)
		png_set_palette_to_rgb(png_ptr);
	else if(color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
		png_set_gray_1_2_4_to_8(png_ptr);
	if(png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
		png_set_tRNS_to_alpha(png_ptr); // create full alpha channel

	if(bit_depth==16) // Do we need this?
		png_set_strip_16(png_ptr);
	else if (bit_depth < 8)
		png_set_packing(png_ptr);

	png_uint_32 pwidth, pheight;
	
	png_read_update_info(png_ptr, info_ptr);
	png_get_IHDR(png_ptr, info_ptr, &pwidth, &pheight, &bit_depth, &color_type, NULL, NULL, NULL);
	
	/* set params */
	*w = (unsigned int)pwidth;
	*h = (unsigned int)pheight;
	
	/* set format */
	if(color_type == PNG_COLOR_TYPE_GRAY) {
		*format=1;
	}
	else if(color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
		*format=2;
	}
	else if(color_type == PNG_COLOR_TYPE_RGB) {
		*format=3;
	}
	else if(color_type == PNG_COLOR_TYPE_RGB_ALPHA) {
		*format=4;
	}
	else {
		fprintf(stderr, "%s: Invalid png format.\n", file);
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		fclose(fp);
		return 0;
	}

	/* read raw data into buffer */
	*buffer = (unsigned char*)malloc(pheight * pwidth * (*format) );
	row_pointers = (png_bytep*)malloc( sizeof(png_bytep) * pheight );

	if(!(*buffer) || !row_pointers) {
		fprintf(stderr,"Failed to allocate buffer.\n");
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		fclose(fp);

		free(*buffer);
		free(row_pointers);
		return 0;
	}

	png_uint_32 i;
	for(i=0; i<pheight; i++)
		row_pointers[i] = ((*buffer) + i * pwidth * (*format) );
	png_read_image(png_ptr, row_pointers);

	png_read_end(png_ptr,NULL);
	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
	fclose(fp);

	/* success */
	free(row_pointers);
	return 1;
}

bool load_bmp(const char *file, unsigned char **buffer, unsigned *w, unsigned *h, unsigned *format)
{
	BITMAPFILEHEADER	bfh;
	BITMAPINFOHEADER	bfi;
	FILE				*fp;
	int					i;

	if (! (fp=fopen(file, "rb")))
		return 0;

	// load bitmap header
	fread(&bfh, sizeof(bfh), 1, fp);
	fread(&bfi, sizeof(bfi), 1, fp);

	if (ferror(fp))
	{
		fclose(fp);
		return 0;
	}
	if (bfh.bfType != 0x4D42)
	{
		fclose(fp);
		return 0;
	}

	// accept only 24-bit resolutions
	if (bfi.biBitCount != 24)
	{
		fclose(fp);
		return 0;
	}

	// hand details to caller
	*w = bfi.biWidth;
	*h = bfi.biHeight;
	*buffer = (unsigned char*)malloc(bfi.biWidth*bfi.biHeight*4);
	*format = 4; // 3 + additional alpha channel -- always 4

	// read (flipped rows and bgr -> rgb + a)
	// TODO: Make alpha channel optional to preserve memory? (Or is that even the case?)

	// FIXME: BOUNDS ERROR HERE!
	for (i=bfi.biHeight-1; i>=0; i--)
	{
		int x;
		for (x=0; x<bfi.biWidth*4; x+=4)
		{
			*buffer[bfi.biWidth*i*4+x+2] = fgetc(fp);
			*buffer[bfi.biWidth*i*4+x+1] = fgetc(fp);
			*buffer[bfi.biWidth*i*4+x] = fgetc(fp);

			// set alpha channel (RGB: 255, 0, 255)
			if (*buffer[bfi.biWidth*i*4+x] == 255 &&
				*buffer[bfi.biWidth*i*4+x+1] == 0 &&
				*buffer[bfi.biWidth*i*4+x+2] == 255)
			{
				*buffer[bfi.biWidth*i*4+x+3] = 0;
			}
			else
			{
				*buffer[bfi.biWidth*i*4+x+3] = 255;
			}
		}
	}

	fclose(fp);
	return 1;
}

/*
bool load_tga(const char *file, unsigned char **buffer,
			  unsigned *w, unsigned *h, unsigned *format)
{
	if (! (fp=fopen(file, "rb")))
		return false;
	return true;
}
*/

struct TGAHeader {
	unsigned char identSize; // size of ID field following header
	unsigned char colorMapType; // 0=none,1=palette
	unsigned char imageType; // 0=none,1=indexed,2=rgb,3=gray,+8=rle packed
	short colorMapStart; // first color map entry in palette
	short colorMapLength; // number of colors in palette
	unsigned char colorMapBits; // bits per color entry 15,16,24,32
	short xStart; // image x (todo: endian?)
	short yStart; // image y (todo: endian?)
	short width; // px width (todo: endian?)
	short height; // px height (todo: endian?)
	unsigned char bits; // bps
	unsigned char desc; // descriptor bits
};

bool ReadTgaHeader(FILE *fp, TGAHeader *head)
{
	fread(&head->identSize, sizeof(head->identSize), 1, fp);
	fread(&head->colorMapType, sizeof(head->colorMapType), 1, fp);
	fread(&head->imageType, sizeof(head->imageType), 1, fp);
	fread(&head->colorMapStart, sizeof(head->colorMapStart), 1, fp);
	fread(&head->colorMapLength, sizeof(head->colorMapLength), 1, fp);
	fread(&head->colorMapBits, sizeof(head->colorMapBits), 1, fp);
	fread(&head->xStart, sizeof(head->xStart), 1, fp);
	fread(&head->yStart, sizeof(head->yStart), 1, fp);
	fread(&head->width, sizeof(head->width), 1, fp);
	fread(&head->height, sizeof(head->height), 1, fp);
	fread(&head->bits, sizeof(head->bits), 1, fp);
	fread(&head->desc, sizeof(head->desc), 1, fp);

	if (feof(fp) || ferror(fp))
		return false;
	return true;
}

bool load_tga(const char *file, unsigned char **data, unsigned *width, unsigned *height, unsigned *bpp)
{
	FILE *fp = fopen(file, "rb");
	if (fp)
	{
		TGAHeader head;
		if (ReadTgaHeader(fp, &head))
		{
			// skip ident header
			fseek(fp, head.identSize, SEEK_CUR);

			// read pixel data
			// note: no palette or compression support atm
			if ((head.bits == 8 || head.bits == 16 ||
				 head.bits == 24 || head.bits == 32) &&
				head.colorMapType == 0)
			{
				*width = head.width;
				*height = head.height;
				*bpp = head.bits/8;
				int bytespx = (*bpp);
				int size = (*width)*(*height)*bytespx;
				*data = (unsigned char*)malloc(size);
				
				if (!(*data) || fread(*data, 1, size, fp) != size)
				{
					free(*data);
					*data = NULL;
					fclose(fp);
					return false;
				}

				// flip BGR to RGB
				if (head.bits == 24) {
					for (int i=0; i<size; i+=bytespx)
					{
						unsigned char c = (*data)[i];
						(*data)[i] = (*data)[i+2];
						(*data)[i+2] = c;
					}
				}

				fclose(fp);
				return true;
			}
		}
		fclose(fp);
	}
	return false;
}
