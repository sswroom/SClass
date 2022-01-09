#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
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

UOSInt Media::CodeImageGen::EAN8CodeImageGen::GetMinLength()
{
	return 8;
}

UOSInt Media::CodeImageGen::EAN8CodeImageGen::GetMaxLength()
{
	return 8;
}

Media::DrawImage *Media::CodeImageGen::EAN8CodeImageGen::GenCode(const UTF8Char *code, UOSInt codeWidth, Media::DrawEngine *eng)
{
	UTF8Char sbuff[2];
	if (code == 0)
		return 0;

	UOSInt i = 8;
	UOSInt j = 0;
	UOSInt k;
	const UTF8Char *tmpStr = code;
	UTF8Char c;
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

	UInt8 bitCode[67];
	bitCode[0] = 1;
	bitCode[1] = 0;
	bitCode[2] = 1;
	j = 3;
	i = 4;
	while (i-- > 0)
	{
		switch (*code++) //L-code
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
		switch (*code++) //R-code
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
	code = code - 8;

	UOSInt h = codeWidth * 70;
	UOSInt y = h - codeWidth;
	Double y2;
	Double fh = 12.0 * Math::UOSInt2Double(codeWidth);

	Media::DrawImage *dimg = eng->CreateImage32((4 + 67) * codeWidth, h, Media::AT_NO_ALPHA);
	Media::DrawBrush *b;
	Media::DrawPen *p;
	Media::DrawFont *f;
	b = dimg->NewBrushARGB(0xffffffff);
	dimg->DrawRect(0, 0, (Double)dimg->GetWidth(), (Double)dimg->GetHeight(), 0, b);
	dimg->DelBrush(b);

	p = dimg->NewPenARGB(0xff000000, 1, 0, 0);
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
				y2 = Math::UOSInt2Double(y) - fh * 0.5;
				break;
			default:
				y2 = Math::UOSInt2Double(y) - fh;
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

	f = dimg->NewFontPx(UTF8STRC("Arial"), fh, Media::DrawEngine::DFS_NORMAL, 0);
	b = dimg->NewBrushARGB(0xff000000);
	sbuff[1] = 0;
	i = codeWidth * 5;
	j = 4;
	while (j-- > 0)
	{
		sbuff[0] = *code++;
		dimg->DrawString((Double)i, Math::UOSInt2Double(y) - fh, sbuff, f, b);
		i += 7 * codeWidth;
	}
	i += 5 * codeWidth;
	j = 4;
	while (j-- > 0)
	{
		sbuff[0] = *code++;
		dimg->DrawString((Double)i, Math::UOSInt2Double(y) - fh, sbuff, f, b);
		i += 7 * codeWidth;
	}
	dimg->DelBrush(b);
	dimg->DelFont(f);

	return dimg;
}
