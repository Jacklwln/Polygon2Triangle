#ifndef _AVL
#define _AVL

#include <cstddef>
#include <iostream>
#include "Comparable.h"

using namespace std;

template <class KeyType>
class AvlNode
{
public:
	enum {
		MAX_SUBTREES = 2
	};
	enum dir_t
	{
		LEFT = 0,
		RIGHT = 1
	};

	static dir_t Opposite(dir_t dir)
	{
		return dir_t(1 - int(dir));
	}

	AvlNode(Comparable<KeyType>  *item = NULL);
	virtual ~AvlNode(void);

	Comparable<KeyType>* Data() const
	{
		return myData;
	}

	KeyType Key() const
	{
		return myData->Key();
	}
	short Bal() const
	{
		return myBal;
	}
	AvlNode* Subtree(dir_t dir) const
	{
		return mySubtree[dir];
	}
	AvlNode<KeyType>* Parent(AvlNode<KeyType> *);
	static AvlNode<KeyType>* Search(KeyType key, AvlNode<KeyType> * root, cmp_t cmp = EQ_CMP);
	static AvlNode<KeyType>* Insert(Comparable<KeyType> * item, AvlNode<KeyType> * & root);
	static Comparable<KeyType>* Delete(KeyType key, AvlNode<KeyType> * & root, cmp_t cmp = EQ_CMP);
	int Height() const;
	int Check() const;

#ifdef CUSTOM_ALLOCATE
	void *
		operator new(size_t);

	void
		operator delete(void *);
#endif


private:
	enum balance_t
	{
		LEFT_HEAVY = -1,
		BALANCED = 0,
		RIGHT_HEAVY = 1
	};
	enum height_effect_t
	{
		HEIGHT_NOCHANGE = 0,
		HEIGHT_CHANGE = 1
	};

	inline static int LEFT_IMBALANCE(short bal)
	{
		return (bal < LEFT_HEAVY);
	}

	inline static int RIGHT_IMBALANCE(short bal)
	{
		return (bal > RIGHT_HEAVY);
	}


	Comparable<KeyType>* myData;
	AvlNode<KeyType>* mySubtree[MAX_SUBTREES];
	short myBal;

	void Reset(void)
	{
		myBal = 0;
		mySubtree[LEFT] = mySubtree[RIGHT] = NULL;
	}

	static AvlNode<KeyType>* Insert(Comparable<KeyType>* item,
		AvlNode<KeyType>* &root,
		int &change);

	static Comparable<KeyType>* Delete(KeyType key,
		AvlNode<KeyType>* &root,
		int &change,
		cmp_t cmp = EQ_CMP);

	static int RotateOnce(AvlNode<KeyType> * & root, dir_t dir);

	static int RotateTwice(AvlNode<KeyType> * & root, dir_t dir);


	static int ReBalance(AvlNode<KeyType> * & root);

	cmp_t Compare(KeyType key, cmp_t cmp = EQ_CMP) const;

private:
	AvlNode(const AvlNode<KeyType> &);
	AvlNode & operator=(const AvlNode<KeyType> &);

};


template <class KeyType>
class AvlTree {
private:
	AvlTree(const AvlTree<KeyType>&);
	AvlTree& operator=(const AvlTree<KeyType>&);

public:
	AvlNode<KeyType> *myRoot;

	AvlTree() : myRoot(NULL) {}
	~AvlTree()
	{
		if (myRoot)
			delete myRoot;
	}

	void DumpTree() const;

	int IsEmpty() const
	{
		return myRoot == NULL;
	}

	AvlNode<KeyType>* Search(KeyType key, cmp_t cmp = EQ_CMP)
	{
		return AvlNode<KeyType>::Search(key, myRoot, cmp);
	}

	AvlNode<KeyType>* Insert(Comparable<KeyType> *item)
	{
		return AvlNode<KeyType>::Insert(item, myRoot);
	}

	Comparable<KeyType>* Delete(KeyType key, cmp_t cmp = EQ_CMP)
	{
		return AvlNode<KeyType>::Delete(key, myRoot, cmp);
	}

	AvlNode<KeyType>* Next(AvlNode<KeyType>*node)
	{
		AvlNode<KeyType> *q, *p = node->Subtree(AvlNode<KeyType>::RIGHT);
		if (p)
		{
			while (p->Subtree(AvlNode<KeyType>::LEFT)) p = p->Subtree(AvlNode<KeyType>::LEFT);
			return p;
		}
		else {
			q = node;
			p = node->Parent(myRoot);
			while (p && (q == p->Subtree(AvlNode<KeyType>::RIGHT))) {
				q = p;
				p = p->Parent(myRoot);
			}
			return p;
		}
	}

	AvlNode<KeyType> *Prev(AvlNode<KeyType>*node)
	{
		AvlNode<KeyType> *q, *p = node->Subtree(AvlNode<KeyType>::LEFT);
		if (p)
		{
			while (p->Subtree(AvlNode<KeyType>::RIGHT)) p = p->Subtree(AvlNode<KeyType>::RIGHT);
			return p;
		}
		else {
			q = node;
			p = node->Parent(myRoot);
			while (p && (q == p->Subtree(AvlNode<KeyType>::LEFT))) {
				q = p;
				p = p->Parent(myRoot);
			}

			return p;
		}
	}

	int Check() const
	{
		return (myRoot) ? myRoot->Check() : 1;
	}
};


inline static int MIN(int a, int b)
{
	return ((a) < (b)) ? (a) : (b);
}


inline static int MAX(int a, int b)
{
	return ((a) > (b)) ? (a) : (b);
}


template <class KeyType>
AvlNode<KeyType>::AvlNode(Comparable<KeyType> *item) : myData(item), myBal(0)
{
	Reset();
}

template <class KeyType>
AvlNode<KeyType>::~AvlNode(void)
{
	if (mySubtree[LEFT])
		delete mySubtree[LEFT];
	if (mySubtree[RIGHT])
		delete mySubtree[RIGHT];
}


template <class KeyType>
int AvlNode<KeyType>::RotateOnce(AvlNode<KeyType>* &root, dir_t dir)
{
	dir_t otherDir = Opposite(dir);
	AvlNode<KeyType> *oldRoot = root;

	int  heightChange = (root->mySubtree[otherDir]->myBal == 0)
		? HEIGHT_NOCHANGE
		: HEIGHT_CHANGE;

	root = oldRoot->mySubtree[otherDir];

	oldRoot->mySubtree[otherDir] = root->mySubtree[dir];
	root->mySubtree[dir] = oldRoot;
	oldRoot->myBal = -((dir == LEFT) ? --(root->myBal) : ++(root->myBal));

	return  heightChange;
}

template <class KeyType>
int AvlNode<KeyType>::RotateTwice(AvlNode<KeyType>* &root, dir_t dir)
{
	dir_t otherDir = Opposite(dir);
	AvlNode<KeyType> *oldRoot = root;
	AvlNode<KeyType> *oldOtherDirSubtree = root->mySubtree[otherDir];

	root = oldRoot->mySubtree[otherDir]->mySubtree[dir];
	oldRoot->mySubtree[otherDir] = root->mySubtree[dir];
	root->mySubtree[dir] = oldRoot;

	oldOtherDirSubtree->mySubtree[dir] = root->mySubtree[otherDir];
	root->mySubtree[otherDir] = oldOtherDirSubtree;

	root->mySubtree[LEFT]->myBal = -MAX(root->myBal, 0);
	root->mySubtree[RIGHT]->myBal = -MIN(root->myBal, 0);
	root->myBal = 0;

	return  HEIGHT_CHANGE;
}

template <class KeyType>
int AvlNode<KeyType>::ReBalance(AvlNode<KeyType>* &root) {
	int heightChange = HEIGHT_NOCHANGE;

	if (LEFT_IMBALANCE(root->myBal)) {
		if (root->mySubtree[LEFT]->myBal == RIGHT_HEAVY)
		{
			heightChange = RotateTwice(root, RIGHT);
		}
		else {
			heightChange = RotateOnce(root, RIGHT);
		}
	}
	else if (RIGHT_IMBALANCE(root->myBal)) {
		if (root->mySubtree[RIGHT]->myBal == LEFT_HEAVY)
		{
			heightChange = RotateTwice(root, LEFT);
		}
		else
		{
			heightChange = RotateOnce(root, LEFT);
		}
	}

	return  heightChange;
}

template <class KeyType>
cmp_t AvlNode<KeyType>::Compare(KeyType key, cmp_t cmp) const
{
	switch (cmp)
	{
	default:
	case EQ_CMP:
		return  myData->Compare(key);
	case MIN_CMP:
		return  (mySubtree[LEFT] == NULL) ? EQ_CMP : MIN_CMP;
	case MAX_CMP:
		return  (mySubtree[RIGHT] == NULL) ? EQ_CMP : MAX_CMP;
	}
}

template <class KeyType>
AvlNode<KeyType>* AvlNode<KeyType>::Parent(AvlNode<KeyType>* myRoot)
{
	if (this == myRoot) return 0;

	AvlNode<KeyType> *p = myRoot;
	while (p)
	{
		if (this == p->Subtree(AvlNode<KeyType>::LEFT)) return p;
		if (this == p->Subtree(AvlNode<KeyType>::RIGHT)) return p;

		cmp_t  result = p->Compare(this->Key());
		if (result == MIN_CMP)
			p = p->Subtree(AvlNode<KeyType>::LEFT);
		else
			p = p->Subtree(AvlNode<KeyType>::RIGHT);
	}

	return 0;
}

template <class KeyType>
AvlNode<KeyType>* AvlNode<KeyType>::Search(KeyType key, AvlNode<KeyType> * root, cmp_t cmp)
{
	cmp_t result;
	while (root && (result = root->Compare(key, cmp)))
	{
		root = root->mySubtree[(result < 0) ? LEFT : RIGHT];
	}
	return  (root) ? root : NULL;
}

template <class KeyType>
AvlNode<KeyType>* AvlNode<KeyType>::Insert(Comparable<KeyType> *item, AvlNode<KeyType>* &root)
{
	int change;
	return Insert(item, root, change);
}

template <class KeyType>
Comparable<KeyType> *AvlNode<KeyType>::Delete(KeyType key, AvlNode<KeyType>* &root, cmp_t cmp)
{
	int change;
	return Delete(key, root, change, cmp);
}


template <class KeyType>
AvlNode<KeyType>* AvlNode<KeyType>::Insert(Comparable<KeyType> *item, AvlNode<KeyType>* &root, int &change)
{
	if (root == NULL) {
		root = new AvlNode<KeyType>(item);
		change = HEIGHT_CHANGE;
		return root;
	}

	AvlNode<KeyType> * found = NULL;
	int  increase = 0;

	cmp_t  result = root->Compare(item->Key());
	dir_t  dir = (result == MIN_CMP) ? LEFT : RIGHT;

	if (result != EQ_CMP) {
		found = Insert(item, root->mySubtree[dir], change);
		if (!found) return NULL;
		increase = result * change;
	}
	else 
	{
		increase = HEIGHT_NOCHANGE;
		return NULL;
	}

	root->myBal += increase;

	change = (increase && root->myBal)
		? (1 - ReBalance(root))
		: HEIGHT_NOCHANGE;
	return found;
}


template <class KeyType>
Comparable<KeyType>* AvlNode<KeyType>::Delete(KeyType key, AvlNode<KeyType>* &root, int &change, cmp_t cmp)
{
	if (root == NULL)
	{
		change = HEIGHT_NOCHANGE;
		return  NULL;
	}

	Comparable<KeyType> *found = NULL;
	int decrease = 0;

	cmp_t  result = root->Compare(key, cmp);
	dir_t  dir = (result == MIN_CMP) ? LEFT : RIGHT;

	if (result != EQ_CMP)
	{
		found = Delete(key, root->mySubtree[dir], change, cmp);
		if (!found)
			return found;
		decrease = result * change;
	}
	else
	{
		found = root->myData;

		if ((root->mySubtree[LEFT] == NULL) &&
			(root->mySubtree[RIGHT] == NULL)) {
			delete  root;
			root = NULL;
			change = HEIGHT_CHANGE;
			return found;
		}
		else if ((root->mySubtree[LEFT] == NULL) ||
			(root->mySubtree[RIGHT] == NULL)) {
			AvlNode<KeyType> * toDelete = root;
			root = root->mySubtree[(root->mySubtree[RIGHT]) ? RIGHT : LEFT];
			change = HEIGHT_CHANGE;
			toDelete->mySubtree[LEFT] = toDelete->mySubtree[RIGHT] = NULL;
			delete toDelete;
			return found;
		}
		else
		{
			root->myData = Delete(key, root->mySubtree[RIGHT],
				decrease, MIN_CMP);
		}
	}

	root->myBal -= decrease;

	if (decrease)
	{
		if (root->myBal) {
			change = ReBalance(root);
		}
		else {
			change = HEIGHT_CHANGE;
		}
	}
	else {
		change = HEIGHT_NOCHANGE;
	}

	return  found;
}

template <class KeyType>
int AvlNode<KeyType>::Height() const
{
	int leftHeight = (mySubtree[LEFT]) ? mySubtree[LEFT]->Height() : 0;
	int rightHeight = (mySubtree[RIGHT]) ? mySubtree[RIGHT]->Height() : 0;
	return (1 + MAX(leftHeight, rightHeight));
}

template <class KeyType>
int AvlNode<KeyType>::Check() const
{
	int valid = 1;

	if (mySubtree[LEFT])
		valid *= mySubtree[LEFT]->Check();
	if (mySubtree[RIGHT])
		valid *= mySubtree[RIGHT]->Check();

	int leftHeight = (mySubtree[LEFT]) ? mySubtree[LEFT]->Height() : 0;
	int rightHeight = (mySubtree[RIGHT]) ? mySubtree[RIGHT]->Height() : 0;

	int diffHeight = rightHeight - leftHeight;
	if (LEFT_IMBALANCE(diffHeight) || RIGHT_IMBALANCE(diffHeight))
	{
		valid = 0;
		cerr << "Height difference is " << diffHeight
			<< " at node " << Key() << endl;
	}

	if (diffHeight != myBal)
	{
		valid = 0;
		cerr << "Height difference " << diffHeight
			<< " doesn't match balance-factor of " << myBal
			<< " at node " << Key() << endl;
	}

	if (mySubtree[LEFT] && mySubtree[LEFT]->Compare(Key()) == MIN_CMP)
	{
		valid = 0;
		cerr << "Node " << Key() << " is *smaller* than left subtree"
			<< mySubtree[LEFT]->Key() << endl;
	}
	if (mySubtree[RIGHT] && mySubtree[RIGHT]->Compare(Key()) == MAX_CMP)
	{
		valid = 0;
		cerr << "Node " << Key() << " is *greater* than right subtree"
			<< mySubtree[RIGHT]->Key() << endl;
	}

	return valid;
}

static inline ostream& Indent(ostream &os, int len)
{
	for (int i = 0; i < len; i++)
	{
		os << ' ';
	}
	return os;
}

enum TraversalOrder
{
	LTREE,
	KEY,
	RTREE
};

template <class KeyType>
static void Dump(ostream &os, TraversalOrder order, const AvlNode<KeyType> *node, int level = 0)
{
	unsigned len = (level * 5) + 1;
	if (order == LTREE && node->Subtree(AvlNode<KeyType>::LEFT) == NULL)
	{
		Indent(os, len) << "     **NULL**" << endl;
	}
	if (order == KEY)
	{
		Indent(os, len) << node->Key() << ":" << node->Bal() << endl;
	}
	if ((order == RTREE) && node->Subtree(AvlNode<KeyType>::RIGHT) == NULL)
	{
		Indent(os, len) << "     **NULL**" << endl;
	}
}

template <class KeyType>
static void Dump(ostream &os, const AvlNode<KeyType> *node, int level = 0)
{
	if (node == NULL)
	{
		os << "***EMPTY TREE***" << endl;
	}
	else
	{
		Dump(os, RTREE, node, level);
		if (node->Subtree(AvlNode<KeyType>::RIGHT) != NULL)
		{
			Dump(os, node->Subtree(AvlNode<KeyType>::RIGHT), level + 1);
		}
		Dump(os, KEY, node, level);
		if (node->Subtree(AvlNode<KeyType>::LEFT) != NULL)
		{
			Dump(os, node->Subtree(AvlNode<KeyType>::LEFT), level + 1);
		}
		Dump(os, LTREE, node, level);
	}
}

template <class KeyType>
void AvlTree<KeyType>::DumpTree() const
{
	Dump(cout, myRoot);
}

#endif