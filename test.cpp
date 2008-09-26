#include <stdio.h>
#include <string.h>

//那就写一个函数输出一堆x,y就可以了，显示的模块后边在想怎么做
#define OPERATION_NONE      -1
#define OPERATION_SMOOTH    0
#define OPERATION_REVERSE   1

/** appname -in bmpInFile -out bmpOutFile -operation "smooth | reverse
 | ..."
 */
int main ( int argc, const char* argv[] )
{

    // 从应用程序取文件的路径和所需要进行的操作
    if ( argc != 7 ) {
	printf ( "should use like follow: %s -in bmpInFile -out bmpOutFile
 -operation 'smooth | reverse | ...'", argv[0] );
	return 1;
    }

    // get infile path, outfile path and operation
    char infile[256], outfile[256];
    int operation = OPERATION_NONE;
    for ( int i=1; i<argc; i+=2 ) {
	if ( strcmp ( argv[i], "-in" )==0 ) {
	    strcpy ( infile, argv[i+1] );
	} else if ( strcmp ( argv[i], "-out" )==0 ) {
	    strcpy ( outfile, argv[i+1] );
	} else if ( strcmp ( argv[i], "-operation" ) == 0 ) {
	    if ( strcmp ( "smooth", argv[i+1] )==0 ) {
		operation = OPERATION_SMOOTH;
	    } else if ( strcmp ( "reverse", argv[i+1] )==0 ) {
		operation = OPERATION_REVERSE;
	    } else {
		printf ("%s is invalid operation", argv[i+1] );
	    }
	} else {
	    printf ( "should use like follow: %s -in bmpInFile -out bmpOutFile
 -operation 'smooth | reverse | ...'", argv[0] );
	    return 1;
	}
    }

    // 在这里调用你的操作
    switch ( operation ) {
    case OPERATION_SMOOTH:
	printf ("smooth operation here");
	break;
    case OPERATION_REVERSE:
	printf ("reverse operation here");
	break;
    }
    return 0;
}