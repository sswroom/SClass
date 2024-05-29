#ifndef _SM_DATA_BYTEBUFFERA
#define _SM_DATA_BYTEBUFFERA
#include "Data/ByteArray.h"

namespace Data
{
	class ByteBufferA : public ByteArray
	{
	private:
		Bool deleted;
	public:
		ByteBufferA() : ByteArray(MemAllocA(UInt8, 0), 0)
		{
			this->deleted = false;
		}

		ByteBufferA(UOSInt buffSize) : ByteArray(MemAllocA(UInt8, buffSize), buffSize)
		{
			this->deleted = false;
		}

		~ByteBufferA()
		{
			if (!this->deleted)
				MemFreeAArr(this->buff);
		}

		void ReplaceBy(ByteBufferA &buff)
		{
			if (!buff.deleted)
			{
				if (!this->deleted)
					MemFreeAArr(this->buff);
				this->buff = buff.buff;
				this->buffSize = buff.buffSize;
#if defined(CHECK_RANGE)
				this->prevSize = buff.prevSize;
#endif
				buff.buffSize = 0;
				buff.deleted = true;
			}
		}

		void ChangeSize(UOSInt buffSize)
		{
			if (!this->deleted)
				MemFreeAArr(this->buff);
			this->buff = MemAllocA(UInt8, buffSize);
			this->buffSize = buffSize;
#if defined(CHECK_RANGE)
			this->prevSize = 0;
#endif
		}

		void Delete()
		{
			if (!this->deleted)
			{
				MemFreeAArr(this->buff);
				this->buffSize = 0;
				this->deleted = true;
#if defined(CHECK_RANGE)
				this->prevSize = 0;
#endif
			}
		}

		ByteBufferA &operator=(ByteBufferA &) = delete;
	};
}
#endif