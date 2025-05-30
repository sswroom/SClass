#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Math/BigIntLSB.h"
#include "Text/MyString.h"

void BigIntLSB_Neg(UOSInt *valBuff, UOSInt valSize)
{
	valSize = valSize / sizeof(UOSInt);
	UOSInt *tmpPtr = valBuff;
	UOSInt i = valSize;
	while (i-- > 0)
	{
		*tmpPtr = ~*tmpPtr;
		tmpPtr++;
	}
	while (tmpPtr > valBuff)
	{
		if (++*valBuff)
			break;
		valBuff++;
	}
}

void BigIntLSB_Add(UOSInt *destBuff, const UOSInt *srcBuff, UOSInt destSize, UOSInt srcSize)
{
	destSize = destSize / sizeof(UOSInt);
	srcSize = srcSize / sizeof(UOSInt);
	UOSInt i;
	Bool carry = false;
	if (srcSize < destSize)
	{
		destSize -= srcSize;
		i = srcSize;
		while (i-- > 0)
		{
			carry = MyADC_UOS(carry + *srcBuff, *destBuff, destBuff);
			srcBuff++;
			destBuff++;
		}
		if ((OSInt)srcBuff[-1] < 0)
		{
			while (destSize-- > 0)
			{
				carry = MyADC_UOS(carry - 1, *destBuff, destBuff);
				destBuff++;
			}
		}
		else
		{
			while (carry && destSize-- > 0)
			{
				carry = MyADC_UOS(carry, *destBuff, destBuff);
				destBuff++;
			}
		}
	}
	else
	{
		i = srcSize;
		while (i-- > 0)
		{
			carry = MyADC_UOS(carry + *srcBuff, *destBuff, destBuff);
			srcBuff++;
			destBuff++;
		}
	}
}

void BigIntLSB_And(UOSInt *destBuff, const UOSInt *srcBuff, UOSInt destSize, UOSInt srcSize)
{
	destSize = destSize / sizeof(UOSInt);
	srcSize = srcSize / sizeof(UOSInt);
	UOSInt i;
	if (srcSize < destSize)
	{
		destSize -= srcSize;
		i = srcSize;
		while (i-- > 0)
		{
			*destBuff = *srcBuff & *destBuff;
			srcBuff++;
			destBuff++;
		}
		if ((OSInt)srcBuff[-1] >= 0)
		{
			MemClear(destBuff, destSize * sizeof(UOSInt));
		}
	}
	else
	{
		i = srcSize;
		while (i-- > 0)
		{
			*destBuff = *srcBuff & *destBuff;
			srcBuff++;
			destBuff++;
		}
	}
}

void BigIntLSB_Or(UOSInt *destBuff, const UOSInt *srcBuff, UOSInt destSize, UOSInt srcSize)
{
	destSize = destSize / sizeof(UOSInt);
	srcSize = srcSize / sizeof(UOSInt);
	UOSInt i;
	if (srcSize < destSize)
	{
		destSize -= srcSize;
		i = srcSize;
		while (i-- > 0)
		{
			*destBuff = *srcBuff | *destBuff;
			srcBuff++;
			destBuff++;
		}
		if ((OSInt)srcBuff[-1] >= 0)
		{
			while (destSize-- > 0)
			{
				*destBuff = (UOSInt)-1;
				destBuff++;
			}
		}
	}
	else
	{
		i = srcSize;
		while (i-- > 0)
		{
			*destBuff = *srcBuff | *destBuff;
			srcBuff++;
			destBuff++;
		}
	}
}

void BigIntLSB_Xor(UOSInt *destBuff, const UOSInt *srcBuff, UOSInt destSize, UOSInt srcSize)
{
	destSize = destSize / sizeof(UOSInt);
	srcSize = srcSize / sizeof(UOSInt);
	UOSInt i;
	if (srcSize < destSize)
	{
		destSize -= srcSize;
		i = srcSize;
		while (i-- > 0)
		{
			*destBuff = *srcBuff ^ *destBuff;
			srcBuff++;
			destBuff++;
		}
		if ((OSInt)srcBuff[-1] < 0)
		{
			while (destSize-- > 0)
			{
				*destBuff = ~*destBuff;
				destBuff++;
			}
		}
	}
	else
	{
		i = srcSize;
		while (i-- > 0)
		{
			*destBuff = *srcBuff ^ *destBuff;
			srcBuff++;
			destBuff++;
		}
	}
}

UInt32 BigIntLSB_MulUI32(UOSInt *valBuff, UOSInt valSize, UInt32 val) //return overflow value
{
	UOSInt i = valSize / sizeof(UOSInt);
	Bool neg;
	if ((OSInt)valBuff[i - 1] < 0)
	{
		neg = true;
		BigIntLSB_Neg(valBuff, valSize);
	}
	else
	{
		neg = false;
	}
	UOSInt *tmpPtr = valBuff;
	UOSInt hiVal = 0;
	while (i-- > 0)
	{
		UOSInt tmpVal = hiVal;
		hiVal += MyADC_UOS(tmpVal, MyMUL_UOS(*tmpPtr, val, &hiVal), tmpPtr);
		tmpPtr++;
	}
	if (neg)
	{
		BigIntLSB_Neg(valBuff, valSize);
	}
	return (UInt32)hiVal;
}

UInt32 BigIntLSB_DivUI32(UOSInt *valBuff, UOSInt valSize, UInt32 val) //return remainder
{
	UOSInt i = valSize / sizeof(UOSInt);
	Bool neg;
	if ((OSInt)valBuff[i - 1] < 0)
	{
		neg = true;
		BigIntLSB_Neg(valBuff, valSize);
	}
	else
	{
		neg = false;
	}
	UOSInt reminder = 0;
	while (i-- > 0)
	{
		valBuff[i] = MyDIV_UOS(valBuff[i], reminder, val, &reminder);
	}
	if (neg)
	{
		BigIntLSB_Neg(valBuff, valSize);
	}
	return (UInt32)reminder;
}

void BigIntLSB_AssignI32(UOSInt *valBuff, UOSInt valSize, Int32 val)
{
	valSize = valSize / sizeof(UOSInt);
	UOSInt v;
	*(OSInt*)valBuff = (OSInt)val;
	if (val < 0)
	{
		v = (UOSInt)-1;
	}
	else
	{
		v = 0;
	}
	valBuff++;
	valSize--;
	while (valSize-- > 0)
	{
		*valBuff++ = v;
	}
}

void BigIntLSB_AssignStr(UOSInt *valBuff, UOSInt valSize, const UTF8Char *val)
{
	valSize = valSize / sizeof(UOSInt);
	UOSInt maxSize = 0;
	UOSInt *valPtr;
	Bool neg;
	UTF8Char c;
	if (val[0] == '-')
	{
		neg = true;
		val++;
	}
	else
	{
		neg = false;
	}
	while (true)
	{
		c = *val++;
		if (c == 0)
		{
			break;
		}
		if (c < '0' || c > '9')
		{
			MemClear(valBuff, valSize * sizeof(UOSInt));
			return;
		}
		UOSInt v = (UOSInt)c - '0';
		UOSInt v2;
		UOSInt i = maxSize;
		valPtr = valBuff;
		while (i-- > 0)
		{
			v2 += MyADC_UOS(MyMUL_UOS(*valPtr, 10, &v2), v, valPtr);
			v = v2;
			valPtr++;
		}
		if (v && maxSize < valSize)
		{
			maxSize++;
			*valPtr = v;
		}
	}
	if (maxSize < valSize)
	{
		MemClear(&valBuff[maxSize], (valSize - maxSize) * sizeof(UOSInt));
	}
	if (neg)
	{
		BigIntLSB_Neg(valBuff, valSize * sizeof(UOSInt));
	}
}

UTF8Char *BigIntLSB_ToString(UTF8Char *buff, const UOSInt *valArr, UOSInt *tmpArr, UOSInt valSize)
{
	UTF8Char *buffEnd = buff + valSize * 3;
	UTF8Char *buffCurr = buffEnd;
	MemCopyNO(tmpArr, valArr, valSize);
	valSize = valSize / sizeof(UOSInt);
	if ((OSInt)tmpArr[valSize - 1] < 0)
	{
		*buff++ = '-';
		BigIntLSB_Neg(tmpArr, valSize * sizeof(UOSInt));
	}
	UOSInt maxSize = valSize;
	while (maxSize > 0 && tmpArr[maxSize - 1] == 0)
	{
		maxSize--;
	}
	if (maxSize == 0)
	{
		*buff++ = '0';
		*buff = 0;
		return buff;
	}
	while (maxSize > 0)
	{
		UOSInt reminder = 0;
		UOSInt i = maxSize;
		while (i-- > 0)
		{
			tmpArr[i] = MyDIV_UOS(tmpArr[i], reminder, 10, &reminder);
		}
		*--buffCurr = (UTF8Char)('0' + reminder);
		if (tmpArr[maxSize - 1] == 0)
			maxSize--;
	}
	return Text::StrConcatC(buff, buffCurr, (UOSInt)(buffEnd - buffCurr)).Ptr();
}

Math::BigIntLSB::BigIntLSB(UOSInt valSize)
{
	if (valSize & 15)
	{
		valSize = valSize + 16 - (valSize & 15);
	}
	if (valSize < 16)
		valSize = 16;
	this->valSize = valSize;
	this->valArr = MemAllocA(UOSInt, valSize / sizeof(UOSInt));
	this->tmpArr = MemAllocA(UOSInt, valSize / sizeof(UOSInt));
}

Math::BigIntLSB::BigIntLSB(UOSInt valSize, Text::CStringNN val)
{
	if (valSize & 15)
	{
		valSize = valSize + 16 - (valSize & 15);
	}
	if (valSize < 16)
		valSize = 16;
	this->valSize = valSize;
	this->valArr = MemAllocA(UOSInt, valSize / sizeof(UOSInt));
	this->tmpArr = MemAllocA(UOSInt, valSize / sizeof(UOSInt));
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
	this->valSize = valSize;
	this->valArr = MemAllocAArr(UOSInt, valSize / sizeof(UOSInt));
	this->tmpArr = MemAllocAArr(UOSInt, valSize / sizeof(UOSInt));
	MemCopyAC(this->valArr.Ptr(), val.Ptr(), valSize);
}


Math::BigIntLSB::~BigIntLSB()
{
	MemFreeAArr(this->valArr);
	MemFreeAArr(this->tmpArr);
}

Bool Math::BigIntLSB::IsNeg() const
{
	return (this->valArr[this->valSize - 1] & 0x80) != 0;
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
	UOSInt i = this->valSize / sizeof(UOSInt);
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
	UOSInt iSize = valSize >> 2;
	while (iSize-- > 0)
		*ptr++ = rnd->NextInt32();
}

void Math::BigIntLSB::FromBytesMSB(UnsafeArray<const UInt8> valBuff, UOSInt buffLen)
{
	UnsafeArray<UInt8> destPtr = UnsafeArray<UInt8>::ConvertFrom(this->valArr);
	if (this->valSize > buffLen)
	{
		UOSInt i = buffLen;
		while (i-- > 0)
		{
			*destPtr++ = valBuff[i];
		}
		if (valBuff[0] & 0x80)
		{
			i = this->valSize - buffLen;
			while (i-- > 0)
			{
				*destPtr++ = 0xff;
			}
		}
		else
		{
			MemClear(destPtr.Ptr(), this->valSize - buffLen);
		}
	}
	else
	{
		buffLen = this->valSize;
		while (buffLen-- > 0)
		{
			*destPtr++ = valBuff[buffLen];
		}
	}
}

UOSInt Math::BigIntLSB::GetOccupiedSize() const
{
	UOSInt size = this->valSize;
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
	return this->valSize;
}

UOSInt Math::BigIntLSB::GetBytesMSB(UnsafeArray<UInt8> byteBuff, Bool occupiedOnly) const
{
	UOSInt size;
	if (occupiedOnly)
		size = this->GetOccupiedSize();
	else
		size = this->valSize;
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
	UOSInt i = (this->valSize >> 2) - 1;
	UnsafeArray<UInt32> buff = UnsafeArray<UInt32>::ConvertFrom(this->valArr);
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
	UOSInt i = (this->valSize >> 2) - 1;
	UnsafeArray<Int32> buff = UnsafeArray<Int32>::ConvertFrom(this->valArr);
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
	BigIntLSB_AssignI32(this->valArr.Ptr(), this->valSize, val);
}

void Math::BigIntLSB::AssignStr(UnsafeArray<const UTF8Char> val)
{
	BigIntLSB_AssignStr(this->valArr.Ptr(), this->valSize, val.Ptr());
}

void Math::BigIntLSB::AssignBI(NN<const BigIntLSB> val)
{
	if (this->valSize <= val->valSize)
	{
		MemCopyAC(this->valArr.Ptr(), val->valArr.Ptr(), this->valSize);
	}
	else
	{
		MemCopyAC(this->valArr.Ptr(), val->valArr.Ptr(), val->valSize);
		if (val->IsNeg())
		{
			MemFillB((UInt8*)&this->valArr[val->valSize], this->valSize - val->valSize, 0xff);
		}
		else
		{
			MemClearAC(&this->valArr[val->valSize], this->valSize - val->valSize);
		}
	}
}

void Math::BigIntLSB::Neg()
{
	BigIntLSB_Neg(this->valArr.Ptr(), this->valSize);
}

void Math::BigIntLSB::AndBI(NN<const BigIntLSB> val)
{
	BigIntLSB_And(this->valArr.Ptr(), val->valArr.Ptr(), this->valSize, val->valSize);
}

void Math::BigIntLSB::OrBI(NN<const BigIntLSB> val)
{
	BigIntLSB_Or(this->valArr.Ptr(), val->valArr.Ptr(), this->valSize, val->valSize);
}

void Math::BigIntLSB::XorBI(NN<const BigIntLSB> val)
{
	BigIntLSB_Xor(this->valArr.Ptr(), val->valArr.Ptr(), this->valSize, val->valSize);
}

Bool Math::BigIntLSB::SetFactorial(UInt32 val)
{
	UInt32 i;
	*this = 2;
	i = 3;
	while (i <= val)
	{
		if (BigIntLSB_MulUI32(this->valArr.Ptr(), this->valSize, i) != 0)
		{
			return true;
		}
		i++;
	}
	return false;
}

UInt32 Math::BigIntLSB::MultiplyBy(UInt32 val)
{
	return BigIntLSB_MulUI32(this->valArr.Ptr(), this->valSize, val);
}

UInt32 Math::BigIntLSB::DivideBy(UInt32 val)
{
	return BigIntLSB_DivUI32(this->valArr.Ptr(), this->valSize, val);
}

Int32 Math::BigIntLSB::operator =(Int32 val)
{
	this->AssignI32(val);
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
	BigIntLSB_Add(this->valArr.Ptr(), val->valArr.Ptr(), this->valSize, val->valSize);
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

NN<Math::BigIntLSB> Math::BigIntLSB::operator *=(UInt32 val)
{
	BigIntLSB_MulUI32(this->valArr.Ptr(), this->valSize, val);
	return *this;
}

NN<Math::BigIntLSB> Math::BigIntLSB::operator /=(UInt32 val)
{
	BigIntLSB_DivUI32(this->valArr.Ptr(), this->valSize, val);
	return *this;
}

UnsafeArray<UTF8Char> Math::BigIntLSB::ToString(UnsafeArray<UTF8Char> buff) const
{
	return BigIntLSB_ToString(buff.Ptr(), this->valArr.Ptr(), this->tmpArr.Ptr(), this->valSize);
}

UnsafeArray<UTF8Char> Math::BigIntLSB::ToHex(UnsafeArray<UTF8Char> buff)
{
	UnsafeArray<UTF8Char> currPtr = buff;
	UOSInt vSize = this->valSize;
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
	UOSInt vSize = this->valSize;
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
	UnsafeArray<UTF8Char> sbuff = MemAllocArr(UTF8Char, this->valSize * 3 + 1);
	UnsafeArray<UTF8Char> sptr = ToString(sbuff);
	sb->AppendP(sbuff, sptr);
	MemFreeArr(sbuff);
}
