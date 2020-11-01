#include<stdio.h>
#include<stdlib.h>
#include<math.h>

typedef unsigned short int WORD;
typedef unsigned int DWORD;
typedef int LONG;
typedef unsigned char BYTE;
//asdf1234
/*********** *********** *********** *********** *********** *********** ***********
* definition :struct
* Description :λͼ�ļ�ͷ
*********** *********** *********** *********** *********** *********** ***********/
#pragma pack(1)/////////////////���ṹ���г�Ա      ��n�ֽڶ���
typedef struct tagBITMAPFILEHEADER
{
	WORD bfType;////////////////�ļ����ͣ�����ΪBM
	DWORD bfSize;///////////////ָ���ļ���С�����ֽ�Ϊ��λ��3-6�ֽڣ���λ��ǰ��
	WORD bfReserved1;///////////�ļ������֣�����Ϊ0
	WORD bfReserved2;///////////�ļ������֣�����Ϊ0
	DWORD bfOffBits;////////////���ļ�ͷ��ʵ��λͼ���ݵ�ƫ���ֽ�����11-14�ֽڣ���λ��ǰ��
}BITMAPFILEHEADER;
/*********** *********** *********** *********** *********** *********** ***********
* definition :struct
* Description :λͼ��Ϣͷ
*********** *********** *********** *********** *********** *********** ***********/
typedef struct tagBITMAPINFOHEADER
{
	DWORD biSize;///////////////���ṹ��ռ���ֽ�����Ϊ40��ע�⣺ʵ�ʲ���������44�������ֽڲ����ԭ��
	LONG biWidth;///////////////λͼ�Ŀ�ȣ�������Ϊ��λ
	LONG biHeight;//////////////λͼ�ĸ߶ȣ�������Ϊ��λ
	WORD biPlanes;//////////////Ŀ���豸�ļ��𣬱���Ϊ1
	WORD biBitCount;////////////ÿ�����������λ����1��˫ɫ����4(16ɫ����8(256ɫ��16(�߲�ɫ)��24�����ɫ����32֮һ
	DWORD biCompression;////////λͼѹ�����ͣ�0����ѹ������1(BI_RLE8ѹ�����ͣ���2(BI_RLE4ѹ�����ͣ�֮һ
	DWORD biSizeImage;//////////λͼ�Ĵ�С(���а�����Ϊ�˲���������4�ı�������ӵĿ��ֽ�)�����ֽ�Ϊ��λ
	LONG biXPelsPerMeter;///////λͼˮƽ�ֱ��ʣ�ÿ��������
	LONG biYPelsPerMeter;///////λͼ��ֱ�ֱ��ʣ�ÿ��������
	DWORD biClrUsed;////////////λͼʵ��ʹ�õ���ɫ���е���ɫ��������ֵΪ0,��ʹ����ɫ��Ϊ2��biBitCount�η�
	DWORD biClrImportant;///////λͼ��ʾ��������Ҫ����ɫ��������ֵΪ0,�����е���ɫ����Ҫ
}BITMAPINFOHEADER;
#pragma pack()//////////////////ȡ���Զ����ֽڷ�ʽ
/*********** *********** *********** *********** *********** *********** ***********
* definition :struct
* Description :��ɫ��
*********** *********** *********** *********** *********** *********** ***********/
typedef struct tagRGBQUAD
{
	BYTE rgbBlue;///////////////��ɫ�����ȣ�0-255)
	BYTE rgbGreen;//////////////��ɫ�����ȣ�0-255)
	BYTE rgbRed;////////////////��ɫ�����ȣ�0-255)
	BYTE rgbReserved;///////////����������Ϊ0
}RGBQUAD;


bool ReadBmp(BITMAPFILEHEADER &fileHeader, BITMAPINFOHEADER &infoHeader, RGBQUAD *&rgbPalette, void *img[500], char *FileName)
{
	FILE *fpIn = fopen(FileName, "rb");
	int i = 0, sizeOfHang = 0, sizeOfPalette = 0;

	if (fpIn == NULL)
		return false;

	fread(&fileHeader, sizeof(BITMAPFILEHEADER), 1, fpIn);
	fread(&infoHeader, sizeof(BITMAPINFOHEADER), 1, fpIn);

	if (infoHeader.biBitCount<16)
	{
		sizeOfPalette = int(pow(2.0, infoHeader.biBitCount));
		rgbPalette = new RGBQUAD[sizeOfPalette];

		fread(rgbPalette, sizeof(RGBQUAD), sizeOfPalette, fpIn);
	}


	if ((infoHeader.biBitCount * infoHeader.biWidth) % 32 == 0)
		sizeOfHang = (infoHeader.biBitCount * infoHeader.biWidth) / 8;
	else
		sizeOfHang = ((infoHeader.biBitCount * infoHeader.biWidth) / 32 + 1) * 4;

	for (i = 0; i<infoHeader.biHeight; i++)
	{
		img[i] = (void *) new BYTE[sizeOfHang];
		fread(img[i], sizeOfHang, 1, fpIn);
	}

	fclose(fpIn);
	return true;
}


void SaveBmp(BITMAPFILEHEADER &fileHeader, BITMAPINFOHEADER &infoHeader, RGBQUAD *rgbPalette, void *img[500], char *FileName)
{
	FILE *fpOut = fopen(FileName, "wb");
	int i = 0, sizeOfHang = 0, sizeOfPalette = 0;

	fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, fpOut);
	fwrite(&infoHeader, sizeof(BITMAPINFOHEADER), 1, fpOut);

	if (infoHeader.biBitCount<16)
	{
		sizeOfPalette = int(pow(2.0, infoHeader.biBitCount));
		fwrite(rgbPalette, sizeof(RGBQUAD), sizeOfPalette, fpOut);
	}


	if ((infoHeader.biBitCount * infoHeader.biWidth) % 32 == 0)
		sizeOfHang = (infoHeader.biBitCount * infoHeader.biWidth) / 8;
	else
		sizeOfHang = ((infoHeader.biBitCount * infoHeader.biWidth) / 32 + 1) * 4;

	for (i = 0; i<infoHeader.biHeight; i++)
		fwrite(img[i], sizeOfHang, 1, fpOut);

	fclose(fpOut);
}

void dft(short in_array[100][100], double re_array[100][100], double im_array[100][100], long height, long width)
{
	double re, im, temp;
	double pi = 3.1415926535898;
	printf("dft start\n");
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			re = 0;
			im = 0;

			for (int x = 0; x < height; x++) {
				for (int y = 0; y < width; y++) {
					temp = (double)i * x / (double)height +
						(double)j * y / (double)width;
					re += in_array[x][y] * cos(-2 * pi * temp);
					im += in_array[x][y] * sin(-2 * pi * temp);
				}
			}

			re_array[i][j] = re;
			im_array[i][j] = im;
		}
	}
	printf("dft done\n");
}

void idft(double re_array[100][100], double im_array[100][100], short out_array[100][100], long height, long width)
{
	double real, temp;
	double pi = 3.1415926535898;
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			real = 0;

			for (int x = 0; x < height; x++) {
				for (int y = 0; y < width; y++) {
					temp = (double)i * x / (double)height +
						(double)j * y / (double)width;
					real += re_array[x][y] * cos(2 * pi * temp) -
						im_array[x][y] * sin(2 * pi * temp);
				}
			}

			out_array[i][j] = (short)((float)real / sqrt(float(width*height)));
			if (out_array[i][j] > 0xff)
				out_array[i][j] = 0xff;
			else if (out_array[i][j] < 0)
				out_array[i][j] = 0;
		}
	}
	printf("idft done\n");
}

void DFT_Shift(short array[100][100], int height, int width) 
{
	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width; i++) {
			if ((i + j) % 2)
			{
				array[j][i] = -array[j][i];
			}
		}
	}
}

void IdealBand(double re_array[100][100], double im_array[100][100], int d0, int w, int size_h, int size_w)
{
	for (int j = 0; j<size_h; j++)
		for (int i = 0; i<size_w; i++)
		{
			double distance = sqrt(double(j - size_h / 2)*(j - size_h / 2) + (i - size_w / 2)*(i - size_w / 2));
			//printf("%f ", distance);
			if (distance<(d0 - w / 2) || distance>(d0 + w / 2))
			{
				if (re_array[j][i] != 0)
					printf("re_array");
				if (im_array[j][i] != 0)
					printf("im_array");
				re_array[j][i] = 0.0;
				im_array[j][i] = 0.0;
			}
		}
}

int main()
{
	BITMAPFILEHEADER fileHeader;
	BITMAPINFOHEADER infoHeader;

	double re_array[100][100];
	double im_array[100][100];
	short out_array[100][100];
	short array[100][100];
	short array_f[100][100];

	RGBQUAD *rgbPalette;//��ɫ��

	void * img[500];
	char *FileName = "2.bmp";

	ReadBmp(fileHeader, infoHeader, rgbPalette, img, FileName);

	for (int i = 0; i < infoHeader.biHeight; i++)
		for (int j = 0; j < infoHeader.biWidth; j++)
		{
			array[i][j] = (short)((unsigned char *)img[i])[j];
		}
	int width = infoHeader.biWidth;
	int height = infoHeader.biHeight;
	DFT_Shift(array, (int)infoHeader.biHeight, (int)infoHeader.biWidth);
	dft(array, re_array, im_array, (long)infoHeader.biHeight, (long)infoHeader.biWidth);
	IdealBand(re_array, im_array, 8, 8, (int)infoHeader.biHeight, (int)infoHeader.biWidth);
	
	idft(re_array, im_array, out_array, (long)infoHeader.biHeight, (long)infoHeader.biWidth);
	DFT_Shift(out_array, (int)infoHeader.biHeight, (int)infoHeader.biWidth);

	for (int i = 0; i < infoHeader.biHeight; i++)
	{
		for (int j = 0; j < infoHeader.biWidth; j++)
		{
			((unsigned char *)img[i])[j] = (unsigned char)((int)out_array[i][j]);
		}
	}


	SaveBmp(fileHeader, infoHeader, rgbPalette, img, "test_target_ball.bmp");
	system("pause");
	return 0;

}