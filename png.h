#pragma once


class PngWrapper
{
    png_structp _png_ptr;
    png_infop   _info_ptr
public:
    PngWrapper ( char* path );
    ~PngWrapper ();
    void ToIIF ( ImageWrapper& img );
    bool FromIIF ( ImageWrapper& img );
};
