#ifndef _SM_DATA_BTREEUTF8MAP
#define _SM_DATA_BTREEUTF8MAP
#include "MyMemory.h"
#include "Core/ByteTool_C.h"
#include "Crypto/Hash/CRC32RC.h"
#include "Data/DataMap.hpp"
#include "Text/CString.h"
#include "Text/MyString.h"

namespace Data
{
	template <typename T> struct BTreeUTF8Node
	{
		UInt32 nodeCnt;
		UInt32 maxLev;
		Optional<BTreeUTF8Node<T>> parNode;
		Optional<BTreeUTF8Node<T>> leftNode;
		Optional<BTreeUTF8Node<T>> rightNode;
		T nodeVal;
		UInt32 nodeHash;
		UOSInt keyLen;
		UTF8Char nodeKey[1];
	};

	template <class T> class BTreeUTF8Map : public DataMap<Text::CStringNN, T>
	{
	protected:
		NN<Crypto::Hash::CRC32RC> crc;
		Optional<BTreeUTF8Node<T>> rootNode;

	protected:
		void OptimizeNode(NN<BTreeUTF8Node<T>> node);
		void ReleaseNodeTree(NN<BTreeUTF8Node<T>> node);
		NN<BTreeUTF8Node<T>> NewNode(Text::CStringNN key, UInt32 hash, T val);
		virtual T PutNode(NN<BTreeUTF8Node<T>> node, Text::CStringNN key, UInt32 hash, T val);
		Optional<BTreeUTF8Node<T>> RemoveNode(NN<BTreeUTF8Node<T>> node);
		void FillArr(UnsafeArray<T> *arr, Optional<BTreeUTF8Node<T>> node);
		void FillNameArr(UnsafeArray<Text::CStringNN> *arr, Optional<BTreeUTF8Node<T>> node);
		virtual UInt32 CalHash(UnsafeArray<const UTF8Char> key, UOSInt keyLen) const;
	public:
		BTreeUTF8Map();
		virtual ~BTreeUTF8Map();

		virtual T Put(Text::CStringNN key, T val);
		virtual T Get(Text::CStringNN key) const;
		virtual T Remove(Text::CStringNN key);
		virtual Bool IsEmpty() const;
		virtual UnsafeArray<T> ToArray(OutParam<UOSInt> objCnt);
		virtual UnsafeArray<Text::CStringNN> ToNameArray(OutParam<UOSInt> objCnt);
		virtual void Clear();
	};

	template <class T> void BTreeUTF8Map<T>::OptimizeNode(NN<BTreeUTF8Node<T>> node)
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

	template <class T> void BTreeUTF8Map<T>::ReleaseNodeTree(NN<BTreeUTF8Node<T>> node)
	{
		NN<BTreeUTF8Node<T>> nnnode;
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

	template <class T> NN<BTreeUTF8Node<T>> BTreeUTF8Map<T>::NewNode(Text::CStringNN key, UInt32 hash, T val)
	{
		BTreeUTF8Node<T> *node = (BTreeUTF8Node<T> *)MAlloc(sizeof(BTreeUTF8Node<T>) + sizeof(UTF8Char) * key.leng);
		node->nodeCnt = 0;
		node->parNode = 0;
		node->leftNode = 0;
		node->rightNode = 0;
		node->nodeVal = val;
		node->nodeHash = hash;
		node->maxLev = 0;
		node->keyLen = key.leng;
		key.ConcatTo(UnsafeArray<UTF8Char>::FromPtrNoCheck(node->nodeKey));
		return NN<BTreeUTF8Node<T>>::FromPtr(node);
	}

	template <class T> T BTreeUTF8Map<T>::PutNode(NN<BTreeUTF8Node<T>> node, Text::CStringNN key, UInt32 hash, T val)
	{
		NN<BTreeUTF8Node<T>> tmpNode;
		NN<BTreeUTF8Node<T>> leftNode;
		NN<BTreeUTF8Node<T>> rightNode;
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
				return 0;
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
				return 0;
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

	template <class T> Optional<BTreeUTF8Node<T>> BTreeUTF8Map<T>::RemoveNode(NN<BTreeUTF8Node<T>> node)
	{
		Optional<BTreeUTF8Node<T>> parNode = node->parNode;
		NN<BTreeUTF8Node<T>> nnparNode;
		while (parNode.SetTo(nnparNode))
		{
			nnparNode->nodeCnt--;
			parNode = nnparNode->parNode;
		}
		NN<BTreeUTF8Node<T>> leftNode;
		NN<BTreeUTF8Node<T>> rightNode;
		if (!node->leftNode.SetTo(leftNode) && !node->rightNode.SetTo(rightNode))
		{
			MemFreeNN(node);
			return 0;
		}
		else if (node->rightNode.SetTo(rightNode) && node->leftNode.IsNull())
		{
			NN<BTreeUTF8Node<T>> outNode = rightNode;
			outNode->parNode = node->parNode;
			MemFreeNN(node);
			return outNode;
		}
		else if (node->leftNode.SetTo(leftNode) && node->rightNode.IsNull())
		{
			NN<BTreeUTF8Node<T>> outNode = leftNode;
			outNode->parNode = node->parNode;
			MemFreeNN(node);
			return outNode;
		}

		if (leftNode->nodeCnt >= rightNode->nodeCnt)
		{
			NN<BTreeUTF8Node<T>> outNode = leftNode;
			Optional<BTreeUTF8Node<T>> tmpNode;
			NN<BTreeUTF8Node<T>> nntmpNode;
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
			NN<BTreeUTF8Node<T>> outNode = rightNode;
			Optional<BTreeUTF8Node<T>> tmpNode;
			NN<BTreeUTF8Node<T>> nntmpNode;
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

	template <class T> UInt32 BTreeUTF8Map<T>::CalHash(UnsafeArray<const UTF8Char> key, UOSInt keyLen) const
	{
		return this->crc->CalcDirect(key, keyLen);
	}

	template <class T> void BTreeUTF8Map<T>::FillArr(UnsafeArray<T> *arr, Optional<BTreeUTF8Node<T>> node)
	{
		NN<BTreeUTF8Node<T>> nnnode;
		if (!node.SetTo(nnnode))
			return;
		FillArr(arr, nnnode->leftNode);
		**arr = nnnode->nodeVal;
		++*arr;
		FillArr(arr, nnnode->rightNode);
	}

	template <class T> void BTreeUTF8Map<T>::FillNameArr(UnsafeArray<Text::CStringNN> *arr, Optional<BTreeUTF8Node<T>> node)
	{
		NN<BTreeUTF8Node<T>> nnnode;
		if (!node.SetTo(nnnode))
			return;
		FillNameArr(arr, nnnode->leftNode);
		**arr = {nnnode->nodeKey, nnnode->keyLen};
		++*arr;
		FillNameArr(arr, nnnode->rightNode);
	}

	template <class T> BTreeUTF8Map<T>::BTreeUTF8Map() : DataMap<Text::CStringNN, T>()
	{
		rootNode = 0;
		NEW_CLASSNN(crc, Crypto::Hash::CRC32RC());
	}

	template <class T> BTreeUTF8Map<T>::~BTreeUTF8Map()
	{
		NN<BTreeUTF8Node<T>> rootNode;
		if (this->rootNode.SetTo(rootNode))
		{
			ReleaseNodeTree(rootNode);
			this->rootNode = 0;
		}
		crc.Delete();
	}

	template <class T> T BTreeUTF8Map<T>::Put(Text::CStringNN key, T val)
	{
		NN<BTreeUTF8Node<T>> rootNode;
		UInt32 hash = CalHash(key.v, key.leng);
		if (!this->rootNode.SetTo(rootNode))
		{
			this->rootNode = NewNode(key, hash, val);
			return 0;
		}
		else
		{
			T tmpVal;
			tmpVal = PutNode(rootNode, key, hash, val);
			return tmpVal;
		}
	}

	template <class T> T BTreeUTF8Map<T>::Get(Text::CStringNN key) const
	{
		UInt32 hash = CalHash(key.v, key.leng);
		Optional<BTreeUTF8Node<T>> node = this->rootNode;
		NN<BTreeUTF8Node<T>> nnnode;
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
		return 0;
	}

	template <class T> T BTreeUTF8Map<T>::Remove(Text::CStringNN key)
	{
		NN<BTreeUTF8Node<T>> rootNode;
		if (!this->rootNode.SetTo(rootNode))
			return 0;
		if (Text::StrEqualsC(rootNode->nodeKey, rootNode->keyLen, key.v, key.leng))
		{
			T nodeVal = rootNode->nodeVal;
			this->rootNode = RemoveNode(rootNode);
			return nodeVal;
		}
		else
		{
			UInt32 hash = CalHash(key.v, key.leng);
			Optional<BTreeUTF8Node<T>> node = rootNode;
			NN<BTreeUTF8Node<T>> nnnode;
			NN<BTreeUTF8Node<T>> parNode = rootNode;
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
			return 0;
		}
	}

	template <class T> Bool BTreeUTF8Map<T>::IsEmpty() const
	{
		return this->rootNode.IsNull();
	}

	template <class T> UnsafeArray<T> BTreeUTF8Map<T>::ToArray(OutParam<UOSInt> objCnt)
	{
		NN<BTreeUTF8Node<T>> rootNode;
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

	template <class T> UnsafeArray<Text::CStringNN> BTreeUTF8Map<T>::ToNameArray(OutParam<UOSInt> objCnt)
	{
		UOSInt cnt = 0;
		NN<BTreeUTF8Node<T>> rootNode;
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

	template <class T>void BTreeUTF8Map<T>::Clear()
	{
		NN<BTreeUTF8Node<T>> rootNode;
		if (this->rootNode.SetTo(rootNode))
		{
			ReleaseNodeTree(rootNode);
			this->rootNode = 0;
		}
	}
}

#endif
