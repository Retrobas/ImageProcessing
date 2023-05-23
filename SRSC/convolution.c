#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "ip.h"

// extern function
extern image_ptr creat_pnm(int rows, int cols, int type);
extern image_ptr read_pnm(char *filename, int *rows, int *cols, int *type);
extern void write_pnm(image_ptr ptr, char *filename, int rows, int cols, int magic_number);
extern void median_filt(image_ptr source, int cols, int rows, char *filename, int side);
extern void mean_filt(image_ptr source, int cols, int rows, char *filename, int side);

// create image
void process_convolution();

// process convole
void convolve(image_ptr source, int cols, int rows, int kwidth, int kheight, float *kernel, int bias, char *filename);


// filters
float averageFilter[3][3] =
{
	{1.0 / 9, 1.0 / 9, 1.0 / 9},
	{1.0 / 9, 1.0 / 9, 1.0 / 9},
	{1.0 / 9, 1.0 / 9, 1.0 / 9}
};
float highpassFilter[3][3] =
{
	{-1.0 / 9, -1.0 / 9, -1.0 / 9},
	{-1.0 / 9, 8.0 / 9, -1.0 / 9},
	{-1.0 / 9, -1.0 / 9, -1.0 / 9}
};
float sharpeningFilter[3][3] =
{
	{-1, -1, -1},
	{-1, 9, -1},
	{-1, -1, -1}
};

float prewittGx[3][3] =
{
	{-1, -1, -1},
	{0, 0, 0},
	{1, 1, 1}
};
float prewittGy[3][3] =
{
	{-1, 0, 1},
	{-1, 0, 1},
	{-1, 0, 1}
};

float sobelGx[3][3] =
{
	{-1, -2, -1},
	{0, 0, 0},
	{1, 2, 1}
};
float sobelGy[3][3] =
{
	{-1, 0, 1},
	{-2, 0, 2},
	{-1, 0, 1}
};

// 9시방향부터 시계방향
float compassPrewitt[8][3][3] =
{
	{{ -1, 1, 1 }, {-1, -2, 1}, {-1, 1, 1}},
	{{ -1, -1, 1 }, {-1, -2, 1}, {1, 1, 1}},
	{{-1, -1, -1}, {1, -2, 1}, {1, 1, 1}},
	{{1, -1, -1}, {1, -2, -1}, {1, 1, 1} },
	{{1, 1, -1}, {1, -2, -1}, {1, 1, -1}},
	{{1, 1, 1}, {1, -2, -1}, {1, -1, -1}},
	{{1, 1, 1}, {1, -2, 1}, {-1, -1, -1}},
	{{1, 1, 1}, {-1, -2, 1}, {-1, -1, 1}}
};

float compassKirsh[8][3][3] =
{
	{{-3, -3, 5}, {-3, 0, 5}, {-3, -3, 5}},
	{{-3, -3, -3}, {-3, 0, 5}, {-3, 5, 5}},
	{{-3, -3, -3}, {-3, 0, -3}, {5, 5, 5}},
	{{-3, -3, -3}, {5, 0, -3}, {5, 5, -3}},
	{{5, -3, -3}, {5, 0, -3}, {5, -3, -3}},
	{{5, 5, -3}, {5, 0, -3}, {-3, -3, -3}},
	{{5, 5, 5}, {-3, 0, -3}, {-3, -3, -3}},
	{{-3, 5, 5}, {-3, 0, 5}, {-3, -3, -3}}
};

float compassRobinson3[8][3][3] =
{
	{{-1, 0, 1}, {-1, 0, 1}, {-1, 0, 1}},
	{{-1, -1, 0}, {-1, 0, 1}, {0, 1, 1}},
	{{-1, -1, -1}, {0, 0, 0}, {1, 1, 1}},
	{{0, -1, -1}, {1, 0, -1}, {1, 1, 0}},
	{{1, 0, -1}, {1, 0, -1}, {1, 0, -1}},
	{{1, 1, 0}, {1, 0, -1}, {0, -1, -1}},
	{{1, 1, 1}, {0, 0, 0}, {-1, -1, -1}},
	{{0, 1, 1}, {-1, 0, 1}, {-1, -1, 0}}
};

float compassRobinson5[8][3][3] =
{
	{{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}},
	{{-2, -1, 0}, {-1, 0, 1}, {0, 1, 2}},
	{{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}},
	{{0, -1, -2}, {1, 0, -1}, {2, 1, 0}},
	{{1, 0, -1}, {2, 0, -2}, {1, 0, -1}},
	{{2, 1, 0}, {1, 0, -1}, {0, -1, -2}},
	{{1, 2, 1}, {0, 0, 0}, {-1, -2, -1}},
	{{0, 1, 2}, {-1, 0, 1}, {-2, -1, 0}}
};

float LoG[5][5] = {
	{0, 0, -1, 0, 0},
	{0, -1, -2, -1, 0},
	{-1, -2, 16, -2, -1},
	{0, -1, -2, -1, 0},
	{0, 0, -1, 0, 0}
};

void convolve(image_ptr source, int cols, int rows, int kwidth, int kheight, float *kernel, int bias, char *filename)
{
	int x, y, i;           /* image loop variables */
	int kernx, kerny;      /* kernel loop variables */
	int index;             /* image index */
	int xextra, yextra;       /* size of boundary */
	int conv_line;            /* size of output line */
	float sum;                /* accumulator used during convolution */
	unsigned long destadd;    /* destination image address */
	unsigned long sourceadd;  /* index into source image */
	unsigned long sourcebase; /* address of line */
	unsigned char *dest;      /* destination image line */
	FILE *fp;                 /* output file pointer */
	unsigned char left[25];   /* storage of left pixel for duplication */
	unsigned char right[25];  /* storage of right pixel for duplication */
	int xpad, ypad;           /* number of pixels to duplicate at edges */
	int last_line;            /* last line to process */

	yextra = (kheight / 2) * 2;
	ypad = yextra / 2;
	xextra = (kwidth / 2) * 2;
	xpad = xextra / 2;
	conv_line = cols - xextra;
	last_line = rows - yextra;
	dest = malloc(cols);

	if ((fp = fopen(filename, "wb")) == NULL)
	{
		printf("Unable to open %s for output\n", filename);
		exit(1);
	}
	fprintf(fp, "P5\n%d %d\n255\n", cols, rows); /* print out header */

	for (y = 0; y < last_line; y++)
	{
		sourcebase = (unsigned long)cols * y;
		destadd = 0;
		for (x = xextra / 2; x < (cols - xpad); x++)
		{
			sum = 0.0;
			index = 0;
			for (kerny = 0; kerny < kheight; kerny++)
				for (kernx = 0; kernx < kwidth; kernx++)
				{
					sourceadd = sourcebase + kernx + kerny * cols;
					sum += (source[sourceadd] * kernel[index++]);
				}

			sum += bias;
			CLIP(sum, 0.0, 255.0);

			dest[destadd++] = (unsigned char)sum;
			sourcebase++;
		} /* for x */
		for (i = 0; i < xpad; i++)
			left[i] = dest[0];
		for (i = 0; i < xpad; i++)
			right[i] = dest[conv_line - 1];
		if (y == 0)
			for (i = 0; i < ypad; i++)
			{
				fwrite(left, 1, xpad, fp);
				fwrite(dest, 1, conv_line, fp);
				fwrite(right, 1, xpad, fp);
			}
		fwrite(left, 1, xpad, fp);
		fwrite(dest, 1, conv_line, fp);
		fwrite(right, 1, xpad, fp);
		if (y == (last_line - 1))
			for (i = 0; i < ypad; i++)
			{
				fwrite(left, 1, xpad, fp);
				fwrite(dest, 1, conv_line, fp);
				fwrite(right, 1, xpad, fp);
			}
	}
}

void process_convolution()
{
	char filein[100];
	char fileout[100];
	int rows, cols, type;
	image_ptr buffer = NULL;
	unsigned long bytes_per_pixel;
	unsigned long number_of_pixels;

	printf("input name of input file\n");
	gets(filein);

	printf("\ninput name of output file\n");
	gets(fileout);
	printf("\n");

	buffer = read_pnm(filein, &rows, &cols, &type);

	if (type == PPM)
		bytes_per_pixel = 3;
	else
		bytes_per_pixel = 1;
	number_of_pixels = (bytes_per_pixel) * (rows) * (cols);

	convolve(buffer, cols, rows, 5, 5, LoG, 0, fileout);
	//median_filt(buffer, cols, rows, fileout, 3);
	//mean_filt(buffer, cols, rows, fileout, 3);

	IP_FREE(buffer);
}