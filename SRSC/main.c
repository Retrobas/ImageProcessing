#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include "ip.h"

#pragma region extern function
// pnm.c
extern image_ptr creat_pnm(int rows, int cols, int type);
extern image_ptr read_pnm(char *filename, int *rows, int *cols, int *type);
extern void write_pnm(image_ptr ptr, char *filename, int rows, int cols, int magic_number);

// bmp.c
void ConvertBMPtoPGM(char *filename, char *fileout);

// point_processing.c
extern void process_LUT(int *operation(int));
extern void creat_LUT(int *operation(int));

// histogram.c
extern void process_HE();
extern void creat_histogram();
extern void creat_CDF();

// convolution.c
extern void process_convolution();

// zone_plate.c
void creat_zone_plate();

// image_scaling.c
extern void nearest_neighbor_interpolation(image_ptr buffer, char *fileout, int rows, int cols, float x_scale, float y_scale, int type);
extern void bilinear_interpolation(image_ptr buffer, char *fileout, int rows, int cols, float x_scale, float y_scale, int type);

// frame_processing.c
extern void process_frame(int (*frame_operation)(int, int));

// image_scaling.c
extern void process_image_scaling(int process_type);

extern void ConvertBMPtoPGM();

extern int MakeFFTImage();

#pragma endregion

int main(int argc, char *argv[])
{
	MakeFFTImage();
	return 0;
}
