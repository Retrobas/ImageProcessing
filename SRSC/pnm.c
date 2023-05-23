#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include "ip.h"

image_ptr creat_pnm(int rows, int cols, int type);
image_ptr read_pnm(char *filename, int *rows, int *cols, int *type);
void write_pnm(image_ptr ptr, char *filename, int rows, int cols, int magic_number);

// white spcae와 주석을 무시하고 숫자를 읽어온다.
int getnum(FILE *fp)
{
	char c;
	int result;

	// white space 무시
	do
	{
		c = getc(fp);
	} while ((c == ' ') || (c == '\t') || (c == '\n') || (c == '\r'));

	// 주석 무시
	if ((c < '0') || (c > '9'))
	{
		if (c == '#')
		{
			// 주석이 나오면 계속 무시한다.
			while (c == '#')
			{
				// 한 줄 무시
				while (c != '\n')
					c = getc(fp);
				c = getc(fp);
			}
		}
		// 숫자가 아니고 주석도 아니라면 오류
		else
		{
			printf("Garbage in ASCII fields\n");
			exit(1);
		}
	}

	// 숫자 읽어오기
	result = 0;
	do
	{
		result = result * 10 + (c - '0');
		c = getc(fp);
	} while ((c >= '0') && (c <= '9'));

	return result;
}

// 이진 파일인 RAWBITS PNM file 을 읽어와 char로 변환해 image_ptr을 내놓는 함수
// 이미지 크기(cols, rows)와 (type)은 함수 내부에서 정해진다.
image_ptr read_pnm(char *filename, int *rows, int *cols, int *type)
{
	int i;
	int row_size;              
	int maxval;                
	FILE *fp;                  
	int firstchar, secchar;    
	image_ptr result;             
	unsigned long offset;      
	unsigned long total_size;  
	unsigned long total_bytes; 
	float scale;               

	// 이진 파일로 연다. (ASCII PNM인 P1, P2, P3는 고려하지 않음)
	if ((fp = fopen(filename, "rb")) == NULL)
	{
		printf("Unable to open %s for reading\n", filename);
		exit(1);
	}

	// Magic Number를 가져온다.
	firstchar = getc(fp);
	secchar = getc(fp);
	*type = secchar - '0';

	// Image size (ASCII) 를 저장한다.
	*cols = getnum(fp);
	*rows = getnum(fp);

	// Magic Number는 P로 시작한다.
	if (firstchar != 'P')
	{
		printf("You silly goof... This is not a PPM file!\n");
		exit(1);
	}

	switch (secchar)
	{
	case '4':            /* PBM */
		scale = 0.125;	 // pixel 당 1bit [0, 1]
		maxval = 1;
		break;
	case '5':            /* PGM */
		scale = 1.0;	 // pixel 당 1byte [0, 256)
		maxval = getnum(fp);
		break;
	case '6':             /* PPM */
		scale = 3.0;	 // pixel 당 3bytes [0, 256) x 3
		maxval = getnum(fp);
		break;
	default:             /* Error */
		printf("read_pnm: This is not a Portable bitmap RAWBITS file\n");
		exit(1);
		break;
	}

	// Image size (byte) 를 저장한다.
	row_size = (*cols) * scale;
	total_size = (unsigned long)(*rows) * row_size;

	// 이미지를 읽어와 저장한다.
	result = (image_ptr)IP_MALLOC(total_size);
	if (result == NULL)
	{
		printf("Unable to malloc %lu bytes\n", total_size);
		exit(1);
	}
	total_bytes = 0;
	offset = 0;
	for (i = 0; i < (*rows); i++)
	{
		total_bytes += fread(result + offset, 1, row_size, fp);
		offset += row_size;
	}

	// 잘못 읽어왔다면 오류처리
	if (total_size != total_bytes)
	{
		printf("Failed miserably trying to read %ld bytes\nRead %ld bytes\n",
			total_size, total_bytes);
		exit(1);
	}

	fclose(fp);
	return result;
}

// char인 image_ptr을 이진 파일인 RAWBITS PNM file로 변경하는 함수
void write_pnm(image_ptr ptr, char *filename, int rows, int cols, int magic_number)
{
	FILE *fp;             
	long offset;          
	long total_bytes;     
	long total_size;      
	int row_size;         
	int i;                
	float scale;          

	switch (magic_number)
	{
	case 4:            /* PBM */
		scale = 0.125;
		break;
	case 5:            /* PGM */
		scale = 1.0;
		break;
	case 6:             /* PPM */
		scale = 3.0;
		break;
	default:             /* Error */
		printf("write_pnm: This is not a Portable bitmap RAWBITS file\n");
		exit(1);
		break;
	}

	if ((fp = fopen(filename, "wb")) == NULL)
	{
		printf("Unable to open %s for output\n", filename);
		exit(1);
	}

	// Magic Number와 이미지 크기 쓰기
	fprintf(fp, "P%d\n%d %d\n", magic_number, cols, rows);

	// bitmap file이 아니라면 level은 255로 고정
	if (magic_number != 4)	
		fprintf(fp, "255\n");

	// 크기 설정 후 이미지 저장
	row_size = cols * scale;
	total_size = (long)row_size * rows;
	offset = 0;
	total_bytes = 0;
	for (i = 0; i < rows; i++)
	{
		total_bytes += fwrite(ptr + offset, 1, row_size, fp);
		offset += row_size;
	}

	if (total_bytes != total_size)
		printf("Tried to write %ld bytes...Only wrote %ld\n", total_size, total_bytes);

	fclose(fp);
}

// 빈 PNM 이미지를 만든다.
image_ptr creat_pnm(int rows, int cols, int type)
{
	int i;
	int row_size;
	image_ptr result;
	unsigned long total_size;
	float scale;

	// Magic Number에 따른 scale값을 결정한다.
	switch (type)
	{
	case 4:            /* PBM */
		scale = 0.125;	 // pixel 당 1bit [0, 1]
		break;
	case 5:            /* PGM */
		scale = 1.0;	 // pixel 당 1byte [0, 256)
		break;
	case 6:             /* PPM */
		scale = 3.0;	 // pixel 당 3bytes [0, 256) x 3
		break;
	default:             /* Error */
		printf("creat_pnm: This is not a Portable bitmap RAWBITS file\n");
		exit(1);
		break;
	}

	// Image size (byte) 를 저장한다.
	row_size = cols * scale;
	total_size = (unsigned long)(rows) * row_size;

	// 이미지를 읽어와 저장한다.
	result = (image_ptr)IP_MALLOC(total_size);
	if (result == NULL)
	{
		printf("Unable to malloc %lu bytes\n", total_size);
		exit(1);
	}

	return result;
}

// RAWBITS PNM file 의 이미지 크기, 스케일값을 결정하고
// 헤더를 제외한 실제 읽어야 하는 파일의 시작부분을 반환하는 함수
FILE *pnm_open(int *rows, int *cols, int *maxval, char *filename)
{
	int firstchar, secchar;
	float scale;
	FILE *fp;

	if ((fp = fopen(filename, "rb")) == NULL)
	{
		printf("Unable to open %s for reading\n", filename);
		exit(1);
	}

	firstchar = getc(fp);
	secchar = getc(fp);

	if (firstchar != 'P')
	{
		printf("You silly goof... This is not a PPM file!\n");
		exit(1);
	}

	*cols = getnum(fp);
	*rows = getnum(fp);

	switch (secchar)
	{
	case '4':            /* PBM */
		scale = 0.125;
		*maxval = 1;
		break;
	case '5':            /* PGM */
		scale = 1.0;
		*maxval = getnum(fp);
		break;
	case '6':             /* PPM */
		scale = 3.0;
		*maxval = getnum(fp);
		break;
	default:             /* Error */
		printf("read_pnm: This is not a Portable bitmap RAWBITS file\n");
		exit(1);
		break;
	}

	return fp;
}

mesh *read_mesh(char *filename)
{
	FILE *fp;
	mesh *mesh_data;
	int width, height, mesh_size;

	/* open mesh file for input */
	if ((fp = fopen(filename, "rb")) == NULL)
	{
		printf("Unable to open mesh file %s for reading\n", filename);
		exit(1);
	}

	mesh_data = malloc(sizeof(mesh));
	/* read dimensions of mesh */
	fread(&width, sizeof(int), 1, fp);
	fread(&height, sizeof(int), 1, fp);
	mesh_data->width = width;
	mesh_data->height = height;
	mesh_size = width * height;

	/* allocate memory for mesh data */
	mesh_data->x_data = malloc(sizeof(float) * mesh_size);
	mesh_data->y_data = malloc(sizeof(float) * mesh_size);

	fread(mesh_data->x_data, sizeof(float), mesh_size, fp);
	fread(mesh_data->y_data, sizeof(float), mesh_size, fp);

	return(mesh_data);
}
