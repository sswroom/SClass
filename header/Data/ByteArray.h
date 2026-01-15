#ifndef _SM_DATA_BYTEARRAY
#define _SM_DATA_BYTEARRAY
#include "Data/ByteArrayBase.hpp"

namespace Data
{
	class ByteArrayR;
	class ByteArray : public ByteArrayBase<UInt8>
	{
	private:
		static void FORCEINLINE CopyArray(Data::ByteArray destArr, const ByteArrayR &srcArr);
#if defined(CHECK_RANGE)
		ByteArray(UnsafeArray<UInt8> buff, UIntOS buffSize, UIntOS prevSize) : ByteArrayBase(buff, buffSize)
		{
			this->prevSize = prevSize;
		}
#endif

	public:
		ByteArray() = default;
		ByteArray(UnsafeArray<UInt8> buff, UIntOS buffSize) : ByteArrayBase(buff, buffSize)
		{
#if defined(CHECK_RANGE)
			this->prevSize = 0;
#endif
		}

		virtual ~ByteArray()
		{

		}

		ByteArray SubArray(UIntOS ofst, UIntOS size) const
		{
			CheckError(ofst + size);
#if defined(CHECK_RANGE)
			return ByteArray(&buff[ofst], size, this->prevSize + ofst);
#else
			return ByteArray(buff + ofst, size);
#endif
		}

		ByteArray WithSize(UIntOS size) const
		{
			CheckError(size);
#if defined(CHECK_RANGE)
			return ByteArray(buff, size, this->prevSize);
#else
			return ByteArray(buff, size);
#endif
		}
		
		ByteArray SubArray(UIntOS ofst) const
		{
			CheckError(ofst);
#if defined(CHECK_RANGE)
			return ByteArray(buff + ofst, this->buffSize - ofst, this->prevSize + ofst);
#else
			return ByteArray(buff + ofst, this->buffSize - ofst);
#endif
		}

		void CopyFrom(UIntOS destIndex, const ByteArray &srcArr) const
		{
			CheckError(destIndex + srcArr.GetSize());
			MemCopyNO(&buff[destIndex], srcArr.Arr().Ptr(), srcArr.GetSize());
		}

		void FORCEINLINE CopyFrom(UIntOS destIndex, const ByteArrayR &srcArr) const
		{
			CopyArray(this->SubArray(destIndex), srcArr);
		}

		void FORCEINLINE CopyFrom(const ByteArrayR &srcArr) const
		{
			CopyArray(NNTHIS, srcArr);
		}

		void CopyInner(UIntOS destIndex, UIntOS srcIndex, UIntOS cnt)
		{
#if defined(CHECK_RANGE)
			if (destIndex > srcIndex)
				CheckError(destIndex + cnt);
			else
				CheckError(srcIndex + cnt);
#endif
			MemCopyO(&buff[destIndex], &buff[srcIndex], cnt);
		}

		ByteArray &operator+=(UIntOS ofst)
		{
			CheckError(ofst);
			this->buff = buff + ofst;
			this->buffSize -= ofst;
#if defined(CHECK_RANGE)
			this->prevSize += ofst;
#endif
			return NNTHIS;
		}

		ByteArray &operator+=(IntOS ofst)
		{
#if defined(CHECK_RANGE)
			if (ofst < 0)
				CheckErrorPrev((UIntOS)-ofst);
			else
				CheckError((UIntOS)ofst);
#endif
			this->buff = buff + ofst;
			this->buffSize -= (UIntOS)ofst;
#if defined(CHECK_RANGE)
			this->prevSize += (UIntOS)ofst;
#endif
			return NNTHIS;
		}

#if _OSINT_SIZE == 64
		ByteArray &operator+=(UInt32 ofst)
		{
			CheckError(ofst);
			this->buff = buff + ofst;
			this->buffSize -= ofst;
#if defined(CHECK_RANGE)
			this->prevSize += ofst;
#endif
			return NNTHIS;
		}

		ByteArray &operator+=(Int32 ofst)
		{
#if defined(CHECK_RANGE)
			if (ofst < 0)
				CheckErrorPrev((UIntOS)-(IntOS)ofst);
			else
				CheckError((UIntOS)ofst);
#endif
			this->buff = buff + ofst;
			this->buffSize -= (UIntOS)(IntOS)ofst;
#if defined(CHECK_RANGE)
			this->prevSize += (UIntOS)(IntOS)ofst;
#endif
			return NNTHIS;
		}
#endif

		UInt8 *operator++(int)
		{
			CheckError(1);
			this->buff = buff + 1;
			this->buffSize--;
#if defined(CHECK_RANGE)
			this->prevSize++;
#endif
			return &this->buff[-1];
		}

		ByteArray &operator-=(UIntOS ofst)
		{
			CheckErrorPrev(ofst);
			this->buff = buff -( IntOS)ofst;
			this->buffSize += ofst;
#if defined(CHECK_RANGE)
			this->prevSize -= ofst;
#endif
			return NNTHIS;
		}

		ByteArray operator+(UIntOS ofst)
		{
			CheckError(ofst);
#if defined(CHECK_RANGE)
			return ByteArray(buff + ofst, this->buffSize - ofst, this->prevSize + ofst);
#else
			return ByteArray(buff + ofst, this->buffSize - ofst);
#endif
		}

		ByteArray operator+(IntOS ofst)
		{
#if defined(CHECK_RANGE)
			if (ofst < 0)
				CheckErrorPrev((UIntOS)-ofst);
			else
				CheckError((UIntOS)ofst);
			return ByteArray(buff + ofst, this->buffSize - (UIntOS)ofst, this->prevSize + (UIntOS)ofst);
#else
			return ByteArray(buff + ofst, this->buffSize - (UIntOS)ofst);
#endif
		}

#if _OSINT_SIZE == 64
		ByteArray operator+(UInt32 ofst)
		{
			CheckError(ofst);
#if defined(CHECK_RANGE)
			return ByteArray(buff + ofst, this->buffSize - ofst, this->prevSize + ofst);
#else
			return ByteArray(buff + ofst, this->buffSize - ofst);
#endif
		}

		ByteArray operator+(Int32 ofst)
		{
#if defined(CHECK_RANGE)
			if (ofst < 0)
				CheckErrorPrev((UIntOS)-(IntOS)ofst);
			else
				CheckError((UIntOS)ofst);
			return ByteArray(buff + ofst, this->buffSize - (UIntOS)(IntOS)ofst, this->prevSize + (UIntOS)(IntOS)ofst);
#else
			return ByteArray(buff + ofst, this->buffSize - (UIntOS)(IntOS)ofst);
#endif
		}

		ByteArray operator-(Int32 ofst)
		{
#if defined(CHECK_RANGE)
			if (ofst >= 0)
				CheckErrorPrev((UIntOS)ofst);
			else
				CheckError((UIntOS)-(IntOS)ofst);
			return ByteArray(buff - ofst, this->buffSize + (UIntOS)(IntOS)ofst, this->prevSize - (UIntOS)(IntOS)ofst);
#else
			return ByteArray(buff - ofst, this->buffSize + (UIntOS)(IntOS)ofst);
#endif
		}
#endif

		ByteArray operator-(UIntOS ofst)
		{
			CheckErrorPrev(ofst);
#if defined(CHECK_RANGE)
			return ByteArray(buff - (IntOS)ofst, this->buffSize + ofst, this->prevSize - ofst);
#else
			return ByteArray(buff - (IntOS)ofst, this->buffSize + ofst);
#endif
		}

		IntOS operator-(const ByteArray &buff)
		{
			return this->buff - buff.buff;
		}


		void Clear(UIntOS ofst, UIntOS cnt)
		{
			CheckError(ofst + cnt);
			MemClear(&buff[ofst], cnt);
		}

		void WriteI16(UIntOS ofst, Int16 val)
		{
			CheckError(ofst + 2);
			WriteInt16(&buff[ofst], val);
		}

		void WriteU16(UIntOS ofst, UInt16 val)
		{
			CheckError(ofst + 2);
			WriteUInt16(&buff[ofst], val);
		}

		void WriteMI16(UIntOS ofst, Int16 val)
		{
			CheckError(ofst + 2);
			WriteMInt16(&buff[ofst], val);
		}

		void WriteMU16(UIntOS ofst, UInt16 val)
		{
			CheckError(ofst + 2);
			WriteMUInt16(&buff[ofst], val);
		}

		void WriteNI16(UIntOS ofst, Int16 val)
		{
			CheckError(ofst + 2);
			WriteNInt16(&buff[ofst], val);
		}

		void WriteNU16(UIntOS ofst, UInt16 val)
		{
			CheckError(ofst + 2);
			WriteNUInt16(&buff[ofst], val);
		}

		void WriteI32(UIntOS ofst, Int32 val)
		{
			CheckError(ofst + 4);
			WriteInt32(&buff[ofst], val);
		}

		void WriteU32(UIntOS ofst, UInt32 val)
		{
			CheckError(ofst + 4);
			WriteUInt32(&buff[ofst], val);
		}

		void WriteMI32(UIntOS ofst, Int32 val)
		{
			CheckError(ofst + 4);
			WriteMInt32(&buff[ofst], val);
		}

		void WriteMU32(UIntOS ofst, UInt32 val)
		{
			CheckError(ofst + 4);
			WriteMUInt32(&buff[ofst], val);
		}

		void WriteNI32(UIntOS ofst, Int32 val)
		{
			CheckError(ofst + 4);
			WriteNInt32(&buff[ofst], val);
		}

		void WriteNU32(UIntOS ofst, UInt32 val)
		{
			CheckError(ofst + 4);
			WriteNUInt32(&buff[ofst], val);
		}
	};

	class ByteArrayR : public ByteArrayBase<const UInt8>
	{
#if defined(CHECK_RANGE)
	private:
		ByteArrayR(UnsafeArray<const UInt8> buff, UIntOS buffSize, UIntOS prevSize) : ByteArrayBase(buff, buffSize)
		{
			this->prevSize = prevSize;
		}
#endif

	public:
		ByteArrayR(const ByteArray &arr) : ByteArrayBase(arr.Arr(), arr.GetSize())
		{
#if defined(CHECK_RANGE)
			this->prevSize = arr.GetPrevSize();
#endif
		}

		ByteArrayR(UnsafeArray<const UInt8> buff, UIntOS buffSize) : ByteArrayBase(buff, buffSize)
		{
#if defined(CHECK_RANGE)
			this->prevSize = 0;
#endif
		}

		~ByteArrayR()
		{

		}

		ByteArrayR SubArray(UIntOS ofst, UIntOS size) const
		{
			CheckError(ofst + size);
			return ByteArrayR(buff + ofst, size);
		}
		
		ByteArrayR SubArray(UIntOS ofst) const
		{
			CheckError(ofst);
			return ByteArrayR(buff + ofst, this->buffSize - ofst);
		}

		ByteArrayR WithSize(UIntOS size) const
		{
			CheckError(size);
#if defined(CHECK_RANGE)
			return ByteArrayR(buff, size, this->prevSize);
#else
			return ByteArrayR(buff, size);
#endif
		}

		ByteArrayR &operator+=(UIntOS ofst)
		{
			CheckError(ofst);
			this->buff = buff + ofst;
			this->buffSize -= ofst;
#if defined(CHECK_RANGE)
			this->prevSize += ofst;
#endif
			return NNTHIS;
		}

		ByteArrayR &operator+=(IntOS ofst)
		{
#if defined(CHECK_RANGE)
			if (ofst < 0)
				CheckErrorPrev((UIntOS)-ofst);
			else
				CheckError((UIntOS)ofst);
#endif
			this->buff = buff + ofst;
			this->buffSize -= (UIntOS)ofst;
#if defined(CHECK_RANGE)
			this->prevSize += (UIntOS)ofst;
#endif
			return NNTHIS;
		}

#if _OSINT_SIZE == 64
		ByteArrayR &operator+=(UInt32 ofst)
		{
			CheckError(ofst);
			this->buff = buff + ofst;
			this->buffSize -= ofst;
#if defined(CHECK_RANGE)
			this->prevSize += ofst;
#endif
			return NNTHIS;
		}

		ByteArrayR &operator+=(Int32 ofst)
		{
#if defined(CHECK_RANGE)
			if (ofst < 0)
				CheckErrorPrev((UIntOS)-(IntOS)ofst);
			else
				CheckError((UIntOS)ofst);
#endif
			this->buff = buff + ofst;
			this->buffSize -= (UIntOS)(IntOS)ofst;
#if defined(CHECK_RANGE)
			this->prevSize += (UIntOS)(IntOS)ofst;
#endif
			return NNTHIS;
		}
#endif
	};

	void FORCEINLINE ByteArray::CopyArray(Data::ByteArray destArr, const ByteArrayR &srcArr)
	{
		destArr.CheckError(srcArr.GetSize());
		MemCopyNO(destArr.buff.Ptr(), srcArr.Arr().Ptr(), srcArr.GetSize());
	}
}
#define BYTEARR(var) Data::ByteArray(UARR(var), sizeof(var))
#endif