#include "ip.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void nearest_neighbor_interpolation(image_ptr buffer, char *fileout, int rows, int cols, float x_scale, float y_scale, int type);
void bilinear_interpolation(image_ptr buffer, char *fileout, int rows, int cols, float x_scale, float y_scale, int type);

/****************************************************************************
 * Func: scale_pnm                                                          *
 *                                                                          *
 * Desc: scale an image using nearest neighbor interpolation                *
 *                                                                          *
 * Params: buffer - pointer to image in memory                              *
 *         fileout - name of output file                                    *
 *         rows - number of rows in image                                   *
 *         cols - number of columns in image                                *
 *         x_scale - scale factor in X direction                            *
 *         y_scale - scale factor in Y direction                            *
 *         type - graphics file type (5 = PGM    6 = PPM)                   *
 ****************************************************************************/
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
	unsigned long source_index;
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
			float result = (unsigned long)(EWbottom + NSweight * (EWtop - EWbottom));

			if (type == 5)      /* PGM */
				line_buff[index++] = result;
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

void cubic_convolution_interpolation(image_ptr buffer, char *fileout, int rows, int cols, float x_scale, float y_scale, int type, float a)
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

			unsigned char mid = buffer[Y_Source * cols + X_Source];
			unsigned char nextX = buffer[Y_Source * cols + X_Source + 1];
			unsigned char nextY = buffer[(Y_Source + 1) * cols + X_Source];

			// col 보간
			float EWtop1 = (a + 2) * pow(X_Source, 3) - (a + 3) * pow(X_Source, 2) + 1;
			float EWbottom1 = SW1 + EWweight * (SE1 - SW1);

			// row 보간
			float result = (unsigned long)(EWbottom + NSweight * (EWtop - EWbottom));

			if (type == 5)      /* PGM */
				line_buff[index++] = result;
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