#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayList.h"
#include "Data/ArrayListInt32.h"
#include "IO/Console.h"
#include "IO/FileStream.h"
#include "IO/FileParser.h"
#include "IO/StreamData.h"
#include "IO/Path.h"
#include "IO/Stream.h"
#include "IO/StreamReader.h"
#include "IO/StmData/FileData.h"
#include "Manage/HiResClock.h"
#include "Map/MapDrawLayer.h"
#include "Map/MapView.h"
#include "Map/MapConfig2.h"
#include "Map/CIPLayer2.h"
#include "Math/Math.h"
#include "Math/GeometryTool.h"
#include "Media/DrawEngine.h"
#include "Media/FrameInfo.h"
#include "Media/IImgResizer.h"
#include "Media/StaticImage.h"
#include "Media/ImageList.h"
#include "Media/StaticImage.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "Text/StringTool.h"

#ifdef _WIN32_WCE
#define MulDiv(x,y,z) ((x) * (y) / (z))
#endif

#define SFLG_ROTATE 1
#define SFLG_SMART 2
#define SFLG_ALIGN 4
#define LBLMINDIST 150
//#define NOSCH
//#define PrintDebug(msg) IO::Console::PrintStr(msg)
#define PrintDebug(msg)

#define PI 3.14159265358979323846

Bool Map::MapConfig2::IsDoorNum(const UTF8Char *txt)
{
	UTF8Char c;
	while ((c = *txt++) != 0)
	{
		if (c == '-')
		{

		}
		else if (c >= '0' && c <= '9')
		{
		}
		else
		{
			return false;
		}
	}
	return true;
}

void Map::MapConfig2::DrawChars(NN<Media::DrawImage> img, Text::CStringNN str1, Math::Coord2DDbl scnPos, Double scaleW, Double scaleH, Data::ArrayList<MapFontStyle*> *fontStyle, Bool isAlign)
{
	UTF8Char sbuff[256];
	str1.ConcatTo(sbuff);
	Math::Size2DDbl size;
	UInt16 absH;
	UOSInt fntCount;
	UInt32 i;
	UInt32 maxSize;
	UInt32 maxIndex;
	UOSInt lblSize;
	Map::MapFontStyle *font;
	Map::MapFontStyle *font2;

	if (fontStyle == 0)
		return;
	if (scaleW < 0)
	{
		scaleW = -scaleW;
		scaleH = -scaleH;
	}

	fntCount = fontStyle->GetCount();

	maxSize = 0;
	maxIndex = 0;
	i = 0;
	while (i < fntCount)
	{
		font = fontStyle->GetItem(i);
		if (font->fontType == 0)
		{
			if (maxSize < font->fontSizePt)
			{
				maxSize = font->fontSizePt;
				maxIndex = i;
			}
		}
		i++;
	}

	if (maxSize == 0)
	{
		return;
	}

	lblSize = str1.leng;
	font = (Map::MapFontStyle*)fontStyle->GetItem(maxIndex);
	if (scaleH == 0)
	{
		scaleH = 0;
	}
	size = img->GetTextSize(font->font, str1);

	if (scaleH == 0)
	{
		Double rcLeft;
		Double rcRight;
		Double rcTop;
		Double rcBottom;
		NN<Media::DrawBrush> b;
		NN<Media::DrawPen> p;
		img->SetTextAlign(Media::DrawEngine::DRAW_POS_TOPLEFT);

		i = 0;
		while (i < fntCount)
		{
			font = (Map::MapFontStyle*)fontStyle->GetItem(i);
			if (font->fontType == 1)
			{
				Media::DrawPen *p = 0;
				Media::DrawBrush *b = 0;
				rcLeft = scnPos.x - ((size.x + font->fontSizePt) * 0.5);
				rcRight = rcLeft + size.x + font->fontSizePt;
				rcTop = scnPos.y - ((size.y + font->fontSizePt) * 0.5);
				rcBottom = rcTop + size.y + font->fontSizePt;

				b = (Media::DrawBrush *)font->other;
				if (i < fntCount - 1)
				{
					font2 = (Map::MapFontStyle*)fontStyle->GetItem(i + 1);
					if (font2->fontType == 2 && font2->fontSizePt == font->fontSizePt)
					{
						i++;
						p = (Media::DrawPen*)font2->other;
						rcRight += font2->thick * 0.5;
						rcBottom += font2->thick * 0.5;
					}
				}

				img->DrawRect(Math::Coord2DDbl(rcLeft, rcTop), Math::Size2DDbl(rcRight - rcLeft, rcBottom - rcTop), p, b);
			}
			else if (font->fontType == 2 && p.Set((Media::DrawPen*)font->other))
			{
				Math::Coord2DDbl pt[5];
				rcLeft = scnPos.x - ((size.x + font->fontSizePt) * 0.5);
				rcRight = rcLeft + size.x + font->fontSizePt;
				rcTop = scnPos.y - ((size.y + font->fontSizePt) * 0.5);
				rcBottom = rcTop + size.y + font->fontSizePt;

				pt[0].x = rcLeft;
				pt[0].y = rcTop;
				pt[1].x = rcRight;
				pt[1].y = rcTop;
				pt[2].x = rcRight;
				pt[2].y = rcBottom;
				pt[3].x = rcLeft;
				pt[3].y = rcBottom;
				pt[4] = pt[0];

				img->DrawPolyline(pt, 5, p);
			}
			else if (font->fontType == 0 && b.Set((Media::DrawBrush*)font->other))
			{
				img->DrawString(scnPos - (size * 0.5), str1, font->font, b);
			}
			else if (font->fontType == 4 && b.Set((Media::DrawBrush*)font->other))
			{
				img->DrawStringB(scnPos - (size * 0.5), str1, font->font, b, (UInt32)Double2Int32(font->thick));
			}
			i++;
		}

		return;
	}


	if (scaleH < 0)
		absH = (UInt16)-scaleH;
	else
		absH = (UInt16)scaleH;

	Double degD = Math_ArcTan2(scaleH, scaleW);

/*	Int32 deg = Double2Int32(Math_ArcTan2(scaleH, scaleW) * 1800 / PI);
	while (deg < 0)
		deg += 3600;*/

	Double lastScaleW = scaleW;
	Double lastScaleH = scaleH;

	i = 0;
	while (i < fntCount)
	{
		NN<Media::DrawBrush> nnb;
		Media::DrawBrush *b;
		NN<Media::DrawPen> nnp;
		Media::DrawPen *p;

		scaleW = lastScaleW;
		scaleH = lastScaleH;
		Math::Coord2DDbl pt[5];
		Double xPos;
		Double yPos;
		Double sVal;
		Double cVal;

		font = (Map::MapFontStyle *)fontStyle->GetItem(i);

		if (font->fontType == 1)
		{
			xPos = size.x + font->fontSizePt;
			yPos = size.y + font->fontSizePt;
			Double xs = ((xPos * 0.5) * (sVal = Math_Sin(degD)));
			Double ys = ((yPos * 0.5) * sVal);
			Double xc = ((xPos * 0.5) * (cVal = Math_Cos(degD)));
			Double yc = ((yPos * 0.5) * cVal);

			pt[0].x = scnPos.x - xc - ys;
			pt[0].y = scnPos.y + xs - yc;
			pt[1].x = scnPos.x + xc - ys;
			pt[1].y = scnPos.y - xs - yc;
			pt[2].x = scnPos.x + xc + ys;
			pt[2].y = scnPos.y - xs + yc;
			pt[3].x = scnPos.x - xc + ys;
			pt[3].y = scnPos.y + xs + yc;

			p = 0;
			b = (Media::DrawBrush*)font->other;

			if (i < fntCount - 1)
			{
				font2 = (Map::MapFontStyle *)fontStyle->GetItem(i + 1);
				if (font2->fontType == 2 && font2->fontSizePt == font->fontSizePt)
				{
					i++;
					p = (Media::DrawPen*)font2->other;
				}
			}
			img->DrawPolygon(pt, 4, p, b);
		}
		else if (font->fontType == 2)
		{
			if (nnp.Set((Media::DrawPen*)font->other))
			{
				xPos = size.x + font->fontSizePt;
				yPos = size.y + font->fontSizePt;
				Int32 xs = (Int32) ((xPos * 0.5) * (sVal = Math_Sin(degD)));
				Int32 ys = (Int32) ((yPos * 0.5) * sVal);
				Int32 xc = (Int32) ((xPos * 0.5) * (cVal = Math_Cos(degD)));
				Int32 yc = (Int32) ((yPos * 0.5) * cVal);

				pt[0].x = scnPos.x - xc - ys;
				pt[0].y = scnPos.y + xs - yc;
				pt[1].x = scnPos.x + xc - ys;
				pt[1].y = scnPos.y - xs - yc;
				pt[2].x = scnPos.x + xc + ys;
				pt[2].y = scnPos.y - xs + yc;
				pt[3].x = scnPos.x - xc + ys;
				pt[3].y = scnPos.y + xs + yc;
				pt[4] = pt[0];
				
				img->DrawPolyline(pt, 5, nnp);
			}
		}
		else if (font->fontType == 0 || font->fontType == 4)
		{
			if (nnb.Set((Media::DrawBrush*)font->other))
			{
				if (isAlign)
				{
					img->SetTextAlign(Media::DrawEngine::DRAW_POS_TOPLEFT);
					Math::Coord2DDbl currPt = Math::Coord2DDbl(0, 0);
					Double startX;
					Double startY;
					Double tmp;
					Int32 type;
					Math::Size2DDbl szThis = img->GetTextSize(font->font, str1);

					if ((szThis.x * absH) < (szThis.y * UOSInt2Double(lblSize) * scaleW))
					{
						scaleW = -scaleW;
						startX = scnPos.x - (tmp = (szThis.x * 0.5));
						if (scaleW)
							startY = scnPos.y - (szThis.y * 0.5) - (tmp * scaleH / scaleW);
						else
							startY = scnPos.y - (szThis.y * 0.5);
						type = 0;
					}
					else
					{
						scaleW = -scaleW;
						if (scaleH > 0)
						{
							startY = scnPos.y - (tmp = ((szThis.y * UOSInt2Double(lblSize)) * 0.5));
							startX = scnPos.x - (tmp * scaleW / scaleH);
						}
						else if (scaleH)
						{
							scaleW = -scaleW;
							scaleH = -scaleH;
							startY = scnPos.y - (tmp = ((szThis.y * UOSInt2Double(lblSize)) * 0.5));
							startX = scnPos.x - (tmp * scaleW / scaleH);
						}
						else
						{
							startY = scnPos.y - (tmp = ((szThis.y * UOSInt2Double(lblSize)) * 0.5));
							startX = scnPos.x;
						}
						type = 1;
					}

	//				Double cHeight;
	//				if (scaleH < 0)
	//					cHeight = szThis.height;
	//				else
	//					cHeight = -szThis.height;

					currPt.x = 0;
					currPt.y = 0;

					UOSInt cnt;
					UTF8Char *lbl = sbuff;
					cnt = lblSize;

					while (cnt--)
					{
						szThis = img->GetTextSize(font->font, {lbl, 1});

						if (type)
						{
							if (font->fontType == 0)
							{
								UTF8Char l[2];
								l[0] = lbl[0];
								l[1] = 0;
								img->DrawString(Math::Coord2DDbl(startX + currPt.x - (szThis.x * 0.5), startY + currPt.y), {l, 1}, font->font, nnb);
							}
							else
							{
								UTF8Char l[2];
								l[0] = lbl[0];
								l[1] = 0;
								img->DrawStringB(Math::Coord2DDbl(startX + currPt.x - (szThis.x * 0.5), startY + currPt.y), {l, 1}, font->font, nnb, (UInt32)Double2Int32(font->thick));
							}

							currPt.y += szThis.y;

							if (scaleH)
								currPt.x = (currPt.y * scaleW / scaleH);
						}
						else
						{
							if (font->fontType == 0)
							{
								UTF8Char l[2];
								l[0] = lbl[0];
								l[1] = 0;
								img->DrawString(Math::Coord2DDbl(startX + currPt.x, startY + currPt.y), {l, 1}, font->font, nnb);
							}
							else
							{
								UTF8Char l[2];
								l[0] = lbl[0];
								l[1] = 0;
								img->DrawStringB(Math::Coord2DDbl(startX + currPt.x, startY + currPt.y), {l, 1}, font->font, nnb, (UInt32)Double2Int32(font->thick));
							}

							currPt.x += szThis.x;
							if (scaleW)
								currPt.y = (Int16)((Int32)currPt.x * (Int32)scaleH / (Int32)scaleW);
						}
						lbl += 1;
					}

				}
				else
				{
					img->SetTextAlign(Media::DrawEngine::DRAW_POS_CENTER);

					if (font->fontType == 0)
					{
						img->DrawStringRot(scnPos, str1, font->font, nnb, (degD * 180.0 / PI));
					}
					else if (font->fontType == 4)
					{
						img->DrawStringRotB(scnPos, str1, font->font, nnb, (degD * 180.0 / PI), (UInt32)Double2Int32(font->thick));
					}
				}
			}
		}
		i++;
	}
}

void Map::MapConfig2::DrawCharsLA(NN<Media::DrawImage> img, Text::CStringNN str1, Math::Coord2DDbl *mapPts, Math::Coord2D<Int32> *scnPts, UOSInt nPoints, UInt32 thisPt, Double scaleN, Double scaleD, Data::ArrayList<MapFontStyle*> *fontStyle, Math::RectAreaDbl *realBounds)
{
	UTF8Char sbuff[256];
	str1.ConcatTo(sbuff);
	UOSInt lblSize = str1.leng;
	Math::Coord2DDbl centPt = scnPts[thisPt].ToDouble() + (scnPts[thisPt + 1].ToDouble() - scnPts[thisPt].ToDouble()) * scaleN / scaleD;
	Math::Coord2DDbl currPt;
	Math::Coord2DDbl nextPt;
	Math::Coord2DDbl startPt = Math::Coord2DDbl(0, 0);
	Math::Coord2DDbl diff;
	Math::Coord2DDbl aDiff;
	Math::Coord2DDbl min;
	Math::Coord2DDbl max;
	UOSInt i;
	UOSInt j;
	Double angleOfst;
	Math::Size2DDbl szThis;
	Math::Size2DDbl szLast;
	Int32 mode;
	UOSInt fntCount;
	Map::MapFontStyle *font;
	Bool found;

	if (fontStyle == 0)
		return;

	max = min = centPt;

	fntCount = fontStyle->GetCount();
	font = (Map::MapFontStyle*)fontStyle->GetItem(0);

	diff = scnPts[thisPt + 1].ToDouble() - scnPts[thisPt].ToDouble();
	aDiff = diff.Abs();

	if (aDiff.x > aDiff.y)
	{
		if (diff.x > 0)
		{
			mode = 0;
			angleOfst = 0;
		}
		else
		{
			mode = 1;
			angleOfst = PI;
		}
	}
	else
	{
		if (diff.y > 0)
		{
			mode = 0;
			angleOfst = 0;
		}
		else
		{
			mode = 1;
			angleOfst = PI;
		}
	}

	currPt = centPt;
	i = lblSize;
	j = thisPt;
	nextPt = diff;
	diff = Math::Coord2DDbl(0, 0);

	while (i-- > 0)
	{
		szThis = img->GetTextSize(font->font, {&str1.v[i], 1});
		diff += szThis;
	}
	found = false;
	if (mode == 0)
	{
		if (aDiff.x > aDiff.y)
		{
			if (nextPt.x > 0)
			{
				if ((centPt.x - diff.x) >= scnPts[j].x)
				{
					startPt.x = centPt.x - diff.x;
					startPt.y = scnPts[j].y + (scnPts[j + 1].y - scnPts[j].y) * (startPt.x - scnPts[j].x) / (scnPts[j + 1].x - scnPts[j].x);
					found = true;
				}
			}
			else
			{
				if ((centPt.x + diff.x) >= scnPts[j].x)
				{
					startPt.x = centPt.x + diff.x;
					startPt.y = scnPts[j].y + (scnPts[j + 1].y - scnPts[j].y) * (startPt.x - scnPts[j].x) / (scnPts[j + 1].x - scnPts[j].x);
					found = true;
				}
			}
		}
		else
		{
			if (nextPt.y > 0)
			{
				if ((centPt.y - diff.y) >= scnPts[j].y)
				{
					startPt.y = centPt.y - diff.y;
					startPt.x = scnPts[j].x + (scnPts[j + 1].x - scnPts[j].x) * (startPt.y - scnPts[j].y) / (scnPts[j + 1].y - scnPts[j].y);
					found = true;
				}
			}
			else
			{
				if ((centPt.y + diff.y) >= scnPts[j].y)
				{
					startPt.y = centPt.y + diff.y;
					startPt.x = scnPts[j].x + (scnPts[j + 1].x - scnPts[j].x) * (startPt.y - scnPts[j].y) / (scnPts[j + 1].y - scnPts[j].y);
					found = true;
				}
			}
		}
	}
	else
	{
		if (aDiff.x > aDiff.y)
		{
			if (nextPt.x > 0)
			{
				if ((centPt.x - diff.x) >= scnPts[j + 1].x)
				{
					startPt.x = centPt.x - diff.x;
					startPt.y = scnPts[j].y + (scnPts[j + 1].y - scnPts[j].y) * (startPt.x - scnPts[j].x) / (scnPts[j + 1].x - scnPts[j].x);
					found = true;
				}
			}
			else
			{
				if ((centPt.x - diff.x) >= scnPts[j + 1].x)
				{
					startPt.x = centPt.x - diff.x;
					startPt.y = scnPts[j].y + (scnPts[j + 1].y - scnPts[j].y) * (startPt.x - scnPts[j].x) / (scnPts[j + 1].x - scnPts[j].x);
					found = true;
				}
			}
		}
		else
		{
			if (nextPt.y > 0)
			{
				if ((centPt.y - diff.y) >= scnPts[j + 1].y)
				{
					startPt.y = centPt.y - diff.y;
					startPt.x = scnPts[j].x + (scnPts[j + 1].x - scnPts[j].x) * (startPt.y - scnPts[j].y) / (scnPts[j + 1].y - scnPts[j].y);
					found = true;
				}
			}
			else
			{
				if ((centPt.y - diff.y) >= scnPts[j + 1].y)
				{
					startPt.y = centPt.y - diff.y;
					startPt.x = scnPts[j].x + (scnPts[j + 1].x - scnPts[j].x) * (startPt.y - scnPts[j].y) / (scnPts[j + 1].y - scnPts[j].y);
					found = true;
				}
			}
		}
	}

	if (!found)
	{
		if (mode == 0)
		{
			while (j-- > 0)
			{
				if (aDiff.x > aDiff.y)
				{
					if ((scnPts[j].x - (centPt.x - diff.x) >= 0) ^ (scnPts[j + 1].x - (centPt.x - diff.x) >= 0))
					{
						startPt.x = centPt.x - diff.x;
						startPt.y = scnPts[j].y + (scnPts[j + 1].y - scnPts[j].y) * (startPt.x - scnPts[j].x) / (scnPts[j + 1].x - scnPts[j].x);
						break;
					}
					else if ((scnPts[j].x - (centPt.x + diff.x) >= 0) ^ (scnPts[j + 1].x - (centPt.x + diff.x) >= 0))
					{
						startPt.x = centPt.x + diff.x;
						startPt.y = scnPts[j].y + (scnPts[j + 1].y - scnPts[j].y) * (startPt.x - scnPts[j].x) / (scnPts[j + 1].x - scnPts[j].x);
						break;
					}
					else if ((scnPts[j].y - (centPt.y - diff.y) >= 0) ^ (scnPts[j + 1].y - (centPt.y - diff.y) >= 0))
					{
						startPt.y = centPt.y - diff.y;
						startPt.x = scnPts[j].x + (scnPts[j + 1].x - scnPts[j].x) * (startPt.y - scnPts[j].y) / (scnPts[j + 1].y - scnPts[j].y);
						break;
					}
					else if ((scnPts[j].y - (centPt.y + diff.y) >= 0) ^ (scnPts[j + 1].y - (centPt.y + diff.y) >= 0))
					{
						startPt.y = centPt.y + diff.y;
						startPt.x = scnPts[j].x + (scnPts[j + 1].x - scnPts[j].x) * (startPt.y - scnPts[j].y) / (scnPts[j + 1].y - scnPts[j].y);
						break;
					}
				}
				else
				{
					if ((scnPts[j].y - (centPt.y - diff.y) >= 0) ^ (scnPts[j + 1].y - (centPt.y - diff.y) >= 0))
					{
						startPt.y = centPt.y - diff.y;
						startPt.x = scnPts[j].x + (scnPts[j + 1].x - scnPts[j].x) * (startPt.y - scnPts[j].y) / (scnPts[j + 1].y - scnPts[j].y);
						break;
					}
					else if ((scnPts[j].y - (centPt.y + diff.y) >= 0) ^ (scnPts[j + 1].y - (centPt.y + diff.y) >= 0))
					{
						startPt.y = centPt.y + diff.y;
						startPt.x = scnPts[j].x + (scnPts[j + 1].x - scnPts[j].x) * (startPt.y - scnPts[j].y) / (scnPts[j + 1].y - scnPts[j].y);
						break;
					}
					else if ((scnPts[j].x - (centPt.x - diff.x) >= 0) ^ (scnPts[j + 1].x - (centPt.x - diff.x) >= 0))
					{
						startPt.x = centPt.x - diff.x;
						startPt.y = scnPts[j].y + (scnPts[j + 1].y - scnPts[j].y) * (startPt.x - scnPts[j].x) / (scnPts[j + 1].x - scnPts[j].x);
						break;
					}
					else if ((scnPts[j].x - (centPt.x + diff.x) >= 0) ^ (scnPts[j + 1].x - (centPt.x + diff.x) >= 0))
					{
						startPt.x = centPt.x + diff.x;
						startPt.y = scnPts[j].y + (scnPts[j + 1].y - scnPts[j].y) * (startPt.x - scnPts[j].x) / (scnPts[j + 1].x - scnPts[j].x);
						break;
					}
				}

			}
			if (j == (UOSInt)-1)
			{
				j = 0;
				startPt = scnPts[0].ToDouble();
			}
		}
		else
		{
			j++;
			while (j < nPoints - 1)
			{
				if (aDiff.x > aDiff.y)
				{
					if ((scnPts[j].x - (centPt.x - diff.x) >= 0) ^ (scnPts[j + 1].x - (centPt.x - diff.x) >= 0))
					{
						startPt.x = centPt.x - diff.x;
						startPt.y = scnPts[j].y + (scnPts[j + 1].y - scnPts[j].y) * (startPt.x - scnPts[j].x) / (scnPts[j + 1].x - scnPts[j].x);
						break;
					}
					else if ((scnPts[j].x - (centPt.x + diff.x) >= 0) ^ (scnPts[j + 1].x - (centPt.x + diff.x) >= 0))
					{
						startPt.x = centPt.x + diff.x;
						startPt.y = scnPts[j].y + (scnPts[j + 1].y - scnPts[j].y) * (startPt.x - scnPts[j].x) / (scnPts[j + 1].x - scnPts[j].x);
						break;
					}
					else if ((scnPts[j].y - (centPt.y - diff.y) >= 0) ^ (scnPts[j + 1].y - (centPt.y - diff.y) >= 0))
					{
						startPt.y = centPt.y - diff.y;
						startPt.x = scnPts[j].x + (scnPts[j + 1].x - scnPts[j].x) * (startPt.y - scnPts[j].y) / (scnPts[j + 1].y - scnPts[j].y);
						break;
					}
					else if ((scnPts[j].y - (centPt.y + diff.y) >= 0) ^ (scnPts[j + 1].y - (centPt.y + diff.y) >= 0))
					{
						startPt.y = centPt.y + diff.y;
						startPt.x = scnPts[j].x + (scnPts[j + 1].x - scnPts[j].x) * (startPt.y - scnPts[j].y) / (scnPts[j + 1].y - scnPts[j].y);
						break;
					}
				}
				else
				{
					if ((scnPts[j].y - (centPt.y - diff.y) >= 0) ^ (scnPts[j + 1].y - (centPt.y - diff.y) >= 0))
					{
						startPt.y = centPt.y - diff.y;
						startPt.x = scnPts[j].x + (scnPts[j + 1].x - scnPts[j].x) * (startPt.y - scnPts[j].y) / (scnPts[j + 1].y - scnPts[j].y);
						break;
					}
					else if ((scnPts[j].y - (centPt.y + diff.y) >= 0) ^ (scnPts[j + 1].y - (centPt.y + diff.y) >= 0))
					{
						startPt.y = centPt.y + diff.y;
						startPt.x = scnPts[j].x + (scnPts[j + 1].x - scnPts[j].x) * (startPt.y - scnPts[j].y) / (scnPts[j + 1].y - scnPts[j].y);
						break;
					}
					else if ((scnPts[j].x - (centPt.x - diff.x) >= 0) ^ (scnPts[j + 1].x - (centPt.x - diff.x) >= 0))
					{
						startPt.x = centPt.x - diff.x;
						startPt.y = scnPts[j].y + (scnPts[j + 1].y - scnPts[j].y) * (startPt.x - scnPts[j].x) / (scnPts[j + 1].x - scnPts[j].x);
						break;
					}
					else if ((scnPts[j].x - (centPt.x + diff.x) >= 0) ^ (scnPts[j + 1].x - (centPt.x + diff.x) >= 0))
					{
						startPt.x = centPt.x + diff.x;
						startPt.y = scnPts[j].y + (scnPts[j + 1].y - scnPts[j].y) * (startPt.x - scnPts[j].x) / (scnPts[j + 1].x - scnPts[j].x);
						break;
					}
				}

				j++;
			}
			if (j >= nPoints - 1)
			{
				j = nPoints - 2;
				startPt = scnPts[j + 1].ToDouble();
			}
		}
	}

	if ((OSInt)j < 0)
	{
		j = 0;
	}
	else if (j >= nPoints - 1)
	{
		j -= 1;
	}
	NN<Media::DrawBrush> nnb;
	UOSInt startInd = j;
	img->SetTextAlign(Media::DrawEngine::DRAW_POS_CENTER);

	i = 0;
	while (i < fntCount)
	{
		font = (Map::MapFontStyle *)fontStyle->GetItem(i);
		if (nnb.Set((Media::DrawBrush*)font->other))
		{
			UTF8Char *lbl = sbuff;
			UTF8Char *nextPos = lbl;
			UTF8Char nextChar = *lbl;
			Double angle;
			Double angleDegree;
			Double lastAngle;
			UOSInt lastAInd;
			Math::Coord2DDbl lastPt;

			szLast.x = 0;
			szLast.y = 0;

			lastPt = currPt = startPt;
			j = startInd;

			angle = angleOfst - Math_ArcTan2((mapPts[j].y - mapPts[j + 1].y), (mapPts[j + 1].x - mapPts[j].x));
			angleDegree = angle * 180.0 / PI;
			while (angleDegree < 0)
			{
				angleDegree += 360;
			}
			lastAngle = angleDegree;
			lastAInd = j;

			while (nextChar)
			{
				lbl = nextPos;
				*lbl = nextChar;
				nextPos++;
				nextChar = *nextPos;
				*nextPos = 0;

				szThis = img->GetTextSize(font->font, CSTRP(lbl, nextPos));
				while (true)
				{
					if (angleDegree <= 90)
					{
						nextPt.x = currPt.x + ((szLast.x + szThis.x) * 0.5);
						nextPt.y = currPt.y - ((szLast.y + szThis.y) * 0.5);
					}
					else if (angleDegree <= 180)
					{
						nextPt = currPt - ((szLast + szThis) * 0.5);
					}
					else if (angleDegree <= 270)
					{
						nextPt.x = currPt.x - ((szLast.x + szThis.x) * 0.5);
						nextPt.y = currPt.y + ((szLast.y + szThis.y) * 0.5);
					}
					else
					{
						nextPt = currPt + ((szLast + szThis) * 0.5);
					}

					if (((nextPt.x > scnPts[j].x) ^ (nextPt.x > scnPts[j + 1].x)) || (nextPt.x == scnPts[j].x) || (nextPt.x == scnPts[j + 1].x))
					{
						Double tempY = scnPts[j].y + (scnPts[j + 1].y - scnPts[j].y) * (nextPt.x - scnPts[j].x) / (scnPts[j + 1].x - scnPts[j].x);
						tempY -= currPt.y;
						if (tempY < 0)
							tempY = -tempY;
						if (tempY > (szLast.y + szThis.y) * 0.5)
						{
							currPt.y = nextPt.y;
							currPt.x = scnPts[j].x + (scnPts[j + 1].x - scnPts[j].x) * (currPt.y - scnPts[j].y) / (scnPts[j + 1].y - scnPts[j].y);
						}
						else
						{
							currPt.x = nextPt.x;
							currPt.y = scnPts[j].y + (scnPts[j + 1].y - scnPts[j].y) * (nextPt.x - scnPts[j].x) / (scnPts[j + 1].x - scnPts[j].x);
						}
						break;
					}
					else if (((nextPt.y > scnPts[j].y) ^ (nextPt.y > scnPts[j + 1].y)) || (nextPt.y == scnPts[j].y) || (nextPt.y == scnPts[j + 1].y))
					{
						currPt.y = nextPt.y;
						currPt.x = scnPts[j].x + (scnPts[j + 1].x - scnPts[j].x) * (currPt.y - scnPts[j].y) / (scnPts[j + 1].y - scnPts[j].y);
						break;
					}
					else
					{
						if (mode == 0)
						{
							j++;
							if (j >= nPoints - 1)
							{
								j = nPoints - 2;

								Double tempY = scnPts[j].y + (scnPts[j + 1].y - scnPts[j].y) * (nextPt.x - scnPts[j].x) / (scnPts[j + 1].x - scnPts[j].x);
								tempY -= currPt.y;
								if (tempY < 0)
									tempY = -tempY;
								if (tempY > (szLast.y + szThis.y) * 0.5)
								{
									currPt.y = nextPt.y;
									currPt.x = scnPts[j].x + (scnPts[j + 1].x - scnPts[j].x) * (currPt.y - scnPts[j].y) / (scnPts[j + 1].y - scnPts[j].y);
								}
								else
								{
									currPt.x = nextPt.x;
									currPt.y = scnPts[j].y + (scnPts[j + 1].y - scnPts[j].y) * (nextPt.x - scnPts[j].x) / (scnPts[j + 1].x - scnPts[j].x);
								}
								break;
							}
						}
						else if (mode == 1)
						{
							j--;
							if ((OSInt)j < 0)
							{
								j = 0;

								Double tempY = scnPts[j].y + (scnPts[j + 1].y - scnPts[j].y) * (nextPt.x - scnPts[j].x) / (scnPts[j + 1].x - scnPts[j].x);
								tempY -= currPt.y;
								if (tempY < 0)
									tempY = -tempY;
								if (tempY > (szLast.y + szThis.y) * 0.5)
								{
									currPt.y = nextPt.y;
									currPt.x = scnPts[j].x + (scnPts[j + 1].x - scnPts[j].x) * (currPt.y - scnPts[j].y) / (scnPts[j + 1].y - scnPts[j].y);
								}
								else
								{
									currPt.x = nextPt.x;
									currPt.y = scnPts[j].y + (scnPts[j + 1].y - scnPts[j].y) * (nextPt.x - scnPts[j].x) / (scnPts[j + 1].x - scnPts[j].x);
								}
								break;
							}
						}

						angle = angleOfst - Math_ArcTan2((mapPts[j].y - mapPts[j + 1].y), (mapPts[j + 1].x - mapPts[j].x));
						angleDegree = angle * 180.0 / PI;
						while (angleDegree < 0)
						{
							angleDegree += 360;
						}
					}
				}

				Double angleDiff;
				if (lastAngle > angleDegree)
				{
					angleDiff = lastAngle - angleDegree;
				}
				else
				{
					angleDiff = angleDegree - lastAngle;
				}
				if (angleDiff >= 135 && angleDiff <= 215)
				{
					if (lastAngle <= 90)
					{
						nextPt.x = lastPt.x + ((szLast.x + szThis.x) * 0.5);
						nextPt.y = lastPt.y - ((szLast.y + szThis.y) * 0.5);
					}
					else if (lastAngle <= 180)
					{
						nextPt = lastPt - ((szLast + szThis) * 0.5);
					}
					else if (lastAngle <= 270)
					{
						nextPt.x = lastPt.x - ((szLast.x + szThis.x) * 0.5);
						nextPt.y = lastPt.y + ((szLast.y + szThis.y) * 0.5);
					}
					else
					{
						nextPt = lastPt + ((szLast + szThis) * 0.5);
					}
					Double tempY = scnPts[lastAInd].y + (scnPts[lastAInd + 1].y - scnPts[lastAInd].y) * (nextPt.x - scnPts[lastAInd].x) / (scnPts[lastAInd + 1].x - scnPts[lastAInd].x);
					Double tempX = scnPts[lastAInd].x + (scnPts[lastAInd + 1].x - scnPts[lastAInd].x) * (nextPt.y - scnPts[lastAInd].y) / (scnPts[lastAInd + 1].y - scnPts[lastAInd].y);
					tempY -= lastPt.y;
					tempX -= lastPt.x;
					if (tempY < 0)
						tempY = -tempY;
					if (tempX < 0)
						tempX = -tempX;
					if (tempX <= (szLast.x + szThis.x) * 0.5)
					{
						currPt.y = nextPt.y;
						currPt.x = scnPts[lastAInd].x + (scnPts[lastAInd + 1].x - scnPts[lastAInd].x) * (nextPt.y - scnPts[lastAInd].y) / (scnPts[lastAInd + 1].y - scnPts[lastAInd].y);
					}
					else
					{
						currPt.x = nextPt.x;
						currPt.y = scnPts[lastAInd].y + (scnPts[lastAInd + 1].y - scnPts[lastAInd].y) * (nextPt.x - scnPts[lastAInd].x) / (scnPts[lastAInd + 1].x - scnPts[lastAInd].x);
					}
				}
				else
				{
					lastAngle = angleDegree;
					lastAInd = j;
				}


				Double xadd = szThis.x * 0.5;
				Double yadd = szThis.y * 0.5;
				if ((currPt.x - xadd) < min.x)
				{
					min.x = currPt.x - xadd;
				}
				if ((currPt.x + xadd) > max.x)
				{
					max.x = currPt.x + xadd;
				}
				if ((currPt.y - yadd) < min.y)
				{
					min.y = currPt.y - yadd;
				}
				if ((currPt.y + yadd) > max.y)
				{
					max.y = currPt.y + yadd;
				}

				lastPt = currPt;
				if (mode == 0)
				{
					if (font->fontType == 0)
					{
						img->DrawString(currPt, CSTRP(lbl, nextPos), font->font, nnb);
					}
					else
					{
						img->DrawStringB(currPt, CSTRP(lbl, nextPos), font->font, nnb, (UInt32)Double2Int32(font->thick));
					}
				}
				else
				{
					if (font->fontType == 0)
					{
						img->DrawString(currPt, CSTRP(lbl, nextPos), font->font, nnb);
					}
					else
					{
						img->DrawStringB(currPt, CSTRP(lbl, nextPos), font->font, nnb, (UInt32)Double2Int32(font->thick));
					}
				}
				szLast = szThis;
			}
		}
		i++;
	}
	realBounds->min = min;
	realBounds->max = max;
}

void Map::MapConfig2::DrawCharsLAo(NN<Media::DrawImage> img, Text::CStringNN str1, Double *mapPts, Math::Coord2D<Int32> *scnPts, UOSInt nPoints, UInt32 thisPt, Double scaleN, Double scaleD, Data::ArrayList<MapFontStyle*> *fontStyle)
{
	UTF8Char sbuff[256];
	str1.ConcatTo(sbuff);
	UOSInt lblSize = str1.leng;
	Math::Coord2DDbl centPt = scnPts[thisPt].ToDouble() + (scnPts[thisPt + 1].ToDouble() - scnPts[thisPt].ToDouble()) * scaleN / scaleD;
	Math::Coord2DDbl currPt;
	Math::Coord2DDbl nextPt;
	Double startX = 0;
	Double startY = 0;
	Math::Coord2DDbl diff;
	Math::Coord2DDbl aDiff;
	UOSInt i;
	UOSInt j;
	Math::Size2DDbl szThis;
	Int32 mode;
	UOSInt fntCount;
	Map::MapFontStyle *font;
	Bool found;

	if (fontStyle == 0)
		return;

	fntCount = fontStyle->GetCount();
	font = (Map::MapFontStyle*)fontStyle->GetItem(0);

	diff = scnPts[thisPt + 1].ToDouble() - scnPts[thisPt].ToDouble();
	aDiff = diff.Abs();
	if (aDiff.x > aDiff.y)
	{
		if (diff.x > 0)
		{
			mode = 0;
		}
		else
		{
			mode = 1;
		}
	}
	else
	{
		if (diff.y > 0)
		{
			mode = 0;
		}
		else
		{
			mode = 1;
		}
	}

	currPt = centPt;
	i = lblSize;
	j = thisPt;
	nextPt = diff;
	diff.x = 0;
	diff.y = 0;

	while (i-- > 0)
	{
		szThis = img->GetTextSize(font->font, {&str1.v[i], 1});
		diff += szThis;
	}
	found = false;
	if (mode == 0)
	{
		if (aDiff.x > aDiff.y)
		{
			if (nextPt.x > 0)
			{
				if ((centPt.x - diff.x) >= scnPts[j].x)
				{
					startX = centPt.x - diff.x;
					startY = scnPts[j].y + (scnPts[j + 1].y - scnPts[j].y) * (startX - scnPts[j].x) / (scnPts[j + 1].x - scnPts[j].x);
					found = true;
				}
			}
			else
			{
				if ((centPt.x + diff.x) >= scnPts[j].x)
				{
					startX = centPt.x + diff.x;
					startY = scnPts[j].y + (scnPts[j + 1].y - scnPts[j].y) * (startX - scnPts[j].x) / (scnPts[j + 1].x - scnPts[j].x);
					found = true;
				}
			}
		}
		else
		{
			if (nextPt.y > 0)
			{
				if ((centPt.y - diff.y) >= scnPts[j].y)
				{
					startY = centPt.y - diff.y;
					startX = scnPts[j].x + (scnPts[j + 1].x - scnPts[j].x) * (startY - scnPts[j].y) / (scnPts[j + 1].y - scnPts[j].y);
					found = true;
				}
			}
			else
			{
				if ((centPt.y + diff.y) >= scnPts[j].y)
				{
					startY = centPt.y + diff.y;
					startX = scnPts[j].x + (scnPts[j + 1].x - scnPts[j].x) * (startY - scnPts[j].y) / (scnPts[j + 1].y - scnPts[j].y);
					found = true;
				}
			}
		}
	}
	else
	{
		if (aDiff.x > aDiff.y)
		{
			if (nextPt.x > 0)
			{
				if ((centPt.x - diff.x) >= scnPts[j + 1].x)
				{
					startX = centPt.x - diff.x;
					startY = scnPts[j].y + (scnPts[j + 1].y - scnPts[j].y) * (startX - scnPts[j].x) / (scnPts[j + 1].x - scnPts[j].x);
					found = true;
				}
			}
			else
			{
				if ((centPt.x - diff.x) >= scnPts[j + 1].x)
				{
					startX = centPt.x - diff.x;
					startY = scnPts[j].y + (scnPts[j + 1].y - scnPts[j].y) * (startX - scnPts[j].x) / (scnPts[j + 1].x - scnPts[j].x);
					found = true;
				}
			}
		}
		else
		{
			if (nextPt.y > 0)
			{
				if ((centPt.y - diff.y) >= scnPts[j + 1].y)
				{
					startY = centPt.y - diff.y;
					startX = scnPts[j].x + (scnPts[j + 1].x - scnPts[j].x) * (startY - scnPts[j].y) / (scnPts[j + 1].y - scnPts[j].y);
					found = true;
				}
			}
			else
			{
				if ((centPt.y - diff.y) >= scnPts[j + 1].y)
				{
					startY = centPt.y - diff.y;
					startX = scnPts[j].x + (scnPts[j + 1].x - scnPts[j].x) * (startY - scnPts[j].y) / (scnPts[j + 1].y - scnPts[j].y);
					found = true;
				}
			}
		}
	}

	if (!found)
	{
		if (mode == 0)
		{
			while (j-- > 0)
			{
				if (aDiff.x > aDiff.y)
				{
					if ((scnPts[j].x - (centPt.x - diff.x) >= 0) ^ (scnPts[j + 1].x - (centPt.x - diff.x) >= 0))
					{
						startX = centPt.x - diff.x;
						startY = scnPts[j].y + (scnPts[j + 1].y - scnPts[j].y) * (startX - scnPts[j].x) / (scnPts[j + 1].x - scnPts[j].x);
						break;
					}
					else if ((scnPts[j].x - (centPt.x + diff.x) >= 0) ^ (scnPts[j + 1].x - (centPt.x + diff.x) >= 0))
					{
						startX = centPt.x + diff.x;
						startY = scnPts[j].y + (scnPts[j + 1].y - scnPts[j].y) * (startX - scnPts[j].x) / (scnPts[j + 1].x - scnPts[j].x);
						break;
					}
					else if ((scnPts[j].y - (centPt.y - diff.y) >= 0) ^ (scnPts[j + 1].y - (centPt.y - diff.y) >= 0))
					{
						startY = centPt.y - diff.y;
						startX = scnPts[j].x + (scnPts[j + 1].x - scnPts[j].x) * (startY - scnPts[j].y) / (scnPts[j + 1].y - scnPts[j].y);
						break;
					}
					else if ((scnPts[j].y - (centPt.y + diff.y) >= 0) ^ (scnPts[j + 1].y - (centPt.y + diff.y) >= 0))
					{
						startY = centPt.y + diff.y;
						startX = scnPts[j].x + (scnPts[j + 1].x - scnPts[j].x) * (startY - scnPts[j].y) / (scnPts[j + 1].y - scnPts[j].y);
						break;
					}
				}
				else
				{
					if ((scnPts[j].y - (centPt.y - diff.y) >= 0) ^ (scnPts[j + 1].y - (centPt.y - diff.y) >= 0))
					{
						startY = centPt.y - diff.y;
						startX = scnPts[j].x + (scnPts[j + 1].x - scnPts[j].x) * (startY - scnPts[j].y) / (scnPts[j + 1].y - scnPts[j].y);
						break;
					}
					else if ((scnPts[j].y - (centPt.y + diff.y) >= 0) ^ (scnPts[j + 1].y - (centPt.y + diff.y) >= 0))
					{
						startY = centPt.y + diff.y;
						startX = scnPts[j].x + (scnPts[j + 1].x - scnPts[j].x) * (startY - scnPts[j].y) / (scnPts[j + 1].y - scnPts[j].y);
						break;
					}
					else if ((scnPts[j].x - (centPt.x - diff.x) >= 0) ^ (scnPts[j + 1].x - (centPt.x - diff.x) >= 0))
					{
						startX = centPt.x - diff.x;
						startY = scnPts[j].y + (scnPts[j + 1].y - scnPts[j].y) * (startX - scnPts[j].x) / (scnPts[j + 1].x - scnPts[j].x);
						break;
					}
					else if ((scnPts[j].x - (centPt.x + diff.x) >= 0) ^ (scnPts[j + 1].x - (centPt.x + diff.x) >= 0))
					{
						startX = centPt.x + diff.x;
						startY = scnPts[j].y + (scnPts[j + 1].y - scnPts[j].y) * (startX - scnPts[j].x) / (scnPts[j + 1].x - scnPts[j].x);
						break;
					}
				}

			}
			if (j == (UOSInt)-1)
			{
				j = 0;
				startX = scnPts[0].x;
				startY = scnPts[0].y;
			}
		}
		else
		{
			j++;
			while (j < nPoints - 1)
			{
				if (aDiff.x > aDiff.y)
				{
					if ((scnPts[j].x - (centPt.x - diff.x) >= 0) ^ (scnPts[j + 1].x - (centPt.x - diff.x) >= 0))
					{
						startX = centPt.x - diff.x;
						startY = scnPts[j].y + (scnPts[j + 1].y - scnPts[j].y) * (startX - scnPts[j].x) / (scnPts[j + 1].x - scnPts[j].x);
						break;
					}
					else if ((scnPts[j].x - (centPt.x + diff.x) >= 0) ^ (scnPts[j + 1].x - (centPt.x + diff.x) >= 0))
					{
						startX = centPt.x + diff.x;
						startY = scnPts[j].y + (scnPts[j + 1].y - scnPts[j].y) * (startX - scnPts[j].x) / (scnPts[j + 1].x - scnPts[j].x);
						break;
					}
					else if ((scnPts[j].y - (centPt.y - diff.y) >= 0) ^ (scnPts[j + 1].y - (centPt.y - diff.y) >= 0))
					{
						startY = centPt.y - diff.y;
						startX = scnPts[j].x + (scnPts[j + 1].x - scnPts[j].x) * (startY - scnPts[j].y) / (scnPts[j + 1].y - scnPts[j].y);
						break;
					}
					else if ((scnPts[j].y - (centPt.y + diff.y) >= 0) ^ (scnPts[j + 1].y - (centPt.y + diff.y) >= 0))
					{
						startY = centPt.y + diff.y;
						startX = scnPts[j].x + (scnPts[j + 1].x - scnPts[j].x) * (startY - scnPts[j].y) / (scnPts[j + 1].y - scnPts[j].y);
						break;
					}
				}
				else
				{
					if ((scnPts[j].y - (centPt.y - diff.y) >= 0) ^ (scnPts[j + 1].y - (centPt.y - diff.y) >= 0))
					{
						startY = centPt.y - diff.y;
						startX = scnPts[j].x + (scnPts[j + 1].x - scnPts[j].x) * (startY - scnPts[j].y) / (scnPts[j + 1].y - scnPts[j].y);
						break;
					}
					else if ((scnPts[j].y - (centPt.y + diff.y) >= 0) ^ (scnPts[j + 1].y - (centPt.y + diff.y) >= 0))
					{
						startY = centPt.y + diff.y;
						startX = scnPts[j].x + (scnPts[j + 1].x - scnPts[j].x) * (startY - scnPts[j].y) / (scnPts[j + 1].y - scnPts[j].y);
						break;
					}
					else if ((scnPts[j].x - (centPt.x - diff.x) >= 0) ^ (scnPts[j + 1].x - (centPt.x - diff.x) >= 0))
					{
						startX = centPt.x - diff.x;
						startY = scnPts[j].y + (scnPts[j + 1].y - scnPts[j].y) * (startX - scnPts[j].x) / (scnPts[j + 1].x - scnPts[j].x);
						break;
					}
					else if ((scnPts[j].x - (centPt.x + diff.x) >= 0) ^ (scnPts[j + 1].x - (centPt.x + diff.x) >= 0))
					{
						startX = centPt.x + diff.x;
						startY = scnPts[j].y + (scnPts[j + 1].y - scnPts[j].y) * (startX - scnPts[j].x) / (scnPts[j + 1].x - scnPts[j].x);
						break;
					}
				}

				j++;
			}
			if (j >= nPoints - 1)
			{
				j = nPoints - 2;
				startX = scnPts[j + 1].x;
				startY = scnPts[j + 1].y;
			}
		}
	}

	NN<Media::DrawBrush> b;
	if ((OSInt)j < 0)
	{
		j = 0;
	}	
	else if (j >= nPoints - 1)
	{
		j -= 1;
	}
	img->SetTextAlign(Media::DrawEngine::DRAW_POS_TOPLEFT);
	i = 0;
	while (i < fntCount)
	{
		font = (Map::MapFontStyle *)fontStyle->GetItem(i);
		if (b.Set((Media::DrawBrush*)font->other))
		{
			UTF8Char *lbl = sbuff;
			UTF8Char l[2];
			UOSInt currInd;
			UOSInt lastInd;
			UOSInt cnt;
			cnt = lblSize;
			currPt.x = startX;
			currPt.y = startY;
			lastInd = (UOSInt)-1;
			currInd = j;

			while (cnt--)
			{
				if (lastInd != currInd)
				{
					lastInd = currInd;

					diff = scnPts[currInd + 1].ToDouble() - scnPts[currInd].ToDouble();
					aDiff = diff.Abs();

					if (mode == 0)
					{
						nextPt = scnPts[currInd + 1].ToDouble();
					}
					else
					{
						nextPt = scnPts[currInd].ToDouble();
					}
				}

				szThis = img->GetTextSize(font->font, {lbl, 1});
				l[0] = lbl[0];
				l[1] = 0;
				if (font->fontType == 0)
				{
					img->DrawString(currPt - (szThis * 0.5), {l, 1}, font->font, b);
				}
				else
				{
					img->DrawStringB(currPt - (szThis * 0.5), {l, 1}, font->font, b, (UInt32)Double2Int32(font->thick));
				}

				found = false;
				if (mode == 0)
				{
					if (aDiff.x > aDiff.y)
					{
						if (diff.x > 0)
						{
							if (currPt.x + szThis.x <= nextPt.x)
							{
								currPt.x += szThis.x;
								currPt.y = scnPts[currInd].y + (scnPts[currInd + 1].y - scnPts[currInd].y) * (currPt.x - scnPts[currInd].x) / (scnPts[currInd + 1].x - scnPts[currInd].x);
								found = true;
							}
							else
							{
								nextPt.x = currPt.x + szThis.x;
							}
						}
						else
						{
							if (currPt.x - szThis.x >= nextPt.x)
							{
								currPt.x -= szThis.x;
								currPt.y = scnPts[currInd].y + (scnPts[currInd + 1].y - scnPts[currInd].y) * (currPt.x - scnPts[currInd].x) / (scnPts[currInd + 1].x - scnPts[currInd].x);
								found = true;
							}
							else
							{
								nextPt.x = currPt.x - szThis.x;
							}
						}
						if (!found)
						{
							currInd++;
							while (currInd < nPoints - 1)
							{
								if (((scnPts[currInd].x - nextPt.x > 0) ^ (scnPts[currInd + 1].x - nextPt.x > 0)) || (scnPts[currInd].x == nextPt.x) || (scnPts[currInd + 1].x == nextPt.x))
								{
									currPt.x = nextPt.x;
									currPt.y = scnPts[currInd].y + (scnPts[currInd + 1].y - scnPts[currInd].y) * (currPt.x - scnPts[currInd].x) / (scnPts[currInd + 1].x - scnPts[currInd].x);
									found = true;
									break;
								}
								else if (((scnPts[currInd].y - (currPt.y - szThis.y) > 0) ^ (scnPts[currInd + 1].y - (currPt.y - szThis.y) > 0)) || (scnPts[currInd].y == (currPt.y - szThis.y)) || (scnPts[currInd + 1].y == (currPt.y - szThis.y)))
								{
									currPt.y = currPt.y - szThis.y;
									currPt.x = scnPts[currInd].x + (scnPts[currInd + 1].x - scnPts[currInd].x) * (currPt.y - scnPts[currInd].y) / (scnPts[currInd + 1].y - scnPts[currInd].y);
									found = true;
									break;
								}
								else if (((scnPts[currInd].y - (currPt.y + szThis.y) > 0) ^ (scnPts[currInd + 1].y - (currPt.y + szThis.y) > 0)) || (scnPts[currInd].y == (currPt.y + szThis.y)) || (scnPts[currInd + 1].y == (currPt.y + szThis.y)))
								{
									currPt.y = currPt.y + szThis.y;
									currPt.x = scnPts[currInd].x + (scnPts[currInd + 1].x - scnPts[currInd].x) * (currPt.y - scnPts[currInd].y) / (scnPts[currInd + 1].y - scnPts[currInd].y);
									found = true;
									break;
								}
								currInd++;
							}
							if (!found)
							{
								lastInd = (UOSInt)-1;
								currInd--;
							}
						}
					}
					else
					{
						if (diff.y > 0)
						{
							if (currPt.y + szThis.y <= nextPt.y)
							{
								currPt.y += szThis.y;
								currPt.x = scnPts[currInd].x + (scnPts[currInd + 1].x - scnPts[currInd].x) * (currPt.y - scnPts[currInd].y) / (scnPts[currInd + 1].y - scnPts[currInd].y);
								found = true;
							}
							else
							{
								nextPt.y = currPt.y + szThis.y;
							}
						}
						else
						{
							if (currPt.y - szThis.y >= nextPt.y)
							{
								currPt.y -= szThis.y;
								currPt.x = scnPts[currInd].x + (scnPts[currInd + 1].x - scnPts[currInd].x) * (currPt.y - scnPts[currInd].y) / (scnPts[currInd + 1].y - scnPts[currInd].y);
								found = true;
							}
							else
							{
								nextPt.y = currPt.y - szThis.y;
							}
						}
						if (!found)
						{
							currInd++;
							while (currInd < nPoints - 1)
							{
								if (((scnPts[currInd].y - nextPt.y > 0) ^ (scnPts[currInd + 1].y - nextPt.y > 0)) || (scnPts[currInd].y == nextPt.y) || (scnPts[currInd + 1].y == nextPt.y))
								{
									currPt.y = nextPt.y;
									currPt.x = scnPts[currInd].x + (scnPts[currInd + 1].x - scnPts[currInd].x) * (currPt.y - scnPts[currInd].y) / (scnPts[currInd + 1].y - scnPts[currInd].y);
									found = true;
									break;
								}
								else if (((scnPts[currInd].x - (currPt.x - szThis.x) > 0) ^ (scnPts[currInd + 1].x - (currPt.x - szThis.x) > 0)) || (scnPts[currInd].x == (currPt.x - szThis.x)) || (scnPts[currInd + 1].x == (currPt.x - szThis.x)))
								{
									currPt.x = currPt.x - szThis.x;
									currPt.y = scnPts[currInd].y + (scnPts[currInd + 1].y - scnPts[currInd].y) * (currPt.x - scnPts[currInd].x) / (scnPts[currInd + 1].x - scnPts[currInd].x);
									found = true;
									break;
								}
								else if (((scnPts[currInd].x - (currPt.x + szThis.x) > 0) ^ (scnPts[currInd + 1].x - (currPt.x + szThis.x) > 0)) || (scnPts[currInd].x == (currPt.x + szThis.x)) || (scnPts[currInd + 1].x == (currPt.x + szThis.x)))
								{
									currPt.x = currPt.x + szThis.x;
									currPt.y = scnPts[currInd].y + (scnPts[currInd + 1].y - scnPts[currInd].y) * (currPt.x - scnPts[currInd].x) / (scnPts[currInd + 1].x - scnPts[currInd].x);
									found = true;
									break;
								}
								currInd++;
							}
							if (!found)
							{
								lastInd = (UOSInt)-1;
								currInd--;
							}
						}
					}
				}
				else
				{
					if (aDiff.x > aDiff.y)
					{
						if (diff.x < 0)
						{
							if (currPt.x + szThis.x <= nextPt.x)
							{
								currPt.x += szThis.x;
								currPt.y = scnPts[currInd].y + (scnPts[currInd + 1].y - scnPts[currInd].y) * (currPt.x - scnPts[currInd].x) / (scnPts[currInd + 1].x - scnPts[currInd].x);
								found = true;
							}
							else
							{
								nextPt.x = currPt.x + szThis.x;
							}
						}
						else
						{
							if (currPt.x - szThis.x >= nextPt.x)
							{
								currPt.x -= szThis.x;
								currPt.y = scnPts[currInd].y + (scnPts[currInd + 1].y - scnPts[currInd].y) * (currPt.x - scnPts[currInd].x) / (scnPts[currInd + 1].x - scnPts[currInd].x);
								found = true;
							}
							else
							{
								nextPt.x = currPt.x - szThis.x;
							}
						}
						if (!found)
						{
							while (currInd > 0)
							{
								currInd--;
								if (((scnPts[currInd].x - nextPt.x > 0) ^ (scnPts[currInd + 1].x - nextPt.x > 0) || (scnPts[currInd].x == nextPt.x)) || (scnPts[currInd + 1].x == nextPt.x))
								{
									currPt.x = nextPt.x;
									currPt.y = scnPts[currInd].y + (scnPts[currInd + 1].y - scnPts[currInd].y) * (currPt.x - scnPts[currInd].x) / (scnPts[currInd + 1].x - scnPts[currInd].x);
									found = true;
									break;
								}
								else if (((scnPts[currInd].y - (currPt.y - szThis.y) > 0) ^ (scnPts[currInd + 1].y - (currPt.y - szThis.y) > 0)) || (scnPts[currInd].y == (currPt.y - szThis.y)) || (scnPts[currInd + 1].y == (currPt.y - szThis.y)))
								{
									currPt.y = currPt.y - szThis.y;
									currPt.x = scnPts[currInd].x + (scnPts[currInd + 1].x - scnPts[currInd].x) * (currPt.y - scnPts[currInd].y) / (scnPts[currInd + 1].y - scnPts[currInd].y);
									found = true;
									break;
								}
								else if (((scnPts[currInd].y - (currPt.y + szThis.y) > 0) ^ (scnPts[currInd + 1].y - (currPt.y + szThis.y) > 0)) || (scnPts[currInd].y == (currPt.y + szThis.y)) || (scnPts[currInd + 1].y == (currPt.y + szThis.y)))
								{
									currPt.y = currPt.y + szThis.y;
									currPt.x = scnPts[currInd].x + (scnPts[currInd + 1].x - scnPts[currInd].x) * (currPt.y - scnPts[currInd].y) / (scnPts[currInd + 1].y - scnPts[currInd].y);
									found = true;
									break;
								}
							}
							if (!found)
							{
								lastInd = (UOSInt)-1;
							}
						}
					}
					else
					{
						if (diff.y < 0)
						{
							if (currPt.y + szThis.y <= nextPt.y)
							{
								currPt.y += szThis.y;
								currPt.x = scnPts[currInd].x + (scnPts[currInd + 1].x - scnPts[currInd].x) * (currPt.y - scnPts[currInd].y) / (scnPts[currInd + 1].y - scnPts[currInd].y);
								found = true;
							}
							else
							{
								nextPt.y = currPt.y + szThis.y;
							}
						}
						else
						{
							if (currPt.y - szThis.y >= nextPt.y)
							{
								currPt.y -= szThis.y;
								currPt.x = scnPts[currInd].x + (scnPts[currInd + 1].x - scnPts[currInd].x) * (currPt.y - scnPts[currInd].y) / (scnPts[currInd + 1].y - scnPts[currInd].y);
								found = true;
							}
							else
							{
								nextPt.y = currPt.y - szThis.y;
							}
						}
						if (!found)
						{
							while (currInd > 0)
							{
								currInd--;
								if (((scnPts[currInd].y - nextPt.y > 0) ^ (scnPts[currInd + 1].y - nextPt.y > 0)) || (scnPts[currInd].y == nextPt.y) || (scnPts[currInd + 1].y == nextPt.y))
								{
									currPt.y = nextPt.y;
									currPt.x = scnPts[currInd].x + (scnPts[currInd + 1].x - scnPts[currInd].x) * (currPt.y - scnPts[currInd].y) / (scnPts[currInd + 1].y - scnPts[currInd].y);
									found = true;
									break;
								}
								else if (((scnPts[currInd].x - (currPt.x - szThis.x) > 0) ^ (scnPts[currInd + 1].x - (currPt.x - szThis.x) > 0)) || (scnPts[currInd].x == (currPt.x - szThis.x)) || (scnPts[currInd + 1].x == (currPt.x - szThis.x)))
								{
									currPt.x = currPt.x - szThis.x;
									currPt.y = scnPts[currInd].y + (scnPts[currInd + 1].y - scnPts[currInd].y) * (currPt.x - scnPts[currInd].x) / (scnPts[currInd + 1].x - scnPts[currInd].x);
									found = true;
									break;
								}
								else if (((scnPts[currInd].x - (currPt.x + szThis.x) > 0) ^ (scnPts[currInd + 1].x - (currPt.x + szThis.x) > 0)) || (scnPts[currInd].x == (currPt.x + szThis.x)) || (scnPts[currInd + 1].x == (currPt.x + szThis.x)))
								{
									currPt.x = currPt.x + szThis.x;
									currPt.y = scnPts[currInd].y + (scnPts[currInd + 1].y - scnPts[currInd].y) * (currPt.x - scnPts[currInd].x) / (scnPts[currInd + 1].x - scnPts[currInd].x);
									found = true;
									break;
								}
							}
							if (!found)
							{
								lastInd = (UOSInt)-1;
							}
						}
					}
				}
				lbl += 1;
			}
		}
		i++;
	}
}

void Map::MapConfig2::DrawCharsL(NN<Media::DrawImage> img, Text::CStringNN str1, Math::Coord2DDbl *mapPts, Math::Coord2D<Int32> *scnPts, UOSInt nPoints, UInt32 thisPt, Double scaleN, Double scaleD, Data::ArrayList<MapFontStyle*> *fontStyle, Math::RectAreaDbl *realBounds)
{
	UTF8Char sbuff[256];
	str1.ConcatTo(sbuff);
	UOSInt lblSize = str1.leng;
	Math::Coord2DDbl centPt = scnPts[thisPt].ToDouble() + (scnPts[thisPt + 1].ToDouble() - scnPts[thisPt].ToDouble()) * scaleN / scaleD;
	Math::Coord2DDbl currPt;
	Math::Coord2DDbl nextPt;
	Double startX = 0;
	Double startY = 0;
	Math::Coord2DDbl diff;
	Math::Coord2DDbl aDiff;
	Math::Coord2DDbl min;
	Math::Coord2DDbl max;
	Double angleOfst;
	UOSInt i;
	UOSInt j;
	Math::Size2DDbl szThis;
	Math::Size2DDbl szLast;
	Int32 mode;
	UOSInt fntCount;
	Map::MapFontStyle *font;

	if (fontStyle == 0)
		return;

	min = max = centPt;

	fntCount = fontStyle->GetCount();
	font = (Map::MapFontStyle*)fontStyle->GetItem(0);
	if (str1.Equals(UTF8STRC("King Cho Road")))
	{
		i = 0;
	}

	diff = scnPts[thisPt + 1].ToDouble() - scnPts[thisPt].ToDouble();
	aDiff = diff.Abs();

	if (diff.x > 0)
	{
		mode = 0;
		angleOfst = 0;
	}
	else
	{
		mode = 1;
		angleOfst = PI;
	}
/*	if (aDiff.x > aDiff.y)
	{
		if (diff.x > 0)
		{
			mode = 0;
		}
		else
		{
			mode = 1;
		}
	}
	else
	{
		if (diff.y > 0)
		{
			mode = 0;
		}
		else
		{
			mode = 1;
		}
	}*/

	currPt = centPt;
	i = lblSize;
	j = thisPt;
	nextPt = diff;
	diff.x = 0;
	diff.y = 0;

	szThis = img->GetTextSize(font->font, str1);
	diff.x = szThis.x * 0.5;
	diff.y = diff.x * diff.x;

	if (mode == 0)
	{
		while (j >= 0)
		{
			startX = scnPts[j].x - centPt.x;
			startY = scnPts[j].y - centPt.y;
			diff.x = (startX * startX) + (startY * startY);
			if (diff.x >= diff.y)
			{
				if (startX > 0)
				{
					aDiff.x = startX;
				}
				else
				{
					aDiff.x = -startX;
				}
				if (startY > 0)
				{
					aDiff.y = startY;
				}
				else
				{
					aDiff.y = -startY;
				}

				if (aDiff.x > aDiff.y)
				{
					startX = centPt.x + (startX * Math_Sqrt(diff.y) / Math_Sqrt(diff.x));
					startY = scnPts[j].y + (scnPts[j + 1].y - scnPts[j].y) * (startX - scnPts[j].x) / (scnPts[j + 1].x - scnPts[j].x);
				}
				else
				{
					startY = centPt.y + (startY * Math_Sqrt(diff.y) / Math_Sqrt(diff.x));
					startX = scnPts[j].x + (scnPts[j + 1].x - scnPts[j].x) * (startY - scnPts[j].y) / (scnPts[j + 1].y - scnPts[j].y);
				}
				break;
			}
			if (j <= 0)
			{
				startX = scnPts[0].x;
				startY = scnPts[0].y;
				break;
			}
			j--;
		}
	}
	else
	{
		while (j < nPoints - 1)
		{
			startX = scnPts[j + 1].x - centPt.x;
			startY = scnPts[j + 1].y - centPt.y;
			diff.x = (startX * startX) + (startY * startY);
			if (diff.x >= diff.y)
			{
				if (startX > 0)
				{
					aDiff.x = startX;
				}
				else
				{
					aDiff.x = -startX;
				}
				if (startY > 0)
				{
					aDiff.y = startY;
				}
				else
				{
					aDiff.y = -startY;
				}

				if (aDiff.x > aDiff.y)
				{
					startX = centPt.x + (startX * Math_Sqrt(diff.y) / Math_Sqrt(diff.x));
					startY = scnPts[j].y + (scnPts[j + 1].y - scnPts[j].y) * (startX - scnPts[j].x) / (scnPts[j + 1].x - scnPts[j].x);
				}
				else
				{
					startY = centPt.y + (startY * Math_Sqrt(diff.y) / Math_Sqrt(diff.x));
					startX = scnPts[j].x + (scnPts[j + 1].x - scnPts[j].x) * (startY - scnPts[j].y) / (scnPts[j + 1].y - scnPts[j].y);
				}
				break;
			}

			j++;
			if (j >= nPoints - 1)
			{
				j = nPoints - 2;
				startX = scnPts[j + 1].x;
				startY = scnPts[j + 1].y;
				break;
			}
		}
	}

	NN<Media::DrawBrush> b;
	UOSInt startInd = j;
	img->SetTextAlign(Media::DrawEngine::DRAW_POS_CENTER);
	i = 0;
	while (i < fntCount)
	{
		font = (Map::MapFontStyle *)fontStyle->GetItem(i);
		if (b.Set((Media::DrawBrush*)font->other))
		{
			////////////////////////////////
			UTF8Char *lbl = sbuff;
			UTF8Char *nextPos = lbl;
			UTF32Char nextChar;
			Double angle;
			Double cosAngle;
			Double sinAngle;
			Double dist;
			Double angleDegree;
			Double lastAngle;
			Double lastX;
			Double lastY;

			szLast.x = 0;

			lastX = currPt.x = startX;
			lastY = currPt.y = startY;
			j = startInd;
			UOSInt lastInd = j;

			angle = angleOfst - Math_ArcTan2((mapPts[j].y - mapPts[j + 1].y), (mapPts[j + 1].x - mapPts[j].x));
			angleDegree = angle * 180.0 / PI;
			cosAngle = Math_Cos(angle);
			sinAngle = Math_Sin(angle);
			lastAngle = angleDegree;

			Text::StrReadChar(lbl, nextChar);
			while (nextChar)
			{
				lbl = nextPos;
				Text::StrWriteChar(lbl, nextChar);

				while (true)
				{
					nextPos = (UTF8Char*)Text::StrReadChar(nextPos, nextChar);
					if (nextChar == 0)
					{
						nextPos--;
						break;
					}
					if (nextChar == ' ')
					{
						Text::StrReadChar(nextPos, nextChar);
						*nextPos = 0;
						break;
					}
					else if (nextChar >= 0x3f00 && nextChar <= 0x9f00)
					{
						Text::StrReadChar(nextPos, nextChar);
						*nextPos = 0;
						break;
					}
				}

				szThis = img->GetTextSize(font->font, CSTRP(lbl, nextPos));
				dist = (szLast.x + szThis.x) * 0.5;
				nextPt.x = currPt.x + (dist * cosAngle);
				nextPt.y = currPt.y - (dist * sinAngle);
				if ( (((nextPt.x > scnPts[j].x) ^ (nextPt.x > scnPts[j + 1].x)) || (nextPt.x == scnPts[j].x) || (nextPt.x == scnPts[j + 1].x)) && (((nextPt.y > scnPts[j].y) ^ (nextPt.y > scnPts[j + 1].y)) || (nextPt.y == scnPts[j].y) || (nextPt.y == scnPts[j + 1].y)))
				{
					currPt.x = nextPt.x;
					currPt.y = nextPt.y;
				}
				else
				{
					diff.x = szLast.x + szThis.x;
					diff.y = (diff.x * diff.x) * 0.25;

					if (mode == 0)
					{
						j++;
						while (j < nPoints - 1)
						{
							nextPt.x = scnPts[j + 1].x - currPt.x;
							nextPt.y = scnPts[j + 1].y - currPt.y;
							diff.x = (nextPt.x * nextPt.x) + (nextPt.y * nextPt.y);
							if (diff.x < diff.y)
							{
								j++;
							}
							else
							{
								if (nextPt.x > 0)
								{
									aDiff.x = nextPt.x;
								}
								else
								{
									aDiff.x = -nextPt.x;
								}
								if (nextPt.y > 0)
								{
									aDiff.y = nextPt.y;
								}
								else
								{
									aDiff.y = -nextPt.y;
								}

								if (aDiff.x > aDiff.y)
								{
									if ((scnPts[j].x < scnPts[j + 1].x) ^ (nextPt.x > 0))
									{
										currPt.x = currPt.x - nextPt.x * Math_Sqrt(diff.y) / Math_Sqrt(diff.x);
									}
									else
									{
										currPt.x = currPt.x + nextPt.x * Math_Sqrt(diff.y) / Math_Sqrt(diff.x);
									}
									if (((currPt.x > scnPts[j].x) ^ (currPt.x > scnPts[j + 1].x)) || (currPt.x == scnPts[j].x) || (currPt.x == scnPts[j + 1].x))
									{
									}
									else
									{
										currPt.x = scnPts[j].x;
									}
									currPt.y = scnPts[j].y + (scnPts[j + 1].y - scnPts[j].y) * (currPt.x - scnPts[j].x) / (scnPts[j + 1].x - scnPts[j].x);
								}
								else
								{
									if ((scnPts[j].y < scnPts[j + 1].y) ^ (nextPt.y > 0))
									{
										currPt.y = currPt.y - (nextPt.y * Math_Sqrt(diff.y) / Math_Sqrt(diff.x));
									}
									else
									{
										currPt.y = currPt.y + (nextPt.y * Math_Sqrt(diff.y) / Math_Sqrt(diff.x));
									}
									if (((currPt.y > scnPts[j].y) ^ (currPt.y > scnPts[j + 1].y)) || (currPt.y == scnPts[j].y) || (currPt.y == scnPts[j + 1].y))
									{
									}
									else
									{
										currPt.y = scnPts[j].y;
									}
									currPt.x = scnPts[j].x + (scnPts[j + 1].x - scnPts[j].x) * (currPt.y - scnPts[j].y) / (scnPts[j + 1].y - scnPts[j].y);
								}
								break;
							}
						}
						if (j == nPoints - 1)
						{
							j--;

							currPt.x = currPt.x + (dist * cosAngle);
							currPt.y = currPt.y - (dist * sinAngle);
						}
					}
					else if (mode == 1)
					{
						while (j-- > 0)
						{
							nextPt.x = scnPts[j].x - currPt.x;
							nextPt.y = scnPts[j].y - currPt.y;
							diff.x = (nextPt.x * nextPt.x) + (nextPt.y * nextPt.y);
							if (diff.x < diff.y)
							{

							}
							else
							{
								if (nextPt.x > 0)
								{
									aDiff.x = nextPt.x;
								}
								else
								{
									aDiff.x = -nextPt.x;
								}
								if (nextPt.y > 0)
								{
									aDiff.y = nextPt.y;
								}
								else
								{
									aDiff.y = -nextPt.y;
								}

								if (aDiff.x > aDiff.y)
								{
									if ((scnPts[j].x < scnPts[j + 1].x) ^ (nextPt.x > 0))
									{
										currPt.x = currPt.x + (nextPt.x * Math_Sqrt(diff.y) / Math_Sqrt(diff.x));
									}
									else
									{
										currPt.x = currPt.x - (nextPt.x * Math_Sqrt(diff.y) / Math_Sqrt(diff.x));
									}
									if (((currPt.x > scnPts[j].x) ^ (currPt.x > scnPts[j + 1].x)) || (currPt.x == scnPts[j].x) || (currPt.x == scnPts[j + 1].x))
									{
									}
									else
									{
										currPt.x = scnPts[j + 1].x;
									}
									currPt.y = scnPts[j].y + (scnPts[j + 1].y - scnPts[j].y) * (currPt.x - scnPts[j].x) / (scnPts[j + 1].x - scnPts[j].x);
								}
								else
								{
									if ((scnPts[j].y < scnPts[j + 1].y) ^ (nextPt.y > 0))
									{
										currPt.y = currPt.y + (nextPt.y * Math_Sqrt(diff.y) / Math_Sqrt(diff.x));
									}
									else
									{
										currPt.y = currPt.y - (nextPt.y * Math_Sqrt(diff.y) / Math_Sqrt(diff.x));
									}
									if (((currPt.y > scnPts[j].y) ^ (currPt.y > scnPts[j + 1].y)) || (currPt.y == scnPts[j].y) || (currPt.y == scnPts[j + 1].y))
									{
									}
									else
									{
										currPt.y = scnPts[j + 1].y;
									}
									currPt.x = scnPts[j].x + (scnPts[j + 1].x - scnPts[j].x) * (currPt.y - scnPts[j].y) / (scnPts[j + 1].y - scnPts[j].y);
								}
								break;
							}
						}
						if (j == (UOSInt)-1)
						{
							j = 0;
							currPt.x = currPt.x + (dist * cosAngle);
							currPt.y = currPt.y - (dist * sinAngle);
						}
					}

					if (j != lastInd)
					{
						lastInd = j;
						angle = angleOfst - Math_ArcTan2((mapPts[j].y - mapPts[j + 1].y), (mapPts[j + 1].x - mapPts[j].x));
						angleDegree = angle * 180.0 / PI;
						cosAngle = Math_Cos(angle);
						sinAngle = Math_Sin(angle);
					}
				}

				Double angleDiff;
				if (lastAngle > angleDegree)
				{
					angleDiff = lastAngle - angleDegree;
				}
				else
				{
					angleDiff = angleDegree - lastAngle;
				}
				if (angleDiff >= 150 && angleDiff <= 210)
				{
					Double lsa = Math_Sin(lastAngle * PI / 180.0);
					Double lca = Math_Cos(lastAngle * PI / 180.0);
					currPt.x = lastX + (dist * lca);
					currPt.y = lastY - (dist * lsa);

					Double xadd = szThis.x * lca;
					Double yadd = szThis.x * lsa;
					if (xadd < 0)
						xadd = -xadd;
					if (yadd < 0)
						yadd = -yadd;
					if ((currPt.x - xadd) < min.x)
					{
						min.x = (currPt.x - xadd);
					}
					if ((currPt.x + xadd) > max.x)
					{
						max.x = (currPt.x + xadd);
					}
					if ((currPt.y - yadd) < min.y)
					{
						min.y = (currPt.y - yadd);
					}
					if ((currPt.y + yadd) > max.y)
					{
						max.y = (currPt.y + yadd);
					}

					if (mode == 0)
					{
						if (font->fontType == 0)
						{
							img->DrawStringRot(currPt, CSTRP(lbl, nextPos), font->font, b, lastAngle);
						}
						else
						{
							img->DrawStringRotB(currPt, CSTRP(lbl, nextPos), font->font, b, lastAngle, (UInt32)Double2Int32(font->thick));
						}
					}
					else
					{
						if (font->fontType == 0)
						{
							img->DrawStringRot(currPt, CSTRP(lbl, nextPos), font->font, b, lastAngle);
						}
						else
						{
							img->DrawStringRotB(currPt, CSTRP(lbl, nextPos), font->font, b, lastAngle, (UInt32)Double2Int32(font->thick));
						}
					}
				}
				else
				{
					lastAngle = angleDegree;
					Double xadd = szThis.x * cosAngle;
					Double yadd = szThis.x * sinAngle;
					if (xadd < 0)
						xadd = -xadd;
					if (yadd < 0)
						yadd = -yadd;
					if ((currPt.x - xadd) < min.x)
					{
						min.x = (currPt.x - xadd);
					}
					if ((currPt.x + xadd) > max.x)
					{
						max.x = (currPt.x + xadd);
					}
					if ((currPt.y - yadd) < min.y)
					{
						min.y = (currPt.y - yadd);
					}
					if ((currPt.y + yadd) > max.y)
					{
						max.y = (currPt.y + yadd);
					}

					if (mode == 0)
					{
						if (font->fontType == 0)
						{
							img->DrawStringRot(currPt, CSTRP(lbl, nextPos), font->font, b, angleDegree);
						}
						else
						{
							img->DrawStringRotB(currPt, CSTRP(lbl, nextPos), font->font, b, angleDegree, (UInt32)Double2Int32(font->thick));
						}
					}
					else
					{
						if (font->fontType == 0)
						{
							img->DrawStringRot(currPt, CSTRP(lbl, nextPos), font->font, b, angleDegree);
						}
						else
						{
							img->DrawStringRotB(currPt, CSTRP(lbl, nextPos), font->font, b, angleDegree, (UInt32)Double2Int32(font->thick));
						}
					}

				}
				lastX = currPt.x;
				lastY = currPt.y;
				szLast.x = szThis.x;
			}
		}
		i++;
	}
	realBounds->min = min;
	realBounds->max = max;
}

void Map::MapConfig2::GetCharsSize(NN<Media::DrawImage> img, OutParam<Math::Coord2DDbl> size, Text::CStringNN label, Data::ArrayList<MapFontStyle*> *fontStyle, Double scaleW, Double scaleH)
{
	Double xSizeAdd = 0;
	Double ySizeAdd = 0;

	Map::MapFontStyle *font;
	UOSInt fntCount;

	UInt32 i;
	Double maxSize;
	UInt32 maxIndex;

	fntCount = fontStyle->GetCount();;

	maxSize = 0;
	maxIndex = 0;
	i = 0;
	while (i < fntCount)
	{
		font = (Map::MapFontStyle*)fontStyle->GetItem(i);
		if (font->fontType == 0)
		{
			if (maxSize < font->fontSizePt)
			{
				maxSize = font->fontSizePt;
				maxIndex = i;
			}
		}
		else if (font->fontType == 4)
		{
			if (maxSize < font->fontSizePt + font->thick * 2.0)
			{
				maxSize = font->fontSizePt + font->thick * 2.0;
				maxIndex = i;
				xSizeAdd = font->thick * 2.0;
				ySizeAdd = font->thick * 2.0;
			}
		}
		else if (font->fontType == 1 || font->fontType == 2)
		{
			if (xSizeAdd < (font->fontSizePt + font->thick))
			{
				xSizeAdd = (font->fontSizePt + font->thick);
			}
			if (ySizeAdd < (font->fontSizePt + font->thick))
			{
				ySizeAdd = (font->fontSizePt + font->thick);
			}
		}
		i++;
	}

	if (maxSize == 0)
	{
		return;
	}

	font = fontStyle->GetItem(maxIndex);
	Math::Size2DDbl szTmp = img->GetTextSize(font->font, label);

	if (scaleH == 0)
	{
		size.Set(Math::Coord2DDbl(szTmp.x + xSizeAdd, szTmp.y + ySizeAdd));
		return;
	}


	Double pt[8];

	if (scaleW < 0)
	{
		scaleW = -scaleW;
		scaleH = -scaleH;
	}
	Double degD = Math_ArcTan2(scaleH, scaleW);
	Double xPos = szTmp.x + xSizeAdd;
	Double yPos = szTmp.y + ySizeAdd;
	Double sVal;
	Double cVal;
	Double xs = ((xPos * 0.5) * (sVal = Math_Sin(degD)));
	Double ys = ((yPos * 0.5) * sVal);
	Double xc = ((xPos * 0.5) * (cVal = Math_Cos(degD)));
	Double yc = ((yPos * 0.5) * cVal);

	pt[0] = -xc - ys;
	pt[1] = xs - yc;
	pt[2] = xc - ys;
	pt[3] = -xs - yc;
	pt[4] = xc + ys;
	pt[5] = -xs + yc;
	pt[6] = -xc + ys;
	pt[7] = xs + yc;

	Double maxX = pt[0];
	Double minX = pt[0];
	Double maxY = pt[1];
	Double minY = pt[1];
	if (pt[2] > maxX) maxX = pt[2];
	if (pt[2] < minX) minX = pt[2];
	if (pt[3] > maxY) maxY = pt[3];
	if (pt[3] < minY) minY = pt[3];
	if (pt[4] > maxX) maxX = pt[4];
	if (pt[4] < minX) minX = pt[4];
	if (pt[5] > maxY) maxY = pt[5];
	if (pt[5] < minY) minY = pt[5];
	if (pt[6] > maxX) maxX = pt[6];
	if (pt[6] < minX) minX = pt[6];
	if (pt[7] > maxY) maxY = pt[7];
	if (pt[7] < minY) minY = pt[7];
	size.Set(Math::Coord2DDbl(maxX - minX, maxY - minY));
}

UInt32 Map::MapConfig2::ToColor(const UTF8Char *str)
{
	UInt32 v = Text::StrHex2UInt32C(str);
	return 0xff000000 | ((v & 0xff) << 16) | (v & 0xff00) | ((v >> 16) & 0xff);
}

Optional<Map::MapDrawLayer> Map::MapConfig2::GetDrawLayer(Text::CStringNN name, NN<Data::ArrayListNN<Map::MapDrawLayer>> layerList, NN<IO::Writer> errWriter)
{
	NN<Map::CIPLayer2> cip;
	UOSInt i = layerList->GetCount();
	while (i-- > 0)
	{
		NN<Map::MapDrawLayer> lyr;
		lyr = layerList->GetItemNoCheck(i);
		if (IO::Path::FileNameCompare(name.v, lyr->GetName()->v) == 0)
		{
			return lyr;
		}
	}
	NEW_CLASSNN(cip, Map::CIPLayer2(name));
	if (cip->IsError())
	{
		errWriter->Write(CSTR("Error in loading "));
		errWriter->WriteLine(name);
		cip.Delete();
		return 0;
	}
	layerList->Add(cip);
	return cip;
}

void Map::MapConfig2::DrawPoints(NN<Media::DrawImage> img, MapLayerStyle *lyrs, NN<Map::MapView> view, Bool *isLayerEmpty, Map::MapScheduler *sch, NN<Media::DrawEngine> eng, Media::IImgResizer *resizer, Math::RectAreaDbl *objBounds, UOSInt *objCnt, UOSInt maxObjCnt)
{
	NN<Math::Geometry::Vector2D> vec;
	UOSInt imgW;
	UOSInt imgH;
	UOSInt i;
#ifdef NOSCH
	OSInt j;
	Math::Coord2D<Int32> pts;
	Math::RectAreaDbl *objPtr = &objBounds[*objCnt];
#endif
	Map::GetObjectSess *session;
	NN<Map::MapDrawLayer> lyr = lyrs->lyr;

#ifndef NOSCH
	sch->SetDrawType(lyr, 0, 0, lyrs->img, UOSInt2Double(lyrs->img->GetWidth()) * 0.5, UOSInt2Double(lyrs->img->GetHeight()) * 0.5, isLayerEmpty);
	sch->SetDrawObjs(objBounds, objCnt, maxObjCnt);
#endif
	Data::ArrayListInt64 arri;
	Math::RectAreaDbl rect = view->GetVerticalRect();
	lyrs->lyr->GetObjectIdsMapXY(arri, 0, rect, true);
	if (arri.GetCount() == 0)
	{
		return;
	}
	session = lyrs->lyr->BeginGetObject();
	Media::DrawImage *dimg;
	if (img->GetHDPI() != 96)
	{
		imgW = lyrs->img->GetWidth();
		imgH = lyrs->img->GetHeight();
		Media::DrawImage *gimg2 = lyrs->img;
		NN<Media::DrawImage> gimg;
		if (gimg.Set(eng->CreateImage32(Math::Size2D<UOSInt>((UOSInt)Double2OSInt(UOSInt2Double(imgW) * img->GetHDPI() / 96.0), (UOSInt)Double2OSInt(UOSInt2Double(imgH) * img->GetHDPI() / 96.0)), gimg2->GetAlphaType())))
		{
			Bool revOrder;
			Bool revOrder2;
			UInt8 *bmpBits = gimg->GetImgBits(revOrder);
			UInt8 *bmpBits2 = gimg2->GetImgBits(revOrder2);
			resizer->Resize(bmpBits2, (OSInt)imgW << 2, UOSInt2Double(imgW), UOSInt2Double(imgH), 0, 0, bmpBits, Double2Int32(UOSInt2Double(imgW) * img->GetHDPI() / 96.0) << 2, (UInt32)Double2Int32(UOSInt2Double(imgW) * img->GetHDPI() / 96.0), (UInt32)Double2Int32(UOSInt2Double(imgH) * img->GetHDPI() / 96.0));
			gimg->GetImgBitsEnd(true);
			gimg2->GetImgBitsEnd(false);
			dimg = gimg.Ptr();
			imgW = (UInt32)Double2Int32(UOSInt2Double(imgW) * img->GetHDPI() / 96.0) >> 1;
			imgH = (UInt32)Double2Int32(UOSInt2Double(imgH) * img->GetHDPI() / 96.0) >> 1;
	#ifndef NOSCH
			sch->SetDrawType(lyr, 0, 0, dimg, UOSInt2Double(dimg->GetWidth()) * 0.5, UOSInt2Double(dimg->GetHeight()) * 0.5, isLayerEmpty);
	#endif
		}
		else
		{
			dimg = 0;
		}
	}
	else
	{
		imgW = lyrs->img->GetWidth() >> 1;
		imgH = lyrs->img->GetHeight() >> 1;
		dimg = lyrs->img;
	}

	i = arri.GetCount();
	while (i-- > 0)
	{
		if (vec.Set(lyrs->lyr->GetNewVectorById(session, arri.GetItem(i))))
		{
#ifdef NOSCH
			j = dobj->nPoint;
			while (j-- > 0)
			{
				if (view->MapXYToScnXY(&dobj->pointArr[j], &pts, 1, Math::Coord2D<Int32>(0, 0)))
					*isLayerEmpty = false;
				img->DrawImagePt(dimg, objPtr->tl.x = pts.x - imgW, objPtr->tl.y = pts.y - imgH);
				objPtr->br.x = pts.x + imgW;
				objPtr->br.y = pts.y + imgH;
			}
			lyrs->lyr->ReleaseObject(session, dobj);
#else
			sch->Draw(vec);
#endif
		}
	}

	lyrs->lyr->EndGetObject(session);
#ifndef NOSCH
	sch->WaitForFinish();
#endif
	NN<Media::DrawImage> tmpImg;
	if (img->GetHDPI() != 96 && tmpImg.Set(dimg))
	{
		eng->DeleteImage(tmpImg);
	}
}

void Map::MapConfig2::DrawString(NN<Media::DrawImage> img, MapLayerStyle *lyrs, NN<Map::MapView> view, Data::ArrayList<MapFontStyle*> **fonts, MapLabels2 *labels, UInt32 maxLabels, UInt32 *labelCnt, Bool *isLayerEmpty)
{
	Map::NameArray *arr;
	UOSInt i;
	Math::Geometry::Vector2D *vec;
	Double scaleW;
	Double scaleH;
	Math::Coord2DDbl pts;
	Map::GetObjectSess *session;
	UOSInt imgWidth;
	UOSInt imgHeight;

	if (lyrs->img)
	{
		imgWidth = lyrs->img->GetWidth();
		imgHeight = lyrs->img->GetHeight();
	}
	else
	{
		imgWidth = 0;
		imgHeight = 0;
	}

	Data::ArrayListInt64 arri;
	Text::StringBuilderUTF8 sb;
	Math::RectAreaDbl rect = view->GetVerticalRect();
	lyrs->lyr->GetObjectIdsMapXY(arri, &arr, rect, false);
	session = lyrs->lyr->BeginGetObject();
	i = arri.GetCount();
	while (i-- > 0)
	{
		if ((vec = lyrs->lyr->GetNewVectorById(session, arri.GetItem(i))) != 0)
		{
			if (lyrs->bkColor & SFLG_SMART)
			{
				switch (vec->GetVectorType())
				{
				case Math::Geometry::Vector2D::VectorType::Point:
				{
					Math::Coord2DDbl pt = vec->GetCenter();
					sb.ClearStr();
					lyrs->lyr->GetString(sb, arr, arri.GetItem(i), 0);
					AddLabel(labels, maxLabels, labelCnt, sb.ToCString(), 1, &pt, lyrs->priority, Map::DRAW_LAYER_POINT, lyrs->style, lyrs->bkColor, view, (UOSInt2Double(imgWidth) * view->GetHDPI() / view->GetDDPI()), (UOSInt2Double(imgHeight) * view->GetHDPI() / view->GetDDPI()));
					break;
				}
				case Math::Geometry::Vector2D::VectorType::Polyline:
				{
					Math::Geometry::Polyline *pl = (Math::Geometry::Polyline*)vec;
					UOSInt k;
					UOSInt maxSize;
					UOSInt maxPos;
					UOSInt nPoint;
					Math::Coord2DDbl *pointArr;
					NN<Math::Geometry::LineString> ls;
					maxSize = (pl->GetItem(0).SetTo(ls))?ls->GetPointCount():0;
					maxPos = 0;
					k = pl->GetCount();
					while (k-- > 1)
					{
						if (pl->GetItem(k).SetTo(ls))
						{
							nPoint = ls->GetPointCount();
							if (nPoint > maxSize)
							{
								maxSize = nPoint;
								maxPos = k;
							}
						}
					}
					sb.ClearStr();
					lyrs->lyr->GetString(sb, arr, arri.GetItem(i), 0);
					if (pl->GetItem(maxPos).SetTo(ls))
					{
						pointArr = ls->GetPointList(nPoint);
						AddLabel(labels, maxLabels, labelCnt, sb.ToCString(), nPoint, pointArr, lyrs->priority, lyrs->lyr->GetLayerType(), lyrs->style, lyrs->bkColor, view, (UOSInt2Double(imgWidth) * view->GetHDPI() / view->GetDDPI()), (UOSInt2Double(imgHeight) * view->GetHDPI() / view->GetDDPI()));
					}
					break;
				}
				case Math::Geometry::Vector2D::VectorType::Polygon:	
				{
					Math::Geometry::Polygon *pg = (Math::Geometry::Polygon*)vec;
					UOSInt k;
					UOSInt maxSize;
					UOSInt maxPos;
					UOSInt nPoint;
					Math::Coord2DDbl *pointArr;
					NN<Math::Geometry::LinearRing> lr;
					maxSize = (pg->GetItem(0).SetTo(lr))?lr->GetPointCount():0;
					maxPos = 0;
					k = pg->GetCount();
					while (k-- > 1)
					{
						if (pg->GetItem(k).SetTo(lr))
						{
							nPoint = lr->GetPointCount();
							if (nPoint > maxSize)
							{
								maxSize = nPoint;
								maxPos = k;
							}
						}
					}
					sb.ClearStr();
					lyrs->lyr->GetString(sb, arr, arri.GetItem(i), 0);
					if (pg->GetItem(maxPos).SetTo(lr))
					{
						pointArr = lr->GetPointList(nPoint);
						AddLabel(labels, maxLabels, labelCnt, sb.ToCString(), nPoint, pointArr, lyrs->priority, lyrs->lyr->GetLayerType(), lyrs->style, lyrs->bkColor, view, (UOSInt2Double(imgWidth) * view->GetHDPI() / view->GetDDPI()), (UOSInt2Double(imgHeight) * view->GetHDPI() / view->GetDDPI()));
					}
					break;
				}
				case Math::Geometry::Vector2D::VectorType::LineString:
				case Math::Geometry::Vector2D::VectorType::MultiPoint:
				case Math::Geometry::Vector2D::VectorType::MultiPolygon:
				case Math::Geometry::Vector2D::VectorType::GeometryCollection:
				case Math::Geometry::Vector2D::VectorType::CircularString:
				case Math::Geometry::Vector2D::VectorType::CompoundCurve:
				case Math::Geometry::Vector2D::VectorType::CurvePolygon:
				case Math::Geometry::Vector2D::VectorType::MultiCurve:
				case Math::Geometry::Vector2D::VectorType::MultiSurface:
				case Math::Geometry::Vector2D::VectorType::Curve:
				case Math::Geometry::Vector2D::VectorType::Surface:
				case Math::Geometry::Vector2D::VectorType::PolyhedralSurface:
				case Math::Geometry::Vector2D::VectorType::Tin:
				case Math::Geometry::Vector2D::VectorType::Triangle:
				case Math::Geometry::Vector2D::VectorType::LinearRing:
				case Math::Geometry::Vector2D::VectorType::Image:
				case Math::Geometry::Vector2D::VectorType::String:
				case Math::Geometry::Vector2D::VectorType::Ellipse:
				case Math::Geometry::Vector2D::VectorType::PieArea:
				case Math::Geometry::Vector2D::VectorType::Unknown:
				default:
					break;
				}
				DEL_CLASS(vec);
			}
			else
			{
				sb.ClearStr();
				lyrs->lyr->GetString(sb, arr, arri.GetItem(i), 0);
				switch (vec->GetVectorType())
				{
				case Math::Geometry::Vector2D::VectorType::Polyline:
				{
					Math::Geometry::Polyline *pl = (Math::Geometry::Polyline*)vec;
					NN<Math::Geometry::LineString> ls;
					UOSInt nPoint;
					if (pl->GetItem(pl->GetCount() >> 1).SetTo(ls))
					{
						Math::Coord2DDbl *pointArr = ls->GetPointList(nPoint);
						if (nPoint & 1)
						{
							UOSInt l = nPoint >> 1;
							pts = pointArr[l];

							scaleW = pointArr[l + 1].x - pointArr[l - 1].x;
							scaleH = pointArr[l + 1].y - pointArr[l - 1].y;
						}
						else
						{
							UOSInt l = nPoint >> 1;
							pts.x = (pointArr[l - 1].x + pointArr[l].x) * 0.5;
							pts.y = (pointArr[l - 1].y + pointArr[l].y) * 0.5;

							scaleW = pointArr[l].x - pointArr[l - 1].x;
							scaleH = pointArr[l].y - pointArr[l - 1].y;
						}

						if (view->InViewXY(pts))
						{
							pts = view->MapXYToScnXY(pts);

							if ((lyrs->bkColor & SFLG_ROTATE) == 0)
								scaleW = scaleH = 0;
							DrawChars(img, sb.ToCString(), pts, scaleW, scaleH, fonts[lyrs->style], (lyrs->bkColor & SFLG_ALIGN) != 0);
						}
					}
					break;
				}
				case Math::Geometry::Vector2D::VectorType::Polygon:
				{
					Math::Geometry::Polygon *pg = (Math::Geometry::Polygon*)vec;
					pts = pg->GetCenter();
					if (view->InViewXY(pts))
					{
						pts = view->MapXYToScnXY(pts);
						DrawChars(img, sb.ToCString(), pts, 0, 0, fonts[lyrs->style], (lyrs->bkColor & SFLG_ALIGN) != 0);
					}
					break;
				}
				case Math::Geometry::Vector2D::VectorType::Point:
				{
					pts = vec->GetCenter();
					if (view->InViewXY(pts))
					{
						pts = view->MapXYToScnXY(pts);
						DrawChars(img, sb.ToCString(), pts, 0, 0, fonts[lyrs->style], (lyrs->bkColor & SFLG_ALIGN) != 0);
					}
					break;
				}
				case Math::Geometry::Vector2D::VectorType::LineString:
				case Math::Geometry::Vector2D::VectorType::MultiPoint:
				case Math::Geometry::Vector2D::VectorType::MultiPolygon:
				case Math::Geometry::Vector2D::VectorType::GeometryCollection:
				case Math::Geometry::Vector2D::VectorType::CircularString:
				case Math::Geometry::Vector2D::VectorType::CompoundCurve:
				case Math::Geometry::Vector2D::VectorType::CurvePolygon:
				case Math::Geometry::Vector2D::VectorType::MultiCurve:
				case Math::Geometry::Vector2D::VectorType::MultiSurface:
				case Math::Geometry::Vector2D::VectorType::Curve:
				case Math::Geometry::Vector2D::VectorType::Surface:
				case Math::Geometry::Vector2D::VectorType::PolyhedralSurface:
				case Math::Geometry::Vector2D::VectorType::Tin:
				case Math::Geometry::Vector2D::VectorType::Triangle:
				case Math::Geometry::Vector2D::VectorType::LinearRing:
				case Math::Geometry::Vector2D::VectorType::Image:
				case Math::Geometry::Vector2D::VectorType::String:
				case Math::Geometry::Vector2D::VectorType::Ellipse:
				case Math::Geometry::Vector2D::VectorType::PieArea:
				case Math::Geometry::Vector2D::VectorType::Unknown:
				default:
					break;
				}
				DEL_CLASS(vec);
			}
		}
	}
	lyrs->lyr->EndGetObject(session);
	lyrs->lyr->ReleaseNameArr(arr);
}

UInt32 Map::MapConfig2::NewLabel(MapLabels2 *labels, UInt32 maxLabel, UInt32 *labelCnt, Int32 priority)
{
	Int32 minPriority;
	UInt32 i;
	UInt32 j;
	Double k;

	if (*labelCnt >= maxLabel)
	{
		i = *labelCnt;
		minPriority = labels[0].priority;
		j = (UInt32)-1;
		k = labels[0].totalSize;
		while (i--)
		{
			if (labels[i].priority < minPriority)
			{
				minPriority = labels[i].priority;
				j = i;
				k = labels[i].totalSize;
			}
			else if (labels[i].priority == minPriority)
			{
				if (labels[i].totalSize < k)
				{
					j = i;
					k = labels[i].totalSize;
				}
			}
		}
		if (j == (UInt32)-1)
			return j;
		if (labels[j].label)
			labels[j].label->Release();
		if (labels[j].points)
			MemFreeA(labels[j].points);
		labels[j].label = 0;
		labels[j].points = 0;
		labels[j].priority = priority;
		return j;
	}
	else
	{
		i = (*labelCnt)++;
		labels[i].priority = priority;
		labels[i].label = 0;
		labels[i].points = 0;
		return i;
	}
}


Bool Map::MapConfig2::AddLabel(MapLabels2 *labels, UInt32 maxLabel, UInt32 *labelCnt, Text::CString labelt, UOSInt nPoint, Math::Coord2DDbl *points, Int32 priority, Int32 recType, UInt32 fontStyle, UInt32 flags, NN<Map::MapView> view, Double xOfst, Double yOfst)
{
	Double size;
	Double visibleSize;

	UInt32 i;
	UOSInt j;

	Math::Coord2DDbl *ptPtr;

	Math::Coord2DDbl scnPos;
	Double scnSqrLen;
	Int32 found;

	Math::RectAreaDbl rect = view->GetVerticalRect();
	Double left = rect.min.x;
	Double right = rect.max.x;
	Double top = rect.min.y;
	Double bottom = rect.max.y;
	Math::Coord2DDbl mapPos = view->GetCenter();

	if (recType == 1) //Point
	{
/*		if (xOfst > 100 || xOfst < -100)
		{
			printf("Ofst Out of range: %d\n", xOfst);
		}*/
		found = 0;
		i = 0;
		while (i < *labelCnt)
		{
			if (recType == labels[i].shapeType)
			{
				if (labels[i].label->Equals(labelt))
				{
					found = 1;

					ptPtr = points;
					j = nPoint;
					while (j--)
					{
						scnPos = mapPos - *ptPtr;
						scnPos = scnPos * scnPos;
						scnSqrLen = scnPos.x + scnPos.y;
						if (scnSqrLen < labels[i].currSize)
						{
							labels[i].pos = ptPtr[0];
							labels[i].fontStyle = fontStyle;
							labels[i].scaleW = 0;
							labels[i].scaleH = 0;
							labels[i].currSize = scnSqrLen;
							labels[i].xOfst = xOfst;
							labels[i].yOfst = yOfst;
							if (priority > labels[i].priority)
								labels[i].priority = priority;
						}
						labels[i].totalSize++;
						ptPtr++;
					}
					return true;
				}
			}
			i++;
		}

		if (found == 0)
		{
			i = (UInt32)-1;
			ptPtr = points;
			j = nPoint;
			while (j--)
			{
				if (ptPtr[0].x >= left && ptPtr[0].x < right && ptPtr[0].y >= top && ptPtr[0].y < bottom)
				{
					found = 1;

					i = NewLabel(labels, maxLabel, labelCnt, priority);
					if (i == (UInt32)-1)
					{
						return false;
					}

					labels[i].label = Text::String::New(labelt).Ptr();
					labels[i].pos = ptPtr[0];
					labels[i].fontStyle = fontStyle;
					labels[i].scaleW = 0;
					labels[i].scaleH = 0;
					labels[i].priority = priority;

					scnPos = mapPos - ptPtr[0];
					scnPos = scnPos * scnPos;
					scnSqrLen = scnPos.x + scnPos.y;

					labels[i].currSize = scnSqrLen;
					labels[i].totalSize = UOSInt2Double(nPoint);
					labels[i].nPoints = 0;
					labels[i].shapeType = 1;
					labels[i].points = 0;
					labels[i].flags = flags;
					labels[i].xOfst = xOfst;
					labels[i].yOfst = yOfst;

					ptPtr = points;
					j = nPoint;
					while (j--)
					{
						scnPos = mapPos - ptPtr[0];
						scnPos = scnPos * scnPos;
						scnSqrLen = scnPos.x + scnPos.y;
						if (scnSqrLen < labels[i].currSize)
						{
							labels[i].pos = ptPtr[0];
							labels[i].fontStyle = fontStyle;
							labels[i].scaleW = 0;
							labels[i].scaleH = 0;
							labels[i].currSize = scnSqrLen;
							if (priority > labels[i].priority)
								labels[i].priority = priority;
						}
						ptPtr++;
					}

					return true;
				}
				ptPtr += 2;
			}
		}
		return false;
	}
	else if (recType == 3) //lines
	{
		Double tmp;
		Math::Coord2DDbl lastPt = points[0];
		Math::Coord2DDbl thisPt;

		Int32 toUpdate;

		visibleSize = 0;
		size = 0;

		i = 1;
		while (i < nPoint)
		{
			lastPt = points[i - 1];
			thisPt = points[i];
			if (lastPt.x > thisPt.x)
			{
				tmp = lastPt.x;
				lastPt.x = thisPt.x;
				thisPt.x = tmp;
			}
			if (lastPt.y > thisPt.y)
			{
				tmp = lastPt.y;
				lastPt.y = thisPt.y;
				thisPt.y = tmp;
			}

			if ((thisPt.y - lastPt.y) > (thisPt.x - lastPt.x))
				size += thisPt.y - lastPt.y;
			else
				size += thisPt.x - lastPt.x;

			if (left < thisPt.x && right > lastPt.x && top < thisPt.y && bottom > lastPt.y)
			{
				if (left > lastPt.x)
				{
					lastPt.y += (left - lastPt.x) * (thisPt.y - lastPt.y) / (thisPt.x - lastPt.x);
					lastPt.x = left;
				}
				if (top > lastPt.y)
				{
					lastPt.x += (top - lastPt.y) * (thisPt.x - lastPt.x) / (thisPt.y - lastPt.y);
					lastPt.y = top;
				}
				if (right < thisPt.x)
				{
					thisPt.y += (right - lastPt.x) * (thisPt.y - lastPt.y) / (thisPt.x - lastPt.x);
					thisPt.x = right;
				}
				if (bottom < thisPt.y)
				{
					thisPt.x += (bottom - lastPt.y) * (thisPt.x - lastPt.x) / (thisPt.y - lastPt.y);
					thisPt.y = bottom;
				}

				if ((thisPt.y - lastPt.y) > (thisPt.x - lastPt.x))
					visibleSize += thisPt.y - lastPt.y;
				else
					visibleSize += thisPt.x - lastPt.x;

			}
			i++;
		}

		toUpdate = 0;
		found = 0;
		Double totalSize = 0;
		UInt32 foundInd = 0;
		Math::Coord2DDbl startPt;
		Math::Coord2DDbl endPt;
		startPt = points[0];
		endPt = points[nPoint - 1];
		i = 0;

		while (i < *labelCnt)
		{
			if (recType == labels[i].shapeType)
			{
				if (labels[i].label->Equals(labelt))
				{
					found++;
					if (totalSize == 0)
					{
						labels[i].totalSize += size;
						totalSize = labels[i].totalSize;
					}
					else
					{
						labels[i].totalSize = totalSize;
					}


					if (priority > labels[i].priority)
						labels[i].priority = priority;
					else
						priority = labels[i].priority;

					if (found >= 2)
					{

					}
					else if (labels[i].points[0] == endPt)
					{
//						wprintf(L"Shape: %s merged (%d + %d)\n", labelt, labels[i].nPoints, nPoint);
						UOSInt newSize = labels[i].nPoints + nPoint - 1;
						Math::Coord2DDbl* newArr = MemAllocA(Math::Coord2DDbl, newSize);

						MemCopyNO(newArr, points, nPoint << 4);
						MemCopyNO(&newArr[nPoint], &labels[i].points[1], (labels[i].nPoints - 1) << 4);

						startPt = newArr[0];
						endPt = newArr[newSize - 1];

						MemFreeA(labels[i].points);
						labels[i].points = newArr;
						labels[i].nPoints = newSize;
						labels[i].currSize += visibleSize;
						toUpdate = 0;
						foundInd = i;
					}
					else if (labels[i].points[labels[i].nPoints - 1] == startPt)
					{
//						wprintf(L"Shape: %s merged (%d + %d)\n", labelt, labels[i].nPoints, nPoint);
						UOSInt newSize = labels[i].nPoints + nPoint - 1;
						Math::Coord2DDbl* newArr = MemAllocA(Math::Coord2DDbl, newSize);

						MemCopyNO(newArr, labels[i].points, labels[i].nPoints << 4);
						MemCopyNO(&newArr[labels[i].nPoints], &points[1], (nPoint - 1) << 4);

						startPt = newArr[0];
						endPt = newArr[newSize - 1];

						MemFreeA(labels[i].points);
						labels[i].points = newArr;
						labels[i].nPoints = newSize;
						labels[i].currSize += visibleSize;
						toUpdate = 0;
						foundInd = i;
					}
					else if (labels[i].points[0] == startPt)
					{
//						wprintf(L"Shape: %s inverse merged (%d + %d)\n", labelt, labels[i].nPoints, nPoint);
						UOSInt newSize = labels[i].nPoints + nPoint - 1;
						Math::Coord2DDbl* newArr = MemAllocA(Math::Coord2DDbl, newSize << 1);
						UOSInt k;
						Int32 l;
						l = 0;
						k = labels[i].nPoints;
						while (k-- > 1)
						{
							newArr[l++] = labels[i].points[k << 1];
							newArr[l++] = labels[i].points[(k << 1) + 1];
						}
						MemCopyNO(&newArr[l], points, nPoint << 4);

						startPt = newArr[0];
						endPt = newArr[newSize - 1];

						MemFreeA(labels[i].points);
						labels[i].points = newArr;
						labels[i].nPoints = newSize;
						labels[i].currSize += visibleSize;
						toUpdate = 0;
						foundInd = i;
					}
					else if (labels[i].points[labels[i].nPoints - 1] == endPt)
					{
//						wprintf(L"Shape: %s inverse merged (%d + %d)\n", labelt, labels[i].nPoints, nPoint);
						UOSInt newSize = labels[i].nPoints + nPoint - 1;
						Math::Coord2DDbl* newArr = MemAllocA(Math::Coord2DDbl, newSize);
						MemCopyNO(newArr, labels[i].points, labels[i].nPoints << 4);
						UOSInt k;
						UOSInt l;
						l = labels[i].nPoints;
						k = nPoint - 1;
						while (k-- > 0)
						{
							newArr[l++] = points[k];
						}
						startPt = newArr[0];
						endPt = newArr[newSize - 1];
						MemFreeA(labels[i].points);
						labels[i].points = newArr;
						labels[i].nPoints = newSize;
						labels[i].currSize += visibleSize;
						toUpdate = 0;
						foundInd = i;
					}
					else
					{
						if (labels[i].currSize < visibleSize)
						{
							toUpdate = 1;
							foundInd = i;
						}

						found = 0;
					}
				}
			}
			i++;
		}
		if (found == 0 && visibleSize > 0)
		{
//			wprintf(L"New Label: %s, nPoint = %d\n", labelt, nPoint);
			i = NewLabel(labels, maxLabel, labelCnt, priority);

			if (i < 0)
				return false;

			labels[i].shapeType = 3;
			labels[i].fontStyle = fontStyle;
			labels[i].totalSize = size;
			labels[i].currSize = visibleSize;
			labels[i].flags = flags;

			labels[i].label = Text::String::New(labelt).Ptr();
			labels[i].points = 0;

			toUpdate = 1;
		}
		else if (toUpdate)
		{
//			wprintf(L"Update Label: %s, nPoint: %d > %d\n", labelt, labels[i].nPoints, nPoint);
			i = foundInd;
		}

		if (toUpdate)
		{
			j = labels[i].nPoints = nPoint;
			if (labels[i].points)
				MemFreeA(labels[i].points);
			labels[i].points = ptPtr = MemAllocA(Math::Coord2DDbl, nPoint);
			MemCopyNO(ptPtr, points, j << 4);
			return true;
		}
		return false;
	}
	else if (recType == 5) //polygon
	{
		if (nPoint <= 2)
			return false;

		found = 0;

		if (IsDoorNum(labelt.v))
		{
		}
		else
		{
			i = 0;
			while (i < *labelCnt)
			{
				if (recType == labels[i].shapeType)
				{
					if (labels[i].label->Equals(labelt))
					{
						found = 1;
						break;
					}
				}
				i++;
			}
		}

		if (found == 0)
		{
			Math::Coord2DDbl lastPt;
			Math::Coord2DDbl thisPt;
			Math::Coord2DDbl thisT;
			Math::Coord2DDbl* outPts;
			UOSInt outPtCnt;
			Double sum;
			Math::Coord2DDbl sumVal;

			Math::Coord2DDbl *tmpPts;
			tmpPts = MemAllocA(Math::Coord2DDbl, nPoint << 1);
			outPtCnt = Math::GeometryTool::BoundPolygonY(points, nPoint, tmpPts, top, bottom, {0, 0});
			outPts = MemAllocA(Math::Coord2DDbl, nPoint << 1);
			outPtCnt = Math::GeometryTool::BoundPolygonX(tmpPts, outPtCnt, outPts, left, right, {0, 0});
			MemFreeA(tmpPts);

			i = 0;
			sum = 0;
			sumVal = {0, 0};
			lastPt = outPts[outPtCnt - 1];
			while (i < outPtCnt)
			{
				thisPt = outPts[i];

				sum += (lastPt.x * thisPt.y) - (lastPt.y * thisPt.x);

				lastPt = thisPt;
				i++;
			}
			if (sum != 0)
			{
				Math::Coord2DDbl *finalPts;
				UInt32 finalCnt;
				Math::Coord2DDbl max;
				Math::Coord2DDbl min;
				finalCnt = 0;
				finalPts = MemAllocA(Math::Coord2DDbl, outPtCnt);
				sumVal += max = min = lastPt = finalPts[0] = outPts[0];
				finalCnt++;

				i = 2;
				while (i < outPtCnt)
				{
					thisPt = outPts[i - 1];
					if ((outPts[i].x - lastPt.x) * (lastPt.y - thisPt.y) == (outPts[i].y - lastPt.y) * (lastPt.x - thisPt.x))
					{

					}
					else
					{
						sumVal += finalPts[finalCnt] = thisPt;
						max = max.Max(thisPt);
						min = min.Min(thisPt);
						finalCnt++;
					}
					lastPt = thisPt;
					i++;
				}

				thisPt = outPts[outPtCnt - 1];
				sumVal += finalPts[finalCnt] = thisPt;
				finalCnt++;
				max = max.Max(thisPt);
				min = min.Min(thisPt);

				lastPt = thisPt;
				sum = 0;
				thisPt.y = (max.y + min.y) * 0.5;
				i = 0;
				while (i < finalCnt)
				{
					thisT = finalPts[i];
					if ((lastPt.y >= thisPt.y && thisT.y < thisPt.y) || (thisT.y >= thisPt.y && lastPt.y < thisPt.y))
					{
						thisPt.y = lastPt.y + (thisPt.y - lastPt.y) * (thisT.x - lastPt.x) / (thisT.y - lastPt.y);
						if (sum == 0)
						{
							min.x = thisPt.x;
							max.x = thisPt.x;
						}
						else
						{
							if (thisPt.x > max.x)
								max.x = thisPt.x;
							if (thisPt.x < min.x)
								min.x = thisPt.x;
						}
						sum = 1;
					}
					lastPt = thisT;
					i++;
				}


				MemFreeA(outPts);
				outPts = finalPts;
				outPtCnt = finalCnt;

				thisPt = (max + min) * 0.5;

				i = NewLabel(labels, maxLabel, labelCnt, priority);
				if (i < 0)
				{
					MemFreeA(outPts);
					return false;
				}

				labels[i].label = Text::String::New(labelt).Ptr();
				labels[i].pos = thisPt;
				labels[i].fontStyle = fontStyle;
				labels[i].scaleW = 0;
				labels[i].scaleH = 0;
				labels[i].priority = priority;

				labels[i].currSize = UOSInt2Double(outPtCnt);
				labels[i].totalSize = UOSInt2Double(outPtCnt);
				labels[i].nPoints = outPtCnt;
				labels[i].shapeType = 5;
				if (labels[i].points)
					MemFreeA(labels[i].points);
				labels[i].points = outPts;
				labels[i].flags = flags;
				return true;
			}
			else
			{
				MemFreeA(outPts);
			}
		}
		return false;
	}
	return false;
}

void Map::MapConfig2::SwapLabel(MapLabels2 *mapLabels, UInt32 index, UInt32 index2)
{
	MapLabels2 l;
	l.label = mapLabels[index].label;
	l.pos = mapLabels[index].pos;
	l.fontStyle = mapLabels[index].fontStyle;
	l.scaleW = mapLabels[index].scaleW;
	l.scaleH = mapLabels[index].scaleH;
	l.priority = mapLabels[index].priority;
	l.totalSize = mapLabels[index].totalSize;
	l.currSize = mapLabels[index].currSize;
	l.mapRate = mapLabels[index].mapRate;
	l.nPoints = mapLabels[index].nPoints;
	l.shapeType = mapLabels[index].shapeType;
	l.points = mapLabels[index].points;
	l.flags = mapLabels[index].flags;
	l.xOfst = mapLabels[index].xOfst;
	l.yOfst = mapLabels[index].yOfst;

	mapLabels[index].label = mapLabels[index2].label;
	mapLabels[index].pos = mapLabels[index2].pos;
	mapLabels[index].fontStyle = mapLabels[index2].fontStyle;
	mapLabels[index].scaleW = mapLabels[index2].scaleW;
	mapLabels[index].scaleH = mapLabels[index2].scaleH;
	mapLabels[index].priority = mapLabels[index2].priority;
	mapLabels[index].totalSize = mapLabels[index2].totalSize;
	mapLabels[index].currSize = mapLabels[index2].currSize;
	mapLabels[index].mapRate = mapLabels[index2].mapRate;
	mapLabels[index].nPoints = mapLabels[index2].nPoints;
	mapLabels[index].shapeType = mapLabels[index2].shapeType;
	mapLabels[index].points = mapLabels[index2].points;
	mapLabels[index].flags = mapLabels[index2].flags;
	mapLabels[index].xOfst = mapLabels[index2].xOfst;
	mapLabels[index].yOfst = mapLabels[index2].yOfst;

	mapLabels[index2].label = l.label;
	mapLabels[index2].pos = l.pos;
	mapLabels[index2].fontStyle = l.fontStyle;
	mapLabels[index2].scaleW = l.scaleW;
	mapLabels[index2].scaleH = l.scaleH;
	mapLabels[index2].priority = l.priority;
	mapLabels[index2].totalSize = l.totalSize;
	mapLabels[index2].currSize = l.currSize;
	mapLabels[index2].mapRate = l.mapRate;
	mapLabels[index2].nPoints = l.nPoints;
	mapLabels[index2].shapeType = l.shapeType;
	mapLabels[index2].points = l.points;
	mapLabels[index2].flags = l.flags;
	mapLabels[index2].xOfst = l.xOfst;
	mapLabels[index2].yOfst = l.yOfst;
}

Bool Map::MapConfig2::LabelOverlapped(Math::RectAreaDbl *points, UOSInt nPoints, Math::RectAreaDbl rect)
{
	while (nPoints--)
	{
		if (rect.OverlapOrTouch(points[nPoints])) return true;
	}

	return false;
}

void Map::MapConfig2::DrawLabels(NN<Media::DrawImage> img, MapLabels2 *labels, UInt32 maxLabel, UInt32 *labelCnt, NN<Map::MapView> view, Data::ArrayList<MapFontStyle*> **fonts, NN<Media::DrawEngine> drawEng, Math::RectAreaDbl *objBounds, UOSInt *objCnt)
{
	UInt32 i;
	UOSInt j;
	Text::String *lastLbl = 0;
	UOSInt scnWidth = img->GetWidth();
	UOSInt scnHeight = img->GetHeight();

	if (*labelCnt)
	{
		Math::Coord2DDbl szThis;
		UOSInt currPt;

		Math::RectAreaDbl rect;
		Math::Coord2DDbl scnPt;
		Double thisPts[10];
		UInt32 thisCnt = 0;

		i = 0;
		j = *labelCnt;
		while (j--)
		{
			currPt = 0;
			i = 0;
			while (i < j)
			{
				if (labels[i].priority < labels[i + 1].priority)
				{
					SwapLabel(labels, i, i + 1);
					currPt = 1;
				}
				else if (labels[i].priority == labels[i + 1].priority)
				{
					if (labels[i].totalSize < labels[i + 1].totalSize)
					{
						SwapLabel(labels, i, i + 1);
						currPt = 1;
					}
				}
				i++;
			}

			if (currPt == 0)
			{
				break;
			}
		}

		currPt = *objCnt;
		i = 0;
		while (i < *labelCnt)
		{

		//	labels[i].shapeType = 0;
			if (labels[i].shapeType == 1)
			{
				GetCharsSize(img, szThis, labels[i].label->ToCString(), fonts[labels[i].fontStyle], 0, 0);//labels[i].scaleW, labels[i].scaleH);
				Math::Coord2DDbl scnD = view->MapXYToScnXY(labels[i].pos / labels[i].mapRate);
				scnPt = scnD;

				Bool valid = true;
				if (labels[i].xOfst == 0)
				{
					rect.min = scnPt - (szThis * 0.5);
					rect.max = rect.min + szThis;

					valid = LabelOverlapped(objBounds, currPt, rect);
				}
				if (valid)
				{
					rect.min.x = scnPt.x + 1 + (labels[i].xOfst * 0.5);
					rect.min.y = scnPt.y - (szThis.y * 0.5);
					rect.max = rect.min + szThis;

					valid = LabelOverlapped(objBounds, currPt, rect);
				}
				if (valid)
				{
					rect.min.x = scnPt.x - szThis.x - 1 - (labels[i].xOfst * 0.5);
					rect.min.y = scnPt.y - (szThis.y * 0.5);
					rect.max = rect.min + szThis;

					valid = LabelOverlapped(objBounds, currPt, rect);
				}
				if (valid)
				{
					rect.min.x = scnPt.x - (szThis.x * 0.5);
					rect.min.y = scnPt.y - szThis.y - 1 - (labels[i].yOfst * 0.5);
					rect.max = rect.min + szThis;

					valid = LabelOverlapped(objBounds, currPt, rect);
				}
				if (valid)
				{
					rect.min.x = scnPt.x - (szThis.x * 0.5);
					rect.min.y = scnPt.y + 1 + (labels[i].yOfst * 0.5);
					rect.max = rect.min + szThis;

					valid = LabelOverlapped(objBounds, currPt, rect);
				}

				if (!valid)
				{
					Math::Coord2DDbl center = rect.GetCenter();
					DrawChars(img, labels[i].label->ToCString(), center, 0, 0, fonts[labels[i].fontStyle], 0);

					objBounds[currPt] = rect;
					currPt++;
				}
			}
			else if (labels[i].shapeType == 3)
			{
//				wprintf(L"Label: %s, nPoints: %d\n", labels[i].label, labels[i].nPoints);

				if (lastLbl)
				{
					if (!Text::StringTool::Equals(lastLbl, labels[i].label))
					{
						thisCnt = 0;
					}
				}
				else
				{
					thisCnt = 0;
				}

				if (thisCnt == 0)
				{
					if (lastLbl)
						lastLbl->Release();
					lastLbl = labels[i].label->Clone().Ptr();
				}
				else
				{
				//	MessageBoxW(NULL, L"Test", lastLbl, MB_OK);
				}

				Math::Coord2D<Int32> *points = MemAlloc(Math::Coord2D<Int32>, labels[i].nPoints);
				view->MapXYToScnXY(labels[i].points, points, labels[i].nPoints, Math::Coord2D<Int32>(0, 0));
				OSInt minX = 0;
				OSInt minY = 0;
				OSInt maxX = 0;
				OSInt maxY = 0;
				Math::Coord2D<OSInt> diff;
				Double scaleN;
				Double scaleD;
				OSInt lastX;
				OSInt lastY;
				OSInt thisX;
				OSInt thisY;
				UInt32 k;
				Bool hasPoint;

				j = labels[i].nPoints - 1;
				lastX = points[j].x;
				lastY = points[j].y;
				if (lastX >= 0 && lastX < (OSInt)scnWidth && lastY >= 0 && lastY < (OSInt)scnHeight)
				{
					maxX = minX = lastX;
					maxY = minY = lastY;
					hasPoint = true;
				}
				else
				{
					hasPoint = false;
				}
				while (j > 0)
				{
					lastX = points[j].x;
					lastY = points[j].y;

					j -= 1;

					thisX = points[j].x;
					thisY = points[j].y;

					if (lastX > (OSInt)scnWidth)
					{
						if (thisX > (OSInt)scnWidth)
						{
							continue;
						}
						else
						{
							lastY = thisY + MulDivOS(lastY - thisY, (OSInt)scnWidth - thisX, lastX - thisX);
							lastX = (OSInt)scnWidth;
						}
					}
					else if (lastX < 0)
					{
						if (thisX < 0)
						{
							continue;
						}
						else
						{
							lastY = thisY + MulDivOS(lastY - thisY, 0 - thisX, lastX - thisX);
							lastX = 0;
						}
					}

					if (thisX < 0)
					{
						thisY = lastY + MulDivOS(thisY - lastY, 0 - lastX, thisX - lastX);
						thisX = 0;
					}
					else if (thisX > (OSInt)scnWidth)
					{
						thisY = lastY + MulDivOS(thisY - lastY, (OSInt)scnWidth - lastX, thisX - lastX);
						thisX = (OSInt)scnWidth;
					}


					if (lastY > (OSInt)scnHeight)
					{
						if (thisY > (OSInt)scnHeight)
						{
							continue;
						}
						else
						{
							lastX = thisX + MulDivOS(lastX - thisX, (OSInt)scnHeight - thisY, lastY - thisY);
							lastY = (OSInt)scnHeight;
						}
					}
					else if (lastY < 0)
					{
						if (thisY < 0)
						{
							continue;
						}
						else
						{
							lastX = thisX + MulDivOS(lastX - thisX, 0 - thisY, lastY - thisY);
							lastY = 0;
						}
					}

					if (thisY < 0)
					{
						thisX = lastX + MulDivOS(thisX - lastX, 0 - lastY, thisY - lastY);
						thisY = 0;
					}
					else if (thisY > (OSInt)scnHeight)
					{
						thisX = lastX + MulDivOS(thisX - lastX, (OSInt)scnHeight - lastY, thisY - lastY);
						thisY = (OSInt)scnHeight;
					}

					if (!hasPoint)
					{
						minX = maxX = lastX;
						minY = maxY = lastY;
					}
					hasPoint = true;
					if (minX > lastX)
						minX = lastX;
					if (maxX < lastX)
						maxX = lastX;
					if (minY > lastY)
						minY = lastY;
					if (maxY < lastY)
						maxY = lastY;

					if (minX > thisX)
						minX = thisX;
					if (maxX < thisX)
						maxX = thisX;
					if (minY > thisY)
						minY = thisY;
					if (maxY < thisY)
						maxY = thisY;

				}
				if (!hasPoint)
				{
					j = (labels[i].nPoints - 1) >> 1;
					minX = maxX = points[j].x;
					minY = maxY = points[j].y;
					if (points[j + 1].x > minX)
					{
						maxX = points[j + 1].x;
					}
					else
					{
						minX = points[j + 1].x;
					}
					if (points[j + 1].y > minY)
					{
						maxY = points[j + 1].y;
					}
					else
					{
						minY = points[j + 1].y;
					}
				}
				diff.x = maxX - minX;
				diff.y = maxY - minY;
				scaleN = 0;
				scaleD = 1;

				if (minX > maxX || minY > maxY)
				{
//					wprintf(L"Error in handling label: %s\n", lastLbl);
				}
				if (diff.x > diff.y)
				{
					scnPt.x = OSInt2Double((maxX + minX) >> 1);
					k = 0;
					while (k < labels[i].nPoints - 1)
					{
						if (points[k].x >= scnPt.x && points[k + 1].x <= scnPt.x)
						{
							scaleD = points[k].x - points[k + 1].x;
							scaleN = (points[k].x - scnPt.x);
							break;
						}
						else if (points[k].x <= scnPt.x && points[k + 1].x >= scnPt.x)
						{
							scaleD = points[k + 1].x - points[k].x;
							scaleN = scnPt.x - points[k].x;
							break;
						}
						k++;
					}

				}
				else
				{
					scnPt.y = OSInt2Double((maxY + minY) >> 1);
					k = 0;
					while (k < labels[i].nPoints - 1)
					{
						if (points[k].y >= scnPt.y && points[k + 1].y <= scnPt.y)
						{
							scaleD = points[k].y - points[k + 1].y;
							scaleN = (points[k].y - scnPt.y);
							break;
						}
						else if (points[k].y <= scnPt.y && points[k + 1].y >= scnPt.y)
						{
							scaleD = points[k + 1].y - points[k].y;
							scaleN = scnPt.y - points[k].y;
							break;
						}
						k++;
					}
				}
				scnPt = points[k].ToDouble() + (points[k + 1].ToDouble() - points[k].ToDouble()) * scaleN / scaleD;
				if (labels[i].flags & SFLG_ROTATE)
				{
					labels[i].scaleW = labels[i].points[k + 1].x - labels[i].points[k].x;
					labels[i].scaleH = labels[i].points[k + 1].y - labels[i].points[k].y;
				}
				else
				{
					labels[i].scaleW = 0;
					labels[i].scaleH = 0;
				}
				GetCharsSize(img, szThis, labels[i].label->ToCString(), fonts[labels[i].fontStyle], labels[i].scaleW, labels[i].scaleH);
				if (OSInt2Double(diff.x) < szThis.x && OSInt2Double(diff.y) < szThis.y)
				{
					rect.min = scnPt - (szThis * 0.5);
					rect.max = rect.min + szThis;

					j = LabelOverlapped(objBounds, currPt, rect);
					if (j == 0)
					{
						Math::Coord2DDbl center = rect.GetCenter();
						DrawChars(img, labels[i].label->ToCString(), center, labels[i].scaleW, labels[i].scaleH, fonts[labels[i].fontStyle], (labels[i].flags & SFLG_ALIGN) != 0);

						objBounds[currPt] = rect;
						currPt++;
					}
				}
				else
				{
					OSInt scnDiff = (maxX - minX) >> 1;
					Int32 tryCnt = 50;
					j = 1;
					while (j)
					{
						rect.min = scnPt - (szThis * 0.5);
						rect.max = rect.min + szThis;

						j = LabelOverlapped(objBounds, currPt, rect);
						if (j == 0 || --tryCnt <= 0)
						{
							break;
						}
						else
						{
							/////////////////////////////////////
							if (diff.x > diff.y)
							{
								if (scnDiff < 0)
								{
									scnPt.x += OSInt2Double(scnDiff) - 1;
									if (scnPt.x <= OSInt2Double(minX))
									{
										scnDiff = -scnDiff;
										scnPt.x = OSInt2Double(((minX + maxX) >> 1) + scnDiff + 1);
									}
								}
								else
								{
									scnPt.x += OSInt2Double(scnDiff + 1);
									if (scnPt.x >= OSInt2Double(maxX))
									{
										scnDiff = scnDiff >> 1;
										if (scnDiff < 30)
											break;
										scnDiff = -scnDiff;
										scnPt.x = OSInt2Double(((minX + maxX) >> 1) + scnDiff - 1);
									}
								}

								k = 0;
								while (k < labels[i].nPoints - 1)
								{
									if (points[k].x >= scnPt.x && points[k + 1].x <= scnPt.x)
									{
										scaleD = points[k].x - points[k + 1].x;
										scaleN = (points[k].x - scnPt.x);
										break;
									}
									else if (points[k].x <= scnPt.x && points[k + 1].x >= scnPt.x)
									{
										scaleD = points[k + 1].x - points[k].x;
										scaleN = scnPt.x - points[k].x;
										break;
									}
									k++;
								}

							}
							else
							{
								if (scnDiff < 0)
								{
									scnPt.y += OSInt2Double(scnDiff - 1);
									if (scnPt.y <= OSInt2Double(minY))
									{
										scnDiff = -scnDiff;
										scnPt.y = OSInt2Double(((minY + maxY) >> 1) + scnDiff + 1);
									}
								}
								else
								{
									scnPt.y += OSInt2Double(scnDiff + 1);
									if (scnPt.y >= OSInt2Double(maxY))
									{
										scnDiff = scnDiff >> 1;
										if (scnDiff < 30)
											break;
										scnDiff = -scnDiff;
										scnPt.y = OSInt2Double(((minY + maxY) >> 1) + scnDiff - 1);
									}
								}

								k = 0;
								while (k < labels[i].nPoints - 1)
								{
									if (points[k].y >= scnPt.y && points[k + 1].y <= scnPt.y)
									{
										scaleD = points[k].y - points[k + 1].y;
										scaleN = (points[k].y - scnPt.y);
										break;
									}
									else if (points[k].y <= scnPt.y && points[k + 1].y >= scnPt.y)
									{
										scaleD = points[k + 1].y - points[k].y;
										scaleN = scnPt.y - points[k].y;
										break;
									}
									k++;
								}
							}
							scnPt = points[k].ToDouble() + (points[k + 1].ToDouble() - points[k].ToDouble()) * scaleN / scaleD;
							if (labels[i].flags & SFLG_ROTATE)
							{
								labels[i].scaleW = labels[i].points[k + 1].x - labels[i].points[k].x;
								labels[i].scaleH = labels[i].points[k + 1].y - labels[i].points[k].y;
							}
							else
							{
								labels[i].scaleW = 0;
								labels[i].scaleH = 0;
							}
							GetCharsSize(img, szThis, labels[i].label->ToCString(), fonts[labels[i].fontStyle], labels[i].scaleW, labels[i].scaleH);
						}
					}

					if (!j && thisCnt < 10)
					{
						UInt32 m;
						UInt32 n = 1;
						Double tmpV;
						m = thisCnt;
						while (n && m > 0)
						{
							n = 0;
							tmpV = thisPts[--m];
							if ((tmpV - LBLMINDIST) < rect.max.y && (tmpV + LBLMINDIST) > rect.min.y)
							{
								n++;
							}
							tmpV = thisPts[--m];
							if ((tmpV - LBLMINDIST) < rect.max.x && (tmpV + LBLMINDIST) > rect.min.x)
							{
								n++;
							}
							n = (n != 2);
						}

						if (n)
						{
							Math::Coord2DDbl center = rect.GetCenter();
							if ((labels[i].flags & SFLG_ALIGN) != 0)
							{
								Math::RectAreaDbl realBounds;
								DrawCharsLA(img, labels[i].label->ToCString(), labels[i].points, points, labels[i].nPoints, k, scaleN, scaleD, fonts[labels[i].fontStyle], &realBounds);

								objBounds[currPt] = realBounds;
								currPt++;
							}
							else if ((labels[i].flags & SFLG_ROTATE) != 0)
							{
								Math::RectAreaDbl realBounds;
								DrawCharsL(img, labels[i].label->ToCString(), labels[i].points, points, labels[i].nPoints, k, scaleN, scaleD, fonts[labels[i].fontStyle], &realBounds);

								objBounds[currPt] = realBounds;
								currPt++;
							}
							else
							{
								DrawChars(img, labels[i].label->ToCString(), center, labels[i].scaleW, labels[i].scaleH, fonts[labels[i].fontStyle], (labels[i].flags & SFLG_ALIGN) != 0);

								objBounds[currPt] = rect;
								currPt++;
							}

							thisPts[thisCnt++] = center.x;
							thisPts[thisCnt++] = center.y;
						}
						j = (thisCnt < 10);
					}
				}
				MemFree(points);
			}
			else if (labels[i].shapeType == 5)
			{
				GetCharsSize(img, szThis, labels[i].label->ToCString(), fonts[labels[i].fontStyle], 0, 0);//labels[i].scaleW, labels[i].scaleH);
				Math::Coord2DDbl scnD = view->MapXYToScnXY(labels[i].pos / labels[i].mapRate);
				scnPt = scnD;

				Bool valid = true;
				if (valid)
				{
					rect.min = scnPt - (szThis * 0.5);
					rect.max = rect.min + szThis;

					valid = LabelOverlapped(objBounds, currPt, rect);
				}
				if (valid)
				{
					rect.min.x = scnPt.x + 1;
					rect.min.y = scnPt.y - (szThis.y * 0.5);
					rect.max = rect.min + szThis;

					valid = LabelOverlapped(objBounds, currPt, rect);
				}
				if (valid)
				{
					rect.min.x = scnPt.x - szThis.x - 1;
					rect.min.y = scnPt.y - (szThis.y * 0.5);
					rect.max = rect.min + szThis;

					valid = LabelOverlapped(objBounds, currPt, rect);
				}
				if (valid)
				{
					rect.min.x = scnPt.x - (szThis.x * 0.5);
					rect.min.y = scnPt.y - szThis.y - 1;
					rect.max = rect.min + szThis;

					valid = LabelOverlapped(objBounds, currPt, rect);
				}
				if (valid)
				{
					rect.min.x = scnPt.x - (szThis.x * 0.5);
					rect.min.y = scnPt.y + 1;
					rect.max = rect.min + szThis;

					valid = LabelOverlapped(objBounds, currPt, rect);
				}

				if (!valid)
				{
					Math::Coord2DDbl center = rect.GetCenter();
					DrawChars(img, labels[i].label->ToCString(), center, 0, 0, fonts[labels[i].fontStyle], 0);

					objBounds[currPt] = rect;
					currPt++;
				}
			}
			else
			{

			}

			i++;
		}

		*objCnt = currPt;
	}

	i = *labelCnt;
	while (i-- > 0)
	{
		labels[i].label->Release();
		if (labels[i].points)
			MemFreeA(labels[i].points);
	}
	if (lastLbl)
		lastLbl->Release();
}

Map::MapConfig2::MapConfig2(Text::CStringNN fileName, NN<Media::DrawEngine> eng, NN<Data::ArrayListNN<Map::MapDrawLayer>> layerList, Parser::ParserList *parserList, Text::CString forceBase, NN<IO::Writer> errWriter, Int32 maxScale, Int32 minScale)
{
	UTF8Char lineBuff[1024];
	UTF8Char layerName[512];
	UTF8Char *layerNameEnd;
	UTF8Char *baseDir = layerName;
	Text::PString strs[10];
	UTF8Char *sptr;
	UOSInt i;
	UOSInt j;
	MapLineStyle *currLine;
	MapFontStyle *currFont;
	MapLayerStyle *currLayer;
	MapLayerStyle *currLayer2;
	Data::ArrayList<MapLayerStyle*> poiArr;

	this->drawEng = eng;
	this->inited = false;
	this->lines = 0;
	this->fonts = 0;
	this->drawList = 0;

	IO::FileStream fstm(fileName, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	if (fstm.IsError())
	{
		this->bgColor = 0;
		this->nLine = 0;
		this->nFont = 0;
		this->nStr = 0;
		this->lines = 0;
		this->fonts = 0;
		this->drawList = 0;
	}
	else
	{
		IO::StreamReader rdr(fstm);
		if (forceBase.leng > 0)
		{
			baseDir = forceBase.ConcatTo(layerName);
		}
		while ((sptr = rdr.ReadLine(lineBuff, 1023)) != 0)
		{
			UOSInt strCnt;
			Int32 lyrType;
			strCnt = Text::StrSplitTrimP(strs, 10, {lineBuff, (UOSInt)(sptr - lineBuff)}, ',');

			lyrType = Text::StrToInt32(strs[0].v);
			switch (lyrType)
			{
			case 1:
				this->bgColor = ToColor(strs[1].v);
				this->nLine = Text::StrToUInt32(strs[2].v);
				this->nFont = Text::StrToUInt32(strs[3].v);
				this->nStr = Text::StrToUInt32(strs[4].v);
				this->lines = MemAlloc(Data::ArrayList<MapLineStyle*>*, this->nLine);
				this->fonts = MemAlloc(Data::ArrayList<MapFontStyle*>*, this->nFont);
				i = this->nLine;
				while (i-- > 0)
				{
					this->lines[i] = 0;
				}
				i = this->nFont;
				while (i-- > 0)
				{
					this->fonts[i] = 0;
				}
				NEW_CLASS(this->drawList, Data::ArrayList<MapLayerStyle*>());

				this->inited = true;
				break;
			case 2:
				if (forceBase.leng == 0)
				{
					baseDir = fileName.ConcatTo(layerName);
					baseDir = IO::Path::AppendPath(layerName, baseDir, strs[1].ToCString());
//					baseDir = Text::StrConcat(layerName, strs[1]);
				}
				break;
			case 3:
				i = Text::StrToUInt32(strs[1].v);
				if (i >= this->nLine)
				{
					PrintDebug(L"Error found in MapLayer files, line id too large\r\n");
					break;
				}
				if (this->lines[i] == 0)
				{
					NEW_CLASS(this->lines[i], Data::ArrayList<MapLineStyle*>());
				}

				if (strCnt == 5)
				{
					currLine = MemAlloc(MapLineStyle, 1);
					currLine->lineType = Text::StrToInt32(strs[2].v);
					currLine->lineWidth = Text::StrToInt32(strs[3].v);
					currLine->color = ToColor(strs[4].v);
					currLine->styles = 0;
					this->lines[i]->Add(currLine);
				}
				else
				{
					j = 6;
					while (j < strCnt)
					{
						strs[j++].v[-1] = ',';
					}
					sptr = strs[strCnt-1].v;
					while (*sptr++);
					currLine = MemAlloc(MapLineStyle, 1);
					currLine->lineType = Text::StrToInt32(strs[2].v);
					currLine->lineWidth = Text::StrToInt32(strs[3].v);
					currLine->color = ToColor(strs[4].v);
					currLine->styles = MemAlloc(UTF8Char, (UOSInt)(sptr - strs[5].v));
					Text::StrConcatC(currLine->styles, strs[5].v, (UOSInt)(sptr - strs[5].v - 1));
					this->lines[i]->Add(currLine);
				}
				break;
			case 5:
				i = Text::StrToUInt32(strs[1].v);
				if (i >= this->nFont)
				{
					PrintDebug(L"Error found in MapLayer files, font id too large\r\n");
					break;
				}
				if (this->fonts[i] == 0)
				{
					NEW_CLASS(this->fonts[i], Data::ArrayList<MapFontStyle*>());
				}

				currFont = MemAlloc(MapFontStyle, 1);
				currFont->fontType = Text::StrToInt32(strs[2].v);
				currFont->fontName = Text::String::New(strs[3].v, strs[3].leng);
				currFont->fontSizePt = (Text::StrToUInt32(strs[4].v) * 3) >> 2;
				currFont->thick = Text::StrToInt32(strs[5].v);
				currFont->color = ToColor(strs[6].v);
				this->fonts[i]->Add(currFont);
				break;
			case 0:
				break;
			case 13:
				break;
			case 6:
				{
					NN<Map::MapDrawLayer> lyr;
					layerNameEnd = strs[1].ConcatTo(baseDir);
					if (GetDrawLayer(CSTRP(layerName, layerNameEnd), layerList, errWriter).SetTo(lyr))
					{
						currLayer = MemAlloc(MapLayerStyle, 1);
						currLayer->drawType = 6;
						currLayer->minScale = Text::StrToInt32(strs[2].v);
						currLayer->maxScale = Text::StrToInt32(strs[3].v);
						currLayer->img = 0;
						currLayer->lyr = lyr;
						currLayer->style = Text::StrToUInt32(strs[4].v);
						currLayer->bkColor = 0;
						if (currLayer->style < this->nLine)
						{
							this->drawList->Add(currLayer);
						}
						else
						{
							lyr.Delete();
							MemFree(currLayer);
						}
					}
				}
				break;
			case 7:
				{
					NN<Map::MapDrawLayer> lyr;
					layerNameEnd = strs[1].ConcatTo(baseDir);
					if (GetDrawLayer(CSTRP(layerName, layerNameEnd), layerList, errWriter).SetTo(lyr))
					{
						currLayer = MemAlloc(MapLayerStyle, 1);
						currLayer->drawType = 7;
						currLayer->minScale = Text::StrToInt32(strs[2].v);
						currLayer->maxScale = Text::StrToInt32(strs[3].v);
						currLayer->img = 0;
						currLayer->lyr = lyr;
						currLayer->style = Text::StrToUInt32(strs[4].v);
						currLayer->bkColor = ToColor(strs[5].v);
						if (currLayer->style < this->nLine)
						{
							this->drawList->Add(currLayer);
						}
						else
						{
							lyr.Delete();
							MemFree(currLayer);
						}
					}
				}
				break;
			case 8:
//					minScale = Text::StrToInt32(strs[2]);
//					maxScale = Text::StrToInt32(strs[3]);
				break;
			case 9:
				{
					NN<Map::MapDrawLayer> lyr;
					layerNameEnd = strs[1].ConcatTo(baseDir);
					if (GetDrawLayer(CSTRP(layerName, layerNameEnd), layerList, errWriter).SetTo(lyr))
					{
						currLayer = MemAlloc(MapLayerStyle, 1);
						currLayer->drawType = 9;
						currLayer->minScale = Text::StrToInt32(strs[2].v);
						currLayer->maxScale = Text::StrToInt32(strs[3].v);
						currLayer->img = 0;
						currLayer->lyr = lyr;
						currLayer->priority = Text::StrToInt32(strs[4].v);
						currLayer->style = Text::StrToUInt32(strs[5].v);
						currLayer->bkColor = Text::StrToUInt32(strs[6].v);
						if (currLayer->style < this->nFont)
						{
							this->drawList->Add(currLayer);
						}
						else
						{
							lyr.Delete();
							MemFree(currLayer);
						}
					}
				}
				break;
			case 10:
				currLayer = MemAlloc(MapLayerStyle, 1);
				currLayer->drawType = 10;
				currLayer->minScale = Text::StrToInt32(strs[2].v);
				currLayer->maxScale = Text::StrToInt32(strs[3].v);
				currLayer->img = 0;
				{
					IO::ParsedObject *obj;
					{
						IO::StmData::FileData fd(strs[4].ToCString(), false);
						obj = parserList->ParseFile(fd);
					}
					if (obj)
					{
						if (obj->GetParserType() == IO::ParserType::ImageList)
						{
							Media::ImageList *imgList = (Media::ImageList*)obj;
							if (imgList->GetCount() > 0)
							{
								imgList->ToStaticImage(0);
								NN<Media::StaticImage> img;
								if (Optional<Media::StaticImage>::ConvertFrom(imgList->GetImage(0, 0)).SetTo(img))
								{
									if (img->To32bpp())
									{
										currLayer->img = this->drawEng->ConvImage(img);
										DEL_CLASS(obj);
									}
									else
									{
										DEL_CLASS(obj);
									}
								}
								else
								{
									DEL_CLASS(obj);
								}
							}
							else
							{
								DEL_CLASS(obj);
							}
						}
						else
						{
							DEL_CLASS(obj);
						}
					}
				}
				if (currLayer->img == 0)
				{
					currLayer->img = this->drawEng->LoadImage(strs[4].ToCString());
				}
				NN<Media::DrawImage> img;
				if (!img.Set(currLayer->img))
				{
					MemFree(currLayer);
				}
				else
				{
					layerNameEnd = strs[1].ConcatTo(baseDir);
					if (!GetDrawLayer(CSTRP(layerName, layerNameEnd), layerList, errWriter).SetTo(currLayer->lyr))
					{
						this->drawEng->DeleteImage(img);
						MemFree(currLayer);
					}
					else
					{
/*						if (osSize != 1)
						{
							Media::GDIImage *newImg;
							Media::GDIImage *oldImg = (Media::GDIImage*)currLayer->img;
							newImg = (Media::GDIImage*)this->drawEng->CreateImage32(oldImg->GetWidth() * OVERSAMPLESIZE, oldImg->GetHeight() * OVERSAMPLESIZE);
							resizer->Resize((UInt8*)oldImg->bmpBits, oldImg->GetWidth() * 4, oldImg->GetWidth(), oldImg->GetHeight(), (UInt8*)newImg->bmpBits, newImg->GetWidth() * 4, newImg->GetWidth(), newImg->GetHeight());
							this->drawEng->DeleteImage(oldImg);
							currLayer->img = newImg;
						}*/
						this->drawList->Add(currLayer);
						poiArr.Add(currLayer);
					}
				}
				break;
			case 14:
				if (strCnt > 1)
				{
					Map::MapConfig2::MapArea *area;
					area = MemAlloc(Map::MapConfig2::MapArea, 1);
					area->mcc = Text::StrToInt32(strs[1].v);
					if (strCnt > 2)
					{
						layerNameEnd = strs[2].ConcatTo(baseDir);
						NEW_CLASS(area->data, Map::MapLayerData(CSTRP(layerName, layerNameEnd)));
					}
					else
					{
						area->data = 0;
					}
					this->areaList.Add(area);
				}
				break;
			default:
				break;
			}
		}

		if (this->inited)
		{
			i = this->drawList->GetCount();
			while (i-- > 0)
			{
				currLayer = this->drawList->GetItem(i);
				if (currLayer->drawType == 9)
				{
					j = poiArr.GetCount();
					while (j-- > 0)
					{
						currLayer2 = poiArr.GetItem(j);
						if (currLayer2->lyr == currLayer->lyr)
						{
							currLayer->img = currLayer2->img;
							break;
						}
					}
				}
			}
		}
	}
}

Map::MapConfig2::~MapConfig2()
{
	UOSInt i;
	UOSInt j;
	Map::MapLineStyle *currLine;
	Map::MapFontStyle *currFont;
	Map::MapLayerStyle *currLyr;

	if (this->lines)
	{
		i = this->nLine;
		while (i-- > 0)
		{
			if (this->lines[i])
			{
				j = this->lines[i]->GetCount();
				while (j-- > 0)
				{
					currLine = (Map::MapLineStyle*)this->lines[i]->GetItem(j);
					if (currLine->styles)
					{
						MemFree(currLine->styles);
					}
					MemFree(currLine);
				}
				DEL_CLASS(this->lines[i]);
			}
		}
		MemFree(this->lines);
		this->lines = 0;
	}
	if (this->fonts)
	{
		i = this->nFont;
		while (i-- > 0)
		{
			if (this->fonts[i])
			{
				j = this->fonts[i]->GetCount();
				while (j-- > 0)
				{
					currFont = (Map::MapFontStyle*)this->fonts[i]->GetItem(j);
					currFont->fontName->Release();
					MemFree(currFont);
				}
				DEL_CLASS(this->fonts[i]);
			}
		}
		MemFree(this->fonts);
	}

	if (this->drawList)
	{
		NN<Media::DrawImage> img;
		UOSInt i = this->drawList->GetCount();
		while (i-- > 0)
		{
			currLyr = this->drawList->GetItem(i);
			if (img.Set(currLyr->img) && currLyr->drawType == 10)
			{
				this->drawEng->DeleteImage(img);
			}
			MemFree(currLyr);
		}
		DEL_CLASS(this->drawList);
	}
	i = this->areaList.GetCount();
	while (i-- > 0)
	{
		Map::MapConfig2::MapArea *area = this->areaList.GetItem(i);
		if (area->data)
		{
			DEL_CLASS(area->data);
		}
		MemFree(area);
	}
}

Bool Map::MapConfig2::IsError()
{
	return this->drawList == 0;
}

Optional<Media::DrawPen> Map::MapConfig2::CreatePen(NN<Media::DrawImage> img, UInt32 lineStyle, UOSInt lineLayer)
{
	if (lineStyle >= this->nLine)
	{
		return 0;
	}
	if (this->lines[lineStyle] == 0)
		return 0;
	Map::MapLineStyle *thisLine = (Map::MapLineStyle*)this->lines[lineStyle]->GetItem(lineLayer);
	if (thisLine == 0)
		return 0;

	if (thisLine->lineType == 0)
	{
		return img->NewPenARGB(thisLine->color, thisLine->lineWidth * img->GetHDPI() / 96.0, 0, 0);
	}
	else if (thisLine->lineType == 1)
	{
		return img->NewPenARGB(thisLine->color, thisLine->lineWidth * img->GetHDPI() / 96.0, 0, 0);
	}
	else if (thisLine->lineType == 2)
	{
		UInt8 *pattern = MemAlloc(UInt8, Text::StrCharCnt(thisLine->styles));
		UInt32 currVal;
		UTF8Char *currCh;
		UInt32 i;

		currCh = thisLine->styles;
		currVal = 0;
		i = 0;
		while (true)
		{
			if (*currCh >= 0x30 && *currCh <= 0x39)
			{
				currVal = currVal * 10 + *currCh - 48;
			}
			else
			{
				pattern[i++] = (UInt8)Double2Int32(currVal * img->GetHDPI() / 96.0);
				currVal = 0;
				if (*currCh == ',')
				{
				}
				else if (*currCh == 0)
				{
					break;
				}
				else
				{
					break;
				}
			}
			currCh++;
		}
		NN<Media::DrawPen> pen = img->NewPenARGB(thisLine->color, thisLine->lineWidth * img->GetHDPI() / 96.0, pattern, i);
		MemFree(pattern);
		return pen;
	}
	return 0;
}

UTF8Char *Map::MapConfig2::DrawMap(NN<Media::DrawImage> img, NN<Map::MapView> view, Bool *isLayerEmpty, Map::MapScheduler *mapSch, Media::IImgResizer *resizer, UTF8Char *slowLayer, Double *slowTime)
{
	//Manage::HiResClock clk;
	UTF8Char *retPtr = 0;
	const UTF8Char *maxName = 0;
	Double thisSpd = 0;
	Double maxSpd = 0;
	UInt32 index;
	UInt32 index2;
	UOSInt layerCnt = this->drawList->GetCount();
	Data::ArrayList<MapFontStyle*> **myArrs;
	Data::ArrayList<MapFontStyle*> *fontArr;
	NN<Map::MapDrawLayer> lyr;
	Map::MapLayerStyle *lyrs;
	NN<Math::Geometry::Vector2D> vec;
	Map::MapFontStyle *fnt;
	Map::MapFontStyle *fnt2;
	NN<Media::DrawBrush> brush;
	Optional<Media::DrawPen> pen;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt l;
	Int64 lastId;
	Int64 thisId;
	Double thisScale;
	UInt32 labelCnt = 0;
	UInt32 maxLabel = this->nStr;
	*isLayerEmpty = true;
	Manage::HiResClock clk;
	Map::MapLabels2 *labels = MemAlloc(Map::MapLabels2, maxLabel);
	Math::RectAreaDbl *objBounds = MemAllocA(Math::RectAreaDbl, this->nStr * 2);
	UOSInt objCnt = 0;
	view->SetDestImage(img);

	Data::ArrayListInt64 arr;
	thisScale = view->GetMapScale();
#ifndef NOSCH
	mapSch->SetMapView(view, img);
#endif

	brush = img->NewBrushARGB(this->bgColor);
	img->DrawRect(Math::Coord2DDbl(0, 0), img->GetSize().ToDouble(), 0, brush);
	img->DelBrush(brush);

	myArrs = MemAlloc(Data::ArrayList<MapFontStyle*>*, this->nFont);
	i = this->nFont;
	while (i-- > 0)
	{
		fontArr = this->fonts[i];
		if (fontArr)
		{
			Map::MapFontStyle *f;
			j = 0;
			k = fontArr->GetCount();
			NEW_CLASS(myArrs[i], Data::ArrayList<MapFontStyle*>());
			while (j < k)
			{
				fnt = (Map::MapFontStyle*)fontArr->GetItem(j);
				fnt2 = MemAlloc(Map::MapFontStyle, 1);
				fnt2->color = fnt->color;
				fnt2->fontName = fnt->fontName;
				fnt2->fontSizePt = fnt->fontSizePt;
				fnt2->fontType = fnt->fontType;
				fnt2->thick = fnt->thick;
				if (fnt->fontType == 0)
				{
					Media::DrawEngine::DrawFontStyle s = Media::DrawEngine::DFS_NORMAL;
					if (fnt->thick > 0)
						s = (Media::DrawEngine::DrawFontStyle)(s | Media::DrawEngine::DFS_BOLD);
					fnt2->font = img->NewFontPt(fnt->fontName->ToCString(), fnt->fontSizePt, s, 0);
					fnt2->other = img->NewBrushARGB(fnt->color).Ptr();
				}
				else if (fnt->fontType == 1)
				{
					fnt2->other = img->NewBrushARGB(fnt->color).Ptr();
				}
				else if (fnt->fontType == 2)
				{
					fnt2->other = img->NewPenARGB(fnt->color, fnt->thick * img->GetHDPI() / 96.0, 0, 0).Ptr();
				}
				else if (fnt->fontType == 4)
				{
					f = 0;
					l = j + 1;
					while (l < k)
					{
						if (fontArr->GetItem(l)->fontType == 0)
						{
							f = fontArr->GetItem(l);
							break;
						}
						l++;
					}
					if (f)
					{
						Media::DrawEngine::DrawFontStyle s = Media::DrawEngine::DFS_NORMAL;
						if (f->thick > 0)
							s = (Media::DrawEngine::DrawFontStyle)(s | Media::DrawEngine::DFS_BOLD);
						fnt2->font = img->NewFontPt(f->fontName->ToCString(), f->fontSizePt, s, 0);
					}
					else
					{
						fnt2->font = img->NewFontPt(fnt->fontName->ToCString(), fnt->fontSizePt, Media::DrawEngine::DFS_NORMAL, 0);
					}
					fnt2->other = img->NewBrushARGB(fnt->color).Ptr();
					fnt2->thick = fnt2->thick * img->GetHDPI() / 96.0;
				}

				myArrs[i]->Add(fnt2);
				j++;
			}
		}
		else
		{
			myArrs[i] = 0;
		}
	}

	index = 0;
	while (index < layerCnt)
	{
		Map::GetObjectSess *session;
		lyrs = this->drawList->GetItem(index++);
		if (thisScale > lyrs->minScale && thisScale <= lyrs->maxScale)
		{
			clk.Start();
			if (lyrs->drawType == 7)
			{
				lyr = lyrs->lyr;
				arr.Clear();
				Math::RectAreaDbl rect = view->GetVerticalRect();
				lyr->GetObjectIdsMapXY(arr, 0, rect, true);

				if ((i = arr.GetCount()) > 0)
				{
#ifdef NOSCH
					Data::ArrayList<Map::DrawObjectL*> drawArr;
#else
					mapSch->SetDrawType(lyr, pen = CreatePen(img, lyrs->style, 0), brush = img->NewBrushARGB(lyrs->bkColor), 0, 0, 0, isLayerEmpty);
#endif

					session = lyr->BeginGetObject();
					lastId = -1;
					while (i-- > 0)
					{
						thisId = arr.GetItem(i);
						if (thisId != lastId)
						{
							lastId = thisId;
							if (vec.Set(lyr->GetNewVectorById(session, thisId)))
							{
#ifndef NOSCH
								mapSch->Draw(vec);
#else
								if (view->MapXYToScnXY(dobj->pointArr, dobj->pointArr, dobj->nPoint, Math::Coord2DDbl(0, 0)))
									*isLayerEmpty = false;
								drawArr.Add(dobj);
#endif
							}
						}
					}
					lyr->EndGetObject(session);
#ifdef NOSCH
					pen = CreatePen(img, lyrs->style, 0);
					brush = img->NewBrushARGB(lyrs->bkColor);
					i = drawArr.GetCount();
					while (i-- > 0)
					{
						dobj = drawArr.GetItem(i);
						k = dobj->nPtOfst;
						j = 1;
						while (j < k)
						{
							dobj->ptOfstArr[j - 1] = dobj->ptOfstArr[j] - dobj->ptOfstArr[j - 1];
							j++;
						}
						dobj->ptOfstArr[k - 1] = dobj->nPoint - dobj->ptOfstArr[k - 1];

						img->DrawPolyPolygon(dobj->pointArr, dobj->ptOfstArr, dobj->nPtOfst, pen, brush);
					}
					img->DelPen(pen);
					img->DelBrush(brush);

					if (pen)
					{
						index2 = 1;
						while (pen = CreatePen(img, lyrs->style, index2++))
						{
							i = drawArr.GetCount();
							while (i-- > 0)
							{
								dobj = drawArr.GetItem(i);
								k = dobj->nPtOfst;
								j = 1;
								while (j < k)
								{
									dobj->ptOfstArr[j - 1] = dobj->ptOfstArr[j] - dobj->ptOfstArr[j - 1];
									j++;
								}
								dobj->ptOfstArr[k - 1] = dobj->nPoint - dobj->ptOfstArr[k - 1];

								img->DrawPolyPolygon(dobj->pointArr, dobj->ptOfstArr, dobj->nPtOfst, pen, 0);
							}
							img->DelPen(pen);
						}
					}

					i = drawArr.GetCount();
					while (i-- > 0)
					{
						dobj = drawArr.RemoveAt(i);
						lyr->ReleaseObject(session, dobj);
					}
#else
					if (!pen.IsNull())
					{
						index2 = 1;
						while (!(pen = CreatePen(img, lyrs->style, index2++)).IsNull())
						{
							mapSch->DrawNextType(pen, 0);
						}
					}
					mapSch->WaitForFinish();
#endif
				}
			}
			else if (lyrs->drawType == 6)
			{
				lyr = lyrs->lyr;
				arr.Clear();
				Math::RectAreaDbl rect = view->GetVerticalRect();
				lyr->GetObjectIdsMapXY(arr, 0, rect, true);

				if ((i = arr.GetCount()) > 0)
				{
#ifdef NOSCH
					Data::ArrayList<Map::DrawObjectL*> drawArr;
#else
					mapSch->SetDrawType(lyr, pen = CreatePen(img, lyrs->style, 0), 0, 0, 0, 0, isLayerEmpty);
#endif

					session = lyr->BeginGetObject();
					lastId = -1;
					while (i-- > 0)
					{
						thisId = arr.GetItem(i);
						if (thisId != lastId)
						{
							lastId = thisId;
							if (vec.Set(lyr->GetNewVectorById(session, thisId)))
							{
#ifndef NOSCH
								mapSch->Draw(vec);
#else
								if (view->MapXYToScnXY(dobj->pointArr, dobj->pointArr, dobj->nPoint, Math::Coord2DDbl(0, 0)))
									*isLayerEmpty = false;
								drawArr.Add(dobj);
#endif
							}
						}
					}
					lyr->EndGetObject(session);

#ifdef NOSCH
					index2 = 0;
					while (pen = CreatePen(img, lyrs->style, index2))
					{
						index2++;
						i = drawArr.GetCount();
						while (i-- > 0)
						{
							dobj = drawArr.GetItem(i);

							k = dobj->nPtOfst;
							j = 1;
							while (j < k)
							{
								img->DrawPolyline(&dobj->pointArr[dobj->ptOfstArr[j-1]], dobj->ptOfstArr[j] - dobj->ptOfstArr[j - 1], pen);
								j++;
							}
							img->DrawPolyline(&dobj->pointArr[dobj->ptOfstArr[k-1] << 1], dobj->nPoint - dobj->ptOfstArr[k - 1], pen);
						}
						img->DelPen(pen);
					}

					i = drawArr.GetCount();
					while (i-- > 0)
					{
						dobj = drawArr.RemoveAt(i);
						lyr->ReleaseObject(session, dobj);
					}
#else
					if (!pen.IsNull())
					{
						index2 = 1;
						while (!(pen = CreatePen(img, lyrs->style, index2++)).IsNull())
						{
							mapSch->DrawNextType(pen, 0);
						}
					}
					mapSch->WaitForFinish();
#endif
				}
			}
			else if (lyrs->drawType == 9)
			{
				DrawString(img, lyrs, view, myArrs, labels, maxLabel, &labelCnt, isLayerEmpty);
			}
			else if (lyrs->drawType == 10)
			{
				DrawPoints(img, lyrs, view, isLayerEmpty, mapSch, drawEng, resizer, objBounds, &objCnt, maxLabel);
			}
			thisSpd = clk.GetTimeDiff();
			if (thisSpd > maxSpd)
			{
				maxSpd = thisSpd;
				maxName = lyrs->lyr->GetName()->v;
			}
		}
	}

	DrawLabels(img, labels, maxLabel, &labelCnt, view, myArrs, drawEng, objBounds, &objCnt);

	NN<Media::DrawBrush> b;
	NN<Media::DrawPen> p;
	i = this->nFont;
	while (i-- > 0)
	{
		fontArr = myArrs[i];
		if (fontArr)
		{
			j = 0;
			k = fontArr->GetCount();
			while (j < k)
			{
				fnt = (Map::MapFontStyle*)fontArr->GetItem(j);
				if (fnt->fontType == 0)
				{
					img->DelFont(fnt->font);
					if (b.Set((Media::DrawBrush*)fnt->other))
						img->DelBrush(b);
				}
				else if (fnt->fontType == 1)
				{
					if (b.Set((Media::DrawBrush*)fnt->other))
						img->DelBrush(b);
				}
				else if (fnt->fontType == 2)
				{
					if (p.Set((Media::DrawPen*)fnt->other))
						img->DelPen(p);
				}
				else if (fnt->fontType == 4)
				{
					img->DelFont(fnt->font);
					if (b.Set((Media::DrawBrush*)fnt->other))
						img->DelBrush(b);
				}
				MemFree(fnt);

				j++;
			}
			DEL_CLASS(fontArr);
		}
	}
	MemFree(myArrs);
	MemFree(labels);
	MemFreeA(objBounds);

	//Double t = clk.GetTimeDiff();
	//printf("Time used: %d\n", (Int32)(t * 1000));
	if (maxName && maxSpd && slowLayer && slowTime)
	{
		*slowTime = maxSpd;
		retPtr = Text::StrConcat(slowLayer, maxName);
	}

	return retPtr;
}

UInt32 Map::MapConfig2::GetBGColor()
{
	return this->bgColor;
}

Bool Map::MapConfig2::SupportMCC(Int32 mcc)
{
	if (mcc == 0)
		return true;
	UOSInt i = this->areaList.GetCount();
	while (i-- > 0)
	{
		if (this->areaList.GetItem(i)->mcc == mcc)
			return true;
	}
	return false;
}

Int32 Map::MapConfig2::QueryMCC(Math::Coord2DDbl pos)
{
	UOSInt i = this->areaList.GetCount();
	Text::StringBuilderUTF8 sb;
	while (i-- > 0)
	{
		Map::MapConfig2::MapArea *area;
		area = this->areaList.GetItem(i);
		if (area->data)
		{
			if (area->data->GetPGLabel(sb, pos, 0, 0))
			{
//				IO::Console::PrintStr(L"Area found: ");
//				IO::Console::PrintStr(buff);
//				IO::Console::PrintStr(L"\n");
				return area->mcc;
			}
		}
	}
	return 0;
}

void Map::MapConfig2::ReleaseLayers(NN<Data::ArrayListNN<Map::MapDrawLayer>> layerList)
{
	layerList->DeleteAll();
}
