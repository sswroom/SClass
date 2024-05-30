#ifndef _SM_DATA_ICASEBTREEUTF8MAP
#define _SM_DATA_ICASEBTREEUTF8MAP
#include "Data/BTreeUTF8Map.h"

namespace Data
{
	template <class T> class ICaseBTreeUTF8Map : public Data::BTreeUTF8Map<T>
	{
	protected:
		T PutNode(BTreeUTF8Node<T> *node, Text::CStringNN key, UInt32 hash, T val);
		virtual UInt32 CalHash(UnsafeArray<const UTF8Char> key, UOSInt keyLen) const;
	public:
		ICaseBTreeUTF8Map();
		virtual ~ICaseBTreeUTF8Map();

		virtual T Get(Text::CStringNN key) const;
		virtual T Remove(Text::CStringNN key);
	};

	template <class T> T ICaseBTreeUTF8Map<T>::PutNode(BTreeUTF8Node<T> *node, Text::CStringNN key, UInt32 hash, T val)
	{
		BTreeUTF8Node<T> *tmpNode;
		T retVal;
		OSInt i;
		if (node->nodeHash == hash)
		{
			i = Text::StrCompareICase(node->nodeKey, key.v);
		}
		else if (node->nodeHash > hash)
		{
			i = 1;
		}
		else
		{
			i = -1;
		}
		if (i == 0)
		{
			retVal = node->nodeVal;
			node->nodeVal = val;
			return retVal;
		}
		else if (i > 0)
		{
			if (node->leftNode == 0)
			{
				tmpNode = node;
				node->leftNode = this->NewNode(key, hash, val);
				node->leftNode->parNode = node;
				while (node)
				{
					node->nodeCnt++;
					node = node->parNode;
				}
				node = tmpNode;
				UInt32 currLev = 1;
				while (node)
				{
					if (node->maxLev >= currLev)
					{
						break;
					}
					node->maxLev = currLev;
					currLev++;
					node = node->parNode;
				}
				return 0;
			}
			else
			{
				retVal = PutNode(node->leftNode, key, hash, val);
/*				Int32 leftLev;
				Int32 rightLev;
				if (node->rightNode == 0)
				{
					rightLev = 0;
				}
				else
				{
					rightLev = node->rightNode->maxLev + 1;
				}
				leftLev = node->leftNode->maxLev + 1;
				if (leftLev > rightLev + 1)
				{
					OptimizeNode(node);
				}*/
				return retVal;
			}
		}
		else
		{
			if (node->rightNode == 0)
			{
				tmpNode = node;
				node->rightNode = this->NewNode(key, hash, val);
				node->rightNode->parNode = node;
				while (node)
				{
					node->nodeCnt++;
					node = node->parNode;
				}
				node = tmpNode;
				UInt32 currLev = 1;
				while (node)
				{
					if (node->maxLev >= currLev)
					{
						break;
					}
					node->maxLev = currLev;
					currLev++;
					node = node->parNode;
				}
				return 0;
			}
			else
			{
				retVal = PutNode(node->rightNode, key, hash, val);
/*				Int32 leftLev;
				Int32 rightLev;
				if (node->leftNode == 0)
				{
					leftLev = 0;
				}
				else
				{
					leftLev = node->leftNode->maxLev + 1;
				}
				rightLev = node->rightNode->maxLev + 1;
				if (rightLev > leftLev + 1)
				{
					OptimizeNode(node);
				}*/
				return retVal;
			}
		}

	}

	template <class T> UInt32 ICaseBTreeUTF8Map<T>::CalHash(UnsafeArray<const UTF8Char> key, UOSInt keyLen) const
	{
		UTF8Char sbuff[256];
		UOSInt charCnt = (UOSInt)(Text::StrToUpperC(sbuff, key, keyLen) - sbuff);
		return this->crc->CalcDirect(sbuff, charCnt);
	}

	template <class T> ICaseBTreeUTF8Map<T>::ICaseBTreeUTF8Map() : BTreeUTF8Map<T>()
	{
	}

	template <class T> ICaseBTreeUTF8Map<T>::~ICaseBTreeUTF8Map()
	{
	}

	template <class T> T ICaseBTreeUTF8Map<T>::Get(Text::CStringNN key) const
	{
		UInt32 hash = CalHash(key.v, key.leng);
		BTreeUTF8Node<T> *node = this->rootNode;
		while (node)
		{
			OSInt i;
			if (node->nodeHash == hash)
			{
				i = Text::StrCompareICase(node->nodeKey, key.v);
			}
			else if (node->nodeHash > hash)
			{
				i = 1;
			}
			else
			{
				i = -1;
			}
			if (i > 0)
			{
				node = node->leftNode;
			}
			else if (i < 0)
			{
				node = node->rightNode;
			}
			else
			{
				return node->nodeVal;
			}
		}
		return 0;
	}

	template <class T> T ICaseBTreeUTF8Map<T>::Remove(Text::CStringNN key)
	{
		if (this->rootNode == 0)
			return 0;
		if (Text::StrEqualsICaseC(this->rootNode->nodeKey, this->rootNode->keyLen, key.v, key.leng))
		{
			T nodeVal = this->rootNode->nodeVal;
			this->rootNode = this->RemoveNode(this->rootNode);
			return nodeVal;
		}
		else
		{
			UInt32 hash = CalHash(key.v, key.leng);
			BTreeUTF8Node<T> *node = this->rootNode;
			BTreeUTF8Node<T>* parNode = node;
			while (node)
			{
				OSInt i;
				if (node->nodeHash == hash)
				{
					i = Text::StrCompareICase(node->nodeKey, key.v);
				}
				else if (node->nodeHash > hash)
				{
					i = 1;
				}
				else
				{
					i = -1;
				}
				if (i > 0)
				{
					parNode = node;
					node = node->leftNode;
				}
				else if (i < 0)
				{
					parNode = node;
					node = node->rightNode;
				}
				else
				{
					T nodeVal = node->nodeVal;
					if (parNode->leftNode == node)
					{
						parNode->leftNode = this->RemoveNode(node);
					}
					else
					{
						parNode->rightNode = this->RemoveNode(node);
					}
					return nodeVal;
				}
			}
			return 0;
		}
	}
};

#endif
