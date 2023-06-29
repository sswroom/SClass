#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Media/CodeImageGen/EAN13CodeImageGen.h"
#include "Text/MyString.h"

Media::CodeImageGen::EAN13CodeImageGen::EAN13CodeImageGen()
{
}

Media::CodeImageGen::EAN13CodeImageGen::~EAN13CodeImageGen()
{
}

Media::CodeImageGen::CodeImageGen::CodeType Media::CodeImageGen::EAN13CodeImageGen::GetCodeType()
{
	return Media::CodeImageGen::CodeImageGen::CT_EAN13;
}

UOSInt Media::CodeImageGen::EAN13CodeImageGen::GetMinLength()
{
	return 13;
}

UOSInt Media::CodeImageGen::EAN13CodeImageGen::GetMaxLength()
{
	return 13;
}

Media::DrawImage *Media::CodeImageGen::EAN13CodeImageGen::GenCode(Text::CString code, UOSInt codeWidth, Media::DrawEngine *eng)
{
	UTF8Char sbuff[2];
	if (code.v == 0)
		return 0;

	UOSInt i = 13;
	UOSInt j = 0;
	UOSInt k;
	const UTF8Char *tmpStr = code.v;
	WChar c;
	while (i-- > 0)
	{
		c = *tmpStr++;
		if (c < '0' || c > '9')
			return 0;
		if (i & 1)
		{
			j += (UOSInt)(c - '0') * 3;
		}
		else
		{
			j += (UOSInt)(c - '0');
		}
	}
	if (*tmpStr != 0)
		return 0;
	if ((j % 10) != 0)
		return 0;

	const UTF8Char *codePtr = code.v;
	UInt8 bitCode[95];
	bitCode[0] = 1;
	bitCode[1] = 0;
	bitCode[2] = 1;
	j = 3;
	switch (*codePtr++)
	{
	case '0':
		tmpStr = (const UTF8Char*)"LLLLLL";
		break;
	case '1':
		tmpStr = (const UTF8Char*)"LLGLGG";
		break;
	case '2':
		tmpStr = (const UTF8Char*)"LLGGLG";
		break;
	case '3':
		tmpStr = (const UTF8Char*)"LLGGGL";
		break;
	case '4':
		tmpStr = (const UTF8Char*)"LGLLGG";
		break;
	case '5':
		tmpStr = (const UTF8Char*)"LGGLLG";
		break;
	case '6':
		tmpStr = (const UTF8Char*)"LGGGLL";
		break;
	case '7':
		tmpStr = (const UTF8Char*)"LGLGLG";
		break;
	case '8':
		tmpStr = (const UTF8Char*)"LGLGGL";
		break;
	case '9':
		tmpStr = (const UTF8Char*)"LGGLGL";
		break;
	}
	i = 6;
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
	}
	bitCode[j + 0] = 0;
	bitCode[j + 1] = 1;
	bitCode[j + 2] = 0;
	bitCode[j + 3] = 1;
	bitCode[j + 4] = 0;
	j += 5;
	i = 6;
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
	codePtr = codePtr - 13;

	UOSInt h = codeWidth * 70;
	UOSInt y = h - codeWidth;
	Double y2;
	Double fh = 12.0 * UOSInt2Double(codeWidth);

	Media::DrawImage *dimg = eng->CreateImage32(Math::Size2D<UOSInt>((9 + 11 + 95) * codeWidth, h), Media::AT_NO_ALPHA);
	Media::DrawBrush *b;
	Media::DrawPen *p;
	Media::DrawFont *f;
	b = dimg->NewBrushARGB(0xffffffff);
	dimg->DrawRect(Math::Coord2DDbl(0, 0), dimg->GetSize().ToDouble(), 0, b);
	dimg->DelBrush(b);

	p = dimg->NewPenARGB(0xff000000, 1, 0, 0);
	i = 0;
	j = (9 + 2) * codeWidth;
	while (i < 95)
	{
		if (bitCode[i])
		{
			switch (i)
			{
			case 0:
			case 2:
			case 46:
			case 48:
			case 92:
			case 94:
				y2 = UOSInt2Double(y) - fh * 0.5;
				break;
			default:
				y2 = UOSInt2Double(y) - fh;
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
	sbuff[0] = *codePtr++;
	sbuff[1] = 0;
	dimg->DrawString(Math::Coord2DDbl((Double)codeWidth, UOSInt2Double(y) - fh), {sbuff, 1}, f, b);
	i = codeWidth * (5 + 9);
	j = 6;
	while (j-- > 0)
	{
		sbuff[0] = *codePtr++;
		dimg->DrawString(Math::Coord2DDbl((Double)i, UOSInt2Double(y) - fh), {sbuff, 1}, f, b);
		i += 7 * codeWidth;
	}
	i += 5 * codeWidth;
	j = 6;
	while (j-- > 0)
	{
		sbuff[0] = *codePtr++;
		dimg->DrawString(Math::Coord2DDbl((Double)i, UOSInt2Double(y) - fh), {sbuff, 1}, f, b);
		i += 7 * codeWidth;
	}
	dimg->DelBrush(b);
	dimg->DelFont(f);

	return dimg;
}
