#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Math/BigFloat.h"
#include "Math/BigIntUtil.h"
#include <stdlib.h>

void Math::BigFloat::RemZero()
{
	UnsafeArray<UOSInt> tarr;
	UOSInt maxCnt = this->valCnt;
	while (maxCnt-- > 0)
	{
		if (this->valArr[maxCnt])
		{
			break;
		}
	}
	maxCnt++;
	if (maxCnt == 0)
	{
		this->valIndex = 0;
	}
	while (maxCnt > 0)
	{
		MemCopyNO(tmpArr.Ptr(), valArr.Ptr(), maxCnt * sizeof(UOSInt));
		if (BigIntUtil::LSBDivUOS2(tmpArr, maxCnt, 10) != 0)
		{
			break;
		}
		tarr = this->tmpArr;
		this->tmpArr = this->valArr;
		this->valArr = tarr;
		this->valIndex++;
		if (this->valArr[maxCnt - 1] == 0)
		{
			maxCnt--;
		}
	}
}

void Math::BigFloat::PrepareTmpBuff(UOSInt tmpCnt)
{
	if (tmpCnt > this->valCnt)
	{
		tmpCnt = this->valCnt;
	}
	MemCopyNO(this->tmpArr.Ptr(), this->valArr.Ptr(), this->valCnt * sizeof(UOSInt));
	this->tmpIndex = this->valIndex;

	UnsafeArray<UOSInt> tarr = this->tmpArr;
	UOSInt i = this->valCnt;
	while (i > tmpCnt)
	{
		if (tarr[i - 1])
		{
			BigIntUtil::LSBDivUOS2(tarr, i, 10);
			this->tmpIndex++;
		}
		else
		{
			i--;
		}
	}
}

void Math::BigFloat::PrepareTmpBuff(NN<const BigFloat> val)
{
	if (this->valCnt >= val->valCnt)
	{
		MemCopyNO(this->tmpArr.Ptr(), val->valArr.Ptr(), val->valCnt * sizeof(UOSInt));
		if (this->valCnt > val->valCnt)
		{
			MemClear(&this->tmpArr[val->valCnt], (this->valCnt - val->valCnt) * sizeof(UOSInt));
		}
		this->tmpIndex = val->valIndex;
	}
	else
	{
		NN<BigFloat>::ConvertFrom(val)->PrepareTmpBuff(this->valCnt);
		MemCopyNO(this->tmpArr.Ptr(), val->valArr.Ptr(), this->valCnt * sizeof(UOSInt));
		this->tmpIndex = val->tmpIndex;
	}
}

void Math::BigFloat::PrepareSum()
{
	UOSInt vCnt = this->valCnt;
	Int32 tindex = this->tmpIndex;
	UnsafeArray<UOSInt> tarr = this->tmpArr;
	Int32 vindex = this->valIndex;
	UnsafeArray<UOSInt> varr = this->valArr;
	while (tindex > vindex)
	{
		if (tarr[vCnt - 1])
		{
			BigIntUtil::LSBDivUOS2(varr, vCnt, 10);
			vindex++;
		}
		else
		{
			BigIntUtil::LSBMulUOS2(tarr, vCnt, 10);
			tindex--;
		}
	}

	while (tindex < vindex)
	{
		if (varr[vCnt - 1])
		{
			BigIntUtil::LSBDivUOS2(tarr, vCnt, 10);
			tindex++;
		}
		else
		{
			BigIntUtil::LSBMulUOS2(varr, vCnt, 10);
			vindex--;
		}
	}
	this->tmpIndex = tindex;
	this->valIndex = vindex;
}

void Math::BigFloat::DoSum()
{
	BigIntUtil::LSBAdd2(this->valArr, this->tmpArr, this->valCnt, this->valCnt);
}

Bool Math::BigFloat::DoSubtract()
{
	UnsafeArray<UOSInt> tarr = this->tmpArr;
	UnsafeArray<UOSInt> varr = this->valArr;
	UOSInt vCnt = this->valCnt;
	UOSInt tActCnt = this->valCnt;
	UOSInt vActCnt = this->valCnt;
	Int32 mode;
	UOSInt i;
	UOSInt j;
	Bool ret = false;
	while (tActCnt > 0)
	{
		if (tarr[tActCnt - 1])
			break;
		tActCnt -= 1;
	}
	while (vActCnt > 0)
	{
		if (varr[vActCnt - 1])
			break;
		vActCnt -= 1;
	}
	if (tActCnt > vActCnt)
	{
		mode = 2;
	}
	else if (tActCnt < vActCnt)
	{
		mode = 0;
	}
	else
	{
		mode = 1;
		while (tActCnt > 0)
		{
			i = tarr[tActCnt - 1];
			j = varr[tActCnt - 1];
			if (i > j)
			{
				mode = 2;
				break;
			}
			else if (i < j)
			{
				mode = 0;
				break;
			}
			tActCnt -= 1;
		}
	}

	if (mode == 1)
	{
		MemClear(&this->valArr[0], vCnt * sizeof(UOSInt));
		this->valIndex = 0;
		this->neg = false;
	}
	else if (mode == 0)
	{
		BigIntUtil::LSBSub2(varr, tarr, vCnt, tActCnt);
	}
	else
	{
		BigIntUtil::LSBSub2(tarr, varr, vCnt, vActCnt);
		this->valArr = tarr;
		this->tmpArr = varr;
		ret = true;
	}
	return ret;
}

Math::BigFloat::BigFloat(NN<const BigFloat> val)
{
	this->valCnt = val->valCnt;
	this->valArr = MemAllocArr(UOSInt, this->valCnt);
	this->tmpArr = MemAllocArr(UOSInt, this->valCnt);
	this->valIndex = val->valIndex;
	this->tmpIndex = val->tmpIndex;
	this->neg = val->neg;
	MemCopyNO(this->valArr.Ptr(), val->valArr.Ptr(), this->valCnt * sizeof(UOSInt));
	MemCopyNO(this->tmpArr.Ptr(), val->tmpArr.Ptr(), this->valCnt * sizeof(UOSInt));
}

Math::BigFloat::BigFloat(UOSInt valSize, Text::CStringNN val)
{
	if (valSize < 16)
		valSize = 16;
	if (valSize & (sizeof(UOSInt) - 1))
	{
		valSize += sizeof(UOSInt) - (valSize & (sizeof(UOSInt) - 1));
	}
	this->valCnt = valSize / sizeof(UOSInt);
	this->valArr = MemAllocArr(UOSInt, valCnt);
	this->tmpArr = MemAllocArr(UOSInt, valCnt);
	this->valIndex = 0;
	this->tmpIndex = 0;
	*this = val;
}

Math::BigFloat::BigFloat(UOSInt valSize, OSInt val)
{
	if (valSize < 16)
		valSize = 16;
	if (valSize & (sizeof(UOSInt) - 1))
	{
		valSize += sizeof(UOSInt) - (valSize & (sizeof(UOSInt) - 1));
	}
	this->valCnt = valSize / sizeof(UOSInt);
	this->valArr = MemAllocArr(UOSInt, this->valCnt);
	this->tmpArr = MemAllocArr(UOSInt, this->valCnt);
	this->valIndex = 0;
	this->tmpIndex = 0;
	*this = val;
}

Math::BigFloat::BigFloat(UOSInt valSize)
{
	if (valSize < 16)
		valSize = 16;
	if (valSize & (sizeof(UOSInt) - 1))
	{
		valSize += sizeof(UOSInt) - (valSize & (sizeof(UOSInt) - 1));
	}
	this->valCnt = valSize / sizeof(UOSInt);
	this->valArr = MemAllocArr(UOSInt, this->valCnt);
	this->tmpArr = MemAllocArr(UOSInt, this->valCnt);
	this->valIndex = 0;
	this->tmpIndex = 0;
	this->neg = false;
	MemClear(this->valArr.Ptr(), valSize);
	MemClear(this->tmpArr.Ptr(), valSize);
}

Math::BigFloat::~BigFloat()
{
	MemFreeArr(this->valArr);
	MemFreeArr(this->tmpArr);
}

OSInt Math::BigFloat::operator =(OSInt val)
{
	UnsafeArray<UOSInt> varr = this->valArr;
	this->valIndex = 0;
	if (val < 0)
	{
		varr[0] = (UOSInt)-val;
		this->neg = true;
	}
	else 
	{
		varr[0] = (UOSInt)val;
		this->neg = false;
	}
	MemClear(&varr[1], (this->valCnt - 1) * sizeof(UOSInt));
	this->RemZero();
	return val;
}

/*Double Math::BigFloat::operator =(Double val)
{
	Char *strBuff;
	Int32 digit;
	Int32 sign;
#ifdef __linux__
	strBuff = ecvt(val, 20, &digit, &sign);
#else
	strBuff = _ecvt(val, 20, &digit, &sign);
#endif
	MemClear(this->valArr, this->valSize);
	UInt8 *varr = valArr;
	Int32 vsize = valSize;
	Int32 strLen = 0;
	_asm
	{
		mov esi,strBuff
		mov ebx,10
bfequlop:
		mov ecx,vsize
		mov edi,varr
		mov edx,0
		shr ecx,2
		cmp byte ptr [esi],0
		jz bfequlop5
bfequlop2:
		mov eax,dword ptr [edi]
		mov dword ptr [edi],edx
		mul ebx
		add dword ptr [edi],eax
		adc edx,0
		add edi,4
		dec ecx
		jnz bfequlop2
		movzx eax,byte ptr [esi]
		inc esi
		mov edi,varr
		sub eax,0x30
		add dword ptr [edi],eax
		jnb bfequlop4
bfequlop3:
		add edi,4
		inc dword ptr [edi]
		jb bfequlop3
bfequlop4:
		inc strLen
		jmp bfequlop
bfequlop5:
	}
	this->isNeg = (sign != 0);
	this->valIndex = -strLen + digit;
	this->RemZero();
	return val;
}*/

NN<Math::BigFloat> Math::BigFloat::operator =(Text::CStringNN val)
{
	Int32 refIndex = 0;
	Int32 refIndex2 = 0;
	UnsafeArray<UOSInt> varr = valArr;
	UOSInt vCnt = valCnt;
	Bool isDot = false;
	Int32 isExt = 0;
	UTF8Char c;
	UOSInt cInd = 0;
	if (val.v[0] == '-')
	{
		this->neg = true;
		val = val.Substring(1);
	}
	else
	{
		this->neg = false;
	}
	MemClear(&varr[0], vCnt * sizeof(UOSInt));
	
	while ((c = val.v[cInd++]) != 0)
	{
		if (c == '.')
		{
			if (isDot)
				return *this;
			isDot = true;
		}
		else if (c == '+')
		{
			if (isExt)
				return *this;
			isExt = 1;
		}
		else if (c == '-')
		{
			if (isExt)
				return *this;
			isExt = 2;
		}
		else if (c >= 0x30 && c <= 0x39)
		{
			if (isExt)
			{
				refIndex2 = refIndex2 * 10 + (c - 0x30);
			}
			else if (varr[valCnt - 1])
			{
				if (!isDot)
				{
					refIndex++;
				}
			}
			else
			{
				BigIntUtil::LSBMulUOS2(varr, vCnt, 10);
				BigIntUtil::LSBAddUOS2(varr, vCnt, (UOSInt)c - 0x30);
				if (isDot)
					refIndex--;
			}
		}
		else
		{
			return *this;
		}
	}

	if (isExt == 2)
	{
		refIndex2 = -refIndex2;
	}
	valIndex = refIndex + refIndex2;
	RemZero();
	return *this;
}

NN<Math::BigFloat> Math::BigFloat::operator =(NN<const BigFloat> val)
{
	if (val->valCnt > this->valCnt)
	{
		this->valCnt = val->valCnt;
		MemFreeArr(this->valArr);
		this->valArr = MemAllocArr(UOSInt, this->valCnt);
		MemCopyNO(this->valArr.Ptr(), val->valArr.Ptr(), this->valCnt * sizeof(UOSInt));
		this->valIndex = val->valIndex;
	}
	else
	{
		this->valIndex = val->valIndex;
		MemCopyNO(this->valArr.Ptr(), val->valArr.Ptr(), val->valCnt * sizeof(UOSInt));
		if (val->valCnt < this->valCnt)
		{
			MemClear(&this->valArr[val->valCnt], (this->valCnt - val->valCnt) * sizeof(UOSInt));
		}
	}
	this->neg = val->neg;
	return *this;
}

NN<Math::BigFloat> Math::BigFloat::operator +=(NN<const BigFloat> val)
{
	PrepareTmpBuff(val);
	PrepareSum();
	if (this->neg == val->neg)
	{
		DoSum();
	}
	else
	{
		if (DoSubtract())
			this->neg = !this->neg;
	}
	return *this;
}

NN<Math::BigFloat> Math::BigFloat::operator -=(NN<const BigFloat> val)
{
	PrepareTmpBuff(val);
	PrepareSum();
	if (this->neg != val->neg)
	{
		DoSum();
	}
	else
	{
		if (DoSubtract())
			this->neg = !this->neg;
	}
	return *this;
}

NN<Math::BigFloat> Math::BigFloat::operator *=(NN<const Math::BigFloat> val)
{
	UOSInt tCnt = this->valCnt;
	UOSInt vCnt = val->valCnt;
	UOSInt tmpCnt = tCnt + vCnt;
	UnsafeArray<UOSInt> tarr = MemAlloc(UOSInt, tmpCnt);
	Int32 tIndex = this->valIndex + val->valIndex;
	UnsafeArray<UOSInt> varr = val->valArr;
	MemCopyNO(tarr.Ptr(), this->valArr.Ptr(), tCnt * sizeof(UOSInt));
	MemClear(&tarr[tCnt], vCnt * sizeof(UOSInt));
	BigIntUtil::LSBMul2(tarr, varr, tmpCnt, vCnt);
	while (tmpCnt > 0)
	{
		if (tarr[tmpCnt - 1])
			break;
		tmpCnt--;
	}
	while (tmpCnt >= tCnt)
	{
		BigIntUtil::LSBDivUOS2(tarr, tmpCnt, 10);
		tIndex++;
		if (tarr[tmpCnt - 1] == 0)
			tmpCnt--;
	}
	MemCopyNO(this->valArr.Ptr(), tarr.Ptr(), this->valCnt * sizeof(UOSInt));

	MemFreeArr(tarr);
	this->valIndex = tIndex;
	if (val->neg)
	{
		this->neg = !this->neg;
	}
	this->RemZero();
	return *this;
}

NN<Math::BigFloat> Math::BigFloat::operator *=(OSInt val)
{
	if (val == 0)
	{
		MemClear(this->valArr.Ptr(), this->valCnt * sizeof(UOSInt));
		this->valIndex = 0;
		this->neg = false;
		return *this;
	}

	if (val < 0)
	{
		val = -val;
		this->neg = !this->neg;
	}
	UnsafeArray<UOSInt> tharr = this->valArr;
	UOSInt thCnt = this->valCnt;
	Int32 tIndex = this->valIndex;
	while (tharr[thCnt - 1] != 0)
	{
		BigIntUtil::LSBDivUOS2(tharr, thCnt, 10);
		tIndex++;
	}
	BigIntUtil::LSBMulUOS2(tharr, thCnt, (UOSInt)val);
	this->valIndex = tIndex;
	this->RemZero();
	return *this;
}

NN<Math::BigFloat> Math::BigFloat::operator /=(NN<const Math::BigFloat> val)
{
/*	Int32 tsize;
	UInt8 *tarr = MemAlloc(UInt8, tsize = this->valSize + val->valSize);
	UInt8 *tarr2 = MemAlloc(UInt8, tsize);
	Int32 tIndex = this->valIndex - val->valIndex;
	UInt8 *tharr = this->valArr;
	MemCopyNO(val->tmpArr, val->valArr, val->valSize);
	UInt8 *vaarr = val->tmpArr;
	Int32 thsize = this->valSize;
	Int32 thsize2 = this->valSize;
	Int32 vasize = val->valSize;
	Int32 thisASize;
	Int32 valASize;
	Int32 tmp;
	Int32 tmp1;

	_asm
	{
		mov esi,vaarr
		mov eax,-4
		mov ecx,0
		mov edx,vasize
bfdivlop3:
		cmp dword ptr [esi+ecx],0
		jz bfdivlop4
		mov eax,ecx
bfdivlop4:
		add ecx,4
		cmp ecx,edx
		jb bfdivlop3
		add eax,4
		jz bfdivexit
		cmp eax,4
		jz bfdivexit2
		mov valASize,eax

		mov esi,tharr
		mov eax,-4
		mov ecx,0
		mov edx,thsize
bfdivlop:
		cmp dword ptr [esi+ecx],0
		jz bfdivlop2
		mov eax,ecx
bfdivlop2:
		add ecx,4
		cmp ecx,edx
		jb bfdivlop
		add eax,4
		jz bfdivzero
		mov thisASize,eax

		mov ecx,thisASize
		mov esi,tharr
		mov edi,tarr
		shr ecx,2
		rep movsd

		mov ebx,10
		mov edi,valASize
		add edi,thsize
		sub edi,4
		mov edx,0

bfdivlop6:
		mov esi,tarr
		mov ecx,thisASize
bfdivlop5:
		mov eax,dword ptr [esi]
		mov dword ptr [esi],edx
		add esi,4
		mul ebx
		add dword ptr [esi - 4],eax
		adc edx,0
		sub ecx,4
		jnz bfdivlop5
		dec tIndex
		cmp edx,0
		jz bfdivlop6
		mov dword ptr [esi],edx
		mov edx,0
		add thisASize,4
		cmp thisASize,edi
		jb bfdivlop6

		mov edi,tharr
		mov ecx,thsize
		shr ecx,2
		mov eax,0
		rep stosd
bfdivlop7:
		mov esi,tarr
		mov edi,vaarr
		mov ecx,thisASize
		mov ebx,valASize
		mov edx,dword ptr [edi+ebx-4]
		mov eax,dword ptr [edi+ebx-8]
		mov tmp, 0
bfdivlop8:
		shr edx,1
		rcr eax,1
		inc tmp
		cmp edx,0
		jnz bfdivlop8
		cmp edx,0xffffffff
		jnz bfdivlop9
		shr eax,1
bfdivlop9:
		inc eax
		mov tmp1,eax

		mov ebx,dword ptr [esi+ecx-4]
		mov edx,dword ptr [esi+ecx-8]
		mov eax,dword ptr [esi+ecx-12]
bfdivlop10:
		shr ebx,1
		rcr edx,1
		rcr eax,1
		dec tmp
		cmp ebx,0
		jnz bfdivlop10
		shr edx,1
		rcr eax,1
		dec tmp

		div tmp1
		mov tmp1,eax
		cmp tmp,0
		jl bfdivlop11

		mov ecx,tmp
		cmp ecx,0
		jz bfdivlop10a
		shr eax,cl
		jz bfdivlop12
		mov tmp1,eax
bfdivlop10a:

		mov ebx,valASize
		add esi,thisASize
		sub esi,ebx
		sub esi,4
		mov ecx,0

bfdivlop10b:
		mov eax,dword ptr [edi+ecx]
		mul tmp1
		sub dword ptr [esi+ecx],eax
		sbb dword ptr [esi+ecx+4],edx
		jnb bfdivlop10d

		push ebx
		lea ebx,[ecx+4]
bfdivlop10c:
		add ebx,4
		sub dword ptr [esi+ebx],1
		jb bfdivlop10c
		pop ebx

bfdivlop10d:
		add ecx,4
		cmp ecx,ebx
		jb bfdivlop10b

		mov ecx,thisASize
		sub ecx,valASize
		mov eax,tmp1
		mov esi,tharr
		add dword ptr [esi+ecx-4],eax
		jmp bfdivlop13

bfdivlop12:
		mov ecx,32
		sub ecx,tmp
		shl eax,cl
		mov tmp1,eax

		mov ebx,valASize
		add esi,thisASize
		sub esi,ebx
		sub esi,8
		mov ecx,0

bfdivlop12b:
		mov eax,dword ptr [edi+ecx]
		mul tmp1
		sub dword ptr [esi+ecx],eax
		sbb dword ptr [esi+ecx+4],edx
		jnb bfdivlop12d

		push ebx
		lea ebx,[ecx+4]
bfdivlop12c:
		add ebx,4
		sub dword ptr [esi+ebx],1
		jb bfdivlop12c
		pop ebx

bfdivlop12d:
		add ecx,4
		cmp ecx,ebx
		jb bfdivlop12b

		mov ecx,thisASize
		sub ecx,valASize
		mov eax,tmp1
		mov esi,tharr
		add dword ptr [esi+ecx-8],eax
		jmp bfdivlop13

bfdivlop11:
		mov ecx,32
		add ecx,tmp
		jz bfdivlop11a
		shr eax,cl
		jz bfdivlop16
		mov tmp1,eax
bfdivlop11a:

		mov ebx,valASize
		add esi,thisASize
		sub esi,ebx
		mov ecx,0

bfdivlop11b:
		mov eax,dword ptr [edi+ecx]
		mul tmp1
		sub dword ptr [esi+ecx],eax
		sbb dword ptr [esi+ecx+4],edx
		jnb bfdivlop11d

		push ebx
		lea ebx,[ecx+4]
bfdivlop11c:
		add ebx,4
		sub dword ptr [esi+ebx],1
		jb bfdivlop11c
		pop ebx

bfdivlop11d:
		add ecx,4
		cmp ecx,ebx
		jb bfdivlop11b

		mov ecx,thisASize
		sub ecx,valASize
		mov eax,tmp1
		mov esi,tharr
		add dword ptr [esi+ecx],eax
		jmp bfdivlop13
bfdivlop16:
		mov eax,tmp1
		mov ecx,tmp
		neg ecx
		shl eax,cl
		mov tmp1,eax

		mov ebx,valASize
		add esi,thisASize
		sub esi,ebx
		sub esi,4
		mov ecx,0

bfdivlop16b:
		mov eax,dword ptr [edi+ecx]
		mul tmp1
		sub dword ptr [esi+ecx],eax
		sbb dword ptr [esi+ecx+4],edx
		jnb bfdivlop16d

		push ebx
		lea ebx,[ecx+4]
bfdivlop16c:
		add ebx,4
		sub dword ptr [esi+ebx],1
		jb bfdivlop16c
		pop ebx

bfdivlop16d:
		add ecx,4
		cmp ecx,ebx
		jb bfdivlop16b

		mov ecx,thisASize
		sub ecx,valASize
		mov eax,tmp1
		mov esi,tharr
		add dword ptr [esi+ecx-4],eax
		jmp bfdivlop13

bfdivlop13:
		mov ecx,thisASize
		mov esi,tarr
bfdivlop14:
		cmp dword ptr [esi+ecx-4],0
		jnz bfdivlop15
		sub thisASize,4
		sub ecx,4
		cmp ecx,valASize
		jbe bfdivlop17
		jmp bfdivlop14
bfdivlop15:
		jmp bfdivlop7

bfdivlop17:
		mov esi,tarr
		mov edi,vaarr
		mov ecx,valASize
bfdivlop17a:
		mov eax,dword ptr [edi+ecx-4]
		cmp eax,dword ptr [esi+ecx-4]
		ja bfdivlop18
		jb bfdivlop17b
		sub ecx,4
		jz bfdivlop17b
		jmp bfdivlop17a
bfdivlop17b:
		mov ecx,valASize
		mov ebx,0
bfdivlop17d:
		mov eax,dword ptr [edi+ebx]
		sub dword ptr [esi+ebx],eax
		jnb bfdivlop17c

//bfdivlop17e:
		mov edx,ebx
bfdivlop17f:
		add edx,4
		sub dword ptr [esi+edx],1
		jb bfdivlop17f
bfdivlop17c:
		add ebx,4
		cmp ebx,ecx
		jb bfdivlop17d
		mov ebx,tharr
bfdivlop17g:
		add ebx,4
		inc dword ptr [ebx-4]
		jb bfdivlop17g
		mov ecx,valASize
		jmp bfdivlop17a
bfdivlop18:
		mov ecx,valASize
		mov ebx,4
		shr dword ptr [edi],1

		setb dl
bfdivlop18a:
		shr dword ptr [edi+ebx],1
		jnb bfdivlop18b
		or byte ptr [edi+ebx-1],0x80
bfdivlop18b:
		add ebx,4
		cmp ebx,ecx
		jb bfdivlop18a

		mov ebx,0
bfdivlop18c:
		add ebx,4
		add dword ptr [edi+ebx-4],edx
		jb bfdivlop18c

		mov ecx,valASize
bfdivlop18d:
		mov eax,dword ptr [edi+ecx-4]
		cmp eax,dword ptr [esi+ecx-4]
		ja bfdivend
		jb bfdivlop18e
		sub ecx,4
		jz bfdivlop18e
		jmp bfdivlop18d
bfdivlop18e:
		mov ecx,0
bfdivlop18f:
		add ecx,4
		inc dword ptr [esi+ecx-4]
		jb bfdivlop18f
		jmp bfdivend
bfdivzero:
////////////////////////////////////////
		jmp bfdivend
bfdivend:
	}
	//MemCopy(this->valArr, tarr, this->valSize);

	MemFree(tarr);
	MemFree(tarr2);
	this->valIndex = tIndex;
	if (val->isNeg)
	{
		this->isNeg = !this->isNeg;
	}
	RemZero();
	_asm
	{
		jmp bfdivexit
bfdivexit2:
	}
	*this /= *(UInt32*)val->valArr;
	this->valIndex -= val->valIndex;
	if (val->isNeg)
	{
		this->isNeg = !this->isNeg;
	}
	return this;
	_asm
	{
bfdivexit:
	}*/
	return *this;
}

NN<Math::BigFloat> Math::BigFloat::operator /=(UOSInt val)
{
	return *this;
}

NN<Math::BigFloat> Math::BigFloat::ToNeg()
{
	UOSInt i = this->valCnt;
	this->neg = !this->neg;
	while (i-- > 0)
	{
		if (this->valArr[i] == 0)
			return *this;
	}
	this->neg = false;
	return *this;
}

Bool Math::BigFloat::IsZero()
{
	UOSInt i = this->valCnt;
	while (i > 0)
	{
		i--;
		if (this->valArr[i])
			return false;
	}
	return true;
}

Bool Math::BigFloat::operator >(NN<const BigFloat> val)
{
	if (this->neg != val->neg)
	{
		if (this->neg)
			return false;
		else
			return true;
	}
	Math::BigFloat tmp(this->valCnt * sizeof(UOSInt));
	tmp = *this;
	tmp -= val;
	if (tmp.neg)
		return false;
	else if (tmp.IsZero())
		return false;
	else
		return true;
}

Bool Math::BigFloat::operator >=(NN<const BigFloat> val)
{
	if (this->neg != val->neg)
	{
		if (this->neg)
			return false;
		else
			return true;
	}
	Math::BigFloat tmp(this->valCnt * sizeof(UOSInt));
	tmp = *this;
	tmp -= val;
	if (tmp.neg)
		return false;
	else if (tmp.IsZero())
		return true;
	else
		return true;
}

Bool Math::BigFloat::operator <(NN<const BigFloat> val)
{
	if (this->neg != val->neg)
	{
		if (this->neg)
			return true;
		else
			return false;
	}
	Math::BigFloat tmp(this->valCnt * sizeof(UOSInt));
	tmp = *this;
	tmp -= val;
	if (tmp.neg)
		return true;
	else if (tmp.IsZero())
		return false;
	else
		return false;
}

Bool Math::BigFloat::operator <=(NN<const BigFloat> val)
{
	if (this->neg != val->neg)
	{
		if (this->neg)
			return true;
		else
			return false;
	}
	Math::BigFloat tmp(this->valCnt * sizeof(UOSInt));
	tmp = *this;
	tmp -= val;
	if (tmp.neg)
		return true;
	else if (tmp.IsZero())
		return true;
	else
		return false;
}

Bool Math::BigFloat::operator ==(NN<const BigFloat> val)
{
	if (this->neg != val->neg)
	{
		return false;
	}
	Math::BigFloat tmp(this->valCnt * sizeof(UOSInt));
	tmp = *this;
	tmp -= val;
	if (tmp.IsZero())
		return true;
	else
		return false;
}

NN<Math::BigFloat> Math::BigFloat::Factorial(UOSInt val)
{
	if (val < 1)
	{
		*this = (OSInt)1;
		return *this;
	}
	*this = (OSInt)1;
	Math::BigFloat tmp(this->valCnt * sizeof(UOSInt));
	while (val > 1)
	{
		tmp = (OSInt)val;
		*this *= tmp;
		val--;
	}
	return *this;
}

NN<Math::BigFloat> Math::BigFloat::SetPI()
{
	Math::BigFloat tmpVal(this->valCnt * sizeof(UOSInt));
	OSInt endInd = -(OSInt)(this->valCnt * sizeof(UOSInt) * 5) >> 1;
	OSInt v = 1;
	UOSInt v2 = 3;
	*this = (OSInt)1;
	tmpVal = (OSInt)1;
	while (v < 0x10000)
	{
		tmpVal *= v;
		tmpVal /= v2;
		*this += tmpVal;

		if (tmpVal.valIndex - this->valIndex < endInd)
			break;
		v++;
		v2 += 2;
	}
	tmpVal = (OSInt)2;
	*this *= tmpVal;
	return *this;
}

NN<Math::BigFloat> Math::BigFloat::SetE(NN<Math::BigFloat> val)
{
	Math::BigFloat tmpVal(this->valCnt * sizeof(UOSInt));
	Math::BigFloat tmpVal2(val->valCnt * sizeof(UOSInt));
	OSInt endInd = -(OSInt)(this->valCnt * sizeof(UOSInt) * 5) >> 1;
	UOSInt i = 2;
	tmpVal2 = val;
	tmpVal = tmpVal2;
	*this = (OSInt)1;
	*this += tmpVal;

	while (true)
	{
		tmpVal *= tmpVal2;
		tmpVal /= i;
		if (i > 10 && tmpVal.valIndex - this->valIndex < endInd)
			break;
		*this += tmpVal;
		i++;
	}
	return *this;
}

NN<Math::BigFloat> Math::BigFloat::SetLn(NN<BigFloat> val)
{
	if (val->neg)
	{
		return *this;

/*		tmp.isNeg = true;
		if (tmp >= val)
		{
			tmp.isNeg = false;
			tmp2 = val;
			tmp2 -= &tmp;
			tmp2 /= val;
			tmp = &tmp2;
			*this = &tmp;
			while (true)
			{
				tmp *= &tmp2;
				tmp3 = &tmp;
				tmp3 /= ++i;
				*this += &tmp3;
				if (i > 10 && tmp3.valIndex - this->valIndex < this->valIndex - this->valSize)
					break;
			}
		}
		else
		{
		}*/
	}
	else
	{
		Math::BigFloat tmp(this->valCnt * sizeof(UOSInt));
		Math::BigFloat tmp2(this->valCnt * sizeof(UOSInt));
		Math::BigFloat tmp3(this->valCnt * sizeof(UOSInt));
		Math::BigFloat e0(this->valCnt * sizeof(UOSInt));
		Math::BigFloat e1(this->valCnt * sizeof(UOSInt));
		Math::BigFloat e2(this->valCnt * sizeof(UOSInt));
		UOSInt i = 1;
		Int32 j;
		tmp = (OSInt)1;
		e0.SetE(tmp);
		e2 = tmp;
		e2 /= e0;

		OSInt endInd = -(OSInt)(this->valCnt * sizeof(UOSInt) * 5) >> 1;
		if (val.Ptr()[0] > e0)
		{
			tmp3 = val;
			tmp2 = (OSInt)0;
			while (tmp3 > e0)
			{
				tmp = (OSInt)1;
				e1.SetE(tmp);
				e2 = tmp;
				e2 /= e1;

				e1 *= e1;
				j = 1;
				while (tmp3 > e1)
				{
					e1 *= e1;
					e2 *= e2;
					tmp *= 2;
					j++;
				}
				tmp3 *= e2;
				tmp2 += tmp;
			}
			*this = tmp2;

			tmp = (OSInt)1;
			tmp2 = tmp3;
			tmp2 -= tmp;
			tmp2 /= tmp3;
			tmp = tmp2;
			*this += tmp;
			while (true)
			{
				tmp *= tmp2;
				tmp3 = tmp;
				tmp3 /= ++i;
				*this += tmp3;
				if (i > 10 && tmp3.valIndex - this->valIndex < endInd)
					break;
			}
		}
		else if (val.Ptr()[0] > tmp)
		{
			tmp.neg = false;
			tmp2 = val;
			tmp2 -= tmp;
			tmp2 /= val;
			tmp = tmp2;
			*this = tmp;
			while (true)
			{
				tmp *= tmp2;
				tmp3 = tmp;
				tmp3 /= ++i;
				*this += tmp3;
				if (i > 10 && tmp3.valIndex - this->valIndex < endInd)
					break;
			}
		}
		else if (val.Ptr()[0] >= e2)
		{
			tmp2 = val;
			tmp = (OSInt)1;
			tmp2 -= tmp;
			*this = tmp2;
			tmp = tmp2;
			while (true)
			{
				i++;
				tmp *= tmp2;
				tmp3 = tmp;
				tmp3 /= i;
				if (i & 1)
				{
					*this += tmp3;
				}
				else
				{
					*this -= tmp3;
				}
				if (i > 10 && tmp3.valIndex - this->valIndex < endInd)
					break;
			}
		}
		else
		{
			e0 = e2;

			tmp3 = val;
			tmp2 = (OSInt)0;
			while (tmp3 < e0)
			{
				tmp = (OSInt)1;
				e1.SetE(tmp);
				e2 = tmp;
				e2 /= e1;

				e2 *= e2;
				j = 1;
				while (tmp3 < e2)
				{
					e1 *= e1;
					e2 *= e2;
					tmp *= 2;
					j++;
				}
				tmp3 *= e1;
				tmp2 -= tmp;
			}
			*this = tmp2;

			tmp2 = tmp3;
			tmp = (OSInt)1;
			tmp2 -= tmp;
			*this += tmp2;
			tmp = tmp2;
			while (true)
			{
				i++;
				tmp *= tmp2;
				tmp3 = tmp;
				tmp3 /= i;
				if (i & 1)
				{
					*this += tmp3;
				}
				else
				{
					*this -= tmp3;
				}
				if (i > 10 && tmp3.valIndex - this->valIndex < endInd)
					break;
			}
		}
	}
	return *this;
}

NN<Math::BigFloat> Math::BigFloat::SetSin(NN<const Math::BigFloat> val)
{
	Math::BigFloat tmpVal(this->valCnt * sizeof(UOSInt));
	UOSInt i = 1;
	OSInt endInd = -(OSInt)(this->valCnt * sizeof(UOSInt) * 5) >> 1;
	tmpVal = val;
	*this = tmpVal;

	while (true)
	{
		tmpVal *= val;
		tmpVal *= val;
		tmpVal /= ++i;
		tmpVal /= ++i;
		tmpVal.ToNeg();
		if (i > 10 && tmpVal.valIndex - this->valIndex < endInd)
			break;
		*this += tmpVal;
	}
	return *this;
}

NN<Math::BigFloat> Math::BigFloat::SetCos(NN<const Math::BigFloat> val)
{
	Math::BigFloat tmpVal(this->valCnt * sizeof(UOSInt));
	OSInt endInd = -(OSInt)(this->valCnt * sizeof(UOSInt) * 5) >> 1;
	UOSInt i = 0;
	tmpVal = (OSInt)1;
	*this = tmpVal;

	while (true)
	{
		tmpVal *= val;
		tmpVal *= val;
		tmpVal /= ++i;
		tmpVal /= ++i;
		tmpVal.ToNeg();
		if (i > 10 && tmpVal.valIndex - this->valIndex < endInd)
			break;
		*this += tmpVal;
	}
	return *this;
}

NN<Math::BigFloat> Math::BigFloat::SetTan(NN<const Math::BigFloat> val)
{
	Math::BigFloat tmpVal(this->valCnt * sizeof(UOSInt));
	tmpVal.SetCos(val);
	this->SetSin(val);
	return *this /= tmpVal;
}

UOSInt Math::BigFloat::GetSize()
{
	return this->valCnt * sizeof(UOSInt);
}

UnsafeArray<UTF8Char> Math::BigFloat::ToString(UnsafeArray<UTF8Char> buff)
{
	UnsafeArray<UTF8Char> strBuff;
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UTF8Char> sptr2;
	UOSInt vCnt = valCnt;
	Int32 vindex = valIndex;
	OSInt ssize;
	MemCopyNO(tmpArr.Ptr(), valArr.Ptr(), vCnt);

	ssize = (OSInt)(vCnt * sizeof(UOSInt) * 3);
	strBuff = MemAlloc(UTF8Char, (UOSInt)ssize);
	sptr = BigIntUtil::LSBToString2(strBuff, this->valArr, this->tmpArr, this->valCnt);
	
	if (strBuff[0] == '0' && strBuff[1] == 0)
	{
		*buff++ = '0';
		*buff = 0;
		MemFreeArr(strBuff);
		return buff;
	}
	if (this->neg)
	{
		*buff++ = '-';
	}
	ssize = (sptr - strBuff);
	sptr2 = sptr;
	sptr = strBuff;
	if (vindex > 3 || (vindex < -ssize - 2))
	{
		*buff++ = *sptr++;
		if (sptr != sptr2)
		{
			*buff++ = '.';
			vindex += (Int32)(sptr2 - sptr);
			buff = Text::StrConcat(buff, sptr);
		}
		if (vindex > 0)
		{
			*buff++ = '+';
			buff = Text::StrInt32(buff, vindex);
		}
		else if (vindex < 0)
		{
			buff = Text::StrInt32(buff, vindex);
		}
	}
	else if (vindex >= 0)
	{
		buff = Text::StrConcat(buff, strBuff);
		while (vindex-- > 0)
		{
			*buff++ = '0';
		}
		*buff = 0;
	}
	else if (vindex > -ssize)
	{
		ssize = ssize + vindex;
		while (ssize-- > 0)
		{
			*buff++ = *sptr++;
		}
		*buff++ = '.';
		buff = Text::StrConcat(buff, sptr);
	}
	else
	{
		*buff++ = '0';
		*buff++ = '.';
		ssize = -vindex - ssize;
		while (ssize-- > 0)
		{
			*buff++ = '0';
		}
		buff = Text::StrConcat(buff, sptr);
	}
	MemFreeArr(strBuff);
	return buff;
}

void Math::BigFloat::ToString(NN<Text::StringBuilderUTF8> sb)
{
	UnsafeArray<UTF8Char> strBuff;
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UTF8Char> sptr2;
	UOSInt vCnt = valCnt;
	Int32 vindex = valIndex;
	OSInt ssize;
	MemCopyNO(tmpArr.Ptr(), valArr.Ptr(), vCnt);

	ssize = (OSInt)(vCnt * sizeof(UOSInt) * 3);
	strBuff = MemAlloc(UTF8Char, (UOSInt)ssize);
	sptr = BigIntUtil::LSBToString2(strBuff, this->valArr, this->tmpArr, this->valCnt);
	
	if (strBuff[0] == '0' && strBuff[1] == 0)
	{
		sb->AppendUTF8Char('0');
		MemFreeArr(strBuff);
		return;
	}
	if (this->neg)
	{
		sb->AppendUTF8Char('-');
	}
	ssize = (sptr - strBuff);
	sptr2 = sptr;
	sptr = strBuff;
	if (vindex > 3 || (vindex < -ssize - 2))
	{
		sb->AppendUTF8Char(*sptr++);
		if (sptr != sptr2)
		{
			sb->AppendUTF8Char('.');
			vindex += (Int32)(sptr2 - sptr);
			sb->AppendP(sptr, sptr2);
		}
		if (vindex > 0)
		{
			sb->AppendUTF8Char('+');
			sb->AppendI32(vindex);
		}
		else if (vindex < 0)
		{
			sb->AppendI32(vindex);
		}
	}
	else if (vindex >= 0)
	{
		sb->AppendP(strBuff, sptr2);
		sb->AppendChar('0', (UInt32)vindex);
	}
	else if (vindex > -ssize)
	{
		ssize = ssize + vindex;
		sb->AppendC(sptr, (UOSInt)ssize);
		sptr += ssize;
		sb->AppendUTF8Char('.');
		sb->AppendP(sptr, sptr2);
	}
	else
	{
		sb->Append(CSTR("0."));
		ssize = -vindex - ssize;
		sb->AppendChar('0', (UOSInt)ssize);
		sb->AppendP(sptr, sptr2);
	}
	MemFreeArr(strBuff);
}
