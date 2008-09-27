/*
 * Copyright (C)2007 Giulio Bai
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


/*
 * Put the functions you like in this file, included in 'clip.c'
 *
 * NOTE the only parameter must be 'char *your_name[]' to contain
 * the array of arguments an option has.
 *
 * To use them in your methods just type 'your_name[0]' for the first, etc
 */

// ------------------------------------------------------------------------

int PrintInfo (char *args [])
{
    printf ("---------------------------------------------------\n");
    printf ("im - Image Processer v0.1\n");
    printf ("---------------------------------------------------\n\n");
	
    printf ("-h  --help                                         Displays this help\n");
    printf ("-smooth inputImage outputImage bSmooth     	    smooth or sharpen image\n");
    printf ("-negative inputImage outputImage           	    negative image\n");
    printf ("-togray inputImage outputImage             	    change image to gray\n");
    printf ("-lighten inputImage outputImage intensity  	    lighten image\n");
    printf ("-to24bit inputImage outputImage            	    change bitcount to 24bit\n");
	
    return 0;
}

// ------------------------------------------------------------------------

int Smooth ( char* args[] )
{
    bool bSmooth = atoi(args[2]);
    int matrixSmooth[9] = {1,2,1,2,4,2,1,2,1};
    int matrixSharpen[9] = {-1,-2,-1,-2,19,-2,-1,-2,-1};
    try {
	BMPProcesser bmp ;
	bmp.read_bmp( args[0] );
	if ( bSmooth )	bmp.imageSoomth ( matrixSmooth );
	else            bmp.imageSoomth ( matrixSharpen );
	bmp.write_bmp( args[1] );
    } catch ( exception& e ) {
	LOG_ERROR ( e.what() );
    }
}

// ------------------------------------------------------------------------

int Negative ( char* args[] )
{
    try {
	BMPProcesser bmp ;
	bmp.read_bmp( args[0] );
	bmp.image_negative();
	bmp.write_bmp( args[1] );
    } catch ( exception& e ) {
	LOG_ERROR ( e.what() );
    }
}

// ------------------------------------------------------------------------

int ToGray ( char* args[] )
{
    try {
	BMPProcesser bmp ;
	bmp.read_bmp( args[0] );
	bmp.imageToGray();
	bmp.write_bmp( args[1] );
    } catch ( exception& e ) {
	LOG_ERROR ( e.what() );
    }
}

// ------------------------------------------------------------------------

int Lighten ( char* args[] )
{
    float intensity = atof ( args[2] );
    try {
	BMPProcesser bmp ;
	bmp.read_bmp( args[0] );
	bmp.imageLighten( intensity );
	bmp.write_bmp( args[1] );
    } catch ( exception& e ) {
	LOG_ERROR ( e.what() );
    }
}

// ------------------------------------------------------------------------

int To24Bit ( char* args[] )
{
    try {
	BMPProcesser bmp ;
	bmp.read_bmp( args[0] );
	bmp.to_24bit();
	bmp.write_bmp( args[1] );
    } catch ( exception& e ) {
	LOG_ERROR ( e.what() );
    }
}

// ------------------------------------------------------------------------

