#include "Stdafx.h"
#include "Math/Math_C.h"
#include "Media/DrawEngine.h"
#include "Sync/ThreadUtil.h"
#include "UI/DObj/ButtonDObj.h"

UInt32 __stdcall UI::DObj::ButtonDObj::ClickThread(AnyType userObj)
{
	NN<UI::DObj::ButtonDObj> me = userObj.GetNN<UI::DObj::ButtonDObj>();
	me->clkHdlr(me->clkUserObj);
	return 0;
}

UI::DObj::ButtonDObj::ButtonDObj(NN<Media::DrawEngine> deng, Text::CString fileNameUnclick, Text::CString fileNameClicked, Math::Coord2D<OSInt> tl, Bool rectMode, UI::UIEvent clkHdlr, AnyType clkUserObj) : DirectObject(tl)
{
	this->deng = deng;
	if (fileNameUnclick.leng == 0)
	{
		this->bmpUnclick = 0;
	}
	else
	{
		this->bmpUnclick = this->deng->LoadImage(fileNameUnclick.OrEmpty());
	}

	if (fileNameClicked.leng == 0)
	{
		this->bmpClicked = 0;
	}
	else
	{
		this->bmpClicked = this->deng->LoadImage(fileNameClicked.OrEmpty());
	}
	this->isVisible = true;
	NEW_CLASS(this->rnd, Data::RandomOS());
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
	NN<Media::DrawImage> img;
	if (this->bmpUnclick.SetTo(img))
	{
		this->deng->DeleteImage(img);
		this->bmpUnclick = 0;
	}
	if (this->bmpClicked.SetTo(img))
	{
		this->deng->DeleteImage(img);
		this->bmpClicked = 0;
	}
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
		if (this->isMouseUp && (currTime.DiffMS(this->downTime) >= 500 || this->bmpClicked.IsNull() || this->bmpUnclick.IsNull()))
		{
			this->isMouseDown = false;
			this->isMouseUp = false;
			if (this->isMouseClick)
			{
				this->isMouseClick = false;
				Sync::ThreadUtil::Create(ClickThread, this);
				return true;
			}
		}
	}
	return false;
}

void UI::DObj::ButtonDObj::DrawObject(NN<Media::DrawImage> dimg)
{
	if (!this->isVisible)
		return;

	Math::Coord2D<OSInt> tl = this->GetCurrPos();
	this->dispTL = tl;
	NN<Media::DrawImage> bmpUnclick;
	NN<Media::DrawImage> bmpClicked;
	if (this->bmpUnclick.SetTo(bmpUnclick) && this->bmpClicked.SetTo(bmpClicked))
	{
		if (bmpUnclick->GetWidth() == bmpClicked->GetWidth() && bmpUnclick->GetHeight() == bmpClicked->GetHeight())
		{
			if (this->isMouseDown)
			{
				Data::DateTime currTime;
				currTime.SetCurrTime();
				Double t = Int64_Double(currTime.DiffMS(this->downTime)) * 0.001;
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
			NN<Media::DrawImage> bmpS1 = bmpUnclick;
			NN<Media::DrawImage> bmpS2 = bmpClicked;
			NN<Media::DrawImage> bmpTmp;
			if (this->deng->CreateImage32(bmpS1->GetSize(), bmpS1->GetAlphaType()).SetTo(bmpTmp))
			{
				bmpTmp->SetAlphaType(bmpS1->GetAlphaType());
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
				Bool revOrder;
				UnsafeArray<UInt8> ptrS1;
				UnsafeArray<UInt8> ptrS2;
				UnsafeArray<UInt8> ptrD;
				if (bmpS1->GetImgBits(revOrder).SetTo(ptrS1) && bmpS2->GetImgBits(revOrder).SetTo(ptrS2) && bmpTmp->GetImgBits(revOrder).SetTo(ptrD))
				{
					UOSInt lineBytes = bmpS1->GetWidth() * 4;
					UOSInt i;
					UOSInt j = bmpS1->GetHeight();
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
					bmpTmp->GetImgBitsEnd(true);
					bmpS2->GetImgBitsEnd(false);
					bmpS1->GetImgBitsEnd(false);
				}
	#endif
				dimg->DrawImagePt(bmpTmp, tl.ToDouble());
				this->deng->DeleteImage(bmpTmp);
			}
		}
		else
		{
			dimg->DrawImagePt(bmpUnclick, tl.ToDouble());
		}
	}
	else if (this->bmpUnclick.SetTo(bmpUnclick))
	{
		dimg->DrawImagePt(bmpUnclick, tl.ToDouble());
	}
	else if (this->bmpClicked.SetTo(bmpClicked))
	{
		dimg->DrawImagePt(bmpClicked, tl.ToDouble());
	}
}

Bool UI::DObj::ButtonDObj::IsObject(Math::Coord2D<OSInt> scnPos)
{
	if (scnPos.x < this->dispTL.x || scnPos.y < this->dispTL.y || !this->isVisible)
		return false;
	NN<Media::DrawImage> bmpChk;
	if (!this->bmpUnclick.SetTo(bmpChk))
	{
		if (!this->bmpClicked.SetTo(bmpChk))
			return false;
	}
	if (this->dispTL.x + (OSInt)bmpChk->GetWidth() <= scnPos.x || this->dispTL.y + (OSInt)bmpChk->GetHeight() <= scnPos.y)
		return false;
	if (this->rectMode)
		return true;
	return (bmpChk->GetPixel32(scnPos.x - this->dispTL.x, scnPos.y - this->dispTL.y) & 0xff000000) != 0;
}

/*System::Windows::Forms::Cursor ^UI::DObj::ButtonDObj::GetCursor()
{
	return System::Windows::Forms::Cursors::Hand;
}*/

void UI::DObj::ButtonDObj::OnMouseDown()
{
	this->downAlpha = this->alpha;
	this->isMouseDown = true;
	this->downTime.SetCurrTime();
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
