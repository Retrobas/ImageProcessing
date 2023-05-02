#include "ip.h"

void histogram_equalize(image_ptr buffer, unsigned long number_of_pixels);
void change_histogram(image_ptr buffer, unsigned long number_of_pixels);
void change_CDF(image_ptr buffer, unsigned long number_of_pixels);

void histogram_equalize(image_ptr buffer, unsigned long number_of_pixels)
{
	unsigned long histogram[256] = { 0, };
	unsigned long CDF[256];
	float scale_factor;
	unsigned long i;
	unsigned long sum;

	// 히스토그램 계산
	for (i = 0; i < number_of_pixels; i++)
		histogram[buffer[i]]++;

	// CDF 계산
	sum = 0;
	scale_factor = 255.0 / number_of_pixels;
	for (i = 0; i < 256; i++)
	{
		sum += histogram[i];
		CDF[i] = (sum * scale_factor) + 0.5;
	}

	// CDF를 LUT로 사용
	for (i = 0; i < number_of_pixels; i++)
		buffer[i] = CDF[buffer[i]];
}

void change_histogram(image_ptr buffer, unsigned long number_of_pixels)
{
	unsigned long histogram[256] = { 0, };
	unsigned long i, j;

	// 히스토그램 계산
	for (i = 0; i < number_of_pixels; i++)
		histogram[buffer[i]]++;

	// 히스토그램 정규화 [0, 256)
	int max = 0;
	for (i = 0; i < 256; i++) {
		if (max < histogram[i]) max = histogram[i];
	}
	for (i = 0; i < 256; i++) {
		histogram[i] = (unsigned long)((float)histogram[i] / max * 255.0f);
	}

	// 256 x 256 파일이라고 가정
	for (j = 0; j < 256; ++j) {
		for (i = 0; i < histogram[j]; ++i) {
			buffer[256 * (255 - i) + j] = 0;
		}
		for (; i < 256; ++i) {
			buffer[256 * (255 - i) + j] = 255;
		}
	}
}

void change_CDF(image_ptr buffer, unsigned long number_of_pixels)
{
	unsigned long histogram[256] = { 0, };
	unsigned long CDF[256];
	float scale_factor;
	unsigned long i, j;
	unsigned long sum;

	// 히스토그램 계산
	for (i = 0; i < number_of_pixels; i++)
		histogram[buffer[i]]++;

	// CDF 계산
	sum = 0;
	scale_factor = 255.0 / number_of_pixels;
	for (i = 0; i < 256; i++)
	{
		sum += histogram[i];
		CDF[i] = (sum * scale_factor) + 0.5;
	}

	// CDF 정규화 [0, 256)
	int max = 0;
	for (i = 0; i < 256; i++) {
		if (max < CDF[i]) max = CDF[i];
	}
	for (i = 0; i < 256; i++) {
		CDF[i] = (unsigned long)((float)CDF[i] / max * 255.0f);
	}

	// 256 x 256 파일이라고 가정
	for (j = 0; j < 256; ++j) {
		for (i = 0; i < CDF[j]; ++i) {
			buffer[256 * (255 - i) + j] = 0;
		}
		for (; i < 256; ++i) {
			buffer[256 * (255 - i) + j] = 255;
		}
	}
}