#include "Stdafx.h"
#include "Math/BigIntUtil.h"
#include "Text/MyString.h"

void Math::BigIntUtil::LSBNeg(UnsafeArray<UIntOS> valBuff, UIntOS valCnt)
{
	UnsafeArray<UIntOS> tmpPtr = valBuff;
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

void Math::BigIntUtil::LSBAdd(UnsafeArray<UIntOS> destBuff, UnsafeArray<const UIntOS> srcBuff, UIntOS destCnt, UIntOS srcCnt)
{
	UIntOS i;
	UIntOS carry = 0;
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
		if ((IntOS)srcBuff[-1] < 0)
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

void Math::BigIntUtil::LSBSub(UnsafeArray<UIntOS> destBuff, UnsafeArray<const UIntOS> srcBuff, UIntOS destCnt, UIntOS srcCnt)
{
	UIntOS i;
	UIntOS carry = 0;
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

void Math::BigIntUtil::LSBMul(UnsafeArray<UIntOS> destBuff, UnsafeArray<const UIntOS> srcBuff, UIntOS destCnt, UIntOS srcCnt)
{
	Bool neg = false;
	UnsafeArray<UIntOS> tmpBuff;
	UnsafeArrayOpt<UIntOS> soriBuff = nullptr;
	UnsafeArray<UIntOS> sBuff;
	UIntOS i;
	UIntOS v;
	UIntOS lastHi;
	tmpBuff = MemAlloc(UIntOS, destCnt + srcCnt);
	if ((IntOS)destBuff[destCnt - 1] < 0)
	{
		LSBNeg(destBuff, destCnt);
		neg = !neg;
	}
	if ((IntOS)srcBuff[srcCnt - 1] < 0)
	{
		sBuff = MemAllocArr(UIntOS, srcCnt);
		soriBuff = sBuff;
		MemCopyNO(sBuff.Ptr(), srcBuff.Ptr(), srcCnt * sizeof(UIntOS));
		srcBuff = sBuff;
		neg = !neg;
	}
	MemClear(&tmpBuff[0], (destCnt + srcCnt) * sizeof(UIntOS));
	sBuff = tmpBuff;
	while (srcCnt > 0)
	{
		v = srcBuff[0];
		lastHi = 0;
		i = 0;
		while (i < destCnt)
		{
			UIntOS tmpVal = lastHi;
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

	MemCopyNO(destBuff.Ptr(), tmpBuff.Ptr(), destCnt * sizeof(UIntOS));
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

void Math::BigIntUtil::LSBAnd(UnsafeArray<UIntOS> destBuff, UnsafeArray<const UIntOS> srcBuff, UIntOS destCnt, UIntOS srcCnt)
{
	UIntOS i;
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
		if ((IntOS)srcBuff[-1] >= 0)
		{
			MemClear(&destBuff[0], destCnt * sizeof(UIntOS));
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

void Math::BigIntUtil::LSBOr(UnsafeArray<UIntOS> destBuff, UnsafeArray<const UIntOS> srcBuff, UIntOS destCnt, UIntOS srcCnt)
{
	UIntOS i;
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
		if ((IntOS)srcBuff[-1] >= 0)
		{
			while (destCnt-- > 0)
			{
				*destBuff = (UIntOS)-1;
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


void Math::BigIntUtil::LSBXor(UnsafeArray<UIntOS> destBuff, UnsafeArray<const UIntOS> srcBuff, UIntOS destCnt, UIntOS srcCnt)
{
	UIntOS i;
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
		if ((IntOS)srcBuff[-1] < 0)
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

UIntOS Math::BigIntUtil::LSBAddUOS(UnsafeArray<UIntOS> valBuff, UIntOS valCnt, UIntOS val) //return overflow value
{
	while (val != 0 && valCnt-- > 0)
	{
		val = MyADD_UOS(valBuff[0], val, &valBuff[0]);
		valBuff++;
	}
	return val;
}

UIntOS Math::BigIntUtil::LSBMulUOS(UnsafeArray<UIntOS> valBuff, UIntOS valCnt, UIntOS val) //return overflow value
{
	UIntOS i = valCnt;
	Bool neg;
	if ((IntOS)valBuff[valCnt - 1] < 0)
	{
		neg = true;
		LSBNeg(valBuff, valCnt);
	}
	else
	{
		neg = false;
	}
	UnsafeArray<UIntOS> tmpPtr = valBuff;
	UIntOS hiVal = 0;
	while (i-- > 0)
	{
		UIntOS tmpVal = hiVal;
		hiVal += MyADD_UOS(tmpVal, MyMUL_UOS(*tmpPtr, val, &hiVal), tmpPtr.Ptr());
		tmpPtr++;
	}
	if (neg)
	{
		LSBNeg(valBuff, valCnt);
	}
	return hiVal;
}

UIntOS Math::BigIntUtil::LSBDivUOS(UnsafeArray<UIntOS> valBuff, UIntOS valCnt, UIntOS val) //return remainder
{
	UIntOS i = valCnt;
	Bool neg;
	if ((IntOS)valBuff[valCnt - 1] < 0)
	{
		neg = true;
		LSBNeg(valBuff, valCnt);
	}
	else
	{
		neg = false;
	}
	UIntOS reminder = 0;
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

void Math::BigIntUtil::LSBAssignI(UnsafeArray<UIntOS> valBuff, UIntOS valCnt, IntOS val)
{
	UIntOS v;
	UnsafeArray<IntOS>::ConvertFrom(valBuff)[0] = val;
	if (val < 0)
	{
		v = (UIntOS)-1;
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

void Math::BigIntUtil::LSBAssignU(UnsafeArray<UIntOS> valBuff, UIntOS valCnt, UIntOS val)
{
	valBuff[0] = val;
	valBuff++;
	valCnt--;
	while (valCnt-- > 0)
	{
		*valBuff++ = 0;
	}
}

void Math::BigIntUtil::LSBAssignStr(UnsafeArray<UIntOS> valBuff, UIntOS valCnt, UnsafeArray<const UTF8Char> val)
{
	UIntOS maxCnt = 0;
	UnsafeArray<UIntOS> valPtr;
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
			MemClear(&valBuff[0], valCnt * sizeof(UIntOS));
			return;
		}
		UIntOS v = (UIntOS)c - '0';
		UIntOS v2;
		UIntOS i = maxCnt;
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
		MemClear(&valBuff[maxCnt], (valCnt - maxCnt) * sizeof(UIntOS));
	}
	if (neg)
	{
		LSBNeg(valBuff, valCnt);
	}
}

UnsafeArray<UTF8Char> Math::BigIntUtil::LSBToString(UnsafeArray<UTF8Char> buff, UnsafeArray<const UIntOS> valArr, UnsafeArray<UIntOS> tmpArr, UIntOS valCnt)
{
	UnsafeArray<UTF8Char> buffEnd = buff + valCnt * 3 * sizeof(UIntOS);
	UnsafeArray<UTF8Char> buffCurr = buffEnd;
	MemCopyNO(tmpArr.Ptr(), valArr.Ptr(), valCnt * sizeof(UIntOS));
	if ((IntOS)tmpArr[valCnt - 1] < 0)
	{
		*buff++ = '-';
		LSBNeg(tmpArr, valCnt);
	}
	UIntOS maxCnt = valCnt;
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
		UIntOS reminder = 0;
		UIntOS i = maxCnt;
		while (i-- > 0)
		{
			tmpArr[i] = MyDIV_UOS(tmpArr[i], reminder, 10, &reminder);
		}
		*--buffCurr = (UTF8Char)('0' + reminder);
		if (tmpArr[maxCnt - 1] == 0)
			maxCnt--;
	}
	return Text::StrConcatC(buff, buffCurr, (UIntOS)(buffEnd - buffCurr));
}
