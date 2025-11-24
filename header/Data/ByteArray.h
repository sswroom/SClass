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
		ByteArray(UnsafeArray<UInt8> buff, UOSInt buffSize, UOSInt prevSize) : ByteArrayBase(buff, buffSize)
		{
			this->prevSize = prevSize;
		}
#endif

	public:
		ByteArray() = default;
		ByteArray(UnsafeArray<UInt8> buff, UOSInt buffSize) : ByteArrayBase(buff, buffSize)
		{
#if defined(CHECK_RANGE)
			this->prevSize = 0;
#endif
		}

		virtual ~ByteArray()
		{

		}

		ByteArray SubArray(UOSInt ofst, UOSInt size) const
		{
			CheckError(ofst + size);
#if defined(CHECK_RANGE)
			return ByteArray(&buff[ofst], size, this->prevSize + ofst);
#else
			return ByteArray(buff + ofst, size);
#endif
		}

		ByteArray WithSize(UOSInt size) const
		{
			CheckError(size);
#if defined(CHECK_RANGE)
			return ByteArray(buff, size, this->prevSize);
#else
			return ByteArray(buff, size);
#endif
		}
		
		ByteArray SubArray(UOSInt ofst) const
		{
			CheckError(ofst);
#if defined(CHECK_RANGE)
			return ByteArray(buff + ofst, this->buffSize - ofst, this->prevSize + ofst);
#else
			return ByteArray(buff + ofst, this->buffSize - ofst);
#endif
		}

		void CopyFrom(UOSInt destIndex, const ByteArray &srcArr) const
		{
			CheckError(destIndex + srcArr.GetSize());
			MemCopyNO(&buff[destIndex], srcArr.Arr().Ptr(), srcArr.GetSize());
		}

		void FORCEINLINE CopyFrom(UOSInt destIndex, const ByteArrayR &srcArr) const
		{
			CopyArray(this->SubArray(destIndex), srcArr);
		}

		void FORCEINLINE CopyFrom(const ByteArrayR &srcArr) const
		{
			CopyArray(NNTHIS, srcArr);
		}

		void CopyInner(UOSInt destIndex, UOSInt srcIndex, UOSInt cnt)
		{
#if defined(CHECK_RANGE)
			if (destIndex > srcIndex)
				CheckError(destIndex + cnt);
			else
				CheckError(srcIndex + cnt);
#endif
			MemCopyO(&buff[destIndex], &buff[srcIndex], cnt);
		}

		ByteArray &operator+=(UOSInt ofst)
		{
			CheckError(ofst);
			this->buff = buff + ofst;
			this->buffSize -= ofst;
#if defined(CHECK_RANGE)
			this->prevSize += ofst;
#endif
			return NNTHIS;
		}

		ByteArray &operator+=(OSInt ofst)
		{
#if defined(CHECK_RANGE)
			if (ofst < 0)
				CheckErrorPrev((UOSInt)-ofst);
			else
				CheckError((UOSInt)ofst);
#endif
			this->buff = buff + ofst;
			this->buffSize -= (UOSInt)ofst;
#if defined(CHECK_RANGE)
			this->prevSize += (UOSInt)ofst;
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
				CheckErrorPrev((UOSInt)-(OSInt)ofst);
			else
				CheckError((UOSInt)ofst);
#endif
			this->buff = buff + ofst;
			this->buffSize -= (UOSInt)(OSInt)ofst;
#if defined(CHECK_RANGE)
			this->prevSize += (UOSInt)(OSInt)ofst;
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

		ByteArray &operator-=(UOSInt ofst)
		{
			CheckErrorPrev(ofst);
			this->buff = buff -( OSInt)ofst;
			this->buffSize += ofst;
#if defined(CHECK_RANGE)
			this->prevSize -= ofst;
#endif
			return NNTHIS;
		}

		ByteArray operator+(UOSInt ofst)
		{
			CheckError(ofst);
#if defined(CHECK_RANGE)
			return ByteArray(buff + ofst, this->buffSize - ofst, this->prevSize + ofst);
#else
			return ByteArray(buff + ofst, this->buffSize - ofst);
#endif
		}

		ByteArray operator+(OSInt ofst)
		{
#if defined(CHECK_RANGE)
			if (ofst < 0)
				CheckErrorPrev((UOSInt)-ofst);
			else
				CheckError((UOSInt)ofst);
			return ByteArray(buff + ofst, this->buffSize - (UOSInt)ofst, this->prevSize + (UOSInt)ofst);
#else
			return ByteArray(buff + ofst, this->buffSize - (UOSInt)ofst);
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
				CheckErrorPrev((UOSInt)-(OSInt)ofst);
			else
				CheckError((UOSInt)ofst);
			return ByteArray(buff + ofst, this->buffSize - (UOSInt)(OSInt)ofst, this->prevSize + (UOSInt)(OSInt)ofst);
#else
			return ByteArray(buff + ofst, this->buffSize - (UOSInt)(OSInt)ofst);
#endif
		}

		ByteArray operator-(Int32 ofst)
		{
#if defined(CHECK_RANGE)
			if (ofst >= 0)
				CheckErrorPrev((UOSInt)ofst);
			else
				CheckError((UOSInt)-(OSInt)ofst);
			return ByteArray(buff - ofst, this->buffSize + (UOSInt)(OSInt)ofst, this->prevSize - (UOSInt)(OSInt)ofst);
#else
			return ByteArray(buff - ofst, this->buffSize + (UOSInt)(OSInt)ofst);
#endif
		}
#endif

		ByteArray operator-(UOSInt ofst)
		{
			CheckErrorPrev(ofst);
#if defined(CHECK_RANGE)
			return ByteArray(buff - (OSInt)ofst, this->buffSize + ofst, this->prevSize - ofst);
#else
			return ByteArray(buff - (OSInt)ofst, this->buffSize + ofst);
#endif
		}

		OSInt operator-(const ByteArray &buff)
		{
			return this->buff - buff.buff;
		}


		void Clear(UOSInt ofst, UOSInt cnt)
		{
			CheckError(ofst + cnt);
			MemClear(&buff[ofst], cnt);
		}

		void WriteI16(UOSInt ofst, Int16 val)
		{
			CheckError(ofst + 2);
			WriteInt16(&buff[ofst], val);
		}

		void WriteU16(UOSInt ofst, UInt16 val)
		{
			CheckError(ofst + 2);
			WriteUInt16(&buff[ofst], val);
		}

		void WriteMI16(UOSInt ofst, Int16 val)
		{
			CheckError(ofst + 2);
			WriteMInt16(&buff[ofst], val);
		}

		void WriteMU16(UOSInt ofst, UInt16 val)
		{
			CheckError(ofst + 2);
			WriteMUInt16(&buff[ofst], val);
		}

		void WriteNI16(UOSInt ofst, Int16 val)
		{
			CheckError(ofst + 2);
			WriteNInt16(&buff[ofst], val);
		}

		void WriteNU16(UOSInt ofst, UInt16 val)
		{
			CheckError(ofst + 2);
			WriteNUInt16(&buff[ofst], val);
		}

		void WriteI32(UOSInt ofst, Int32 val)
		{
			CheckError(ofst + 4);
			WriteInt32(&buff[ofst], val);
		}

		void WriteU32(UOSInt ofst, UInt32 val)
		{
			CheckError(ofst + 4);
			WriteUInt32(&buff[ofst], val);
		}

		void WriteMI32(UOSInt ofst, Int32 val)
		{
			CheckError(ofst + 4);
			WriteMInt32(&buff[ofst], val);
		}

		void WriteMU32(UOSInt ofst, UInt32 val)
		{
			CheckError(ofst + 4);
			WriteMUInt32(&buff[ofst], val);
		}

		void WriteNI32(UOSInt ofst, Int32 val)
		{
			CheckError(ofst + 4);
			WriteNInt32(&buff[ofst], val);
		}

		void WriteNU32(UOSInt ofst, UInt32 val)
		{
			CheckError(ofst + 4);
			WriteNUInt32(&buff[ofst], val);
		}
	};

	class ByteArrayR : public ByteArrayBase<const UInt8>
	{
#if defined(CHECK_RANGE)
	private:
		ByteArrayR(UnsafeArray<const UInt8> buff, UOSInt buffSize, UOSInt prevSize) : ByteArrayBase(buff, buffSize)
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

		ByteArrayR(UnsafeArray<const UInt8> buff, UOSInt buffSize) : ByteArrayBase(buff, buffSize)
		{
#if defined(CHECK_RANGE)
			this->prevSize = 0;
#endif
		}

		~ByteArrayR()
		{

		}

		ByteArrayR SubArray(UOSInt ofst, UOSInt size) const
		{
			CheckError(ofst + size);
			return ByteArrayR(buff + ofst, size);
		}
		
		ByteArrayR SubArray(UOSInt ofst) const
		{
			CheckError(ofst);
			return ByteArrayR(buff + ofst, this->buffSize - ofst);
		}

		ByteArrayR WithSize(UOSInt size) const
		{
			CheckError(size);
#if defined(CHECK_RANGE)
			return ByteArrayR(buff, size, this->prevSize);
#else
			return ByteArrayR(buff, size);
#endif
		}

		ByteArrayR &operator+=(UOSInt ofst)
		{
			CheckError(ofst);
			this->buff = buff + ofst;
			this->buffSize -= ofst;
#if defined(CHECK_RANGE)
			this->prevSize += ofst;
#endif
			return NNTHIS;
		}

		ByteArrayR &operator+=(OSInt ofst)
		{
#if defined(CHECK_RANGE)
			if (ofst < 0)
				CheckErrorPrev((UOSInt)-ofst);
			else
				CheckError((UOSInt)ofst);
#endif
			this->buff = buff + ofst;
			this->buffSize -= (UOSInt)ofst;
#if defined(CHECK_RANGE)
			this->prevSize += (UOSInt)ofst;
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
				CheckErrorPrev((UOSInt)-(OSInt)ofst);
			else
				CheckError((UOSInt)ofst);
#endif
			this->buff = buff + ofst;
			this->buffSize -= (UOSInt)(OSInt)ofst;
#if defined(CHECK_RANGE)
			this->prevSize += (UOSInt)(OSInt)ofst;
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