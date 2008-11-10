#pragma once

class CFileWrapper
{
    FILE* _file;
public:
    CFileWrapper ( const char* name, const char* flag ) {
	if ( 0 == (_file = fopen ( name, flag )) )
	    throw std::logic_error("Cannot open this file" );
    }
    ~CFileWrapper () {
	fclose ( _file );
    }
    FILE* handle() { return _file; }
};

class CMallocWrapper
{
    void* _data;
public:
    CMallocWrapper ( int size ) {
	if ( 0 == (_data = malloc ( size )) )
	    throw std::out_of_range("out of memory" );
    }
    ~CMallocWrapper () {
	free ( _data );
    }
    void* handle() { return _data; }
};
