#ifndef __CACHE_H__
#define __CACHE_H__

/**
 * this is a Cache for a Matrix
 * The Matrix is M * M, but only 
 * size row will be stored.
 * a LRU cache algorithm is used
 */

class Cache
{
public:
	Cache( int _M, int _size ) : M(_M), size(_size)
	{
		int i;
		nodes = new Node[size];
		for( i = 0; i < size; i ++ )
		{
			int prev = i - 1 != -1 ? i - 1 : size - 1;
			int next = i + 1 != size ? i + 1 : 0;
			nodes[i].prev = &nodes[prev];
			nodes[i].next = &nodes[next];
			nodes[i].data = new double[M];
			nodes[i].index = -1;
		}
		head = &nodes[0];
		index2node = new Node*[M];
		for( i = 0; i < M; i ++ )
			index2node[i] = NULL;
	}
	bool getData( int rowNum, double *&row )
	{
		Node *rNode = index2node[rowNum];
		if( rNode != NULL )
		{
			row = rNode->data;
			// move rNode to last (head->prev)
			if( rNode != head )
			{
				// delete rNode, and insert it to head->prev
				rNode->prev->next = rNode->next;
				rNode->next->prev = rNode->prev;
				rNode->next = head;
				rNode->prev = head->prev;
				head->prev->next = rNode;
				head->prev = rNode;
			}
			else
				head = head->next;
			return true;
		}
		else
		{
			if( head->index != -1 )
				index2node[head->index] = NULL;
			head->index = rowNum;
			row = head->data;
			index2node[rowNum] = head;
			head = head->next;
			return false;
		}
	}
	~Cache()
	{
		for( int i = 0; i < size; i ++ )
			delete []nodes[i].data;
		delete []nodes;
		delete []index2node;
	}
private:
	struct Node
	{
		double *data;
		int index;
		Node *prev;
		Node *next;
	};

	int M;
	int size;
	Node *head;
	Node *nodes;
	Node **index2node;
};

#endif
