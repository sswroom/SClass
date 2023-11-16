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
#include "IO/StreamWriter.h"
#include "IO/StmData/FileData.h"
#include "Manage/HiResClock.h"
#include "Map/MapDrawLayer.h"
#include "Map/MapView.h"
#include "Map/MapConfig2TGen.h"
#include "Map/CIPLayer2.h"
#include "Math/GeometryTool.h"
#include "Math/Math.h"
#include "Media/DrawEngine.h"
#include "Media/FrameInfo.h"
#include "Media/IImgResizer.h"
#include "Media/StaticImage.h"
#include "Media/ImageList.h"
#include "Media/Resizer/LanczosResizerH8_8.h"
#include "Sync/Event.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/MyStringW.h"
#include "Text/StringTool.h"
#include "Text/UTF8Writer.h"

#define SFLG_ROTATE 1
#define SFLG_SMART 2
#define SFLG_ALIGN 4
#define LBLMINDIST 150
//#define NOSCH
//#define PrintDebug(msg) IO::Console::PrintStr(msg)
#define PrintDebug(msg)

#define PI 3.14159265358979323846

class MapLogger
{
private:
	Text::UTF8Writer *writer;
	IO::FileStream *fs;
	Double scnW;
	Double scnH;
public:
	MapLogger(Text::CString fileName, Map::MapView *view)
	{
		if (fileName.leng > 0)
		{
			NotNullPtr<IO::FileStream> fs;
			NEW_CLASSNN(fs, IO::FileStream(fileName.OrEmpty(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
			this->fs = fs.Ptr();
			NEW_CLASS(writer, Text::UTF8Writer(fs));
			writer->WriteSignature();
		}
		else
		{
			fs = 0;
			writer = 0;
		}
		scnW = view->GetScnWidth();
		scnH = view->GetScnHeight();
	}

	~MapLogger()
	{
		if (writer)
		{
			DEL_CLASS(writer);
			DEL_CLASS(fs);
		}
	}

	void AddString(const UTF8Char *label, Double x, Double y, Double scaleW, Double scaleH, UInt32 fontStyle, Bool isAlign, const Math::RectAreaDbl *bounds)
	{
		UTF8Char sbuff[256];
		UTF8Char *sptr;
		if (this->writer == 0 || x < -scnW || x > scnW * 2 || y < -scnH || y > scnH * 2)
		{
			return;
		}
		sptr = Text::StrConcatC(sbuff, UTF8STRC("1,"));
		sptr = Text::StrToCSVRec(sptr, label);
		sptr = Text::StrConcatC(sptr, UTF8STRC(","));
		sptr = Text::StrDouble(sptr, x);
		sptr = Text::StrConcatC(sptr, UTF8STRC(","));
		sptr = Text::StrDouble(sptr, y);
		sptr = Text::StrConcatC(sptr, UTF8STRC(","));
		sptr = Text::StrDouble(sptr, bounds->tl.x);
		sptr = Text::StrConcatC(sptr, UTF8STRC(","));
		sptr = Text::StrDouble(sptr, bounds->tl.y);
		sptr = Text::StrConcatC(sptr, UTF8STRC(","));
		sptr = Text::StrDouble(sptr, bounds->br.x);
		sptr = Text::StrConcatC(sptr, UTF8STRC(","));
		sptr = Text::StrDouble(sptr, bounds->br.y);
		sptr = Text::StrConcatC(sptr, UTF8STRC(","));
		sptr = Text::StrDouble(sptr, scaleW);
		sptr = Text::StrConcatC(sptr, UTF8STRC(","));
		sptr = Text::StrDouble(sptr, scaleH);
		sptr = Text::StrConcatC(sptr, UTF8STRC(","));
		sptr = Text::StrUInt32(sptr, fontStyle);
		sptr = Text::StrConcatC(sptr, UTF8STRC(","));
		if (isAlign)
		{
			sptr = Text::StrConcatC(sptr, UTF8STRC("1"));
		}
		else
		{
			sptr = Text::StrConcatC(sptr, UTF8STRC("0"));
		}
		this->writer->WriteLineC(sbuff, (UOSInt)(sptr - sbuff));
	}

	void AddStringL(const UTF8Char *label, Math::Coord2DDbl *mapPts, Math::Coord2D<Int32> *scnPts, UOSInt nPoints, UInt32 thisPt, Double scaleN, Double scaleD, UInt32 fontStyle, Bool isAlign, const Math::RectAreaDbl *bounds)
	{
		UTF8Char sbuff[256];
		UTF8Char *sptr;
		UOSInt i;
		if (this->writer == 0 || bounds->br.x < -scnW || bounds->tl.x > scnW * 2 || bounds->br.y < -scnH || bounds->tl.y > scnH * 2)
		{
			return;
		}
		Math::Coord2DDbl center = bounds->GetCenter();
		sptr = Text::StrConcatC(sbuff, UTF8STRC("2,"));
		sptr = Text::StrToCSVRec(sptr, label);
		sptr = Text::StrConcatC(sptr, UTF8STRC(","));
		sptr = Text::StrDouble(sptr, center.x);
		sptr = Text::StrConcatC(sptr, UTF8STRC(","));
		sptr = Text::StrDouble(sptr, center.y);
		sptr = Text::StrConcatC(sptr, UTF8STRC(","));
		sptr = Text::StrDouble(sptr, bounds->tl.x);
		sptr = Text::StrConcatC(sptr, UTF8STRC(","));
		sptr = Text::StrDouble(sptr, bounds->tl.y);
		sptr = Text::StrConcatC(sptr, UTF8STRC(","));
		sptr = Text::StrDouble(sptr, bounds->br.x);
		sptr = Text::StrConcatC(sptr, UTF8STRC(","));
		sptr = Text::StrDouble(sptr, bounds->br.y);
		sptr = Text::StrConcatC(sptr, UTF8STRC(","));
		sptr = Text::StrUOSInt(sptr, nPoints);
		sptr = Text::StrConcatC(sptr, UTF8STRC(","));
		sptr = Text::StrUInt32(sptr, thisPt);
		sptr = Text::StrConcatC(sptr, UTF8STRC(","));
		sptr = Text::StrDouble(sptr, scaleN);
		sptr = Text::StrConcatC(sptr, UTF8STRC(","));
		sptr = Text::StrDouble(sptr, scaleD);
		sptr = Text::StrConcatC(sptr, UTF8STRC(","));
		sptr = Text::StrUInt32(sptr, fontStyle);
		sptr = Text::StrConcatC(sptr, UTF8STRC(","));
		if (isAlign)
		{
			sptr = Text::StrConcatC(sptr, UTF8STRC("1"));
		}
		else
		{
			sptr = Text::StrConcatC(sptr, UTF8STRC("0"));
		}
		this->writer->WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
		i = 0;
		while (i < nPoints)
		{
			sptr = Text::StrConcatC(sbuff, UTF8STRC(",{"));
			sptr = Text::StrDouble(sptr, mapPts->x);
			sptr = Text::StrConcatC(sptr, UTF8STRC(","));
			sptr = Text::StrDouble(sptr, mapPts->y);
			sptr = Text::StrConcatC(sptr, UTF8STRC(","));
			sptr = Text::StrInt32(sptr, scnPts[0].x);
			sptr = Text::StrConcatC(sptr, UTF8STRC(","));
			sptr = Text::StrInt32(sptr, scnPts[0].y);
			sptr = Text::StrConcatC(sptr, UTF8STRC("}"));
			mapPts += 1;
			scnPts += 1;
			this->writer->WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
			i++;
		}

		this->writer->WriteLine();
	}
};

Bool Map::MapConfig2TGen::IsDoorNum(const UTF8Char *txt)
{
	WChar c;
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

void Map::MapConfig2TGen::DrawChars(NotNullPtr<Media::DrawImage> img, Text::CStringNN str1, Double scnPosX, Double scnPosY, Double scaleW, Double scaleH, Data::ArrayList<MapFontStyle*> *fontStyle, Bool isAlign)
{
	UTF8Char sbuff[256];
	str1.ConcatTo(sbuff);
	Math::Size2DDbl size;
	Double absH;
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
		font = (Map::MapFontStyle*)fontStyle->GetItem(i);
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
	font = fontStyle->GetItem(maxIndex);
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
		NotNullPtr<Media::DrawBrush> b;
		img->SetTextAlign(Media::DrawEngine::DRAW_POS_TOPLEFT);

		i = 0;
		while (i < fntCount)
		{
			font = fontStyle->GetItem(i);
			if (font->fontType == 1)
			{
				Media::DrawPen *p = 0;
				Media::DrawBrush *b = 0;
				rcLeft = scnPosX - ((size.x + font->fontSizePt) * 0.5);
				rcRight = rcLeft + size.x + font->fontSizePt;
				rcTop = scnPosY - ((size.y + font->fontSizePt) * 0.5);
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
			else if (font->fontType == 2)
			{
				Math::Coord2DDbl pt[5];
				rcLeft = scnPosX - ((size.x + font->fontSizePt) * 0.5);
				rcRight = rcLeft + size.x + font->fontSizePt;
				rcTop = scnPosY - ((size.y + font->fontSizePt) * 0.5);
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

				img->DrawPolyline(pt, 5, (Media::DrawPen*)font->other);
			}
			else if (font->fontType == 0 && b.Set((Media::DrawBrush*)font->other))
			{
				img->DrawString(Math::Coord2DDbl(scnPosX - (size.x * 0.5), scnPosY - (size.y * 0.5)), str1, font->font, b);
			}
			else if (font->fontType == 4 && b.Set((Media::DrawBrush*)font->other))
			{
				img->DrawStringB(Math::Coord2DDbl(scnPosX - (size.x * 0.5), scnPosY - (size.y * 0.5)), str1, font->font, b, (UInt32)Double2Int32(font->thick));
			}
			i++;
		}

		return;
	}


	if (scaleH < 0)
		absH = -scaleH;
	else
		absH = scaleH;

	Double degD = Math_ArcTan2(scaleH, scaleW);

	Int32 deg = (Int32) (Math_ArcTan2(scaleH, scaleW) * 1800 / PI);
	while (deg < 0)
		deg += 3600;

	Double lastScaleW = scaleW;
	Double lastScaleH = scaleH;

	i = 0;
	while (i < fntCount)
	{
		NotNullPtr<Media::DrawBrush> b;
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

			pt[0].x = scnPosX - xc - ys;
			pt[0].y = scnPosY + xs - yc;
			pt[1].x = scnPosX + xc - ys;
			pt[1].y = scnPosY - xs - yc;
			pt[2].x = scnPosX + xc + ys;
			pt[2].y = scnPosY - xs + yc;
			pt[3].x = scnPosX - xc + ys;
			pt[3].y = scnPosY + xs + yc;


			p = 0;
			if (b.Set((Media::DrawBrush*)font->other))
			{
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
		}
		else if (font->fontType == 2)
		{
			xPos = size.x + font->fontSizePt;
			yPos = size.y + font->fontSizePt;
			Double xs = ((xPos * 0.5) * (sVal = Math_Sin(degD)));
			Double ys = ((yPos * 0.5) * sVal);
			Double xc = ((xPos * 0.5) * (cVal = Math_Cos(degD)));
			Double yc = ((yPos * 0.5) * cVal);

			pt[0].x = scnPosX - xc - ys;
			pt[0].y = scnPosY + xs - yc;
			pt[1].x = scnPosX + xc - ys;
			pt[1].y = scnPosY - xs - yc;
			pt[2].x = scnPosX + xc + ys;
			pt[2].y = scnPosY - xs + yc;
			pt[3].x = scnPosX - xc + ys;
			pt[3].y = scnPosY + xs + yc;
			pt[4] = pt[0];

			img->DrawPolyline(pt, 5, (Media::DrawPen*)font->other);
		}
		else if ((font->fontType == 0 || font->fontType == 4) && b.Set((Media::DrawBrush*)font->other))
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
					startX = scnPosX - (tmp = (szThis.x * 0.5));
					if (scaleW)
						startY = scnPosY - (szThis.y * 0.5) - (tmp * scaleH / scaleW);
					else
						startY = scnPosY - (szThis.y * 0.5);
					type = 0;
				}
				else
				{
					scaleW = -scaleW;
					if (scaleH > 0)
					{
						startY = scnPosY - (tmp = ((szThis.y * UOSInt2Double(lblSize)) * 0.5));
						startX = scnPosX - (tmp * scaleW / scaleH);
					}
					else if (scaleH)
					{
						scaleW = -scaleW;
						scaleH = -scaleH;
						startY = scnPosY - (tmp = ((szThis.y * UOSInt2Double(lblSize)) * 0.5));
						startX = scnPosX - (tmp * scaleW / scaleH);
					}
					else
					{
						startY = scnPosY - (tmp = ((szThis.y * UOSInt2Double(lblSize)) * 0.5));
						startX = scnPosX;
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
							img->DrawString(Math::Coord2DDbl(startX + currPt.x - szThis.x * 0.5, startY + currPt.y), {l, 1}, font->font, b);
						}
						else
						{
							UTF8Char l[2];
							l[0] = lbl[0];
							l[1] = 0;
							img->DrawStringB(Math::Coord2DDbl(startX + currPt.x - szThis.x * 0.5, startY + currPt.y), {l, 1}, font->font, b, (UInt32)Double2Int32(font->thick));
						}

						currPt.y += szThis.y;

						if (scaleH)
							currPt.x = currPt.y * scaleW / scaleH;
					}
					else
					{
						if (font->fontType == 0)
						{
							UTF8Char l[2];
							l[0] = lbl[0];
							l[1] = 0;
							img->DrawString(Math::Coord2DDbl(startX + currPt.x, startY + currPt.y), {l, 1}, font->font, b);
						}
						else
						{
							UTF8Char l[2];
							l[0] = lbl[0];
							l[1] = 0;
							img->DrawStringB(Math::Coord2DDbl(startX + currPt.x, startY + currPt.y), {l, 1}, font->font, b, (UInt32)Double2Int32(font->thick));
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
					img->DrawStringRot(Math::Coord2DDbl(scnPosX, scnPosY), str1, font->font, b, (Int32)(degD * 180 / PI));
				}
				else if (font->fontType == 4)
				{
					img->DrawStringRotB(Math::Coord2DDbl(scnPosX, scnPosY), str1, font->font, b, (Int32)(degD * 180 / PI), (UInt32)Double2Int32(font->thick));
				}
			}
		}
		i++;
	}
}

void Map::MapConfig2TGen::DrawCharsLA(NotNullPtr<Media::DrawImage> img, Text::CStringNN str1, Math::Coord2DDbl *mapPts, Math::Coord2D<Int32> *scnPts, UOSInt nPoints, UInt32 thisPt, Double scaleN, Double scaleD, Data::ArrayList<MapFontStyle*> *fontStyle, Math::RectAreaDbl *realBounds)
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
	NotNullPtr<Media::DrawBrush> b;
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
						Double tempY = scnPts[j].y + (scnPts[j + 1].y - scnPts[j].y) * (Double)(nextPt.x - scnPts[j].x) / (scnPts[j + 1].x - scnPts[j].x);
						tempY -= currPt.y;
						if (tempY < 0)
							tempY = -tempY;
						if (tempY > (szLast.y + szThis.y) * 0.5)
						{
							currPt.y = nextPt.y;
							currPt.x = scnPts[j].x + (scnPts[j + 1].x - scnPts[j].x) * (Double)(currPt.y - scnPts[j].y) / (scnPts[j + 1].y - scnPts[j].y);
						}
						else
						{
							currPt.x = nextPt.x;
							currPt.y = scnPts[j].y + (scnPts[j + 1].y - scnPts[j].y) * (Double)(nextPt.x - scnPts[j].x) / (scnPts[j + 1].x - scnPts[j].x);
						}
						break;
					}
					else if (((nextPt.y > scnPts[j].y) ^ (nextPt.y > scnPts[j + 1].y)) || (nextPt.y == scnPts[j].y) || (nextPt.y == scnPts[j + 1].y))
					{
						currPt.y = nextPt.y;
						currPt.x = scnPts[j].x + (scnPts[j + 1].x - scnPts[j].x) * (Double)(currPt.y - scnPts[j].y) / (scnPts[j + 1].y - scnPts[j].y);
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

								Double tempY = scnPts[j].y + (scnPts[j + 1].y - scnPts[j].y) * (Double)(nextPt.x - scnPts[j].x) / (scnPts[j + 1].x - scnPts[j].x);
								tempY -= currPt.y;
								if (tempY < 0)
									tempY = -tempY;
								if (tempY > (szLast.y + szThis.y) * 0.5)
								{
									currPt.y = nextPt.y;
									currPt.x = scnPts[j].x + (scnPts[j + 1].x - scnPts[j].x) * (Double)(currPt.y - scnPts[j].y) / (scnPts[j + 1].y - scnPts[j].y);
								}
								else
								{
									currPt.x = nextPt.x;
									currPt.y = scnPts[j].y + (scnPts[j + 1].y - scnPts[j].y) * (Double)(nextPt.x - scnPts[j].x) / (scnPts[j + 1].x - scnPts[j].x);
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

								Double tempY = scnPts[j].y + (scnPts[j + 1].y - scnPts[j].y) * (Double)(nextPt.x - scnPts[j].x) / (scnPts[j + 1].x - scnPts[j].x);
								tempY -= currPt.y;
								if (tempY < 0)
									tempY = -tempY;
								if (tempY > (szLast.y + szThis.y) * 0.5)
								{
									currPt.y = nextPt.y;
									currPt.x = scnPts[j].x + (scnPts[j + 1].x - scnPts[j].x) * (Double)(currPt.y - scnPts[j].y) / (scnPts[j + 1].y - scnPts[j].y);
								}
								else
								{
									currPt.x = nextPt.x;
									currPt.y = scnPts[j].y + (scnPts[j + 1].y - scnPts[j].y) * (Double)(nextPt.x - scnPts[j].x) / (scnPts[j + 1].x - scnPts[j].x);
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
					Double tempY = scnPts[lastAInd].y + (scnPts[lastAInd + 1].y - scnPts[lastAInd].y) * (Double)(nextPt.x - scnPts[lastAInd].x) / (scnPts[lastAInd + 1].x - scnPts[lastAInd].x);
					Double tempX = scnPts[lastAInd].x + (scnPts[lastAInd + 1].x - scnPts[lastAInd].x) * (Double)(nextPt.y - scnPts[lastAInd].y) / (scnPts[lastAInd + 1].y - scnPts[lastAInd].y);
					tempY -= lastPt.y;
					tempX -= lastPt.x;
					if (tempY < 0)
						tempY = -tempY;
					if (tempX < 0)
						tempX = -tempX;
					if (tempX <= (szLast.x + szThis.x) * 0.5)
					{
						currPt.y = nextPt.y;
						currPt.x = scnPts[lastAInd].x + (scnPts[lastAInd + 1].x - scnPts[lastAInd].x) * (Double)(nextPt.y - scnPts[lastAInd].y) / (scnPts[lastAInd + 1].y - scnPts[lastAInd].y);
					}
					else
					{
						currPt.x = nextPt.x;
						currPt.y = scnPts[lastAInd].y + (scnPts[lastAInd + 1].y - scnPts[lastAInd].y) * (Double)(nextPt.x - scnPts[lastAInd].x) / (scnPts[lastAInd + 1].x - scnPts[lastAInd].x);
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
						img->DrawString(currPt, CSTRP(lbl, nextPos), font->font, b);
					}
					else
					{
						img->DrawStringB(currPt, CSTRP(lbl, nextPos), font->font, b, (UInt32)Double2Int32(font->thick));
					}
				}
				else
				{
					if (font->fontType == 0)
					{
						img->DrawString(currPt, CSTRP(lbl, nextPos), font->font, b);
					}
					else
					{
						img->DrawStringB(currPt, CSTRP(lbl, nextPos), font->font, b, (UInt32)Double2Int32(font->thick));
					}
				}
				szLast = szThis;
			}
		}
		i++;
	}
	realBounds->tl = min;
	realBounds->br = max;
}

void Map::MapConfig2TGen::DrawCharsLAo(NotNullPtr<Media::DrawImage> img, Text::CStringNN str1, Double *mapPts, Math::Coord2D<Int32> *scnPts, UOSInt nPoints, UInt32 thisPt, Double scaleN, Double scaleD, Data::ArrayList<MapFontStyle*> *fontStyle)
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
	NotNullPtr<Media::DrawBrush> b;
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

void Map::MapConfig2TGen::DrawCharsL(NotNullPtr<Media::DrawImage> img, Text::CStringNN str1, Math::Coord2DDbl *mapPts, Math::Coord2D<Int32> *scnPts, UOSInt nPoints, UInt32 thisPt, Double scaleN, Double scaleD, Data::ArrayList<MapFontStyle*> *fontStyle, Math::RectAreaDbl *realBounds)
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

	NotNullPtr<Media::DrawBrush> b;
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

			Text::StrReadChar(lbl, &nextChar);
			while (nextChar)
			{
				lbl = nextPos;
				Text::StrWriteChar(lbl, nextChar);

				while (true)
				{
					nextPos = (UTF8Char*)Text::StrReadChar(nextPos, &nextChar);
					if (nextChar == 0)
					{
						nextPos--;
						break;
					}
					if (nextChar == ' ')
					{
						Text::StrReadChar(nextPos, &nextChar);
						*nextPos = 0;
						break;
					}
					else if (nextChar >= 0x3f00 && nextChar <= 0x9f00)
					{
						Text::StrReadChar(nextPos, &nextChar);
						*nextPos = 0;
						break;
					}
				}

				szThis = img->GetTextSize(font->font, CSTRP(lbl, nextPos));
				dist = (szLast.x + szThis.x) * 0.5;
				nextPt.x = currPt.x + dist * cosAngle;
				nextPt.y = currPt.y - dist * sinAngle;
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
										currPt.x = currPt.x - (nextPt.x * Math_Sqrt(diff.y) / Math_Sqrt(diff.x));
									}
									else
									{
										currPt.x = currPt.x + (nextPt.x * Math_Sqrt(diff.y) / Math_Sqrt(diff.x));
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
						angleDegree = (angle * 180.0 / PI);
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
	realBounds->tl = min;
	realBounds->br = max;
}

void Map::MapConfig2TGen::GetCharsSize(NotNullPtr<Media::DrawImage> img, Math::Coord2DDbl *size, Text::CStringNN label, Data::ArrayList<MapFontStyle*> *fontStyle, Double scaleW, Double scaleH)
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

	font = (Map::MapFontStyle*)fontStyle->GetItem(maxIndex);
	Math::Size2DDbl szTmp = img->GetTextSize(font->font, label);

	if (scaleH == 0)
	{
		size->x = szTmp.x + xSizeAdd;
		size->y = szTmp.y + ySizeAdd;

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
	size->x = maxX - minX;
	size->y = maxY - minY;
}

UInt32 Map::MapConfig2TGen::ToColor(const UTF8Char *str)
{
	UInt32 v = (UInt32)Text::StrHex2Int32C(str);
	return 0xff000000 | ((v & 0xff) << 16) | (v & 0xff00) | ((v >> 16) & 0xff);
}

Map::MapDrawLayer *Map::MapConfig2TGen::GetDrawLayer(Text::CStringNN name, Data::ArrayList<Map::MapDrawLayer*> *layerList, IO::Writer *errWriter)
{
	Map::CIPLayer2 *cip;
	UOSInt i = layerList->GetCount();
	while (i-- > 0)
	{
		Map::MapDrawLayer *lyr;
		lyr = layerList->GetItem(i);
		if (IO::Path::FileNameCompare(name.v, lyr->GetName()->v) == 0)
		{
			return lyr;
		}
	}
	NEW_CLASS(cip, Map::CIPLayer2(name));
	if (cip->IsError())
	{
		errWriter->WriteStrC(UTF8STRC("Error in loading "));
		errWriter->WriteLineC(name.v, name.leng);
		DEL_CLASS(cip);
		return 0;
	}
	layerList->Add(cip);
	return cip;
}

void Map::MapConfig2TGen::DrawPoints(NotNullPtr<Media::DrawImage> img, MapLayerStyle *lyrs, Map::MapView *view, Bool *isLayerEmpty, Map::MapScheduler *sch, NotNullPtr<Media::DrawEngine> eng, Media::IImgResizer *resizer, Math::RectAreaDbl *objBounds, UOSInt *objCnt, UOSInt maxObjCnt)
{
	Math::Geometry::Vector2D *vec;
	UOSInt imgW;
	UOSInt imgH;
	UOSInt i;
#ifdef NOSCH
	UOSInt j;
	Int32 pts[2];
	Double *objPtr = &objBounds[4 * *objCnt];
#endif
	Map::GetObjectSess *session;
	NotNullPtr<Map::MapDrawLayer> lyr;
	if (!lyr.Set(lyrs->lyr))
		return;

#ifndef NOSCH
	sch->SetDrawType(lyr, 0, 0, lyrs->img, UOSInt2Double(lyrs->img->GetWidth()) * 0.5, UOSInt2Double(lyrs->img->GetHeight()) * 0.5, isLayerEmpty);
	sch->SetDrawObjs(objBounds, objCnt, maxObjCnt);
#endif
	Data::ArrayListInt64 arri;
	Math::RectAreaDbl rect = view->GetVerticalRect();
	lyrs->lyr->GetObjectIdsMapXY(arri, 0, Math::RectAreaDbl(rect.tl - rect.GetSize(), rect.br + rect.GetSize()), true);
	if (arri.GetCount() <= 0)
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
		Media::DrawImage *gimg = eng->CreateImage32(Math::Size2D<UOSInt>((UOSInt)Double2OSInt(UOSInt2Double(imgW) * img->GetHDPI() / 96.0), (UOSInt)Double2OSInt(UOSInt2Double(imgH) * img->GetHDPI() / 96.0)), gimg2->GetAlphaType());
		gimg->SetAlphaType(gimg2->GetAlphaType());
		Bool revOrder;
		Bool revOrder2;
		UInt8 *bmpBits = gimg->GetImgBits(revOrder);
		UInt8 *bmpBits2 = gimg2->GetImgBits(revOrder2);
		resizer->Resize(bmpBits2, (OSInt)imgW << 2, UOSInt2Double(imgW), UOSInt2Double(imgH), 0, 0, bmpBits, Double2Int32(UOSInt2Double(imgW) * img->GetHDPI() / 96.0) << 2, (UInt32)Double2Int32(UOSInt2Double(imgW) * img->GetHDPI() / 96.0), (UInt32)Double2Int32(UOSInt2Double(imgH) * img->GetHDPI() / 96.0));
		gimg->GetImgBitsEnd(true);
		gimg2->GetImgBitsEnd(false);
		dimg = gimg;
		imgW = (UInt32)Double2Int32(UOSInt2Double(imgW) * img->GetHDPI() / 96.0) >> 1;
		imgH = (UInt32)Double2Int32(UOSInt2Double(imgH) * img->GetHDPI() / 96.0) >> 1;
#ifndef NOSCH
		sch->SetDrawType(lyr, 0, 0, dimg, UOSInt2Double(dimg->GetWidth()) * 0.5, UOSInt2Double(dimg->GetHeight()) * 0.5, isLayerEmpty);
#endif
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
		if ((vec = lyrs->lyr->GetNewVectorById(session, arri.GetItem(i))) != 0)
		{
#ifdef NOSCH
			j = dobj->nPoints;
			while (j-- > 0)
			{
				if (view->LatLonToScnXY(&dobj->points[j << 1], pts, 1, 0, 0))
					*isLayerEmpty = false;
				img->DrawImagePt(dimg, objPtr[0] = pts[0] - imgW, objPtr[1] = pts[1] - imgH);
				objPtr[2] = pts[0] + imgW;
				objPtr[3] = pts[1] + imgH;
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
	NotNullPtr<Media::DrawImage> tmpImg;
	if (img->GetHDPI() != 96 && tmpImg.Set(dimg))
	{
		eng->DeleteImage(tmpImg);
	}
}

void Map::MapConfig2TGen::DrawString(NotNullPtr<Media::DrawImage> img, MapLayerStyle *lyrs, Map::MapView *view, Data::ArrayList<MapFontStyle*> **fonts, MapLabels2 *labels, UOSInt maxLabels, UOSInt *labelCnt, Bool *isLayerEmpty)
{
	Map::NameArray *arr;
	UOSInt i;
	Math::Geometry::Vector2D *vec;
	Double scaleW;
	Double scaleH;
	Math::Coord2DDbl pts;
	UTF8Char *sptr;
	UTF8Char *sptrEnd;
	UTF8Char lblStr[128];
	Map::GetObjectSess *session;
	UOSInt imgWidth;
	UOSInt imgHeight;

	if ((lyrs->bkColor & SFLG_SMART) == 0 && labelCnt[2] == 0)
	{
		return;
	}

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
	Math::RectAreaDbl rect = view->GetVerticalRect();
	Double tmpSize = rect.GetWidth() * 1.5;
	lyrs->lyr->GetObjectIdsMapXY(arri, &arr, Math::RectAreaDbl(rect.tl - tmpSize, rect.br + tmpSize), false);
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
					sptrEnd = lyrs->lyr->GetString(sptr = lblStr, sizeof(lblStr), arr, arri.GetItem(i), 0);
					AddLabel(labels, maxLabels, labelCnt, CSTRP(sptr, sptrEnd), 1, &pt, lyrs->priority, Map::DRAW_LAYER_POINT, lyrs->style, lyrs->bkColor, view, (UOSInt2Double(imgWidth) * view->GetHDPI() / view->GetDDPI()), (UOSInt2Double(imgHeight) * view->GetHDPI() / view->GetDDPI()));
					break;
				}
				case Math::Geometry::Vector2D::VectorType::Polyline:
				case Math::Geometry::Vector2D::VectorType::Polygon:	
				{
					Math::Geometry::PointOfstCollection *ptOfst = (Math::Geometry::PointOfstCollection*)vec;
					UOSInt k;
					UInt32 maxSize;
					UInt32 maxPos;
					UOSInt nPtOfst;
					UInt32 *ptOfstArr = ptOfst->GetPtOfstList(nPtOfst);
					UOSInt nPoint;
					Math::Coord2DDbl *pointArr = ptOfst->GetPointList(nPoint);
					maxSize = (UInt32)nPoint - (maxPos = ptOfstArr[nPtOfst - 1]);
					k = nPtOfst;
					while (k-- > 1)
					{
						if ((ptOfstArr[k] - ptOfstArr[k - 1]) > maxSize)
							maxSize = (ptOfstArr[k] - (maxPos = ptOfstArr[k - 1]));
					}
					sptrEnd = lyrs->lyr->GetString(sptr = lblStr, sizeof(lblStr), arr, arri.GetItem(i), 0);
					AddLabel(labels, maxLabels, labelCnt, CSTRP(sptr, sptrEnd), maxSize, &pointArr[maxPos], lyrs->priority, lyrs->lyr->GetLayerType(), lyrs->style, lyrs->bkColor, view, (UOSInt2Double(imgWidth) * view->GetHDPI() / view->GetDDPI()), (UOSInt2Double(imgHeight) * view->GetHDPI() / view->GetDDPI()));
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
				sptrEnd = lyrs->lyr->GetString(sptr = lblStr, sizeof(lblStr), arr, arri.GetItem(i), 0);
				switch (vec->GetVectorType())
				{
				case Math::Geometry::Vector2D::VectorType::Polyline:
				{
					Math::Geometry::Polyline *pl = (Math::Geometry::Polyline*)vec;
					UOSInt nPoint;
					Math::Coord2DDbl *pointArr = pl->GetPointList(nPoint);
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


					if ((lyrs->bkColor & SFLG_ROTATE) == 0)
						scaleW = scaleH = 0;
					Math::Coord2DDbl szThis;
					GetCharsSize(img, &szThis, CSTRP(sptr, sptrEnd), fonts[lyrs->style], scaleW, scaleH);
					pts = view->MapXYToScnXY(pts);
					if ((pts.x + (szThis.x * 0.5)) >= 0 && (pts.y + (szThis.y * 0.5)) >= 0 && (pts.x - (szThis.x * 0.5)) <= UOSInt2Double(view->GetScnWidth()) && (pts.y - (szThis.y * 0.5)) <= UOSInt2Double(view->GetScnHeight()))
					{
						DrawChars(img, CSTRP(sptr, sptrEnd), pts.x, pts.y, scaleW, scaleH, fonts[lyrs->style], (lyrs->bkColor & SFLG_ALIGN) != 0);
					}
					break;
				}
				case Math::Geometry::Vector2D::VectorType::Polygon:
				{
					Math::Geometry::Polygon *pg = (Math::Geometry::Polygon*)vec;
					UOSInt nPoint;
					Math::Coord2DDbl *pointArr = pg->GetPointList(nPoint);
					UOSInt nPtOfst;
					UInt32 *ptOfstArr = pg->GetPtOfstList(nPtOfst);
					pts = Math::GeometryTool::GetPolygonCenter(nPtOfst, nPoint, ptOfstArr, pointArr);
					Math::Coord2DDbl szThis;
					GetCharsSize(img, &szThis, CSTRP(sptr, sptrEnd), fonts[lyrs->style], 0, 0);
					pts = view->MapXYToScnXY(pts);
					if ((pts.x + (szThis.x * 0.5)) >= 0 && (pts.y + (szThis.y * 0.5)) >= 0 && (pts.x - (szThis.x * 0.5)) <= UOSInt2Double(view->GetScnWidth()) && (pts.y - (szThis.y * 0.5)) <= UOSInt2Double(view->GetScnHeight()))
					{
						DrawChars(img, CSTRP(sptr, sptrEnd), pts.x, pts.y, 0, 0, fonts[lyrs->style], (lyrs->bkColor & SFLG_ALIGN) != 0);
					}
					break;
				}
				case Math::Geometry::Vector2D::VectorType::Point:
				{
					pts = vec->GetCenter();
					Math::Coord2DDbl szThis;
					GetCharsSize(img, &szThis, CSTRP(sptr, sptrEnd), fonts[lyrs->style], 0, 0);
					pts = view->MapXYToScnXY(pts);
					if ((pts.x + (szThis.x * 0.5)) >= 0 && (pts.y + (szThis.y * 0.5)) >= 0 && (pts.x - (szThis.x * 0.5)) <= UOSInt2Double(view->GetScnWidth()) && (pts.y - (szThis.y * 0.5)) <= UOSInt2Double(view->GetScnHeight()))
					{
						DrawChars(img, CSTRP(sptr, sptrEnd), pts.x, pts.y, 0, 0, fonts[lyrs->style], (lyrs->bkColor & SFLG_ALIGN) != 0);
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

UOSInt Map::MapConfig2TGen::NewLabel(MapLabels2 *labels, UOSInt maxLabel, UOSInt *labelCnt, Int32 priority)
{
	Int32 minPriority;
	UOSInt i;
	UOSInt j;
	Double k;

	if (*labelCnt >= maxLabel)
	{
		i = *labelCnt;
		minPriority = labels[0].priority;
		j = (UOSInt)-1;
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
		if (j == (UOSInt)-1)
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


Bool Map::MapConfig2TGen::AddLabel(MapLabels2 *labels, UOSInt maxLabel, UOSInt *labelCnt, Text::CString labelt, UOSInt nPoint, Math::Coord2DDbl *points, Int32 priority, Int32 recType, UInt32 fontStyle, UInt32 flags, Map::MapView *view, Double xOfst, Double yOfst)
{
	Double size;
//	Int32 visibleSize;

	UOSInt i;
	UOSInt j;

	Math::Coord2DDbl *ptPtr;

	Math::Coord2DDbl scnPos;
	Double scnSqrLen;
	Int32 found;

/*	Int32 left = view->GetLeft();
	Int32 right = view->GetRight();
	Int32 top = view->GetTop();
	Int32 bottom = view->GetBottom();
	Int32 mapPosX = view->GetCentX();
	Int32 mapPosY = view->GetCentY();*/

	if (recType == 1) //Point
	{
		Math::Coord2DDbl sumVal;

		found = 0;
		i = 0;
		while (i < *labelCnt)
		{
			if (recType == labels[i].shapeType)
			{
				if (labels[i].label->Equals(labelt))
				{
					found = 1;

					sumVal.x = labels[i].scaleW * labels[i].totalSize;
					sumVal.y = labels[i].scaleH * labels[i].totalSize;

					ptPtr = points;
					j = nPoint;
					while (j--)
					{
						sumVal += *ptPtr++;
					}
					labels[i].totalSize += UOSInt2Double(nPoint);
					labels[i].scaleW = (sumVal.x / labels[i].totalSize);
					labels[i].scaleH = (sumVal.y / labels[i].totalSize);

					scnPos.x = labels[i].scaleW - labels[i].pos.x;
					scnPos.y = labels[i].scaleH - labels[i].pos.y;
					scnPos = scnPos * scnPos;;
					labels[i].currSize = scnPos.x + scnPos.y;

					ptPtr = points;
					j = nPoint;
					while (j--)
					{
						scnPos.x = labels[i].scaleW - ptPtr[0].x;
						scnPos.y = labels[i].scaleH - ptPtr[0].y;
						scnPos = scnPos * scnPos;
						scnSqrLen = scnPos.x + scnPos.y;
						if (scnSqrLen < labels[i].currSize)
						{
							labels[i].pos = ptPtr[0];
							labels[i].fontStyle = fontStyle;
							labels[i].currSize = scnSqrLen;
							labels[i].xOfst = xOfst;
							labels[i].yOfst = yOfst;
							if (priority > labels[i].priority)
								labels[i].priority = priority;
						}
						ptPtr++;
					}
					return true;
				}
			}
			i++;
		}

		if (found == 0 && nPoint > 0)
		{
			i = (UOSInt)-1;
			sumVal = {0, 0};
			ptPtr = points;
			j = nPoint;
			while (j--)
			{
				sumVal += *ptPtr++;
			}

			found = 1;

			i = NewLabel(labels, maxLabel, labelCnt, priority);
			if (i < 0)
			{
				return false;
			}

			labels[i].totalSize = UOSInt2Double(nPoint);
			labels[i].scaleW = (sumVal.x / UOSInt2Double(nPoint));
			labels[i].scaleH = (sumVal.y / UOSInt2Double(nPoint));
			labels[i].label = Text::String::New(labelt).Ptr();
			labels[i].pos = points[0];
			labels[i].fontStyle = fontStyle;
			labels[i].priority = priority;

			scnPos.x = labels[i].scaleW - points[0].x;
			scnPos.y = labels[i].scaleH - points[0].y;
			scnPos = scnPos * scnPos;
			labels[i].currSize = scnPos.x + scnPos.y;
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
				scnPos.x = labels[i].scaleW - ptPtr->x;
				scnPos.y = labels[i].scaleH - ptPtr->y;
				scnPos = scnPos * scnPos;
				scnSqrLen = scnPos.x + scnPos.y;
				if (scnSqrLen < labels[i].currSize)
				{
					labels[i].pos = ptPtr[0];
					labels[i].currSize = scnSqrLen;
					if (priority > labels[i].priority)
						labels[i].priority = priority;
				}
				ptPtr++;
			}
			return true;
		}
		return false;
	}
	else if (recType == 3) //lines
	{
		Double tmp;
		Math::Coord2DDbl lastPt = points[0];
		Math::Coord2DDbl thisPt;

		Int32 toUpdate;

//		visibleSize = 0;
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

/*			if (left < thisPtX && right > lastPtX && top < thisPtY && bottom > lastPtY)
			{
				if (left > lastPtX)
				{
					lastPtY += MulDiv(left - lastPtX, thisPtY - lastPtY, thisPtX - lastPtX);
					lastPtX = left;
				}
				if (top > lastPtY)
				{
					lastPtX += MulDiv(top - lastPtY, thisPtX - lastPtX, thisPtY - lastPtY);
					lastPtY = top;
				}
				if (right < thisPtX)
				{
					thisPtY += MulDiv(right - lastPtX, thisPtY - lastPtY, thisPtX - lastPtX);
					thisPtX = right;
				}
				if (bottom < thisPtY)
				{
					thisPtX += MulDiv(bottom - lastPtY, thisPtX - lastPtX, thisPtY - lastPtY);
					thisPtY = bottom;
				}

				if ((thisPtY - lastPtY) > (thisPtX - lastPtX))
					visibleSize += thisPtY - lastPtY;
				else
					visibleSize += thisPtX - lastPtX;

			}*/
			i++;
		}

		toUpdate = 0;
		found = 0;
		Double totalSize = 0;
		UOSInt foundInd = 0;
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
						labels[i].currSize += size;//visibleSize;
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
						labels[i].currSize += size;//visibleSize;
						toUpdate = 0;
						foundInd = i;
					}
					else if (labels[i].points[0] == startPt)
					{
//						wprintf(L"Shape: %s inverse merged (%d + %d)\n", labelt, labels[i].nPoints, nPoint);
						UOSInt newSize = labels[i].nPoints + nPoint - 1;
						Math::Coord2DDbl* newArr = MemAllocA(Math::Coord2DDbl, newSize);
						UOSInt k;
						UOSInt l;
						l = 0;
						k = labels[i].nPoints;
						while (k-- > 1)
						{
							newArr[l++] = labels[i].points[k];
						}
						MemCopyNO(&newArr[l], points, nPoint << 4);

						startPt = newArr[0];
						endPt = newArr[newSize - 1];

						MemFreeA(labels[i].points);
						labels[i].points = newArr;
						labels[i].nPoints = newSize;
						labels[i].currSize += size;//visibleSize;
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
						labels[i].currSize += size;//visibleSize;
						toUpdate = 0;
						foundInd = i;
					}
					else
					{
						if (labels[i].currSize < size)//visibleSize)
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
		if (found == 0)// && visibleSize > 0)
		{
//			wprintf(L"New Label: %s, nPoint = %d\n", labelt, nPoint);
			i = NewLabel(labels, maxLabel, labelCnt, priority);

			if (i < 0)
				return false;

			labels[i].shapeType = 3;
			labels[i].fontStyle = fontStyle;
			labels[i].totalSize = size;
			labels[i].currSize = size;//visibleSize;
			labels[i].flags = flags;

			labels[i].label = Text::String::New(labelt).Ptr();
			labels[i].points = 0;

			toUpdate = 1;
			foundInd = i;
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
		else if (found)
		{
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

			outPts = MemAllocA(Math::Coord2DDbl, nPoint);
			outPtCnt = 0;
			MemCopyNO(outPts, points, nPoint << 4);
			outPtCnt = nPoint;

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
						thisPt.x = lastPt.x + (thisPt.y - lastPt.y) * (thisT.x - lastPt.x) / (thisT.y - lastPt.y);
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
				labels[i].scaleW = (max.x + min.x) * 0.5; //& 1
				labels[i].scaleH = (max.y + min.y) * 0.5; //& 1
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

void Map::MapConfig2TGen::SwapLabel(MapLabels2 *mapLabels, UOSInt index, UOSInt index2)
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

Bool Map::MapConfig2TGen::LabelOverlapped(Math::RectAreaDbl *points, UOSInt nPoints, Math::RectAreaDbl rect)
{
	while (nPoints--)
	{
		if (rect.OverlapOrTouch(points[nPoints])) return true;
	}

	return false;
}

void Map::MapConfig2TGen::DrawLabels(NotNullPtr<Media::DrawImage> img, MapLabels2 *labels, UOSInt maxLabel, UOSInt *labelCnt, Map::MapView *view, Data::ArrayList<MapFontStyle*> **fonts, NotNullPtr<Media::DrawEngine> drawEng, Math::RectAreaDbl *objBounds, UOSInt *objCnt)
{
	UOSInt i;
	UOSInt j;
	Text::String *lastLbl = 0;
/*	Int32 left = view->GetLeft();
	Int32 top = view->GetTop();
	Int32 right = view->GetRight();
	Int32 bottom = view->GetBottom();*/
/*	Int32 scnWidth = view->GetScnWidth();
	Int32 scnHeight = view->GetScnHeight();
	Int32 scaleWidth = right - left;
	Int32 scaleHeight = bottom - top;*/
	MapLogger *log = (MapLogger*)(OSInt)labelCnt[1];

	if (*labelCnt)
	{
		Math::Coord2DDbl szThis;
		UOSInt currPt;

		Math::RectAreaDbl rect;
		Math::Coord2DDbl scnPt;
		Double thisPts[10];
		Int32 thisCnt = 0;

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
				GetCharsSize(img, &szThis, labels[i].label->ToCString(), fonts[labels[i].fontStyle], 0, 0);//labels[i].scaleW, labels[i].scaleH);
				Math::Coord2DDbl scnD = view->MapXYToScnXY(labels[i].pos / labels[i].mapRate);
				scnPt = scnD;

				j = 1;
				if (labels[i].xOfst == 0)
				{
					rect.tl = scnPt - (szThis * 0.5);
					rect.br = rect.tl + szThis;

					j = LabelOverlapped(objBounds, currPt, rect);
				}
				if (j)
				{
					rect.tl.x = scnPt.x + 1 + (labels[i].xOfst * 0.5);
					rect.tl.y = scnPt.y - (szThis.y * 0.5);
					rect.br = rect.tl + szThis;

					j = LabelOverlapped(objBounds, currPt, rect);
				}
				if (j)
				{
					rect.tl.x = scnPt.x - szThis.x - 1 - (labels[i].xOfst * 0.5);
					rect.tl.y = scnPt.y - (szThis.y * 0.5);
					rect.br = rect.tl + szThis;

					j = LabelOverlapped(objBounds, currPt, rect);
				}
				if (j)
				{
					rect.tl.x = scnPt.x - (szThis.x * 0.5);
					rect.tl.y = scnPt.y - szThis.y - 1 - (labels[i].yOfst * 0.5);
					rect.br = rect.tl + szThis;

					j = LabelOverlapped(objBounds, currPt, rect);
				}
				if (j)
				{
					rect.tl.x = scnPt.x - (szThis.x * 0.5);
					rect.tl.y = scnPt.y + 1 + (labels[i].yOfst * 0.5);
					rect.br = rect.tl + szThis;

					j = LabelOverlapped(objBounds, currPt, rect);
				}

				if (!j)
				{
					Math::Coord2DDbl center = rect.GetCenter();
					DrawChars(img, labels[i].label->ToCString(), center.x, center.y, 0, 0, fonts[labels[i].fontStyle], 0);

					objBounds[currPt] = rect;

					log->AddString(labels[i].label->v, center.x, center.y, 0, 0, labels[i].fontStyle, 0, &objBounds[currPt]);

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

//				Int32 *points = (Int32*)MemAlloc(labels[i].nPoints << 3);
//				view->MapToScnXY(labels[i].points, points, labels[i].nPoints, 0, 0);
				Math::Coord2DDbl min;
				Math::Coord2DDbl max;
				Math::Coord2DDbl diff;
				Double scaleN;
				Double scaleD;
				Math::Coord2DDbl lastPt;
				Math::Coord2DDbl thisPt;
				UInt32 k;
//				Bool hasPoint;

				j = (labels[i].nPoints) - 1;
				lastPt = labels[i].points[j];
				max = min = lastPt;
				while (j > 0)
				{
					j -= 1;
					thisPt = labels[i].points[j];
					min = min.Min(thisPt);
					max = max.Max(thisPt);
				}
/*				lastX = points[j];
				lastY = points[j + 1];
				if (lastX >= 0 && lastX < scnWidth && lastY >= 0 && lastY < scnHeight)
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
					lastX = points[j];
					lastY = points[j + 1];

					j -= 2;

					thisX = points[j];
					thisY = points[j + 1];

					if (lastX > scnWidth)
					{
						if (thisX > scnWidth)
						{
							continue;
						}
						else
						{
							lastY = thisY + MulDiv(lastY - thisY, scnWidth - thisX, lastX - thisX);
							lastX = scnWidth;
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
							lastY = thisY + MulDiv(lastY - thisY, 0 - thisX, lastX - thisX);
							lastX = 0;
						}
					}

					if (thisX < 0)
					{
						thisY = lastY + MulDiv(thisY - lastY, 0 - lastX, thisX - lastX);
						thisX = 0;
					}
					else if (thisX > scnWidth)
					{
						thisY = lastY + MulDiv(thisY - lastY, scnWidth - lastX, thisX - lastX);
						thisX = scnWidth;
					}


					if (lastY > scnHeight)
					{
						if (thisY > scnHeight)
						{
							continue;
						}
						else
						{
							lastX = thisX + MulDiv(lastX - thisX, scnHeight - thisY, lastY - thisY);
							lastY = scnHeight;
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
							lastX = thisX + MulDiv(lastX - thisX, 0 - thisY, lastY - thisY);
							lastY = 0;
						}
					}

					if (thisY < 0)
					{
						thisX = lastX + MulDiv(thisX - lastX, 0 - lastY, thisY - lastY);
						thisY = 0;
					}
					else if (thisY > scnHeight)
					{
						thisX = lastX + MulDiv(thisX - lastX, scnHeight - lastY, thisY - lastY);
						thisY = scnHeight;
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
					j = (labels[i].nPoints - 1) & ~1;
					minX = maxX = points[j];
					minY = maxY = points[j + 1];
					if (points[j + 2] > minX)
					{
						maxX = points[j + 2];
					}
					else
					{
						minX = points[j + 2];
					}
					if (points[j + 3] > minY)
					{
						maxY = points[j + 3];
					}
					else
					{
						minY = points[j + 3];
					}
				}*/
				diff = max - min;
				scaleN = 0;
				scaleD = 1;

				if (diff.x == 0 && diff.y == 0)
				{
					k = 0;
				}
				else
				{
					if (min.x > max.x || min.y > max.y)
					{
					//	wprintf(L"Error in handling label: %s\n", lastLbl);
					}
					if (diff.x > diff.y)
					{
						scnPt.x = (max.x + min.x) * 0.5;
						k = 0;
						while ((UOSInt)k < labels[i].nPoints - 1)
						{
							if (labels[i].points[k].x >= scnPt.x && labels[i].points[k + 1].x <= scnPt.x)
							{
								scaleD = labels[i].points[k].x - labels[i].points[k + 1].x;
								scaleN = (labels[i].points[k].x - scnPt.x);
								break;
							}
							else if (labels[i].points[k].x <= scnPt.x && labels[i].points[k + 1].x >= scnPt.x)
							{
								scaleD = labels[i].points[k + 1].x - labels[i].points[k].x;
								scaleN = scnPt.x - labels[i].points[k].x;
								break;
							}
							k++;
						}

					}
					else
					{
						scnPt.y = (max.y + min.y) * 0.5;
						k = 0;
						while ((UOSInt)k < labels[i].nPoints - 1)
						{
							if (labels[i].points[k].y >= scnPt.y && labels[i].points[k + 1].y <= scnPt.y)
							{
								scaleD = labels[i].points[k].y - labels[i].points[k + 1].y;
								scaleN = (labels[i].points[k].y - scnPt.y);
								break;
							}
							else if (labels[i].points[k].y <= scnPt.y && labels[i].points[k + 1].y >= scnPt.y)
							{
								scaleD = labels[i].points[k + 1].y - labels[i].points[k].y;
								scaleN = scnPt.y - labels[i].points[k].y;
								break;
							}
							k++;
						}
					}
				}

				labels[i].scaleW = labels[i].points[k + 1].x - labels[i].points[k].x;
				labels[i].scaleH = labels[i].points[k + 1].y - labels[i].points[k].y;
				GetCharsSize(img, &szThis, labels[i].label->ToCString(), fonts[labels[i].fontStyle], labels[i].scaleW, labels[i].scaleH);
//				scnPtX = labels[i].points[(k << 1)] + MulDiv(labels[i].points[(k << 1) + 2] - labels[i].points[(k << 1)], scaleN, scaleD);
//				scnPtY = labels[i].points[(k << 1) + 1] + MulDiv(labels[i].points[(k << 1) + 3] - labels[i].points[(k << 1) + 1], scaleN, scaleD);
				Math::Coord2DDbl dscnPt;
				Math::Coord2DDbl dblArr[3];
				dblArr[0] = min / labels[i].mapRate;
				dblArr[1] = max / labels[i].mapRate;
				if (scaleD == 0)
				{
					dscnPt = labels[i].points[k];
				}
				else
				{
					dscnPt = labels[i].points[k] + (labels[i].points[k + 1] - labels[i].points[k]) / (Double)scaleD * scaleN;
				}
				dblArr[2] = dscnPt / labels[i].mapRate;
				Math::Coord2D<Int32> tmpArr[3];
/*				tmpArr[0] = minX;
				tmpArr[1] = minY;
				tmpArr[2] = maxX;
				tmpArr[3] = maxY;
				tmpArr[4] = scnPtX;
				tmpArr[5] = scnPtY;*/
				view->MapXYToScnXY(dblArr, tmpArr, 3, Math::Coord2D<Int32>(0, 0));

				if ((tmpArr[1].x - tmpArr[0].x) < szThis.x && (tmpArr[0].y - tmpArr[1].y) < szThis.y)
				{
					rect.tl = tmpArr[2].ToDouble() - (szThis * 0.5);
					rect.br = rect.tl + szThis;

					j = LabelOverlapped(objBounds, currPt, rect);
					if (j == 0)
					{
						Math::Coord2DDbl center = rect.GetCenter();
						DrawChars(img, labels[i].label->ToCString(), center.x, center.y, labels[i].scaleW, labels[i].scaleH, fonts[labels[i].fontStyle], (labels[i].flags & SFLG_ALIGN) != 0);

						objBounds[currPt] = rect;

						log->AddString(labels[i].label->v, center.x, center.y, labels[i].scaleW, labels[i].scaleH, labels[i].fontStyle, (labels[i].flags & SFLG_ALIGN) != 0, &objBounds[currPt]);

						currPt++;
					}
				}
				else
				{
					Double scnDiff = (max.x - min.x) * 0.5;
					j = 1;
					while (j)
					{
						Math::Coord2DDbl tmpD = view->MapXYToScnXY(dscnPt / labels[i].mapRate);
						tmpArr[2].x = Double2Int32(tmpD.x);
						tmpArr[2].y = Double2Int32(tmpD.y);
						rect.tl = tmpArr[2].ToDouble() - (szThis * 0.5);
						rect.br = rect.tl + szThis;

						j = LabelOverlapped(objBounds, currPt, rect);
						if (j == 0)
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
									dscnPt.x += scnDiff - 1;
									if (dscnPt.x <= min.x)
									{
										scnDiff = -scnDiff;
										dscnPt.x = ((min.x + max.x) * 0.5) + scnDiff + 1;
									}
								}
								else
								{
									dscnPt.x += scnDiff + 1;
									if (dscnPt.x >= max.x)
									{
										scnDiff = scnDiff * 0.51;
										if (scnDiff < 30)
											break;
										scnDiff = -scnDiff;
										dscnPt.x = ((min.x + max.x) * 0.5) + scnDiff - 1;
									}
								}

								k = 0;
								while ((UOSInt)k < labels[i].nPoints - 1)
								{
									if (labels[i].points[k].x >= dscnPt.x && labels[i].points[k + 1].x <= dscnPt.x)
									{
										scaleD = labels[i].points[k].x - labels[i].points[k + 1].x;
										scaleN = (labels[i].points[k].x - dscnPt.x);
										break;
									}
									else if (labels[i].points[k].x <= dscnPt.x && labels[i].points[k + 1].x >= dscnPt.x)
									{
										scaleD = labels[i].points[k + 1].x - labels[i].points[k].x;
										scaleN = (dscnPt.x - labels[i].points[k].x);
										break;
									}
									k++;
								}

							}
							else
							{
								if (scnDiff < 0)
								{
									dscnPt.y += scnDiff - 1;
									if (dscnPt.y <= min.y)
									{
										scnDiff = -scnDiff;
										dscnPt.y = ((min.y + max.y) * 0.5) + scnDiff + 1;
									}
								}
								else
								{
									dscnPt.y += scnDiff + 1;
									if (dscnPt.y >= max.y)
									{
										scnDiff = scnDiff * 0.5;
										if (scnDiff < 30)
											break;
										scnDiff = -scnDiff;
										dscnPt.y = ((min.y + max.y) * 0.5) + scnDiff - 1;
									}
								}

								k = 0;
								while ((UOSInt)k < labels[i].nPoints - 1)
								{
									if (labels[i].points[k].y >= dscnPt.y && labels[i].points[k + 1].y <= dscnPt.y)
									{
										scaleD = labels[i].points[k].y - labels[i].points[k + 1].y;
										scaleN = (labels[i].points[k].y - dscnPt.y);
										break;
									}
									else if (labels[i].points[k].y <= dscnPt.y && labels[i].points[k + 1].y >= dscnPt.y)
									{
										scaleD = labels[i].points[k + 1].y - labels[i].points[k].y;
										scaleN = (dscnPt.y - labels[i].points[k].y);
										break;
									}
									k++;
								}
							}
							dscnPt = labels[i].points[k] + (labels[i].points[k + 1] - labels[i].points[k]) / (Double)scaleD * scaleN;
							labels[i].scaleW = labels[i].points[k + 1].x - labels[i].points[k].x;
							labels[i].scaleH = labels[i].points[k + 1].y - labels[i].points[k].y;
							GetCharsSize(img, &szThis, labels[i].label->ToCString(), fonts[labels[i].fontStyle], labels[i].scaleW, labels[i].scaleH);
						}
					}

					if (!j && thisCnt < 10)
					{
						long m;
						long n = 1;
						Double tmpV;
						m = thisCnt;
						while (n && m > 0)
						{
							n = 0;
							tmpV = thisPts[--m];
							if ((tmpV - LBLMINDIST) < rect.br.y && (tmpV + LBLMINDIST) > rect.tl.y)
							{
								n++;
							}
							tmpV = thisPts[--m];
							if ((tmpV - LBLMINDIST) < rect.br.x && (tmpV + LBLMINDIST) > rect.tl.x)
							{
								n++;
							}
							n = (n != 2);
						}

						if (n)
						{
							Math::Coord2DDbl center = rect.GetCenter();
							Math::Coord2D<Int32> *points;
							if ((labels[i].flags & SFLG_ALIGN) != 0)
							{
								Math::RectAreaDbl realBounds;
								points = MemAlloc(Math::Coord2D<Int32>, labels[i].nPoints);
								view->MapXYToScnXY(labels[i].points, points, labels[i].nPoints, Math::Coord2D<Int32>(0, 0));
								DrawCharsLA(img, labels[i].label->ToCString(), labels[i].points, points, labels[i].nPoints, k, scaleN, scaleD, fonts[labels[i].fontStyle], &realBounds);
								DrawCharsLA(img, labels[i].label->ToCString(), labels[i].points, points, labels[i].nPoints, k, scaleN, scaleD, fonts[labels[i].fontStyle], &realBounds);
								MemFree(points);

								objBounds[currPt] = realBounds;
								currPt++;
							}
							else if ((labels[i].flags & SFLG_ROTATE) != 0)
							{
								Math::RectAreaDbl realBounds;
								points = MemAlloc(Math::Coord2D<Int32>, labels[i].nPoints);
								view->MapXYToScnXY(labels[i].points, points, labels[i].nPoints, Math::Coord2D<Int32>(0, 0));
								DrawCharsL(img, labels[i].label->ToCString(), labels[i].points, points, labels[i].nPoints, k, scaleN, scaleD, fonts[labels[i].fontStyle], &realBounds);
								log->AddStringL(labels[i].label->v, labels[i].points, points, labels[i].nPoints, k, scaleN, scaleD, labels[i].fontStyle, false, &realBounds);
								MemFree(points);

								objBounds[currPt] = realBounds;
								currPt++;
							}
							else
							{
								DrawChars(img, labels[i].label->ToCString(), center.x, center.y, labels[i].scaleW, labels[i].scaleH, fonts[labels[i].fontStyle], (labels[i].flags & SFLG_ALIGN) != 0);

								objBounds[currPt] = rect;
								log->AddString(labels[i].label->v, center.x, center.y, labels[i].scaleW, labels[i].scaleH, labels[i].fontStyle, (labels[i].flags & SFLG_ALIGN) != 0, &objBounds[currPt]);

								currPt++;
							}

							thisPts[thisCnt++] = center.x;
							thisPts[thisCnt++] = center.y;
						}
						j = (thisCnt < 10);
					}
				}
			}
			else if (labels[i].shapeType == 5)
			{
				GetCharsSize(img, &szThis, labels[i].label->ToCString(), fonts[labels[i].fontStyle], 0, 0);//labels[i].scaleW, labels[i].scaleH);
				Math::Coord2DDbl scnD = view->MapXYToScnXY(labels[i].pos / labels[i].mapRate);
				scnPt = scnD;

				j = 1;
				if (j)
				{
					rect.tl = scnPt  - (szThis * 0.5);
					rect.br = rect.tl + szThis;

					j = LabelOverlapped(objBounds, currPt, rect);
				}
				if (j)
				{
					rect.tl.x = scnPt.x + 1;
					rect.tl.y = scnPt.y - (szThis.y * 0.5);
					rect.br = rect.tl + szThis;

					j = LabelOverlapped(objBounds, currPt, rect);
				}
				if (j)
				{
					rect.tl.x = scnPt.x - szThis.x - 1;
					rect.tl.y = scnPt.y - (szThis.y * 0.5);
					rect.br = rect.tl + szThis;

					j = LabelOverlapped(objBounds, currPt, rect);
				}
				if (j)
				{
					rect.tl.x = scnPt.x - (szThis.x * 0.5);
					rect.tl.y = scnPt.y - szThis.y - 1;
					rect.br = rect.tl + szThis;

					j = LabelOverlapped(objBounds, currPt, rect);
				}
				if (j)
				{
					rect.tl.x = scnPt.x - (szThis.x * 0.5);
					rect.tl.y = scnPt.y + 1;
					rect.br = rect.tl + szThis;

					j = LabelOverlapped(objBounds, currPt, rect);
				}

				if (!j)
				{
					Math::Coord2DDbl center = rect.GetCenter();
					DrawChars(img, labels[i].label->ToCString(), center.x, center.y, 0, 0, fonts[labels[i].fontStyle], 0);

					objBounds[currPt] = rect;

					log->AddString(labels[i].label->v, center.x, center.y, 0, 0, labels[i].fontStyle, 0, &objBounds[currPt]);

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

void Map::MapConfig2TGen::LoadLabels(NotNullPtr<Media::DrawImage> img, Map::MapConfig2TGen::MapLabels2 *labels, UOSInt maxLabel, UOSInt *labelCnt, Map::MapView *view, Data::ArrayList<MapFontStyle*> **fonts, NotNullPtr<Media::DrawEngine> drawEng, Math::RectAreaDbl *objBounds, UOSInt *objCnt, Text::CString fileName, Int32 xId, Int32 yId, Double xOfst, Double yOfst, IO::Stream *dbStream)
{
	IO::FileStream *fs = 0;
	IO::StreamReader *reader;
	UTF8Char c;
	UTF8Char sbuff[512];
	Text::PString strs[15];
	UTF8Char *sptr;
	UTF8Char *sptr2;
	UOSInt i;
	NotNullPtr<IO::Stream> stm;
	if (stm.Set(dbStream))
	{
		NEW_CLASS(reader, IO::StreamReader(stm));
	}
	else
	{
		sptr = fileName.ConcatTo(sbuff);
		i = Text::StrLastIndexOfCharC(sbuff, (UOSInt)(sptr - sbuff), IO::Path::PATH_SEPERATOR);
		UTF8Char *fname = &sbuff[i + 1];
		fname[-1] = 0;
		i = Text::StrLastIndexOfCharC(sbuff, (UOSInt)(fname - sbuff - 1), IO::Path::PATH_SEPERATOR);
		UTF8Char *fpath = &sbuff[i + 1];
		sptr = Text::StrInt32(fpath, xId >> 5);
		sptr = Text::StrConcatC(sptr, UTF8STRC("_"));
		sptr = Text::StrInt32(sptr, yId >> 5);
		*sptr++ = IO::Path::PATH_SEPERATOR;
		fname = sptr;
		sptr = Text::StrInt32(sptr, xId);
		sptr = Text::StrConcatC(sptr, UTF8STRC("_"));
		sptr = Text::StrInt32(sptr, yId);
		sptr = Text::StrConcatC(sptr, UTF8STRC(".db"));

		NotNullPtr<IO::FileStream> nnfs;
		NEW_CLASSNN(nnfs, IO::FileStream(CSTRP(sbuff, sptr), IO::FileMode::ReadOnly, IO::FileShare::DenyAll, IO::FileStream::BufferType::Normal));
		if (nnfs->IsError())
		{
			nnfs.Delete();
			return;
		}
		fs = nnfs.Ptr();
		NEW_CLASS(reader, IO::StreamReader(nnfs));
	}

	while (reader->ReadLine(sbuff, 255))
	{
		Bool err = false;
		i = Text::StrCSVSplitP(strs, 15, sbuff);
		if (i == 3)
		{
			xOfst = Text::StrToInt32(strs[1].v) * img->GetHDPI() / 96.0;
			yOfst = Text::StrToInt32(strs[2].v) * img->GetHDPI() / 96.0;
		}
		else if (i == 12)
		{
			Int32 lblType;
			Text::PString label;
			Double scnX;
			Double scnY;
			Double minX;
			Double minY;
			Double maxX;
			Double maxY;
			Double scaleW;
			Double scaleH;
			Int32 fontStyle;
			Int32 isAlign;
			lblType = Text::StrToInt32(strs[0].v);
			label = strs[1];
			scnX = Text::StrToDouble(strs[2].v);
			scnY = Text::StrToDouble(strs[3].v);
			minX = Text::StrToDouble(strs[4].v);
			minY = Text::StrToDouble(strs[5].v);
			maxX = Text::StrToDouble(strs[6].v);
			maxY = Text::StrToDouble(strs[7].v);
			scaleW = Text::StrToDouble(strs[8].v);
			scaleH = Text::StrToDouble(strs[9].v);
			fontStyle = Text::StrToInt32(strs[10].v);
			isAlign = Text::StrToInt32(strs[11].v);
			if (lblType == 1)
			{
				if (maxX >= 0 && minX < UOSInt2Double(view->GetScnWidth()) && maxY >= 0 && minY < UOSInt2Double(view->GetScnHeight()))
				{
					DrawChars(img, label.ToCString(), scnX + xOfst, scnY + yOfst, scaleW, scaleH, fonts[fontStyle], isAlign != 0);
				}
			}
			else
			{
				err = true;
			}
		}
		else if (i == 15)
		{
			Bool eol;
			Int32 lblType;
			NotNullPtr<Text::String> label;
//			Double scnX;
//			Double scnY;
			Double minX;
			Double minY;
			Double maxX;
			Double maxY;
			UInt32 nPoints;
			UInt32 ptCurr;
			Double scaleN;
			Double scaleD;
			Int32 fontStyle;
			Int32 isAlign;
			Math::Coord2DDbl *ptArr;
			Math::Coord2DDbl *currPt;
//			Double mapRate;
			lblType = Text::StrToInt32(strs[0].v);
			label = Text::String::New(strs[1].ToCString());
//			scnX = Text::StrToDouble(strs[2].v);
//			scnY = Text::StrToDouble(strs[3].v);
			minX = Text::StrToDouble(strs[4].v);
			minY = Text::StrToDouble(strs[5].v);
			maxX = Text::StrToDouble(strs[6].v);
			maxY = Text::StrToDouble(strs[7].v);
			nPoints = Text::StrToUInt32(strs[8].v);
			ptCurr = Text::StrToUInt32(strs[9].v);
			scaleN = Text::StrToDouble(strs[10].v);
			scaleD = Text::StrToDouble(strs[11].v);
			fontStyle = Text::StrToInt32(strs[12].v);
			isAlign = Text::StrToInt32(strs[13].v);
			//////////////////////////////////////////////////
//			mapRate = 200000.0;

			if (lblType == 2)
			{
				ptArr = MemAllocA(Math::Coord2DDbl, nPoints);
				currPt = ptArr;
				sptr = strs[14].ConcatTo(sbuff);
				eol = false;
				i = 0;
				while (true)
				{
					if (eol)
					{
					}
					else
					{
						if (reader->GetLastLineBreak(sptr) == sptr)
						{
							reader->ReadLine(sptr, 255 - (UOSInt)(sptr - sbuff));
						}
						else
						{
							eol = true;
							*sptr = 0;
						}
					}

					sptr = sbuff;
					if (*sptr != '{')
					{
						err = true;
						break;
					}
					while (true)
					{
						c = *sptr++;
						if (c == 0)
						{
							err = true;
							break;
						}
						else if (c == '}')
						{
							sptr[-1] = 0;
							sptr2 = &sptr[-1];
							if (*sptr == 0 && eol)
							{
							}
							else if (*sptr != ',')
							{
								err = true;
							}
							else
							{
								sptr++;
							}
							break;
						}
					}
					if (err)
						break;
					if (Text::StrSplitP(strs, 5, {&sbuff[1], (UOSInt)(sptr2 - &sbuff[1])}, ',') != 4)
					{
						err = true;
						break;
					}

					currPt[0].x = Text::StrToDouble(strs[0].v);
					currPt[0].y = Text::StrToDouble(strs[1].v);
					currPt += 1;
					i++;

					if (*sptr == 0)
						break;
					if (i >= nPoints)
					{
						err = true;
						break;
					}
					sptr = Text::StrConcat(sbuff, sptr);
				}
				if (i != nPoints)
				{
					err = true;
				}
				else
				{
					if (maxX >= 0 && minX < UOSInt2Double(view->GetScnWidth()) && maxY >= 0 && minY < UOSInt2Double(view->GetScnHeight()))
					{
						Math::Coord2D<Int32> *scnPts;
						Math::RectAreaDbl realBounds;
						scnPts = MemAlloc(Math::Coord2D<Int32>, nPoints);
						view->MapXYToScnXY(ptArr, scnPts, nPoints, Math::Coord2D<Int32>(0, 0));
						if (isAlign)
						{
							DrawCharsLA(img, label->ToCString(), ptArr, scnPts, nPoints, ptCurr, scaleN, scaleD, fonts[fontStyle], &realBounds);
						}
						else
						{
							DrawCharsL(img, label->ToCString(), ptArr, scnPts, nPoints, ptCurr, scaleN, scaleD, fonts[fontStyle], &realBounds);
						}
						MemFree(scnPts);
					}
				}

				MemFreeA(ptArr);
			}
			else
			{
				err = true;
			}
			label->Release();
		}
		else
		{
			err = true;
		}
	}
	DEL_CLASS(reader);
	if (fs)
	{
		DEL_CLASS(fs);
	}
}

Map::MapConfig2TGen::MapConfig2TGen(Text::CStringNN fileName, NotNullPtr<Media::DrawEngine> eng, Data::ArrayList<Map::MapDrawLayer*> *layerList, Parser::ParserList *parserList, const UTF8Char *forceBase, IO::Writer *errWriter, Int32 maxScale, Int32 minScale)
{
	UTF8Char lineBuff[1024];
	UTF8Char layerName[512];
	UTF8Char *layerNameEnd;
	UTF8Char sbuff[512];
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

	if (minScale == 0)
	{
		minScale = 400;
	}
	if (maxScale < minScale)
	{
		maxScale = 0x7fffffff;
	}

	this->drawEng = eng;
	this->inited = false;

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
//		Media::Resizer::LanczosResizerH8_8 resizer(3, 3, Media::AT_NO_ALPHA);
		if (forceBase)
		{
			baseDir = Text::StrConcat(layerName, forceBase);
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
				this->nStr = 5000;//Text::StrToInt32(strs[4]);
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
				if (forceBase == 0)
				{
					baseDir = fileName.ConcatTo(layerName);
					baseDir = IO::Path::AppendPath(layerName, baseDir, strs[1].ToCString());
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
				currLayer = MemAlloc(MapLayerStyle, 1);
				currLayer->drawType = 6;
				currLayer->minScale = Text::StrToInt32(strs[2].v);
				currLayer->maxScale = Text::StrToInt32(strs[3].v);
				currLayer->img = 0;
				if (currLayer->minScale < maxScale && currLayer->maxScale >= minScale)
				{
					layerNameEnd = strs[1].ConcatTo(baseDir);
					currLayer->lyr = GetDrawLayer(CSTRP(layerName, layerNameEnd), layerList, errWriter);
					if (currLayer->lyr == 0)
					{
						MemFree(currLayer);
					}
					else
					{
						currLayer->style = Text::StrToUInt32(strs[4].v);
						currLayer->bkColor = 0;
						if (currLayer->style < this->nLine)
						{
							this->drawList->Add(currLayer);
						}
					}
				}
				else
				{
					MemFree(currLayer);
				}
				break;
			case 7:
				currLayer = MemAlloc(MapLayerStyle, 1);
				currLayer->drawType = 7;
				currLayer->minScale = Text::StrToInt32(strs[2].v);
				currLayer->maxScale = Text::StrToInt32(strs[3].v);
				currLayer->img = 0;
				if (currLayer->minScale < maxScale && currLayer->maxScale >= minScale)
				{
					layerNameEnd = strs[1].ConcatTo(baseDir);
					currLayer->lyr = GetDrawLayer(CSTRP(layerName, layerNameEnd), layerList, errWriter);
					if (currLayer->lyr == 0)
					{
						MemFree(currLayer);
					}
					else
					{
						currLayer->style = Text::StrToUInt32(strs[4].v);
						currLayer->bkColor = ToColor(strs[5].v);
						if (currLayer->style < this->nLine)
						{
							this->drawList->Add(currLayer);
						}
					}
				}
				else
				{
					MemFree(currLayer);
				}
				break;
			case 8:
//					minScale = Text::StrToInt32(strs[2]);
//					maxScale = Text::StrToInt32(strs[3]);
				break;
			case 9:
				currLayer = MemAlloc(MapLayerStyle, 1);
				currLayer->drawType = 9;
				currLayer->minScale = Text::StrToInt32(strs[2].v);
				currLayer->maxScale = Text::StrToInt32(strs[3].v);
				currLayer->img = 0;
				if (currLayer->minScale < maxScale && currLayer->maxScale >= minScale)
				{
					layerNameEnd = strs[1].ConcatTo(baseDir);
					currLayer->lyr = GetDrawLayer(CSTRP(layerName, layerNameEnd), layerList, errWriter);
					if (currLayer->lyr == 0)
					{
						MemFree(currLayer);
					}
					else
					{
						currLayer->priority = Text::StrToInt32(strs[4].v);
						currLayer->style = Text::StrToUInt32(strs[5].v);
						currLayer->bkColor = Text::StrToUInt32(strs[6].v);
						if (currLayer->style < this->nFont)
						{
							this->drawList->Add(currLayer);
						}
					}
				}
				else
				{
					MemFree(currLayer);
				}
				break;
			case 10:
				currLayer = MemAlloc(MapLayerStyle, 1);
				currLayer->drawType = 10;
				currLayer->minScale = Text::StrToInt32(strs[2].v);
				currLayer->maxScale = Text::StrToInt32(strs[3].v);
				currLayer->img = 0;
				if (currLayer->minScale < maxScale && currLayer->maxScale >= minScale)
				{
					IO::ParserType pt;
					IO::ParsedObject *obj;
					sptr = fileName.ConcatTo(sbuff);
					sptr = IO::Path::AppendPath(sbuff, sptr, strs[4].ToCString());
					{
						IO::StmData::FileData fd(CSTRP(sbuff, sptr), false);
						obj = parserList->ParseFile(fd, &pt);
					}
					if (obj)
					{
						if (obj->GetParserType() == IO::ParserType::ImageList)
						{
							Media::ImageList *imgList = (Media::ImageList*)obj;
							if (imgList->GetCount() > 0)
							{
								imgList->ToStaticImage(0);
								NotNullPtr<Media::StaticImage> img;
								if (img.Set((Media::StaticImage*)imgList->GetImage(0, 0)))
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
					if (currLayer->img == 0)
					{
						currLayer->img = this->drawEng->LoadImage(strs[4].ToCString());
					}
					NotNullPtr<Media::DrawImage> img;
					if (!img.Set(currLayer->img))
					{
						MemFree(currLayer);
					}
					else
					{
						layerNameEnd = strs[1].ConcatTo(baseDir);
						currLayer->lyr = GetDrawLayer(CSTRP(layerName, layerNameEnd), layerList, errWriter);
						if (currLayer->lyr == 0)
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
								resizer.Resize((UInt8*)oldImg->bmpBits, oldImg->GetWidth() * 4, oldImg->GetWidth(), oldImg->GetHeight(), (UInt8*)newImg->bmpBits, newImg->GetWidth() * 4, newImg->GetWidth(), newImg->GetHeight());
								this->drawEng->DeleteImage(oldImg);
								currLayer->img = newImg;
							}*/
							this->drawList->Add(currLayer);
							poiArr.Add(currLayer);
						}
					}
				}
				else
				{
					MemFree(currLayer);
				}
				break;
			default:
				break;
			}
		}

		i = this->drawList->GetCount();
		while (i-- > 0)
		{
			currLayer = (MapLayerStyle*)this->drawList->GetItem(i);
			if (currLayer->drawType == 9)
			{
				j = poiArr.GetCount();
				while (j-- > 0)
				{
					currLayer2 = (MapLayerStyle*)poiArr.GetItem(j);
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

Map::MapConfig2TGen::~MapConfig2TGen()
{
	UOSInt i;
	UOSInt j;
	Map::MapLineStyle *currLine;
	Map::MapFontStyle *currFont;
	Map::MapLayerStyle *currLyr;
	NotNullPtr<Media::DrawImage> img;

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
		UOSInt i = this->drawList->GetCount();
		while (i-- > 0)
		{
			currLyr = (Map::MapLayerStyle*)this->drawList->GetItem(i);
			if (img.Set(currLyr->img) && currLyr->drawType == 10)
			{
				this->drawEng->DeleteImage(img);
			}
			MemFree(currLyr);
		}
		DEL_CLASS(this->drawList);
	}
}

Bool Map::MapConfig2TGen::IsError()
{
	return this->drawList == 0;
}

Media::DrawPen *Map::MapConfig2TGen::CreatePen(NotNullPtr<Media::DrawImage> img, UInt32 lineStyle, UOSInt lineLayer)
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
		Int32 currVal;
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
		Media::DrawPen *pen = img->NewPenARGB(thisLine->color, thisLine->lineWidth * img->GetHDPI() / 96.0, pattern, i);
		MemFree(pattern);
		return pen;
	}
	return 0;
}

WChar *Map::MapConfig2TGen::DrawMap(NotNullPtr<Media::DrawImage> img, Map::MapView *view, Bool *isLayerEmpty, Map::MapScheduler *mapSch, Media::IImgResizer *resizer, Text::CString dbOutput, DrawParam *params)
{
//	Manage::HiResClock clk;
	UInt32 index;
	UInt32 index2;
	UOSInt layerCnt = this->drawList->GetCount();
	Data::ArrayList<MapFontStyle*> **myArrs;
	Data::ArrayList<MapFontStyle*> *fontArr;
	NotNullPtr<Map::MapDrawLayer> lyr;
	Map::MapLayerStyle *lyrs;
	Math::Geometry::Vector2D *vec;
	Map::MapFontStyle *fnt;
	Map::MapFontStyle *fnt2;
	NotNullPtr<Media::DrawBrush> brush;
	Media::DrawPen *pen;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt l;
	Int64 lastId;
	Int64 thisId;
	Double thisScale;
	UOSInt labelCnt[3];
//	view->SetDestImage(img);
	MapLogger log(dbOutput, view);
	Data::ArrayListInt64 arr;

	labelCnt[0] = 0;
	labelCnt[1] = (UOSInt)&log;
	labelCnt[2] = (UInt32)params->labelType;
	UOSInt maxLabel = this->nStr;
	*isLayerEmpty = true;
	Map::MapConfig2TGen::MapLabels2 *labels = MemAlloc(Map::MapConfig2TGen::MapLabels2, maxLabel);
	Math::RectAreaDbl *objBounds = MemAllocA(Math::RectAreaDbl, this->nStr * 2);
	UOSInt objCnt = 0;

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
					fnt2->other = img->NewPenARGB(fnt->color, fnt->thick * img->GetHDPI() / 96.0, 0, 0);
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
		lyrs = (Map::MapLayerStyle*)this->drawList->GetItem(index++);
		if (thisScale > lyrs->minScale && thisScale <= lyrs->maxScale)
		{
			if (lyrs->drawType == 7 && params->labelType != 0)
			{
				if (lyr.Set(lyrs->lyr))
				{
					arr.Clear();
					Math::RectAreaDbl rect = view->GetVerticalRect();
					lyr->GetObjectIdsMapXY(arr, 0, rect, true);

					if ((i = arr.GetCount()) > 0)
					{
#ifdef NOSCH
						Data::ArrayList *drawArr;
						NEW_CLASS(drawArr, Data::ArrayList());
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
								if ((vec = lyr->GetNewVectorById(session, thisId)) != 0)
								{
#ifndef NOSCH
									mapSch->Draw(vec);
#else
									if (view->LatLonToScnXY(dobj->points, dobj->points, dobj->nPoints, 0, 0))
										*isLayerEmpty = false;
									drawArr->Add(dobj);
#endif
								}
							}
						}
						lyr->EndGetObject(session);
#ifdef NOSCH
						pen = CreatePen(img, lyrs->style, 0, osSize);
						brush = img->NewBrushARGB(lyrs->bkColor);
						i = drawArr->GetCount();
						while (i-- > 0)
						{
							dobj = (DrawObject*)drawArr->GetItem(i);
							k = dobj->nParts;
							j = 1;
							while (j < k)
							{
								dobj->parts[j - 1] = dobj->parts[j] - dobj->parts[j - 1];
								j++;
							}
							dobj->parts[k - 1] = dobj->nPoints - dobj->parts[k - 1];

							img->DrawPolyPolygon(dobj->points, dobj->parts, dobj->nParts, pen, brush);
						}
						img->DelPen(pen);
						img->DelBrush(brush);

						if (pen)
						{
							index2 = 1;
							while (pen = CreatePen(img, lyrs->style, index2++, osSize))
							{
								i = drawArr->GetCount();
								while (i-- > 0)
								{
									dobj = (DrawObject*)drawArr->GetItem(i);
									k = dobj->nParts;
									j = 1;
									while (j < k)
									{
										dobj->parts[j - 1] = dobj->parts[j] - dobj->parts[j - 1];
										j++;
									}
									dobj->parts[k - 1] = dobj->nPoints - dobj->parts[k - 1];

									img->DrawPolyPolygon(dobj->points, dobj->parts, dobj->nParts, pen, 0);
								}
								img->DelPen(pen);
							}
						}

						i = drawArr->GetCount();
						while (i-- > 0)
						{
							dobj = (DrawObject*)drawArr->RemoveAt(i);
							lyr->ReleaseObject(session, dobj);
						}
						DEL_CLASS(drawArr);
#else

						if (pen)
						{
							index2 = 1;
							while ((pen = CreatePen(img, lyrs->style, index2++)) != 0)
							{
								mapSch->DrawNextType(pen, 0);
							}
						}
						mapSch->WaitForFinish();
#endif
					}
				}
			}
			else if (lyrs->drawType == 6 && params->labelType != 0)
			{
				if (lyr.Set(lyrs->lyr))
				{
					arr.Clear();
					Math::RectAreaDbl rect = view->GetVerticalRect();
					lyr->GetObjectIdsMapXY(arr, 0, rect, true);

					if ((i = arr.GetCount()) > 0)
					{
#ifdef NOSCH
						Data::ArrayList *drawArr;
						NEW_CLASS(drawArr, Data::ArrayList());
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
								if ((vec = lyr->GetNewVectorById(session, thisId)) != 0)
								{
#ifndef NOSCH
									mapSch->Draw(vec);
#else
									if (view->LatLonToScnXY(dobj->points, dobj->points, dobj->nPoints, 0, 0))
										*isLayerEmpty = false;
									drawArr->Add(dobj);
#endif
								}
							}
						}
						lyr->EndGetObject(session);

#ifdef NOSCH
						index2 = 0;
						while (pen = CreatePen(img, lyrs->style, index2, osSize))
						{
							index2++;
							i = drawArr->GetCount();
							while (i-- > 0)
							{
								dobj = (DrawObject*)drawArr->GetItem(i);

								k = dobj->nParts;
								j = 1;
								while (j < k)
								{
									img->DrawPolyline(&dobj->points[dobj->parts[j-1] << 1], dobj->parts[j] - dobj->parts[j - 1], pen);
									j++;
								}
								img->DrawPolyline(&dobj->points[dobj->parts[k-1] << 1], dobj->nPoints - dobj->parts[k - 1], pen);
							}
							img->DelPen(pen);
						}

						i = drawArr->GetCount();
						while (i-- > 0)
						{
							dobj = (DrawObject*)drawArr->RemoveAt(i);
							lyr->ReleaseObject(session, dobj);
						}
						DEL_CLASS(drawArr);
#else
						if (pen)
						{
							index2 = 1;
							while ((pen = CreatePen(img, lyrs->style, index2++)) != 0)
							{
								mapSch->DrawNextType(pen, 0);
							}
						}
						mapSch->WaitForFinish();
#endif
					}
				}
			}
			else if (lyrs->drawType == 9)
			{
				DrawString(img, lyrs, view, myArrs, labels, maxLabel, labelCnt, isLayerEmpty);
			}
			else if (lyrs->drawType == 10)
			{
				DrawPoints(img, lyrs, view, isLayerEmpty, mapSch, drawEng, resizer, objBounds, &objCnt, maxLabel);
			}
		}
	}

	///////////////////////////////////////////////
	//blkId[0], blkId[1]
	Double w = view->GetScnWidth();
	Double h = view->GetScnHeight();
	if (params->labelType == 1)
	{
		LoadLabels(img, labels, maxLabel, labelCnt, view, myArrs, drawEng, objBounds, &objCnt, dbOutput, params->tileX - 1, params->tileY, -w, 0, 0);
		LoadLabels(img, labels, maxLabel, labelCnt, view, myArrs, drawEng, objBounds, &objCnt, dbOutput, params->tileX + 1, params->tileY, w, 0, 0);
		LoadLabels(img, labels, maxLabel, labelCnt, view, myArrs, drawEng, objBounds, &objCnt, dbOutput, params->tileX, params->tileY - 1, 0, h, 0);
		LoadLabels(img, labels, maxLabel, labelCnt, view, myArrs, drawEng, objBounds, &objCnt, dbOutput, params->tileX, params->tileY + 1, 0, -h, 0);
	}
	else if (params->labelType == 2)
	{
		LoadLabels(img, labels, maxLabel, labelCnt, view, myArrs, drawEng, objBounds, &objCnt, dbOutput, params->tileX, params->tileY, 0, 0, params->dbStream);
	}
	DrawLabels(img, labels, maxLabel, labelCnt, view, myArrs, drawEng, objBounds, &objCnt);

	NotNullPtr<Media::DrawBrush> b;
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
					img->DelPen((Media::DrawPen*)fnt->other);
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

//	Double t = clk.GetTimeDiff();
//	printf("Time used: %d\n", (Int32)(t * 1000));
	return 0;
}

UInt32 Map::MapConfig2TGen::GetBGColor()
{
	return this->bgColor;
}

void Map::MapConfig2TGen::ReleaseLayers(Data::ArrayList<Map::MapDrawLayer*> *layerList)
{
	Map::MapDrawLayer *lyr;
	UOSInt i;
	i = layerList->GetCount();
	while (i-- > 0)
	{
		lyr = layerList->GetItem(i);
		DEL_CLASS(lyr);
	}
}
