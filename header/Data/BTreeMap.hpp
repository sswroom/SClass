#ifndef _SM_DATA_BTREEMAP
#define _SM_DATA_BTREEMAP
#include "MyMemory.h"
#include "Crypto/Hash/CRC32RC.h"
#include "Data/DataMap.hpp"
#include "Text/MyString.h"
#include "Text/String.h"

namespace Data
{
	template <typename T> struct BTreeNode
	{
		UInt32 nodeCnt;
		UInt32 maxLev;
		Optional<BTreeNode<T>> parNode;
		Optional<BTreeNode<T>> leftNode;
		Optional<BTreeNode<T>> rightNode;
		T nodeVal;
		NN<Text::String> nodeStr;
		UInt32 nodeHash;
	};

	template <class T> class BTreeMap : public DataMap<NN<Text::String>, T>
	{
	protected:
		Crypto::Hash::CRC32RC crc;
		Optional<BTreeNode<T>> rootNode;

	protected:
		void OptimizeNode(NN<BTreeNode<T>> node);
		void ReleaseNodeTree(NN<BTreeNode<T>> node);
		NN<BTreeNode<T>> NewNode(NN<Text::String> key, UInt32 hash, T val);
		virtual T PutNode(NN<BTreeNode<T>> node, NN<Text::String> key, UInt32 hash, T val);
		Optional<BTreeNode<T>> RemoveNode(NN<BTreeNode<T>> node);
		void FillArr(InOutParam<UnsafeArray<T>> arr, NN<BTreeNode<T>> node);
		void FillNameArr(InOutParam<UnsafeArray<NN<Text::String>>> arr, NN<BTreeNode<T>> node);
		virtual UInt32 CalHash(NN<Text::String> key) const;
		UInt32 CalHash(Text::CStringNN key) const;
	public:
		BTreeMap();
		virtual ~BTreeMap();

		virtual T Put(NN<Text::String> key, T val);
		virtual T Get(NN<Text::String> key) const;
		T Get(Text::CStringNN key) const;
		virtual T Remove(NN<Text::String> key);
		virtual Bool IsEmpty() const;
		virtual UnsafeArray<T> ToArray(OutParam<UOSInt> objCnt);
		virtual UnsafeArray<NN<Text::String>> ToNameArray(OutParam<UOSInt> objCnt);
		virtual void Clear();
	};

	template <class T> void BTreeMap<T>::OptimizeNode(NN<BTreeNode<T>> node)
	{
		Int32 leftLev;
		Int32 rightLev;
		NN<BTreeNode<T>> nnnode;
		if (node->leftNode.SetTo(nnnode))
		{
			leftLev = nnnode->nodeCnt + 1;
		}
		else
		{
			leftLev = 0;
		}
		if (node->rightNode.SetTo(nnnode))
		{
			rightLev = nnnode->nodeCnt + 1;
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

	template <class T> void BTreeMap<T>::ReleaseNodeTree(NN<BTreeNode<T>> node)
	{
		NN<BTreeNode<T>> nnnode;
		if (node->leftNode.SetTo(nnnode))
		{
			ReleaseNodeTree(nnnode);
		}
		if (node->rightNode.SetTo(nnnode))
		{
			ReleaseNodeTree(nnnode);
		}
		node->nodeStr->Release();
		MemFreeNN(node);
	}

	template <class T> NN<BTreeNode<T>> BTreeMap<T>::NewNode(NN<Text::String> key, UInt32 hash, T val)
	{
		NN<BTreeNode<T>> node = MemAllocNN(BTreeNode<T>);
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

	template <class T> T BTreeMap<T>::PutNode(NN<BTreeNode<T>> node, NN<Text::String> key, UInt32 hash, T val)
	{
		NN<BTreeNode<T>> tmpNode;
		NN<BTreeNode<T>> nnnode;
		Optional<BTreeNode<T>> optnode;
		T retVal;
		OSInt i;
		if (node->nodeHash == hash)
		{
			i = node->nodeStr->CompareToFast(key->ToCString());
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
			if (!node->leftNode.SetTo(nnnode))
			{
				tmpNode = node;
				node->leftNode = nnnode = NewNode(key, hash, val);
				nnnode->parNode = node;
				optnode = node;
				while (optnode.SetTo(node))
				{
					node->nodeCnt++;
					optnode = node->parNode;
				}
				optnode = tmpNode;
				UInt32 currLev = 1;
				while (optnode.SetTo(node))
				{
					if (node->maxLev >= currLev)
					{
						break;
					}
					node->maxLev = currLev;
					currLev++;
					optnode = node->parNode;
				}
				return 0;
			}
			else
			{
				retVal = PutNode(nnnode, key, hash, val);
				return retVal;
			}
		}
		else
		{
			if (!node->rightNode.SetTo(nnnode))
			{
				tmpNode = node;
				node->rightNode = nnnode = NewNode(key, hash, val);
				nnnode->parNode = node;
				optnode = node;
				while (optnode.SetTo(node))
				{
					node->nodeCnt++;
					optnode = node->parNode;
				}
				optnode = tmpNode;
				UInt32 currLev = 1;
				while (optnode.SetTo(node))
				{
					if (node->maxLev >= currLev)
					{
						break;
					}
					node->maxLev = currLev;
					currLev++;
					optnode = node->parNode;
				}
				return 0;
			}
			else
			{
				retVal = PutNode(nnnode, key, hash, val);
				return retVal;
			}
		}

	}

	template <class T> Optional<BTreeNode<T>> BTreeMap<T>::RemoveNode(NN<BTreeNode<T>> node)
	{
		Optional<BTreeNode<T>> parNode = node->parNode;
		NN<BTreeNode<T>> leftNode;
		NN<BTreeNode<T>> rightNode;
		while (parNode.SetTo(leftNode))
		{
			leftNode->nodeCnt--;
			parNode = leftNode->parNode;
		}
		if (!node->leftNode.SetTo(leftNode))
		{
			if (node->rightNode.SetTo(rightNode))
			{
				rightNode->parNode = node->parNode;
				node->nodeStr->Release();
				MemFreeNN(node);
				return rightNode;
			}
			else
			{
				node->nodeStr->Release();
				MemFreeNN(node);
				return 0;
			}
		}
		else if (!node->rightNode.SetTo(rightNode))
		{
			leftNode->parNode = node->parNode;
			node->nodeStr->Release();
			MemFreeNN(node);
			return leftNode;
		}

		if (leftNode->nodeCnt >= rightNode->nodeCnt)
		{
			NN<BTreeNode<T>> outNode = leftNode;
			Optional<BTreeNode<T>> tmpNode;
			NN<BTreeNode<T>> nnnode;
			while (outNode->rightNode.SetTo(outNode));
			tmpNode = outNode->parNode;
			while (tmpNode.SetTo(nnnode) && nnnode != node)
			{
				nnnode->nodeCnt--;
				tmpNode = nnnode->parNode;
			}
			if (outNode->parNode.SetTo(rightNode) && outNode->leftNode.SetTo(leftNode))
			{
				if (rightNode->leftNode == outNode)
				{
					rightNode->leftNode = outNode->leftNode;
				}
				else
				{
					rightNode->rightNode = outNode->leftNode;
				}
				leftNode->parNode = rightNode;
			}
			outNode->parNode = node->parNode;
			outNode->leftNode = node->leftNode;
			outNode->rightNode = node->rightNode;
			outNode->nodeCnt = 0;
			if (outNode->leftNode.SetTo(leftNode))
			{
				leftNode->parNode = outNode;
				outNode->nodeCnt += leftNode->nodeCnt + 1;
			}
			if (outNode->rightNode.SetTo(rightNode))
			{
				rightNode->parNode = outNode;
				outNode->nodeCnt += rightNode->nodeCnt + 1;
			}
			node->nodeStr->Release();
			MemFreeNN(node);
			return outNode;
		}
		else
		{
			NN<BTreeNode<T>> outNode = rightNode;
			Optional<BTreeNode<T>> tmpNode;
			NN<BTreeNode<T>> nnnode;
			while (outNode->leftNode.SetTo(outNode));
			tmpNode = outNode->parNode;
			while (tmpNode.SetTo(nnnode) && nnnode != node)
			{
				nnnode->nodeCnt--;
				tmpNode = nnnode->parNode;
			}
			if (outNode->parNode.SetTo(leftNode) && outNode->rightNode.SetTo(rightNode))
			{
				if (leftNode->leftNode == outNode)
				{
					leftNode->leftNode = outNode->rightNode;
				}
				else
				{
					leftNode->rightNode = outNode->rightNode;
				}
				rightNode->parNode = leftNode;
			}
			outNode->parNode = node->parNode;
			outNode->leftNode = node->leftNode;
			outNode->rightNode = node->rightNode;
			outNode->nodeCnt = 0;
			if (outNode->leftNode.SetTo(leftNode))
			{
				leftNode->parNode = outNode;
				outNode->nodeCnt += leftNode->nodeCnt + 1;
			}
			if (outNode->rightNode.SetTo(rightNode))
			{
				rightNode->parNode = outNode;
				outNode->nodeCnt += rightNode->nodeCnt + 1;
			}
			node->nodeStr->Release();
			MemFreeNN(node);
			return outNode;
		}
	}

	template <class T> UInt32 BTreeMap<T>::CalHash(NN<Text::String> key) const
	{
		return this->crc.CalcDirect(key->v, key->leng);
	}

	template <class T> UInt32 BTreeMap<T>::CalHash(Text::CStringNN key) const
	{
		return this->crc.CalcDirect(key.v, key.leng);
	}

	template <class T> void BTreeMap<T>::FillArr(InOutParam<UnsafeArray<T>> arr, NN<BTreeNode<T>> node)
	{
		UnsafeArray<T> narr;
		NN<BTreeNode<T>> nnnode;
		if (node->leftNode.SetTo(nnnode))
			FillArr(arr, nnnode);
		narr = arr.Get();
		narr[0] = node->nodeVal;
		arr.Set(narr + 1);
		if (node->rightNode.SetTo(nnnode))
			FillArr(arr, nnnode);
	}

	template <class T> void BTreeMap<T>::FillNameArr(InOutParam<UnsafeArray<NN<Text::String>>> arr, NN<BTreeNode<T>> node)
	{
		UnsafeArray<NN<Text::String>> narr;
		NN<BTreeNode<T>> nnnode;
		if (node->leftNode.SetTo(nnnode))
			FillNameArr(arr, nnnode);
		narr = arr.Get();
		narr[0] = node->nodeStr;
		arr.Set(narr + 1);
		if (node->rightNode.SetTo(nnnode))
			FillNameArr(arr, nnnode);
	}

	template <class T> BTreeMap<T>::BTreeMap() : DataMap<NN<Text::String>, T>()
	{
		rootNode = 0;
	}

	template <class T> BTreeMap<T>::~BTreeMap()
	{
		NN<BTreeNode<T>> node;
		if (this->rootNode.SetTo(node))
		{
			ReleaseNodeTree(node);
			this->rootNode = 0;
		}
	}

	template <class T> T BTreeMap<T>::Put(NN<Text::String> key, T val)
	{
		UInt32 hash = CalHash(key);
		NN<BTreeNode<T>> node;
		if (!this->rootNode.SetTo(node))
		{
			this->rootNode = NewNode(key, hash, val);
			return 0;
		}
		else
		{
			T tmpVal;
			tmpVal = PutNode(node, key, hash, val);
			return tmpVal;
		}
	}

	template <class T> T BTreeMap<T>::Get(NN<Text::String> key) const
	{
		UInt32 hash = CalHash(key);
		Optional<BTreeNode<T>> optnode = this->rootNode;
		NN<BTreeNode<T>> node;
		while (optnode.SetTo(node))
		{
			OSInt i;
			if (node->nodeHash == hash)
			{
				i = node->nodeStr->CompareToFast(key->ToCString());
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
				optnode = node->leftNode;
			}
			else if (i < 0)
			{
				optnode = node->rightNode;
			}
			else
			{
				return node->nodeVal;
			}
		}
		return 0;
	}

	template <class T> T BTreeMap<T>::Get(Text::CStringNN key) const
	{
		UInt32 hash = CalHash(key);
		Optional<BTreeNode<T>> optnode = this->rootNode;
		NN<BTreeNode<T>> node;
		while (optnode.SetTo(node))
		{
			OSInt i;
			if (node->nodeHash == hash)
			{
				i = node->nodeStr->CompareToFast(key);
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
				optnode = node->leftNode;
			}
			else if (i < 0)
			{
				optnode = node->rightNode;
			}
			else
			{
				return node->nodeVal;
			}
		}
		return 0;
	}

	template <class T> T BTreeMap<T>::Remove(NN<Text::String> key)
	{
		NN<BTreeNode<T>> node;
		if (!this->rootNode.SetTo(node))
			return 0;
		if (node->nodeStr->Equals(key))
		{
			T nodeVal = node->nodeVal;
			this->rootNode = RemoveNode(node);
			return nodeVal;
		}
		else
		{
			UInt32 hash = CalHash(key);
			Optional<BTreeNode<T>> parNode = 0;
			NN<BTreeNode<T>> nnparNode;
			Optional<BTreeNode<T>> optnode = this->rootNode;
			NN<BTreeNode<T>> node;
			while (optnode.SetTo(node))
			{
				OSInt i;
				if (node->nodeHash == hash)
				{
					i = node->nodeStr->CompareToFast(key->ToCString());
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
					optnode = node->leftNode;
				}
				else if (i < 0)
				{
					parNode = node;
					optnode = node->rightNode;
				}
				else
				{
					T nodeVal = node->nodeVal;
					if (parNode.SetTo(nnparNode))
					{
						if (nnparNode->leftNode == node)
						{
							nnparNode->leftNode = RemoveNode(node);
						}
						else
						{
							nnparNode->rightNode = RemoveNode(node);
						}
					}
					return nodeVal;
				}
			}
			return 0;
		}
	}

	template <class T> Bool BTreeMap<T>::IsEmpty() const
	{
		return this->rootNode.IsNull();
	}

	template <class T> UnsafeArray<T> BTreeMap<T>::ToArray(OutParam<UOSInt> objCnt)
	{
		UOSInt cnt = 0;
		NN<BTreeNode<T>> node;
		if (!this->rootNode.SetTo(node))
		{
			objCnt.Set(0);
			return MemAllocArr(T, 0);
		}
		cnt = node->nodeCnt + 1;
		UnsafeArray<T> outArr = MemAllocArr(T, cnt);
		UnsafeArray<T> tmpArr = outArr;
		FillArr(tmpArr, node);
		objCnt.Set(cnt);
		return outArr;
	}

	template <class T> UnsafeArray<NN<Text::String>> BTreeMap<T>::ToNameArray(OutParam<UOSInt> objCnt)
	{
		UOSInt cnt = 0;
		NN<BTreeNode<T>> node;
		if (!this->rootNode.SetTo(node))
		{
			objCnt.Set(0);
			return MemAllocArr(NN<Text::String>, 0);
		}
		cnt = node->nodeCnt + 1;
		UnsafeArray<NN<Text::String>> outArr = MemAllocArr(NN<Text::String>, cnt);
		UnsafeArray<NN<Text::String>> tmpArr = outArr;
		FillNameArr(tmpArr, node);
		objCnt.Set(cnt);
		return outArr;
	}

	template <class T>void BTreeMap<T>::Clear()
	{
		NN<BTreeNode<T>> node;
		if (this->rootNode.SetTo(node))
		{
			ReleaseNodeTree(node);
			this->rootNode = 0;
		}
	}
}

#endif
