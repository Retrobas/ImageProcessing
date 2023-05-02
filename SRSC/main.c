#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include "ip.h"

extern image_ptr creat_pnm(int rows, int cols, int type);
extern image_ptr read_pnm(char *filename, int *rows, int *cols, int *type);
extern void write_pnm(image_ptr ptr, char *filename, int rows, int cols, int magic_number);

extern void apply_LUT(image_ptr buffer, unsigned long number_of_pixels, int *operation(int));
extern void change_LUT(image_ptr buffer, unsigned long number_of_pixels, int *operation(int));
extern int solarizing(int value);

extern void histogram_equalize(image_ptr buffer, unsigned long number_of_pixels);
extern void change_histogram(image_ptr buffer, unsigned long number_of_pixels);
void change_CDF(image_ptr buffer, unsigned long number_of_pixels);

extern void convolve(image_ptr source, int cols, int rows, int kwidth, int kheight, float *kernel, int bias, char *filename);
extern void makeczp(image_ptr buffer, int rows, int cols, int V, int H);

void change_pgm();
void processing_LUT();
void creat_LUT();

void processing_HE();
void creat_histogram();
void creat_CDF();

void processing_convolution();
void creat_zone_plate();

int main(int argc, char *argv[])
{
	// HE: processing_HE();
	// Histogram: creat_histogram();
	// CDF: creat_CDF();
	return 0;
}

void creat_LUT()
{
	char fileout[100];
	image_ptr buffer = NULL;
	unsigned long bytes_per_pixel;
	unsigned long number_of_pixels;

	printf("Input name of output file\n");
	gets(fileout);
	printf("\n");

	buffer = creat_pnm(256, 256, 5);
	bytes_per_pixel = 1;
	number_of_pixels = 256 * 256;

	change_LUT(buffer, number_of_pixels, solarizing);
	write_pnm(buffer, fileout, 256, 256, 5);
	IP_FREE(buffer);
}

void processing_LUT()
{
	char filein[100];
	char fileout[100];
	int rows, cols, type;
	image_ptr buffer = NULL;
	unsigned long bytes_per_pixel;
	unsigned long number_of_pixels;

	printf("Input name of input file\n");
	gets(filein);

	printf("\nInput name of output file\n");
	gets(fileout);
	printf("\n");

	buffer = read_pnm(filein, &rows, &cols, &type);

	if (type == PPM)
		bytes_per_pixel = 3;
	else
		bytes_per_pixel = 1;
	number_of_pixels = (bytes_per_pixel) * (rows) * (cols);

	apply_LUT(buffer, number_of_pixels, solarizing);
	write_pnm(buffer, fileout, rows, cols, type);

	IP_FREE(buffer);
}

void change_pgm()
{
	char filein[100];
	char fileout[100];
	int rows, cols, type;
	image_ptr buffer = NULL;
	unsigned long bytes_per_pixel;
	unsigned long number_of_pixels;
	int i, j;

	printf("Input name of input file\n");
	gets(filein);

	printf("\nInput name of output file\n");
	gets(fileout);
	printf("\n");

	buffer = read_pnm(filein, &rows, &cols, &type);

	if (type == PPM)
		bytes_per_pixel = 3;
	else
		bytes_per_pixel = 1;
	number_of_pixels = (bytes_per_pixel) * (rows) * (cols);

	for (i = 0; i < rows; ++i) {
		for (j = 0; j < cols; ++j) {
			//buffer[cols * i + j] = (float)j / (cols) * 255.0;
			//buffer[cols * i + j] = (float)i / rows * 255.0;
			//buffer[cols * i + j] = (float)(i + j);
			//CLIP(buffer[cols * i + j], 0, 255);
			if (i == 90 || i == 140 || j == 90 || j == 140) buffer[cols * i + j] = 255;
		}
	}

	write_pnm(buffer, fileout, rows, cols, type);

	IP_FREE(buffer);
}

void processing_HE()
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

	// 이미지를 he가 적용된 이미지로 변경하고 저장한다.
	histogram_equalize(buffer, number_of_pixels);
	write_pnm(buffer, fileout, rows, cols, type);

	IP_FREE(buffer);
}

void creat_histogram()
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

	// 이미지를 histogram으로 변경하고 저장한다.
	change_histogram(buffer, number_of_pixels);
	write_pnm(buffer, fileout, 256, 256, 5);	// 크기가 256 x 256 인 histogram 생성

	IP_FREE(buffer);
}

void processing_convolution()
{
	char filein[100];
	char fileout[100];
	int rows, cols, type;
	image_ptr buffer = NULL;
	unsigned long bytes_per_pixel;
	unsigned long number_of_pixels;

	float averageFilter[3][3] = { {1.0 / 9, 1.0 / 9, 1.0 / 9}, {1.0 / 9, 1.0 / 9, 1.0 / 9}, {1.0 / 9, 1.0 / 9, 1.0 / 9} };
	float highpassFilter[3][3] = { {-1.0 / 9, -1.0 / 9, -1.0 / 9}, {-1.0 / 9, 8.0 / 9, -1.0 / 9}, {-1.0 / 9, -1.0 / 9, -1.0 / 9} };
	float sharpeningFilter[3][3] = { {-1, -1, -1}, {-1, 9, -1}, {-1, -1, -1} };

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

	convolve(buffer, cols, rows, 3, 3, sharpeningFilter, 0, fileout);
	IP_FREE(buffer);
}

void creat_zone_plate()
{
	char fileout[100];
	image_ptr buffer = NULL;
	unsigned long bytes_per_pixel;
	unsigned long number_of_pixels;

	printf("Input name of output file\n");
	gets(fileout);
	printf("\n");

	buffer = creat_pnm(256, 256, 5);
	bytes_per_pixel = 1;
	number_of_pixels = 256 * 256;

	makeczp(buffer, 256, 256, 100, 100);
	write_pnm(buffer, fileout, 256, 256, 5);
	IP_FREE(buffer);
}

void creat_CDF()
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

	// 이미지를 histogram의 CDF로 변경하고 저장한다.
	change_CDF(buffer, number_of_pixels);
	write_pnm(buffer, fileout, 256, 256, 5);	// 크기가 256 x 256 인 CDF 생성

	IP_FREE(buffer);
}