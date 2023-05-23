#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <math.h> 
#include "ip.h"

// extern function
extern image_ptr creat_pnm(int rows, int cols, int type);
extern void write_pnm(image_ptr ptr, char *filename, int rows, int cols, int magic_number);

// creat image
void creat_zone_plate();

// save zone plate image
void makeczp(image_ptr buffer, int rows, int cols, int V, int H);

void makeczp(image_ptr buffer, int rows, int cols, int V, int H)
{
	int i, j;
	double x, y;
	double cos_val;

	/* process image via the Look-up table */
	for (i = 0; i < rows; i++) {
		for (j = 0; j < cols; j++) {
			// ������ ������ �������� ����� x,y�� ��ǥ�� ����Ѵ�.
			x = i / 2.0 - rows / 4.0;
			y = j / 2.0 - cols / 4.0;
			
			// circular zone plate ���� ����
			cos_val = cos(PI / V * (x * x) + PI / H * (y * y));

			// ���� ����
			buffer[i * cols + j] = 127.5 * (1 + cos_val);
		}
	}
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