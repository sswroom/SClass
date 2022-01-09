#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Media/CodeImageGen/EAN5CodeImageGen.h"
#include "Text/MyString.h"

Media::CodeImageGen::EAN5CodeImageGen::EAN5CodeImageGen()
{
}

Media::CodeImageGen::EAN5CodeImageGen::~EAN5CodeImageGen()
{
}

Media::CodeImageGen::CodeImageGen::CodeType Media::CodeImageGen::EAN5CodeImageGen::GetCodeType()
{
	return Media::CodeImageGen::CodeImageGen::CT_EAN5;
}

UOSInt Media::CodeImageGen::EAN5CodeImageGen::GetMinLength()
{
	return 5;
}

UOSInt Media::CodeImageGen::EAN5CodeImageGen::GetMaxLength()
{
	return 5;
}

Media::DrawImage *Media::CodeImageGen::EAN5CodeImageGen::GenCode(const UTF8Char *code, UOSInt codeWidth, Media::DrawEngine *eng)
{
	UTF8Char sbuff[2];
	if (code == 0)
		return 0;

	UOSInt i = 5;
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
			j += (UOSInt)(c - '0') * 9;
		}
		else
		{
			j += (UOSInt)(c - '0') * 3;
		}
	}
	if (*tmpStr != 0)
		return 0;
	UInt8 bitCode[48];
	bitCode[0] = 0;
	bitCode[1] = 1;
	bitCode[2] = 0;
	bitCode[3] = 1;
	bitCode[4] = 1;
	switch (j % 10)
	{
	case 0:
		tmpStr = (const UTF8Char*)"GGLLL";
		break;
	case 1:
		tmpStr = (const UTF8Char*)"GLGLL";
		break;
	case 2:
		tmpStr = (const UTF8Char*)"GLLGL";
		break;
	case 3:
		tmpStr = (const UTF8Char*)"GLLLG";
		break;
	case 4:
		tmpStr = (const UTF8Char*)"LGGLL";
		break;
	case 5:
		tmpStr = (const UTF8Char*)"LLGGL";
		break;
	case 6:
		tmpStr = (const UTF8Char*)"LLLGG";
		break;
	case 7:
		tmpStr = (const UTF8Char*)"LGLGL";
		break;
	case 8:
		tmpStr = (const UTF8Char*)"LGLLG";
		break;
	case 9:
		tmpStr = (const UTF8Char*)"LLGLG";
		break;
	}
	j = 5;
	i = 5;
	while (i-- > 0)
	{
		if (*tmpStr++ == 'L') //L-code
		{
			switch (*code++)
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
			switch (*code++)
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
	}
	code = code - 5;

	UOSInt h = codeWidth * 70;
	UOSInt y = h - codeWidth;
	Double fh = 12.0 * Math::UOSInt2Double(codeWidth);

	Media::DrawImage *dimg = eng->CreateImage32((4 + 48) * codeWidth, h, Media::AT_NO_ALPHA);
	Media::DrawBrush *b;
	Media::DrawPen *p;
	Media::DrawFont *f;
	b = dimg->NewBrushARGB(0xffffffff);
	dimg->DrawRect(0, 0, (Double)dimg->GetWidth(), (Double)dimg->GetHeight(), 0, b);
	dimg->DelBrush(b);

	p = dimg->NewPenARGB(0xff000000, 1, 0, 0);
	i = 0;
	j = 2 * codeWidth;
	while (i < 48)
	{
		if (bitCode[i])
		{
			k = codeWidth;
			while (k-- > 0)
			{
				dimg->DrawLine((Double)j, Math::UOSInt2Double(codeWidth) + fh, (Double)j, (Double)y, p);
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
	j = 5;
	while (j-- > 0)
	{
		sbuff[0] = *code++;
		dimg->DrawString((Double)i, (Double)codeWidth, sbuff, f, b);
		i += 9 * codeWidth;
	}
	dimg->DelBrush(b);
	dimg->DelFont(f);

	return dimg;
}
