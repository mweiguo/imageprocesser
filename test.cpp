#include "stdafx.h"
#include "bmp.h"
#include <stdio.h>
#include <string.h>

/** appname -i bmpInFile -o bmpOutFile -c "smooth | sharpen | reverse | togray |..."
 */
int main( int argc, const char* argv[] )
{
    float lightenNum = 0;
    LOG_LEVEL ( tinyLog::debug );
    LOG_FORMAT ( "" );
    LOG_SHOWTITLE ( true );
    // 从应用程序取文件的路径和所需要进行的操作
    if ( argc < 6) {
        LOG_ERROR ( "usage: \n%s -i bmpInFile -o bmpOutFile -c 'ligthen | smooth | sharpen | reverse | togray | ...' -p 'lightenPara'", argv[0] );
        return 1;
    }

    // get infile path, outfile path and operation
    char infile[256], outfile[256];
    int operation = OPERATION_NONE;
    for ( int i=1; i<argc; i+=2 ) {
        if ( strcmp ( argv[i], "-i" )==0 ) {
            strcpy ( infile, argv[i+1] );
        } else if ( strcmp ( argv[i], "-o" )==0 ) {
            strcpy ( outfile, argv[i+1] );
        } else if ( strcmp ( argv[i], "-c" ) == 0 ) {
            if ( strcmp ( "smooth", argv[i+1] )==0 ) {
                operation = OPERATION_SMOOTH;
            } else if ( strcmp ( "reverse", argv[i+1] )==0 ) {
                operation = OPERATION_REVERSE;
            } else if ( strcmp ( "togray", argv[i+1] )==0 ) {
                operation = OPERATION_TOGRAY;
            }else if ( strcmp ( "sharpen", argv[i+1])==0 ) {
                operation = OPERATION_SHARPEN;
            }else if ( strcmp( "lighten",argv[i+1]) == 0 ){
                operation = OPERATION_LIGHTEN;  
            }else if ( strcmp( "to24bits",argv[i+1]) == 0 ){
                operation = OPERATION_TO24BIT;  
            }else {
                printf("%s is invalid operation", argv[i+1] );
            }
        }else if ( strcmp( argv[i],"-p" )==0 ){
            lightenNum = atof(argv[i+1]);
        }else {
            printf( "should use like follow: %s -i bmpInFile -o bmpOutFile -c 'lighten | smooth | sharpen | reverse | togray | ...'", argv[0] );
            return 1;
        }
    }

    
    try {
        BMPProcesser bmp ;
	LOG_INFO("");
        bmp.read_bmp( infile );

        int matrixSharpen[9] = {-1,-2,-1,-2,19,-2,-1,-2,-1};
        int matrixSmooth[9] = {1,2,1,2,4,2,1,2,1};
        switch ( operation ) {
        case OPERATION_SMOOTH:          bmp.imageSoomth(matrixSmooth);     break;
        case OPERATION_REVERSE:         bmp.image_negative();              break;
        case OPERATION_TOGRAY:          bmp.imageToGray();                 break;
        case OPERATION_SHARPEN:         bmp.imageSoomth(matrixSharpen);    break;
        case OPERATION_LIGHTEN:         bmp.imageLighten( lightenNum );    break;
        case OPERATION_TO24BIT:         bmp.to_24bit();                    break;
        }

        bmp.write_bmp( outfile );
    } catch ( std::exception& e ) {
        LOG_ERROR ( e.what() );
    }
    return 0;
}
