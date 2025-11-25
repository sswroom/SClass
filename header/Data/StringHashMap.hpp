#ifndef _SM_DATA_STRINGHASHMAP
#define _SM_DATA_STRINGHASHMAP
#include "MyMemory.h"
#include "Crypto/Hash/CRC32RC.h"
#include "Data/DataMap.hpp"
#include "Text/String.h"

namespace Data
{
	template <typename T> struct HashNodeItem
	{
		UInt32 cnt;
		Text::String *str;
		T val;
		UInt32 hash;
	};

	template <typename T> struct HashNodeLayer
	{
		UOSInt capacity;
		HashNodeLayer<T> *parentLayer;
		HashNodeLayer<T> *nextLayer;
		HashNodeItem<T> items[1];
	};

	template <class T> class StringHashMap : public DataMap<Text::String*, T>
	{
	protected:
		Crypto::Hash::CRC32RC crc;
		HashNodeLayer<T> *rootNode;

	protected:
		void ReleaseNode(HashNodeLayer<T> *layer, UOSInt index);
		void CreateNextLayer(HashNodeLayer<T> *layer);

		virtual UInt32 CalHash(Text::String *key);
	public:
		StringHashMap();
		virtual ~StringHashMap();

		virtual T Put(Text::String *key, T val);
		virtual T Get(Text::String *key);
		virtual T Remove(Text::String *key);
		virtual Bool IsEmpty();
		virtual T *ToArray(UOSInt *objCnt);
		virtual void Clear();
	};

	template <class T> void StringHashMap<T>::ReleaseNode(HashNodeLayer<T> *layer, UOSInt index)
	{
		layer->items[index].str->Release();
		layer->items[index].str = 0;
		if (layer->nextLayer)
		{
			if (layer->nextLayer->items[index << 1].str)
			{
				this->ReleaseNode(layer->nextLayer, index << 1);
			}
			if (layer->nextLayer->items[(index << 1) + 1].str)
			{
				this->ReleaseNode(layer->nextLayer, (index << 1) + 1);
			}
		}
	}

	template <class T> void StringHashMap<T>::CreateNextLayer(HashNodeLayer<T> *layer)
	{
		layer->nextLayer = (HashNodeLayer<T>*)MAlloc(sizeof(HashNodeLayer<T>) + sizeof(HashNodeItem<T>) * ((layer->capacity << 1) - 1));
		layer->nextLayer->capacity = layer->capacity << 1;
		layer->nextLayer->nextLayer = 0;
		layer->nextLayer->parentLayer = layer;
		UOSInt i = layer->nextLayer->capacity;
		while (i-- > 0)
		{
			layer->nextLayer->items[i].str = 0;
		}
	}


	template <class T> UInt32 StringHashMap<T>::CalHash(Text::String *key)
	{
		return this->crc.CalcDirect(key->v, key->leng);
	}

	template <class T> StringHashMap<T>::StringHashMap()
	{
		this->rootNode = 0;
	}

	template <class T> StringHashMap<T>::~StringHashMap()
	{
		HashNodeLayer<T> *node;
		HashNodeLayer<T> *nextNode = this->rootNode;
		if (nextNode)
		{
			node = nextNode;
			nextNode = node->nextLayer;
			MemFree(node);
		}
	}

	template <class T> T StringHashMap<T>::Put(Text::String *key, T val)
	{
		UInt32 hash = this->CalHash(key);
		UOSInt index;
		HashNodeLayer<T> *currLayer;
		if (this->rootNode == 0)
		{
			this->rootNode = MemAlloc(HashNodeLayer<T>, 1);
			this->rootNode->capacity = 1;
			this->rootNode->nextLayer = 0;
			this->rootNode->parentLayer = 0;
			this->rootNode->items[0].cnt = 1;
			this->rootNode->items[0].str = key->Clone();
			this->rootNode->items[0].val = val;
			this->rootNode->items[0].hash = hash;
			return 0;
		}
		OSInt i;
		currLayer = this->rootNode;
		index = 0;
		while (true)
		{
			if (currLayer->items[index].str == 0)
			{
				currLayer->items[index].cnt = 1;
				currLayer->items[index].str = key->Clone();
				currLayer->items[index].val = val;
				currLayer->items[index].hash = hash;

				while (currLayer->parentLayer)
				{
					currLayer = currLayer->parentLayer;
					index = index >> 1;
					currLayer->items[index].cnt++;
				}
				return 0;
			}

			if (currLayer->items[index].hash == hash)
			{
				i = currLayer->items[index].str->CompareTo(key);
			}
			else if (currLayer->items[index].hash > hash)
			{
				i = 1;
			}
			else
			{
				i = -1;
			}
			if (i == 0)
			{
				T oldVal = currLayer->items[index].val;
				currLayer->items[index].val = val;
				return oldVal;
			}
			if (currLayer->nextLayer == 0)
			{
				this->CreateNextLayer(currLayer);
			}
			currLayer = currLayer->nextLayer;
			if (i == 1)
			{
				index = index << 1;
			}
			else
			{
				index = (index << 1) + 1;
			}
		}
	}

	template <class T> T StringHashMap<T>::Get(Text::String *key)
	{
		if (this->rootNode == 0 || this->rootNode->items[0].str == 0)
		{
			return 0;
		}
		UInt32 hash = this->CalHash(key);
		UOSInt index = 0;
		OSInt i;
		HashNodeLayer<T> *currLayer = this->rootNode;
		while (true)
		{
			if (currLayer->items[index].str == 0)
			{
				return 0;
			}

			if (currLayer->items[index].hash == hash)
			{
				i = currLayer->items[index].str->CompareTo(key);
			}
			else if (currLayer->items[index].hash > hash)
			{
				i = 1;
			}
			else
			{
				i = -1;
			}
			if (i == 0)
			{
				return currLayer->items[index].val;
			}
			else if (currLayer->nextLayer == 0)
			{
				return 0;
			}
			currLayer = currLayer->nextLayer;
			if (i == 1)
			{
				index = index << 1;
			}
			else
			{
				index = (index << 1) + 1;
			}
		}
	}

	template <class T> T StringHashMap<T>::Remove(Text::String *key)
	{
		/////////////////////////////
		return 0;
	}

	template <class T> Bool StringHashMap<T>::IsEmpty()
	{
		return this->rootNode == 0 || this->rootNode->items[0].str == 0;
	}

	template <class T> T *StringHashMap<T>::ToArray(UOSInt *objCnt)
	{
		//////////////////////////////
		return 0;
	}

	template <class T> void StringHashMap<T>::Clear()
	{
		if (this->rootNode && this->rootNode->items[0].str)
		{
			this->ReleaseNode(this->rootNode, 0);
		}
	}
}

#endif
