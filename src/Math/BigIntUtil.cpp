#include "Stdafx.h"
#include "Math/BigIntUtil.h"
#include "Text/MyString.h"

void Math::BigIntUtil::LSBNeg(UnsafeArray<UOSInt> valBuff, UOSInt valCnt)
{
	UnsafeArray<UOSInt> tmpPtr = valBuff;
	while (valCnt-- > 0)
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

void Math::BigIntUtil::LSBAdd(UnsafeArray<UOSInt> destBuff, UnsafeArray<const UOSInt> srcBuff, UOSInt destCnt, UOSInt srcCnt)
{
	UOSInt i;
	UOSInt carry = 0;
	if (srcCnt < destCnt)
	{
		destCnt -= srcCnt;
		i = srcCnt;
		while (i-- > 0)
		{
			*destBuff = MyADC_UOS(*srcBuff, *destBuff, carry, &carry);
			srcBuff++;
			destBuff++;
		}
		if ((OSInt)srcBuff[-1] < 0)
		{
			while (destCnt-- > 0)
			{
				carry = MyADD_UOS(carry - 1, *destBuff, destBuff.Ptr());
				destBuff++;
			}
		}
		else
		{
			while (carry && destCnt-- > 0)
			{
				carry = MyADD_UOS(1, *destBuff, destBuff.Ptr());
				destBuff++;
			}
		}
	}
	else
	{
		i = destCnt;
		while (i-- > 0)
		{
			*destBuff = MyADC_UOS(*srcBuff, *destBuff, carry, &carry);
			srcBuff++;
			destBuff++;
		}
	}
}

void Math::BigIntUtil::LSBSub(UnsafeArray<UOSInt> destBuff, UnsafeArray<const UOSInt> srcBuff, UOSInt destCnt, UOSInt srcCnt)
{
	UOSInt i;
	UOSInt carry = 0;
	if (srcCnt < destCnt)
	{
		destCnt -= srcCnt;
		i = srcCnt;
		while (i-- > 0)
		{
			*destBuff = MySBB_UOS(*destBuff, *srcBuff, carry, &carry);
			srcBuff++;
			destBuff++;
		}
		while (carry && destCnt-- > 0)
		{
			carry = MySUB_UOS(*destBuff, carry, destBuff.Ptr());
			destBuff++;
		}
	}
	else
	{
		i = destCnt;
		while (i-- > 0)
		{
			*destBuff = MySBB_UOS(*destBuff, *srcBuff, carry, &carry);
			srcBuff++;
			destBuff++;
		}
	}
}

void Math::BigIntUtil::LSBMul(UnsafeArray<UOSInt> destBuff, UnsafeArray<const UOSInt> srcBuff, UOSInt destCnt, UOSInt srcCnt)
{
	Bool neg = false;
	UnsafeArray<UOSInt> tmpBuff;
	UnsafeArrayOpt<UOSInt> soriBuff = 0;
	UnsafeArray<UOSInt> sBuff;
	UOSInt i;
	UOSInt v;
	UOSInt lastHi;
	tmpBuff = MemAlloc(UOSInt, destCnt + srcCnt);
	if ((OSInt)destBuff[destCnt - 1] < 0)
	{
		LSBNeg(destBuff, destCnt);
		neg = !neg;
	}
	if ((OSInt)srcBuff[srcCnt - 1] < 0)
	{
		sBuff = MemAllocArr(UOSInt, srcCnt);
		soriBuff = sBuff;
		MemCopyNO(sBuff.Ptr(), srcBuff.Ptr(), srcCnt * sizeof(UOSInt));
		srcBuff = sBuff;
		neg = !neg;
	}
	MemClear(&tmpBuff[0], (destCnt + srcCnt) * sizeof(UOSInt));
	sBuff = tmpBuff;
	while (srcCnt > 0)
	{
		v = srcBuff[0];
		lastHi = 0;
		i = 0;
		while (i < destCnt)
		{
			UOSInt tmpVal = lastHi;
			lastHi += MyADD_UOS(sBuff[i], MyMUL_UOS(destBuff[i], v, &lastHi), &sBuff[i]);
			lastHi += MyADD_UOS(sBuff[i], tmpVal, &sBuff[i]);
			i++;
		}
		while (lastHi)
		{
			lastHi = MyADD_UOS(1, sBuff[i], &sBuff[i]);
			i++;
		}
		sBuff++;
		srcBuff++;
		srcCnt--;
	}

	MemCopyNO(destBuff.Ptr(), tmpBuff.Ptr(), destCnt * sizeof(UOSInt));
	if (soriBuff.SetTo(sBuff))
	{
		MemFreeArr(sBuff);
	}
	MemFreeArr(tmpBuff);
	if (neg)
	{
		LSBNeg(destBuff, destCnt);
	}
}

void Math::BigIntUtil::LSBAnd(UnsafeArray<UOSInt> destBuff, UnsafeArray<const UOSInt> srcBuff, UOSInt destCnt, UOSInt srcCnt)
{
	UOSInt i;
	if (srcCnt < destCnt)
	{
		destCnt -= srcCnt;
		i = srcCnt;
		while (i-- > 0)
		{
			*destBuff = *srcBuff & *destBuff;
			srcBuff++;
			destBuff++;
		}
		if ((OSInt)srcBuff[-1] >= 0)
		{
			MemClear(&destBuff[0], destCnt * sizeof(UOSInt));
		}
	}
	else
	{
		i = srcCnt;
		while (i-- > 0)
		{
			*destBuff = *srcBuff & *destBuff;
			srcBuff++;
			destBuff++;
		}
	}
}

void Math::BigIntUtil::LSBOr(UnsafeArray<UOSInt> destBuff, UnsafeArray<const UOSInt> srcBuff, UOSInt destCnt, UOSInt srcCnt)
{
	UOSInt i;
	if (srcCnt < destCnt)
	{
		destCnt -= srcCnt;
		i = srcCnt;
		while (i-- > 0)
		{
			*destBuff = *srcBuff | *destBuff;
			srcBuff++;
			destBuff++;
		}
		if ((OSInt)srcBuff[-1] >= 0)
		{
			while (destCnt-- > 0)
			{
				*destBuff = (UOSInt)-1;
				destBuff++;
			}
		}
	}
	else
	{
		i = srcCnt;
		while (i-- > 0)
		{
			*destBuff = *srcBuff | *destBuff;
			srcBuff++;
			destBuff++;
		}
	}
}


void Math::BigIntUtil::LSBXor(UnsafeArray<UOSInt> destBuff, UnsafeArray<const UOSInt> srcBuff, UOSInt destCnt, UOSInt srcCnt)
{
	UOSInt i;
	if (srcCnt < destCnt)
	{
		destCnt -= srcCnt;
		i = srcCnt;
		while (i-- > 0)
		{
			*destBuff = *srcBuff ^ *destBuff;
			srcBuff++;
			destBuff++;
		}
		if ((OSInt)srcBuff[-1] < 0)
		{
			while (destCnt-- > 0)
			{
				*destBuff = ~*destBuff;
				destBuff++;
			}
		}
	}
	else
	{
		i = srcCnt;
		while (i-- > 0)
		{
			*destBuff = *srcBuff ^ *destBuff;
			srcBuff++;
			destBuff++;
		}
	}
}

UOSInt Math::BigIntUtil::LSBAddUOS(UnsafeArray<UOSInt> valBuff, UOSInt valCnt, UOSInt val) //return overflow value
{
	while (val != 0 && valCnt-- > 0)
	{
		val = MyADD_UOS(valBuff[0], val, &valBuff[0]);
		valBuff++;
	}
	return val;
}

UOSInt Math::BigIntUtil::LSBMulUOS(UnsafeArray<UOSInt> valBuff, UOSInt valCnt, UOSInt val) //return overflow value
{
	UOSInt i = valCnt;
	Bool neg;
	if ((OSInt)valBuff[valCnt - 1] < 0)
	{
		neg = true;
		LSBNeg(valBuff, valCnt);
	}
	else
	{
		neg = false;
	}
	UnsafeArray<UOSInt> tmpPtr = valBuff;
	UOSInt hiVal = 0;
	while (i-- > 0)
	{
		UOSInt tmpVal = hiVal;
		hiVal += MyADD_UOS(tmpVal, MyMUL_UOS(*tmpPtr, val, &hiVal), tmpPtr.Ptr());
		tmpPtr++;
	}
	if (neg)
	{
		LSBNeg(valBuff, valCnt);
	}
	return hiVal;
}

UOSInt Math::BigIntUtil::LSBDivUOS(UnsafeArray<UOSInt> valBuff, UOSInt valCnt, UOSInt val) //return remainder
{
	UOSInt i = valCnt;
	Bool neg;
	if ((OSInt)valBuff[valCnt - 1] < 0)
	{
		neg = true;
		LSBNeg(valBuff, valCnt);
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
		LSBNeg(valBuff, valCnt);
	}
	return reminder;
}

void Math::BigIntUtil::LSBAssignI(UnsafeArray<UOSInt> valBuff, UOSInt valCnt, OSInt val)
{
	UOSInt v;
	UnsafeArray<OSInt>::ConvertFrom(valBuff)[0] = val;
	if (val < 0)
	{
		v = (UOSInt)-1;
	}
	else
	{
		v = 0;
	}
	valBuff++;
	valCnt--;
	while (valCnt-- > 0)
	{
		*valBuff++ = v;
	}
}

void Math::BigIntUtil::LSBAssignU(UnsafeArray<UOSInt> valBuff, UOSInt valCnt, UOSInt val)
{
	valBuff[0] = val;
	valBuff++;
	valCnt--;
	while (valCnt-- > 0)
	{
		*valBuff++ = 0;
	}
}

void Math::BigIntUtil::LSBAssignStr(UnsafeArray<UOSInt> valBuff, UOSInt valCnt, UnsafeArray<const UTF8Char> val)
{
	UOSInt maxCnt = 0;
	UnsafeArray<UOSInt> valPtr;
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
			MemClear(&valBuff[0], valCnt * sizeof(UOSInt));
			return;
		}
		UOSInt v = (UOSInt)c - '0';
		UOSInt v2;
		UOSInt i = maxCnt;
		valPtr = valBuff;
		while (i-- > 0)
		{
			v2 += MyADD_UOS(MyMUL_UOS(*valPtr, 10, &v2), v, valPtr.Ptr());
			v = v2;
			valPtr++;
		}
		if (v && maxCnt < valCnt)
		{
			maxCnt++;
			*valPtr = v;
		}
	}
	if (maxCnt < valCnt)
	{
		MemClear(&valBuff[maxCnt], (valCnt - maxCnt) * sizeof(UOSInt));
	}
	if (neg)
	{
		LSBNeg(valBuff, valCnt);
	}
}

UnsafeArray<UTF8Char> Math::BigIntUtil::LSBToString(UnsafeArray<UTF8Char> buff, UnsafeArray<const UOSInt> valArr, UnsafeArray<UOSInt> tmpArr, UOSInt valCnt)
{
	UnsafeArray<UTF8Char> buffEnd = buff + valCnt * 3 * sizeof(UOSInt);
	UnsafeArray<UTF8Char> buffCurr = buffEnd;
	MemCopyNO(tmpArr.Ptr(), valArr.Ptr(), valCnt * sizeof(UOSInt));
	if ((OSInt)tmpArr[valCnt - 1] < 0)
	{
		*buff++ = '-';
		LSBNeg(tmpArr, valCnt);
	}
	UOSInt maxCnt = valCnt;
	while (maxCnt > 0 && tmpArr[maxCnt - 1] == 0)
	{
		maxCnt--;
	}
	if (maxCnt == 0)
	{
		*buff++ = '0';
		*buff = 0;
		return buff;
	}
	while (maxCnt > 0)
	{
		UOSInt reminder = 0;
		UOSInt i = maxCnt;
		while (i-- > 0)
		{
			tmpArr[i] = MyDIV_UOS(tmpArr[i], reminder, 10, &reminder);
		}
		*--buffCurr = (UTF8Char)('0' + reminder);
		if (tmpArr[maxCnt - 1] == 0)
			maxCnt--;
	}
	return Text::StrConcatC(buff, buffCurr, (UOSInt)(buffEnd - buffCurr));
}
