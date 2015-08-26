#ifndef __STRID_H__
#define __STRID_H__

#include <cstdio>
#include <map>
#include <string>
#include <vector>
using namespace std;

class StrId
{
public:
	int add( const string str )
	{
		Iter iter = str2id.find( str );
		if ( iter == str2id.end() )
		{
			int size = (int)id2str.size();
			str2id.insert(pair<string,int>(str,size));
			id2str.push_back(str);
			return size;
		}
		return iter->second;
	}
	bool get( const int id, string &outstr )
	{
		if ( id < 0 || id >= (int)id2str.size() )
			return false;
		outstr = id2str[id];
		return true;
	}
	bool get( const string str, int &outid )
	{
		Iter iter = str2id.find(str);
		if ( iter == str2id.end() )
			return false;
		else
		{
			outid = iter->second;
			return true;
		}
	}
	void print( FILE *fout )
	{
		int size = (int)id2str.size();
		fprintf( fout, "%d\n", size );
		for( int i = 0; i < size; i ++ )
			fprintf( fout, "%s\n", id2str[i].c_str() );
	}
	void read( FILE *fin )
	{
		str2id.clear();
		id2str.clear();
		int size;
		char buf[1024];
		fscanf( fin, "%d", &size );
		for( int i = 0; i < size; i ++ )
		{
			fscanf( fin, "%s", buf );
			str2id.insert(pair<string,int>(buf,i));
			id2str.push_back(buf);
		}
	}
	int size()	{	return id2str.size();	}
private:
	typedef map<string,int>::iterator Iter;
	map<string,int> str2id;
	vector<string> id2str;
};

#endif
