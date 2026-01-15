#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math_C.h"
#include "Media/CodeImageGen/EAN8CodeImageGen.h"
#include "Text/MyString.h"

Media::CodeImageGen::EAN8CodeImageGen::EAN8CodeImageGen()
{
}

Media::CodeImageGen::EAN8CodeImageGen::~EAN8CodeImageGen()
{
}

Media::CodeImageGen::CodeImageGen::CodeType Media::CodeImageGen::EAN8CodeImageGen::GetCodeType()
{
	return Media::CodeImageGen::CodeImageGen::CT_EAN8;
}

UIntOS Media::CodeImageGen::EAN8CodeImageGen::GetMinLength()
{
	return 8;
}

UIntOS Media::CodeImageGen::EAN8CodeImageGen::GetMaxLength()
{
	return 8;
}

Optional<Media::DrawImage> Media::CodeImageGen::EAN8CodeImageGen::GenCode(Text::CStringNN code, UIntOS codeWidth, NN<Media::DrawEngine> eng)
{
	UTF8Char sbuff[2];
	UIntOS i = 8;
	UIntOS j = 0;
	UIntOS k;
	UnsafeArray<const UTF8Char> tmpStr = code.v;
	UTF8Char c;
	while (i-- > 0)
	{
		c = *tmpStr++;
		if (c < '0' || c > '9')
			return nullptr;
		if (i & 1)
		{
			j += (UIntOS)(c - '0') * 3;
		}
		else
		{
			j += (UIntOS)(c - '0');
		}
	}
	if (*tmpStr != 0)
		return nullptr;
	if ((j % 10) != 0)
		return nullptr;

	UnsafeArray<const UTF8Char> codePtr = code.v;
	UInt8 bitCode[67];
	bitCode[0] = 1;
	bitCode[1] = 0;
	bitCode[2] = 1;
	j = 3;
	i = 4;
	while (i-- > 0)
	{
		switch (*codePtr++) //L-code
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
	bitCode[j + 0] = 0;
	bitCode[j + 1] = 1;
	bitCode[j + 2] = 0;
	bitCode[j + 3] = 1;
	bitCode[j + 4] = 0;
	j += 5;
	i = 4;
	while (i-- > 0)
	{
		switch (*codePtr++) //R-code
		{
		case '0':
			bitCode[j + 0] = 1;
			bitCode[j + 1] = 1;
			bitCode[j + 2] = 1;
			bitCode[j + 3] = 0;
			bitCode[j + 4] = 0;
			bitCode[j + 5] = 1;
			bitCode[j + 6] = 0;
			break;
		case '1':
			bitCode[j + 0] = 1;
			bitCode[j + 1] = 1;
			bitCode[j + 2] = 0;
			bitCode[j + 3] = 0;
			bitCode[j + 4] = 1;
			bitCode[j + 5] = 1;
			bitCode[j + 6] = 0;
			break;
		case '2':
			bitCode[j + 0] = 1;
			bitCode[j + 1] = 1;
			bitCode[j + 2] = 0;
			bitCode[j + 3] = 1;
			bitCode[j + 4] = 1;
			bitCode[j + 5] = 0;
			bitCode[j + 6] = 0;
			break;
		case '3':
			bitCode[j + 0] = 1;
			bitCode[j + 1] = 0;
			bitCode[j + 2] = 0;
			bitCode[j + 3] = 0;
			bitCode[j + 4] = 0;
			bitCode[j + 5] = 1;
			bitCode[j + 6] = 0;
			break;
		case '4':
			bitCode[j + 0] = 1;
			bitCode[j + 1] = 0;
			bitCode[j + 2] = 1;
			bitCode[j + 3] = 1;
			bitCode[j + 4] = 1;
			bitCode[j + 5] = 0;
			bitCode[j + 6] = 0;
			break;
		case '5':
			bitCode[j + 0] = 1;
			bitCode[j + 1] = 0;
			bitCode[j + 2] = 0;
			bitCode[j + 3] = 1;
			bitCode[j + 4] = 1;
			bitCode[j + 5] = 1;
			bitCode[j + 6] = 0;
			break;
		case '6':
			bitCode[j + 0] = 1;
			bitCode[j + 1] = 0;
			bitCode[j + 2] = 1;
			bitCode[j + 3] = 0;
			bitCode[j + 4] = 0;
			bitCode[j + 5] = 0;
			bitCode[j + 6] = 0;
			break;
		case '7':
			bitCode[j + 0] = 1;
			bitCode[j + 1] = 0;
			bitCode[j + 2] = 0;
			bitCode[j + 3] = 0;
			bitCode[j + 4] = 1;
			bitCode[j + 5] = 0;
			bitCode[j + 6] = 0;
			break;
		case '8':
			bitCode[j + 0] = 1;
			bitCode[j + 1] = 0;
			bitCode[j + 2] = 0;
			bitCode[j + 3] = 1;
			bitCode[j + 4] = 0;
			bitCode[j + 5] = 0;
			bitCode[j + 6] = 0;
			break;
		case '9':
			bitCode[j + 0] = 1;
			bitCode[j + 1] = 1;
			bitCode[j + 2] = 1;
			bitCode[j + 3] = 0;
			bitCode[j + 4] = 1;
			bitCode[j + 5] = 0;
			bitCode[j + 6] = 0;
			break;
		}
		j += 7;
	}
	bitCode[j + 0] = 1;
	bitCode[j + 1] = 0;
	bitCode[j + 2] = 1;
	j += 3;
	codePtr = codePtr - 8;

	UIntOS h = codeWidth * 70;
	UIntOS y = h - codeWidth;
	Double y2;
	Double fh = 12.0 * UIntOS2Double(codeWidth);

	NN<Media::DrawImage> dimg;
	if (!eng->CreateImage32(Math::Size2D<UIntOS>((4 + 67) * codeWidth, h), Media::AT_ALPHA_ALL_FF).SetTo(dimg))
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
	while (i < 67)
	{
		if (bitCode[i])
		{
			switch (i)
			{
			case 0:
			case 2:
			case 32:
			case 34:
			case 64:
			case 66:
				y2 = UIntOS2Double(y) - fh * 0.5;
				break;
			default:
				y2 = UIntOS2Double(y) - fh;
				break;
			}

			k = codeWidth;
			while (k-- > 0)
			{
				dimg->DrawLine((Double)j, (Double)codeWidth, (Double)j, y2, p);
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
	sbuff[1] = 0;
	i = codeWidth * 5;
	j = 4;
	while (j-- > 0)
	{
		sbuff[0] = *codePtr++;
		dimg->DrawString(Math::Coord2DDbl((Double)i, UIntOS2Double(y) - fh), {sbuff, 1}, f, b);
		i += 7 * codeWidth;
	}
	i += 5 * codeWidth;
	j = 4;
	while (j-- > 0)
	{
		sbuff[0] = *codePtr++;
		dimg->DrawString(Math::Coord2DDbl((Double)i, UIntOS2Double(y) - fh), {sbuff, 1}, f, b);
		i += 7 * codeWidth;
	}
	dimg->DelBrush(b);
	dimg->DelFont(f);

	return dimg;
}
