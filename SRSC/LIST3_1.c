/***************************************************************************
 * File: filter operation test                                             *
 *                                                                         *
 * Desc: This program performs filtering operation with zone plate image   *
 ***************************************************************************/


#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include "ip.h"

 //#define operation(VALUE)        ((float) VALUE * 1.0) // original image

extern void write_pnm(image_ptr ptr, char filein[], int rows,
	int cols, int magic_number);
extern image_ptr read_pnm(char *filename, int *rows, int *cols,
	int *type);
extern void convolve(image_ptr source, int cols, int rows, int kwidth, int kheight,
	float *kernel, int bias, char *filename);
extern void makeczp(image_ptr buffer, int rows, int cols, int V, int H);

int main(int argc, char *argv[])
{
	char filein[100];                   /* name of input file */
	char fileout[100];                  /* name of output file */
	int rows, cols;                     /* image rows and columns */
	unsigned long i;                    /* counting index */
	unsigned long bytes_per_pixel;      /* number of bytes per image pixel */
	image_ptr buffer;                   /* pointer to image buffer */
	// image_ptr obuffer;                   /* pointer to image buffer */
	unsigned long number_of_pixels;     /* total number of pixels in image */
	int type;                           /* what type of image data */

	// Convolution 계수 저장
	float mask[3][3] = { {-0.111, -0.111, -0.111}, {-0.111, 0.888, -0.111}, {-0.111, -0.111, -0.111} };

	/* set input filename and output file name */
	if (argc == 3)
	{
		strcpy(filein, argv[1]);
		strcpy(fileout, argv[2]);
	}
	else
	{
		printf("Input name of input file\n");
		gets(filein);
		printf("\nInput name of output file\n");
		gets(fileout);
		printf("\n");
	}

	buffer = read_pnm(filein, &rows, &cols, &type);

	/* determine bytes_per_pixel, 3 for color, 1 for gray-scale */
	if (type == PPM)
		bytes_per_pixel = 3;
	else
		bytes_per_pixel = 1;

	number_of_pixels = bytes_per_pixel * rows * cols;


	// makeczp 함수 call을 이용한 zone plate 영상 생성
	makeczp(buffer, rows, cols, 350, 350);
	//convolve(buffer, cols, rows, 3, 3, mask, 0, fileout);

	write_pnm(buffer, fileout, rows, cols, type);
	IP_FREE(buffer);
	return 0;
}
