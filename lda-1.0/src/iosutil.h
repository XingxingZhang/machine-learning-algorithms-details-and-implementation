#ifndef __IOSUTIL_H__
#define __IOSUTIL_H__

/**
 * This is an efficient text line parser
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

class iosutil
{
public:
	iosutil( int initBufSize = 1024 ) : bufSize(initBufSize)
	{
		bufSize = bufSize < 2 ? 2 : bufSize;
		buf = (char*)malloc(bufSize*sizeof(char));
		if ( buf == NULL )
			fprintf( stderr, "iosutil out of memory!\n" );
	}
	const char *getline( FILE *fin )
	{
		buf[bufSize - 2] = 0;
		if ( fgets(buf,bufSize,fin) == NULL )
			return NULL;
		int checkPos = bufSize - 2;
		if ( buf[checkPos] == 0 || buf[checkPos] == '\n' )
			return (const char*)buf;
		do{
			int oldSize = bufSize;
			resize();
			checkPos = bufSize - 2;
			buf[checkPos] = 0;
			fgets(buf+oldSize-1,bufSize-oldSize+1,fin);
		}while( buf[checkPos] != 0 && buf[checkPos] != '\n' );
		
		return (const char*)buf;
	}
	void start()
	{
		tok_index = 0;
		tmp_char = 0;
	}
	const char *token( const char *sep )
	{
		if (tmp_char != 0)
			buf[tok_index] = tmp_char;
		while( buf[tok_index] != '\0' && strchr(sep,buf[tok_index]) )
			tok_index ++;
		if (buf[tok_index] == '\0')
			return NULL;
		char *word = buf+tok_index;
		while( buf[tok_index] != '\0' && !strchr(sep,buf[tok_index]) )
			tok_index ++;
		tmp_char = buf[tok_index];
		buf[tok_index] = '\0';

		return (const char*)word;
	}
	~iosutil()
	{
		free(buf);
	}
private:
	char *buf;
	int bufSize;
	int tok_index;
	char tmp_char;

	void resize()
	{
		bufSize += bufSize;
		buf = (char*)realloc(buf,bufSize*sizeof(char));
		if ( buf == NULL )
			fprintf( stderr, "iosutil out of memory!\n" );
	}
};

#endif
