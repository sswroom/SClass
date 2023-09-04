#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Media/MSFontRenderer.h"
#include "Media/StaticImage.h"

Media::MSFontRenderer::MSFontRenderer(NotNullPtr<Text::String> sourceName, const UInt8 *fontBuff, UOSInt buffSize) : Media::FontRenderer(sourceName)
{
	this->fontBuff = 0;
	UOSInt ver = ReadUInt16(&fontBuff[0]);
	UOSInt fontSize = ReadUInt16(&fontBuff[2]);
	UOSInt hdrSize;
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
		MemCopyNO(this->fontBuff, fontBuff, fontSize);
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

Media::StaticImage *Media::MSFontRenderer::CreateImage(UTF32Char charCode, Math::Size2D<UOSInt> targetSize) const
{
	UInt32 ver = ReadUInt16(&this->fontBuff[0]);
	UOSInt i;
	UOSInt ofst;
	UOSInt fntW;
	UOSInt fntH = ReadUInt16(&this->fontBuff[88]);
	if (charCode < this->fontBuff[95] || charCode > this->fontBuff[96])
	{
		charCode = this->fontBuff[97];
	}
	i = (UOSInt)charCode - (UOSInt)this->fontBuff[95];
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
		return 0;
	}
	UOSInt imgSize = fntH * ((fntW + 7) >> 3);
	Media::StaticImage *simg;
	Media::ColorProfile color(Media::ColorProfile::CPT_PUNKNOWN);
	NEW_CLASS(simg, Media::StaticImage(Math::Size2D<UOSInt>(fntW, fntH), 0, 1, Media::PF_PAL_W1, imgSize, color, Media::ColorProfile::YUVT_UNKNOWN, Media::AT_PREMUL_ALPHA, Media::YCOFST_C_CENTER_LEFT));
	simg->info.hdpi = ReadUInt16(&this->fontBuff[72]);
	simg->info.vdpi = ReadUInt16(&this->fontBuff[70]);
	simg->pal[0] = 0;
	simg->pal[1] = 0;
	simg->pal[2] = 0;
	simg->pal[3] = 0xff;
	simg->pal[4] = 0xff;
	simg->pal[5] = 0xff;
	simg->pal[6] = 0xff;
	simg->pal[7] = 0;
	UInt8 *srcPtr;
	UInt8 *destPtr;
	UInt8 *tmpPtr;
	UOSInt j;
	UOSInt lineSize = (fntW + 7) >> 3;
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
