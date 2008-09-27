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

int PrintInfo (char *args [])
{
	printf ("---------------------------------------------------\n");
	printf ("CLiP - Command Line Parser v1.2\n");
	printf ("---------------------------------------------------\n\n");
	
	printf ("-h  --help                       Displays this help\n");
	printf ("    --name [your_name,other]     Links 2 names\n");
	printf ("-d         [a_name]              Outputs a_name\n\n");
	
	printf ("---------------------------------------------------\n");
	printf ("CLiP is free and licensed under the GNU General\n");
	printf ("Public License; therefore you are free to use, edit\n");
	printf ("and redistribute the source code.\n");
	printf ("Homepage is at http://giulio.hewle.com/pf/clip\n\n");
	
	printf ("This software is platform independent and developed\n");
	printf ("in C by Giulio Bai <giulio@hewle.com>\n\n");
	
	printf ("Feel absolutly free to edit the code and submit your\n");
	printf ("changes: any help is kindly appreciated!\n\n");
	printf ("More info at http://giulio.hewle.com/pf/cclip\n");
	printf ("---------------------------------------------------\n\n");
	
	return 0;
}

int Smooth ( char* args[] )
{
    int matrixSmooth[9] = {1,2,1,2,4,2,1,2,1};
    int matrixSharpen[9] = {-1,-2,-1,-2,19,-2,-1,-2,-1};
    try {
	BMPProcesser bmp ;
	bmp.read_bmp( infile );
	bmp.imageSoomth(matrixSmooth);
	bmp.write_bmp( outfile );
    } catch ( exception& e ) {
	LOG_ERROR ( e.what() );
    }
}

int Negative ( char* args[] )
{
    try {
	BMPProcesser bmp ;
	bmp.read_bmp( infile );
	bmp.image_negative();
	bmp.write_bmp( outfile );
    } catch ( exception& e ) {
	LOG_ERROR ( e.what() );
    }
}

int ToGray ( char* args[] )
{
    try {
	BMPProcesser bmp ;
	bmp.read_bmp( infile );
	bmp.imageToGray();
	bmp.write_bmp( outfile );
    } catch ( exception& e ) {
	LOG_ERROR ( e.what() );
    }
}


int Lighten ( char* args[] )
{
    try {
	BMPProcesser bmp ;
	bmp.read_bmp( infile );
	bmp.imageLighten( lightenNum );
	bmp.write_bmp( outfile );
    } catch ( exception& e ) {
	LOG_ERROR ( e.what() );
    }
}


int To24Bit ( char* args[] )
{
    try {
	BMPProcesser bmp ;
	bmp.read_bmp( infile );
	bmp.to_24bit();
	bmp.write_bmp( outfile );
    } catch ( exception& e ) {
	LOG_ERROR ( e.what() );
    }
}

