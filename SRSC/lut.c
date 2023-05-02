#include "ip.h"

void apply_LUT(image_ptr buffer, unsigned long number_of_pixels, int *operation(int));
void change_LUT(image_ptr buffer, unsigned long number_of_pixels, int *operation(int));
void set_LUT(unsigned char *LUT, int *operation(int));

int nullLUT(int value);
int customLUT(int value);
int gamma_collection(int value);
int nagative_transform(int value);
int streching(int value);
int compression(int value);
int posterizing(int value);
int thresholding(int value);
int bounded_thresholding(int value);
int iso_intensity_contouring(int value);
int solarizing(int value);

void apply_LUT(image_ptr buffer, unsigned long number_of_pixels, int *operation(int))
{
	unsigned char LUT[256];
	unsigned long i;

	set_LUT(LUT, operation);

	for (i = 0; i < number_of_pixels; i++)
		buffer[i] = LUT[buffer[i]];
}

void change_LUT(image_ptr buffer, unsigned long number_of_pixels, int *operation(int))
{
	unsigned char LUT[256];
	unsigned long i, j;

	set_LUT(LUT, operation);

	// 256 x 256 �����̶�� ����
	for (j = 0; j < 256; ++j) {
		for (i = 0; i < LUT[j]; ++i) {
			buffer[256 * (255 - i) + j] = 255;
		}
		for (; i < 256; ++i) {
			buffer[256 * (255 - i) + j] = 0;
		}
	}
}

void set_LUT(unsigned char *LUT, int *operation(int))
{
	unsigned long i;
	int temp;

	for (i = 0; i < 256; i++)
	{
		temp = operation(i);
		CLIP(temp, 0, 255);
		LUT[i] = temp;
	}
}

#pragma region operation ����

int nullLUT(int value)
{
	return value;
}

int customLUT(int value)
{
	return value;
}

// contrast ����/���� ȿ��
int gamma_collection(int value)
{
	float gamma = 2;
	return (pow(value, gamma) / pow(255, gamma)) * 255;
}

// ������ LUT�� y=x�ε�, �̰� LUT�� y =-x+255 �� nagative
int nagative_transform(int value)
{
	return 255 - value;
}

// contrast ���� ȿ��
int streching(int value)
{
	int low = 30, high = 30;
	if (value < low) return 0;
	else if (value > 255 - high) return 255;

	float a = 255.0 / (255 - low - high);
	return a * (value - low);
}

// contrast ���� ȿ��
int compression(int value)
{
	int low = 30, high = 30;
	return (255 - low - high) / 255.0 * value + low;
}

// �� level�� ����
int posterizing(int value)
{
	int step = 32;
	return value / step * step;
}

// level�� binary �������� ����
int thresholding(int value)
{
	return value < 128 ? 0 : 255;
}

// Ư�� ������ level�� binary �������� ����
int bounded_thresholding(int value)
{
	int low = 80, mid = 120, high = 160;
	if (value > low && value < mid) return low;
	else if (value >= mid && value < high) return high;
	else return value;
}

// Ư�� level�� ���ֹ���
int iso_intensity_contouring(int value)
{
	return value % 32 < 16 ? 0 : value;
}

// Ư�� �κ��� nagative�ϰ� �ٲ�
int solarizing(int value)
{
	return  value < 180 ? value : 255 - value;
}

#pragma endregion
