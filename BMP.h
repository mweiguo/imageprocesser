// BMP.h: interface for the CBMP class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BMP_H__65B7E19A_F952_4244_8979_74AB9669C358__INCLUDED_)
#define AFX_BMP_H__65B7E19A_F952_4244_8979_74AB9669C358__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "windows.h"

#define OPERATION_NONE      -1
#define OPERATION_SMOOTH     0
#define OPERATION_REVERSE    1
#define OPERATION_TOGRAY     2
#define OPERATION_SHARPEN    3
#define OPERATION_LIGHTEN    4

typedef struct{
unsigned char r,g,b;}rgbData;
typedef struct{
int r,g,b;}rgbInt;

class CBMP  
{
public:
	CBMP();
	virtual ~CBMP();

	// 读取文件
	bool read_bmp ( const char* imageName );
	// 写文件
	bool write_bmp ( const char* imageName );
	// 图像处理类函数
	bool image_negative ();
	byte bound( float pixel , byte low , byte high );
	bool imageLighten( float multiple );
	bool imageToGray();
	bool iamgeSoomth( int *matrix );
	bool pixelNormalization( void* tmp );
	bool log_transform ();
	bool power_law_transform ();

	BITMAPFILEHEADER bf;
	BITMAPINFOHEADER bi;
	void* imageData;
	int imageSize;
	
};

#endif // !defined(AFX_BMP_H__65B7E19A_F952_4244_8979_74AB9669C358__INCLUDED_)
