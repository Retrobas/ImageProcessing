#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <math.h>
#include "ip.h" 

// extern function
extern image_ptr creat_pnm(int rows, int cols, int type);
extern image_ptr read_pnm(char *filename, int *rows, int *cols, int *type);
extern int getnum(FILE *fp);

// creat image
void process_frame(int (*frame_operation)(int, int));

// process frame processing
void frame_processing(char *file1, char *file2, char *fileout, int (*frame_operation)(int, int));

void frame_processing(char *file1, char *file2, char *fileout, int (*frame_operation)(int, int))
{
	int rows, cols;                     /* image rows and columns */
	unsigned long i, j;                  /* x and y loop variables */
	unsigned long bytes_per_pixel;      /* number of bytes per image pixel */
	unsigned char *buff1, *buff2;       /* input line buffers */
	unsigned char *buff_out;            /* output line buffer */
	int file_type;                      /* what type of image data */
	FILE *fp1, *fp2;                    /* input file pointers */
	FILE *fpout;                        /* output file pointer */
	int firstchar, secchar;             /* first and second characters of file */
	int line;                           /* size of line in bytes */
	int temp;                           /* temporary variable */

	if ((fpout = fopen(fileout, "wb")) == NULL)
	{
		printf("Unable to open %s for writing\n", fileout);
		exit(1);
	}

	if ((fp1 = fopen(file1, "rb")) == NULL)
	{
		printf("Unable to open %s for reading\n", file1);
		exit(1);
	}

	firstchar = getc(fp1);
	secchar = getc(fp1);

	if (firstchar != 'P')
	{
		printf("You silly goof... This is not a PPM file!\n");
		exit(1);
	}

	cols = getnum(fp1);
	rows = getnum(fp1);
	getnum(fp1);     /* consume max value from input file header */

	if ((fp2 = fopen(file2, "rb")) == NULL)
	{
		printf("Unable to open %s for reading\n", file2);
		exit(1);
	}

	firstchar = getc(fp2);
	if (getc(fp2) != secchar)
	{
		printf("Input files are different types...exiting\n");
		exit(1);
	}

	if (firstchar != 'P')
	{
		printf("You silly goof... This is not a PPM file!\n");
		exit(1);
	}

	if (getnum(fp2) != cols)
	{
		printf("different column sizes...exiting\n");
		exit(1);
	}

	if (getnum(fp2) != rows)
	{
		printf("different row sizes...exiting\n");
		exit(1);
	}

	getnum(fp2);     /* consume max value from input file header */

	/* determine bytes_per_pixel, 3 for color, 1 for gray-scale */
	if (toupper(secchar) == 'P')
	{
		bytes_per_pixel = 3;
		file_type = PPM;
	}
	else
	{
		bytes_per_pixel = 1;
		file_type = PGM;
	}

	line = bytes_per_pixel * cols;
	buff1 = malloc(line);
	buff2 = malloc(line);
	buff_out = malloc(line);

	if ((buff1 == NULL) || (buff2 == NULL) || (buff_out == NULL))
	{
		printf("unable to malloc enough memory for measley frame process\n");
		exit(1);
	}

	/* print output file header */
	fprintf(fpout, "P%c\n%d %d\n255\n", secchar, cols, rows);

	for (i = 0; i < rows; i++)
	{
		fread(buff1, 1, line, fp1);
		fread(buff2, 1, line, fp2);
		for (j = 0; j < line; j++)
		{
			temp = frame_operation(buff1[j], buff2[j]);
			CLIP(temp, 0, 255);
			buff_out[j] = temp;
		}
		fwrite(buff_out, 1, line, fpout);
	}

	free(buff1);
	free(buff2);
	free(buff_out);
	fclose(fp1);
	fclose(fp2);
	fclose(fpout);
	return 0;
}

void process_frame(int (*frame_operation)(int, int))
{
	char file1[100];
	char file2[100];
	char fileout[100];
	int rows, cols, type;
	image_ptr buffer = NULL;
	unsigned long bytes_per_pixel;
	unsigned long number_of_pixels;

	// 콘솔 입력
	printf("Input name of first input file\n");
	gets(file1);
	printf("Input name of second input file\n");
	gets(file2);
	printf("\nInput name of output file\n");
	gets(fileout);
	printf("\n");

	frame_processing(file1, file2, fileout, frame_operation);
}