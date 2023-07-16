#ifndef _SM_DATA_BYTEBUFFER
#define _SM_DATA_BYTEBUFFER
#include "Data/ByteArray.h"

namespace Data
{
	class ByteBuffer : public ByteArray
	{
	public:
		ByteBuffer() : ByteArray(0, 0)
		{
		}

		ByteBuffer(UOSInt buffSize) : ByteArray(MemAlloc(UInt8, buffSize), buffSize)
		{
		}

		~ByteBuffer()
		{
			if (this->buff)
				MemFree(this->buff);
		}

		void ReplaceBy(ByteBuffer &buff)
		{
			if (this->buff)
				MemFree(this->buff);
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
				MemFree(this->buff);
			this->buff = MemAlloc(UInt8, buffSize);
			this->buffSize = buffSize;
#if defined(CHECK_RANGE)
			this->prevSize = 0;
#endif
		}

		void Delete()
		{
			if (this->buff)
				MemFree(this->buff);
			this->buff = 0;
			this->buffSize = 0;
#if defined(CHECK_RANGE)
			this->prevSize = 0;
#endif
		}

		ByteBuffer &operator=(ByteBuffer &) = delete;
	};
}
#endif