#include <stdio.h>
#include "iosutil.h"

int main( int argc, char **argv )
{
	if ( argc != 2 )
	{
		fprintf( stderr, "test <infile>\n" );
		return 1;
	}

	FILE *fin = fopen( argv[1], "r" );
	iosutil io(2);
	const char *buf;
	int no = 0;
	while( (buf=io.getline(fin)) != NULL )
	{
		printf( "%4d\t%s", ++ no, buf );
		io.start();
		const char *word = NULL;
		while( (word=io.token( " \t\r\n" )) != NULL )
			printf( "[%s]", word );
		putchar( '\n' );
	}

	fclose(fin);

	return 0;
}
