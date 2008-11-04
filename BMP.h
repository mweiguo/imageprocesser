#pragma once

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
    virtual void  display () = 0;
};

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
    virtual void  display ();
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
    virtual void  display ();
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
    virtual void  display ();
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
    virtual void  display ();
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
    BFHEADER bf;                        // file header
    InfoHeaderWrapper *_pih;            // info header
    RGBA32* _colorTable;
    int _colorEntries;                  // color table
    void* imageData;                    // data section

};


void* changebitcnt_4to24 ( void* data, int32& size, RGBA32* _colorTable );
void* changebitcnt_16to24 ( void* data, int32& size );
void* changebitcnt_32to24 ( void* data, int32& size );

void* decode_rle8 ( void* data, int32 size, int32 width, int32 height );
void* decode_rle4 ( void* data, int32 size, int32& width, int32& height );

void dump8 (uint8* data, int32 width, int32 height );
void dump24 (RGB24* data, int32 width, int32 height );
