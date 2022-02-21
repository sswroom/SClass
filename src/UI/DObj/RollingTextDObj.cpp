#include "Stdafx.h"
#include "Math/Math.h"
#include "Media/DrawImageTool.h"
#include "Media/ImageUtil.h"
#include "Text/StringBuilder.h"
#include "UI/DObj/RollingTextDObj.h"

void UI::DObj::RollingTextDObj::UpdateBGImg()
{
	if (this->dimg)
	{
		this->deng->DeleteImage(this->dimg);
		this->dimg = 0;
	}

	if (this->txt)
	{
		Media::DrawImage *dimg = this->deng->CreateImage32(this->width, this->height, Media::AT_NO_ALPHA);
		Media::DrawFont *f = dimg->NewFontPx(this->fontName->v, this->fontName->leng, this->fontSize, Media::DrawEngine::DFS_ANTIALIAS, this->codePage);
		Media::DrawBrush *b;
		Data::ArrayList<const UTF8Char *> lines;
		const UTF8Char *s;
		UOSInt i;
		UOSInt j;
		Double currY;

		Media::DrawImageTool::SplitString(dimg, this->txt->v, &lines, f, OSInt2Double(this->width));
		dimg->DelFont(f);
		this->deng->DeleteImage(dimg);

		this->dimg = this->deng->CreateImage32(this->width, (UInt32)Double2Int32(this->lineHeight * UOSInt2Double(lines.GetCount())), Media::AT_NO_ALPHA);
		f = this->dimg->NewFontPx(this->fontName->v, this->fontName->leng, this->fontSize, Media::DrawEngine::DFS_ANTIALIAS, this->codePage);
		b = this->dimg->NewBrushARGB(0xffffffff);
		currY = 0;
		i = 0;
		j = lines.GetCount();
		while (i < j)
		{
			s = lines.GetItem(i);
			this->dimg->DrawString(0, currY, s, f, b);
			Text::StrDelNew(s);
			currY += this->lineHeight;
			i++;
		}
		this->dimg->DelBrush(b);
		this->dimg->DelFont(f);

		Bool revOrder;
		UInt8 *bmpPtr = this->dimg->GetImgBits(&revOrder);
		ImageUtil_ColorReplace32A2(bmpPtr, this->dimg->GetWidth(), this->dimg->GetHeight(), this->fontColor);
		this->dimg->GetImgBitsEnd(true);
		this->dimg->SetAlphaType(Media::AT_ALPHA);
/*
		UInt8 *imgPtr = (UInt8*)((Media::GDIImage*)this->dimg)->bmpBits;
		Int32 c = this->fontColor;
		OSInt w = this->dimg->GetWidth();
		OSInt h = this->dimg->GetHeight();
		OSInt pxCnt;
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

UI::DObj::RollingTextDObj::RollingTextDObj(Media::DrawEngine *deng, Text::CString txt, Text::CString fontName, Double fontSize, UInt32 fontColor, OSInt left, OSInt top, UOSInt width, UOSInt height, UInt32 codePage, Double rollSpeed) : DirectObject(left, top)
{
	this->deng = deng;
	if (txt.leng > 0)
	{
		this->txt = Text::String::New(txt);
	}
	else
	{
		this->txt = 0;
	}
	if (fontName.leng > 0)
	{
		this->fontName = Text::String::New(fontName);
	}
	else
	{
		this->fontName = Text::String::New(UTF8STRC("Arial"));
	}
	this->fontSize = fontSize;
	this->fontColor = fontColor;
	this->lineHeight = fontSize * 1.5;
	this->width = width;
	this->height = height;
	this->codePage = codePage;
	this->rollSpeed = rollSpeed;
	this->dimg = 0;
	this->lastRollPos = -1;
	NEW_CLASS(this->startTime, Data::DateTime());
	this->startTime->SetCurrTimeUTC();

	this->UpdateBGImg();
}

UI::DObj::RollingTextDObj::~RollingTextDObj()
{
	SDEL_STRING(this->txt);
	this->fontName->Release();
	DEL_CLASS(this->startTime);

	if (this->dimg)
	{
		this->deng->DeleteImage(this->dimg);
		this->dimg = 0;
	}
}

Bool UI::DObj::RollingTextDObj::IsChanged()
{
	if (this->dimg == 0)
		return false;
	if (this->dimg->GetHeight() <= this->height)
		return false;
	Data::DateTime currTime;
	currTime.SetCurrTimeUTC();
	Int64 t = currTime.DiffMS(this->startTime);
	UOSInt h = this->dimg->GetHeight();
	OSInt currPos = Double2Int32(Int64_Double(t) * this->rollSpeed * 0.001);
	while (currPos >= (OSInt)h)
	{
		currPos -= (OSInt)h;
		this->startTime->AddMS(Double2Int32(UOSInt2Double(h) / this->rollSpeed * 1000.0));
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

void UI::DObj::RollingTextDObj::DrawObject(Media::DrawImage *dimg)
{
	OSInt left;
	OSInt top;
	this->GetCurrPos(&left, &top);
	if (this->dimg == 0)
	{
		return;
	}
	UOSInt h = this->dimg->GetHeight();
	if (h <= this->height)
	{
		dimg->DrawImagePt(this->dimg, OSInt2Double(left), OSInt2Double(top));
	}
	else
	{
		Data::DateTime currTime;
		currTime.SetCurrTimeUTC();
		Int64 t = currTime.DiffMS(this->startTime);
		OSInt currPos = Double2Int32(Int64_Double(t) * this->rollSpeed * 0.001);
		while (currPos >= (OSInt)h)
		{
			currPos -= (OSInt)h;
			this->startTime->AddMS(Double2Int32(UOSInt2Double(h) / this->rollSpeed * 1000.0));
		}
		this->lastRollPos = currPos;

		if ((h - (UOSInt)currPos) >= this->height)
		{
			dimg->DrawImagePt3(this->dimg, OSInt2Double(left), OSInt2Double(top), 0, OSInt2Double(currPos), OSInt2Double(this->width), OSInt2Double(this->height));
		}
		else
		{
			dimg->DrawImagePt3(this->dimg, OSInt2Double(left), OSInt2Double(top), 0, OSInt2Double(currPos), OSInt2Double(this->width), OSInt2Double((OSInt)h - currPos));
			dimg->DrawImagePt3(this->dimg, OSInt2Double(left), OSInt2Double(top + (OSInt)h - currPos), 0, 0, UOSInt2Double(this->width), OSInt2Double(this->height - h + (UOSInt)currPos));
		}
	}
}

Bool UI::DObj::RollingTextDObj::IsObject(OSInt x, OSInt y)
{
	return false;
}

/*System::Windows::Forms::Cursor ^UI::DObj::TextDObj::GetCursor()
{
	return System::Windows::Forms::Cursors::Arrow;
}*/

void UI::DObj::RollingTextDObj::OnMouseDown()
{
}

void UI::DObj::RollingTextDObj::OnMouseUp()
{
}

void UI::DObj::RollingTextDObj::OnMouseClick()
{
}
