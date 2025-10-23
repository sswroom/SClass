#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Math/BigIntLSB.h"
#include "Math/BigIntUtil.h"
#include "Text/MyString.h"

Math::BigIntLSB::BigIntLSB(UOSInt valSize)
{
	if (valSize & 15)
	{
		valSize = valSize + 16 - (valSize & 15);
	}
	if (valSize < 16)
		valSize = 16;
	this->valCnt = valSize / sizeof(UOSInt);
	this->valArr = MemAllocA(UOSInt, this->valCnt);
	this->tmpArr = MemAllocA(UOSInt, this->valCnt);
}

Math::BigIntLSB::BigIntLSB(UOSInt valSize, Text::CStringNN val)
{
	if (valSize & 15)
	{
		valSize = valSize + 16 - (valSize & 15);
	}
	if (valSize < 16)
		valSize = 16;
	this->valCnt = valSize / sizeof(UOSInt);
	this->valArr = MemAllocA(UOSInt, this->valCnt);
	this->tmpArr = MemAllocA(UOSInt, this->valCnt);
	this->AssignStr(val.v);
}

Math::BigIntLSB::BigIntLSB(UOSInt valSize, UnsafeArray<const UInt8> val)
{
	if (valSize & 15)
	{
		valSize = valSize + 16 - (valSize & 15);
	}
	if (valSize < 16)
		valSize = 16;
	this->valCnt = valSize / sizeof(UOSInt);
	this->valArr = MemAllocAArr(UOSInt, this->valCnt);
	this->tmpArr = MemAllocAArr(UOSInt, this->valCnt);
	MemCopyAC(this->valArr.Ptr(), val.Ptr(), valSize);
}


Math::BigIntLSB::~BigIntLSB()
{
	MemFreeAArr(this->valArr);
	MemFreeAArr(this->tmpArr);
}

Bool Math::BigIntLSB::IsNeg() const
{
	return ((OSInt)this->valArr[this->valCnt - 1]) < 0;
}

Bool Math::BigIntLSB::IsOdd() const
{
	return (this->valArr[0] & 1) != 0;
}

Bool Math::BigIntLSB::IsEven() const
{
	return (this->valArr[0] & 1) == 0;
}

void Math::BigIntLSB::ByteSwap()
{
	UOSInt i = this->valCnt;
	UnsafeArray<UOSInt> srcPtr = this->valArr;
	UnsafeArray<UOSInt> destPtr = (this->tmpArr + i - 1);
#if (_OSINT_SIZE == 64)
	while (i-- > 0)
	{
		*destPtr = BSWAPU64(*srcPtr);
		destPtr--;
		srcPtr++;
	}
#else
	while (i-- > 0)
	{
		*destPtr = BSWAPU32(*srcPtr);
		destPtr--;
		srcPtr++;
	}
#endif
	UnsafeArray<UOSInt> vArr = this->valArr;
	this->valArr = this->tmpArr;
	this->tmpArr = vArr;
}

void Math::BigIntLSB::SetRandom(NN<Data::Random> rnd)
{
	UnsafeArray<Int32> ptr = UnsafeArray<Int32>::ConvertFrom(this->valArr);
	UOSInt iSize = this->valCnt * sizeof(UOSInt) >> 2;
	while (iSize-- > 0)
		*ptr++ = rnd->NextInt32();
}

void Math::BigIntLSB::FromBytesMSB(UnsafeArray<const UInt8> valBuff, UOSInt buffLen)
{
	UnsafeArray<UInt8> destPtr = UnsafeArray<UInt8>::ConvertFrom(this->valArr);
	UOSInt valSize = this->valCnt * sizeof(UOSInt);
	if (valSize > buffLen)
	{
		UOSInt i = buffLen;
		while (i-- > 0)
		{
			*destPtr++ = valBuff[i];
		}
		if (valBuff[0] & 0x80)
		{
			i = valSize - buffLen;
			while (i-- > 0)
			{
				*destPtr++ = 0xff;
			}
		}
		else
		{
			MemClear(destPtr.Ptr(), valSize - buffLen);
		}
	}
	else
	{
		buffLen = valSize;
		while (buffLen-- > 0)
		{
			*destPtr++ = valBuff[buffLen];
		}
	}
}

UOSInt Math::BigIntLSB::GetOccupiedSize() const
{
	UOSInt size = this->valCnt * sizeof(UOSInt);
	UnsafeArray<const UInt8> valArr = UnsafeArray<const UInt8>::ConvertFrom(this->valArr);
	if (valArr[size - 1] & 0x80)
	{
		while (size > 0)
		{
			if (valArr[size - 1] != 0xff)
			{
				break;
			}
			size--;
		}
		if (size > 0 && (valArr[size - 1] & 0x80) == 0)
		{
			return size + 1;
		}
		else
		{
			return size;
		}
	}
	else
	{
		while (size > 0)
		{
			if (valArr[size - 1] != 0)
			{
				return size;
			}
			size--;
		}
		return size;
	}
}

UOSInt Math::BigIntLSB::GetStoreSize() const
{
	return this->valCnt * sizeof(UOSInt);
}

UOSInt Math::BigIntLSB::GetBytesMSB(UnsafeArray<UInt8> byteBuff, Bool occupiedOnly) const
{
	UOSInt size;
	if (occupiedOnly)
		size = this->GetOccupiedSize();
	else
		size = this->valCnt * sizeof(UOSInt);
	UOSInt i = size;
	UnsafeArray<const UInt8> valArr = UnsafeArray<const UInt8>::ConvertFrom(this->valArr);
	while (i-- > 0)
	{
		*byteBuff++ = valArr[i];
	}
	return size;
}

Bool Math::BigIntLSB::EqualsToUI32(UInt32 val)
{
	UOSInt i = this->valCnt - 1;
	UnsafeArray<UOSInt> buff = UnsafeArray<UOSInt>::ConvertFrom(this->valArr);
	if (*buff != val)
		return false;
	while (i-- > 0)
	{
		buff++;
		if (*buff != 0)
			return false;
	}
	return true;
}

Bool Math::BigIntLSB::EqualsToI32(Int32 val)
{
	UOSInt i = this->valCnt - 1;
	UnsafeArray<OSInt> buff = UnsafeArray<OSInt>::ConvertFrom(this->valArr);
	if (*buff != val)
		return false;
	val = val >> 31;
	while (i-- > 0)
	{
		buff++;
		if (*buff != val)
			return false;
	}
	return true;
}

void Math::BigIntLSB::AssignI32(Int32 val)
{
	BigIntUtil::LSBAssignI2(this->valArr, this->valCnt, val);
}

void Math::BigIntLSB::AssignU32(UInt32 val)
{
	BigIntUtil::LSBAssignU2(this->valArr, this->valCnt, val);
}

void Math::BigIntLSB::AssignI64(Int64 val)
{
#if _OSINT_SIZE == 64
	BigIntUtil::LSBAssignI2(this->valArr, this->valCnt, val);
#else
	this->valArr[0] = (UInt32)(val & 0xffffffff);
	BigIntUtil::LSBAssignI2(this->valArr + 1, this->valCnt - 1, (Int32)(val >> 32));
#endif
}

void Math::BigIntLSB::AssignU64(UInt64 val)
{
#if _OSINT_SIZE == 64
	BigIntUtil::LSBAssignU2(this->valArr, this->valCnt, val);
#else
	this->valArr[0] = (UInt32)(val & 0xffffffff);
	BigIntUtil::LSBAssignU2(this->valArr + 1, this->valCnt - 1, (UInt32)(val >> 32));
#endif
}

void Math::BigIntLSB::AssignStr(UnsafeArray<const UTF8Char> val)
{
	BigIntUtil::LSBAssignStr2(this->valArr, this->valCnt, val);
}

void Math::BigIntLSB::AssignBI(NN<const BigIntLSB> val)
{
	if (this->valCnt <= val->valCnt)
	{
		MemCopyAC(this->valArr.Ptr(), val->valArr.Ptr(), this->valCnt * sizeof(UOSInt));
	}
	else
	{
		MemCopyAC(this->valArr.Ptr(), val->valArr.Ptr(), val->valCnt * sizeof(UOSInt));
		if (val->IsNeg())
		{
			MemFillB((UInt8*)&this->valArr[val->valCnt], (this->valCnt - val->valCnt) * sizeof(UOSInt), 0xff);
		}
		else
		{
			MemClearAC(&this->valArr[val->valCnt], this->valCnt - val->valCnt);
		}
	}
}

void Math::BigIntLSB::Neg()
{
	BigIntUtil::LSBNeg2(this->valArr, this->valCnt);
}

void Math::BigIntLSB::AndBI(NN<const BigIntLSB> val)
{
	BigIntUtil::LSBAnd2(this->valArr, val->valArr, this->valCnt, val->valCnt);
}

void Math::BigIntLSB::OrBI(NN<const BigIntLSB> val)
{
	BigIntUtil::LSBOr2(this->valArr, val->valArr, this->valCnt, val->valCnt);
}

void Math::BigIntLSB::XorBI(NN<const BigIntLSB> val)
{
	BigIntUtil::LSBXor2(this->valArr, val->valArr, this->valCnt, val->valCnt);
}

Bool Math::BigIntLSB::SetFactorial(UOSInt val)
{
	UOSInt i;
	*this = 2;
	i = 3;
	while (i <= val)
	{
		if (BigIntUtil::LSBMulUOS2(this->valArr, this->valCnt, i) != 0)
		{
			return true;
		}
		i++;
	}
	return false;
}

UOSInt Math::BigIntLSB::MultiplyBy(UOSInt val)
{
	return BigIntUtil::LSBMulUOS2(this->valArr, this->valCnt, val);
}

UOSInt Math::BigIntLSB::DivideBy(UOSInt val)
{
	return BigIntUtil::LSBDivUOS2(this->valArr, this->valCnt, val);
}

Int32 Math::BigIntLSB::operator =(Int32 val)
{
	this->AssignI32(val);
	return val;
}

UInt32 Math::BigIntLSB::operator =(UInt32 val)
{
	this->AssignU32(val);
	return val;
}


Int64 Math::BigIntLSB::operator =(Int64 val)
{
	this->AssignI64(val);
	return val;
}

UInt64 Math::BigIntLSB::operator =(UInt64 val)
{
	this->AssignU64(val);
	return val;
}

NN<Math::BigIntLSB> Math::BigIntLSB::operator =(Text::CStringNN val)
{
	this->AssignStr(val.v);
	return *this;
}

NN<Math::BigIntLSB> Math::BigIntLSB::operator =(NN<const BigIntLSB> val)
{
	this->AssignBI(val);
	return *this;
}

NN<Math::BigIntLSB> Math::BigIntLSB::operator +=(NN<Math::BigIntLSB> val)
{
	BigIntUtil::LSBAdd2(this->valArr, val->valArr, this->valCnt, val->valCnt);
	return *this;
}

NN<Math::BigIntLSB> Math::BigIntLSB::operator ^=(NN<const Math::BigIntLSB> val)
{
	this->XorBI(val);
	return *this;
}

NN<Math::BigIntLSB> Math::BigIntLSB::operator &=(NN<const Math::BigIntLSB> val)
{
	this->AndBI(val);
	return *this;
}

NN<Math::BigIntLSB> Math::BigIntLSB::operator |=(NN<const Math::BigIntLSB> val)
{
	this->OrBI(val);
	return *this;
}

NN<Math::BigIntLSB> Math::BigIntLSB::operator *=(UOSInt val)
{
	BigIntUtil::LSBMulUOS2(this->valArr, this->valCnt, val);
	return *this;
}

NN<Math::BigIntLSB> Math::BigIntLSB::operator /=(UOSInt val)
{
	BigIntUtil::LSBDivUOS2(this->valArr, this->valCnt, val);
	return *this;
}

Bool Math::BigIntLSB::operator ==(NN<const BigIntLSB> val)
{
	UOSInt thisCnt = this->valCnt;
	UOSInt valCnt = val->valCnt;
	UOSInt i;
	UOSInt v;
	UnsafeArray<UOSInt> thisVal = this->valArr;
	UnsafeArray<UOSInt> valVal = val->valArr;
	if (thisCnt > valCnt)
	{
		i = 0;
		while (i < valCnt)
		{
			if (thisVal[i] != valVal[i]) return false;
			i++;
		}
		v = ((OSInt)valVal[valCnt - 1] < 0)?(UOSInt)-1:0;
		while (i < thisCnt)
		{
			if (thisVal[i] != v) return false;
			i++;
		}
	}
	else
	{
		i = 0;
		while (i < thisCnt)
		{
			if (thisVal[i] != valVal[i]) return false;
			i++;
		}
		v = ((OSInt)thisVal[thisCnt - 1] < 0)?(UOSInt)-1:0;
		while (i < valCnt)
		{
			if (valVal[i] != v) return false;
			i++;
		}
	}
	return true;
}

Bool Math::BigIntLSB::operator !=(NN<const BigIntLSB> val)
{
	if (*this == val)
		return false;
	return true;
}

UnsafeArray<UTF8Char> Math::BigIntLSB::ToString(UnsafeArray<UTF8Char> buff) const
{
	return BigIntUtil::LSBToString2(buff, this->valArr, this->tmpArr, this->valCnt);
}

UnsafeArray<UTF8Char> Math::BigIntLSB::ToHex(UnsafeArray<UTF8Char> buff)
{
	UnsafeArray<UTF8Char> currPtr = buff;
	UOSInt vSize = this->valCnt * sizeof(UOSInt);
	UnsafeArray<UInt8> ptr = UnsafeArray<UInt8>::ConvertFrom(this->valArr) + vSize;
	while (vSize--)
	{
		currPtr = Text::StrHexByte(currPtr, *--ptr);
	}
	*currPtr = 0;
	return currPtr;
}

UnsafeArray<UTF8Char> Math::BigIntLSB::ToByteStr(UnsafeArray<UTF8Char> buff)
{
	UnsafeArray<UTF8Char> currPtr = buff;
	UOSInt vSize = this->valCnt * sizeof(UOSInt);
	UnsafeArray<UInt8> ptr = UnsafeArray<UInt8>::ConvertFrom(this->valArr);
	while (vSize--)
	{
		currPtr = Text::StrHexByte(currPtr, *ptr++);
		*currPtr++ = ' ';
	}
	*--currPtr = 0;
	return currPtr;
}

void Math::BigIntLSB::ToString(NN<Text::StringBuilderUTF8> sb) const
{
	UnsafeArray<UTF8Char> sbuff = MemAllocArr(UTF8Char, this->valCnt * 3 * sizeof(UOSInt) + 1);
	UnsafeArray<UTF8Char> sptr = ToString(sbuff);
	sb->AppendP(sbuff, sptr);
	MemFreeArr(sbuff);
}
