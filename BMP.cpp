#include "stdafx.h"
#include "BMP.h"

// -------------------------------------------------------------------------------------------------

BMPProcesser::BMPProcesser()
{
    imageData = NULL;
    _colorTable = 0;
    _colorEntries = 0;
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
    LOG_DEBUG ( "BMPProcesser::read_bmp imageName = %s", imageName );
    CFileWrapper file ( imageName, "rb" );

    // read file header
    fread( &bf, sizeof(uint8), sizeof(bf), file.handle() );
    if ( bf.bfType != 0x4d42 )
        throw std::logic_error("this is not bitmap file" );

    // read info header
    uint32 size = 0;
    fread( &size, sizeof(uint8), sizeof(uint32), file.handle() );
    fseek ( file.handle(), -sizeof(uint32), SEEK_CUR );
    
    switch ( size ) {
    case sizeof(BICHEADER):     _pih = new CIHeaderWrapper();    break;
    case sizeof(BIHEADER):      _pih = new IHeaderWrapper();     break;
    case sizeof(BIV4HEADER):    _pih = new IV4HeaderWrapper();   break;
    case sizeof(BIV5HEADER):    _pih = new IV5HeaderWrapper();   break;
    default: throw std::logic_error("unknown bmp header");
    }
    fread( _pih->headerptr(), sizeof(uint8), _pih->headersize(), file.handle() );
    if ( _pih->imagesize() == 0 && _pih->compression() == BI_RGB )
        _pih->imagesize ( int(_pih->width() * _pih->height() * _pih->bitcount() / 8.0) );

    // read color table
    _colorEntries = 0;
    switch ( _pih->bitcount() ) {
    case 8:  // there most have 256 entries
        _colorEntries = _pih->clrused() == 0 ? 256 : _pih->clrused();
        break;
    case 4:  // there most have 16 entries
        _colorEntries = _pih->clrused() == 0 ? 16 : _pih->clrused();
        break;
    case 1:
        _colorEntries = 2;
        break;
    }
    _colorTable = (RGBA32*)malloc( _colorEntries * sizeof(RGBA32) );
    fread( _colorTable, sizeof(uint8), _colorEntries * sizeof(RGBA32), file.handle() );

    if ( 0 == (imageData = malloc( _pih->imagesize() )))
        throw std::out_of_range ( "no more memory allocate for loading image" );

    // read image data
    fread ( imageData, sizeof(uint8), _pih->imagesize(), file.handle() );
    
    // decode image data if needed
    void* newdata;
    switch ( _pih->compression() ) {
    case BI_RLE8:
        newdata = decode_rle8 ( imageData, _pih->imagesize(), _pih->width(), _pih->height() );
        if ( 0 != newdata ) {
            _pih->imagesize ( _pih->width() * _pih->height() );
            free ( imageData );
            imageData = newdata;
            _pih->compression ( BI_RGB );
        }
        break;
    case BI_RLE4:
        newdata = decode_rle4 ( imageData, _pih->imagesize(), _pih->width(), _pih->height() );
        if ( 0 != newdata ) {
            _pih->imagesize ( _pih->width() * _pih->height() );
            free ( imageData );
            imageData = newdata;
            _pih->compression ( BI_RGB );
        }
        break;
    case BI_RGB:
        break;
    case BI_BITFIELDS:
        throw std::logic_error ("dont not support compression method BI_BITFIELDS");
    case BI_JPEG:
        throw std::logic_error ("dont not support compression method BI_JPEG");
    case BI_PNG:
        throw std::logic_error ("dont not support compression method BI_PNG");
    default:
        throw std::logic_error ("unknown compression method");
    }

    LOG_INFO ("NAME : %s", imageName);
    LOG_INFO ("WIDTH x HEIGHT x BITCOUNT x IMAGESIZE : %d x %d x %d x %d", _pih->width(), _pih->height(), _pih->bitcount(), _pih->imagesize() );
}

// -------------------------------------------------------------------------------------------------

void BMPProcesser::write_bmp( const char* imageName )
{
    LOG_DEBUG ( "BMPProcesser::write_bmp imageName = %s", imageName );
    CFileWrapper file ( imageName, "wb");

    fwrite( &bf, sizeof(uint8), sizeof(bf), file.handle() );
    fwrite ( _pih->headerptr(), sizeof(uint8), _pih->headersize(), file.handle() );
    if ( _colorTable )
        fwrite ( _colorTable, sizeof(uint8), _colorEntries * sizeof(RGBA32), file.handle() );
    fwrite( imageData, sizeof(uint8), _pih->imagesize(), file.handle() );
    LOG_DEBUG( "closing file ok" );
}

// -------------------------------------------------------------------------------------------------

bool BMPProcesser::image_negative()
{       
    LOG_DEBUG ( "BMPProcesser::image_negative" );
    RGBDATA* end = (RGBDATA*)((uint8*)imageData + _pih->imagesize() ), *p;
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
    RGBDATA* end = (RGBDATA*)((uint8*)imageData + _pih->imagesize() ), *p;
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
    RGBINT* end = (RGBINT*)( (int*)tmp + _pih->imagesize()*2 );
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
    tmp = malloc( _pih->imagesize()*2 );
    if ( tmp == NULL )
        return false;
    //将原来的像素值分别赋值给刚申请的地址
    RGBDATA* pEnd = (RGBDATA*)( (uint8*)imageData + _pih->imagesize() );
    RGBDATA* p  = (RGBDATA*)imageData;
    RGBINT* q = (RGBINT*)tmp;
    for ( p,q; p != pEnd; ++p ,++q )    {
        q->x = p->x;
        q->y = p->y;
        q->z = p->z;
    }
    //进行中间的计算，将结果暂时存在新申请的整型空间里
    RGBINT* pCurrent  = (RGBINT*)tmp;
    q = (RGBINT*)tmp;
    for ( int j = 1; j < _pih->height()-1; j++ ){
        for (int i = 1; i< _pih->width()-1; i++ ){
            pCurrent = q + (i + j * _pih->width());
                        
            sumr = 0; sumg = 0;sumb = 0;
            for ( int n = -1;n<2;n++ ){
                for (int m = -1;m<2;m++){
                    idxMatrix = m+n*3;
                    idxImage = m + n * _pih->width();
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
    free( tmp );ptr
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
    RGBDATA* end = (RGBDATA*)((uint8*)imageData + _pih->imagesize() ), *p;
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
    LOG_DEBUG ( "BMPProcesser::to_24bit" );
    if ( 24 == _pih->bitcount() )
        return;

    int32 newsize = _pih->imagesize();
    void* newdata = 0;

    switch ( _pih->bitcount() ) {
    case 32:
        newdata = changebitcnt_32to24 ( imageData, newsize );
        _pih->bitcount( 24 );
        bf.bfSize = bf.bfSize - _pih->imagesize() + newsize;
        free(imageData);
        imageData = newdata;
        _pih->imagesize( newsize );
        break;
    case 16:
        newdata = changebitcnt_16to24 ( imageData, newsize );
        _pih->bitcount( 24 );
        bf.bfSize = bf.bfSize - _pih->imagesize() + newsize;
        free(imageData);
        imageData = newdata;
        _pih->imagesize( newsize );
        break;
    case 8:
	break;
    case 4:
        newdata = changebitcnt_4to24 ( imageData, newsize, _colorTable );
        _pih->bitcount( 24 );
        bf.bfSize = bf.bfSize - _pih->imagesize() + newsize;
        free(imageData);
        imageData = newdata;
        _pih->imagesize( newsize );
        break;
	break;
    }
}

// -------------------------------------------------------------------------------------------------

void* changebitcnt_4to24 ( void* data, int32& size, RGBA32* colorTable )
{
    LOG_DEBUG ("changebitcnt_4to24 data=%0x, size=%d, colorTable=%0x", data, size, colorTable);
    uint8* p = (uint8*)data;
    uint8* end = (uint8*)((uint8*)data+size);
    RGB24* ptr = (RGB24*)malloc( size * 3 );
    RGB24* pbegin = ptr;
    
    for ( ; p!=end; ++p, ++ptr ) {
        ptr->x = colorTable[*p].x;
        ptr->y = colorTable[*p].y;
        ptr->z = colorTable[*p].z;
    }
    size = ((int8*)ptr - (int8*)pbegin);
    return (void*)pbegin;
}

// -------------------------------------------------------------------------------------------------

void* changebitcnt_16to24 ( void* data, int32& size )
{
    R5G6B5* p = (R5G6B5*)data;
    R5G6B5* end = (R5G6B5*)((uint8*)data+size);
    RGB24* ptr = (RGB24*)malloc( size * 3 / 2 );
    RGB24* pbegin = ptr;

    for ( ; p!=end; ++p, ++ptr ) {
        ptr->x = (int8)(255 * p->r / 32.0);
        ptr->y = (int8)(255 * p->g / 64);
        ptr->z = (int8)(255 * p->b / 32);
    }
    size = ((int8*)ptr - (int8*)pbegin);
    return (void*)pbegin;
}

// -------------------------------------------------------------------------------------------------

void* changebitcnt_32to24 ( void* data, int32& size )
{
    LOG_DEBUG ("changebitcnt_32to24 data=%0xd, size=%d", data, size);
    RGBA32* p = (RGBA32*)data;
    RGBA32* end = (RGBA32*)((uint8*)data+size);

    size = size * 3 / 4;
    RGB24* ptr = (RGB24*)malloc( size );
    RGB24* pbegin = ptr;

    for ( int i=0; p != end; i++, ++p, ++ptr ) {
        ptr->x = p->x;
        ptr->y = p->y;
        ptr->z = p->z;
    }
        
    LOG_DEBUG("newdata = %0xd, newsize = %d", ptr, size);
    return (void*)pbegin;
}

// -------------------------------------------------------------------------------------------------

void* decode_rle8 ( void* data, int32 size, int32 width, int32 height )
{
    LOG_DEBUG ( "decompress_rle8 data=%0xd, size=%d, width=%d, height=%d", data, size, width, height );
    uint8* p = (uint8*)data;
    uint8* end = (uint8*)data + size;

    uint8* dst = (uint8*)malloc ( width * height );
    uint8* pbegin = dst;

    for ( ; p!=end; ++p ) {
        uint8 cnt = *p;
        uint8 next = *++p;

        if ( *(p-1) != 0 ) {  // normal
            for ( int i=0; i<cnt; i++ )
                *++dst = next;
        } else {        // escape
            switch ( next ) {
            case 0:     // end of file, check column number
                if ( (dst - pbegin) % width )
                    throw std::logic_error ( "column number incorrect" );
                break;
            case 1:     // end of bitmap
                return pbegin;
            case 2:     // delta, must be align on word boundary
                cnt = *(p + 2) * width + *(p + 1);
                p += 2;
                for ( int i=0; i<cnt; i++ )
                    *++dst = 0;
                break; 
            default:    // absolute mode, must be align on word boundary
                cnt = next % 2 ? next + 1 : next;
                for ( int i=0; i<cnt; i++ )
                    *++dst = *++p;
                break;
            }
        }
    }
    return 0;
}

// -------------------------------------------------------------------------------------------------

void* decode_rle4 ( void* data, int32 size, int32 width, int32 height )
{
    LOG_DEBUG ( "decompress_rle4 data=%0xd, size=%d, width=%d, height=%d", data, size, width, height );
    uint8* p = (uint8*)data;
    uint8* end = (uint8*)data + size;

    uint8* dst = (uint8*)malloc ( width * height );
    uint8* pbegin = dst;

    uint8* pdata = (uint8*)data;

    for ( ; p!=end; ++p ) {
        uint8 cnt = *p;
        uint8 next = *++p;

        // normal
        if ( *(p-1) != 0 ) { 
            for ( int i=0; i<cnt; i++ ) {
                *++dst = next & 0xF0;
                if ( ++i < cnt )
                    *++dst = next & 0x0F;
            }
        } else {   // escape
            switch ( next ) {
            case 0:     // end of line, check column number
                if ( (dst - pbegin) % width ) {
		    char tmp[512];
		    sprintf ( tmp, "(%0x, %0x, %0x, %0x, %0x, %0x, %0x), column number incorrect, offset from image data begin = %x,  (%0x, %0x, %0x, %0x, %0x, %0x, %0x)", *pdata, *(pdata+1), *(pdata+2), *(pdata+3), *(pdata+4), *(pdata+5), *(pdata+6), p - (uint8*)data, *(p-3), *(p-2), *(p-1), *p, *(p+1), *(p+2), *(p+3) );
                    throw std::logic_error ( tmp );
                }
                break;
            case 1:     // end of bitmap
                return pbegin;
            case 2:     // delta, must be align on word boundary
                cnt = *(p + 2) * width + *(p + 1);
                p += 2;
                for ( int i=0; i<cnt; i++ ) {
                    *++dst = 0;
                    *++dst = 0;
                }
                break; 
            default:    // absolute mode, must be align on word boundary
		for ( int i=0; i<next; i++ ) {
                    *++dst = *(p+1+i) & 0xF0;
		    if ( ++i == next ) break;
                    *++dst = *(p+i) & 0x0F;
		}
		
		cnt = (int)(next / 4) + (next % 4 != 0 ? 1 : 0);
		cnt *= 2;
		p += cnt;
                break;
            }
        }
    }
    return 0;
}

// -------------------------------------------------------------------------------------------------

void encode_rle8 ()
{
}
    
// -------------------------------------------------------------------------------------------------

void encode_rle4 ()
{
}
