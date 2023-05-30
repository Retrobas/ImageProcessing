#include <stdio.h>
#include "IP.H"

/*
	Windows에서는 open()을 사용하여 파일을 열면, ASCII 26 (^Z)를 파일의 끝으로 판단한다.
	또한 줄 끝을 CR LF (13 10)에서 LF(10)으로 변환가능성이 있다.
	따라서 fopen()을 통해 “rb”로 안전하게 바이너리 파일로 열어 문제를 해결한다.
*/

extern image_ptr creat_pnm(int rows, int cols, int type);
extern void write_pnm(image_ptr ptr, char *filename, int rows, int cols, int magic_number);

void ConvertBMPtoPGM(char *filename, char *fileout);

void ConvertBMPtoPGM(char *filename, char *fileout)
{
	image_ptr buffer = NULL;
	unsigned long bytes_per_pixel;
	unsigned long number_of_pixels;
	BITMAPHEADER bmp_header;

	// 파일을 이진파일로 연다.
	FILE *file = fopen(filename, "rb");
	if (file == NULL) {
		printf("Can't open file %s\n", filename);
		exit(1);
	}

	// BMP의 header를 분석한다.
	fread(&bmp_header.bmpType, sizeof(bmp_header.bmpType), 1, file);
	fread(&bmp_header.bmpSize, sizeof(bmp_header.bmpSize), 1, file);
	fread(&bmp_header.bmpReserved1, sizeof(bmp_header.bmpReserved1), 1, file);
	fread(&bmp_header.bmpReserved2, sizeof(bmp_header.bmpReserved2), 1, file);
	fread(&bmp_header.bmpOffset, sizeof(bmp_header.bmpOffset), 1, file);
	fread(&bmp_header.bmpHeaderSize, sizeof(bmp_header.bmpHeaderSize), 1, file);
	fread(&bmp_header.bmpWidth, sizeof(bmp_header.bmpWidth), 1, file);
	fread(&bmp_header.bmpHeight, sizeof(bmp_header.bmpHeight), 1, file);
	fread(&bmp_header.bmpPlanes, sizeof(bmp_header.bmpPlanes), 1, file);
	fread(&bmp_header.bmpBitCount, sizeof(bmp_header.bmpBitCount), 1, file);
	fread(&bmp_header.bmpCompression, sizeof(bmp_header.bmpCompression), 1, file);
	fread(&bmp_header.bmpBitmapSize, sizeof(bmp_header.bmpBitmapSize), 1, file);
	fread(&bmp_header.bmpXPelsPerMeter, sizeof(bmp_header.bmpXPelsPerMeter), 1, file);
	fread(&bmp_header.bmpYPelsPerMeter, sizeof(bmp_header.bmpYPelsPerMeter), 1, file);
	fread(&bmp_header.bmpColors, sizeof(bmp_header.bmpColors), 1, file);
	fread(&bmp_header.bmpClrImportant, sizeof(bmp_header.bmpClrImportant), 1, file);

	// PGM이 사용할 매직넘버를 지정한다.
	int rows = bmp_header.bmpHeight, cols = bmp_header.bmpWidth, type = 5;

	// 헤더 정보 출력
	printf("bmpOffset: %d\nbmpWidth: %d\nbmpHeight: %d\n", bmp_header.bmpOffset, cols, rows);

	// 빈 PNM 버퍼를 생성한다.
	buffer = creat_pnm(rows, cols, type);

	if (type == PPM)
		bytes_per_pixel = 3;
	else
		bytes_per_pixel = 1;
	number_of_pixels = (bytes_per_pixel) * (rows) * (cols);

	// 정상 영상 출력
	fseek(file, 0, SEEK_END);
	for (int i = 0; i < rows; ++i) {
		fseek(file, -cols * (i + 1), SEEK_END);
		fread(buffer + i * cols, cols, 1, file);
	}

	// flip된 영상 출력
	//fseek(file, bmp_header.bmpOffset, SEEK_SET);
	//fread(buffer, cols, rows, file);

	// 버퍼를 파일에 output한다.
	write_pnm(buffer, fileout, rows, cols, type);

	IP_FREE(buffer);
}

