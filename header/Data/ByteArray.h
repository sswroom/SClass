#ifndef _SM_DATA_BYTEARRAY
#define _SM_DATA_BYTEARRAY
#include "MyMemory.h"
#include "Data/ByteTool.h"

//#define CHECK_RANGE
#if defined(CHECK_RANGE)
#include <stdio.h>
#endif
namespace Data
{
	class ByteArrayR;
	template <typename T> class ByteArrayBase
	{
	protected:
		T *buff;
		UOSInt buffSize;
#if defined(CHECK_RANGE)
		UOSInt prevSize;
#endif

		void CheckError(UOSInt ofst) const
		{
#if defined(CHECK_RANGE)
			if (ofst > buffSize)
				printf("ByteArray out of bound: ofst = %d, size = %d\r\n", (UInt32)ofst, (UInt32)this->buffSize);
#endif			
		}

		void CheckErrorPrev(UOSInt ofst) const
		{
#if defined(CHECK_RANGE)
			if (ofst > prevSize)
				printf("ByteArray out of bound: prevOfst = %d, prevSize = %d\r\n", (UInt32)ofst, (UInt32)this->prevSize);
#endif			
		}

		ByteArrayBase(T *buff, UOSInt buffSize)
		{
			this->buff = buff;
			this->buffSize = buffSize;
		}

	public:
		T &operator[](UOSInt index) const
		{
			CheckError(index + 1);
			return buff[index];
		}

		T &operator[](OSInt index) const
		{
#if defined(CHECK_RANGE)
			if (index < 0)
				CheckErrorPrev((UOSInt)-index);
			else
				CheckError((UOSInt)index + 1);
#endif
			return buff[index];
		}

#if _OSINT_SIZE == 64
		T &operator[](UInt32 index) const
		{
			CheckError(index + 1);
			return buff[index];
		}

		T &operator[](Int32 index) const
		{
#if defined(CHECK_RANGE)
			if (index < 0)
				CheckErrorPrev((UOSInt)-index);
			else
				CheckError((UOSInt)index + 1);
#endif
			return buff[index];
		}
#endif
		T &operator *()
		{
			CheckError(1);
			return buff[0];
		}

		Bool operator<=(const ByteArrayBase<T> &buff) const
		{
			return this->buff <= buff.buff;
		}

		Bool operator<(const ByteArrayBase<T> &buff) const
		{
			return this->buff < buff.buff;
		}

		Bool operator>=(const ByteArrayBase<T> &buff) const
		{
			return this->buff >= buff.buff;
		}

		Bool operator>(const ByteArrayBase<T> &buff) const
		{
			return this->buff > buff.buff;
		}

		T *GetPtr() const
		{
			return this->buff;
		}

		T *Ptr() const
		{
			return this->buff;
		}

		T *PtrEnd() const
		{
			return &this->buff[this->buffSize];
		}

		Bool IsNull() const
		{
			return this->buff == 0;
		}

		UOSInt GetSize() const
		{
			return this->buffSize;
		}

#if defined(CHECK_RANGE)
		UOSInt GetPrevSize() const
		{
			return this->prevSize;
		}
#endif

		Int16 ReadI16(UOSInt index) const
		{
			CheckError(index + 2);
			return ReadInt16(&buff[index]);
		}

		UInt16 ReadU16(UOSInt index) const
		{
			CheckError(index + 2);
			return ReadUInt16(&buff[index]);
		}

		Int16 ReadMI16(UOSInt index) const
		{
			CheckError(index + 2);
			return ReadMInt16(&buff[index]);
		}

		UInt16 ReadMU16(UOSInt index) const
		{
			CheckError(index + 2);
			return ReadMUInt16(&buff[index]);
		}

		Int16 ReadNI16(UOSInt index) const
		{
			CheckError(index + 2);
			return ReadNInt16(&buff[index]);
		}

		UInt16 ReadNU16(UOSInt index) const
		{
			CheckError(index + 2);
			return ReadNUInt16(&buff[index]);
		}

		Int32 ReadI32(UOSInt index) const
		{
			CheckError(index + 4);
			return ReadInt32(&buff[index]);
		}

		UInt32 ReadU32(UOSInt index) const
		{
			CheckError(index + 4);
			return ReadUInt32(&buff[index]);
		}

		Int32 ReadMI32(UOSInt index) const
		{
			CheckError(index + 4);
			return ReadMInt32(&buff[index]);
		}

		UInt32 ReadMU32(UOSInt index) const
		{
			CheckError(index + 4);
			return ReadMUInt32(&buff[index]);
		}

		Int32 ReadNI32(UOSInt index) const
		{
			CheckError(index + 4);
			return ReadNInt32(&buff[index]);
		}

		UInt32 ReadNU32(UOSInt index) const
		{
			CheckError(index + 4);
			return ReadNUInt32(&buff[index]);
		}
	};

	class ByteArray : public ByteArrayBase<UInt8>
	{
	private:
		static void FORCEINLINE CopyArray(Data::ByteArray destArr, const ByteArrayR &srcArr);
#if defined(CHECK_RANGE)
		ByteArray(UInt8 *buff, UOSInt buffSize, UOSInt prevSize) : ByteArrayBase(buff, buffSize)
		{
			this->prevSize = prevSize;
		}
#endif

	public:
		ByteArray() : ByteArrayBase(0, 0)
		{
#if defined(CHECK_RANGE)
			this->prevSize = 0;
#endif
		}

		ByteArray(UInt8 *buff, UOSInt buffSize) : ByteArrayBase(buff, buffSize)
		{
#if defined(CHECK_RANGE)
			this->prevSize = 0;
#endif
		}

		~ByteArray()
		{

		}

		ByteArray SubArray(UOSInt ofst, UOSInt size) const
		{
			CheckError(ofst + size);
#if defined(CHECK_RANGE)
			return ByteArray(&buff[ofst], size, this->prevSize + ofst);
#else
			return ByteArray(&buff[ofst], size);
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
			return ByteArray(&buff[ofst], this->buffSize - ofst, this->prevSize + ofst);
#else
			return ByteArray(&buff[ofst], this->buffSize - ofst);
#endif
		}

		void CopyFrom(UOSInt destIndex, const ByteArray &srcArr) const
		{
			CheckError(destIndex + srcArr.GetSize());
			MemCopyNO(&buff[destIndex], srcArr.GetPtr(), srcArr.GetSize());
		}

		void FORCEINLINE CopyFrom(UOSInt destIndex, const ByteArrayR &srcArr) const
		{
			CopyArray(this->SubArray(destIndex), srcArr);
		}

		void FORCEINLINE CopyFrom(const ByteArrayR &srcArr) const
		{
			CopyArray(*this, srcArr);
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
			this->buff = &buff[ofst];
			this->buffSize -= ofst;
#if defined(CHECK_RANGE)
			this->prevSize += ofst;
#endif
			return *this;
		}

		ByteArray &operator+=(OSInt ofst)
		{
#if defined(CHECK_RANGE)
			if (ofst < 0)
				CheckErrorPrev((UOSInt)-ofst);
			else
				CheckError((UOSInt)ofst);
#endif
			this->buff = &buff[ofst];
			this->buffSize -= (UOSInt)ofst;
#if defined(CHECK_RANGE)
			this->prevSize += (UOSInt)ofst;
#endif
			return *this;
		}

#if _OSINT_SIZE == 64
		ByteArray &operator+=(UInt32 ofst)
		{
			CheckError(ofst);
			this->buff = &buff[ofst];
			this->buffSize -= ofst;
#if defined(CHECK_RANGE)
			this->prevSize += ofst;
#endif
			return *this;
		}

		ByteArray &operator+=(Int32 ofst)
		{
#if defined(CHECK_RANGE)
			if (ofst < 0)
				CheckErrorPrev((UOSInt)-(OSInt)ofst);
			else
				CheckError((UOSInt)ofst);
#endif
			this->buff = &buff[ofst];
			this->buffSize -= (UOSInt)(OSInt)ofst;
#if defined(CHECK_RANGE)
			this->prevSize += (UOSInt)(OSInt)ofst;
#endif
			return *this;
		}
#endif

		ByteArray &operator++(int)
		{
			CheckError(1);
			this->buff = buff + 1;
			this->buffSize--;
#if defined(CHECK_RANGE)
			this->prevSize++;
#endif
			return *this;
		}

		ByteArray &operator-=(UOSInt ofst)
		{
			CheckErrorPrev(ofst);
			this->buff = &buff[-(OSInt)ofst];
			this->buffSize += ofst;
#if defined(CHECK_RANGE)
			this->prevSize -= ofst;
#endif
			return *this;
		}

		ByteArray operator+(UOSInt ofst)
		{
			CheckError(ofst);
#if defined(CHECK_RANGE)
			return ByteArray(&buff[ofst], this->buffSize - ofst, this->prevSize + ofst);
#else
			return ByteArray(&buff[ofst], this->buffSize - ofst);
#endif
		}

		ByteArray operator+(OSInt ofst)
		{
#if defined(CHECK_RANGE)
			if (ofst < 0)
				CheckErrorPrev((UOSInt)-ofst);
			else
				CheckError((UOSInt)ofst);
			return ByteArray(&buff[ofst], this->buffSize - (UOSInt)ofst, this->prevSize + (UOSInt)ofst);
#else
			return ByteArray(&buff[ofst], this->buffSize - (UOSInt)ofst);
#endif
		}

#if _OSINT_SIZE == 64
		ByteArray operator+(UInt32 ofst)
		{
			CheckError(ofst);
#if defined(CHECK_RANGE)
			return ByteArray(&buff[ofst], this->buffSize - ofst, this->prevSize + ofst);
#else
			return ByteArray(&buff[ofst], this->buffSize - ofst);
#endif
		}

		ByteArray operator+(Int32 ofst)
		{
#if defined(CHECK_RANGE)
			if (ofst < 0)
				CheckErrorPrev((UOSInt)-(OSInt)ofst);
			else
				CheckError((UOSInt)ofst);
			return ByteArray(&buff[ofst], this->buffSize - (UOSInt)(OSInt)ofst, this->prevSize + (UOSInt)(OSInt)ofst);
#else
			return ByteArray(&buff[ofst], this->buffSize - (UOSInt)(OSInt)ofst);
#endif
		}

		ByteArray operator-(Int32 ofst)
		{
#if defined(CHECK_RANGE)
			if (ofst >= 0)
				CheckErrorPrev((UOSInt)ofst);
			else
				CheckError((UOSInt)-(OSInt)ofst);
			return ByteArray(&buff[-ofst], this->buffSize + (UOSInt)(OSInt)ofst, this->prevSize - (UOSInt)(OSInt)ofst);
#else
			return ByteArray(&buff[-ofst], this->buffSize + (UOSInt)(OSInt)ofst);
#endif
		}
#endif

		ByteArray operator-(UOSInt ofst)
		{
			CheckErrorPrev(ofst);
#if defined(CHECK_RANGE)
			return ByteArray(&buff[-(OSInt)ofst], this->buffSize + ofst, this->prevSize - ofst);
#else
			return ByteArray(&buff[-(OSInt)ofst], this->buffSize + ofst);
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
		ByteArrayR(const UInt8 *buff, UOSInt buffSize, UOSInt prevSize) : ByteArrayBase(buff, buffSize)
		{
			this->prevSize = prevSize;
		}
#endif

	public:
		ByteArrayR(const ByteArray &arr) : ByteArrayBase(arr.GetPtr(), arr.GetSize())
		{
#if defined(CHECK_RANGE)
			this->prevSize = arr.GetPrevSize();
#endif
		}

		ByteArrayR(const UInt8 *buff, UOSInt buffSize) : ByteArrayBase(buff, buffSize)
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
			return ByteArrayR(&buff[ofst], size);
		}
		
		ByteArrayR SubArray(UOSInt ofst) const
		{
			CheckError(ofst);
			return ByteArrayR(&buff[ofst], this->buffSize - ofst);
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
			this->buff = &buff[ofst];
			this->buffSize -= ofst;
#if defined(CHECK_RANGE)
			this->prevSize += ofst;
#endif
			return *this;
		}

		ByteArrayR &operator+=(OSInt ofst)
		{
#if defined(CHECK_RANGE)
			if (ofst < 0)
				CheckErrorPrev((UOSInt)-ofst);
			else
				CheckError((UOSInt)ofst);
#endif
			this->buff = &buff[ofst];
			this->buffSize -= (UOSInt)ofst;
#if defined(CHECK_RANGE)
			this->prevSize += (UOSInt)ofst;
#endif
			return *this;
		}

#if _OSINT_SIZE == 64
		ByteArrayR &operator+=(UInt32 ofst)
		{
			CheckError(ofst);
			this->buff = &buff[ofst];
			this->buffSize -= ofst;
#if defined(CHECK_RANGE)
			this->prevSize += ofst;
#endif
			return *this;
		}

		ByteArrayR &operator+=(Int32 ofst)
		{
#if defined(CHECK_RANGE)
			if (ofst < 0)
				CheckErrorPrev((UOSInt)-(OSInt)ofst);
			else
				CheckError((UOSInt)ofst);
#endif
			this->buff = &buff[ofst];
			this->buffSize -= (UOSInt)(OSInt)ofst;
#if defined(CHECK_RANGE)
			this->prevSize += (UOSInt)(OSInt)ofst;
#endif
			return *this;
		}
#endif
	};

	void FORCEINLINE ByteArray::CopyArray(Data::ByteArray destArr, const ByteArrayR &srcArr)
	{
		destArr.CheckError(srcArr.GetSize());
		MemCopyNO(destArr.buff, srcArr.GetPtr(), srcArr.GetSize());
	}
}
#define BYTEARR(var) Data::ByteArray(var, sizeof(var))
#endif