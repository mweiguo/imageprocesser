// BMP.cpp: implementation of the CBMP class.
//
//////////////////////////////////////////////////////////////////////

#include "BMP.h"
// Value = (c.G*59+c.R*30+c.B*11)/100
//Y=(float)(Red*0.299+Green*0.587+Blue*0.114)
//char  [-127, 127] unsigned char [0,255]
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBMP::CBMP()
{
	imageData = NULL;
}

CBMP::~CBMP()
{
	if ( imageData!=NULL ){
		free( imageData );
		imageData = NULL;
	}
}

bool CBMP::read_bmp( const char* imageName )
{
	FILE * fp;
	fp = fopen( imageName,"rb" );
	if ( NULL == fp ){
		printf( "Cannot open this file!\n" );
		return false;
	}
	fread( &bf,sizeof(unsigned char),14,fp );
	if ( bf.bfType != 0x4d42 ){
		fclose( fp );
		return false;
	}
	fread( &bi,sizeof(unsigned char),40,fp );

	if ( bi.biBitCount != 24 ){
		fclose(fp);
		return false;
	}
	imageSize = bi.biWidth*bi.biHeight*3;
	imageData = malloc( imageSize );
	if ( imageData == NULL ){
		printf("excess!");
		fclose(fp);
		return false;
	}
	fread( imageData,sizeof(unsigned char),imageSize,fp );
	fclose(fp);	
	return true;
}
bool CBMP::write_bmp( const char* imageName )
{
	FILE * fp;
	fp = fopen(imageName,"wb");
	if ( NULL==fp ){
		printf( "Cannot open this file!\n" );
		return false;
	}
	fwrite( &bf,sizeof(unsigned char),14,fp );
	fwrite( &bi,sizeof(unsigned char),40,fp );
	fwrite( imageData,sizeof(unsigned char),imageSize,fp );
	fclose(fp);
	return true;
}

bool CBMP::image_negative()
{	
    rgbData* end = (rgbData*)((unsigned char*)imageData + imageSize ), *p;
    for ( p = (rgbData*)imageData; p!=end; ++p ) {
		p->r = 255 - p->r;
		p->g = 255 - p->g;
		p->b = 255 - p->b;
    }
	return true;
}

bool CBMP::imageToGray()
{
	float tmp = 0;
    rgbData* end = (rgbData*)((unsigned char*)imageData + imageSize ), *p;
    for ( p = (rgbData*)imageData; p!=end; ++p ) {
		tmp = (float)(p->r*0.299+p->g*0.587+p->b*0.114);
		p->r = (unsigned char)tmp;
		p->g = (unsigned char)tmp;
		p->b = (unsigned char)tmp;
    }
	return true;	
}

bool CBMP::pixelNormalization( void* tmp )//用于将计算过程中得到的RGB值转化为对应的0-255之间
{
	if ( tmp == NULL || imageData == NULL )
		return false;
	int minR, maxR,minG, maxG,minB, maxB;
    rgbInt* end = (rgbInt*)( (int*)tmp + imageSize*2 );
	rgbInt* p = (rgbInt*)tmp;
	minR = maxR = p->r;
	minG = maxG = p->g;
	minB = maxB = p->b;
	//以下用于遍历所有像素，寻找最小值和最大值
    for ( p ; p!=end; ++p ) {
		if ( p->r > maxR )
			maxR = p->r;
		if ( p->r < minR )
			minR = p->r;
		if ( p->g > maxG )
			maxG = p->g;
		if ( p->g < minG )
			minG = p->g;
		if ( p->b > maxB )
			maxB = p->b;
		if ( p->b < minB )
			minB = p->b;
    }
    //找到最值之后，计算其差，得到下面的计算系数
	int diffR,diffG,diffB;
	diffR = 255/(maxR - minR);
	diffG = 255/(maxG - minG);
	diffB = 255/(maxB - minB);
	rgbData* q = (rgbData*)imageData;
	//用于将每个像素值转换为 0-255之间
    for ( p = (rgbInt*)tmp,q; p!=end; ++p ,++q) {
		q->r = p->r*diffR*(p->r - minR);
		q->g = p->g*diffG*(p->g - minG);
		q->b = p->b*diffB*(p->b - minB);
	}
	return true;
}

bool CBMP::iamgeSoomth( int *matrix )
{
	if ( matrix == NULL )
		return false;
	int* pMid = matrix + 4;
	int sumr, sumg, sumb,denominator = 0;
	int idxImage=0,idxMatrix = 0;
	for ( int i = 0; i < 9; i++ )
		denominator += matrix[i];
	if ( denominator == 0 )
		return false;
	//申请控件，用来缓存中间的计算结果
	void* tmp;
	tmp = malloc( imageSize*2 );
	if ( tmp == NULL )
		return false;
	//将原来的像素值分别赋值给刚申请的地址
	rgbData* pEnd = (rgbData*)( (unsigned char*)imageData + imageSize );
    rgbData* p  = (rgbData*)imageData;
	rgbInt* q = (rgbInt*)tmp;
	for ( p,q; p != pEnd; ++p ,++q )	{
		q->r = p->r;
		q->g = p->g;
		q->b = p->b;
	}
	//进行中间的计算，将结果暂时存在新申请的整型空间里
    rgbInt* pCurrent  = (rgbInt*)tmp;
	q = (rgbInt*)tmp;
	for ( int j = 1; j < bi.biHeight-1; j++ ){
		for ( i = 1; i< bi.biWidth-1; i++ ){
			pCurrent = q + (i +j*bi.biWidth);
			
			sumr = 0; sumg = 0;sumb = 0;
			for ( int n = -1;n<2;n++ ){
				for (int m = -1;m<2;m++){
					idxMatrix = m+n*3;
					idxImage = m+n*bi.biWidth;
					sumr += (pCurrent+idxImage)->r*(*(pMid+idxMatrix));
					sumg += (pCurrent+idxImage)->g*(*(pMid+idxMatrix));
					sumb += (pCurrent+idxImage)->b*(*(pMid+idxMatrix));
				}
			}
			pCurrent->r = sumr/denominator;
			pCurrent->g = sumg/denominator;
			pCurrent->b = sumb/denominator;
		}
	}
	//对整型空间内数据进行归一处理，将结果保存到影响数据内
	if (!pixelNormalization( tmp ))
		return false;
	free( tmp );
	return true;	
}

byte CBMP::bound( float pixel , byte low , byte high )
{
	if ( pixel > high )
		pixel = high;
	else if( pixel < low )
		pixel = low;
	return (byte)pixel;
}
bool CBMP::imageLighten( float multiple )
{
    rgbData* end = (rgbData*)((unsigned char*)imageData + imageSize ), *p;
	float r,g,b;
    for ( p = (rgbData*)imageData; p!=end; ++p ) {
		r = p->r * multiple;
		g = p->g * multiple;
		b = p->b * multiple;
		p->r = bound( r,0,255 );
		p->g = bound( g,0,255 );
		p->b = bound( b,0,255 );
    }
	return true;	
}

/** appname -i bmpInFile -o bmpOutFile -c "smooth | sharpen | reverse | togray |..."
*/

int main( int argc, const char* argv[] )
{
	float lightenNum = 0;
    // 从应用程序取文件的路径和所需要进行的操作
    if ( argc != 9 ) {
		printf ( "should use like follow: %s -i bmpInFile -o bmpOutFile -c 'ligthen | smooth | sharpen | reverse | togray | ...' -p 'lightenPara'", argv[0] );
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
	CBMP bmp ;
	if (!bmp.read_bmp(infile))
		return -1;

	int matrixSharpen[9] = {-1,-2,-1,-2,19,-2,-1,-2,-1};
	int matrixSmooth[9] = {1,2,1,2,4,2,1,2,1};

	// 在这里调用你的操作
    switch ( operation ) {
		case OPERATION_SMOOTH:
			printf ("smooth operation here\n");
			bmp.iamgeSoomth(matrixSmooth);
			break;
		case OPERATION_REVERSE:
			printf ("reverse operation here\n");
			bmp.image_negative();
			break;
		case OPERATION_TOGRAY:
			printf ("ToGray operation here\n");
			bmp.imageToGray();
			break;
		case OPERATION_SHARPEN:
			printf ("Sharpen operation here\n");
			bmp.iamgeSoomth(matrixSharpen);
			break;
		case OPERATION_LIGHTEN:
			printf( "Lighten operation here\n" );
			bmp.imageLighten( lightenNum );
			break;
    }
	if ( !bmp.write_bmp(outfile) )
		return -1; 
	return 0;
}