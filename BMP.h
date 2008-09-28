
#pragma once
#pragma pack(2)

typedef char           int8;
typedef unsigned char  uint8;
typedef short          int16;
typedef unsigned short uint16;
typedef long           int32;
typedef unsigned long  uint32;

enum OPERATION {
    OPERATION_NONE = 0,
    OPERATION_SMOOTH,
    OPERATION_REVERSE,
    OPERATION_TOGRAY,
    OPERATION_SHARPEN,
    OPERATION_LIGHTEN,
    OPERATION_TO24BIT
};
/* #define OPERATION_NONE      -1 */
/* #define OPERATION_SMOOTH     0 */
/* #define OPERATION_REVERSE    1 */
/* #define OPERATION_TOGRAY     2 */
/* #define OPERATION_SHARPEN    3 */
/* #define OPERATION_LIGHTEN    4 */


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

struct R5G6B5
{
uint16 r : 5;
uint16 g : 6;
uint16 b : 5;
};

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

class InfoHeaderWrapper
{
public:
    virtual void* headerptr () = 0;
    virtual int   headersize() = 0;
    virtual int   bitcount() = 0;
    virtual void  bitcount( int v ) = 0;
    virtual int   width() = 0;
    virtual int   height() = 0;
    virtual int   imagesize() = 0;
    virtual void  imagesize( int size ) = 0;
    virtual uint32 compression () = 0;
    virtual void compression ( uint32 v ) = 0;
    virtual int   clrused () = 0;
};

#define BI_RGB        0L
#define BI_RLE8       1L
#define BI_RLE4       2L
#define BI_BITFIELDS  3L
#define BI_JPEG       4L
#define BI_PNG        5L

class CIHeaderWrapper : public InfoHeaderWrapper
{
    BICHEADER bic;
public:
    virtual void* headerptr ()       	  { return &bic; }
    virtual int   headersize() 	     	  { return sizeof(BICHEADER); }
    virtual int   bitcount() 	     	  { return bic.bcBitCount; }
    virtual void  bitcount( int v )  	  { bic.bcBitCount = v; }
    virtual int   width()            	  { return bic.bcWidth; }
    virtual int   height()           	  { return bic.bcHeight; }
    virtual int   imagesize()        	  { return 0; }
    virtual void  imagesize( int size )   {}
    virtual uint32 compression ()         { return BI_RGB; }
    virtual void  compression ( uint32 v) {}
    virtual int   clrused ()              { return 0; }
};

class IHeaderWrapper : public InfoHeaderWrapper
{
    BIHEADER bi;
public:
    virtual void* headerptr ()       	  { return &bi; }
    virtual int   headersize() 	     	  { return sizeof(BIHEADER); }
    virtual int   bitcount() 	     	  { return bi.biBitCount; }
    virtual void  bitcount( int v )  	  { bi.biBitCount = v; }
    virtual int   width()            	  { return bi.biWidth; }
    virtual int   height()           	  { return bi.biHeight; }
    virtual int   imagesize()        	  { return bi.biSizeImage; }
    virtual void  imagesize( int size )   { bi.biSizeImage = size; }
    virtual uint32 compression ()         { return bi.biCompression; }
    virtual void compression ( uint32 v)  { bi.biCompression = v; }
    virtual int   clrused ()              { return bi.biClrUsed; }
};

class IV4HeaderWrapper : public InfoHeaderWrapper
{
    BIV4HEADER biv4;
public:
    virtual void* headerptr ()	     	  { return &biv4; }
    virtual int   headersize() 	     	  { return sizeof(BIV4HEADER); }
    virtual int   bitcount() 	     	  { return biv4.bV4BitCount; }
    virtual void  bitcount( int v )  	  { biv4.bV4BitCount = v; }
    virtual int   width()            	  { return biv4.bV4Width; }
    virtual int   height()           	  { return biv4.bV4Height; }
    virtual int   imagesize()        	  { return biv4.bV4SizeImage; }
    virtual void  imagesize( int size )   { biv4.bV4SizeImage = size; }
    virtual uint32 compression ()         { return biv4.bV4Compression; }
    virtual void compression ( uint32 v)  { biv4.bV4Compression = v; }
    virtual int   clrused ()              { return biv4.bV4ClrUsed; }
};

class IV5HeaderWrapper : public InfoHeaderWrapper
{
    BIV5HEADER biv5;
public:
    virtual void* headerptr () 	     	  { return &biv5; }
    virtual int   headersize() 	     	  { return sizeof(BIV5HEADER); }
    virtual int   bitcount() 	     	  { return biv5.bV5BitCount; }
    virtual void  bitcount( int v )  	  { biv5.bV5BitCount = v; }
    virtual int   width()            	  { return biv5.bV5Width; }
    virtual int   height()           	  { return biv5.bV5Height; }
    virtual int   imagesize()        	  { return biv5.bV5SizeImage; }
    virtual void  imagesize( int size )   { biv5.bV5SizeImage = size; }
    virtual uint32 compression ()         { return biv5.bV5Compression; }
    virtual void compression ( uint32 v)  { biv5.bV5Compression = v; }
    virtual int   clrused ()              { return biv5.bV5ClrUsed; }
};

class BMPProcesser  
{
public:
    BMPProcesser();
    virtual ~BMPProcesser();

    void read_bmp ( const char* imageName );
    void write_bmp ( const char* imageName );

    // 图像处理类函数
    bool image_negative ();
    uint8 bound( float pixel , uint8 low , uint8 high );
    bool imageLighten( float multiple );
    bool imageToGray();
    bool imageSoomth( int *matrix );
    bool pixelNormalization( void* tmp );
    bool log_transform ();
    bool power_law_transform ();
    void to_24bit ();
private:
    BFHEADER bf;
    InfoHeaderWrapper *_pih;
    void* imageData;
    RGBA32* _colorTable;
    int _colorEntries;

};

class CFileWrapper
{
    FILE* _file;
public:
    CFileWrapper ( const char* name, const char* flag ) {
	if ( 0 == (_file = fopen ( name, flag )) )
	    throw std::logic_error("Cannot open this file" );
    }
    ~CFileWrapper () {
	fclose ( _file );
    }
    FILE* handle() { return _file; }
};

class CMallocWrapper
{
    void* _data;
public:
    CMallocWrapper ( int size ) {
	if ( 0 == (_data = malloc ( size )) )
	    throw std::logic_error("out of memory" );
    }
    ~CMallocWrapper () {
	free ( _data );
    }
    void* handle() { return _data; }
};

void* changebitcnt_4to24 ( void* data, int32& size, RGBA32* _colorTable );
void* changebitcnt_16to24 ( void* data, int32& size );
void* changebitcnt_32to24 ( void* data, int32& size );

void* decode_rle8 ( void* data, int32 size, int32 width, int32 height );
void* decode_rle4 ( void* data, int32 size, int32 width, int32 height );
