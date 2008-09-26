#include "stdafx.h"
#include "BMP.h"

// -------------------------------------------------------------------------------------------------

BMPProcesser::BMPProcesser()
{
    imageData = NULL;
}

// -------------------------------------------------------------------------------------------------

BMPProcesser::~BMPProcesser()
{
    if ( imageData!=NULL ){
	free( imageData );
	imageData = NULL;
    }
}

// -------------------------------------------------------------------------------------------------

void BMPProcesser::read_bmp( const char* imageName )
{
    LOG_INFO ("==========================================================");
    LOG_DEBUG ( "BMPProcesser::read_bmp imageName = %s", imageName );
    FILE * fp;
    if ( 0 == (fp = fopen( imageName,"rb" )) )
	throw std::logic_error("Cannot open this file" );

    fread( &bf, sizeof(uint8), sizeof(bf), fp );

    if ( bf.bfType != 0x4d42 ){
	fclose( fp );
	throw std::logic_error("this is not bitmap file" );
    }
    LOG_INFO ("bfSize = %d", bf.bfSize);

    fread( &bi,sizeof(uint8), sizeof(bi), fp );
    LOG_INFO ("biSize = %d", bi.biSize);
    switch ( bi.biSize ) {
    case sizeof(BICHEADER):
	LOG_INFO("BICHEADER");
	break;
    case sizeof(BIHEADER):
	LOG_INFO("BIHEADER");
	break;
    case sizeof(BIV4HEADER):
	LOG_INFO("BIV4HEADER");
	break;
    case sizeof(BIV5HEADER):
	LOG_INFO("BIV5HEADER");
	break;
    }

    imageSize = bi.biWidth * bi.biHeight * bi.biBitCount/8;
    if ( 0 == (imageData = malloc( imageSize ))) {
	fclose(fp);
	throw std::out_of_range ( "no more memory allocate for loading image" );
    }
    fread ( imageData, sizeof(uint8), imageSize, fp );
    fclose(fp);	

    LOG_INFO ("NAME : %s", imageName);
    LOG_INFO ("WIDTH x HEIGHT x BITCOUNT : %d x %d x %d", bi.biWidth, bi.biHeight, bi.biBitCount);
}

// -------------------------------------------------------------------------------------------------

void BMPProcesser::write_bmp( const char* imageName )
{
    LOG_DEBUG ( "BMPProcesser::write_bmp imageName = %s", imageName );
    FILE * fp;
    if ( 0 == (fp = fopen(imageName,"wb")) )
	throw std::logic_error("Cannot open this file" );

    LOG_INFO( "fwrite( %0xd, %d, %d, %0xd )", &bf, sizeof(uint8), sizeof(bf), fp );
    fwrite( &bf, sizeof(uint8), sizeof(bf), fp );
    LOG_INFO( "fwrite( %0xd, %d, %d, %0xd )", &bi, sizeof(uint8), sizeof(bi), fp );
    fwrite( &bi, sizeof(uint8), sizeof(bi), fp );
    LOG_INFO( "fwrite( %0xd, %d, %d, %0xd )", imageData, sizeof(uint8), imageSize, fp );
    fwrite( imageData, sizeof(uint8), imageSize, fp );
    fclose(fp);
    LOG_INFO( "closing file ok" );
}

// -------------------------------------------------------------------------------------------------

bool BMPProcesser::image_negative()
{	
    LOG_DEBUG ( "BMPProcesser::image_negative" );
    RGBDATA* end = (RGBDATA*)((uint8*)imageData + imageSize ), *p;
    for ( p = (RGBDATA*)imageData; p!=end; ++p ) {
	p->x = 255 - p->x;
	p->y = 255 - p->y;
	p->z = 255 - p->z;
    }
    return true;
}

// -------------------------------------------------------------------------------------------------

bool BMPProcesser::imageToGray()
{
    LOG_DEBUG ( "BMPProcesser::imageToGray" );
    float tmp = 0;
    RGBDATA* end = (RGBDATA*)((uint8*)imageData + imageSize ), *p;
    for ( p = (RGBDATA*)imageData; p!=end; ++p ) {
	tmp = (float)(p->x*0.299+p->y*0.587+p->z*0.114);
	p->x = (uint8)tmp;
	p->y = (uint8)tmp;
	p->z = (uint8)tmp;
    }
    return true;	
}

// -------------------------------------------------------------------------------------------------

bool BMPProcesser::pixelNormalization( void* tmp )//用于将计算过程中得到的RGB值转化为对应的0-255之间
{
    if ( tmp == NULL || imageData == NULL )
	return false;
    int minR, maxR,minG, maxG,minB, maxB;
    RGBINT* end = (RGBINT*)( (int*)tmp + imageSize*2 );
    RGBINT* p = (RGBINT*)tmp;
    minR = maxR = p->x;
    minG = maxG = p->y;
    minB = maxB = p->z;
    //以下用于遍历所有像素，寻找最小值和最大值
    for ( p ; p!=end; ++p ) {
	if ( p->x > maxR )
	    maxR = p->x;
	if ( p->x < minR )
	    minR = p->x;
	if ( p->y > maxG )
	    maxG = p->y;
	if ( p->y < minG )
	    minG = p->y;
	if ( p->z > maxB )
	    maxB = p->z;
	if ( p->z < minB )
	    minB = p->z;
    }
    //找到最值之后，计算其差，得到下面的计算系数
    int diffR,diffG,diffB;
    diffR = 255/(maxR - minR);
    diffG = 255/(maxG - minG);
    diffB = 255/(maxB - minB);
    RGBDATA* q = (RGBDATA*)imageData;
    //用于将每个像素值转换为 0-255之间
    for ( p = (RGBINT*)tmp,q; p!=end; ++p ,++q) {
	q->x = p->x * diffR*(p->x - minR);
	q->y = p->y * diffG*(p->y - minG);
	q->z = p->z * diffB*(p->z - minB);
    }
    return true;
}

// -------------------------------------------------------------------------------------------------

bool BMPProcesser::imageSoomth( int *matrix )
{
    LOG_DEBUG ( "BMPProcesser::imageSoomth" );
    if ( matrix == NULL )
	return false;
    int* pMid = matrix + 4;
    int sumr, sumg, sumb,denominator = 0;
    int idxImage=0,idxMatrix = 0;
    for ( int i = 0; i < 9; i++ )
	denominator += matrix[i];
    if ( denominator == 0 )
	return false;
    //申请控件，用来缓存中间的计算结果
    void* tmp;
    tmp = malloc( imageSize*2 );
    if ( tmp == NULL )
	return false;
    //将原来的像素值分别赋值给刚申请的地址
    RGBDATA* pEnd = (RGBDATA*)( (uint8*)imageData + imageSize );
    RGBDATA* p  = (RGBDATA*)imageData;
    RGBINT* q = (RGBINT*)tmp;
    for ( p,q; p != pEnd; ++p ,++q )	{
	q->x = p->x;
	q->y = p->y;
	q->z = p->z;
    }
    //进行中间的计算，将结果暂时存在新申请的整型空间里
    RGBINT* pCurrent  = (RGBINT*)tmp;
    q = (RGBINT*)tmp;
    for ( int j = 1; j < bi.biHeight-1; j++ ){
	for (int i = 1; i< bi.biWidth-1; i++ ){
	    pCurrent = q + (i +j*bi.biWidth);
			
	    sumr = 0; sumg = 0;sumb = 0;
	    for ( int n = -1;n<2;n++ ){
		for (int m = -1;m<2;m++){
		    idxMatrix = m+n*3;
		    idxImage = m+n*bi.biWidth;
		    sumr += (pCurrent+idxImage)->x*(*(pMid+idxMatrix));
		    sumg += (pCurrent+idxImage)->y*(*(pMid+idxMatrix));
		    sumb += (pCurrent+idxImage)->z*(*(pMid+idxMatrix));
		}
	    }
	    pCurrent->x = sumr/denominator;
	    pCurrent->y = sumg/denominator;
	    pCurrent->z = sumb/denominator;
	}
    }
    //对整型空间内数据进行归一处理，将结果保存到影响数据内
    if (!pixelNormalization( tmp ))
	return false;
    free( tmp );
    return true;	
}

// -------------------------------------------------------------------------------------------------

uint8 BMPProcesser::bound( float pixel , uint8 low , uint8 high )
{
    LOG_DEBUG ( "BMPProcesser::bound pixel=%f, low=%d, hight=%d", pixel, low, high );
    if ( pixel > high )
	pixel = high;
    else if( pixel < low )
	pixel = low;
    return (uint8)pixel;
}

// -------------------------------------------------------------------------------------------------

bool BMPProcesser::imageLighten( float multiple )
{
    LOG_DEBUG ( "BMPProcesser::imageLighten multiple=%f", multiple );
    RGBDATA* end = (RGBDATA*)((uint8*)imageData + imageSize ), *p;
    float r,g,b;
    for ( p = (RGBDATA*)imageData; p!=end; ++p ) {
	r = p->x * multiple;
	g = p->y * multiple;
	b = p->z * multiple;
	p->x = bound( r,0,255 );
	p->y = bound( g,0,255 );
	p->z = bound( b,0,255 );
    }
    return true;	
}

// -------------------------------------------------------------------------------------------------

void BMPProcesser::to_24bit ()
{
    LOG_INFO ( "BMPProcesser::to_24bit" );
    if ( 24 == bi.biBitCount )
	return;

    int32 newsize = imageSize;
    void* newdata = 0;
    switch ( bi.biBitCount ) {
    case 32:
	newdata = changebitcnt_32to24 ( imageData, newsize );
	bi.biBitCount = 24;
	bf.bfSize = bf.bfSize - imageSize + newsize;
	free(imageData);
	imageData = newdata;
	imageSize = newsize;
	break;
    case 16:
	newdata = changebitcnt_16to24 ( imageData, newsize );
	bi.biBitCount = 16;
	bf.bfSize = bf.bfSize - imageSize + newsize;
	free(imageData);
	imageData = newdata;
	imageSize = newsize;
	break;
    }
}

// -------------------------------------------------------------------------------------------------

void* changebitcnt_16to24 ( void* data, int32& size )
{
    R5G6B5* p = (R5G6B5*)data;
    R5G6B5* end = (R5G6B5*)((uint8*)data+size);
    RGB24* ptr = (RGB24*)malloc( size * 3 / 2 );
    for ( ; p!=end; ++p, ++ptr ) {
	ptr->x = (int8)(255 * p->r / 32.0);
	ptr->y = (int8)(255 * p->g / 64);
	ptr->z = (int8)(255 * p->b / 32);
    }
    size = ((int8*)ptr - (int8*)data);
    return (void*)ptr;
}

// -------------------------------------------------------------------------------------------------

void* changebitcnt_32to24 ( void* data, int32& size )
{
    LOG_INFO ("changebitcnt_32to24 data=%0xd, size=%d", data, size);
    RGBA32* p = (RGBA32*)data;
    RGBA32* end = (RGBA32*)((uint8*)data+size);

    RGB24* ptr = (RGB24*)malloc( size * 3 / 4 );
    RGB24* pbegin = ptr;
    for ( ; p!=end; ++p, ++ptr ) {
	ptr->x = p->x;
	ptr->y = p->y;
	ptr->z = p->z;
    }
    size = ((int8*)ptr - (int8*)pbegin);
    LOG_INFO("newdata = %0xd, newsize = %d", ptr, size);
    return (void*)ptr;
}

// -------------------------------------------------------------------------------------------------
