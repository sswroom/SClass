#ifndef _SM_DATA_ICASEBTREESTRINGMAPOBJ
#define _SM_DATA_ICASEBTREESTRINGMAPOBJ
#include "Data/BTreeStringMapObj.hpp"

namespace Data
{
	template <class T> class ICaseBTreeStringMapObj : public Data::BTreeStringMapObj<T>
	{
	protected:
		T PutNode(NN<BTreeStringObjNode<T>> node, Text::CStringNN key, UInt32 hash, T val);
		virtual UInt32 CalHash(UnsafeArray<const UTF8Char> key, UIntOS keyLen) const;
	public:
		ICaseBTreeStringMapObj();
		virtual ~ICaseBTreeStringMapObj();

		virtual T Get(Text::CStringNN key) const;
		virtual T Remove(Text::CStringNN key);
	};

	template <class T> T ICaseBTreeStringMapObj<T>::PutNode(NN<BTreeStringObjNode<T>> node, Text::CStringNN key, UInt32 hash, T val)
	{
		NN<BTreeStringObjNode<T>> leftNode;
		NN<BTreeStringObjNode<T>> rightNode;
		NN<BTreeStringObjNode<T>> tmpNode;
		T retVal;
		IntOS i;
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
			if (!node->leftNode.SetTo(leftNode))
			{
				tmpNode = node;
				node->leftNode = leftNode = this->NewNode(key, hash, val);
				leftNode->parNode = node;
				node->nodeCnt++;
				while (node->parNode.SetTo(node))
				{
					node->nodeCnt++;
				}
				node = tmpNode;
				UInt32 currLev = 1;
				while (true)
				{
					if (node->maxLev >= currLev)
					{
						break;
					}
					node->maxLev = currLev;
					currLev++;
					if (!node->parNode.SetTo(node))
					{
						break;
					}
				}
				return nullptr;
			}
			else
			{
				retVal = PutNode(leftNode, key, hash, val);
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
			if (!node->rightNode.SetTo(rightNode))
			{
				tmpNode = node;
				node->rightNode = rightNode = this->NewNode(key, hash, val);
				rightNode->parNode = node;
				node->nodeCnt++;
				while (node->parNode.SetTo(node))
				{
					node->nodeCnt++;
				}
				node = tmpNode;
				UInt32 currLev = 1;
				while (true)
				{
					if (node->maxLev >= currLev)
					{
						break;
					}
					node->maxLev = currLev;
					currLev++;
					if (!node->parNode.SetTo(node))
					{
						break;
					}
				}
				return nullptr;
			}
			else
			{
				retVal = PutNode(rightNode, key, hash, val);
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

	template <class T> UInt32 ICaseBTreeStringMapObj<T>::CalHash(UnsafeArray<const UTF8Char> key, UIntOS keyLen) const
	{
		UTF8Char sbuff[256];
		UIntOS charCnt = (UIntOS)(Text::StrToUpperC(sbuff, key, keyLen) - sbuff);
		return this->crc->CalcDirect(sbuff, charCnt);
	}

	template <class T> ICaseBTreeStringMapObj<T>::ICaseBTreeStringMapObj() : BTreeStringMapObj<T>()
	{
	}

	template <class T> ICaseBTreeStringMapObj<T>::~ICaseBTreeStringMapObj()
	{
	}

	template <class T> T ICaseBTreeStringMapObj<T>::Get(Text::CStringNN key) const
	{
		UInt32 hash = CalHash(key.v, key.leng);
		Optional<BTreeStringObjNode<T>> node = this->rootNode;
		NN<BTreeStringObjNode<T>> nnnode;
		while (node.SetTo(nnnode))
		{
			IntOS i;
			if (nnnode->nodeHash == hash)
			{
				i = Text::StrCompareICase(nnnode->nodeKey, key.v);
			}
			else if (nnnode->nodeHash > hash)
			{
				i = 1;
			}
			else
			{
				i = -1;
			}
			if (i > 0)
			{
				node = nnnode->leftNode;
			}
			else if (i < 0)
			{
				node = nnnode->rightNode;
			}
			else
			{
				return nnnode->nodeVal;
			}
		}
		return nullptr;
	}

	template <class T> T ICaseBTreeStringMapObj<T>::Remove(Text::CStringNN key)
	{
		NN<BTreeStringObjNode<T>> rootNode;
		if (!this->rootNode.SetTo(rootNode))
			return nullptr;
		if (Text::StrEqualsICaseC(rootNode->nodeKey, rootNode->keyLen, key.v, key.leng))
		{
			T nodeVal = rootNode->nodeVal;
			this->rootNode = this->RemoveNode(rootNode);
			return nodeVal;
		}
		else
		{
			UInt32 hash = CalHash(key.v, key.leng);
			Optional<BTreeStringObjNode<T>> node = rootNode;
			NN<BTreeStringObjNode<T>> nnnode;
			NN<BTreeStringObjNode<T>> parNode = rootNode;
			while (node.SetTo(nnnode))
			{
				IntOS i;
				if (nnnode->nodeHash == hash)
				{
					i = Text::StrCompareICase(nnnode->nodeKey, key.v);
				}
				else if (nnnode->nodeHash > hash)
				{
					i = 1;
				}
				else
				{
					i = -1;
				}
				if (i > 0)
				{
					parNode = nnnode;
					node = nnnode->leftNode;
				}
				else if (i < 0)
				{
					parNode = nnnode;
					node = nnnode->rightNode;
				}
				else
				{
					T nodeVal = nnnode->nodeVal;
					if (parNode->leftNode == nnnode)
					{
						parNode->leftNode = this->RemoveNode(nnnode);
					}
					else
					{
						parNode->rightNode = this->RemoveNode(nnnode);
					}
					return nodeVal;
				}
			}
			return nullptr;
		}
	}
};

#endif
