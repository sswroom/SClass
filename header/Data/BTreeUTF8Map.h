#ifndef _SM_DATA_BTREEUTF8MAP
#define _SM_DATA_BTREEUTF8MAP
#include "MyMemory.h"
#include "Crypto/Hash/CRC32RC.h"
#include "Data/ByteTool.h"
#include "Data/DataMap.h"
#include "Text/CString.h"
#include "Text/MyString.h"

namespace Data
{
	template <typename T> struct BTreeUTF8Node
	{
		UInt32 nodeCnt;
		UInt32 maxLev;
		BTreeUTF8Node<T> *parNode;
		BTreeUTF8Node<T> *leftNode;
		BTreeUTF8Node<T> *rightNode;
		T nodeVal;
		UInt32 nodeHash;
		UOSInt keyLen;
		UTF8Char nodeKey[1];
	};

	template <class T> class BTreeUTF8Map : public DataMap<Text::CString, T>
	{
	protected:
		Crypto::Hash::CRC32RC *crc;
		BTreeUTF8Node<T> *rootNode;

	protected:
		void OptimizeNode(BTreeUTF8Node<T> *node);
		void ReleaseNodeTree(BTreeUTF8Node<T> *node);
		BTreeUTF8Node<T> *NewNode(Text::CString key, UInt32 hash, T val);
		virtual T PutNode(BTreeUTF8Node<T> *node, Text::CString key, UInt32 hash, T val);
		BTreeUTF8Node<T> *RemoveNode(BTreeUTF8Node<T> *node);
		void FillArr(T **arr, BTreeUTF8Node<T> *node);
		void FillNameArr(Text::CString **arr, BTreeUTF8Node<T> *node);
		virtual UInt32 CalHash(const UTF8Char *key, UOSInt keyLen) const;
	public:
		BTreeUTF8Map();
		virtual ~BTreeUTF8Map();

		virtual T Put(Text::CString key, T val);
		virtual T Get(Text::CString key) const;
		virtual T Remove(Text::CString key);
		virtual Bool IsEmpty() const;
		virtual T *ToArray(UOSInt *objCnt);
		virtual Text::CString *ToNameArray(UOSInt *objCnt);
		virtual void Clear();
	};

	template <class T> void BTreeUTF8Map<T>::OptimizeNode(BTreeUTF8Node<T> *node)
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

	template <class T> void BTreeUTF8Map<T>::ReleaseNodeTree(BTreeUTF8Node<T> *node)
	{
		if (node->leftNode)
		{
			ReleaseNodeTree(node->leftNode);
		}
		if (node->rightNode)
		{
			ReleaseNodeTree(node->rightNode);
		}
		MemFree(node);
	}

	template <class T> BTreeUTF8Node<T> *BTreeUTF8Map<T>::NewNode(Text::CString key, UInt32 hash, T val)
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
		key.ConcatTo(node->nodeKey);
		return node;
	}

	template <class T> T BTreeUTF8Map<T>::PutNode(BTreeUTF8Node<T> *node, Text::CString key, UInt32 hash, T val)
	{
		BTreeUTF8Node<T> *tmpNode;
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

	template <class T> BTreeUTF8Node<T> *BTreeUTF8Map<T>::RemoveNode(BTreeUTF8Node<T> *node)
	{
		BTreeUTF8Node<T> *parNode = node->parNode;
		while (parNode)
		{
			parNode->nodeCnt--;
			parNode = parNode->parNode;
		}
		if (node->leftNode == 0 && node->rightNode == 0)
		{
			MemFree(node);
			return 0;
		}
		else if (node->leftNode == 0)
		{
			BTreeUTF8Node<T> *outNode = node->rightNode;
			outNode->parNode = node->parNode;
			MemFree(node);
			return outNode;
		}
		else if (node->rightNode == 0)
		{
			BTreeUTF8Node<T> *outNode = node->leftNode;
			outNode->parNode = node->parNode;
			MemFree(node);
			return outNode;
		}

		if (node->leftNode->nodeCnt >= node->rightNode->nodeCnt)
		{
			BTreeUTF8Node<T> *outNode = node->leftNode;
			BTreeUTF8Node<T> *tmpNode;
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
			BTreeUTF8Node<T> *outNode = node->rightNode;
			BTreeUTF8Node<T> *tmpNode;
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

	template <class T> UInt32 BTreeUTF8Map<T>::CalHash(const UTF8Char *key, UOSInt keyLen) const
	{
		return this->crc->CalcDirect(key, keyLen);
	}

	template <class T> void BTreeUTF8Map<T>::FillArr(T **arr, BTreeUTF8Node<T> *node)
	{
		if (node == 0)
			return;
		FillArr(arr, node->leftNode);
		**arr = node->nodeVal;
		++*arr;
		FillArr(arr, node->rightNode);
	}

	template <class T> void BTreeUTF8Map<T>::FillNameArr(Text::CString **arr, BTreeUTF8Node<T> *node)
	{
		if (node == 0)
			return;
		FillNameArr(arr, node->leftNode);
		**arr = {node->nodeKey, node->keyLen};
		++*arr;
		FillNameArr(arr, node->rightNode);
	}

	template <class T> BTreeUTF8Map<T>::BTreeUTF8Map() : DataMap<Text::CString, T>()
	{
		rootNode = 0;
		NEW_CLASS(crc, Crypto::Hash::CRC32RC());
	}

	template <class T> BTreeUTF8Map<T>::~BTreeUTF8Map()
	{
		if (this->rootNode)
		{
			ReleaseNodeTree(this->rootNode);
			this->rootNode = 0;
		}
		DEL_CLASS(crc);
	}

	template <class T> T BTreeUTF8Map<T>::Put(Text::CString key, T val)
	{
		UInt32 hash = CalHash(key.v, key.leng);
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

	template <class T> T BTreeUTF8Map<T>::Get(Text::CString key) const
	{
		UInt32 hash = CalHash(key.v, key.leng);
		BTreeUTF8Node<T> *node = this->rootNode;
		while (node)
		{
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

	template <class T> T BTreeUTF8Map<T>::Remove(Text::CString key)
	{
		if (this->rootNode == 0)
			return 0;
		if (Text::StrEqualsC(this->rootNode->nodeKey, this->rootNode->keyLen, key.v, key.leng))
		{
			T nodeVal = this->rootNode->nodeVal;
			this->rootNode = RemoveNode(this->rootNode);
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

	template <class T> Bool BTreeUTF8Map<T>::IsEmpty() const
	{
		return this->rootNode == 0;
	}

	template <class T> T *BTreeUTF8Map<T>::ToArray(UOSInt *objCnt)
	{
		UOSInt cnt = 0;
		if (this->rootNode)
		{
			cnt = (UOSInt)this->rootNode->nodeCnt + 1;
		}
		T *outArr = MemAlloc(T, cnt);
		T *tmpArr = outArr;
		FillArr(&tmpArr, this->rootNode);
		*objCnt = cnt;
		return outArr;
	}

	template <class T> Text::CString *BTreeUTF8Map<T>::ToNameArray(UOSInt *objCnt)
	{
		UOSInt cnt = 0;
		if (this->rootNode)
		{
			cnt = (UOSInt)this->rootNode->nodeCnt + 1;
		}
		Text::CString *outArr = MemAlloc(Text::CString, cnt);
		Text::CString *tmpArr = outArr;
		FillNameArr(&tmpArr, this->rootNode);
		*objCnt = cnt;
		return outArr;
	}

	template <class T>void BTreeUTF8Map<T>::Clear()
	{
		if (this->rootNode)
		{
			ReleaseNodeTree(this->rootNode);
			this->rootNode = 0;
		}
	}
};

#endif
