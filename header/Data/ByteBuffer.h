#ifndef _SM_DATA_BYTEBUFFER
#define _SM_DATA_BYTEBUFFER
#include "Data/ByteArray.h"

namespace Data
{
	class ByteBuffer : public ByteArray
	{
	private:
		Bool deleted;
	public:
		ByteBuffer() : ByteArray(MemAllocArr(UInt8, 0), 0)
		{
			this->deleted = false;
		}

		ByteBuffer(UOSInt buffSize) : ByteArray(MemAllocArr(UInt8, buffSize), buffSize)
		{
			this->deleted = false;
		}

		ByteBuffer(Data::ByteArrayR buff) : ByteArray(MemAllocArr(UInt8, buff.GetSize()), buff.GetSize())
		{
			this->deleted = false;
			MemCopyNO(this->buff.Ptr(), buff.Arr().Ptr(), buff.GetSize());
		}

		virtual ~ByteBuffer()
		{
			if (this->deleted)
			{
				MemFreeArr(this->buff);
			}
		}

		void ReplaceBy(ByteBuffer &buff)
		{
			if (!buff.deleted)
			{
				if (this->buff.Ptr())
					MemFreeArr(this->buff);
				this->buff = buff.buff;
				this->buffSize = buff.buffSize;
#if defined(CHECK_RANGE)
				this->prevSize = buff.prevSize;
#endif
				buff.deleted = true;
				buff.buffSize = 0;
			}
		}

		void ChangeSize(UOSInt buffSize)
		{
			if (this->buff.Ptr())
				MemFreeArr(this->buff);
			this->buff = MemAllocArr(UInt8, buffSize);
			this->buffSize = buffSize;
#if defined(CHECK_RANGE)
			this->prevSize = 0;
#endif
		}

		void Delete()
		{
			if (!this->deleted)
			{
				MemFreeArr(this->buff);
				this->buffSize = 0;
				this->deleted = true;
			}
#if defined(CHECK_RANGE)
			this->prevSize = 0;
#endif
		}

		ByteBuffer &operator=(ByteBuffer &) = delete;
	};
}
#endif