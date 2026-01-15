#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/ByteTool_C.h"
#include "Math/BigIntLSB.h"
#include "Math/BigIntUtil.h"
#include "Text/MyString.h"

Math::BigIntLSB::BigIntLSB(UIntOS valSize)
{
	if (valSize & 15)
	{
		valSize = valSize + 16 - (valSize & 15);
	}
	if (valSize < 16)
		valSize = 16;
	this->valCnt = valSize / sizeof(UIntOS);
	this->valArr = MemAllocA(UIntOS, this->valCnt);
	this->tmpArr = MemAllocA(UIntOS, this->valCnt);
}

Math::BigIntLSB::BigIntLSB(UIntOS valSize, Text::CStringNN val)
{
	if (valSize & 15)
	{
		valSize = valSize + 16 - (valSize & 15);
	}
	if (valSize < 16)
		valSize = 16;
	this->valCnt = valSize / sizeof(UIntOS);
	this->valArr = MemAllocA(UIntOS, this->valCnt);
	this->tmpArr = MemAllocA(UIntOS, this->valCnt);
	this->AssignStr(val.v);
}

Math::BigIntLSB::BigIntLSB(UIntOS valSize, UnsafeArray<const UInt8> val)
{
	if (valSize & 15)
	{
		valSize = valSize + 16 - (valSize & 15);
	}
	if (valSize < 16)
		valSize = 16;
	this->valCnt = valSize / sizeof(UIntOS);
	this->valArr = MemAllocAArr(UIntOS, this->valCnt);
	this->tmpArr = MemAllocAArr(UIntOS, this->valCnt);
	MemCopyAC(this->valArr.Ptr(), val.Ptr(), valSize);
}


Math::BigIntLSB::~BigIntLSB()
{
	MemFreeAArr(this->valArr);
	MemFreeAArr(this->tmpArr);
}

Bool Math::BigIntLSB::IsNeg() const
{
	return ((IntOS)this->valArr[this->valCnt - 1]) < 0;
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
	UIntOS i = this->valCnt;
	UnsafeArray<UIntOS> srcPtr = this->valArr;
	UnsafeArray<UIntOS> destPtr = (this->tmpArr + i - 1);
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
	UnsafeArray<UIntOS> vArr = this->valArr;
	this->valArr = this->tmpArr;
	this->tmpArr = vArr;
}

void Math::BigIntLSB::SetRandom(NN<Data::Random> rnd)
{
	UnsafeArray<Int32> ptr = UnsafeArray<Int32>::ConvertFrom(this->valArr);
	UIntOS iSize = this->valCnt * sizeof(UIntOS) >> 2;
	while (iSize-- > 0)
		*ptr++ = rnd->NextInt32();
}

void Math::BigIntLSB::FromBytesMSB(UnsafeArray<const UInt8> valBuff, UIntOS buffLen)
{
	UnsafeArray<UInt8> destPtr = UnsafeArray<UInt8>::ConvertFrom(this->valArr);
	UIntOS valSize = this->valCnt * sizeof(UIntOS);
	if (valSize > buffLen)
	{
		UIntOS i = buffLen;
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

UIntOS Math::BigIntLSB::GetOccupiedSize() const
{
	UIntOS size = this->valCnt * sizeof(UIntOS);
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

UIntOS Math::BigIntLSB::GetStoreSize() const
{
	return this->valCnt * sizeof(UIntOS);
}

UIntOS Math::BigIntLSB::GetBytesMSB(UnsafeArray<UInt8> byteBuff, Bool occupiedOnly) const
{
	UIntOS size;
	if (occupiedOnly)
		size = this->GetOccupiedSize();
	else
		size = this->valCnt * sizeof(UIntOS);
	UIntOS i = size;
	UnsafeArray<const UInt8> valArr = UnsafeArray<const UInt8>::ConvertFrom(this->valArr);
	while (i-- > 0)
	{
		*byteBuff++ = valArr[i];
	}
	return size;
}

Bool Math::BigIntLSB::EqualsToUI32(UInt32 val)
{
	UIntOS i = this->valCnt - 1;
	UnsafeArray<UIntOS> buff = UnsafeArray<UIntOS>::ConvertFrom(this->valArr);
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
	UIntOS i = this->valCnt - 1;
	UnsafeArray<IntOS> buff = UnsafeArray<IntOS>::ConvertFrom(this->valArr);
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
	BigIntUtil::LSBAssignI(this->valArr, this->valCnt, val);
}

void Math::BigIntLSB::AssignU32(UInt32 val)
{
	BigIntUtil::LSBAssignU(this->valArr, this->valCnt, val);
}

void Math::BigIntLSB::AssignI64(Int64 val)
{
#if _OSINT_SIZE == 64
	BigIntUtil::LSBAssignI(this->valArr, this->valCnt, val);
#else
	this->valArr[0] = (UInt32)(val & 0xffffffff);
	BigIntUtil::LSBAssignI(this->valArr + 1, this->valCnt - 1, (Int32)(val >> 32));
#endif
}

void Math::BigIntLSB::AssignU64(UInt64 val)
{
#if _OSINT_SIZE == 64
	BigIntUtil::LSBAssignU(this->valArr, this->valCnt, val);
#else
	this->valArr[0] = (UInt32)(val & 0xffffffff);
	BigIntUtil::LSBAssignU(this->valArr + 1, this->valCnt - 1, (UInt32)(val >> 32));
#endif
}

void Math::BigIntLSB::AssignStr(UnsafeArray<const UTF8Char> val)
{
	BigIntUtil::LSBAssignStr(this->valArr, this->valCnt, val);
}

void Math::BigIntLSB::AssignBI(NN<const BigIntLSB> val)
{
	if (this->valCnt <= val->valCnt)
	{
		MemCopyAC(this->valArr.Ptr(), val->valArr.Ptr(), this->valCnt * sizeof(UIntOS));
	}
	else
	{
		MemCopyAC(this->valArr.Ptr(), val->valArr.Ptr(), val->valCnt * sizeof(UIntOS));
		if (val->IsNeg())
		{
			MemFillB((UInt8*)&this->valArr[val->valCnt], (this->valCnt - val->valCnt) * sizeof(UIntOS), 0xff);
		}
		else
		{
			MemClearAC(&this->valArr[val->valCnt], this->valCnt - val->valCnt);
		}
	}
}

void Math::BigIntLSB::Neg()
{
	BigIntUtil::LSBNeg(this->valArr, this->valCnt);
}

void Math::BigIntLSB::AndBI(NN<const BigIntLSB> val)
{
	BigIntUtil::LSBAnd(this->valArr, val->valArr, this->valCnt, val->valCnt);
}

void Math::BigIntLSB::OrBI(NN<const BigIntLSB> val)
{
	BigIntUtil::LSBOr(this->valArr, val->valArr, this->valCnt, val->valCnt);
}

void Math::BigIntLSB::XorBI(NN<const BigIntLSB> val)
{
	BigIntUtil::LSBXor(this->valArr, val->valArr, this->valCnt, val->valCnt);
}

Bool Math::BigIntLSB::SetFactorial(UIntOS val)
{
	UIntOS i;
	*this = 2;
	i = 3;
	while (i <= val)
	{
		if (BigIntUtil::LSBMulUOS(this->valArr, this->valCnt, i) != 0)
		{
			return true;
		}
		i++;
	}
	return false;
}

UIntOS Math::BigIntLSB::MultiplyBy(UIntOS val)
{
	return BigIntUtil::LSBMulUOS(this->valArr, this->valCnt, val);
}

UIntOS Math::BigIntLSB::DivideBy(UIntOS val)
{
	return BigIntUtil::LSBDivUOS(this->valArr, this->valCnt, val);
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
	BigIntUtil::LSBAdd(this->valArr, val->valArr, this->valCnt, val->valCnt);
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

NN<Math::BigIntLSB> Math::BigIntLSB::operator *=(UIntOS val)
{
	BigIntUtil::LSBMulUOS(this->valArr, this->valCnt, val);
	return *this;
}

NN<Math::BigIntLSB> Math::BigIntLSB::operator /=(UIntOS val)
{
	BigIntUtil::LSBDivUOS(this->valArr, this->valCnt, val);
	return *this;
}

Bool Math::BigIntLSB::operator ==(NN<const BigIntLSB> val)
{
	UIntOS thisCnt = this->valCnt;
	UIntOS valCnt = val->valCnt;
	UIntOS i;
	UIntOS v;
	UnsafeArray<UIntOS> thisVal = this->valArr;
	UnsafeArray<UIntOS> valVal = val->valArr;
	if (thisCnt > valCnt)
	{
		i = 0;
		while (i < valCnt)
		{
			if (thisVal[i] != valVal[i]) return false;
			i++;
		}
		v = ((IntOS)valVal[valCnt - 1] < 0)?(UIntOS)-1:0;
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
		v = ((IntOS)thisVal[thisCnt - 1] < 0)?(UIntOS)-1:0;
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
	return BigIntUtil::LSBToString(buff, this->valArr, this->tmpArr, this->valCnt);
}

UnsafeArray<UTF8Char> Math::BigIntLSB::ToHex(UnsafeArray<UTF8Char> buff)
{
	UnsafeArray<UTF8Char> currPtr = buff;
	UIntOS vSize = this->valCnt * sizeof(UIntOS);
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
	UIntOS vSize = this->valCnt * sizeof(UIntOS);
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
	UnsafeArray<UTF8Char> sbuff = MemAllocArr(UTF8Char, this->valCnt * 3 * sizeof(UIntOS) + 1);
	UnsafeArray<UTF8Char> sptr = ToString(sbuff);
	sb->AppendP(sbuff, sptr);
	MemFreeArr(sbuff);
}
