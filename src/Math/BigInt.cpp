#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/BigInt.h"
#include "Text/MyString.h"
#include "Data/ByteTool.h"

extern "C"
{
	void BigInt_ByteSwap(const UInt8 *srcBuff, UInt8 *destBuff, OSInt valSize);
	void BigInt_Neg(UInt8 *valBuff, OSInt valSize);
	void BigInt_Add(UInt8 *destBuff, UInt8 *srcBuff, OSInt destSize, OSInt srcSize);
	void BigInt_And(UInt8 *destBuff, UInt8 *srcBuff, OSInt destSize, OSInt srcSize);
	void BigInt_Or(UInt8 *destBuff, UInt8 *srcBuff, OSInt destSize, OSInt srcSize);
	void BigInt_Xor(UInt8 *destBuff, UInt8 *srcBuff, OSInt destSize, OSInt srcSize);
	void BigInt_AssignI32(UInt8 *valBuff, OSInt valSize, Int32 val);
	void BigInt_AssignStr(UInt8 *valBuff, OSInt valSize, const UTF8Char *val);
	UInt32 BigInt_MulUI32(UInt8 *valBuff, OSInt valSize, UInt32 val); //return overflow value
	UInt32 BigInt_DivUI32(UInt8 *valBuff, OSInt valSize, UInt32 val); //return remainder

	UTF8Char *BigInt_ToString(UTF8Char *buff, const UInt8 *valArr, UInt8 *tmpArr, OSInt valSize);
}

Math::BigInt::BigInt(Int32 valSize)
{
	if (valSize & 15)
	{
		valSize = valSize + 16 - (valSize & 15);
	}
	if (valSize < 16)
		valSize = 16;
	this->valArr = MemAllocA(UInt8, this->valSize = valSize);
	this->tmpArr = MemAllocA(UInt8, valSize);
	*this = 0;
}

Math::BigInt::BigInt(Int32 valSize, const UTF8Char *val)
{
	if (valSize & 15)
	{
		valSize = valSize + 16 - (valSize & 15);
	}
	if (valSize < 16)
		valSize = 16;
	this->valArr = MemAllocA(UInt8, this->valSize = valSize);
	this->tmpArr = MemAllocA(UInt8, valSize);
	*this = val;
}

Math::BigInt::BigInt(Int32 valSize, UInt8 *val)
{
	if (valSize & 15)
	{
		valSize = valSize + 16 - (valSize & 15);
	}
	if (valSize < 16)
		valSize = 16;

	this->valArr = MemAllocA(UInt8, this->valSize = valSize);
	this->tmpArr = MemAllocA(UInt8, valSize);
	MemCopyAC(this->valArr, val, valSize);
}


Math::BigInt::~BigInt()
{
	MemFreeA(this->valArr);
	MemFreeA(this->tmpArr);
}

Bool Math::BigInt::IsNeg() const
{
	return (this->valArr[this->valSize - 1] & 0x80) != 0;
}

Bool Math::BigInt::IsOdd() const
{
	return (this->valArr[0] & 1) != 0;
}

Bool Math::BigInt::IsEven() const
{
	return (this->valArr[0] & 1) == 0;
}

Math::BigInt *Math::BigInt::ByteSwap()
{
	BigInt_ByteSwap(this->valArr, this->tmpArr, this->valSize);
	UInt8 *vArr = this->valArr;
	this->valArr = this->tmpArr;
	this->tmpArr = vArr;
	return this;
}

void Math::BigInt::SetRandom(Data::Random *rnd)
{
	Int32 *ptr = (Int32*)this->valArr;
	Int32 iSize = valSize >> 2;
	while (iSize-- > 0)
		*ptr++ = rnd->NextInt32();
}

Bool Math::BigInt::EqualsToUI32(UInt32 val)
{
	OSInt i = (this->valSize >> 2) - 1;
	UInt8 *buff = this->valArr;
	if (*(UInt32*)buff != val)
		return false;
	while (i-- > 0)
	{
		buff += 4;
		if (*(UInt32*)buff != 0)
			return false;
	}
	return true;
}

Bool Math::BigInt::EqualsToI32(Int32 val)
{
	OSInt i = (this->valSize >> 2) - 1;
	UInt8 *buff = this->valArr;
	if (*(Int32*)buff != val)
		return false;
	val = val >> 31;
	while (i-- > 0)
	{
		buff += 4;
		if (*(Int32*)buff != val)
			return false;
	}
	return true;
}

void Math::BigInt::AssignI32(Int32 val)
{
	BigInt_AssignI32(this->valArr, this->valSize, val);
}

void Math::BigInt::AssignStr(const UTF8Char *val)
{
	BigInt_AssignStr(this->valArr, this->valSize, val);
}

void Math::BigInt::AssignBI(const BigInt *val)
{
	if (this->valSize <= val->valSize)
	{
		MemCopyAC(this->valArr, val->valArr, this->valSize);
	}
	else
	{
		MemCopyAC(this->valArr, val->valArr, val->valSize);
		if (val->IsNeg())
		{
			MemFillB(&this->valArr[val->valSize], this->valSize - val->valSize, 0xff);
		}
		else
		{
			MemClearAC(&this->valArr[val->valSize], this->valSize - val->valSize);
		}
	}
}

void Math::BigInt::Neg()
{
	BigInt_Neg(this->valArr, this->valSize);
}

void Math::BigInt::AndBI(const BigInt *val)
{
	BigInt_And(this->valArr, val->valArr, this->valSize, val->valSize);
}

void Math::BigInt::OrBI(const BigInt *val)
{
	BigInt_Or(this->valArr, val->valArr, this->valSize, val->valSize);
}

void Math::BigInt::XorBI(const BigInt *val)
{
	BigInt_Xor(this->valArr, val->valArr, this->valSize, val->valSize);
}

Bool Math::BigInt::SetFactorial(UInt32 val)
{
	UInt32 i;
	*this = 2;
	i = 3;
	while (i <= val)
	{
		if (BigInt_MulUI32(this->valArr, this->valSize, i) != 0)
		{
			return true;
		}
		i++;
	}
	return false;
}

UInt32 Math::BigInt::MultiplyBy(UInt32 val)
{
	return BigInt_MulUI32(this->valArr, this->valSize, val);
}

UInt32 Math::BigInt::DivideBy(UInt32 val)
{
	return BigInt_DivUI32(this->valArr, this->valSize, val);
}

Int32 Math::BigInt::operator =(Int32 val)
{
	this->AssignI32(val);
	return val;
}

Math::BigInt *Math::BigInt::operator =(const UTF8Char *val)
{
	this->AssignStr(val);
	return this;
}

Math::BigInt *Math::BigInt::operator =(const BigInt *val)
{
	this->AssignBI(val);
	return this;
}

Math::BigInt *Math::BigInt::operator +=(Math::BigInt *val)
{
	BigInt_Add(this->valArr, val->valArr, this->valSize, val->valSize);
	return this;
}

Math::BigInt *Math::BigInt::operator ^=(const Math::BigInt *val)
{
	this->XorBI(val);
	return this;
}

Math::BigInt *Math::BigInt::operator &=(const Math::BigInt *val)
{
	this->AndBI(val);
	return this;
}

Math::BigInt *Math::BigInt::operator |=(const Math::BigInt *val)
{
	this->OrBI(val);
	return this;
}

Math::BigInt *Math::BigInt::operator *=(UInt32 val)
{
	BigInt_MulUI32(this->valArr, this->valSize, val);
	return this;
}

Math::BigInt *Math::BigInt::operator /=(UInt32 val)
{
	BigInt_DivUI32(this->valArr, this->valSize, val);
	return this;
}

UTF8Char *Math::BigInt::ToString(UTF8Char *buff)
{
	return BigInt_ToString(buff, this->valArr, this->tmpArr, this->valSize);
}

UTF8Char *Math::BigInt::ToHex(UTF8Char *buff)
{
	UTF8Char *currPtr = buff;
	Int32 vSize = this->valSize;
	UInt8 *ptr = this->valArr + vSize;
	while (vSize--)
	{
		currPtr = Text::StrHexByte(currPtr, *--ptr);
	}
	*currPtr = 0;
	return currPtr;
}

UTF8Char *Math::BigInt::ToByteStr(UTF8Char *buff)
{
	UTF8Char *currPtr = buff;
	Int32 vSize = this->valSize;
	UInt8 *ptr = this->valArr;
	while (vSize--)
	{
		currPtr = Text::StrHexByte(currPtr, *ptr++);
		*currPtr++ = ' ';
	}
	*--currPtr = 0;
	return currPtr;
}
