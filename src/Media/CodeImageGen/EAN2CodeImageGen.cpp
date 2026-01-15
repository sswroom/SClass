#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math_C.h"
#include "Media/CodeImageGen/EAN2CodeImageGen.h"
#include "Text/MyString.h"

Media::CodeImageGen::EAN2CodeImageGen::EAN2CodeImageGen()
{
}

Media::CodeImageGen::EAN2CodeImageGen::~EAN2CodeImageGen()
{
}

Media::CodeImageGen::CodeImageGen::CodeType Media::CodeImageGen::EAN2CodeImageGen::GetCodeType()
{
	return Media::CodeImageGen::CodeImageGen::CT_EAN2;
}

UIntOS Media::CodeImageGen::EAN2CodeImageGen::GetMinLength()
{
	return 2;
}

UIntOS Media::CodeImageGen::EAN2CodeImageGen::GetMaxLength()
{
	return 2;
}

Optional<Media::DrawImage> Media::CodeImageGen::EAN2CodeImageGen::GenCode(Text::CStringNN code, UIntOS codeWidth, NN<Media::DrawEngine> eng)
{
	UTF8Char sbuff[2];
	UIntOS i = 2;
	UIntOS j;
	UIntOS k;
	UnsafeArray<const UTF8Char> tmpStr = code.v;
	UTF8Char c;
	while (i-- > 0)
	{
		c = *tmpStr++;
		if (c < '0' || c > '9')
			return nullptr;
	}
	if (*tmpStr != 0)
		return nullptr;
	j = Text::StrToUInt32(code.v);

	UnsafeArray<const UTF8Char> codePtr = code.v;
	UInt8 bitCode[21];
	bitCode[0] = 0;
	bitCode[1] = 1;
	bitCode[2] = 0;
	bitCode[3] = 1;
	bitCode[4] = 1;
	switch (j & 3)
	{
	case 0:
		tmpStr = (const UTF8Char*)"LL";
		break;
	case 1:
		tmpStr = (const UTF8Char*)"LG";
		break;
	case 2:
		tmpStr = (const UTF8Char*)"GL";
		break;
	case 3:
		tmpStr = (const UTF8Char*)"GG";
		break;
	}
	j = 5;
	i = 2;
	while (i-- > 0)
	{
		if (*tmpStr++ == 'L') //L-code
		{
			switch (*codePtr++)
			{
			case '0':
				bitCode[j + 0] = 0;
				bitCode[j + 1] = 0;
				bitCode[j + 2] = 0;
				bitCode[j + 3] = 1;
				bitCode[j + 4] = 1;
				bitCode[j + 5] = 0;
				bitCode[j + 6] = 1;
				break;
			case '1':
				bitCode[j + 0] = 0;
				bitCode[j + 1] = 0;
				bitCode[j + 2] = 1;
				bitCode[j + 3] = 1;
				bitCode[j + 4] = 0;
				bitCode[j + 5] = 0;
				bitCode[j + 6] = 1;
				break;
			case '2':
				bitCode[j + 0] = 0;
				bitCode[j + 1] = 0;
				bitCode[j + 2] = 1;
				bitCode[j + 3] = 0;
				bitCode[j + 4] = 0;
				bitCode[j + 5] = 1;
				bitCode[j + 6] = 1;
				break;
			case '3':
				bitCode[j + 0] = 0;
				bitCode[j + 1] = 1;
				bitCode[j + 2] = 1;
				bitCode[j + 3] = 1;
				bitCode[j + 4] = 1;
				bitCode[j + 5] = 0;
				bitCode[j + 6] = 1;
				break;
			case '4':
				bitCode[j + 0] = 0;
				bitCode[j + 1] = 1;
				bitCode[j + 2] = 0;
				bitCode[j + 3] = 0;
				bitCode[j + 4] = 0;
				bitCode[j + 5] = 1;
				bitCode[j + 6] = 1;
				break;
			case '5':
				bitCode[j + 0] = 0;
				bitCode[j + 1] = 1;
				bitCode[j + 2] = 1;
				bitCode[j + 3] = 0;
				bitCode[j + 4] = 0;
				bitCode[j + 5] = 0;
				bitCode[j + 6] = 1;
				break;
			case '6':
				bitCode[j + 0] = 0;
				bitCode[j + 1] = 1;
				bitCode[j + 2] = 0;
				bitCode[j + 3] = 1;
				bitCode[j + 4] = 1;
				bitCode[j + 5] = 1;
				bitCode[j + 6] = 1;
				break;
			case '7':
				bitCode[j + 0] = 0;
				bitCode[j + 1] = 1;
				bitCode[j + 2] = 1;
				bitCode[j + 3] = 1;
				bitCode[j + 4] = 0;
				bitCode[j + 5] = 1;
				bitCode[j + 6] = 1;
				break;
			case '8':
				bitCode[j + 0] = 0;
				bitCode[j + 1] = 1;
				bitCode[j + 2] = 1;
				bitCode[j + 3] = 0;
				bitCode[j + 4] = 1;
				bitCode[j + 5] = 1;
				bitCode[j + 6] = 1;
				break;
			case '9':
				bitCode[j + 0] = 0;
				bitCode[j + 1] = 0;
				bitCode[j + 2] = 0;
				bitCode[j + 3] = 1;
				bitCode[j + 4] = 0;
				bitCode[j + 5] = 1;
				bitCode[j + 6] = 1;
				break;
			}
			j += 7;
		}
		else //G-code
		{
			switch (*codePtr++)
			{
			case '0':
				bitCode[j + 0] = 0;
				bitCode[j + 1] = 1;
				bitCode[j + 2] = 0;
				bitCode[j + 3] = 0;
				bitCode[j + 4] = 1;
				bitCode[j + 5] = 1;
				bitCode[j + 6] = 1;
				break;
			case '1':
				bitCode[j + 0] = 0;
				bitCode[j + 1] = 1;
				bitCode[j + 2] = 1;
				bitCode[j + 3] = 0;
				bitCode[j + 4] = 0;
				bitCode[j + 5] = 1;
				bitCode[j + 6] = 1;
				break;
			case '2':
				bitCode[j + 0] = 0;
				bitCode[j + 1] = 0;
				bitCode[j + 2] = 1;
				bitCode[j + 3] = 1;
				bitCode[j + 4] = 0;
				bitCode[j + 5] = 1;
				bitCode[j + 6] = 1;
				break;
			case '3':
				bitCode[j + 0] = 0;
				bitCode[j + 1] = 1;
				bitCode[j + 2] = 0;
				bitCode[j + 3] = 0;
				bitCode[j + 4] = 0;
				bitCode[j + 5] = 0;
				bitCode[j + 6] = 1;
				break;
			case '4':
				bitCode[j + 0] = 0;
				bitCode[j + 1] = 0;
				bitCode[j + 2] = 1;
				bitCode[j + 3] = 1;
				bitCode[j + 4] = 1;
				bitCode[j + 5] = 0;
				bitCode[j + 6] = 1;
				break;
			case '5':
				bitCode[j + 0] = 0;
				bitCode[j + 1] = 1;
				bitCode[j + 2] = 1;
				bitCode[j + 3] = 1;
				bitCode[j + 4] = 0;
				bitCode[j + 5] = 0;
				bitCode[j + 6] = 1;
				break;
			case '6':
				bitCode[j + 0] = 0;
				bitCode[j + 1] = 0;
				bitCode[j + 2] = 0;
				bitCode[j + 3] = 0;
				bitCode[j + 4] = 1;
				bitCode[j + 5] = 0;
				bitCode[j + 6] = 1;
				break;
			case '7':
				bitCode[j + 0] = 0;
				bitCode[j + 1] = 0;
				bitCode[j + 2] = 1;
				bitCode[j + 3] = 0;
				bitCode[j + 4] = 0;
				bitCode[j + 5] = 0;
				bitCode[j + 6] = 1;
				break;
			case '8':
				bitCode[j + 0] = 0;
				bitCode[j + 1] = 0;
				bitCode[j + 2] = 0;
				bitCode[j + 3] = 1;
				bitCode[j + 4] = 0;
				bitCode[j + 5] = 0;
				bitCode[j + 6] = 1;
				break;
			case '9':
				bitCode[j + 0] = 0;
				bitCode[j + 1] = 0;
				bitCode[j + 2] = 1;
				bitCode[j + 3] = 0;
				bitCode[j + 4] = 1;
				bitCode[j + 5] = 1;
				bitCode[j + 6] = 1;
				break;
			}
			j += 7;
		}
		if (i > 0)
		{
			bitCode[j + 0] = 0;
			bitCode[j + 1] = 1;
			j += 2;
		}
		else
		{
			break;
		}
	}
	codePtr = codePtr - 2;

	UIntOS h = codeWidth * 70;
	UIntOS y = h - codeWidth;
	Double fh = 12.0 * UIntOS2Double(codeWidth);

	NN<Media::DrawImage> dimg;
	if (!eng->CreateImage32(Math::Size2D<UIntOS>((4 + 21) * codeWidth, h), Media::AT_ALPHA_ALL_FF).SetTo(dimg))
		return nullptr;
	NN<Media::DrawBrush> b;
	NN<Media::DrawPen> p;
	NN<Media::DrawFont> f;
	b = dimg->NewBrushARGB(0xffffffff);
	dimg->DrawRect(Math::Coord2DDbl(0, 0), dimg->GetSize().ToDouble(), nullptr, b);
	dimg->DelBrush(b);

	p = dimg->NewPenARGB(0xff000000, 1, nullptr, 0);
	i = 0;
	j = 2 * codeWidth;
	while (i < 21)
	{
		if (bitCode[i])
		{
			k = codeWidth;
			while (k-- > 0)
			{
				dimg->DrawLine((Double)j, (Double)codeWidth + fh, (Double)j, (Double)y, p);
				j++;
			}
		}
		else
		{
			j += codeWidth;
		}
		i++;
	}
	dimg->DelPen(p);

	f = dimg->NewFontPx(CSTR("Arial"), fh, Media::DrawEngine::DFS_NORMAL, 0);
	b = dimg->NewBrushARGB(0xff000000);
	sbuff[0] = *codePtr++;
	sbuff[1] = 0;
	dimg->DrawString(Math::Coord2DDbl((Double)(2 + 5) * UIntOS2Double(codeWidth), (Double)codeWidth), {sbuff, 1}, f, b);
	sbuff[0] = *codePtr++;
	dimg->DrawString(Math::Coord2DDbl((Double)(2 + 5 + 7 + 2) * UIntOS2Double(codeWidth), (Double)codeWidth), {sbuff, 1}, f, b);

	dimg->DelBrush(b);
	dimg->DelFont(f);

	return dimg;
}
