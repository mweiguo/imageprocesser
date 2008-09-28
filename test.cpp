#include "stdafx.h"
#include "bmp.h"
#include <stdio.h>
#include <string.h>
#include "clip.h"

using namespace std;

int main( int argc, char* argv[] )
{
    LOG_LEVEL ( tinyLog::debug );
    LOG_FORMAT ( "" );
    LOG_SHOWTITLE ( true );

    try {
	CLiP clip (argc, argv);
    } catch ( exception& e ) {
	LOG_ERROR ( e.what() );
    }

    return 0;
}
