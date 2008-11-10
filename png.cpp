#include "stdafx.h"

// -------------------------------------------------------------

PngWrapper::PngWrapper ( char* path ) {
    CFileWrapper f = CFileWrapper( path, flag );
    uint8 header[8] = {0};
    fread ( header, sizeof(header), 1, f.handle() );
    if ( 0 != png_sig_cmp ( header, 0, sizeof(header) ) )
	throw std::invalid_argument ("invalid png format");

    if (( (_png_ptr = png_create_read_struct ( PNG_LIBPNG_VER_STRING, 0, 0, 0)) == 0 ) ||
	( (_info_ptr = png_create_info_struct ( png_ptr )) == 0  ))
	throw std::logic_error ("can not init reading png process ");

    png_init_io ( _png_ptr, f.handle() );
    png_set_sig_bytes ( _png_ptr, sizeof(header) );

    // read info
    png_read_png ( png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, 0 );
}
    
// -------------------------------------------------------------

PngWrapper::~PngWrapper (){
    png_destroy_read_struct ( &_png_ptr, &_info_ptr, 0 );
}
    
// -------------------------------------------------------------

void PngWrapper::ToIIF ( ImageWrapper& img ) {
    // get parameters
    uint32 width, height;
    int bitcnt, clrtype, interlace_method, compress_method, filter_method, rc;
    if ( (rc = png_get_IHDR ( _png_ptr, _info_ptr, &width, &height, &bitcnt, &clrtype, &interlace_method, &compress_method, &filter_method )) == 0 ) 
	throw std::logic_error ("");

    png_colorp palette;
    int numPalette;
    if ( (rc = png_get_PLTE ( _png_ptr, _info_ptr, &palette, &numPalette )) == 0 )
	numPalette = 0;

    uint8** pData = png_get_rows ( png_ptr, _info_ptr );
	
    // set iif
    img.header ( width, height, bitcnt );
    if ( numPalette != 0 )
	img.palette ( (RGB24*)palette, numPalette );

    img.data ( pData, (int)(weight * height * bitcnt / 8.0) );
}
    
// -------------------------------------------------------------

bool PngWrapper::FromIIF ( ImageWrapper& img ) {
    uint32 width, height;
    uint16 bitcnt;
    img.header ( width, height, bitcnt );
    png_set_IHDR ( _png_ptr, _info_ptr, width, height, bitcnt); 

    RGB24* colorTable;
    img.palette ( _png_ptr, _info_ptr, &colorTable, numEntries );

    uint8* data;
    img.data ( &data, dataSize );
	
    png_set_rows ( _png_ptr, _info_ptr, data );
}
