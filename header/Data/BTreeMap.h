#ifndef _SM_DATA_BTREEMAP
#define _SM_DATA_BTREEMAP
#include "MyMemory.h"
#include "Crypto/Hash/CRC32RC.h"
#include "Data/IMap.h"
#include "Text/String.h"

namespace Data
{
	template <typename T> struct BTreeNode
	{
		UInt32 nodeCnt;
		UInt32 maxLev;
		BTreeNode<T> *parNode;
		BTreeNode<T> *leftNode;
		BTreeNode<T> *rightNode;
		T nodeVal;
		Text::String *nodeStr;
		UInt32 nodeHash;
	};

	template <class T> class BTreeMap : public IMap<Text::String*, T>
	{
	protected:
		Crypto::Hash::CRC32RC *crc;
		BTreeNode<T> *rootNode;

	protected:
		void OptimizeNode(BTreeNode<T> *node);
		void ReleaseNodeTree(BTreeNode<T> *node);
		BTreeNode<T> *NewNode(Text::String *key, UInt32 hash, T val);
		virtual T PutNode(BTreeNode<T> *node, Text::String *key, UInt32 hash, T val);
		BTreeNode<T> *RemoveNode(BTreeNode<T> *node);
		void FillArr(T **arr, BTreeNode<T> *node);
		void FillNameArr(Text::String ***arr, BTreeNode<T> *node);
		virtual UInt32 CalHash(Text::String *key);
	public:
		BTreeMap();
		virtual ~BTreeMap();

		virtual T Put(Text::String *key, T val);
		virtual T Get(Text::String *key);
		virtual T Remove(Text::String *key);
		virtual Bool IsEmpty();
		virtual T *ToArray(UOSInt *objCnt);
		virtual Text::String **ToNameArray(UOSInt *objCnt);
		virtual void Clear();
	};

	template <class T> void BTreeMap<T>::OptimizeNode(BTreeNode<T> *node)
	{
		Int32 leftLev;
		Int32 rightLev;
		if (node->leftNode)
		{
			leftLev = node->leftNode->nodeCnt + 1;
		}
		else
		{
			leftLev = 0;
		}
		if (node->rightNode)
		{
			rightLev = node->rightNode->nodeCnt + 1;
		}
		else
		{
			rightLev = 0;
		}
		if (leftLev > rightLev + 1)
		{
			/////////////////////////////////
		}
		else if (rightLev > leftLev + 1)
		{
			/////////////////////////////////
		}
	}

	template <class T> void BTreeMap<T>::ReleaseNodeTree(BTreeNode<T> *node)
	{
		if (node->leftNode)
		{
			ReleaseNodeTree(node->leftNode);
		}
		if (node->rightNode)
		{
			ReleaseNodeTree(node->rightNode);
		}
		node->nodeStr->Release();
		MemFree(node);
	}

	template <class T> BTreeNode<T> *BTreeMap<T>::NewNode(Text::String *key, UInt32 hash, T val)
	{
		BTreeNode<T> *node = MemAlloc(BTreeNode<T>, 1);
		node->nodeCnt = 0;
		node->parNode = 0;
		node->leftNode = 0;
		node->rightNode = 0;
		node->nodeVal = val;
		node->nodeHash = hash;
		node->maxLev = 0;
		node->nodeStr = key->Clone();
		return node;
	}

	template <class T> T BTreeMap<T>::PutNode(BTreeNode<T> *node, Text::String *key, UInt32 hash, T val)
	{
		BTreeNode<T> *tmpNode;
		T retVal;
		OSInt i;
		if (node->nodeHash == hash)
		{
			i = node->nodeStr->CompareTo(key);
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
				node->leftNode = NewNode(key, hash, val);
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
				node->rightNode = NewNode(key, hash, val);
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

	template <class T> BTreeNode<T> *BTreeMap<T>::RemoveNode(BTreeNode<T> *node)
	{
		BTreeNode<T> *parNode = node->parNode;
		while (parNode)
		{
			parNode->nodeCnt--;
			parNode = parNode->parNode;
		}
		if (node->leftNode == 0 && node->rightNode == 0)
		{
			node->nodeStr->Release();
			MemFree(node);
			return 0;
		}
		else if (node->leftNode == 0)
		{
			BTreeNode<T> *outNode = node->rightNode;
			outNode->parNode = node->parNode;
			node->nodeStr->Release();
			MemFree(node);
			return outNode;
		}
		else if (node->rightNode == 0)
		{
			BTreeNode<T> *outNode = node->leftNode;
			outNode->parNode = node->parNode;
			node->nodeStr->Release();
			MemFree(node);
			return outNode;
		}

		if (node->leftNode->nodeCnt >= node->rightNode->nodeCnt)
		{
			BTreeNode<T> *outNode = node->leftNode;
			BTreeNode<T> *tmpNode;
			while (outNode->rightNode)
			{
				outNode = outNode->rightNode;
			}
			tmpNode = outNode->parNode;
			while (tmpNode != node && tmpNode != 0)
			{
				tmpNode->nodeCnt--;
				tmpNode = tmpNode->parNode;
			}
			if (outNode->leftNode)
			{
				if (outNode->parNode->leftNode == outNode)
				{
					outNode->parNode->leftNode = outNode->leftNode;
				}
				else
				{
					outNode->parNode->rightNode = outNode->leftNode;
				}
				outNode->leftNode->parNode = outNode->parNode;
			}
			outNode->parNode = node->parNode;
			outNode->leftNode = node->leftNode;
			outNode->rightNode = node->rightNode;
			outNode->nodeCnt = 0;
			if (outNode->leftNode)
			{
				outNode->leftNode->parNode = outNode;
				outNode->nodeCnt += outNode->leftNode->nodeCnt + 1;
			}
			if (outNode->rightNode)
			{
				outNode->rightNode->parNode = outNode;
				outNode->nodeCnt += outNode->rightNode->nodeCnt + 1;
			}
			node->nodeStr->Release();
			MemFree(node);
			return outNode;
		}
		else
		{
			BTreeNode<T> *outNode = node->rightNode;
			BTreeNode<T> *tmpNode;
			while (outNode->leftNode)
			{
				outNode = outNode->leftNode;
			}
			tmpNode = outNode->parNode;
			while (tmpNode != node && tmpNode != 0)
			{
				tmpNode->nodeCnt--;
				tmpNode = tmpNode->parNode;
			}
			if (outNode->rightNode)
			{
				if (outNode->parNode->leftNode == outNode)
				{
					outNode->parNode->leftNode = outNode->rightNode;
				}
				else
				{
					outNode->parNode->rightNode = outNode->rightNode;
				}
				outNode->rightNode->parNode = outNode->parNode;
			}
			outNode->parNode = node->parNode;
			outNode->leftNode = node->leftNode;
			outNode->rightNode = node->rightNode;
			outNode->nodeCnt = 0;
			if (outNode->leftNode)
			{
				outNode->leftNode->parNode = outNode;
				outNode->nodeCnt += outNode->leftNode->nodeCnt + 1;
			}
			if (outNode->rightNode)
			{
				outNode->rightNode->parNode = outNode;
				outNode->nodeCnt += outNode->rightNode->nodeCnt + 1;
			}
			node->nodeStr->Release();
			MemFree(node);
			return outNode;
		}
	}

	template <class T> UInt32 BTreeMap<T>::CalHash(Text::String *key)
	{
		return this->crc->CalcDirect(key->v, key->leng);
	}

	template <class T> void BTreeMap<T>::FillArr(T **arr, BTreeNode<T> *node)
	{
		if (node == 0)
			return;
		FillArr(arr, node->leftNode);
		**arr = node->nodeVal;
		++*arr;
		FillArr(arr, node->rightNode);
	}

	template <class T> void BTreeMap<T>::FillNameArr(Text::String ***arr, BTreeNode<T> *node)
	{
		if (node == 0)
			return;
		FillNameArr(arr, node->leftNode);
		**arr = node->nodeStr;
		++*arr;
		FillNameArr(arr, node->rightNode);
	}

	template <class T> BTreeMap<T>::BTreeMap() : IMap<Text::String*, T>()
	{
		rootNode = 0;
		NEW_CLASS(crc, Crypto::Hash::CRC32RC());
	}

	template <class T> BTreeMap<T>::~BTreeMap()
	{
		if (this->rootNode)
		{
			ReleaseNodeTree(this->rootNode);
			this->rootNode = 0;
		}
		DEL_CLASS(crc);
	}

	template <class T> T BTreeMap<T>::Put(Text::String *key, T val)
	{
		UInt32 hash = CalHash(key);
		if (this->rootNode == 0)
		{
			this->rootNode = NewNode(key, hash, val);
			return 0;
		}
		else
		{
			T tmpVal;
			tmpVal = PutNode(this->rootNode, key, hash, val);
			return tmpVal;
		}
	}

	template <class T> T BTreeMap<T>::Get(Text::String *key)
	{
		UInt32 hash = CalHash(key);
		BTreeNode<T> *node = this->rootNode;
		while (node)
		{
			OSInt i;
			if (node->nodeHash == hash)
			{
				i = node->nodeStr->CompareTo(key);
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

	template <class T> T BTreeMap<T>::Remove(Text::String *key)
	{
		if (this->rootNode == 0)
			return 0;
		if (this->rootNode->nodeStr->Equals(key))
		{
			T nodeVal = this->rootNode->nodeVal;
			this->rootNode = RemoveNode(this->rootNode);
			return nodeVal;
		}
		else
		{
			UInt32 hash = CalHash(key);
			BTreeNode<T> *parNode = 0;
			BTreeNode<T> *node = this->rootNode;
			while (node)
			{
				OSInt i;
				if (node->nodeHash == hash)
				{
					i = node->nodeStr->CompareTo(key);
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
						parNode->leftNode = RemoveNode(node);
					}
					else
					{
						parNode->rightNode = RemoveNode(node);
					}
					return nodeVal;
				}
			}
			return 0;
		}
	}

	template <class T> Bool BTreeMap<T>::IsEmpty()
	{
		return this->rootNode == 0;
	}

	template <class T> T *BTreeMap<T>::ToArray(UOSInt *objCnt)
	{
		UOSInt cnt = 0;
		if (this->rootNode)
		{
			cnt = this->rootNode->nodeCnt + 1;
		}
		T *outArr = MemAlloc(T, cnt);
		T *tmpArr = outArr;
		FillArr(&tmpArr, this->rootNode);
		*objCnt = cnt;
		return outArr;
	}

	template <class T> Text::String **BTreeMap<T>::ToNameArray(UOSInt *objCnt)
	{
		UOSInt cnt = 0;
		if (this->rootNode)
		{
			cnt = this->rootNode->nodeCnt + 1;
		}
		Text::String **outArr = MemAlloc(Text::String*, cnt);
		Text::String **tmpArr = outArr;
		FillNameArr(&tmpArr, this->rootNode);
		*objCnt = cnt;
		return outArr;
	}

	template <class T>void BTreeMap<T>::Clear()
	{
		if (this->rootNode)
		{
			ReleaseNodeTree(this->rootNode);
			this->rootNode = 0;
		}
	}
}

#endif
