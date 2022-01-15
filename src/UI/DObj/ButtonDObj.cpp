#include "Stdafx.h"
#include "Math/Math.h"
#include "Media/GDIEngine.h"
#include "Sync/Thread.h"
#include "UI/DObj/ButtonDObj.h"

UInt32 __stdcall UI::DObj::ButtonDObj::ClickThread(void *userObj)
{
	UI::DObj::ButtonDObj *me = (UI::DObj::ButtonDObj*)userObj;
	me->clkHdlr(me->clkUserObj);
	return 0;
}

UI::DObj::ButtonDObj::ButtonDObj(Media::DrawEngine *deng, const WChar *fileNameUnclick, const WChar *fileNameClicked, Int32 left, Int32 top, Bool rectMode, UI::UIEvent clkHdlr, void *clkUserObj) : DirectObject(left, top)
{
	this->deng = deng;
	if (fileNameUnclick == 0)
	{
		this->bmpUnclick = 0;
	}
	else
	{
		this->bmpUnclick = this->deng->LoadImageW(fileNameUnclick);
	}

	if (fileNameClicked == 0)
	{
		this->bmpClicked = 0;
	}
	else
	{
		this->bmpClicked = this->deng->LoadImageW(fileNameClicked);
	}
	this->isVisible = true;
	NEW_CLASS(this->rnd, Data::RandomOS());
	NEW_CLASS(this->downTime, Data::DateTime());
	this->alpha = this->rnd->NextDouble() * 0.5 + 0.5;
	this->a = 0;
	this->clkHdlr = clkHdlr;
	this->clkUserObj = clkUserObj;
	this->isMouseDown = false;
	this->isMouseUp = false;
	this->rectMode = rectMode;
}

UI::DObj::ButtonDObj::~ButtonDObj()
{
	if (this->bmpUnclick)
	{
		this->deng->DeleteImage(this->bmpUnclick);
		this->bmpUnclick = 0;
	}
	if (this->bmpClicked)
	{
		this->deng->DeleteImage(this->bmpClicked);
		this->bmpClicked = 0;
	}
	DEL_CLASS(this->downTime);
	DEL_CLASS(this->rnd);
}

void UI::DObj::ButtonDObj::SetRectMode(Bool rectMode)
{
	this->rectMode = rectMode;
}

void UI::DObj::ButtonDObj::SetVisible(Bool isVisible)
{
	this->isVisible = isVisible;
}

Bool UI::DObj::ButtonDObj::IsChanged()
{
	if (this->IsMoving())
	{
		return true;
	}
	else if (this->bmpUnclick != 0 && this->bmpClicked != 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

Bool UI::DObj::ButtonDObj::DoEvents()
{
	Data::DateTime currTime;
	currTime.SetCurrTime();
	if (this->isMouseDown && this->isVisible)
	{
		if (this->isMouseUp && (currTime.DiffMS(this->downTime) >= 500 || this->bmpClicked == 0 || this->bmpUnclick == 0))
		{
			this->isMouseDown = false;
			this->isMouseUp = false;
			if (this->isMouseClick)
			{
				this->isMouseClick = false;
				Sync::Thread::Create(ClickThread, this);
				return true;
			}
		}
	}
	return false;
}

void UI::DObj::ButtonDObj::DrawObject(Media::DrawImage *dimg)
{
	OSInt left;
	OSInt top;
	if (!this->isVisible)
		return;

	this->GetCurrPos(&left, &top);
	this->dispLeft = left;
	this->dispTop = top;
	if (this->bmpUnclick && this->bmpClicked)
	{
		if (this->bmpUnclick->GetWidth() == this->bmpClicked->GetWidth() && this->bmpUnclick->GetHeight() == this->bmpClicked->GetHeight())
		{
			if (this->isMouseDown)
			{
				Data::DateTime currTime;
				currTime.SetCurrTime();
				Double t = currTime.DiffMS(this->downTime) * 0.001;
				if (t > 0.5)
				{
					this->alpha = 0;
				}
				else
				{
					this->alpha = (0.5 - t) / 0.5 * this->downAlpha;
				}
			}
			else
			{
				this->a += (this->rnd->NextDouble() - 0.5) * 0.008;
				this->alpha += this->a;
				if (this->alpha > 1)
				{
					this->alpha = 1;
					this->a = 0;
				}
				else if (this->alpha < 0.5)
				{
					this->alpha = 0.5;
					this->a = 0;
				}
			}
			Media::GDIImage *bmpS1 = (Media::GDIImage*)this->bmpUnclick;
			Media::GDIImage *bmpS2 = (Media::GDIImage*)this->bmpClicked;
			Media::GDIImage *bmpTmp = (Media::GDIImage*)this->deng->CreateImage32(bmpS1->GetWidth(), bmpS1->GetHeight(), Media::AT_NO_ALPHA);
			bmpTmp->info->atype = bmpS1->info->atype;
#if defined(HAS_ASM32)
			UInt8 *ptrS1 = (UInt8*)bmpS1->bmpBits;
			UInt8 *ptrS2 = (UInt8*)bmpS2->bmpBits;
			UInt8 *ptrD = (UInt8*)bmpTmp->bmpBits;
//			OSInt bpl = bmpS1->GetWidth() * 4;
//			OSInt w = bmpS1->GetWidth();
//			OSInt h = bmpS1->GetHeight();
			OSInt pxCnt = bmpS1->GetWidth() * bmpS1->GetHeight();
			OSInt a1 = Double2Int32(this->alpha * 32767);
			OSInt a2 = Double2Int32((1 - this->alpha) * 32767);
			if ((((OSInt)ptrD) & 15) != 0 || (pxCnt & 3) != 0)
			{
				_asm
				{
					mov esi,ptrS1
					mov edi,ptrD
					mov ebx,ptrS2
					mov eax,a1
					mov edx,a2
					movd xmm0,eax
					movd xmm1,edx
					punpcklwd xmm0,xmm1
					punpckldq xmm0,xmm0
					punpcklqdq xmm0,xmm0
					pxor xmm1,xmm1
					mov ecx,pxCnt
					align 16
bdolop1:
					movd xmm2,[esi]
					movd xmm3,[ebx]
					punpcklbw xmm2,xmm3
					punpcklbw xmm2,xmm1
					pmaddwd xmm2,xmm0
					psrld xmm2,15
					packssdw xmm2,xmm1
					packuswb xmm2,xmm1
					movd [edi],xmm2
					lea esi,[esi+4]
					lea edi,[edi+4]
					lea ebx,[ebx+4]
					dec ecx
					jnz bdolop1
				}
			}
			else
			{
				_asm
				{
					mov esi,ptrS1
					mov edi,ptrD
					mov ebx,ptrS2
					mov eax,a1
					mov edx,a2
					movd xmm0,eax
					movd xmm1,edx
					punpcklwd xmm0,xmm1
					punpckldq xmm0,xmm0
					punpcklqdq xmm0,xmm0
					pxor xmm1,xmm1
					mov ecx,pxCnt
					shr ecx,2
					align 16
bdolop2:
					movdqu xmm2,[esi]
					movdqu xmm3,[ebx]
					movdqa xmm4,xmm2
					punpcklbw xmm2,xmm3
					punpckhbw xmm4,xmm3
					movdqa xmm5,xmm2
					punpcklbw xmm2,xmm1
					punpckhbw xmm5,xmm1
					pmaddwd xmm2,xmm0
					pmaddwd xmm5,xmm0
					psrld xmm2,15
					psrld xmm5,15
					packssdw xmm2,xmm5
					movdqa xmm3,xmm4
					punpcklbw xmm3,xmm1
					punpckhbw xmm4,xmm1
					pmaddwd xmm3,xmm0
					pmaddwd xmm4,xmm0
					psrld xmm3,15
					psrld xmm4,15
					packssdw xmm3,xmm4
					packuswb xmm2,xmm3
					movntdq [edi],xmm2
					lea esi,[esi+16]
					lea edi,[edi+16]
					lea ebx,[ebx+16]
					dec ecx
					jnz bdolop2
				}
			}
#else
			UInt8 *ptrS1 = (UInt8*)bmpS1->bmpBits;
			UInt8 *ptrS2 = (UInt8*)bmpS2->bmpBits;
			UInt8 *ptrD = (UInt8*)bmpTmp->bmpBits;
			OSInt lineBytes = bmpS1->GetWidth() * 4;
			OSInt i;
			OSInt j = bmpS1->GetHeight();
			Double a1 = this->alpha;
			Double a2 = 1 - this->alpha; 
			while (j-- > 0)
			{
				i = lineBytes;
				while (i-- > 0)
				{
					*ptrD++ = (UInt8)Double2Int32(a1 * (*ptrS1++) + a2 * (*ptrS2++));
				}
			}
#endif
			dimg->DrawImagePt(bmpTmp, OSInt2Double(left), OSInt2Double(top));
			this->deng->DeleteImage(bmpTmp);
		}
		else
		{
			dimg->DrawImagePt(this->bmpUnclick, OSInt2Double(left), OSInt2Double(top));
		}
	}
	else if (this->bmpUnclick)
	{
		dimg->DrawImagePt(this->bmpUnclick, OSInt2Double(left), OSInt2Double(top));
	}
	else if (this->bmpClicked)
	{
		dimg->DrawImagePt(this->bmpClicked, OSInt2Double(left), OSInt2Double(top));
	}
}

Bool UI::DObj::ButtonDObj::IsObject(OSInt x, OSInt y)
{
	if (x < this->dispLeft || y < this->dispTop || !this->isVisible)
		return false;
	Media::DrawImage *bmpChk = this->bmpUnclick;
	if (bmpChk == 0)
	{
		bmpChk = this->bmpClicked;
		if (bmpChk == 0)
			return false;
	}
	if (this->dispLeft + (OSInt)bmpChk->GetWidth() <= x || this->dispTop + (OSInt)bmpChk->GetHeight() <= y)
		return false;
	if (this->rectMode)
		return true;
	return (((Media::GDIImage*)bmpChk)->GetPixel32(x - this->dispLeft, y - this->dispTop) & 0xff000000) != 0;
}

/*System::Windows::Forms::Cursor ^UI::DObj::ButtonDObj::GetCursor()
{
	return System::Windows::Forms::Cursors::Hand;
}*/

void UI::DObj::ButtonDObj::OnMouseDown()
{
	this->downAlpha = this->alpha;
	this->isMouseDown = true;
	this->downTime->SetCurrTime();
}

void UI::DObj::ButtonDObj::OnMouseUp()
{
	this->isMouseUp = true;
}

void UI::DObj::ButtonDObj::OnMouseClick()
{
	if (this->isMouseDown)
	{
		this->isMouseClick = true;
	}
	else
	{
		this->clkHdlr(this->clkUserObj);
	}
}
