#ifndef __VARSTR_H__
#define __VARSTR_H__

#include <string.h>

class VarStr
{
public:
	VarStr( int len = 1024 ) : capacity(len), size(0)
	{
		capacity = capacity > 1 ? capacity : 1;
		carr = new char[capacity+1];
	}
	void clear()	{	size = 0;	}
	void add( char ch )
	{
		if (size < capacity)
		{
			carr[size++] = ch;
			carr[size] = '\0';
		}
		else
		{
			capacity += capacity;
			resize(capacity);
			carr[size++] = ch;
			carr[size] = '\0';
		}
	}
	const char* c_str()
	{
		return (const char*)carr;
	}
	int length()
	{
		return size;
	}
	~VarStr()
	{
		delete []carr;
	}
private:
	char *carr;
	int size;
	int capacity;

	void resize( int newsize )
	{
		char *tmp = new char[newsize+1];
		strcpy(tmp,carr);
		delete []carr;
		carr = tmp;
	}
};

#endif
