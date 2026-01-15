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

		ByteBuffer(UIntOS buffSize) : ByteArray(MemAllocArr(UInt8, buffSize), buffSize)
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
			if (!this->deleted)
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
				this->deleted = false;
#if defined(CHECK_RANGE)
				this->prevSize = buff.prevSize;
#endif
				buff.deleted = true;
				buff.buffSize = 0;
			}
		}

		void ChangeSizeAndClear(UIntOS buffSize)
		{
			if (!this->deleted)
				MemFreeArr(this->buff);
			this->buff = MemAllocArr(UInt8, buffSize);
			this->buffSize = buffSize;
			this->deleted = false;
#if defined(CHECK_RANGE)
			this->prevSize = 0;
#endif
		}

		void ChangeSizeAndKeep(UIntOS buffSize)
		{
			if (this->deleted)
			{
				this->buff = MemAllocArr(UInt8, buffSize);
			}
			else
			{
				UnsafeArray<UInt8> newBuff = MemAllocArr(UInt8, buffSize);
				if (this->buffSize > buffSize)
				{
					MemCopyNO(newBuff.Ptr(), this->buff.Ptr(), buffSize);
				}
				else
				{
					MemCopyNO(newBuff.Ptr(), this->buff.Ptr(), this->buffSize);
				}
				MemFreeArr(this->buff);
				this->buff = newBuff;
			}
			this->buffSize = buffSize;
			this->deleted = false;
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

		ByteArrayR AsByteArray() const
		{
			return ByteArrayR(this->Arr(), this->GetSize());
		}

		ByteBuffer &operator=(ByteBuffer &) = delete;
	};
}
#endif