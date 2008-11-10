#pragma once

// ---------- inner image format

struct ImageHeader {
    uint32 width;
    uint32 height;
    uint16 bitCount;
};

class ImageWrapper {
    ImageHeader  _imageHeader;                // general image's properties
    RGB24*       _colorTable;                 // color table entries
    int          _colorEntries;               // color table entry's count
    RGBA32*      imageData;                   // data section
    int32        dataSize;
public:
    // inner image Accesse Interface
    void header( uint32& width, uint32& height, uint16& bitCount );
    void palette( RGB24** colorTable, int32& numEntries );
    void data( RGBA32** data, int32& dataSize );
    void header( uint32 width, uint32 height, uint16 bitCount );
    void palette( RGB24* colorTable, int32 numEntries );
    void data( RGBA32* data, int32 dataSize );
};
