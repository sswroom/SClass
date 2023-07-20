#ifndef _SM_DATA_BYTEBUFFERA
#define _SM_DATA_BYTEBUFFERA
#include "Data/ByteArray.h"

namespace Data
{
	class ByteBufferA : public ByteArray
	{
	public:
		ByteBufferA() : ByteArray(0, 0)
		{
		}

		ByteBufferA(UOSInt buffSize) : ByteArray(MemAllocA(UInt8, buffSize), buffSize)
		{
		}

		~ByteBufferA()
		{
			if (this->buff)
				MemFreeA(this->buff);
		}

		void ReplaceBy(ByteBufferA &buff)
		{
			if (this->buff)
				MemFreeA(this->buff);
			this->buff = buff.buff;
			this->buffSize = buff.buffSize;
#if defined(CHECK_RANGE)
			this->prevSize = buff.prevSize;
#endif
			buff.buff = 0;
			buff.buffSize = 0;
		}

		void ChangeSize(UOSInt buffSize)
		{
			if (this->buff)
				MemFreeA(this->buff);
			this->buff = MemAllocA(UInt8, buffSize);
			this->buffSize = buffSize;
#if defined(CHECK_RANGE)
			this->prevSize = 0;
#endif
		}

		void Delete()
		{
			if (this->buff)
				MemFreeA(this->buff);
			this->buff = 0;
			this->buffSize = 0;
#if defined(CHECK_RANGE)
			this->prevSize = 0;
#endif
		}

		ByteBufferA &operator=(ByteBufferA &) = delete;
	};
}
#endif