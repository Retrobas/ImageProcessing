/***************************************************************************
 * File: makeczp.c                                                         *
 *                                                                         *
 * Desc: This program generates circular zone plate image                  *
 *                                                                         *
 *                      phi 2   phi 2                                      *
 * f(x,y) = 127.5(1+cos(---X  + ---Y  ))                                   *
 *                       V       H                                         *
 ***************************************************************************/

#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <math.h> 
#include "ip.h"

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
