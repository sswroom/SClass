#include "Stdafx.h"
#include "Core/ByteTool_C.h"
#include "Media/MSFontRenderer.h"
#include "Media/StaticImage.h"

Media::MSFontRenderer::MSFontRenderer(NN<Text::String> sourceName, UnsafeArray<const UInt8> fontBuff, UIntOS buffSize) : Media::FontRenderer(sourceName)
{
	this->fontBuff = 0;
	UIntOS ver = ReadUInt16(&fontBuff[0]);
	UIntOS fontSize = ReadUInt16(&fontBuff[2]);
	UIntOS hdrSize;
	if (ver == 0x200)
	{
		hdrSize = 118;
	}
	else if (ver == 0x300)
	{
		hdrSize = 148;
	}
	else
	{
		return;
	}
	if (buffSize >= fontSize && hdrSize < fontSize)
	{
		this->fontBuff = MemAlloc(UInt8, fontSize);
		MemCopyNO(this->fontBuff, fontBuff.Ptr(), fontSize);
	}
}

Media::MSFontRenderer::~MSFontRenderer()
{
	if (this->fontBuff)
	{
		MemFree(this->fontBuff);
		this->fontBuff = 0;
	}
}

Bool Media::MSFontRenderer::IsError()
{
	return this->fontBuff == 0;
}

UTF32Char Media::MSFontRenderer::GetMinChar() const
{
	return this->fontBuff[95];
}

UTF32Char Media::MSFontRenderer::GetMaxChar() const
{
	return this->fontBuff[96];
}

Optional<Media::StaticImage> Media::MSFontRenderer::CreateImage(UTF32Char charCode, Math::Size2D<UIntOS> targetSize) const
{
	UInt32 ver = ReadUInt16(&this->fontBuff[0]);
	UIntOS i;
	UIntOS ofst;
	UIntOS fntW;
	UIntOS fntH = ReadUInt16(&this->fontBuff[88]);
	if (charCode < this->fontBuff[95] || charCode > this->fontBuff[96])
	{
		charCode = this->fontBuff[97];
	}
	i = (UIntOS)charCode - (UIntOS)this->fontBuff[95];
	if (ver == 0x200)
	{
		i = 118 + i * 4;
		fntW = ReadUInt16(&this->fontBuff[i]);
		ofst = ReadUInt16(&this->fontBuff[i + 2]);
	}
	else if (ver == 0x300)
	{
		i = 148 + i * 6;
		fntW = ReadUInt16(&this->fontBuff[i]);
		ofst = ReadUInt32(&this->fontBuff[i + 2]);
	}
	else
	{
		return nullptr;
	}
	UIntOS imgSize = fntH * ((fntW + 7) >> 3);
	NN<Media::StaticImage> simg;
	Media::ColorProfile color(Media::ColorProfile::CPT_PUNKNOWN);
	UnsafeArray<UInt8> pal;
	NEW_CLASSNN(simg, Media::StaticImage(Math::Size2D<UIntOS>(fntW, fntH), 0, 1, Media::PF_PAL_W1, imgSize, color, Media::ColorProfile::YUVT_UNKNOWN, Media::AT_PREMUL_ALPHA, Media::YCOFST_C_CENTER_LEFT));
	simg->info.hdpi = ReadUInt16(&this->fontBuff[72]);
	simg->info.vdpi = ReadUInt16(&this->fontBuff[70]);
	if (simg->pal.SetTo(pal))
	{
		pal[0] = 0;
		pal[1] = 0;
		pal[2] = 0;
		pal[3] = 0xff;
		pal[4] = 0xff;
		pal[5] = 0xff;
		pal[6] = 0xff;
		pal[7] = 0;
	}
	UInt8 *srcPtr;
	UnsafeArray<UInt8> destPtr;
	UInt8 *tmpPtr;
	UIntOS j;
	UIntOS lineSize = (fntW + 7) >> 3;
	srcPtr = &this->fontBuff[ofst];
	destPtr = simg->data;

	i = fntH;
	while (i-- > 0)
	{
		tmpPtr = srcPtr;
		j = lineSize;
		while (j-- > 0)
		{
			*destPtr++ = (UInt8)~*tmpPtr;
			tmpPtr += fntH;
		}
		srcPtr++;
	}
	return simg;
}
