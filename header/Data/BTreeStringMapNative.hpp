#ifndef _SM_DATA_BTREESTRINGMAPNATIVE
#define _SM_DATA_BTREESTRINGMAPNATIVE
#include "MyMemory.h"
#include "Core/ByteTool_C.h"
#include "Crypto/Hash/CRC32RC.h"
#include "Data/DataMap.hpp"
#include "Text/CString.h"
#include "Text/MyString.h"

namespace Data
{
	template <typename T> struct BTreeStringNativeNode
	{
		UInt32 nodeCnt;
		UInt32 maxLev;
		Optional<BTreeStringNativeNode<T>> parNode;
		Optional<BTreeStringNativeNode<T>> leftNode;
		Optional<BTreeStringNativeNode<T>> rightNode;
		T nodeVal;
		UInt32 nodeHash;
		UOSInt keyLen;
		UTF8Char nodeKey[1];
	};

	template <class T> class BTreeStringMapNative : public DataMap<Text::CStringNN, T>
	{
	protected:
		NN<Crypto::Hash::CRC32RC> crc;
		Optional<BTreeStringNativeNode<T>> rootNode;

	protected:
		void OptimizeNode(NN<BTreeStringNativeNode<T>> node);
		void ReleaseNodeTree(NN<BTreeStringNativeNode<T>> node);
		NN<BTreeStringNativeNode<T>> NewNode(Text::CStringNN key, UInt32 hash, T val);
		virtual T PutNode(NN<BTreeStringNativeNode<T>> node, Text::CStringNN key, UInt32 hash, T val);
		Optional<BTreeStringNativeNode<T>> RemoveNode(NN<BTreeStringNativeNode<T>> node);
		void FillArr(UnsafeArray<T> *arr, Optional<BTreeStringNativeNode<T>> node);
		void FillNameArr(UnsafeArray<Text::CStringNN> *arr, Optional<BTreeStringNativeNode<T>> node);
		virtual UInt32 CalHash(UnsafeArray<const UTF8Char> key, UOSInt keyLen) const;
	public:
		BTreeStringMapNative();
		virtual ~BTreeStringMapNative();

		virtual T Put(Text::CStringNN key, T val);
		virtual T Get(Text::CStringNN key) const;
		virtual T Remove(Text::CStringNN key);
		virtual Bool IsEmpty() const;
		virtual UnsafeArray<T> ToArray(OutParam<UOSInt> objCnt);
		virtual UnsafeArray<Text::CStringNN> ToNameArray(OutParam<UOSInt> objCnt);
		virtual void Clear();
	};

	template <class T> void BTreeStringMapNative<T>::OptimizeNode(NN<BTreeStringNativeNode<T>> node)
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

	template <class T> void BTreeStringMapNative<T>::ReleaseNodeTree(NN<BTreeStringNativeNode<T>> node)
	{
		NN<BTreeStringNativeNode<T>> nnnode;
		if (node->leftNode.SetTo(nnnode))
		{
			ReleaseNodeTree(nnnode);
		}
		if (node->rightNode.SetTo(nnnode))
		{
			ReleaseNodeTree(nnnode);
		}
		MemFreeNN(node);
	}

	template <class T> NN<BTreeStringNativeNode<T>> BTreeStringMapNative<T>::NewNode(Text::CStringNN key, UInt32 hash, T val)
	{
		BTreeStringNativeNode<T> *node = (BTreeStringNativeNode<T> *)MAlloc(sizeof(BTreeStringNativeNode<T>) + sizeof(UTF8Char) * key.leng);
		node->nodeCnt = 0;
		node->parNode = nullptr;
		node->leftNode = nullptr;
		node->rightNode = nullptr;
		node->nodeVal = val;
		node->nodeHash = hash;
		node->maxLev = 0;
		node->keyLen = key.leng;
		key.ConcatTo(UnsafeArray<UTF8Char>::FromPtrNoCheck(node->nodeKey));
		return NN<BTreeStringNativeNode<T>>::FromPtr(node);
	}

	template <class T> T BTreeStringMapNative<T>::PutNode(NN<BTreeStringNativeNode<T>> node, Text::CStringNN key, UInt32 hash, T val)
	{
		NN<BTreeStringNativeNode<T>> tmpNode;
		NN<BTreeStringNativeNode<T>> leftNode;
		NN<BTreeStringNativeNode<T>> rightNode;
		T retVal;
		OSInt i;
		if (node->nodeHash == hash)
		{
			i = Text::StrCompareFastC(node->nodeKey, node->keyLen, key.v, key.leng);
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
				node->leftNode = leftNode = NewNode(key, hash, val);
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
				return (T)0;
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
				node->rightNode = rightNode = NewNode(key, hash, val);
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
				return (T)0;
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

	template <class T> Optional<BTreeStringNativeNode<T>> BTreeStringMapNative<T>::RemoveNode(NN<BTreeStringNativeNode<T>> node)
	{
		Optional<BTreeStringNativeNode<T>> parNode = node->parNode;
		NN<BTreeStringNativeNode<T>> nnparNode;
		while (parNode.SetTo(nnparNode))
		{
			nnparNode->nodeCnt--;
			parNode = nnparNode->parNode;
		}
		NN<BTreeStringNativeNode<T>> leftNode;
		NN<BTreeStringNativeNode<T>> rightNode;
		if (!node->leftNode.SetTo(leftNode) && !node->rightNode.SetTo(rightNode))
		{
			MemFreeNN(node);
			return nullptr;
		}
		else if (node->rightNode.SetTo(rightNode) && node->leftNode.IsNull())
		{
			NN<BTreeStringNativeNode<T>> outNode = rightNode;
			outNode->parNode = node->parNode;
			MemFreeNN(node);
			return outNode;
		}
		else if (node->leftNode.SetTo(leftNode) && node->rightNode.IsNull())
		{
			NN<BTreeStringNativeNode<T>> outNode = leftNode;
			outNode->parNode = node->parNode;
			MemFreeNN(node);
			return outNode;
		}

		if (leftNode->nodeCnt >= rightNode->nodeCnt)
		{
			NN<BTreeStringNativeNode<T>> outNode = leftNode;
			Optional<BTreeStringNativeNode<T>> tmpNode;
			NN<BTreeStringNativeNode<T>> nntmpNode;
			while (outNode->rightNode.SetTo(outNode))
			{
			}
			tmpNode = outNode->parNode;
			while (tmpNode != node && tmpNode.SetTo(nntmpNode))
			{
				nntmpNode->nodeCnt--;
				tmpNode = nntmpNode->parNode;
			}
			if (outNode->leftNode.SetTo(leftNode) && outNode->parNode.SetTo(nntmpNode))
			{
				if (nntmpNode->leftNode == outNode)
				{
					nntmpNode->leftNode = leftNode;
				}
				else
				{
					nntmpNode->rightNode = leftNode;
				}
				leftNode->parNode = nntmpNode;
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
			MemFreeNN(node);
			return outNode;
		}
		else
		{
			NN<BTreeStringNativeNode<T>> outNode = rightNode;
			Optional<BTreeStringNativeNode<T>> tmpNode;
			NN<BTreeStringNativeNode<T>> nntmpNode;
			while (outNode->leftNode.SetTo(outNode))
			{
			}
			tmpNode = outNode->parNode;
			while (tmpNode != node && tmpNode.SetTo(nntmpNode))
			{
				nntmpNode->nodeCnt--;
				tmpNode = nntmpNode->parNode;
			}
			if (outNode->rightNode.SetTo(rightNode) && outNode->parNode.SetTo(nntmpNode))
			{
				if (nntmpNode->leftNode == outNode)
				{
					nntmpNode->leftNode = rightNode;
				}
				else
				{
					nntmpNode->rightNode = rightNode;
				}
				rightNode->parNode = nntmpNode;
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
			MemFreeNN(node);
			return outNode;
		}
	}

	template <class T> UInt32 BTreeStringMapNative<T>::CalHash(UnsafeArray<const UTF8Char> key, UOSInt keyLen) const
	{
		return this->crc->CalcDirect(key, keyLen);
	}

	template <class T> void BTreeStringMapNative<T>::FillArr(UnsafeArray<T> *arr, Optional<BTreeStringNativeNode<T>> node)
	{
		NN<BTreeStringNativeNode<T>> nnnode;
		if (!node.SetTo(nnnode))
			return;
		FillArr(arr, nnnode->leftNode);
		**arr = nnnode->nodeVal;
		++*arr;
		FillArr(arr, nnnode->rightNode);
	}

	template <class T> void BTreeStringMapNative<T>::FillNameArr(UnsafeArray<Text::CStringNN> *arr, Optional<BTreeStringNativeNode<T>> node)
	{
		NN<BTreeStringNativeNode<T>> nnnode;
		if (!node.SetTo(nnnode))
			return;
		FillNameArr(arr, nnnode->leftNode);
		**arr = {nnnode->nodeKey, nnnode->keyLen};
		++*arr;
		FillNameArr(arr, nnnode->rightNode);
	}

	template <class T> BTreeStringMapNative<T>::BTreeStringMapNative() : DataMap<Text::CStringNN, T>()
	{
		rootNode = nullptr;
		NEW_CLASSNN(crc, Crypto::Hash::CRC32RC());
	}

	template <class T> BTreeStringMapNative<T>::~BTreeStringMapNative()
	{
		NN<BTreeStringNativeNode<T>> rootNode;
		if (this->rootNode.SetTo(rootNode))
		{
			ReleaseNodeTree(rootNode);
			this->rootNode = nullptr;
		}
		crc.Delete();
	}

	template <class T> T BTreeStringMapNative<T>::Put(Text::CStringNN key, T val)
	{
		NN<BTreeStringNativeNode<T>> rootNode;
		UInt32 hash = CalHash(key.v, key.leng);
		if (!this->rootNode.SetTo(rootNode))
		{
			this->rootNode = NewNode(key, hash, val);
			return (T)0;
		}
		else
		{
			T tmpVal;
			tmpVal = PutNode(rootNode, key, hash, val);
			return tmpVal;
		}
	}

	template <class T> T BTreeStringMapNative<T>::Get(Text::CStringNN key) const
	{
		UInt32 hash = CalHash(key.v, key.leng);
		Optional<BTreeStringNativeNode<T>> node = this->rootNode;
		NN<BTreeStringNativeNode<T>> nnnode;
		while (node.SetTo(nnnode))
		{
			OSInt i;
			if (nnnode->nodeHash == hash)
			{
				i = Text::StrCompareFastC(nnnode->nodeKey, nnnode->keyLen, key.v, key.leng);
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
		return (T)0;
	}

	template <class T> T BTreeStringMapNative<T>::Remove(Text::CStringNN key)
	{
		NN<BTreeStringNativeNode<T>> rootNode;
		if (!this->rootNode.SetTo(rootNode))
			return (T)0;
		if (Text::StrEqualsC(rootNode->nodeKey, rootNode->keyLen, key.v, key.leng))
		{
			T nodeVal = rootNode->nodeVal;
			this->rootNode = RemoveNode(rootNode);
			return nodeVal;
		}
		else
		{
			UInt32 hash = CalHash(key.v, key.leng);
			Optional<BTreeStringNativeNode<T>> node = rootNode;
			NN<BTreeStringNativeNode<T>> nnnode;
			NN<BTreeStringNativeNode<T>> parNode = rootNode;
			while (node.SetTo(nnnode))
			{
				OSInt i;
				if (nnnode->nodeHash == hash)
				{
					i = Text::StrCompareFastC(nnnode->nodeKey, nnnode->keyLen, key.v, key.leng);
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
						parNode->leftNode = RemoveNode(nnnode);
					}
					else
					{
						parNode->rightNode = RemoveNode(nnnode);
					}
					return nodeVal;
				}
			}
			return (T)0;
		}
	}

	template <class T> Bool BTreeStringMapNative<T>::IsEmpty() const
	{
		return this->rootNode.IsNull();
	}

	template <class T> UnsafeArray<T> BTreeStringMapNative<T>::ToArray(OutParam<UOSInt> objCnt)
	{
		NN<BTreeStringNativeNode<T>> rootNode;
		UOSInt cnt = 0;
		if (this->rootNode.SetTo(rootNode))
		{
			cnt = (UOSInt)rootNode->nodeCnt + 1;
		}
		UnsafeArray<T> outArr = MemAllocArr(T, cnt);
		UnsafeArray<T> tmpArr = outArr;
		FillArr(&tmpArr, this->rootNode);
		objCnt.Set(cnt);
		return outArr;
	}

	template <class T> UnsafeArray<Text::CStringNN> BTreeStringMapNative<T>::ToNameArray(OutParam<UOSInt> objCnt)
	{
		UOSInt cnt = 0;
		NN<BTreeStringNativeNode<T>> rootNode;
		if (this->rootNode.SetTo(rootNode))
		{
			cnt = (UOSInt)rootNode->nodeCnt + 1;
		}
		UnsafeArray<Text::CStringNN> outArr = MemAllocArr(Text::CStringNN, cnt);
		UnsafeArray<Text::CStringNN> tmpArr = outArr;
		FillNameArr(&tmpArr, this->rootNode);
		objCnt.Set(cnt);
		return outArr;
	}

	template <class T>void BTreeStringMapNative<T>::Clear()
	{
		NN<BTreeStringNativeNode<T>> rootNode;
		if (this->rootNode.SetTo(rootNode))
		{
			ReleaseNodeTree(rootNode);
			this->rootNode = nullptr;
		}
	}
}

#endif
