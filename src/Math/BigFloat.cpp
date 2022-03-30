#include "stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Math/BigFloat.h"
#include <stdlib.h>
#ifndef HAS_ASM32
#include <windows.h>
#endif

void Math::BigFloat::RemZero()
{
	UInt8 *tarr;
	UInt8 *varr;
	Int32 vsize = valSize;
	Int32 vindex = 1;
	while (vsize-- > 0)
	{
		if (valArr[vsize])
		{
			vindex = 0;
			break;
		}
	}
	if (vindex)
	{
		valIndex = 0;
	}
	vsize = valSize;
	while (vindex == 0)
	{
		MemCopyNO(tarr = tmpArr, varr = valArr, vsize);
		vindex = 1;
#ifdef HAS_ASM32
		_asm
		{
			mov edi,tarr
			mov ecx,vsize
			add edi,ecx
			shr ecx,2
			mov edx,0
			mov ebx,10
bfrzlop:
			sub edi,4
			mov eax,dword ptr[edi]
			div ebx
			mov dword ptr [edi],eax
			dec ecx
			jnz bfrzlop
			mov vindex,edx
		}
#else
		UInt8 *tmpPtr;
		LARGE_INTEGER li;
		tmpPtr = tarr + vsize;
		vsize >>= 2;
		li.HighPart = 0;
		while (vsize-- > 0)
		{
			tmpPtr -= 4;
			li.LowPart = *(UInt32*)tmpPtr;
			*(UInt32*)tmpPtr = (UInt32)(li.QuadPart / 10);
			li.HighPart = (UInt32)(li.QuadPart % 10);
		}
		vindex = li.HighPart;
#endif

		if (vindex == 0)
		{
			this->valArr = tarr;
			this->tmpArr = varr;
			this->valIndex++;
		}
	}
}

void Math::BigFloat::PrepareTmpBuff(Int32 tmpSize)
{
	if (tmpSize > this->valSize)
	{
		tmpSize = this->valSize;
	}
	MemCopyNO(this->tmpArr, this->valArr, this->valSize);
	this->tmpIndex = this->valIndex;

	UInt8 *tarr = this->tmpArr;
	Int32 i = this->valSize;
	while (i > tmpSize)
	{
		if (*(Int32*)&tarr[i - 4])
		{
			_asm
			{
				mov edi,tarr
				mov ecx,i
				add edi,ecx
				shr ecx,2
				mov edx,0
				mov ebx,10
bfptblop:
				sub edi,4
				mov eax,dword ptr [edi]
				div ebx
				mov dword ptr [edi],eax
				dec ecx
				jnz bfptblop
			}
			this->tmpIndex++;
		}
		else
		{
			i -= 4;
		}
	}
}

void Math::BigFloat::PrepareTmpBuff(const BigFloat *val)
{
	if (this->valSize >= val->valSize)
	{
		MemCopyNO(this->tmpArr, val->valArr, val->valSize);
		if (this->valSize > val->valSize)
		{
			MemClear(&this->tmpArr[val->valSize], this->valSize - val->valSize);
		}
		this->tmpIndex = val->valIndex;
	}
	else
	{
		((BigFloat*)val)->PrepareTmpBuff(this->valSize);
		MemCopyNO(this->tmpArr, val->valArr, this->valSize);
		this->tmpIndex = val->tmpIndex;
	}
}

void Math::BigFloat::PrepareSum()
{
	Int32 vsize = this->valSize;
	Int32 tindex = this->tmpIndex;
	UInt8 *tarr = this->tmpArr;
	Int32 vindex = this->valIndex;
	UInt8 *varr = this->valArr;
	while (tindex > vindex)
	{
		if (*(Int32*)&tarr[vsize - 4])
		{
			_asm
			{
				mov edi,varr
				mov ecx,vsize
				add edi,ecx
				shr ecx,2
				mov ebx,10
				mov edx,0
bfpslop:
				sub edi,4
				mov eax,dword ptr [edi]
				div ebx
				mov dword ptr [edi],eax
				dec ecx
				jnz bfpslop
			}
			vindex++;
		}
		else
		{
			_asm
			{
				mov edi,tarr
				mov ecx,vsize
				shr ecx,2
				mov ebx,10
				mov edx,0
bfpslop2:
				push edx
				mov eax,dword ptr [edi]
				mul ebx
				pop esi
				add eax,esi
				adc edx,0
				mov dword ptr [edi],eax
				add edi,4
				dec ecx
				jnz bfpslop2
			}
			tindex--;
		}
	}

	while (tindex < vindex)
	{
		if (*(Int32*)&varr[vsize - 4])
		{
			_asm
			{
				mov edi,tarr
				mov ecx,vsize
				add edi,ecx
				shr ecx,2
				mov ebx,10
				mov edx,0
bfpslop3:
				sub edi,4
				mov eax,dword ptr [edi]
				div ebx
				mov dword ptr [edi],eax
				dec ecx
				jnz bfpslop3
			}
			tindex++;
		}
		else
		{
			_asm
			{
				mov edi,varr
				mov ecx,vsize
				shr ecx,2
				mov ebx,10
				mov edx,0
bfpslop4:
				push edx
				mov eax,dword ptr [edi]
				mul ebx
				pop esi
				add eax,esi
				adc edx,0
				mov dword ptr [edi],eax
				add edi,4
				dec ecx
				jnz bfpslop4
			}
			vindex--;
		}
	}
	this->tmpIndex = tindex;
	this->valIndex = vindex;
}

void Math::BigFloat::DoSum()
{
	UInt8 *tarr = this->tmpArr;
	UInt8 *varr = this->valArr;
	Int32 vsize = this->valSize;
	_asm
	{
		mov esi,tarr
		mov edi,varr
		mov ecx,vsize
		shr ecx,2
bfdsumlop:
		mov eax,dword ptr [esi]
		add dword ptr [edi],eax
		jnb bfdsumlop2
		push edi
bfdsumlop1:
		add edi,4
		inc dword ptr [edi]
		jb bfdsumlop1
		pop edi
bfdsumlop2:
		add esi,4
		add edi,4
		dec ecx
		jnz bfdsumlop
	}
}

Bool Math::BigFloat::DoSubtract()
{
	UInt8 *tarr = this->tmpArr;
	UInt8 *varr = this->valArr;
	Int32 vsize = this->valSize;
	Int32 tActSize = this->valSize;
	Int32 vActSize = this->valSize;
	Int32 mode;
	UInt32 i;
	UInt32 j;
	Bool ret = false;
	while (tActSize > 0)
	{
		if (*(Int32*)&tarr[tActSize - 4])
			break;
		tActSize -= 4;
	}
	while (vActSize > 0)
	{
		if (*(Int32*)&varr[vActSize - 4])
			break;
		vActSize -= 4;
	}
	if (tActSize > vActSize)
	{
		mode = 2;
	}
	else if (tActSize < vActSize)
	{
		mode = 0;
	}
	else
	{
		mode = 1;
		while (tActSize > 0)
		{
			i = *(UInt32*)&tarr[tActSize - 4];
			j = *(UInt32*)&varr[tActSize - 4];
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
			tActSize -= 4;
		}
	}

	if (mode == 1)
	{
		MemClear(this->valArr, vsize);
		this->valIndex = 0;
		this->isNeg = false;
	}
	else if (mode == 0)
	{
		_asm
		{
			mov esi,tarr
			mov edi,varr
			mov ecx,vsize
			shr ecx,2
bfdsublop:
			mov eax,dword ptr [esi]
			sub dword ptr [edi],eax
			jnb bfdsublop2
			push edi
bfdsublop3:
			add edi,4
			sub dword ptr [edi],1
			jb bfdsublop3
			pop edi
bfdsublop2:
			add edi,4
			add esi,4
			dec ecx
			jnz bfdsublop
		}
	}
	else
	{
		_asm
		{
			mov esi,varr
			mov edi,tarr
			mov ecx,vsize
			shr ecx,2
bfdsublop4:
			mov eax,dword ptr [esi]
			sub dword ptr [edi],eax
			jnb bfdsublop5
			push edi
bfdsublop6:
			add edi,4
			sub dword ptr [edi],1
			jb bfdsublop6
			pop edi
bfdsublop5:
			add edi,4
			add esi,4
			dec ecx
			jnz bfdsublop4
		}
		this->valArr = tarr;
		this->tmpArr = varr;
		ret = true;
	}
	return ret;
}

Math::BigFloat::BigFloat(const BigFloat *val)
{
	this->valSize = val->valSize;
	this->valArr = MemAlloc(UInt8, this->valSize);
	this->tmpArr = MemAlloc(UInt8, this->valSize);
	this->valIndex = val->valIndex;
	this->tmpIndex = val->tmpIndex;
	MemCopyNO(this->valArr, val->valArr, this->valSize);
	MemCopyNO(this->tmpArr, val->tmpArr, this->valSize);
}

Math::BigFloat::BigFloat(Int32 valSize, const WChar *val)
{
	if (valSize < 16)
		valSize = 16;
	if (valSize & 3)
	{
		valSize += 4 - (valSize & 3);
	}
	this->valSize = valSize;
	this->valArr = MemAlloc(UInt8, valSize);
	this->tmpArr = MemAlloc(UInt8, valSize);
	this->valIndex = 0;
	this->tmpIndex = 0;
	if (val)
	{
		*this = val;
	}
	else
	{
		MemClear(this->valArr, valSize);
		MemClear(this->tmpArr, valSize);
	}
}

Math::BigFloat::BigFloat(Int32 valSize, Double val)
{
	if (valSize < 16)
		valSize = 16;
	if (valSize & 3)
	{
		valSize += 4 - (valSize & 3);
	}
	this->valSize = valSize;
	this->valArr = MemAlloc(UInt8, valSize);
	this->tmpArr = MemAlloc(UInt8, valSize);
	this->valIndex = 0;
	this->tmpIndex = 0;
	*this = val;
}

Math::BigFloat::BigFloat(Int32 valSize)
{
	if (valSize < 16)
		valSize = 16;
	if (valSize & 3)
	{
		valSize += 4 - (valSize & 3);
	}
	this->valSize = valSize;
	this->valArr = MemAlloc(UInt8, valSize);
	this->tmpArr = MemAlloc(UInt8, valSize);
	this->valIndex = 0;
	this->tmpIndex = 0;
	MemClear(this->valArr, valSize);
	MemClear(this->tmpArr, valSize);
}

Math::BigFloat::~BigFloat()
{
	MemFree(this->valArr);
	MemFree(this->tmpArr);
}

Int32 Math::BigFloat::operator =(Int32 val)
{
	UInt8 *varr = this->valArr;
	Int32 vsize = this->valSize;
	this->valIndex = 0;
	if (val < 0)
	{
		*(Int32*)varr = -val;
		this->isNeg = true;
	}
	else 
	{
		*(Int32*)varr = val;
		this->isNeg = false;
	}
	_asm
	{
		mov eax,0
		mov ecx,vsize
		mov edi,varr
		add edi,4
		shr ecx,2
		dec ecx
		rep stosd
		mov eax,val
	}
	RemZero();
}

Double Math::BigFloat::operator =(Double val)
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
}

Math::BigFloat *Math::BigFloat::operator =(const WChar *val)
{
	Int32 refIndex = 0;
	Int32 refIndex2 = 0;
	UInt8 *varr = valArr;
	Int32 vsize = valSize;
	Bool isDot = false;
	Int32 isExt = 0;
	WChar c;
	if (*val == '-')
	{
		this->isNeg = true;
		val++;
	}
	else
	{
		this->isNeg = false;
	}
	MemClear(varr, vsize);
	
	while (c = *val++)
	{
		if (c == '.')
		{
			if (isDot)
				return 0;
			isDot = true;
		}
		else if (c == '+')
		{
			if (isExt)
				return 0;
			isExt = 1;
		}
		else if (c == '-')
		{
			if (isExt)
				return 0;
			isExt = 2;
		}
		else if (c >= 0x30 && c <= 0x39)
		{
			if (isExt)
			{
				refIndex2 = refIndex2 * 10 + (c - 0x30);
			}
			else if (varr[valSize - 1])
			{
				if (!isDot)
				{
					refIndex++;
				}
			}
			else
			{
				_asm
				{
					mov ecx,vsize
					mov edi,varr
					shr ecx,2
					mov edx,0
					mov ebx,10
bfequlop:
					mov eax,dword ptr [edi]
					mov dword ptr [edi],edx
					mul ebx
					add dword ptr [edi],eax
					adc edx,0
					add edi,4
					dec ecx
					jnz bfequlop
					movzx eax,c
					mov edi,varr
					sub eax,0x30
					add dword ptr [edi],eax
					jnb bfequlop6
bfequlop5:
					add edi,4
					inc dword ptr [edi]
					jb bfequlop5
bfequlop6:

				}
				if (isDot)
					refIndex--;
			}
		}
		else
		{
			return 0;
		}
	}

	if (isExt == 2)
	{
		refIndex2 = -refIndex2;
	}
	valIndex = refIndex + refIndex2;
	RemZero();
	return this;
}

Math::BigFloat *Math::BigFloat::operator =(const BigFloat *val)
{
	if (val->valSize > this->valSize)
	{
		this->valSize = val->valSize;
		MemFree(this->valArr);
		this->valArr = MemAlloc(UInt8, this->valSize);
		MemCopyNO(this->valArr, val->valArr, this->valSize);
		this->valIndex = val->valIndex;
	}
	else
	{
		this->valIndex = val->valIndex;
		MemCopyNO(this->valArr, val->valArr, val->valSize);
		if (val->valSize < this->valSize)
		{
			MemClear(&this->valArr[val->valSize], this->valSize - val->valSize);
		}
	}
	this->isNeg = val->isNeg;
	return this;
}

Math::BigFloat *Math::BigFloat::operator =(const BigFloat &val)
{
	return *this = &val;
}

Math::BigFloat *Math::BigFloat::operator +=(const BigFloat *val)
{
	PrepareTmpBuff(val);
	PrepareSum();
	if (this->isNeg == val->isNeg)
	{
		DoSum();
	}
	else
	{
		if (DoSubtract())
			this->isNeg = !this->isNeg;
	}
	return this;
}

Math::BigFloat *Math::BigFloat::operator +=(const BigFloat &val)
{
	return *this += &val;
}

Math::BigFloat *Math::BigFloat::operator -=(const BigFloat *val)
{
	PrepareTmpBuff(val);
	PrepareSum();
	if (this->isNeg != val->isNeg)
	{
		DoSum();
	}
	else
	{
		if (DoSubtract())
			this->isNeg = !this->isNeg;
	}
	return this;
}

Math::BigFloat *Math::BigFloat::operator -=(const BigFloat &val)
{
	return *this -= &val;
}

Math::BigFloat *Math::BigFloat::operator *=(const Math::BigFloat *val)
{
	Int32 tsize;
	UInt8 *tarr = MemAlloc(UInt8, tsize = this->valSize + val->valSize);
	Int32 tIndex = this->valIndex + val->valIndex;
	UInt8 *tharr = this->valArr;
	UInt8 *vaarr = val->valArr;
	Int32 thsize = this->valSize;
	Int32 thsize2 = this->valSize;
	Int32 vasize = val->valSize;
	
	MemClear(tarr, tsize);
	_asm
	{
		mov edi,tharr
		mov ecx,thsize
bfmullop:
		cmp dword ptr [edi+ecx-4],0
		jnz bfmullop2
		sub ecx,4
		jnz bfmullop
bfmullop2:
		mov thsize,ecx

		mov edi,vaarr
		mov ecx,vasize
bfmullop3:
		cmp dword ptr [edi+ecx-4],0
		jnz bfmullop4
		sub ecx,4
		jnz bfmullop3
bfmullop4:
		mov vasize,ecx

		mov esi,vaarr
bfmullop4a:
		mov edi,tarr
		mov edx,tharr
		sub thsize,4
		jb bfmullop5
		mov ecx,thsize
		mov ebx,dword ptr [edx + ecx]
		add edi,ecx
		mov ecx,vasize
bfmullop4b:
		sub ecx,4
		jb bfmullop4a
		mov eax,dword ptr [esi + ecx]
		mul ebx
		add dword ptr [edi + ecx],eax
		adc dword ptr [edi + ecx + 4],edx
		jnb bfmullop4b
		push ecx
		add ecx,8
bfmullop4c:
		inc dword ptr [edi + ecx]
		jnb bfmullop4d
		add eax,4
		jmp bfmullop4c
bfmullop4d:
		pop ecx
		jmp bfmullop4b

bfmullop5:
		mov edi,tarr
		mov ecx,tsize
		mov ebx,10
bfmullop6:
		cmp dword ptr [edi + ecx - 4],0
		jnz bfmullop7
		sub ecx,4
		jz bfmullop7
		jmp bfmullop6
bfmullop7:
		cmp ecx,thsize2
		jbe bfmullop8
		mov edx,0
		mov edi,tarr
		push ecx
bfmullop7b:
		sub ecx,4
		jb bfmullop7c
		mov eax,dword ptr [edi + ecx]
		div ebx
		mov dword ptr [edi + ecx],eax
		jmp bfmullop7b
bfmullop7c:
		pop ecx
		inc tIndex
		cmp dword ptr [edi + ecx - 4],0
		jnz bfmullop7
		sub ecx,4
		jmp bfmullop7
bfmullop8:
	}
	MemCopyNO(this->valArr, tarr, this->valSize);

	MemFree(tarr);
	this->valIndex = tIndex;
	if (val->isNeg)
	{
		this->isNeg = !this->isNeg;
	}
	RemZero();
	return this;
}

Math::BigFloat *Math::BigFloat::operator *=(const BigFloat &val)
{
	return *this *= &val;
}

Math::BigFloat *Math::BigFloat::operator *=(Int32 val)
{
	if (val == 0)
	{
		MemClear(this->valArr, this->valSize);
		this->valIndex = 0;
		this->isNeg = false;
		return this;
	}

	if (val < 0)
	{
		val = -val;
		this->isNeg = !this->isNeg;
	}
	UInt8 *tharr = this->valArr;
	Int32 thsize = this->valSize;
	Int32 tIndex = this->valIndex;

	_asm
	{
		mov edi,tharr
		mov ecx,thsize
		mov ebx,10
		cmp dword ptr [edi+ecx-4],0
		jz bfmullop2
bfmullop:
		mov edx,0
bfmullop1:
		mov eax,dword ptr [edi+ecx-4]
		div ebx
		mov dword ptr [edi+ecx-4],eax
		sub ecx,4
		jnz bfmullop1

		inc tIndex
		mov ecx,thsize
		cmp dword ptr [edi+ecx-4],0
		jnz bfmullop
bfmullop2:
		mov ecx,thsize
		mov ebx,val
		mov edx,0
		mov esi,0
bfmullop3:
		mov eax,dword ptr [esi+edi]
		mov dword ptr [esi+edi],edx
		mul ebx
		add dword ptr [esi+edi],eax
		adc edx,0
		add esi,4
		cmp esi,ecx
		jb bfmullop3
	}
	this->valIndex = tIndex;
	RemZero();
	return this;
}

Math::BigFloat *Math::BigFloat::operator /=(const Math::BigFloat *val)
{
	Int32 tsize;
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
	}
	return this;
}

Math::BigFloat *Math::BigFloat::operator /=(const BigFloat &val)
{
	return *this /= &val;
}

Math::BigFloat *Math::BigFloat::operator /=(Int32 val)
{
	if (val == 0)
		return this;
	if (val < 0)
	{
		val = -val;
		this->isNeg = !this->isNeg;
	}
	if (val == 1)
	{
		return this;
	}

	UInt8 *tarr = this->tmpArr;
	UInt8 *varr = this->valArr;
	Int32 vsize = this->valSize;
	Int32 tIndex = this->valIndex;
	Int32 thisASize;

	MemCopyNO(tarr, varr, vsize);
	_asm
	{
		mov esi,tarr
		mov ecx,vsize
		mov edx,-4
		mov ebx,0
bfdivlop:
		cmp dword ptr [esi+ebx],0
		jz bfdivlop2
		mov edx,ebx
bfdivlop2:
		add ebx,4
		cmp ebx,ecx
		jb bfdivlop
		add edx,4
		jz bfdivexit
		mov thisASize,edx

		mov ebx,10
		mov edi,vsize
bfdivlop3:
		mov ecx,thisASize
		mov edx,0
		mov esi,tarr
		cmp ecx,edi
		jnb bfdivlop4
bfdivlop3a:
		mov eax,dword ptr [esi]
		mov dword ptr [esi],edx
		mul ebx
		add dword ptr [esi],eax
		adc edx,0
		add esi,4
		sub ecx,4
		jnz bfdivlop3a
		dec tIndex
		cmp edx,0
		jz bfdivlop3
		mov dword ptr [esi],edx
		add thisASize,4
		jmp bfdivlop3
bfdivlop4:

		mov edi,varr
		mov ecx,vsize
		mov eax,0
		shr ecx,2
		rep stosd

		mov ebx,val
		mov esi,tarr
		mov edi,varr
		mov ecx,thisASize
		mov edx,0
bfdivlop5:
		sub ecx,4
		mov eax,dword ptr [esi+ecx]
		div ebx
		mov dword ptr [edi+ecx],eax
		cmp ecx,0
		jnz bfdivlop5
		shr ebx,1
		adc ebx,0
		cmp edx,ebx
		jb bfdivlop6
bfdivlop7:
		add edi,4
		inc dword ptr [edi-4]
		jb bfdivlop7
bfdivlop6:
	}
	this->valIndex = tIndex;
	RemZero();
	_asm
	{
bfdivexit:
	}
	return this;
}

Math::BigFloat *Math::BigFloat::operator /=(UInt32 val)
{
	if (val == 0)
		return this;
	if (val == 1)
	{
		return this;
	}

	UInt8 *tarr = this->tmpArr;
	UInt8 *varr = this->valArr;
	Int32 vsize = this->valSize;
	Int32 tIndex = this->valIndex;
	Int32 thisASize;

	MemCopyNO(tarr, varr, vsize);
	_asm
	{
		mov esi,tarr
		mov ecx,vsize
		mov edx,-4
		mov ebx,0
bfdivlop:
		cmp dword ptr [esi+ebx],0
		jz bfdivlop2
		mov edx,ebx
bfdivlop2:
		add ebx,4
		cmp ebx,ecx
		jb bfdivlop
		add edx,4
		jz bfdivexit
		mov thisASize,edx

		mov ebx,10
		mov edi,vsize
bfdivlop3:
		mov ecx,thisASize
		mov edx,0
		mov esi,tarr
		cmp ecx,edi
		jnb bfdivlop4
bfdivlop3a:
		mov eax,dword ptr [esi]
		mov dword ptr [esi],edx
		mul ebx
		add dword ptr [esi],eax
		adc edx,0
		add esi,4
		sub ecx,4
		jnz bfdivlop3a
		dec tIndex
		cmp edx,0
		jz bfdivlop3
		mov dword ptr [esi],edx
		add thisASize,4
		jmp bfdivlop3
bfdivlop4:

		mov edi,varr
		mov ecx,vsize
		mov eax,0
		shr ecx,2
		rep stosd

		mov ebx,val
		mov esi,tarr
		mov edi,varr
		mov ecx,thisASize
		mov edx,0
bfdivlop5:
		sub ecx,4
		mov eax,dword ptr [esi+ecx]
		div ebx
		mov dword ptr [edi+ecx],eax
		cmp ecx,0
		jnz bfdivlop5
		shr ebx,1
		adc ebx,0
		cmp edx,ebx
		jb bfdivlop6
bfdivlop7:
		add edi,4
		inc dword ptr [edi-4]
		jb bfdivlop7
bfdivlop6:
	}
	this->valIndex = tIndex;
	RemZero();
	_asm
	{
bfdivexit:
	}
	return this;
}

Math::BigFloat *Math::BigFloat::ToNeg()
{
	Int32 i = this->valSize;
	this->isNeg = !this->isNeg;
	while (i-- > 0)
	{
		if (this->valArr[i] == 0)
			return this;
	}
	this->isNeg = 0;
	return this;
}

Bool Math::BigFloat::IsZero()
{
	Int32 i = this->valSize;
	while (i > 0)
	{
		i -= 4;
		if (*(Int32*)&this->valArr[i])
			return false;
	}
	return true;
}

Bool Math::BigFloat::operator >(const BigFloat *val)
{
	if (this->isNeg != val->isNeg)
	{
		if (this->isNeg)
			return false;
		else
			return true;
	}
	Math::BigFloat tmp(this->valSize);
	tmp = this;
	tmp -= val;
	if (tmp.isNeg)
		return false;
	else if (tmp.IsZero())
		return false;
	else
		return true;
}

Bool Math::BigFloat::operator >=(const BigFloat *val)
{
	if (this->isNeg != val->isNeg)
	{
		if (this->isNeg)
			return false;
		else
			return true;
	}
	Math::BigFloat tmp(this->valSize);
	tmp = this;
	tmp -= val;
	if (tmp.isNeg)
		return false;
	else if (tmp.IsZero())
		return true;
	else
		return true;
}

Bool Math::BigFloat::operator <(const BigFloat *val)
{
	if (this->isNeg != val->isNeg)
	{
		if (this->isNeg)
			return true;
		else
			return false;
	}
	Math::BigFloat tmp(this->valSize);
	tmp = this;
	tmp -= val;
	if (tmp.isNeg)
		return true;
	else if (tmp.IsZero())
		return false;
	else
		return false;
}

Bool Math::BigFloat::operator <=(const BigFloat *val)
{
	if (this->isNeg != val->isNeg)
	{
		if (this->isNeg)
			return true;
		else
			return false;
	}
	Math::BigFloat tmp(this->valSize);
	tmp = this;
	tmp -= val;
	if (tmp.isNeg)
		return true;
	else if (tmp.IsZero())
		return true;
	else
		return false;
}

Bool Math::BigFloat::operator ==(const BigFloat *val)
{
	if (this->isNeg != val->isNeg)
	{
		return false;
	}
	Math::BigFloat tmp(this->valSize);
	tmp = this;
	tmp -= val;
	if (tmp.IsZero())
		return true;
	else
		return false;
}

Math::BigFloat *Math::BigFloat::Factorial(Int32 val)
{
	if (val < 1)
	{
		*this = 1;
		return this;
	}
	*this = 1;
	Math::BigFloat tmp(this->valSize);
	while (val > 1)
	{
		tmp = val;
		*this *= &tmp;
		val--;
	}
	return this;
}

Math::BigFloat *Math::BigFloat::SetPI()
{
	Math::BigFloat tmpVal(this->valSize);
	Int32 endInd = -(this->valSize * 5) >> 1;
	Int32 v = 1;
	Int32 v2 = 3;
	*this = 1;
	tmpVal = 1;
	while (v < 0x10000)
	{
		tmpVal *= v;
		tmpVal /= v2;
		*this += &tmpVal;

		if (tmpVal.valIndex - this->valIndex < endInd)
			break;
		v++;
		v2 += 2;
	}
	tmpVal = 2;
	*this *= &tmpVal;
	return this;
}

Math::BigFloat *Math::BigFloat::SetE(Math::BigFloat *val)
{
	Math::BigFloat tmpVal(this->valSize);
	Math::BigFloat tmpVal2(val->valSize);
	Int32 endInd = -(this->valSize * 5) >> 1;
	Int32 i = 2;
	tmpVal2 = val;
	tmpVal = &tmpVal2;
	*this = 1;
	*this += &tmpVal;

	while (true)
	{
		tmpVal *= &tmpVal2;
		tmpVal /= i;
		if (i > 10 && tmpVal.valIndex - this->valIndex < endInd)
			break;
		*this += &tmpVal;
		i++;
	}
	return this;
}

Math::BigFloat *Math::BigFloat::SetLn(BigFloat *val)
{
	if (val->isNeg)
	{
		return 0;

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
		Math::BigFloat tmp(this->valSize);
		Math::BigFloat tmp2(this->valSize);
		Math::BigFloat tmp3(this->valSize);
		Math::BigFloat e0(this->valSize);
		Math::BigFloat e1(this->valSize);
		Math::BigFloat e2(this->valSize);
		Int32 i = 1;
		Int32 j;
		tmp = 1;
		e0.SetE(&tmp);
		e2 = &tmp;
		e2 /= &e0;

		Int32 endInd = -(this->valSize * 5) >> 1;
		if (*val > &e0)
		{
			tmp3 = val;
			tmp2 = 0;
			while (tmp3 > &e0)
			{
				tmp = 1;
				e1.SetE(&tmp);
				e2 = &tmp;
				e2 /= &e1;

				e1 *= &e1;
				j = 1;
				while (tmp3 > &e1)
				{
					e1 *= &e1;
					e2 *= &e2;
					tmp *= 2;
					j++;
				}
				tmp3 *= &e2;
				tmp2 += &tmp;
			}
			*this = &tmp2;

			tmp = 1;
			tmp2 = &tmp3;
			tmp2 -= &tmp;
			tmp2 /= &tmp3;
			tmp = &tmp2;
			*this += &tmp;
			while (true)
			{
				tmp *= &tmp2;
				tmp3 = &tmp;
				tmp3 /= ++i;
				*this += &tmp3;
				if (i > 10 && tmp3.valIndex - this->valIndex < endInd)
					break;
			}
		}
		else if (*val > &tmp)
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
				if (i > 10 && tmp3.valIndex - this->valIndex < endInd)
					break;
			}
		}
		else if (*val >= &e2)
		{
			tmp2 = val;
			tmp = 1;
			tmp2 -= &tmp;
			*this = &tmp2;
			tmp = &tmp2;
			while (true)
			{
				i++;
				tmp *= &tmp2;
				tmp3 = &tmp;
				tmp3 /= i;
				if (i & 1)
				{
					*this += &tmp3;
				}
				else
				{
					*this -= &tmp3;
				}
				if (i > 10 && tmp3.valIndex - this->valIndex < endInd)
					break;
			}
		}
		else
		{
			e0 = &e2;

			tmp3 = val;
			tmp2 = 0;
			while (tmp3 < &e0)
			{
				tmp = 1;
				e1.SetE(&tmp);
				e2 = &tmp;
				e2 /= &e1;

				e2 *= &e2;
				j = 1;
				while (tmp3 < &e2)
				{
					e1 *= &e1;
					e2 *= &e2;
					tmp *= 2;
					j++;
				}
				tmp3 *= &e1;
				tmp2 -= &tmp;
			}
			*this = &tmp2;

			tmp2 = &tmp3;
			tmp = 1;
			tmp2 -= &tmp;
			*this += &tmp2;
			tmp = &tmp2;
			while (true)
			{
				i++;
				tmp *= &tmp2;
				tmp3 = &tmp;
				tmp3 /= i;
				if (i & 1)
				{
					*this += &tmp3;
				}
				else
				{
					*this -= &tmp3;
				}
				if (i > 10 && tmp3.valIndex - this->valIndex < endInd)
					break;
			}
		}
	}
	return this;
}

Math::BigFloat *Math::BigFloat::SetSin(const Math::BigFloat *val)
{
	Math::BigFloat tmpVal(this->valSize);
	UInt32 i = 1;
	Int32 endInd = -(this->valSize * 5) >> 1;
	tmpVal = val;
	*this = &tmpVal;

	while (true)
	{
		tmpVal *= val;
		tmpVal *= val;
		tmpVal /= ++i;
		tmpVal /= ++i;
		tmpVal.ToNeg();
		if (i > 10 && tmpVal.valIndex - this->valIndex < endInd)
			break;
		*this += &tmpVal;
	}
	return this;
}

Math::BigFloat *Math::BigFloat::SetSin(const Math::BigFloat &val)
{
	return this->SetSin(&val);
}

Math::BigFloat *Math::BigFloat::SetCos(const Math::BigFloat *val)
{
	Math::BigFloat tmpVal(this->valSize);
	Int32 endInd = -(this->valSize * 5) >> 1;
	UInt32 i = 0;
	tmpVal = 1;
	*this = &tmpVal;

	while (true)
	{
		tmpVal *= val;
		tmpVal *= val;
		tmpVal /= ++i;
		tmpVal /= ++i;
		tmpVal.ToNeg();
		if (i > 10 && tmpVal.valIndex - this->valIndex < endInd)
			break;
		*this += &tmpVal;
	}
	return this;
}

Math::BigFloat *Math::BigFloat::SetCos(const Math::BigFloat &val)
{
	return this->SetCos(&val);
}

Math::BigFloat *Math::BigFloat::SetTan(const Math::BigFloat *val)
{
	Math::BigFloat tmpVal(this->valSize);
	tmpVal.SetCos(val);
	this->SetSin(val);
	return *this /= &tmpVal;
}

Math::BigFloat *Math::BigFloat::SetTan(const Math::BigFloat &val)
{
	return this->SetTan(&val);
}

Int32 Math::BigFloat::GetSize()
{
	return this->valSize;
}

WChar *Math::BigFloat::ToString(WChar *buff)
{
	WChar *strBuff;
	WChar *wptr;
	WChar *wptr2;
	Int32 vsize = valSize;
	Int32 vindex = valIndex;
	OSInt ssize;
	UInt8 *tarr = tmpArr;
	MemCopyNO(tmpArr, valArr, vsize = valSize);

	strBuff = MemAlloc(WChar, ssize = vsize * 3);
	wptr = &strBuff[ssize];
	*--wptr = 0;
	wptr2 = wptr;
	_asm
	{
		mov edi,tarr
		mov ecx,vsize
		lea esi,[edi+ecx-4]
bftslop4:
		cmp dword ptr [esi],0
		jnz bftslop5
		cmp esi,tarr
		jz bftslop6
		sub esi,4
		jmp bftslop4
bftslop5:
		mov edi,esi
		mov edx,0
		mov ebx,10
		mov ecx,tarr
bftslop5b:
		mov eax,dword ptr [edi]
		div ebx
		mov dword ptr [edi],eax
		sub edi,4
		cmp edi,ecx
		jnb bftslop5b
		add edx,0x30
		mov ebx,wptr
		mov word ptr [ebx-2],dx
		sub wptr,2
		jmp bftslop4
bftslop6:
	}
	
	if (wptr == wptr2)
	{
		*buff++ = '0';
		*buff = 0;
		MemFree(strBuff);
		return buff;
	}
	if (this->isNeg)
	{
		*buff++ = '-';
	}
	ssize = wptr2 - wptr;
	if (vindex > 3 || (vindex < -ssize - 2))
	{
		*buff++ = *wptr++;
		if (wptr != wptr2)
		{
			*buff++ = '.';
			vindex += (Int32)(wptr2 - wptr);
			buff = Text::StrConcat(buff, wptr);
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
		buff = Text::StrConcat(buff, wptr);
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
			*buff++ = *wptr++;
		}
		*buff++ = '.';
		buff = Text::StrConcat(buff, wptr);
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
		buff = Text::StrConcat(buff, wptr);
	}
	MemFree(strBuff);
	return buff;
}
