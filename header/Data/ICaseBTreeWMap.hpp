#ifndef _SM_DATA_ICASEBTREEWMAP
#define _SM_DATA_ICASEBTREEWMAP
#include "Data/BTreeWMap.hpp"

namespace Data
{
	template <class T> class ICaseBTreeWMap : public Data::BTreeWMap<T>
	{
	protected:
		T PutNode(BTreeWNode<T> *node, const WChar *key, UInt32 hash, T val);
		virtual UInt32 CalHash(const WChar *key);
	public:
		ICaseBTreeWMap();
		virtual ~ICaseBTreeWMap();

		virtual T Get(const WChar *key);
		virtual T Remove(const WChar *key);
	};

	template <class T> T ICaseBTreeWMap<T>::PutNode(BTreeWNode<T> *node, const WChar *key, UInt32 hash, T val)
	{
		BTreeWNode<T> *tmpNode;
		T retVal;
		OSInt i;
		if (node->nodeHash == hash)
		{
			i = Text::StrCompareICase(node->nodeStr, key);
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
				Int32 currLev = 1;
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
				Int32 currLev = 1;
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

	template <class T> UInt32 ICaseBTreeWMap<T>::CalHash(const WChar *key)
	{
		WChar wbuff[256];
		OSInt charCnt = Text::StrToUpper(wbuff, key) - wbuff;
		this->crc->Clear();
		this->crc->Calc((const UInt8*)wbuff, charCnt * sizeof(WChar));
		UInt8 hash[4];
		this->crc->GetValue(hash);
		return ReadMUInt32(hash);
	}

	template <class T> ICaseBTreeWMap<T>::ICaseBTreeWMap() : BTreeWMap<T>()
	{
	}

	template <class T> ICaseBTreeWMap<T>::~ICaseBTreeWMap()
	{
	}

	template <class T> T ICaseBTreeWMap<T>::Get(const WChar *key)
	{
		UInt32 hash = CalHash(key);
		BTreeWNode<T> *node = this->rootNode;
		while (node)
		{
			OSInt i;
			if (node->nodeHash == hash)
			{
				i = Text::StrCompareICase(node->nodeStr, key);
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

	template <class T> T ICaseBTreeWMap<T>::Remove(const WChar *key)
	{
		if (this->rootNode == 0)
			return 0;
		if (Text::StrCompareICase(this->rootNode->nodeStr, key) == 0)
		{
			T nodeVal = this->rootNode->nodeVal;
			this->rootNode = this->RemoveNode(this->rootNode);
			return nodeVal;
		}
		else
		{
			UInt32 hash = CalHash(key);
			BTreeWNode<T> *parNode = 0;
			BTreeWNode<T> *node = this->rootNode;
			while (node)
			{
				OSInt i;
				if (node->nodeHash == hash)
				{
					i = Text::StrCompare(node->nodeStr, key);
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
