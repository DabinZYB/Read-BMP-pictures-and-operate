#pragma once
#include "Imgg.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include "dhash.h"
#pragma pack(1)  /* �����ڽṹ�嶨��֮ǰʹ��,����Ϊ���ýṹ���и���Ա��1�ֽڶ���*/
//BMP��Ϣͷ
typedef struct tagBITMAPINFOHEADER
{
    unsigned int  biSize;      //���ṹ��ռ���ֽ�����15-18�ֽڣ�
    long  biWidth;     //λͼ�Ŀ�ȣ�������Ϊ��λ��19-22�ֽڣ�
    long  biHeight;    //λͼ�ĸ߶ȣ�������Ϊ��λ��23-26�ֽڣ�
    unsigned short biPlanes;    //Ŀ���豸�ļ��𣬱���Ϊ1(27-28�ֽڣ�
    unsigned short biBitCount;  //ÿ�����������λ����������1��˫ɫ����29-30�ֽڣ�,4(16ɫ����8(256ɫ��16(�߲�ɫ)��24�����ɫ��֮һ
    unsigned int  biCompression;//λͼѹ�����ͣ�������0����ѹ��������31-34�ֽڣ� 1(BI_RLE8ѹ�����ͣ���2(BI_RLE4ѹ�����ͣ�֮һ
    unsigned int  biSizeImage;  //λͼ�Ĵ�С(���а�����Ϊ�˲���������4�ı�������ӵĿ��ֽ�)�����ֽ�Ϊ��λ��35-38�ֽڣ�
    long  biXPelsPerMeter;//λͼˮƽ�ֱ��ʣ�ÿ����������39-42�ֽڣ�
    long  biYPelsPerMeter;//λͼ��ֱ�ֱ��ʣ�ÿ����������43-46�ֽ�)
    unsigned int  biClrUsed;      //λͼʵ��ʹ�õ���ɫ���е���ɫ����47-50�ֽڣ�
    unsigned int  biClrImportant; //λͼ��ʾ��������Ҫ����ɫ����51-54�ֽڣ�
}BITMAPINFOHEADER;

//BMPͷ�ļ�
typedef struct tagBITMAPFILEHEADER
{
    unsigned short bfType;      //����ͼƬ���͡� 'BM'
    unsigned int  bfSize;      //λͼ�ļ��Ĵ�С�����ֽ�Ϊ��λ��3-6�ֽڣ���λ��ǰ��
    unsigned short bfReserved1;//λͼ�ļ������֣�����Ϊ0(7-8�ֽڣ�
    unsigned short bfReserved2;//λͼ�ļ������֣�����Ϊ0(9-10�ֽڣ�
    unsigned int  bfOffBits;  //RGB����ƫ�Ƶ�ַ,λͼ���ݵ���ʼλ�ã��������λͼ��11-14�ֽڣ���λ��ǰ��
}BITMAPFILEHEADER;

//��ɫ��
typedef struct RGBQUAD
{
    unsigned char rgbBlue;
    unsigned char rgbGreen;
    unsigned char rgbRed;
    unsigned char rgbReserved;
}RGBQUAD;

//λͼ��Ϣͷ
typedef struct BITMAPINFO
{
    BITMAPFILEHEADER bmfHeader;
    BITMAPINFOHEADER bmiHeader;
    unsigned int RGB_MASK[3];

}BITMAPINFO;
//��ɫ��
typedef RGBQUAD* LPRGBQUAD;

int imresize(unsigned char* datapix, Imgg* ImgSca, int width, int height, int dstH, int dstW);
int read_bmp_file(char* filename);
int save_bmp_file(char* filename, char* name);
int bmp_Histogram(char* filename, char* name);
int bmp_Histogram_equalization(char* filename, char* name);