#include "ip.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

extern image_ptr read_pnm(char *filename, int *rows, int *cols, int *type);

void process_image_scaling(int process_type);

void nearest_neighbor_interpolation(image_ptr buffer, char *fileout, int rows, int cols, float x_scale, float y_scale, int type);
void bilinear_interpolation(image_ptr buffer, char *fileout, int rows, int cols, float x_scale, float y_scale, int type);
void cubic_convolution_interpolation(image_ptr buffer, char *fileout, int rows, int cols, float x_scale, float y_scale, int type, float a);
float cubic_linear_interpolation(float v1, float v2, float v3, float v4, float d, float a);

void nearest_neighbor_interpolation(image_ptr buffer, char *fileout, int rows, int cols, float x_scale, float y_scale, int type)
{
	unsigned long x, y;
	unsigned long index;
	unsigned long source_index;
	unsigned char *line_buff;
	int new_rows, new_cols;
	unsigned line;
	FILE *fp;
	unsigned long X_Source, Y_Source;
	pixel_ptr color_buff;

	if ((fp = fopen(fileout, "wb")) == NULL)
	{
		printf("Unable to open %s for output\n", fileout);
		exit(1);
	}

	new_cols = cols * x_scale;
	new_rows = rows * y_scale;

	fprintf(fp, "P%d\n%d %d\n255\n", type, new_cols, new_rows);

	if (type == 5)        /* PGM file */
		line = new_cols;
	else                 /* PPM file */
	{
		line = new_cols * 3;
		color_buff = (pixel_ptr)buffer;
	}

	line_buff = (unsigned char *)malloc(line);

	for (y = 0; y < new_rows; y++)
	{
		index = 0;
		for (x = 0; x < new_cols; x++)
		{
			X_Source = (unsigned long)((x / x_scale) + 0.5);
			Y_Source = (unsigned long)((y / y_scale) + 0.5);
			source_index = Y_Source * cols + X_Source;
			if (type == 5)      /* PGM */
				line_buff[index++] = buffer[source_index];
			else               /* PPM */
			{
				line_buff[index++] = color_buff[source_index].r;
				line_buff[index++] = color_buff[source_index].g;
				line_buff[index++] = color_buff[source_index].b;
			}
		}
		fwrite(line_buff, 1, line, fp);
	}
	fclose(fp);
}

void bilinear_interpolation(image_ptr buffer, char *fileout, int rows, int cols, float x_scale, float y_scale, int type)
{
	unsigned long x, y;
	unsigned long index;
	unsigned char *line_buff;
	int new_rows, new_cols;
	unsigned line;
	FILE *fp;
	unsigned long X_Source, Y_Source;
	pixel_ptr color_buff;

	if ((fp = fopen(fileout, "wb")) == NULL)
	{
		printf("Unable to open %s for output\n", fileout);
		exit(1);
	}

	new_cols = cols * x_scale;
	new_rows = rows * y_scale;

	fprintf(fp, "P%d\n%d %d\n255\n", type, new_cols, new_rows);

	if (type == 5)        /* PGM file */
		line = new_cols;
	else                 /* PPM file */
	{
		line = new_cols * 3;
		color_buff = (pixel_ptr)buffer;
	}

	line_buff = (unsigned char *)malloc(line);

	for (y = 0; y < new_rows; y++)
	{
		index = 0;
		for (x = 0; x < new_cols; x++)
		{
			X_Source = (unsigned long)((x / x_scale) + 0.5);
			Y_Source = (unsigned long)((y / y_scale) + 0.5);

			unsigned char NW = buffer[Y_Source * cols + X_Source];
			unsigned char NE = buffer[Y_Source * cols + X_Source + 1];
			unsigned char SW = buffer[(Y_Source + 1) * cols + X_Source];
			unsigned char SE = buffer[(Y_Source + 1) * cols + X_Source + 1];

			// 정확한 scale이 된 index값 (float) - 보간된 index (long)
			float EWweight = (x / x_scale) - (float)X_Source;
			float NSweight = (y / y_scale) - (float)Y_Source;

			// col 보간
			float EWtop = NW + EWweight * (NE - NW);
			float EWbottom = SW + EWweight * (SE - SW);

			// row 보간
			float result = EWbottom + NSweight * (EWtop - EWbottom);
			CLIP(result, 0, 255);

			// only PGM
			line_buff[index++] = result;
		}
		fwrite(line_buff, 1, line, fp);
	}
	fclose(fp);
}

float cubic_linear_interpolation(float v1, float v2, float v3, float v4, float d, float a)
{
	float r1, r2, r3, r4;

	r1 = -4 * a + (1 + d) * (8 * a + (1 + d) * (-5 * a + a * (1 + d)));
	r2 = 1 + (d) * (d) * (-a - 3 + (a + 2) * (d));
	r3 = 1 + (1 - d) * (1 - d) * (-a - 3 + (a + 2) * (1 - d));
	r4 = -4 * a + (2 - d) * (8 * a + (2 - d) * (-5 * a + a * (2 - d)));

	return r1 * v1 + r2 * v2 + r3 * v3 + r4 * v4;
}

void cubic_convolution_interpolation(image_ptr buffer, char *fileout, int rows, int cols, float x_scale, float y_scale, int type, float a)
{
	unsigned long x, y;
	unsigned long index;
	unsigned char *line_buff;
	int new_rows, new_cols;
	unsigned line;
	FILE *fp;
	unsigned long X_Source, Y_Source;
	pixel_ptr color_buff;

	/* open new output file */
	if ((fp = fopen(fileout, "wb")) == NULL)
	{
		printf("Unable to open %s for output\n", fileout);
		exit(1);
	}

	new_cols = cols * x_scale;
	new_rows = rows * y_scale;

	/* print out the portable bitmap header */
	fprintf(fp, "P%d\n%d %d\n255\n", type, new_cols, new_rows);

	if (type == 5)        /* PGM file */
		line = new_cols;
	else                 /* PPM file */
	{
		line = new_cols * 3;
		color_buff = (pixel_ptr)buffer;
	}

	line_buff = (unsigned char *)malloc(line);

	for (y = 0; y < new_rows; y++)
	{
		index = 0;
		for (x = 0; x < new_cols; x++)
		{
			X_Source = (unsigned long)((x / x_scale) + 0.5);
			Y_Source = (unsigned long)((y / y_scale) + 0.5);

			unsigned long curIdx = Y_Source * cols + X_Source;
			unsigned char cubic[16] = {
				 buffer[curIdx - cols - 1],  buffer[curIdx - cols],  buffer[curIdx - cols + 1],  buffer[curIdx - cols + 2],
				 buffer[curIdx - 1],  buffer[curIdx],  buffer[curIdx + 1],  buffer[curIdx + 2],
				 buffer[curIdx + cols - 1],  buffer[curIdx + cols],  buffer[curIdx + cols + 1],  buffer[curIdx + cols + 2],
				 buffer[curIdx + cols*2 - 1],  buffer[curIdx + cols * 2],  buffer[curIdx + cols * 2 + 1],  buffer[curIdx + cols * 2 + 2],
			};

			// col 보간
			float d = (x / x_scale) - (float)X_Source;
			float col1 = cubic_linear_interpolation(cubic[0], cubic[1], cubic[2], cubic[3], d, a);
			float col2 = cubic_linear_interpolation(cubic[4], cubic[5], cubic[6], cubic[7], d, a);
			float col3 = cubic_linear_interpolation(cubic[8], cubic[9], cubic[10], cubic[11], d, a);
			float col4 = cubic_linear_interpolation(cubic[12], cubic[13], cubic[14], cubic[15], d, a);

			// row 보간
			d = (y / y_scale) - (float)Y_Source;
			float result = cubic_linear_interpolation(col4, col3, col2, col1, d, a);
			CLIP(result, 0, 255);

			// only PGM
			line_buff[index++] = result;
		}
		fwrite(line_buff, 1, line, fp);
	}
	fclose(fp);
}

// 0: nearest neighbor, 1: bilinear, 2: cubic
void process_image_scaling(int process_type)
{
	char filein[100];
	char fileout[100];
	int rows, cols, type;
	image_ptr buffer = NULL;
	unsigned long bytes_per_pixel;
	unsigned long number_of_pixels;

	// 콘솔 입력
	printf("Input name of input file\n");
	gets(filein);

	printf("\nInput name of output file\n");
	gets(fileout);
	printf("\n");

	// 이진파일을 char로 읽어온다.
	buffer = read_pnm(filein, &rows, &cols, &type);

	// 이미지 타입에 따른 크기 설정
	if (type == PPM)
		bytes_per_pixel = 3;
	else
		bytes_per_pixel = 1;
	number_of_pixels = (bytes_per_pixel) * (rows) * (cols);

	if (process_type == 0) nearest_neighbor_interpolation(buffer, fileout, rows, cols, 2, 2, type);
	else if (process_type == 1) bilinear_interpolation(buffer, fileout, rows, cols, 2, 2, type);
	else cubic_convolution_interpolation(buffer, fileout, rows, cols, 2, 2, type, -1);
}