#pragma once
#pragma pack(2)

// ------------------------------------------------ basic type 
typedef char           int8;
typedef unsigned char  uint8;
typedef short          int16;
typedef unsigned short uint16;
typedef long           int32;
typedef unsigned long  uint32;


template< typename T >
struct TRIPLE
{
    T x, y, z;
};

template< typename T >
struct QUAD
{
    T x, y, z, a;
};

typedef TRIPLE<unsigned char> RGBDATA;
typedef TRIPLE<int> RGBINT;
typedef QUAD<int8>  RGBA32;
typedef TRIPLE<int8> RGB24;
typedef TRIPLE<int32> CIEXYZ;
typedef TRIPLE<CIEXYZ> CIEXYZTRIPLE;

struct R5G6B5 {
    uint16 r : 5;
    uint16 g : 6;
    uint16 b : 5;
};

typedef std::vector<RGB24> IMAGEDATA;


// ------------------------------------------------ kinds of bitmap info type 

struct BFHEADER
{
    uint16 bfType;
    uint32 bfSize;
    uint16 bfReserved1;
    uint16 bfReserved2;
    uint32 bfOffBits;
};

struct BICHEADER {
    uint32    bcSize; 
    uint16    bcWidth; 
    uint16    bcHeight; 
    uint16    bcPlanes; 
    uint16    bcBitCount; 
};

struct BIHEADER
{
    uint32 biSize;
    uint32 biWidth;
    uint32 biHeight;
    uint16 biPlanes;
    uint16 biBitCount;
    uint32 biCompression;
    uint32 biSizeImage;
    uint32 biXPelsPerMeter;
    uint32 biYPelsPerMeter;
    uint32 biClrUsed;
    uint32 biClrImportant;
};

struct BIV4HEADER
{
    uint32        bV4Size;
    int32         bV4Width;
    int32         bV4Height;
    uint16        bV4Planes;
    uint16        bV4BitCount;
    uint32        bV4Compression;
    uint32        bV4SizeImage;
    int32         bV4XPelsPerMeter;
    int32         bV4YPelsPerMeter;
    uint32        bV4ClrUsed;
    uint32        bV4ClrImportant;
    uint32        bV4RedMask;
    uint32        bV4GreenMask;
    uint32        bV4BlueMask;
    uint32        bV4AlphaMask;
    uint32        bV4CSType;
    CIEXYZTRIPLE  bV4Endpoints;
    uint32        bV4GammaRed;
    uint32        bV4GammaGreen;
    uint32        bV4GammaBlue;
};

struct BIV5HEADER
{ 
    uint32        bV5Size; 
    int32         bV5Width; 
    int32         bV5Height; 
    uint16        bV5Planes; 
    uint16        bV5BitCount; 
    uint32        bV5Compression; 
    uint32        bV5SizeImage; 
    int32         bV5XPelsPerMeter; 
    int32         bV5YPelsPerMeter; 
    uint32        bV5ClrUsed; 
    uint32        bV5ClrImportant; 
    uint32        bV5RedMask; 
    uint32        bV5GreenMask; 
    uint32        bV5BlueMask; 
    uint32        bV5AlphaMask; 
    uint32        bV5CSType; 
    CIEXYZTRIPLE  bV5Endpoints; 
    uint32        bV5GammaRed; 
    uint32        bV5GammaGreen; 
    uint32        bV5GammaBlue; 
    uint32        bV5Intent; 
    uint32        bV5ProfileData; 
    uint32        bV5ProfileSize; 
    uint32        bV5Reserved; 
};

#define BI_RGB        0L
#define BI_RLE8       1L
#define BI_RLE4       2L
#define BI_BITFIELDS  3L
#define BI_JPEG       4L
#define BI_PNG        5L

