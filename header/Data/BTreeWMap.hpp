#ifndef _SM_DATA_BTREEWMAP
#define _SM_DATA_BTREEWMAP
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Data/DataMap.hpp"
#include "Crypto/Hash/CRC32R.h"

namespace Data
{
	template <typename T> struct BTreeWNode
	{
		Int32 nodeCnt;
		Int32 maxLev;
		Optional<BTreeWNode<T>> parNode;
		Optional<BTreeWNode<T>> leftNode;
		Optional<BTreeWNode<T>> rightNode;
		T nodeVal;
		UInt32 nodeHash;
		WChar nodeStr[1];
	};

	template <class T> class BTreeWMap : public DataMap<UnsafeArray<const WChar>, T>
	{
	protected:
		NN<Crypto::Hash::CRC32R> crc;
		Optional<BTreeWNode<T>> rootNode;

	protected:
		void OptimizeNode(NN<BTreeWNode<T>> node);
		void ReleaseNodeTree(NN<BTreeWNode<T>> node);
		NN<BTreeWNode<T>> NewNode(UnsafeArray<const WChar> key, UInt32 hash, T val);
		virtual T PutNode(NN<BTreeWNode<T>> node, UnsafeArray<const WChar> key, UInt32 hash, T val);
		Optional<BTreeWNode<T>> RemoveNode(NN<BTreeWNode<T>> node);
		void FillArr(InOutParam<UnsafeArray<T>> arr, NN<BTreeWNode<T>> node);
		virtual UInt32 CalHash(UnsafeArray<const WChar> key);
	public:
		BTreeWMap();
		virtual ~BTreeWMap();

		virtual T Put(UnsafeArray<const WChar> key, T val);
		virtual T Get(UnsafeArray<const WChar> key);
		virtual T Remove(UnsafeArray<const WChar> key);
		virtual Bool IsEmpty();
		virtual UnsafeArray<T> ToArray(OutParam<UIntOS> objCnt);
		virtual void Clear();
	};

	template <class T> void BTreeWMap<T>::OptimizeNode(NN<BTreeWNode<T>> node)
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

	template <class T> void BTreeWMap<T>::ReleaseNodeTree(NN<BTreeWNode<T>> node)
	{
		if (node->leftNode)
		{
			ReleaseNodeTree(node->leftNode);
		}
		if (node->rightNode)
		{
			ReleaseNodeTree(node->rightNode);
		}
		MemFreeNN(node);
	}

	template <class T> NN<BTreeWNode<T>> BTreeWMap<T>::NewNode(UnsafeArray<const WChar> key, UInt32 hash, T val)
	{
		IntOS cnt = Text::StrCharCnt(key);
		NN<BTreeWNode<T>> node = NN<BTreeWNode<T>>::FromPtr((BTreeWNode<T>*)MAlloc(sizeof(BTreeWNode<T>) + sizeof(WChar) * cnt));
		node->nodeCnt = 0;
		node->parNode = 0;
		node->leftNode = 0;
		node->rightNode = 0;
		node->nodeVal = val;
		node->nodeHash = hash;
		node->maxLev = 0;
		Text::StrConcat(node->nodeStr, key);
		return node;
	}

	template <class T> T BTreeWMap<T>::PutNode(NN<BTreeWNode<T>> node, UnsafeArray<const WChar> key, UInt32 hash, T val)
	{
		NN<BTreeWNode<T>> tmpNode;
		T retVal;
		IntOS i;
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
				node->rightNode = NewNode(key, hash, val);
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

	template <class T> Optional<BTreeWNode<T>> BTreeWMap<T>::RemoveNode(NN<BTreeWNode<T>> node)
	{
		Optional<BTreeWNode<T>> parNode = node->parNode;
		NN<BTreeWNode<T>> nnparNode;
		while (parNode.SetTo(nnparNode))
		{
			nnparNode->nodeCnt--;
			parNode = nnparNode->parNode;
		}
		if (node->leftNode == 0 && node->rightNode == 0)
		{
			MemFreeNN(node);
			return 0;
		}
		else if (node->leftNode == 0)
		{
			BTreeWNode<T> *outNode = node->rightNode;
			outNode->parNode = node->parNode;
			MemFreeNN(node);
			return outNode;
		}
		else if (node->rightNode == 0)
		{
			BTreeWNode<T> *outNode = node->leftNode;
			outNode->parNode = node->parNode;
			MemFreeNN(node);
			return outNode;
		}

		if (node->leftNode->nodeCnt >= node->rightNode->nodeCnt)
		{
			BTreeWNode<T> *outNode = node->leftNode;
			BTreeWNode<T> *tmpNode;
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
			MemFree(node);
			return outNode;
		}
	}

	template <class T> UInt32 BTreeWMap<T>::CalHash(UnsafeArray<const WChar> key)
	{
		IntOS charCnt = Text::StrCharCnt(key);
		this->crc->Clear();
		this->crc->Calc(UnsafeArray<const UInt8>::ConvertFrom(key), charCnt * sizeof(WChar));
		UInt8 hash[4];
		this->crc->GetValue(hash);
		return ReadMUInt32(hash);
	}

	template <class T> void BTreeWMap<T>::FillArr(InOutParam<UnsafeArray<T>> arr, NN<BTreeWNode<T>> node)
	{
		NN<BTreeWNode<T>> node2;
		if (node->leftNode.)
		FillArr(arr, node->leftNode);
		**arr = node->nodeVal;
		++*arr;
		FillArr(arr, node->rightNode);
	}

	template <class T> BTreeWMap<T>::BTreeWMap() : IMap<const WChar*, T>()
	{
		rootNode = 0;
		NEW_CLASSNN(crc, Crypto::Hash::CRC32R(Crypto::Hash::CRC32R::GetPolynormialIEEE()));
	}

	template <class T> BTreeWMap<T>::~BTreeWMap()
	{
		if (this->rootNode)
		{
			ReleaseNodeTree(this->rootNode);
			this->rootNode = 0;
		}
		this->crc.Delete();
	}

	template <class T> T BTreeWMap<T>::Put(UnsafeArray<const WChar> key, T val)
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

	template <class T> T BTreeWMap<T>::Get(UnsafeArray<const WChar> key)
	{
		UInt32 hash = CalHash(key);
		Optional<BTreeWNode<T>> node = this->rootNode;
		while (node)
		{
			IntOS i;
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

	template <class T> T BTreeWMap<T>::Remove(UnsafeArray<const WChar> key)
	{
		if (this->rootNode == 0)
			return 0;
		if (Text::StrCompare(this->rootNode->nodeStr, key) == 0)
		{
			T nodeVal = this->rootNode->nodeVal;
			this->rootNode = RemoveNode(this->rootNode);
			return nodeVal;
		}
		else
		{
			UInt32 hash = CalHash(key);
			BTreeWNode<T> *parNode = 0;
			BTreeWNode<T> *node = this->rootNode;
			while (node)
			{
				IntOS i;
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

	template <class T> Bool BTreeWMap<T>::IsEmpty()
	{
		return this->rootNode == 0;
	}

	template <class T> UnsafeArray<T> BTreeWMap<T>::ToArray(OutParam<UIntOS> objCnt)
	{
		UIntOS cnt = 0;
		if (this->rootNode)
		{
			cnt = this->rootNode->nodeCnt + 1;
		}
		UnsafeArray<T> outArr = MemAllocArr(T, cnt);
		UnsafeArray<T> tmpArr = outArr;
		FillArr(tmpArr, this->rootNode);
		objCnt.Set(cnt);
		return outArr;
	}

	template <class T>void BTreeWMap<T>::Clear()
	{
		if (this->rootNode)
		{
			ReleaseNodeTree(this->rootNode);
			this->rootNode = 0;
		}
	}
}

#endif
