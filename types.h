#pragma once
#pragma pack(2)

// ---------- basic type 

typedef char           int8;
typedef unsigned char  uint8;
typedef short          int16;
typedef unsigned short uint16;
typedef long           int32;
typedef unsigned long  uint32;


// ---------- for basic image data type
template< typename T >
struct TRIPLE
{
    T x;
    T y;
    T z;
};

template< typename T >
struct QUAD
{
    T x;
    T y;
    T z;
    T a;
};

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


// ---------- inner image format

struct ImageHeader {
    uint32 width;
    uint32 height;
    uint16 bitCount;
};

struct Image {
    ImageHeader  _imageHeader;                // general image's properties
    RGBA32*      _colorTable;                 // color table entries
    int          _colorEntries;               // color table entry's count
    void*        imageData;                   // data section
};
