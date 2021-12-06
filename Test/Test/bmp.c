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
读取BMP图片信息及像素值
从下往上，从左往右
filename:文件路径
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
        printf("分配错误！");
        return -1;
    }
	fp = fopen(filename, "rb");
    if (fp == NULL)
    {
        printf("打开'image.bmp'失败！\n");
        return -1;
    }
    fread(databuf, 1, sizeof(BITMAPINFO), fp);//读取信息头及文件头信息
    pbmp = (BITMAPINFO*)databuf;
    count = pbmp->bmfHeader.bfOffBits; //实际位图数据的偏移字节数
    fileType = pbmp->bmfHeader.bfType;//位图类型
    color_byte = pbmp->bmiHeader.biBitCount / 8;	//彩色位 8-1；16-2；24-3；32-4 
    biCompression = pbmp->bmiHeader.biCompression; //压缩方式
    ImgHeight = pbmp->bmiHeader.biHeight; //位图高度
    ImgWidth = pbmp->bmiHeader.biWidth;  //位图宽度
    if (fileType == 0x4d42)
    {
        printf("文件类型正确！\n");
    }
    else
    {
        printf("文件类型错误！\n");
        return -1;
    }
    printf("读取的字节：%d\n", sizeof(BITMAPINFO));
    printf("实际位图数据的偏移字节数: %d\n", count);
    printf("彩色位:%d\n", color_byte);
    printf("压缩方式:%d\n", biCompression);
    printf("位图宽度:%d\n", ImgWidth);
    printf("位图高度:%d\n", ImgHeight);

    if ((ImgWidth * color_byte) % 4)rowlen = ((ImgWidth * color_byte) / 4 + 1) * 4;
    else rowlen = ImgWidth * color_byte;
    datapix = (unsigned char*)malloc(ImgHeight * rowlen);
    if (datapix == NULL)
    {
        printf("分配错误！");
        return -1;
    }

    fseek(fp, count, SEEK_SET); 
    fread(datapix, 1, ImgHeight* rowlen, fp); //读取位图数据
    //printf("%d\n", ImgHeight * rowlen);
    if (color_byte == 1)//8位色
    {
        for (i = 0; i < ImgHeight; i++)
        {
            for (j = 0; j < ImgWidth; j++)
            {
                printf("%d ", datapix[i * ImgWidth + j]);
           }
            printf("\n");
        }
       // res = imresize(datapix, imgg, ImgWidth, ImgHeight,height, (width+1));//缩放bmp图片
       // dhash_cvresize(imgg, array, height, (width + 1));//将bmp图片转化为0-1矩阵
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
    else if (color_byte == 2)//16位色
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
    else  if (color_byte == 3)//24位色
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
    else  if (color_byte == 4)//32位色
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
BMP图片函数，输出图片文件
filename:文件路径
name:文件保存路径
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
    RGBQUAD* pColorTable = NULL; //颜色表指针

    pColorTable = malloc(sizeof(RGBQUAD)*256);
    databuf = (unsigned char*)malloc(sizeof(BITMAPINFO));
    fp = (FILE*)malloc(sizeof(FILE));
    bmp = (FILE*)malloc(sizeof(FILE));

    if (databuf == NULL || fp == NULL || bmp == NULL)
    {
        printf("分配错误！");
        return -1;
    }
    fp = fopen(filename, "rb");
    if (fp == NULL)
    {
        printf("打开'image.bmp'失败！\n");
        return -1;
    }
    fread(databuf, 1, sizeof(BITMAPINFO), fp);//读取信息头及文件头信息
    pbmp = (BITMAPINFO*)databuf;
    count = pbmp->bmfHeader.bfOffBits; //实际位图数据的偏移字节数
    fileType = pbmp->bmfHeader.bfType;//位图类型
    color_byte = pbmp->bmiHeader.biBitCount / 8;	//彩色位 8-1；16-2；24-3；32-4 
    biCompression = pbmp->bmiHeader.biCompression; //压缩方式
    ImgHeight = pbmp->bmiHeader.biHeight; //位图高度
    ImgWidth = pbmp->bmiHeader.biWidth;  //位图宽度
    printf("实际位图数据的偏移字节数: %d\n", count);
    printf("彩色位:%d\n", color_byte);
    printf("压缩方式:%d\n", biCompression);
    printf("位图宽度:%d\n", ImgWidth);
    printf("位图高度:%d\n", ImgHeight);
    if (color_byte == 1)
    {
        fread(pColorTable, sizeof(RGBQUAD), 256, fp);//只有8位bmp图读取颜色表
    }
    if ((ImgWidth * color_byte) % 4)rowlen = ((ImgWidth * color_byte) / 4 + 1) * 4;
    else rowlen = ImgWidth * color_byte;
    datapix = (unsigned char*)malloc(ImgHeight * rowlen);
    data = (unsigned char*)malloc(rowlen);
    if (datapix == NULL)
    {
        printf("分配错误！");
        return -1;
    }

    bmp = fopen(name, "wb");
    if (bmp == NULL)
    {
        printf("打开'image.bmp'失败！\n");
        return -1;
    }
    fwrite(pbmp, 1, sizeof(BITMAPINFO), bmp);
    fseek(fp, count, SEEK_SET);
    fread(datapix, 1, ImgHeight * rowlen, fp); //读取位图数据
    if (color_byte == 1)//8位色
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
    if (color_byte == 2 || color_byte == 3 || color_byte == 4)//16位色24位色32位色
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
bmp图片直方图
filename:文件路径
name:文件保存路径
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
    double entropy = 0.0; // 定义图像的熵
    float histArray[256] = { 0.0 };// 定义图像灰度直方图的数组
    unsigned char pixel = 0;
    float max = -9999.0;

    databuf = (unsigned char*)malloc(sizeof(BITMAPINFO));
    fp = (FILE*)malloc(sizeof(FILE));
    txt = (FILE*)malloc(sizeof(FILE));
    if (databuf == NULL || fp == NULL || txt == NULL)
    {
        printf("分配错误！");
        return -1;
    }
    fp = fopen(filename, "rb");
    if (fp == NULL)
    {
        printf("打开'image.bmp'失败！\n");
        return -1;
    }
    fread(databuf, 1, sizeof(BITMAPINFO), fp);//读取信息头及文件头信息
    pbmp = (BITMAPINFO*)databuf;
    count = pbmp->bmfHeader.bfOffBits; //实际位图数据的偏移字节数
    fileType = pbmp->bmfHeader.bfType;//位图类型
    color_byte = pbmp->bmiHeader.biBitCount / 8;	//彩色位 8-1；16-2；24-3；32-4 
    biCompression = pbmp->bmiHeader.biCompression; //压缩方式
    ImgHeight = pbmp->bmiHeader.biHeight; //位图高度
    ImgWidth = pbmp->bmiHeader.biWidth;  //位图宽度
    if (fileType == 0x4d42)
    {
        printf("文件类型正确！\n");
    }
    else
    {
        printf("文件类型错误！\n");
        return -1;
    }
    printf("实际位图数据的偏移字节数: %d\n", count);
    printf("彩色位:%d\n", color_byte);
    printf("压缩方式:%d\n", biCompression);
    printf("位图宽度:%d\n", ImgWidth);
    printf("位图高度:%d\n", ImgHeight);

    if ((ImgWidth * color_byte) % 4)rowlen = ((ImgWidth * color_byte) / 4 + 1) * 4;
    else rowlen = ImgWidth * color_byte;
    datapix = (unsigned char*)malloc(ImgHeight * rowlen);
    if (datapix == NULL)
    {
        printf("分配错误！");
        return -1;
    }

    fseek(fp, count, SEEK_SET);
    fread(datapix, 1, ImgHeight * rowlen, fp); //读取位图数据
    //printf("%d\n", ImgHeight * rowlen);
    if (color_byte == 1)//8位色
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
            //fprintf(txt, "像素值为%d的像元频率为：%lf\n", i, histArray[i]);
            printf("%d\n", (int)histArray[i]);
            //histArray[i] /= (ImgHeight * ImgWidth);// 计算每个像素值的频率
            //if (histArray[i] != 0.0)// 利用灰度直方图计算图像的熵
            //{
            //    entropy -= histArray[i] * (log(histArray[i]) / log(2));
            //}
        }
        fclose(txt);
    }
    else if (color_byte == 2)//16位色
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
            //fprintf(txt, "像素值为%d的像元频率为：%lf\n", i, histArray[i]);
            printf("%d\n", (int)histArray[i]);
            //histArray[i] /= (ImgHeight * ImgWidth);// 计算每个像素值的频率
            //if (histArray[i] != 0.0)// 利用灰度直方图计算图像的熵
            //{
            //    entropy -= histArray[i] * (log(histArray[i]) / log(2));
            //}
        }
        fclose(txt);
    }
    else  if (color_byte == 3)//24位色
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
    else  if (color_byte == 4)//32位色
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
    float gray_prob[256] = { 0.0 };//每个灰度级在图像中的概率
    float gray_cdf_prob[256] = { 0.0 };//每个灰度级经过直方图均衡变换T后的积累分布概率之和
    //计算灰度级在图像中出现的概率
    for (int i = 0; i < 256; i++) {
        gray_prob[i] = ((float)histArray[i] / (ImgHeight * ImgWidth)); //每个灰度级占像素总数的比例
    }
    //计算每个灰度级经直方图均衡变换后的累计分布概率之和
    gray_cdf_prob[0] = gray_prob[0];//灰度值为0为第一个值，概率和等于它本身
    for (int i = 1; i < 256; i++) {
        gray_cdf_prob[i] = gray_cdf_prob[i - 1] + gray_prob[i];
    }
    //构建直方图均衡后的灰度级查找表,公式s = T(r) = (L-1)ΣP(rj)   
    //最终灰度值四舍五入
    for (int i = 0; i < 256; i++) {
        gray_des_lut[i] = (256 - 1) * gray_cdf_prob[i] + 0.5;
    }
    return 0;
}
/*
bmp直方图均衡
filename:文件路径
name:文件保存路径
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
    int histArray[256] = { 0 };// 定义图像灰度直方图的数组
    int histArray2[256] = { 0 };// 定义图像灰度直方图的数组
    int histArray3[256] = { 0 };// 定义图像灰度直方图的数组
    int gray_des_lut[256] = { 0 };//直方图均衡后的灰度查找表
    int gray_des_lut2[256] = { 0 };//直方图均衡后的灰度查找表
    int gray_des_lut3[256] = { 0 };//直方图均衡后的灰度查找表
    unsigned char pixel = 0;
    float max = -9999.0;
    unsigned char* data = NULL;
    unsigned char* data2 = NULL;
    RGBQUAD* pColorTable = NULL; //颜色表指针

    pColorTable = malloc(sizeof(RGBQUAD) * 256);
    databuf = (unsigned char*)malloc(sizeof(BITMAPINFO));
    fp = (FILE*)malloc(sizeof(FILE));
    bmp = (FILE*)malloc(sizeof(FILE));
    if (databuf == NULL || fp == NULL || bmp == NULL)
    {
        printf("分配错误！");
        return -1;
    }
    fp = fopen(filename, "rb");
    if (fp == NULL)
    {
        printf("打开'image.bmp'失败！\n");
        return -1;
    }
    fread(databuf, 1, sizeof(BITMAPINFO), fp);//读取信息头及文件头信息
    pbmp = (BITMAPINFO*)databuf;
    count = pbmp->bmfHeader.bfOffBits; //实际位图数据的偏移字节数
    fileType = pbmp->bmfHeader.bfType;//位图类型
    color_byte = pbmp->bmiHeader.biBitCount / 8;	//彩色位 8-1；16-2；24-3；32-4 
    biCompression = pbmp->bmiHeader.biCompression; //压缩方式
    ImgHeight = pbmp->bmiHeader.biHeight; //位图高度
    ImgWidth = pbmp->bmiHeader.biWidth;  //位图宽度
    if (fileType == 0x4d42)
    {
        printf("文件类型正确！\n");
    }
    else
    {
        printf("文件类型错误！\n");
        return -1;
    }
    printf("实际位图数据的偏移字节数: %d\n", count);
    printf("彩色位:%d\n", color_byte);
    printf("压缩方式:%d\n", biCompression);
    printf("位图宽度:%d\n", ImgWidth);
    printf("位图高度:%d\n", ImgHeight);
    if (color_byte == 1)
    {
        fread(pColorTable, sizeof(RGBQUAD), 256, fp);//只有8位bmp图读取颜色表
    }
    if ((ImgWidth * color_byte) % 4)rowlen = ((ImgWidth * color_byte) / 4 + 1) * 4;
    else rowlen = ImgWidth * color_byte;
    datapix = (unsigned char*)malloc(ImgHeight * rowlen);
    data = (unsigned char*)malloc(rowlen);
    if (datapix == NULL && data == NULL)
    {
        printf("分配错误！");
        return -1;
    }
    bmp = fopen(name, "wb");
    if (bmp == NULL)
    {
        printf("打开'image.bmp'失败！\n");
        return -1;
    }
    fwrite(pbmp, 1, sizeof(BITMAPINFO), bmp);
    fseek(fp, count, SEEK_SET);
    fread(datapix, 1, ImgHeight * rowlen, fp); //读取位图数据
    //printf("%d\n", ImgHeight * rowlen);
    if (color_byte == 1)//8位色
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
                data[j] = gray_des_lut[datapix[j + i * rowlen]]; // 从查找表中读取变换后的灰度值
                //printf("%d ", data[j]);
            }
            fwrite(data, rowlen, 1, bmp);
            //printf("\n");
        }
        fclose(bmp);
    }
    else if (color_byte == 2)//16位色
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
    else  if (color_byte == 3)//24位色
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
    else  if (color_byte == 4)//32位色
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