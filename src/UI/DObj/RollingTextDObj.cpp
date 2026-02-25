#include "Stdafx.h"
#include "Math/Math_C.h"
#include "Media/DrawImageTool.h"
#include "Media/ImageUtil_C.h"
#include "Text/StringBuilder.hpp"
#include "UI/DObj/RollingTextDObj.h"

void UI::DObj::RollingTextDObj::UpdateBGImg()
{
	NN<Media::DrawImage> img;
	if (this->dimg.SetTo(img))
	{
		this->deng->DeleteImage(img);
		this->dimg = nullptr;
	}

	NN<Text::String> txt;
	if (this->txt.SetTo(txt))
	{
		NN<Media::DrawImage> dimg;
		NN<Media::DrawFont> f;
		NN<Media::DrawBrush> b;
		Data::ArrayListStringNN lines;
		NN<Text::String> s;
		Double currY;
		if (this->deng->CreateImage32(this->size, Media::AT_ALPHA_ALL_FF).SetTo(dimg))
		{
			f = dimg->NewFontPx(this->fontName->ToCString(), this->fontSize, Media::DrawEngine::DFS_ANTIALIAS, this->codePage);

			Media::DrawImageTool::SplitString(dimg, txt->ToCString(), lines, f, IntOS2Double(this->size.x));
			dimg->DelFont(f);
			this->deng->DeleteImage(dimg);
		}

		this->dimg = this->deng->CreateImage32(Math::Size2D<UIntOS>(this->size.x, (UInt32)Double2Int32(this->lineHeight * UIntOS2Double(lines.GetCount()))), Media::AT_ALPHA_ALL_FF);
		if (this->dimg.SetTo(dimg))
		{
			f = dimg->NewFontPx(this->fontName->ToCString(), this->fontSize, Media::DrawEngine::DFS_ANTIALIAS, this->codePage);
			b = dimg->NewBrushARGB(0xffffffff);
			currY = 0;
			Data::ArrayIterator<NN<Text::String>> it = lines.Iterator();
			while (it.HasNext())
			{
				s = it.Next();
				dimg->DrawString(Math::Coord2DDbl(0, currY), s, f, b);
				s->Release();
				currY += this->lineHeight;
			}
			dimg->DelBrush(b);
			dimg->DelFont(f);

			Bool revOrder;
			UnsafeArray<UInt8> bmpPtr;
			if (dimg->GetImgBits(revOrder).SetTo(bmpPtr))
			{
				ImageUtil_ColorReplace32A2(bmpPtr.Ptr(), dimg->GetWidth(), dimg->GetHeight(), this->fontColor);
				dimg->GetImgBitsEnd(true);
			}
			dimg->SetAlphaType(Media::AT_ALPHA);
	/*
			UInt8 *imgPtr = (UInt8*)((Media::GDIImage*)this->dimg)->bmpBits;
			Int32 c = this->fontColor;
			IntOS w = this->dimg->GetWidth();
			IntOS h = this->dimg->GetHeight();
			IntOS pxCnt;
			pxCnt = w * h;
	#if defined(HAS_ASM32)
			_asm
			{
				mov esi,imgPtr
				pxor xmm1,xmm1

				mov ecx,pxCnt
				movd xmm2,c
				punpcklbw xmm2,xmm1
				align 16
	ubgilop2a:
				mov eax,dword ptr [esi]
				test eax,eax
				jz ubgilop2b
				movd xmm0,eax
				punpcklwd xmm0,xmm0
				punpcklbw xmm0,xmm0
				psrlw xmm0,1
				movdqa xmm3,xmm0
				pmullw xmm0,xmm2
				pmulhw xmm3,xmm2
				punpcklwd xmm0,xmm3
				psrld xmm0,15
				packssdw xmm0,xmm1
				packuswb xmm0,xmm1
				movd [esi],xmm0

				align 16
	ubgilop2b:
				lea esi,[esi+4]
				dec ecx
				jnz ubgilop2a
			}
	#elif defined(HAS_ASM64)
			_asm
			{
				mov rsi,imgPtr
				pxor xmm1,xmm1

				mov rcx,pxCnt
				movd xmm2,c
				punpcklbw xmm2,xmm1
				align 16
	ubgilop2a:
				mov eax,dword ptr [rsi]
				test eax,eax
				jz ubgilop2b
				movd xmm0,eax
				punpcklwd xmm0,xmm0
				punpcklbw xmm0,xmm0
				psrlw xmm0,1
				movdqa xmm3,xmm0
				pmullw xmm0,xmm2
				pmulhw xmm3,xmm2
				punpcklwd xmm0,xmm3
				psrld xmm0,15
				packssdw xmm0,xmm1
				packuswb xmm0,xmm1
				movd [rsi],xmm0

				align 16
	ubgilop2b:
				lea rsi,[rsi+4]
				dec rcx
				jnz ubgilop2a
			}
	#elif defined(HAS_GCCASM64)
			asm(
			"	mov imgPtr,%rsi\n"
			"	pxor %xmm1,%xmm1\n"

			"	mov pxCnt,%rcx\n"
			"	movd c,%xmm2\n"
			"	punpcklbw %xmm1,%xmm2\n"
			"	.balign 16\n"
			"ubgilop2a:\n"
			"	movl (%rsi),%eax\n"
			"	test %eax,%eax\n"
			"	jz ubgilop2b\n"
			"	movd %eax,%xmm0\n"
			"	punpcklwd %xmm0,%xmm0\n"
			"	punpcklbw %xmm0,%xmm0\n"
			"	psrlw $1,%xmm0\n"
			"	movdqa %xmm0,%xmm3\n"
			"	pmullw %xmm2,%xmm0\n"
			"	pmulhw %xmm2,%xmm3\n"
			"	punpcklwd %xmm3,%xmm0\n"
			"	psrld $15,%xmm0\n"
			"	packssdw %xmm1,%xmm0\n"
			"	packuswb %xmm1,%xmm0\n"
			"	movd %xmm0,(%rsi)\n"

			"	.balign 16\n"
			"ubgilop2b:\n"
			"	lea 4(%rsi),%rsi\n"
			"	dec %rcx\n"
			"	jnz ubgilop2a\n"
			);
	#else
			_asm
			{
				mov esi,imgPtr
				pxor xmm1,xmm1

				mov ecx,pxCnt
				movd xmm2,c
				punpcklbw xmm2,xmm1
				align 16
	ubgilop2a:
				mov eax,dword ptr [esi]
				test eax,eax
				jz ubgilop2b
				movd xmm0,eax
				punpcklwd xmm0,xmm0
				punpcklbw xmm0,xmm0
				psrlw xmm0,1
				movdqa xmm3,xmm0
				pmullw xmm0,xmm2
				pmulhw xmm3,xmm2
				punpcklwd xmm0,xmm3
				psrld xmm0,15
				packssdw xmm0,xmm1
				packuswb xmm0,xmm1
				movd [esi],xmm0

				align 16
	ubgilop2b:
				lea esi,[esi+4]
				dec ecx
				jnz ubgilop2a
			}
	#endif
			*/
		}
	}
}

UI::DObj::RollingTextDObj::RollingTextDObj(NN<Media::DrawEngine> deng, Text::CString txt, Text::CString fontName, Double fontSize, UInt32 fontColor, UInt32 codePage, Math::Coord2D<IntOS> tl, Math::Size2D<UIntOS> size, Double rollSpeed) : DirectObject(tl)
{
	this->deng = deng;
	this->txt = Text::String::NewOrNull(txt);
	Text::CStringNN nnfontName;
	if (fontName.SetTo(nnfontName) && nnfontName.leng > 0)
	{
		this->fontName = Text::String::New(nnfontName);
	}
	else
	{
		this->fontName = Text::String::New(UTF8STRC("Arial"));
	}
	this->fontSize = fontSize;
	this->fontColor = fontColor;
	this->lineHeight = fontSize * 1.5;
	this->size = size;
	this->codePage = codePage;
	this->rollSpeed = rollSpeed;
	this->dimg = nullptr;
	this->lastRollPos = -1;
	this->startTime.SetCurrTimeUTC();

	this->UpdateBGImg();
}

UI::DObj::RollingTextDObj::~RollingTextDObj()
{
	OPTSTR_DEL(this->txt);
	this->fontName->Release();

	NN<Media::DrawImage> img;
	if (this->dimg.SetTo(img))
	{
		this->deng->DeleteImage(img);
		this->dimg = nullptr;
	}
}

Bool UI::DObj::RollingTextDObj::IsChanged()
{
	NN<Media::DrawImage> dimg;
	if (!this->dimg.SetTo(dimg))
		return false;
	if (dimg->GetHeight() <= this->size.y)
		return false;
	Data::DateTime currTime;
	currTime.SetCurrTimeUTC();
	Int64 t = currTime.DiffMS(this->startTime);
	UIntOS h = dimg->GetHeight();
	IntOS currPos = Double2Int32(Int64_Double(t) * this->rollSpeed * 0.001);
	while (currPos >= (IntOS)h)
	{
		currPos -= (IntOS)h;
		this->startTime.AddMS(Double2Int32(UIntOS2Double(h) / this->rollSpeed * 1000.0));
	}
	if (currPos != this->lastRollPos)
	{
		return true;
	}
	return false;
}

Bool UI::DObj::RollingTextDObj::DoEvents()
{
	return false;
}

void UI::DObj::RollingTextDObj::DrawObject(NN<Media::DrawImage> dimg)
{
	Math::Coord2D<IntOS> tl = this->GetCurrPos();
	NN<Media::DrawImage> img;
	if (!this->dimg.SetTo(img))
	{
		return;
	}
	UIntOS h = img->GetHeight();
	if (h <= this->size.y)
	{
		dimg->DrawImagePt(img, tl.ToDouble());
	}
	else
	{
		Data::DateTime currTime;
		currTime.SetCurrTimeUTC();
		Int64 t = currTime.DiffMS(this->startTime);
		IntOS currPos = Double2Int32(Int64_Double(t) * this->rollSpeed * 0.001);
		while (currPos >= (IntOS)h)
		{
			currPos -= (IntOS)h;
			this->startTime.AddMS(Double2Int32(UIntOS2Double(h) / this->rollSpeed * 1000.0));
		}
		this->lastRollPos = currPos;

		if ((h - (UIntOS)currPos) >= this->size.y)
		{
			dimg->DrawImagePt2(img, tl.ToDouble(), Math::Coord2DDbl(0, IntOS2Double(currPos)), this->size.ToDouble());
		}
		else
		{
			dimg->DrawImagePt2(img, tl.ToDouble(), Math::Coord2DDbl(0, IntOS2Double(currPos)), Math::Size2DDbl(IntOS2Double(this->size.x), IntOS2Double((IntOS)h - currPos)));
			dimg->DrawImagePt2(img, Math::Coord2DDbl(IntOS2Double(tl.x), IntOS2Double(tl.y + (IntOS)h - currPos)), Math::Coord2DDbl(0, 0), Math::Size2DDbl(UIntOS2Double(this->size.x), IntOS2Double(this->size.y - h + (UIntOS)currPos)));
		}
	}
}

Bool UI::DObj::RollingTextDObj::IsObject(Math::Coord2D<IntOS> scnPos)
{
	return false;
}

void UI::DObj::RollingTextDObj::OnMouseDown()
{
}

void UI::DObj::RollingTextDObj::OnMouseUp()
{
}

void UI::DObj::RollingTextDObj::OnMouseClick()
{
}
