#ifndef _SM_DATA_BTREEMAPOBJ
#define _SM_DATA_BTREEMAPOBJ
#include "MyMemory.h"
#include "Crypto/Hash/CRC32RC.h"
#include "Data/DataMap.hpp"
#include "Text/MyString.h"
#include "Text/String.h"

namespace Data
{
	template <typename T> struct BTreeObjNode
	{
		UInt32 nodeCnt;
		UInt32 maxLev;
		Optional<BTreeObjNode<T>> parNode;
		Optional<BTreeObjNode<T>> leftNode;
		Optional<BTreeObjNode<T>> rightNode;
		T nodeVal;
		NN<Text::String> nodeStr;
		UInt32 nodeHash;
	};

	template <class T> class BTreeMapObj : public DataMap<NN<Text::String>, T>
	{
	protected:
		Crypto::Hash::CRC32RC crc;
		Optional<BTreeObjNode<T>> rootNode;

	protected:
		void OptimizeNode(NN<BTreeObjNode<T>> node);
		void ReleaseNodeTree(NN<BTreeObjNode<T>> node);
		NN<BTreeObjNode<T>> NewNode(NN<Text::String> key, UInt32 hash, T val);
		virtual T PutNode(NN<BTreeObjNode<T>> node, NN<Text::String> key, UInt32 hash, T val);
		Optional<BTreeObjNode<T>> RemoveNode(NN<BTreeObjNode<T>> node);
		void FillArr(InOutParam<UnsafeArray<T>> arr, NN<BTreeObjNode<T>> node);
		void FillNameArr(InOutParam<UnsafeArray<NN<Text::String>>> arr, NN<BTreeObjNode<T>> node);
		virtual UInt32 CalHash(NN<Text::String> key) const;
		UInt32 CalHash(Text::CStringNN key) const;
	public:
		BTreeMapObj();
		virtual ~BTreeMapObj();

		virtual T Put(NN<Text::String> key, T val);
		virtual T Get(NN<Text::String> key) const;
		T Get(Text::CStringNN key) const;
		virtual T Remove(NN<Text::String> key);
		virtual Bool IsEmpty() const;
		virtual UnsafeArray<T> ToArray(OutParam<UOSInt> objCnt);
		virtual UnsafeArray<NN<Text::String>> ToNameArray(OutParam<UOSInt> objCnt);
		virtual void Clear();
	};

	template <class T> void BTreeMapObj<T>::OptimizeNode(NN<BTreeObjNode<T>> node)
	{
		Int32 leftLev;
		Int32 rightLev;
		NN<BTreeObjNode<T>> nnnode;
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

	template <class T> void BTreeMapObj<T>::ReleaseNodeTree(NN<BTreeObjNode<T>> node)
	{
		NN<BTreeObjNode<T>> nnnode;
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

	template <class T> NN<BTreeObjNode<T>> BTreeMapObj<T>::NewNode(NN<Text::String> key, UInt32 hash, T val)
	{
		NN<BTreeObjNode<T>> node = MemAllocNN(BTreeObjNode<T>);
		node->nodeCnt = 0;
		node->parNode = nullptr;
		node->leftNode = nullptr;
		node->rightNode = nullptr;
		node->nodeVal = val;
		node->nodeHash = hash;
		node->maxLev = 0;
		node->nodeStr = key->Clone();
		return node;
	}

	template <class T> T BTreeMapObj<T>::PutNode(NN<BTreeObjNode<T>> node, NN<Text::String> key, UInt32 hash, T val)
	{
		NN<BTreeObjNode<T>> tmpNode;
		NN<BTreeObjNode<T>> nnnode;
		Optional<BTreeObjNode<T>> optnode;
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

	template <class T> Optional<BTreeObjNode<T>> BTreeMapObj<T>::RemoveNode(NN<BTreeObjNode<T>> node)
	{
		Optional<BTreeObjNode<T>> parNode = node->parNode;
		NN<BTreeObjNode<T>> leftNode;
		NN<BTreeObjNode<T>> rightNode;
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
				return nullptr;
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
			NN<BTreeObjNode<T>> outNode = leftNode;
			Optional<BTreeObjNode<T>> tmpNode;
			NN<BTreeObjNode<T>> nnnode;
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
			NN<BTreeObjNode<T>> outNode = rightNode;
			Optional<BTreeObjNode<T>> tmpNode;
			NN<BTreeObjNode<T>> nnnode;
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

	template <class T> UInt32 BTreeMapObj<T>::CalHash(NN<Text::String> key) const
	{
		return this->crc.CalcDirect(key->v, key->leng);
	}

	template <class T> UInt32 BTreeMapObj<T>::CalHash(Text::CStringNN key) const
	{
		return this->crc.CalcDirect(key.v, key.leng);
	}

	template <class T> void BTreeMapObj<T>::FillArr(InOutParam<UnsafeArray<T>> arr, NN<BTreeObjNode<T>> node)
	{
		UnsafeArray<T> narr;
		NN<BTreeObjNode<T>> nnnode;
		if (node->leftNode.SetTo(nnnode))
			FillArr(arr, nnnode);
		narr = arr.Get();
		narr[0] = node->nodeVal;
		arr.Set(narr + 1);
		if (node->rightNode.SetTo(nnnode))
			FillArr(arr, nnnode);
	}

	template <class T> void BTreeMapObj<T>::FillNameArr(InOutParam<UnsafeArray<NN<Text::String>>> arr, NN<BTreeObjNode<T>> node)
	{
		UnsafeArray<NN<Text::String>> narr;
		NN<BTreeObjNode<T>> nnnode;
		if (node->leftNode.SetTo(nnnode))
			FillNameArr(arr, nnnode);
		narr = arr.Get();
		narr[0] = node->nodeStr;
		arr.Set(narr + 1);
		if (node->rightNode.SetTo(nnnode))
			FillNameArr(arr, nnnode);
	}

	template <class T> BTreeMapObj<T>::BTreeMapObj() : DataMap<NN<Text::String>, T>()
	{
		this->rootNode = nullptr;
	}

	template <class T> BTreeMapObj<T>::~BTreeMapObj()
	{
		NN<BTreeObjNode<T>> node;
		if (this->rootNode.SetTo(node))
		{
			ReleaseNodeTree(node);
			this->rootNode = nullptr;
		}
	}

	template <class T> T BTreeMapObj<T>::Put(NN<Text::String> key, T val)
	{
		UInt32 hash = CalHash(key);
		NN<BTreeObjNode<T>> node;
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

	template <class T> T BTreeMapObj<T>::Get(NN<Text::String> key) const
	{
		UInt32 hash = CalHash(key);
		Optional<BTreeObjNode<T>> optnode = this->rootNode;
		NN<BTreeObjNode<T>> node;
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

	template <class T> T BTreeMapObj<T>::Get(Text::CStringNN key) const
	{
		UInt32 hash = CalHash(key);
		Optional<BTreeObjNode<T>> optnode = this->rootNode;
		NN<BTreeObjNode<T>> node;
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

	template <class T> T BTreeMapObj<T>::Remove(NN<Text::String> key)
	{
		NN<BTreeObjNode<T>> node;
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
			Optional<BTreeObjNode<T>> parNode = nullptr;
			NN<BTreeObjNode<T>> nnparNode;
			Optional<BTreeObjNode<T>> optnode = this->rootNode;
			NN<BTreeObjNode<T>> node;
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

	template <class T> Bool BTreeMapObj<T>::IsEmpty() const
	{
		return this->rootNode.IsNull();
	}

	template <class T> UnsafeArray<T> BTreeMapObj<T>::ToArray(OutParam<UOSInt> objCnt)
	{
		UOSInt cnt = 0;
		NN<BTreeObjNode<T>> node;
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

	template <class T> UnsafeArray<NN<Text::String>> BTreeMapObj<T>::ToNameArray(OutParam<UOSInt> objCnt)
	{
		UOSInt cnt = 0;
		NN<BTreeObjNode<T>> node;
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

	template <class T>void BTreeMapObj<T>::Clear()
	{
		NN<BTreeObjNode<T>> node;
		if (this->rootNode.SetTo(node))
		{
			ReleaseNodeTree(node);
			this->rootNode = nullptr;
		}
	}
}

#endif
