/***************************************************************************
 * File: get_bmphead.c                                                     *
 *                                                                         *
 * Desc: This program reads header part of bmp format image                *
 ***************************************************************************/

#include <sys/types.h>
#include <fcntl.h>
#include "IP.H"

extern image_ptr creat_pnm(int rows, int cols, int type);
extern void write_pnm(image_ptr ptr, char *filename, int rows, int cols, int magic_number);

void ConvertBMPtoPGN(char *filename, char *fileout);

void ConvertBMPtoPGN(char *filename, char *fileout)
{
	image_ptr buffer = NULL;
	unsigned long bytes_per_pixel;
	unsigned long number_of_pixels;
	BITMAPHEADER bmp_header;                // Bitmap 파일의 Header 구조체
	int read_fd;                              // 파일을 읽고 쓰는데 사용할 Descriptor

	read_fd = open(filename, O_RDONLY);     // bmp파일을 open한다.
	if (read_fd == -1) {
		printf("Can't open file %s\n", filename);
		exit(1);
	}

	read(read_fd, &bmp_header.bmpType, sizeof(bmp_header.bmpType));
	read(read_fd, &bmp_header.bmpSize, sizeof(bmp_header.bmpSize));
	read(read_fd, &bmp_header.bmpReserved1, sizeof(bmp_header.bmpReserved1));
	read(read_fd, &bmp_header.bmpReserved2, sizeof(bmp_header.bmpReserved2));
	read(read_fd, &bmp_header.bmpOffset, sizeof(bmp_header.bmpOffset));
	read(read_fd, &bmp_header.bmpHeaderSize, sizeof(bmp_header.bmpHeaderSize));
	read(read_fd, &bmp_header.bmpWidth, sizeof(bmp_header.bmpWidth));
	read(read_fd, &bmp_header.bmpHeight, sizeof(bmp_header.bmpHeight));
	read(read_fd, &bmp_header.bmpPlanes, sizeof(bmp_header.bmpPlanes));
	read(read_fd, &bmp_header.bmpBitCount, sizeof(bmp_header.bmpBitCount));
	read(read_fd, &bmp_header.bmpCompression, sizeof(bmp_header.bmpCompression));
	read(read_fd, &bmp_header.bmpBitmapSize, sizeof(bmp_header.bmpBitmapSize));
	read(read_fd, &bmp_header.bmpXPelsPerMeter, sizeof(bmp_header.bmpXPelsPerMeter));
	read(read_fd, &bmp_header.bmpYPelsPerMeter, sizeof(bmp_header.bmpYPelsPerMeter));
	read(read_fd, &bmp_header.bmpColors, sizeof(bmp_header.bmpColors));
	read(read_fd, &bmp_header.bmpClrImportant, sizeof(bmp_header.bmpClrImportant));

	int rows = bmp_header.bmpHeight, cols = bmp_header.bmpWidth, type = 5;

	buffer = creat_pnm(rows, cols, type);

	if (type == PPM)
		bytes_per_pixel = 3;
	else
		bytes_per_pixel = 1;
	number_of_pixels = (bytes_per_pixel) * (rows) * (cols);

	printf("%d\n", bmp_header.bmpBitmapSize);

	lseek(read_fd, bmp_header.bmpOffset, 0);
	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < cols; ++j) {
			unsigned char r, g, b;
			read(read_fd, buffer + cols * i + j, bmp_header.bmpBitCount / 8);
			//read(read_fd, &g, bmp_header.bmpBitCount / 8);
			//read(read_fd, &b, bmp_header.bmpBitCount / 8);
			//read(read_fd, &r, bmp_header.bmpBitCount / 8);
			//buffer[cols * i + j] = (g + b + r) / 3;
		}
	}

	write_pnm(buffer, fileout, rows, cols, type);

	IP_FREE(buffer);
	close(read_fd);
}

