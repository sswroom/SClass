#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/VectorView.h"
#include "Math/Math_C.h"

Math::VectorView::VectorView(Int32 scnWidth, Int32 scnHeight, Double left, Double top, Double right, Double bottom)
{
	ChangeView(scnWidth, scnHeight, left, top, right, bottom);
}

Math::VectorView::~VectorView()
{
}

void Math::VectorView::ChangeView(Int32 scnWidth, Int32 scnHeight, Double left, Double top, Double right, Double bottom)
{
	this->scnWidth = scnWidth;
	this->scnHeight = scnHeight;
	this->left = left;
	this->top = top;
	this->viewW = right - left;
	this->viewH = bottom - top;
}

Double Math::VectorView::GetLeft()
{
	return this->left;
}

Double Math::VectorView::GetTop()
{
	return this->top;
}

Double Math::VectorView::GetRight()
{
	return this->left + this->viewW;
}

Double Math::VectorView::GetBottom()
{
	return this->top + this->viewH;
}

Double Math::VectorView::GetCentX()
{
	return this->left + (this->viewW * 0.5);
}

Double Math::VectorView::GetCentY()
{
	return this->top + (this->viewH * 0.5);
}

Int32 Math::VectorView::GetScnWidth()
{
	return this->scnWidth;
}

Int32 Math::VectorView::GetScnHeight()
{
	return this->scnHeight;
}

Bool Math::VectorView::InView(Double xPos, Double yPos)
{
	return (xPos >= left) && (xPos < left + viewW) && (yPos >= top) && (yPos < top + viewH);
}

Bool Math::VectorView::ViewToScnXY(const Double *srcArr, Int32 *destArr, Int32 nPoints, Int32 ofstX, Int32 ofstY)
{
	Int32 scnW = this->scnWidth;
	Int32 scnH = this->scnHeight;
	Double xmul = scnW / viewW;
	Double ymul = scnH / viewH;
	Double dleft = this->left;
	Double dtop = this->top;
	if (nPoints == 0)
	{
		return false;
	}
#ifdef HAS_ASM32
	Int32 iminX;
	Int32 imaxX;
	Int32 iminY;
	Int32 imaxY;
	_asm
	{
		mov esi,srcArr
		mov edi,destArr
		mov ecx,nPoints
		mov ebx,0x7fffffff
		mov edx,0x80000000
mtslop:
		fld qword ptr [esi]
		fsub dleft
		fmul xmul
		fiadd ofstX
		fistp dword ptr [edi]

		fld qword ptr [esi+8]
		fsub dtop
		fmul ymul
		fiadd ofstY
		fistp dword ptr [edi+4]

		mov eax,dword ptr [edi]
		cmp ebx,eax
		cmovg ebx,eax
		cmp edx,eax
		cmovl edx,eax

		add esi,16
		add edi,8
		dec ecx
		jnz mtslop

		mov iminX,ebx
		mov imaxX,edx

		mov esi,srcArr
		mov ecx,nPoints
		mov ebx,0x7fffffff
		mov edx,0x80000000
mtslop4:
		add esi,4
		lodsd
		cmp ebx,eax
		cmovg ebx,eax
		cmp edx,eax
		cmovl edx,eax

		dec ecx
		jnz mtslop4

		mov iminY,ebx
		mov imaxY,edx
	}
	return (imaxX >= 0) && (iminX < scnWidth) && (imaxY >= 0) && (iminY < scnHeight);
#else
	Int32 iminX = 0;
	Int32 iminY = 0;
	Int32 imaxX = 0;
	Int32 imaxY = 0;
	Int32 thisX;
	Int32 thisY;
	while (nPoints-- > 0)
	{
		*destArr++ = thisX = Double2Int32((*srcArr++ - dleft) * xmul + ofstX);
		*destArr++ = thisY = Double2Int32((*srcArr++ - dtop) * ymul + ofstY);
		if (iminX == 0 && imaxX == 0)
		{
			iminX = imaxX = thisX;
			iminY = imaxY = thisY;
		}
		else
		{
			if (thisX < iminX)
				iminX = thisX;
			if (thisX > imaxX)
				imaxX = thisX;
			if (thisY < iminY)
				iminY = thisY;
			if (thisY > imaxY)
				imaxY = thisY;
		}
	}
	return (imaxX >= 0) && (iminX < scnWidth) && (imaxY >= 0) && (iminY < scnHeight);
#endif
}

void Math::VectorView::ViewToScnXY(Double xPos, Double yPos, Int32 *scnX, Int32 *scnY)
{
	*scnX = Double2Int32((xPos - this->left) * this->scnWidth / this->viewW);
	*scnY = Double2Int32((yPos - this->top) * this->scnWidth / this->viewH);
}

void Math::VectorView::ScnXYToView(Int32 scnX, Int32 scnY, Double *xPos, Double *yPos)
{
	*xPos = this->left + scnX * viewW / scnWidth;
	*yPos = this->top + scnY * viewH / scnHeight;
}

void Math::VectorView::UpdateSize(Int32 width, Int32 height)
{
	ChangeView(width, height, this->left, this->top, this->left + this->viewW, this->top + this->viewH);
}
