#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <malloc.h>
#include "bmp.h"

static double interpolateCubic(double x)
{
    double biCoef;
    const double a = -0.5f;
    if (x < 0)
    {
        x = -x;
    }
    biCoef = 0;
    if (x <= 1)
    {
        biCoef = ((a + 2.0) * x - (a + 3.0)) * x * x + 1.0;
    }
    else if (x < 2 && x > 1)
    {
        biCoef = ((a * x - (5.0 * a)) * x + (8.0 * a)) * x - 4.0 * a;
    }

    return biCoef;
}
int imresize(unsigned char* datapix, Imgg* ImgSca, int width,int height,int dstH, int dstW)
{
    int i, j, n, m, ii, jj;
    double tmp;
    double scrx, scry;
    double u, v, k1, k2;
    int x, y;
    ImgSca->height = dstH;
    ImgSca->width = dstW;

    for (i = 0; i < ImgSca->height; i++)
    {
        for (j = 0; j < ImgSca->width; j++)
        {
            ImgSca->data[i * ImgSca->width + j] = 0;
        }
    }
    for (n = 0; n < ImgSca->height; n++) {
        for (m = 0; m < ImgSca->width; m++) {
            scrx = (double)n * ((double)height / ImgSca->height);
            scry = (double)m * ((double)width / ImgSca->width);
            x = (int)scrx;
            y = (int)scry;
            u = scrx - (double)x;
            v = scry - (double)y;
            tmp = 0;

            for (ii = -1; ii <= 2; ii++) {
                for (jj = -1; jj <= 2; jj++) {
                    if (x + ii < 0 || y + jj < 0 || x + ii >= height || y + jj >= width)
                        continue;
                    k1 = interpolateCubic(u - (double)ii);
                    k2 = interpolateCubic((double)jj - v);
                    tmp += datapix[(x + ii) * width + (y + jj)] * k1 * k2;
                }
            }
            if (tmp < 0) { tmp = 0; }
            else if (tmp > 255) { tmp = 255; }
            ImgSca->data[n * ImgSca->width + m] = tmp;
        }
    }
    return 0;
}

/*
��ȡBMPͼƬ��Ϣ������ֵ
�������ϣ���������
filename:�ļ�·��
*/
int read_bmp_file(char* filename)
{
    FILE* txt;
	FILE* fp;
    BITMAPINFO* pbmp;
    unsigned char* databuf = NULL;
    unsigned char* datapix = NULL;
    unsigned char color_r = 0, color_g = 0, color_b = 0, alphabend = 0;
    unsigned int count;
    unsigned short color_byte;
    unsigned short fileType;
    unsigned short rowlen;
    unsigned int biCompression;
    long ImgHeight, ImgWidth;
    int res = 0,i,j;
    unsigned char rgb = 0;
    databuf = (unsigned char*)malloc(sizeof(BITMAPINFO));
    fp = (FILE*)malloc(sizeof(FILE));
    txt = (FILE*)malloc(sizeof(FILE));
    if (databuf == NULL || fp == NULL || txt == NULL)
    {
        printf("�������");
        return -1;
    }
	fp = fopen(filename, "rb");
    if (fp == NULL)
    {
        printf("��'image.bmp'ʧ�ܣ�\n");
        return -1;
    }
    fread(databuf, 1, sizeof(BITMAPINFO), fp);//��ȡ��Ϣͷ���ļ�ͷ��Ϣ
    pbmp = (BITMAPINFO*)databuf;
    count = pbmp->bmfHeader.bfOffBits; //ʵ��λͼ���ݵ�ƫ���ֽ���
    fileType = pbmp->bmfHeader.bfType;//λͼ����
    color_byte = pbmp->bmiHeader.biBitCount / 8;	//��ɫλ 8-1��16-2��24-3��32-4 
    biCompression = pbmp->bmiHeader.biCompression; //ѹ����ʽ
    ImgHeight = pbmp->bmiHeader.biHeight; //λͼ�߶�
    ImgWidth = pbmp->bmiHeader.biWidth;  //λͼ���
    if (fileType == 0x4d42)
    {
        printf("�ļ�������ȷ��\n");
    }
    else
    {
        printf("�ļ����ʹ���\n");
        return -1;
    }
    printf("��ȡ���ֽڣ�%d\n", sizeof(BITMAPINFO));
    printf("ʵ��λͼ���ݵ�ƫ���ֽ���: %d\n", count);
    printf("��ɫλ:%d\n", color_byte);
    printf("ѹ����ʽ:%d\n", biCompression);
    printf("λͼ���:%d\n", ImgWidth);
    printf("λͼ�߶�:%d\n", ImgHeight);

    if ((ImgWidth * color_byte) % 4)rowlen = ((ImgWidth * color_byte) / 4 + 1) * 4;
    else rowlen = ImgWidth * color_byte;
    datapix = (unsigned char*)malloc(ImgHeight * rowlen);
    if (datapix == NULL)
    {
        printf("�������");
        return -1;
    }

    fseek(fp, count, SEEK_SET); 
    fread(datapix, 1, ImgHeight* rowlen, fp); //��ȡλͼ����
    //printf("%d\n", ImgHeight * rowlen);
    if (color_byte == 1)//8λɫ
    {
        for (i = 0; i < ImgHeight; i++)
        {
            for (j = 0; j < ImgWidth; j++)
            {
                printf("%d ", datapix[i * ImgWidth + j]);
           }
            printf("\n");
        }
       // res = imresize(datapix, imgg, ImgWidth, ImgHeight,height, (width+1));//����bmpͼƬ
       // dhash_cvresize(imgg, array, height, (width + 1));//��bmpͼƬת��Ϊ0-1����
       // //for (i = 0; i < 8; i++)
       // //{
       // //    for (j = 0; j < 8; j++)
       // //    {
       // //        printf("%d ", array[i*8+j]);
       // //    }
       // //    printf("\n");
       // //}
       // txt = fopen(name,"w");
       //  for (i = 0; i < 8; i++)
       // {
       //     for (j = 0; j < 8; j++)
       //     {
       //         fprintf(txt, "%d", array[i * 8 + j]);
       //         //printf("%d ", array[i*8+j]);
       //     }
       // }
       //// fwrite(array, sizeof(array), 1, txt);
       // fclose(txt);
    }
    else if (color_byte == 2)//16λɫ
    {
        rgb = 0;
        for (i = 0; i < ImgHeight * rowlen; i++)
        {
            switch (rgb)
            {
            case 0:
                if (biCompression == 0)//RGB:1,5,5,5
                {
                    color_r = (datapix[i] & 0X1F) << 3;
                    color_g = ((datapix[i] & 0XE0) >> 2) | (datapix[i + 1] << 6);  //G 11111000 00000111								
                }
                else		//RGB:5,6,5
                {
                    color_r = (datapix[i] & 0X1F) << 3; 	 	//R
                    color_g = ((datapix[i] & 0XE0) >> 3) | (datapix[i + 1] << 5);//00111000  00000111
                }
            case 1:
                if (biCompression == 0)//RGB:1,5,5,5
                {
                    color_b = ((datapix[i]) & 0X7C) << 1;  //B
                }
                else  		//RGB:5,6,5
                {
                    color_b = (datapix[i]) & 0XF8; //B
                }
            }
            rgb++;
            if (rgb == color_byte) 
            {
                //printf("%d %d %d ", color_r,color_g,color_b);
                //if ((i+1) % 256 == 0) { printf("\n"); }
                color_r = 0;
                color_g = 0;
                color_b = 0;
                rgb = 0;
            }
        }
    }
    else  if (color_byte == 3)//24λɫ
    {
        rgb = 0;
        for (i = 0; i < ImgHeight * rowlen; i++)
        {
            switch (rgb)
            {
            case 0:
                color_r = datapix[i]; //R
            case 1:
                color_g = datapix[i];//G
            case 2:
                color_b = datapix[i];//B	  
            }
            rgb++;
            if (rgb == color_byte)
            {
                //printf("%d %d %d ", color_r,color_g,color_b);
                //if ((i+1) % 256 == 0) { printf("\n"); }
                color_r = 0;
                color_g = 0;
                color_b = 0;
                rgb = 0;
            }
        }
    }
    else  if (color_byte == 4)//32λɫ
    {
        rgb = 0;
        for (i = 0; i < ImgHeight * rowlen; i++)
        {
            switch (rgb)
            {
            case 0:
                color_r = datapix[i]; //R
            case 1:
                color_g = datapix[i];//G
            case 2:
                color_b = datapix[i];//B	  
            //case 3:
            //    alphabend=datapix[i];
            }
            rgb++;
            if (rgb == color_byte)
            {
                //printf("%d %d %d ", color_r,color_g,color_b);
                //if ((i+1) % 256 == 0) { printf("\n"); }
                color_r = 0;
                color_g = 0;
                color_b = 0;
                rgb = 0;
            }
        }
    }

    free(databuf);
    databuf = NULL;
    free(datapix);
    datapix = NULL;
    free(fp);
    fp = NULL;
    free(txt);
    txt = NULL;
    return 0;
}

/*
BMPͼƬ���������ͼƬ�ļ�
filename:�ļ�·��
name:�ļ�����·��
*/
int save_bmp_file(char* filename, char* name)
{
    FILE* bmp;
    FILE* fp;
    BITMAPINFO* pbmp;
    unsigned char* databuf = NULL;
    unsigned char* datapix = NULL;
    unsigned char color_r = 0, color_g = 0, color_b = 0, alphabend = 0;
    unsigned int count;
    unsigned short color_byte;
    unsigned short fileType;
    unsigned short rowlen;
    unsigned int biCompression;
    long ImgHeight, ImgWidth;
    int res = 0, i, j;
    unsigned char rgb = 0;
    unsigned int  bmpSize;
    unsigned char* data = NULL;
    RGBQUAD* pColorTable = NULL; //��ɫ��ָ��

    pColorTable = malloc(sizeof(RGBQUAD)*256);
    databuf = (unsigned char*)malloc(sizeof(BITMAPINFO));
    fp = (FILE*)malloc(sizeof(FILE));
    bmp = (FILE*)malloc(sizeof(FILE));

    if (databuf == NULL || fp == NULL || bmp == NULL)
    {
        printf("�������");
        return -1;
    }
    fp = fopen(filename, "rb");
    if (fp == NULL)
    {
        printf("��'image.bmp'ʧ�ܣ�\n");
        return -1;
    }
    fread(databuf, 1, sizeof(BITMAPINFO), fp);//��ȡ��Ϣͷ���ļ�ͷ��Ϣ
    pbmp = (BITMAPINFO*)databuf;
    count = pbmp->bmfHeader.bfOffBits; //ʵ��λͼ���ݵ�ƫ���ֽ���
    fileType = pbmp->bmfHeader.bfType;//λͼ����
    color_byte = pbmp->bmiHeader.biBitCount / 8;	//��ɫλ 8-1��16-2��24-3��32-4 
    biCompression = pbmp->bmiHeader.biCompression; //ѹ����ʽ
    ImgHeight = pbmp->bmiHeader.biHeight; //λͼ�߶�
    ImgWidth = pbmp->bmiHeader.biWidth;  //λͼ���
    printf("ʵ��λͼ���ݵ�ƫ���ֽ���: %d\n", count);
    printf("��ɫλ:%d\n", color_byte);
    printf("ѹ����ʽ:%d\n", biCompression);
    printf("λͼ���:%d\n", ImgWidth);
    printf("λͼ�߶�:%d\n", ImgHeight);
    if (color_byte == 1)
    {
        fread(pColorTable, sizeof(RGBQUAD), 256, fp);//ֻ��8λbmpͼ��ȡ��ɫ��
    }
    if ((ImgWidth * color_byte) % 4)rowlen = ((ImgWidth * color_byte) / 4 + 1) * 4;
    else rowlen = ImgWidth * color_byte;
    datapix = (unsigned char*)malloc(ImgHeight * rowlen);
    data = (unsigned char*)malloc(rowlen);
    if (datapix == NULL)
    {
        printf("�������");
        return -1;
    }

    bmp = fopen(name, "wb");
    if (bmp == NULL)
    {
        printf("��'image.bmp'ʧ�ܣ�\n");
        return -1;
    }
    fwrite(pbmp, 1, sizeof(BITMAPINFO), bmp);
    fseek(fp, count, SEEK_SET);
    fread(datapix, 1, ImgHeight * rowlen, fp); //��ȡλͼ����
    if (color_byte == 1)//8λɫ
    {
        fwrite(pColorTable, sizeof(RGBQUAD), 256, bmp);
        fseek(bmp, count, SEEK_SET);
        //fwrite(datapix, ImgHeight * rowlen,1, bmp);
        for (i = 0; i < ImgHeight; i++)
        {
            for (j = 0; j < ImgWidth; j++)
            {    
                data[j] = datapix[i * ImgWidth + j];
                //printf("%d ", data[j]);
            }
            fwrite(data, rowlen, 1, bmp);
            //printf("\n");
        }
        fclose(bmp);
    }
    if (color_byte == 2 || color_byte == 3 || color_byte == 4)//16λɫ24λɫ32λɫ
    {
        fseek(bmp, count, SEEK_SET);
        fwrite(datapix, ImgHeight * rowlen, 1, bmp);
    }
    fclose(fp);
    free(pColorTable);
    pColorTable = NULL;
    free(databuf);
    databuf = NULL;
    free(datapix);
    datapix = NULL;
    free(data);
    data = NULL;
    free(fp);
    fp = NULL;
    free(bmp);
    bmp = NULL;
    return 0;
}

/*
bmpͼƬֱ��ͼ
filename:�ļ�·��
name:�ļ�����·��
*/
int bmp_Histogram(char* filename, char* name)
{
    FILE* txt;
    FILE* fp;
    BITMAPINFO* pbmp;
    unsigned char* databuf = NULL;
    unsigned char* datapix = NULL;
    unsigned char color_r = 0, color_g = 0, color_b = 0, alphabend = 0;
    unsigned int count;
    unsigned short color_byte;
    unsigned short fileType;
    unsigned short rowlen;
    unsigned int biCompression;
    long ImgHeight, ImgWidth;
    int res = 0, i, j;
    unsigned char rgb = 0;
    double entropy = 0.0; // ����ͼ�����
    float histArray[256] = { 0.0 };// ����ͼ��Ҷ�ֱ��ͼ������
    unsigned char pixel = 0;
    float max = -9999.0;

    databuf = (unsigned char*)malloc(sizeof(BITMAPINFO));
    fp = (FILE*)malloc(sizeof(FILE));
    txt = (FILE*)malloc(sizeof(FILE));
    if (databuf == NULL || fp == NULL || txt == NULL)
    {
        printf("�������");
        return -1;
    }
    fp = fopen(filename, "rb");
    if (fp == NULL)
    {
        printf("��'image.bmp'ʧ�ܣ�\n");
        return -1;
    }
    fread(databuf, 1, sizeof(BITMAPINFO), fp);//��ȡ��Ϣͷ���ļ�ͷ��Ϣ
    pbmp = (BITMAPINFO*)databuf;
    count = pbmp->bmfHeader.bfOffBits; //ʵ��λͼ���ݵ�ƫ���ֽ���
    fileType = pbmp->bmfHeader.bfType;//λͼ����
    color_byte = pbmp->bmiHeader.biBitCount / 8;	//��ɫλ 8-1��16-2��24-3��32-4 
    biCompression = pbmp->bmiHeader.biCompression; //ѹ����ʽ
    ImgHeight = pbmp->bmiHeader.biHeight; //λͼ�߶�
    ImgWidth = pbmp->bmiHeader.biWidth;  //λͼ���
    if (fileType == 0x4d42)
    {
        printf("�ļ�������ȷ��\n");
    }
    else
    {
        printf("�ļ����ʹ���\n");
        return -1;
    }
    printf("ʵ��λͼ���ݵ�ƫ���ֽ���: %d\n", count);
    printf("��ɫλ:%d\n", color_byte);
    printf("ѹ����ʽ:%d\n", biCompression);
    printf("λͼ���:%d\n", ImgWidth);
    printf("λͼ�߶�:%d\n", ImgHeight);

    if ((ImgWidth * color_byte) % 4)rowlen = ((ImgWidth * color_byte) / 4 + 1) * 4;
    else rowlen = ImgWidth * color_byte;
    datapix = (unsigned char*)malloc(ImgHeight * rowlen);
    if (datapix == NULL)
    {
        printf("�������");
        return -1;
    }

    fseek(fp, count, SEEK_SET);
    fread(datapix, 1, ImgHeight * rowlen, fp); //��ȡλͼ����
    //printf("%d\n", ImgHeight * rowlen);
    if (color_byte == 1)//8λɫ
    {
        for (i = 0; i < ImgHeight; i++)
        {
            for (j = 0; j < ImgWidth; j++)
            {
                pixel = datapix[j + i * ImgWidth];
                histArray[pixel]++;
                //printf("%d ", datapix[i * ImgWidth + j]);
            }
            //printf("\n");
        }
        txt = fopen(name, "wb");

        for (int i = 0; i < 256; i++)
        {      
            fprintf(txt, "%lf\n", histArray[i]);
            //fprintf(txt, "����ֵΪ%d����ԪƵ��Ϊ��%lf\n", i, histArray[i]);
            printf("%d\n", (int)histArray[i]);
            //histArray[i] /= (ImgHeight * ImgWidth);// ����ÿ������ֵ��Ƶ��
            //if (histArray[i] != 0.0)// ���ûҶ�ֱ��ͼ����ͼ�����
            //{
            //    entropy -= histArray[i] * (log(histArray[i]) / log(2));
            //}
        }
        fclose(txt);
    }
    else if (color_byte == 2)//16λɫ
    {
        rgb = 0;
        for (i = 0; i < ImgHeight * rowlen; i++)
        {
            switch (rgb)
            {
            case 0:
                if (biCompression == 0)//RGB:1,5,5,5
                {
                    color_r = (datapix[i] & 0X1F) << 3;
                    color_g = ((datapix[i] & 0XE0) >> 2) | (datapix[i + 1] << 6);  //G 11111000 00000111								
                }
                else		//RGB:5,6,5
                {
                    color_r = (datapix[i] & 0X1F) << 3; 	 	//R
                    color_g = ((datapix[i] & 0XE0) >> 3) | (datapix[i + 1] << 5);//00111000  00000111
                }
            case 1:
                if (biCompression == 0)//RGB:1,5,5,5
                {
                    color_b = ((datapix[i]) & 0X7C) << 1;  //B
                }
                else  		//RGB:5,6,5
                {
                    color_b = (datapix[i]) & 0XF8; //B
                }
            }
            rgb++;
            if (rgb == color_byte)
            {
                pixel = color_r*0.3+color_g*0.59+color_b*0.11;
                histArray[pixel]++;
                //printf("%d %d %d ", color_r,color_g,color_b);
                //if ((i+1) % 256 == 0) { printf("\n"); }
                color_r = 0;
                color_g = 0;
                color_b = 0;
                rgb = 0;
            }
        }
        txt = fopen(name, "wb");

        for (int i = 0; i < 256; i++)
        {
            fprintf(txt, "%lf\n", histArray[i]);
            //fprintf(txt, "����ֵΪ%d����ԪƵ��Ϊ��%lf\n", i, histArray[i]);
            printf("%d\n", (int)histArray[i]);
            //histArray[i] /= (ImgHeight * ImgWidth);// ����ÿ������ֵ��Ƶ��
            //if (histArray[i] != 0.0)// ���ûҶ�ֱ��ͼ����ͼ�����
            //{
            //    entropy -= histArray[i] * (log(histArray[i]) / log(2));
            //}
        }
        fclose(txt);
    }
    else  if (color_byte == 3)//24λɫ
    {
        rgb = 0;
        for (i = 0; i < ImgHeight * rowlen; i++)
        {
            switch (rgb)
            {
            case 0:
                color_r = datapix[i]; //R
            case 1:
                color_g = datapix[i];//G
            case 2:
                color_b = datapix[i];//B	  
            }
            rgb++;
            if (rgb == color_byte)
            {
                //printf("%d %d %d ", color_r,color_g,color_b);
                //if ((i+1) % 256 == 0) { printf("\n"); }
                color_r = 0;
                color_g = 0;
                color_b = 0;
                rgb = 0;
            }
        }
    }
    else  if (color_byte == 4)//32λɫ
    {
        rgb = 0;
        for (i = 0; i < ImgHeight * rowlen; i++)
        {
            switch (rgb)
            {
            case 0:
                color_r = datapix[i]; //R
            case 1:
                color_g = datapix[i];//G
            case 2:
                color_b = datapix[i];//B	  
            //case 3:
            //    alphabend=datapix[i];
            }
            rgb++;
            if (rgb == color_byte)
            {
                //printf("%d %d %d ", color_r,color_g,color_b);
                //if ((i+1) % 256 == 0) { printf("\n"); }
                color_r = 0;
                color_g = 0;
                color_b = 0;
                rgb = 0;
            }
        }
    }

    free(databuf);
    databuf = NULL;
    free(datapix);
    datapix = NULL;
    free(fp);
    fp = NULL;
    free(txt);
    txt = NULL;
    return 0;
}

static int Histogram_equalization(int* histArray, int* gray_des_lut ,long ImgHeight, long ImgWidth)
{
    float gray_prob[256] = { 0.0 };//ÿ���Ҷȼ���ͼ���еĸ���
    float gray_cdf_prob[256] = { 0.0 };//ÿ���Ҷȼ�����ֱ��ͼ����任T��Ļ��۷ֲ�����֮��
    //����Ҷȼ���ͼ���г��ֵĸ���
    for (int i = 0; i < 256; i++) {
        gray_prob[i] = ((float)histArray[i] / (ImgHeight * ImgWidth)); //ÿ���Ҷȼ�ռ���������ı���
    }
    //����ÿ���Ҷȼ���ֱ��ͼ����任����ۼƷֲ�����֮��
    gray_cdf_prob[0] = gray_prob[0];//�Ҷ�ֵΪ0Ϊ��һ��ֵ�����ʺ͵���������
    for (int i = 1; i < 256; i++) {
        gray_cdf_prob[i] = gray_cdf_prob[i - 1] + gray_prob[i];
    }
    //����ֱ��ͼ�����ĻҶȼ����ұ�,��ʽs = T(r) = (L-1)��P(rj)   
    //���ջҶ�ֵ��������
    for (int i = 0; i < 256; i++) {
        gray_des_lut[i] = (256 - 1) * gray_cdf_prob[i] + 0.5;
    }
    return 0;
}
/*
bmpֱ��ͼ����
filename:�ļ�·��
name:�ļ�����·��
*/
int bmp_Histogram_equalization(char* filename, char* name)
{
    FILE* bmp;
    FILE* fp;
    BITMAPINFO* pbmp;
    unsigned char* databuf = NULL;
    unsigned char* datapix = NULL;
    unsigned char color_r = 0, color_g = 0, color_b = 0, alphabend = 0;
    unsigned int count;
    unsigned short color_byte;
    unsigned short fileType;
    unsigned short rowlen;
    unsigned int biCompression;
    long ImgHeight, ImgWidth;
    int res = 0, i, j,num;
    unsigned char rgb = 0;
    int histArray[256] = { 0 };// ����ͼ��Ҷ�ֱ��ͼ������
    int histArray2[256] = { 0 };// ����ͼ��Ҷ�ֱ��ͼ������
    int histArray3[256] = { 0 };// ����ͼ��Ҷ�ֱ��ͼ������
    int gray_des_lut[256] = { 0 };//ֱ��ͼ�����ĻҶȲ��ұ�
    int gray_des_lut2[256] = { 0 };//ֱ��ͼ�����ĻҶȲ��ұ�
    int gray_des_lut3[256] = { 0 };//ֱ��ͼ�����ĻҶȲ��ұ�
    unsigned char pixel = 0;
    float max = -9999.0;
    unsigned char* data = NULL;
    unsigned char* data2 = NULL;
    RGBQUAD* pColorTable = NULL; //��ɫ��ָ��

    pColorTable = malloc(sizeof(RGBQUAD) * 256);
    databuf = (unsigned char*)malloc(sizeof(BITMAPINFO));
    fp = (FILE*)malloc(sizeof(FILE));
    bmp = (FILE*)malloc(sizeof(FILE));
    if (databuf == NULL || fp == NULL || bmp == NULL)
    {
        printf("�������");
        return -1;
    }
    fp = fopen(filename, "rb");
    if (fp == NULL)
    {
        printf("��'image.bmp'ʧ�ܣ�\n");
        return -1;
    }
    fread(databuf, 1, sizeof(BITMAPINFO), fp);//��ȡ��Ϣͷ���ļ�ͷ��Ϣ
    pbmp = (BITMAPINFO*)databuf;
    count = pbmp->bmfHeader.bfOffBits; //ʵ��λͼ���ݵ�ƫ���ֽ���
    fileType = pbmp->bmfHeader.bfType;//λͼ����
    color_byte = pbmp->bmiHeader.biBitCount / 8;	//��ɫλ 8-1��16-2��24-3��32-4 
    biCompression = pbmp->bmiHeader.biCompression; //ѹ����ʽ
    ImgHeight = pbmp->bmiHeader.biHeight; //λͼ�߶�
    ImgWidth = pbmp->bmiHeader.biWidth;  //λͼ���
    if (fileType == 0x4d42)
    {
        printf("�ļ�������ȷ��\n");
    }
    else
    {
        printf("�ļ����ʹ���\n");
        return -1;
    }
    printf("ʵ��λͼ���ݵ�ƫ���ֽ���: %d\n", count);
    printf("��ɫλ:%d\n", color_byte);
    printf("ѹ����ʽ:%d\n", biCompression);
    printf("λͼ���:%d\n", ImgWidth);
    printf("λͼ�߶�:%d\n", ImgHeight);
    if (color_byte == 1)
    {
        fread(pColorTable, sizeof(RGBQUAD), 256, fp);//ֻ��8λbmpͼ��ȡ��ɫ��
    }
    if ((ImgWidth * color_byte) % 4)rowlen = ((ImgWidth * color_byte) / 4 + 1) * 4;
    else rowlen = ImgWidth * color_byte;
    datapix = (unsigned char*)malloc(ImgHeight * rowlen);
    data = (unsigned char*)malloc(rowlen);
    if (datapix == NULL && data == NULL)
    {
        printf("�������");
        return -1;
    }
    bmp = fopen(name, "wb");
    if (bmp == NULL)
    {
        printf("��'image.bmp'ʧ�ܣ�\n");
        return -1;
    }
    fwrite(pbmp, 1, sizeof(BITMAPINFO), bmp);
    fseek(fp, count, SEEK_SET);
    fread(datapix, 1, ImgHeight * rowlen, fp); //��ȡλͼ����
    //printf("%d\n", ImgHeight * rowlen);
    if (color_byte == 1)//8λɫ
    {
        fwrite(pColorTable, sizeof(RGBQUAD), 256, bmp);
        for (i = 0; i < ImgHeight; i++)
        {
            for (j = 0; j < ImgWidth; j++)
            {
                pixel = datapix[j + i * ImgWidth];
                histArray[pixel]++;
                //printf("%d ", datapix[i * ImgWidth + j]);
            }
            //printf("\n");
        }
        Histogram_equalization(histArray, gray_des_lut, ImgHeight, ImgWidth);
        fseek(bmp, count, SEEK_SET);
        for (i = 0; i < ImgHeight; i++)
        {
            for (j = 0; j < ImgWidth; j++)
            {
                data[j] = gray_des_lut[datapix[j + i * rowlen]]; // �Ӳ��ұ��ж�ȡ�任��ĻҶ�ֵ
                //printf("%d ", data[j]);
            }
            fwrite(data, rowlen, 1, bmp);
            //printf("\n");
        }
        fclose(bmp);
    }
    else if (color_byte == 2)//16λɫ
    {
        data2 = (unsigned char*)malloc(ImgWidth*3);
        if (data2 == NULL)
        {

            printf("error");
            return -1;
        }
        rgb = 0;
        for (i = 0; i < ImgHeight * rowlen; i++)
        {
            switch (rgb)
            {
            case 0:
                if (biCompression == 0)//RGB:1,5,5,5
                {
                    color_r = (datapix[i] & 0X1F) << 3;
                    color_g = ((datapix[i] & 0XE0) >> 2) | (datapix[i + 1] << 6);  //G 11111000 00000111								
                }
                else		//RGB:5,6,5
                {
                    color_r = (datapix[i] & 0X1F) << 3; 	 	//R
                    color_g = ((datapix[i] & 0XE0) >> 3) | (datapix[i + 1] << 5);//00111000  00000111
                }
            case 1:
                if (biCompression == 0)//RGB:1,5,5,5
                {
                    color_b = ((datapix[i]) & 0X7C) << 1;  //B
                }
                else  		//RGB:5,6,5
                {
                    color_b = (datapix[i]) & 0XF8; //B
                }
            }
            rgb++;
            if (rgb == color_byte)
            {
                //pixel = color_r * 0.3 + color_g * 0.59 + color_b * 0.11;
                pixel = color_r;
                histArray[pixel]++;
                pixel = color_g;
                histArray2[pixel]++;
                pixel = color_b;
                histArray3[pixel]++;
                //printf("%d %d %d ", color_r,color_g,color_b);
                //if ((i+1) % 256 == 0) { printf("\n"); }
                color_r = 0;
                color_g = 0;
                color_b = 0;
                rgb = 0;
            }
        }
        
        Histogram_equalization(histArray, gray_des_lut, ImgHeight, ImgWidth);//r
        Histogram_equalization(histArray2, gray_des_lut2, ImgHeight, ImgWidth);//g
        Histogram_equalization(histArray3, gray_des_lut3, ImgHeight, ImgWidth);//b
        fseek(bmp, count, SEEK_SET);
        num = 0;
        for (i = 0; i < ImgHeight; i++)
        {
            for (j = 0; j < ImgWidth; j++)
            {
                data2[j * 3 + 0] = gray_des_lut[(unsigned char)((datapix[num] & 0X1F) << 3)];
                data2[j * 3 + 1] = gray_des_lut[((unsigned char)((datapix[num] & 0XE0) >> 3) | (unsigned char)(datapix[num + 1] << 5))];
                data2[j * 3 + 2] = gray_des_lut[(unsigned char)((datapix[num+1]) & 0XF8)];
                num = num + 2;
                data[j * 2 + 0] = (unsigned char)(((data2[j * 3 + 0] & 0XF8) >> 3) | ((data2[j * 3 + 1] & 0X1C) << 3));
                data[j * 2 + 1] = (unsigned char)(((data2[j * 3 + 1] & 0XE0) >> 5) | (data2[j * 3 + 2] & 0XF8));
            }
            fwrite(data, rowlen, 1, bmp);
            //printf("\n");
        }
        fclose(bmp);
    }
    else  if (color_byte == 3)//24λɫ
    {
        rgb = 0;
        for (i = 0; i < ImgHeight * rowlen; i++)
        {
            switch (rgb)
            {
            case 0:
                color_r = datapix[i]; //R
            case 1:
                color_g = datapix[i];//G
            case 2:
                color_b = datapix[i];//B	  
            }
            rgb++;
            if (rgb == color_byte)
            {
                //printf("%d %d %d ", color_r,color_g,color_b);
                //if ((i+1) % 256 == 0) { printf("\n"); }
                color_r = 0;
                color_g = 0;
                color_b = 0;
                rgb = 0;
            }
        }
    }
    else  if (color_byte == 4)//32λɫ
    {
        rgb = 0;
        for (i = 0; i < ImgHeight * rowlen; i++)
        {
            switch (rgb)
            {
            case 0:
                color_r = datapix[i]; //R
            case 1:
                color_g = datapix[i];//G
            case 2:
                color_b = datapix[i];//B	  
            //case 3:
            //    alphabend=datapix[i];
            }
            rgb++;
            if (rgb == color_byte)
            {
                //printf("%d %d %d ", color_r,color_g,color_b);
                //if ((i+1) % 256 == 0) { printf("\n"); }
                color_r = 0;
                color_g = 0;
                color_b = 0;
                rgb = 0;
            }
        }
    }


    free(databuf);
    databuf = NULL;
    free(datapix);
    datapix = NULL;
    free(fp);
    fp = NULL;
    free(bmp);
    bmp = NULL;
    return 0;

}