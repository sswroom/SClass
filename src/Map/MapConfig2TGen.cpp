#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayList.h"
#include "Data/ArrayListInt32.h"
#include "IO/Console.h"
#include "IO/FileStream.h"
#include "IO/FileParser.h"
#include "IO/IStreamData.h"
#include "IO/Path.h"
#include "IO/Stream.h"
#include "IO/StreamReader.h"
#include "IO/StreamWriter.h"
#include "IO/StmData/FileData.h"
#include "Manage/HiResClock.h"
#include "Map/IMapDrawLayer.h"
#include "Map/MapView.h"
#include "Map/MapConfig2TGen.h"
#include "Map/CIPLayer2.h"
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
			NEW_CLASS(fs, IO::FileStream(fileName, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
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

	void AddString(const UTF8Char *label, Double x, Double y, Double scaleW, Double scaleH, UInt32 fontStyle, Bool isAlign, const Double *bounds)
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
		sptr = Text::StrDouble(sptr, bounds[0]);
		sptr = Text::StrConcatC(sptr, UTF8STRC(","));
		sptr = Text::StrDouble(sptr, bounds[1]);
		sptr = Text::StrConcatC(sptr, UTF8STRC(","));
		sptr = Text::StrDouble(sptr, bounds[2]);
		sptr = Text::StrConcatC(sptr, UTF8STRC(","));
		sptr = Text::StrDouble(sptr, bounds[3]);
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

	void AddStringL(const UTF8Char *label, Double *mapPts, Int32 *scnPts, UOSInt nPoints, UInt32 thisPt, Double scaleN, Double scaleD, UInt32 fontStyle, Bool isAlign, const Double *bounds)
	{
		UTF8Char sbuff[256];
		UTF8Char *sptr;
		UOSInt i;
		if (this->writer == 0 || bounds[2] < -scnW || bounds[0] > scnW * 2 || bounds[3] < -scnH || bounds[1] > scnH * 2)
		{
			return;
		}
		sptr = Text::StrConcatC(sbuff, UTF8STRC("2,"));
		sptr = Text::StrToCSVRec(sptr, label);
		sptr = Text::StrConcatC(sptr, UTF8STRC(","));
		sptr = Text::StrDouble(sptr, (bounds[2] + bounds[0]) * 0.5);
		sptr = Text::StrConcatC(sptr, UTF8STRC(","));
		sptr = Text::StrDouble(sptr, (bounds[3] + bounds[1]) * 0.5);
		sptr = Text::StrConcatC(sptr, UTF8STRC(","));
		sptr = Text::StrDouble(sptr, bounds[0]);
		sptr = Text::StrConcatC(sptr, UTF8STRC(","));
		sptr = Text::StrDouble(sptr, bounds[1]);
		sptr = Text::StrConcatC(sptr, UTF8STRC(","));
		sptr = Text::StrDouble(sptr, bounds[2]);
		sptr = Text::StrConcatC(sptr, UTF8STRC(","));
		sptr = Text::StrDouble(sptr, bounds[3]);
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
			sptr = Text::StrDouble(sptr, mapPts[0]);
			sptr = Text::StrConcatC(sptr, UTF8STRC(","));
			sptr = Text::StrDouble(sptr, mapPts[1]);
			sptr = Text::StrConcatC(sptr, UTF8STRC(","));
			sptr = Text::StrInt32(sptr, scnPts[0]);
			sptr = Text::StrConcatC(sptr, UTF8STRC(","));
			sptr = Text::StrInt32(sptr, scnPts[1]);
			sptr = Text::StrConcatC(sptr, UTF8STRC("}"));
			mapPts += 2;
			scnPts += 2;
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

void Map::MapConfig2TGen::DrawChars(Media::DrawImage *img, Text::CString str1, Double scnPosX, Double scnPosY, Double scaleW, Double scaleH, Data::ArrayList<MapFontStyle*> *fontStyle, Bool isAlign)
{
	UTF8Char sbuff[256];
	str1.ConcatTo(sbuff);
	Double size[2];
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
	font = (Map::MapFontStyle*)fontStyle->GetItem(maxIndex);
	if (scaleH == 0)
	{
		scaleH = 0;
	}
	img->GetTextSize(font->font, str1, size);

	if (scaleH == 0)
	{
		Double rcLeft;
		Double rcRight;
		Double rcTop;
		Double rcBottom;
		img->SetTextAlign(Media::DrawEngine::DRAW_POS_TOPLEFT);

		i = 0;
		while (i < fntCount)
		{
			font = (Map::MapFontStyle*)fontStyle->GetItem(i);
			if (font->fontType == 1)
			{
				Media::DrawPen *p = 0;
				Media::DrawBrush *b = 0;
				rcLeft = scnPosX - ((size[0] + font->fontSizePt) * 0.5);
				rcRight = rcLeft + size[0] + font->fontSizePt;
				rcTop = scnPosY - ((size[1] + font->fontSizePt) * 0.5);
				rcBottom = rcTop + size[1] + font->fontSizePt;

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

				img->DrawRect(rcLeft, rcTop, rcRight - rcLeft, rcBottom - rcTop, p, b);
			}
			else if (font->fontType == 2)
			{
				Math::Coord2DDbl pt[5];
				rcLeft = scnPosX - ((size[0] + font->fontSizePt) * 0.5);
				rcRight = rcLeft + size[0] + font->fontSizePt;
				rcTop = scnPosY - ((size[1] + font->fontSizePt) * 0.5);
				rcBottom = rcTop + size[1] + font->fontSizePt;

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
			else if (font->fontType == 0)
			{
				img->DrawString(scnPosX - (size[0] * 0.5), scnPosY - (size[1] * 0.5), str1, font->font, (Media::DrawBrush*)font->other);
			}
			else if (font->fontType == 4)
			{
				img->DrawStringB(scnPosX - (size[0] * 0.5), scnPosY - (size[1] * 0.5), str1, font->font, (Media::DrawBrush*)font->other, (UInt32)Double2Int32(font->thick));
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
		Media::DrawBrush *b;
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
			xPos = size[0] + font->fontSizePt;
			yPos = size[1] + font->fontSizePt;
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
			xPos = size[0] + font->fontSizePt;
			yPos = size[1] + font->fontSizePt;
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
		else if (font->fontType == 0 || font->fontType == 4)
		{
			if (isAlign)
			{
				img->SetTextAlign(Media::DrawEngine::DRAW_POS_TOPLEFT);
				Double currX = 0;
				Double currY = 0;
				Double startX;
				Double startY;
				Double tmp;
				Int32 type;
				Double szThis[2];
				img->GetTextSize(font->font, str1, szThis);

				if ((szThis[0] * absH) < (szThis[1] * UOSInt2Double(lblSize) * scaleW))
				{
					scaleW = -scaleW;
					startX = scnPosX - (tmp = (szThis[0] * 0.5));
					if (scaleW)
						startY = scnPosY - (szThis[1] * 0.5) - (tmp * scaleH / scaleW);
					else
						startY = scnPosY - (szThis[1] * 0.5);
					type = 0;
				}
				else
				{
					scaleW = -scaleW;
					if (scaleH > 0)
					{
						startY = scnPosY - (tmp = ((szThis[1] * UOSInt2Double(lblSize)) * 0.5));
						startX = scnPosX - (tmp * scaleW / scaleH);
					}
					else if (scaleH)
					{
						scaleW = -scaleW;
						scaleH = -scaleH;
						startY = scnPosY - (tmp = ((szThis[1] * UOSInt2Double(lblSize)) * 0.5));
						startX = scnPosX - (tmp * scaleW / scaleH);
					}
					else
					{
						startY = scnPosY - (tmp = ((szThis[1] * UOSInt2Double(lblSize)) * 0.5));
						startX = scnPosX;
					}
					type = 1;
				}

//				Double cHeight;
//				if (scaleH < 0)
//					cHeight = szThis[1];
//				else
//					cHeight = -szThis[1];

				currX = 0;
				currY = 0;

				UOSInt cnt;
				UTF8Char *lbl = sbuff;
				cnt = lblSize;

				while (cnt--)
				{
					img->GetTextSize(font->font, {lbl, 1}, szThis);

					if (type)
					{
						if (font->fontType == 0)
						{
							UTF8Char l[2];
							l[0] = lbl[0];
							l[1] = 0;
							img->DrawString(startX + currX - szThis[0] * 0.5, startY + currY, {l, 1}, font->font, (Media::DrawBrush*)font->other);
						}
						else
						{
							UTF8Char l[2];
							l[0] = lbl[0];
							l[1] = 0;
							img->DrawStringB(startX + currX - szThis[0] * 0.5, startY + currY, {l, 1}, font->font, (Media::DrawBrush*)font->other, (UInt32)Double2Int32(font->thick));
						}

						currY += szThis[1];

						if (scaleH)
							currX = currY * scaleW / scaleH;
					}
					else
					{
						if (font->fontType == 0)
						{
							UTF8Char l[2];
							l[0] = lbl[0];
							l[1] = 0;
							img->DrawString(startX + currX, startY + currY, {l, 1}, font->font, (Media::DrawBrush*)font->other);
						}
						else
						{
							UTF8Char l[2];
							l[0] = lbl[0];
							l[1] = 0;
							img->DrawStringB(startX + currX, startY + currY, {l, 1}, font->font, (Media::DrawBrush*)font->other, (UInt32)Double2Int32(font->thick));
						}

						currX += szThis[0];
						if (scaleW)
							currY = (Int16)((Int32)currX * (Int32)scaleH / (Int32)scaleW);
					}
					lbl += 1;
				}

			}
			else
			{
				img->SetTextAlign(Media::DrawEngine::DRAW_POS_CENTER);

				if (font->fontType == 0)
				{
					img->DrawStringRot(scnPosX, scnPosY, str1, font->font, (Media::DrawBrush*)font->other, (Int32)(degD * 180 / PI));
				}
				else if (font->fontType == 4)
				{
					img->DrawStringRotB(scnPosX, scnPosY, str1, font->font, (Media::DrawBrush*)font->other, (Int32)(degD * 180 / PI), (UInt32)Double2Int32(font->thick));
				}
			}
		}
		i++;
	}
}

void Map::MapConfig2TGen::DrawCharsLA(Media::DrawImage *img, Text::CString str1, Double *mapPts, Int32 *scnPts, UOSInt nPoints, UInt32 thisPt, Double scaleN, Double scaleD, Data::ArrayList<MapFontStyle*> *fontStyle, Double *realBounds)
{
	UTF8Char sbuff[256];
	str1.ConcatTo(sbuff);
	UOSInt lblSize = str1.leng;
	Double centX = scnPts[thisPt << 1] + (scnPts[(thisPt << 1) + 2] - scnPts[(thisPt << 1)]) * scaleN / scaleD;
	Double centY = scnPts[(thisPt << 1) + 1] + (scnPts[(thisPt << 1) + 3] - scnPts[(thisPt << 1) + 1]) * scaleN / scaleD;
	Double currX;
	Double currY;
	Double nextX;
	Double nextY;
	Double startX = 0;
	Double startY = 0;
	Double xDiff;
	Double yDiff;
	Double axDiff;
	Double ayDiff;
	Double minX;
	Double maxX;
	Double minY;
	Double maxY;
	UOSInt i;
	UOSInt j;
	Double angleOfst;
	Double szThis[2];
	Double szLast[2];
	Int32 mode;
	UOSInt fntCount;
	Map::MapFontStyle *font;
	Bool found;

	if (fontStyle == 0)
		return;

	maxX = minX = centX;
	maxY = minY = centY;

	fntCount = fontStyle->GetCount();
	font = (Map::MapFontStyle*)fontStyle->GetItem(0);

	xDiff = scnPts[(thisPt << 1) + 2] - scnPts[(thisPt << 1) + 0];
	yDiff = scnPts[(thisPt << 1) + 3] - scnPts[(thisPt << 1) + 1];
	if (xDiff > 0)
	{
		axDiff = xDiff;
	}
	else
	{
		axDiff = -xDiff;
	}
	if (yDiff > 0)
	{
		ayDiff = yDiff;
	}
	else
	{
		ayDiff = -yDiff;
	}

	if (axDiff > ayDiff)
	{
		if (xDiff > 0)
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
		if (yDiff > 0)
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

	currX = centX;
	currY = centY;
	i = lblSize;
	j = thisPt;
	nextX = xDiff;
	nextY = yDiff;
	xDiff = 0;
	yDiff = 0;

	while (i-- > 0)
	{
		img->GetTextSize(font->font, {&str1.v[i], 1}, szThis);
		xDiff += szThis[0];
		yDiff += szThis[1];
	}
	found = false;
	if (mode == 0)
	{
		if (axDiff > ayDiff)
		{
			if (nextX > 0)
			{
				if ((centX - xDiff) >= scnPts[(j << 1)])
				{
					startX = centX - xDiff;
					startY = scnPts[(j << 1) + 1] + (scnPts[(j << 1) + 3] - scnPts[(j << 1) + 1]) * (startX - scnPts[(j << 1)]) / (scnPts[(j << 1) + 2] - scnPts[(j << 1)]);
					found = true;
				}
			}
			else
			{
				if ((centX + xDiff) >= scnPts[(j << 1)])
				{
					startX = centX + xDiff;
					startY = scnPts[(j << 1) + 1] + (scnPts[(j << 1) + 3] - scnPts[(j << 1) + 1]) * (startX - scnPts[(j << 1)]) / (scnPts[(j << 1) + 2] - scnPts[(j << 1)]);
					found = true;
				}
			}
		}
		else
		{
			if (nextY > 0)
			{
				if ((centY - yDiff) >= scnPts[(j << 1) + 1])
				{
					startY = centY - yDiff;
					startX = scnPts[(j << 1) + 0] + (scnPts[(j << 1) + 2] - scnPts[(j << 1) + 0]) * (startY - scnPts[(j << 1) + 1]) / (scnPts[(j << 1) + 3] - scnPts[(j << 1) + 1]);
					found = true;
				}
			}
			else
			{
				if ((centY + yDiff) >= scnPts[(j << 1) + 1])
				{
					startY = centY + yDiff;
					startX = scnPts[(j << 1) + 0] + (scnPts[(j << 1) + 2] - scnPts[(j << 1) + 0]) * (startY - scnPts[(j << 1) + 1]) / (scnPts[(j << 1) + 3] - scnPts[(j << 1) + 1]);
					found = true;
				}
			}
		}
	}
	else
	{
		if (axDiff > ayDiff)
		{
			if (nextX > 0)
			{
				if ((centX - xDiff) >= scnPts[(j << 1) + 2])
				{
					startX = centX - xDiff;
					startY = scnPts[(j << 1) + 1] + (scnPts[(j << 1) + 3] - scnPts[(j << 1) + 1]) * (startX - scnPts[(j << 1)]) / (scnPts[(j << 1) + 2] - scnPts[(j << 1)]);
					found = true;
				}
			}
			else
			{
				if ((centX - xDiff) >= scnPts[(j << 1) + 2])
				{
					startX = centX - xDiff;
					startY = scnPts[(j << 1) + 1] + (scnPts[(j << 1) + 3] - scnPts[(j << 1) + 1]) * (startX - scnPts[(j << 1)]) / (scnPts[(j << 1) + 2] - scnPts[(j << 1)]);
					found = true;
				}
			}
		}
		else
		{
			if (nextY > 0)
			{
				if ((centY - yDiff) >= scnPts[(j << 1) + 3])
				{
					startY = centY - yDiff;
					startX = scnPts[(j << 1) + 0] + (scnPts[(j << 1) + 2] - scnPts[(j << 1) + 0]) * (startY - scnPts[(j << 1) + 1]) / (scnPts[(j << 1) + 3] - scnPts[(j << 1) + 1]);
					found = true;
				}
			}
			else
			{
				if ((centY - yDiff) >= scnPts[(j << 1) + 3])
				{
					startY = centY - yDiff;
					startX = scnPts[(j << 1) + 0] + (scnPts[(j << 1) + 2] - scnPts[(j << 1) + 0]) * (startY - scnPts[(j << 1) + 1]) / (scnPts[(j << 1) + 3] - scnPts[(j << 1) + 1]);
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
				if (axDiff > ayDiff)
				{
					if ((scnPts[(j << 1)] - (centX - xDiff) >= 0) ^ (scnPts[(j << 1) + 2] - (centX - xDiff) >= 0))
					{
						startX = centX - xDiff;
						startY = scnPts[(j << 1) + 1] + (scnPts[(j << 1) + 3] - scnPts[(j << 1) + 1]) * (startX - scnPts[(j << 1)]) / (scnPts[(j << 1) + 2] - scnPts[(j << 1)]);
						break;
					}
					else if ((scnPts[(j << 1)] - (centX + xDiff) >= 0) ^ (scnPts[(j << 1) + 2] - (centX + xDiff) >= 0))
					{
						startX = centX + xDiff;
						startY = scnPts[(j << 1) + 1] + (scnPts[(j << 1) + 3] - scnPts[(j << 1) + 1]) * (startX - scnPts[(j << 1)]) / (scnPts[(j << 1) + 2] - scnPts[(j << 1)]);
						break;
					}
					else if ((scnPts[(j << 1) + 1] - (centY - yDiff) >= 0) ^ (scnPts[(j << 1) + 3] - (centY - yDiff) >= 0))
					{
						startY = centY - yDiff;
						startX = scnPts[(j << 1) + 0] + (scnPts[(j << 1) + 2] - scnPts[(j << 1) + 0]) * (startY - scnPts[(j << 1) + 1]) / (scnPts[(j << 1) + 3] - scnPts[(j << 1) + 1]);
						break;
					}
					else if ((scnPts[(j << 1) + 1] - (centY + yDiff) >= 0) ^ (scnPts[(j << 1) + 3] - (centY + yDiff) >= 0))
					{
						startY = centY + yDiff;
						startX = scnPts[(j << 1) + 0] + (scnPts[(j << 1) + 2] - scnPts[(j << 1) + 0]) * (startY - scnPts[(j << 1) + 1]) / (scnPts[(j << 1) + 3] - scnPts[(j << 1) + 1]);
						break;
					}
				}
				else
				{
					if ((scnPts[(j << 1) + 1] - (centY - yDiff) >= 0) ^ (scnPts[(j << 1) + 3] - (centY - yDiff) >= 0))
					{
						startY = centY - yDiff;
						startX = scnPts[(j << 1) + 0] + (scnPts[(j << 1) + 2] - scnPts[(j << 1) + 0]) * (startY - scnPts[(j << 1) + 1]) / (scnPts[(j << 1) + 3] - scnPts[(j << 1) + 1]);
						break;
					}
					else if ((scnPts[(j << 1) + 1] - (centY + yDiff) >= 0) ^ (scnPts[(j << 1) + 3] - (centY + yDiff) >= 0))
					{
						startY = centY + yDiff;
						startX = scnPts[(j << 1) + 0] + (scnPts[(j << 1) + 2] - scnPts[(j << 1) + 0]) * (startY - scnPts[(j << 1) + 1]) / (scnPts[(j << 1) + 3] - scnPts[(j << 1) + 1]);
						break;
					}
					else if ((scnPts[(j << 1)] - (centX - xDiff) >= 0) ^ (scnPts[(j << 1) + 2] - (centX - xDiff) >= 0))
					{
						startX = centX - xDiff;
						startY = scnPts[(j << 1) + 1] + (scnPts[(j << 1) + 3] - scnPts[(j << 1) + 1]) * (startX - scnPts[(j << 1)]) / (scnPts[(j << 1) + 2] - scnPts[(j << 1)]);
						break;
					}
					else if ((scnPts[(j << 1)] - (centX + xDiff) >= 0) ^ (scnPts[(j << 1) + 2] - (centX + xDiff) >= 0))
					{
						startX = centX + xDiff;
						startY = scnPts[(j << 1) + 1] + (scnPts[(j << 1) + 3] - scnPts[(j << 1) + 1]) * (startX - scnPts[(j << 1)]) / (scnPts[(j << 1) + 2] - scnPts[(j << 1)]);
						break;
					}
				}

			}
			if (j == (UOSInt)-1)
			{
				j = 0;
				startX = scnPts[0];
				startY = scnPts[1];
			}
		}
		else
		{
			j++;
			while (j < nPoints - 1)
			{
				if (axDiff > ayDiff)
				{
					if ((scnPts[(j << 1)] - (centX - xDiff) >= 0) ^ (scnPts[(j << 1) + 2] - (centX - xDiff) >= 0))
					{
						startX = centX - xDiff;
						startY = scnPts[(j << 1) + 1] + (scnPts[(j << 1) + 3] - scnPts[(j << 1) + 1]) * (startX - scnPts[(j << 1)]) / (scnPts[(j << 1) + 2] - scnPts[(j << 1)]);
						break;
					}
					else if ((scnPts[(j << 1)] - (centX + xDiff) >= 0) ^ (scnPts[(j << 1) + 2] - (centX + xDiff) >= 0))
					{
						startX = centX + xDiff;
						startY = scnPts[(j << 1) + 1] + (scnPts[(j << 1) + 3] - scnPts[(j << 1) + 1]) * (startX - scnPts[(j << 1)]) / (scnPts[(j << 1) + 2] - scnPts[(j << 1)]);
						break;
					}
					else if ((scnPts[(j << 1) + 1] - (centY - yDiff) >= 0) ^ (scnPts[(j << 1) + 3] - (centY - yDiff) >= 0))
					{
						startY = centY - yDiff;
						startX = scnPts[(j << 1) + 0] + (scnPts[(j << 1) + 2] - scnPts[(j << 1) + 0]) * (startY - scnPts[(j << 1) + 1]) / (scnPts[(j << 1) + 3] - scnPts[(j << 1) + 1]);
						break;
					}
					else if ((scnPts[(j << 1) + 1] - (centY + yDiff) >= 0) ^ (scnPts[(j << 1) + 3] - (centY + yDiff) >= 0))
					{
						startY = centY + yDiff;
						startX = scnPts[(j << 1) + 0] + (scnPts[(j << 1) + 2] - scnPts[(j << 1) + 0]) * (startY - scnPts[(j << 1) + 1]) / (scnPts[(j << 1) + 3] - scnPts[(j << 1) + 1]);
						break;
					}
				}
				else
				{
					if ((scnPts[(j << 1) + 1] - (centY - yDiff) >= 0) ^ (scnPts[(j << 1) + 3] - (centY - yDiff) >= 0))
					{
						startY = centY - yDiff;
						startX = scnPts[(j << 1) + 0] + (scnPts[(j << 1) + 2] - scnPts[(j << 1) + 0]) * (startY - scnPts[(j << 1) + 1]) / (scnPts[(j << 1) + 3] - scnPts[(j << 1) + 1]);
						break;
					}
					else if ((scnPts[(j << 1) + 1] - (centY + yDiff) >= 0) ^ (scnPts[(j << 1) + 3] - (centY + yDiff) >= 0))
					{
						startY = centY + yDiff;
						startX = scnPts[(j << 1) + 0] + (scnPts[(j << 1) + 2] - scnPts[(j << 1) + 0]) * (startY - scnPts[(j << 1) + 1]) / (scnPts[(j << 1) + 3] - scnPts[(j << 1) + 1]);
						break;
					}
					else if ((scnPts[(j << 1)] - (centX - xDiff) >= 0) ^ (scnPts[(j << 1) + 2] - (centX - xDiff) >= 0))
					{
						startX = centX - xDiff;
						startY = scnPts[(j << 1) + 1] + (scnPts[(j << 1) + 3] - scnPts[(j << 1) + 1]) * (startX - scnPts[(j << 1)]) / (scnPts[(j << 1) + 2] - scnPts[(j << 1)]);
						break;
					}
					else if ((scnPts[(j << 1)] - (centX + xDiff) >= 0) ^ (scnPts[(j << 1) + 2] - (centX + xDiff) >= 0))
					{
						startX = centX + xDiff;
						startY = scnPts[(j << 1) + 1] + (scnPts[(j << 1) + 3] - scnPts[(j << 1) + 1]) * (startX - scnPts[(j << 1)]) / (scnPts[(j << 1) + 2] - scnPts[(j << 1)]);
						break;
					}
				}

				j++;
			}
			if (j >= nPoints - 1)
			{
				j = nPoints - 2;
				startX = scnPts[(j << 1) + 2];
				startY = scnPts[(j << 1) + 3];
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
		////////////////////////////////
		UTF8Char *lbl = sbuff;
		UTF8Char *nextPos = lbl;
		UTF8Char nextChar = *lbl;
		Double angle;
		Double angleDegree;
		Double lastAngle;
		UOSInt lastAInd;
		Double lastX;
		Double lastY;

		szLast[0] = 0;
		szLast[1] = 0;

		lastX = currX = startX;
		lastY = currY = startY;
		j = startInd;

		angle = angleOfst - Math_ArcTan2((mapPts[(j << 1) + 1] - mapPts[(j << 1) + 3]), (mapPts[(j << 1) + 2] - mapPts[(j << 1) + 0]));
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

			img->GetTextSize(font->font, CSTRP(lbl, nextPos), szThis);
			while (true)
			{
				if (angleDegree <= 90)
				{
					nextX = currX + ((szLast[0] + szThis[0]) * 0.5);
					nextY = currY - ((szLast[1] + szThis[1]) * 0.5);
				}
				else if (angleDegree <= 180)
				{
					nextX = currX - ((szLast[0] + szThis[0]) * 0.5);
					nextY = currY - ((szLast[1] + szThis[1]) * 0.5);
				}
				else if (angleDegree <= 270)
				{
					nextX = currX - ((szLast[0] + szThis[0]) * 0.5);
					nextY = currY + ((szLast[1] + szThis[1]) * 0.5);
				}
				else
				{
					nextX = currX + ((szLast[0] + szThis[0]) * 0.5);
					nextY = currY + ((szLast[1] + szThis[1]) * 0.5);
				}

				if (((nextX > scnPts[(j << 1)]) ^ (nextX > scnPts[(j << 1) + 2])) || (nextX == scnPts[(j << 1)]) || (nextX == scnPts[(j << 1) + 2]))
				{
					Double tempY = scnPts[(j << 1) + 1] + (scnPts[(j << 1) + 3] - scnPts[(j << 1) + 1]) * (Double)(nextX - scnPts[(j << 1)]) / (scnPts[(j << 1) + 2] - scnPts[(j << 1)]);
					tempY -= currY;
					if (tempY < 0)
						tempY = -tempY;
					if (tempY > (szLast[1] + szThis[1]) * 0.5)
					{
						currY = nextY;
						currX = scnPts[(j << 1) + 0] + (scnPts[(j << 1) + 2] - scnPts[(j << 1) + 0]) * (Double)(currY - scnPts[(j << 1) + 1]) / (scnPts[(j << 1) + 3] - scnPts[(j << 1) + 1]);
					}
					else
					{
						currX = nextX;
						currY = scnPts[(j << 1) + 1] + (scnPts[(j << 1) + 3] - scnPts[(j << 1) + 1]) * (Double)(nextX - scnPts[(j << 1)]) / (scnPts[(j << 1) + 2] - scnPts[(j << 1)]);
					}
					break;
				}
				else if (((nextY > scnPts[(j << 1) + 1]) ^ (nextY > scnPts[(j << 1) + 3])) || (nextY == scnPts[(j << 1) + 1]) || (nextY == scnPts[(j << 1) + 3]))
				{
					currY = nextY;
					currX = scnPts[(j << 1) + 0] + (scnPts[(j << 1) + 2] - scnPts[(j << 1) + 0]) * (Double)(currY - scnPts[(j << 1) + 1]) / (scnPts[(j << 1) + 3] - scnPts[(j << 1) + 1]);
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

							Double tempY = scnPts[(j << 1) + 1] + (scnPts[(j << 1) + 3] - scnPts[(j << 1) + 1]) * (Double)(nextX - scnPts[(j << 1)]) / (scnPts[(j << 1) + 2] - scnPts[(j << 1)]);
							tempY -= currY;
							if (tempY < 0)
								tempY = -tempY;
							if (tempY > (szLast[1] + szThis[1]) * 0.5)
							{
								currY = nextY;
								currX = scnPts[(j << 1) + 0] + (scnPts[(j << 1) + 2] - scnPts[(j << 1) + 0]) * (Double)(currY - scnPts[(j << 1) + 1]) / (scnPts[(j << 1) + 3] - scnPts[(j << 1) + 1]);
							}
							else
							{
								currX = nextX;
								currY = scnPts[(j << 1) + 1] + (scnPts[(j << 1) + 3] - scnPts[(j << 1) + 1]) * (Double)(nextX - scnPts[(j << 1)]) / (scnPts[(j << 1) + 2] - scnPts[(j << 1)]);
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

							Double tempY = scnPts[(j << 1) + 1] + (scnPts[(j << 1) + 3] - scnPts[(j << 1) + 1]) * (Double)(nextX - scnPts[(j << 1)]) / (scnPts[(j << 1) + 2] - scnPts[(j << 1)]);
							tempY -= currY;
							if (tempY < 0)
								tempY = -tempY;
							if (tempY > (szLast[1] + szThis[1]) * 0.5)
							{
								currY = nextY;
								currX = scnPts[(j << 1) + 0] + (scnPts[(j << 1) + 2] - scnPts[(j << 1) + 0]) * (Double)(currY - scnPts[(j << 1) + 1]) / (scnPts[(j << 1) + 3] - scnPts[(j << 1) + 1]);
							}
							else
							{
								currX = nextX;
								currY = scnPts[(j << 1) + 1] + (scnPts[(j << 1) + 3] - scnPts[(j << 1) + 1]) * (Double)(nextX - scnPts[(j << 1)]) / (scnPts[(j << 1) + 2] - scnPts[(j << 1)]);
							}
							break;
						}
					}

					angle = angleOfst - Math_ArcTan2((mapPts[(j << 1) + 1] - mapPts[(j << 1) + 3]), (mapPts[(j << 1) + 2] - mapPts[(j << 1) + 0]));
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
					nextX = lastX + ((szLast[0] + szThis[0]) * 0.5);
					nextY = lastY - ((szLast[1] + szThis[1]) * 0.5);
				}
				else if (lastAngle <= 180)
				{
					nextX = lastX - ((szLast[0] + szThis[0]) * 0.5);
					nextY = lastY - ((szLast[1] + szThis[1]) * 0.5);
				}
				else if (lastAngle <= 270)
				{
					nextX = lastX - ((szLast[0] + szThis[0]) * 0.5);
					nextY = lastY + ((szLast[1] + szThis[1]) * 0.5);
				}
				else
				{
					nextX = lastX + ((szLast[0] + szThis[0]) * 0.5);
					nextY = lastY + ((szLast[1] + szThis[1]) * 0.5);
				}
				Double tempY = scnPts[(lastAInd << 1) + 1] + (scnPts[(lastAInd << 1) + 3] - scnPts[(lastAInd << 1) + 1]) * (Double)(nextX - scnPts[(lastAInd << 1)]) / (scnPts[(lastAInd << 1) + 2] - scnPts[(lastAInd << 1)]);
				Double tempX = scnPts[(lastAInd << 1) + 0] + (scnPts[(lastAInd << 1) + 2] - scnPts[(lastAInd << 1) + 0]) * (Double)(nextY - scnPts[(lastAInd << 1) + 1]) / (scnPts[(lastAInd << 1) + 3] - scnPts[(lastAInd << 1) + 1]);
				tempY -= lastY;
				tempX -= lastX;
				if (tempY < 0)
					tempY = -tempY;
				if (tempX < 0)
					tempX = -tempX;
				if (tempX <= (szLast[0] + szThis[0]) * 0.5)
				{
					currY = nextY;
					currX = scnPts[(lastAInd << 1) + 0] + (scnPts[(lastAInd << 1) + 2] - scnPts[(lastAInd << 1) + 0]) * (Double)(nextY - scnPts[(lastAInd << 1) + 1]) / (scnPts[(lastAInd << 1) + 3] - scnPts[(lastAInd << 1) + 1]);
				}
				else
				{
					currX = nextX;
					currY = scnPts[(lastAInd << 1) + 1] + (scnPts[(lastAInd << 1) + 3] - scnPts[(lastAInd << 1) + 1]) * (Double)(nextX - scnPts[(lastAInd << 1)]) / (scnPts[(lastAInd << 1) + 2] - scnPts[(lastAInd << 1)]);
				}
			}
			else
			{
				lastAngle = angleDegree;
				lastAInd = j;
			}


			Double xadd = szThis[0] * 0.5;
			Double yadd = szThis[1] * 0.5;
			if ((currX - xadd) < minX)
			{
				minX = currX - xadd;
			}
			if ((currX + xadd) > maxX)
			{
				maxX = currX + xadd;
			}
			if ((currY - yadd) < minY)
			{
				minY = currY - yadd;
			}
			if ((currY + yadd) > maxY)
			{
				maxY = currY + yadd;
			}

			lastX = currX;
			lastY = currY;
			if (mode == 0)
			{
				if (font->fontType == 0)
				{
					img->DrawString(currX, currY, CSTRP(lbl, nextPos), font->font, (Media::DrawBrush*)font->other);
				}
				else
				{
					img->DrawStringB(currX, currY, CSTRP(lbl, nextPos), font->font, (Media::DrawBrush*)font->other, (UInt32)Double2Int32(font->thick));
				}
			}
			else
			{
				if (font->fontType == 0)
				{
					img->DrawString(currX, currY, CSTRP(lbl, nextPos), font->font, (Media::DrawBrush*)font->other);
				}
				else
				{
					img->DrawStringB(currX, currY, CSTRP(lbl, nextPos), font->font, (Media::DrawBrush*)font->other, (UInt32)Double2Int32(font->thick));
				}
			}
			szLast[0] = szThis[0];
			szLast[1] = szThis[1];
		}
		i++;
	}
	realBounds[0] = minX;
	realBounds[1] = minY;
	realBounds[2] = maxX;
	realBounds[3] = maxY;
}

void Map::MapConfig2TGen::DrawCharsLAo(Media::DrawImage *img, Text::CString str1, Double *mapPts, Int32 *scnPts, UOSInt nPoints, UInt32 thisPt, Double scaleN, Double scaleD, Data::ArrayList<MapFontStyle*> *fontStyle)
{
	UTF8Char sbuff[256];
	str1.ConcatTo(sbuff);
	UOSInt lblSize = str1.leng;
	Double centX = scnPts[thisPt << 1] + (scnPts[(thisPt << 1) + 2] - scnPts[(thisPt << 1)]) * scaleN / scaleD;
	Double centY = scnPts[(thisPt << 1) + 1] + (scnPts[(thisPt << 1) + 3] - scnPts[(thisPt << 1) + 1]) * scaleN / scaleD;
	Double currX;
	Double currY;
	Double nextX;
	Double nextY;
	Double startX = 0;
	Double startY = 0;
	Double xDiff;
	Double yDiff;
	Double axDiff;
	Double ayDiff;
	UOSInt i;
	UOSInt j;
	Double szThis[2];
	Int32 mode;
	UOSInt fntCount;
	Map::MapFontStyle *font;
	Bool found;

	if (fontStyle == 0)
		return;

	fntCount = fontStyle->GetCount();
	font = (Map::MapFontStyle*)fontStyle->GetItem(0);

	xDiff = scnPts[(thisPt << 1) + 2] - scnPts[(thisPt << 1) + 0];
	yDiff = scnPts[(thisPt << 1) + 3] - scnPts[(thisPt << 1) + 1];
	if (xDiff > 0)
	{
		axDiff = xDiff;
	}
	else
	{
		axDiff = -xDiff;
	}
	if (yDiff > 0)
	{
		ayDiff = yDiff;
	}
	else
	{
		ayDiff = -yDiff;
	}

	if (axDiff > ayDiff)
	{
		if (xDiff > 0)
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
		if (yDiff > 0)
		{
			mode = 0;
		}
		else
		{
			mode = 1;
		}
	}

	currX = centX;
	currY = centY;
	i = lblSize;
	j = thisPt;
	nextX = xDiff;
	nextY = yDiff;
	xDiff = 0;
	yDiff = 0;

	while (i-- > 0)
	{
		img->GetTextSize(font->font, {&str1.v[i], 1}, szThis);
		xDiff += szThis[0];
		yDiff += szThis[1];
	}
	found = false;
	if (mode == 0)
	{
		if (axDiff > ayDiff)
		{
			if (nextX > 0)
			{
				if ((centX - xDiff) >= scnPts[(j << 1)])
				{
					startX = centX - xDiff;
					startY = scnPts[(j << 1) + 1] + (scnPts[(j << 1) + 3] - scnPts[(j << 1) + 1]) * (startX - scnPts[(j << 1)]) / (scnPts[(j << 1) + 2] - scnPts[(j << 1)]);
					found = true;
				}
			}
			else
			{
				if ((centX + xDiff) >= scnPts[(j << 1)])
				{
					startX = centX + xDiff;
					startY = scnPts[(j << 1) + 1] + (scnPts[(j << 1) + 3] - scnPts[(j << 1) + 1]) * (startX - scnPts[(j << 1)]) / (scnPts[(j << 1) + 2] - scnPts[(j << 1)]);
					found = true;
				}
			}
		}
		else
		{
			if (nextY > 0)
			{
				if ((centY - yDiff) >= scnPts[(j << 1) + 1])
				{
					startY = centY - yDiff;
					startX = scnPts[(j << 1) + 0] + (scnPts[(j << 1) + 2] - scnPts[(j << 1) + 0]) * (startY - scnPts[(j << 1) + 1]) / (scnPts[(j << 1) + 3] - scnPts[(j << 1) + 1]);
					found = true;
				}
			}
			else
			{
				if ((centY + yDiff) >= scnPts[(j << 1) + 1])
				{
					startY = centY + yDiff;
					startX = scnPts[(j << 1) + 0] + (scnPts[(j << 1) + 2] - scnPts[(j << 1) + 0]) * (startY - scnPts[(j << 1) + 1]) / (scnPts[(j << 1) + 3] - scnPts[(j << 1) + 1]);
					found = true;
				}
			}
		}
	}
	else
	{
		if (axDiff > ayDiff)
		{
			if (nextX > 0)
			{
				if ((centX - xDiff) >= scnPts[(j << 1) + 2])
				{
					startX = centX - xDiff;
					startY = scnPts[(j << 1) + 1] + (scnPts[(j << 1) + 3] - scnPts[(j << 1) + 1]) * (startX - scnPts[(j << 1)]) / (scnPts[(j << 1) + 2] - scnPts[(j << 1)]);
					found = true;
				}
			}
			else
			{
				if ((centX - xDiff) >= scnPts[(j << 1) + 2])
				{
					startX = centX - xDiff;
					startY = scnPts[(j << 1) + 1] + (scnPts[(j << 1) + 3] - scnPts[(j << 1) + 1]) * (startX - scnPts[(j << 1)]) / (scnPts[(j << 1) + 2] - scnPts[(j << 1)]);
					found = true;
				}
			}
		}
		else
		{
			if (nextY > 0)
			{
				if ((centY - yDiff) >= scnPts[(j << 1) + 3])
				{
					startY = centY - yDiff;
					startX = scnPts[(j << 1) + 0] + (scnPts[(j << 1) + 2] - scnPts[(j << 1) + 0]) * (startY - scnPts[(j << 1) + 1]) / (scnPts[(j << 1) + 3] - scnPts[(j << 1) + 1]);
					found = true;
				}
			}
			else
			{
				if ((centY - yDiff) >= scnPts[(j << 1) + 3])
				{
					startY = centY - yDiff;
					startX = scnPts[(j << 1) + 0] + (scnPts[(j << 1) + 2] - scnPts[(j << 1) + 0]) * (startY - scnPts[(j << 1) + 1]) / (scnPts[(j << 1) + 3] - scnPts[(j << 1) + 1]);
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
				if (axDiff > ayDiff)
				{
					if ((scnPts[(j << 1)] - (centX - xDiff) >= 0) ^ (scnPts[(j << 1) + 2] - (centX - xDiff) >= 0))
					{
						startX = centX - xDiff;
						startY = scnPts[(j << 1) + 1] + (scnPts[(j << 1) + 3] - scnPts[(j << 1) + 1]) * (startX - scnPts[(j << 1)]) / (scnPts[(j << 1) + 2] - scnPts[(j << 1)]);
						break;
					}
					else if ((scnPts[(j << 1)] - (centX + xDiff) >= 0) ^ (scnPts[(j << 1) + 2] - (centX + xDiff) >= 0))
					{
						startX = centX + xDiff;
						startY = scnPts[(j << 1) + 1] + (scnPts[(j << 1) + 3] - scnPts[(j << 1) + 1]) * (startX - scnPts[(j << 1)]) / (scnPts[(j << 1) + 2] - scnPts[(j << 1)]);
						break;
					}
					else if ((scnPts[(j << 1) + 1] - (centY - yDiff) >= 0) ^ (scnPts[(j << 1) + 3] - (centY - yDiff) >= 0))
					{
						startY = centY - yDiff;
						startX = scnPts[(j << 1) + 0] + (scnPts[(j << 1) + 2] - scnPts[(j << 1) + 0]) * (startY - scnPts[(j << 1) + 1]) / (scnPts[(j << 1) + 3] - scnPts[(j << 1) + 1]);
						break;
					}
					else if ((scnPts[(j << 1) + 1] - (centY + yDiff) >= 0) ^ (scnPts[(j << 1) + 3] - (centY + yDiff) >= 0))
					{
						startY = centY + yDiff;
						startX = scnPts[(j << 1) + 0] + (scnPts[(j << 1) + 2] - scnPts[(j << 1) + 0]) * (startY - scnPts[(j << 1) + 1]) / (scnPts[(j << 1) + 3] - scnPts[(j << 1) + 1]);
						break;
					}
				}
				else
				{
					if ((scnPts[(j << 1) + 1] - (centY - yDiff) >= 0) ^ (scnPts[(j << 1) + 3] - (centY - yDiff) >= 0))
					{
						startY = centY - yDiff;
						startX = scnPts[(j << 1) + 0] + (scnPts[(j << 1) + 2] - scnPts[(j << 1) + 0]) * (startY - scnPts[(j << 1) + 1]) / (scnPts[(j << 1) + 3] - scnPts[(j << 1) + 1]);
						break;
					}
					else if ((scnPts[(j << 1) + 1] - (centY + yDiff) >= 0) ^ (scnPts[(j << 1) + 3] - (centY + yDiff) >= 0))
					{
						startY = centY + yDiff;
						startX = scnPts[(j << 1) + 0] + (scnPts[(j << 1) + 2] - scnPts[(j << 1) + 0]) * (startY - scnPts[(j << 1) + 1]) / (scnPts[(j << 1) + 3] - scnPts[(j << 1) + 1]);
						break;
					}
					else if ((scnPts[(j << 1)] - (centX - xDiff) >= 0) ^ (scnPts[(j << 1) + 2] - (centX - xDiff) >= 0))
					{
						startX = centX - xDiff;
						startY = scnPts[(j << 1) + 1] + (scnPts[(j << 1) + 3] - scnPts[(j << 1) + 1]) * (startX - scnPts[(j << 1)]) / (scnPts[(j << 1) + 2] - scnPts[(j << 1)]);
						break;
					}
					else if ((scnPts[(j << 1)] - (centX + xDiff) >= 0) ^ (scnPts[(j << 1) + 2] - (centX + xDiff) >= 0))
					{
						startX = centX + xDiff;
						startY = scnPts[(j << 1) + 1] + (scnPts[(j << 1) + 3] - scnPts[(j << 1) + 1]) * (startX - scnPts[(j << 1)]) / (scnPts[(j << 1) + 2] - scnPts[(j << 1)]);
						break;
					}
				}

			}
			if (j == (UOSInt)-1)
			{
				j = 0;
				startX = scnPts[0];
				startY = scnPts[1];
			}
		}
		else
		{
			j++;
			while (j < nPoints - 1)
			{
				if (axDiff > ayDiff)
				{
					if ((scnPts[(j << 1)] - (centX - xDiff) >= 0) ^ (scnPts[(j << 1) + 2] - (centX - xDiff) >= 0))
					{
						startX = centX - xDiff;
						startY = scnPts[(j << 1) + 1] + (scnPts[(j << 1) + 3] - scnPts[(j << 1) + 1]) * (startX - scnPts[(j << 1)]) / (scnPts[(j << 1) + 2] - scnPts[(j << 1)]);
						break;
					}
					else if ((scnPts[(j << 1)] - (centX + xDiff) >= 0) ^ (scnPts[(j << 1) + 2] - (centX + xDiff) >= 0))
					{
						startX = centX + xDiff;
						startY = scnPts[(j << 1) + 1] + (scnPts[(j << 1) + 3] - scnPts[(j << 1) + 1]) * (startX - scnPts[(j << 1)]) / (scnPts[(j << 1) + 2] - scnPts[(j << 1)]);
						break;
					}
					else if ((scnPts[(j << 1) + 1] - (centY - yDiff) >= 0) ^ (scnPts[(j << 1) + 3] - (centY - yDiff) >= 0))
					{
						startY = centY - yDiff;
						startX = scnPts[(j << 1) + 0] + (scnPts[(j << 1) + 2] - scnPts[(j << 1) + 0]) * (startY - scnPts[(j << 1) + 1]) / (scnPts[(j << 1) + 3] - scnPts[(j << 1) + 1]);
						break;
					}
					else if ((scnPts[(j << 1) + 1] - (centY + yDiff) >= 0) ^ (scnPts[(j << 1) + 3] - (centY + yDiff) >= 0))
					{
						startY = centY + yDiff;
						startX = scnPts[(j << 1) + 0] + (scnPts[(j << 1) + 2] - scnPts[(j << 1) + 0]) * (startY - scnPts[(j << 1) + 1]) / (scnPts[(j << 1) + 3] - scnPts[(j << 1) + 1]);
						break;
					}
				}
				else
				{
					if ((scnPts[(j << 1) + 1] - (centY - yDiff) >= 0) ^ (scnPts[(j << 1) + 3] - (centY - yDiff) >= 0))
					{
						startY = centY - yDiff;
						startX = scnPts[(j << 1) + 0] + (scnPts[(j << 1) + 2] - scnPts[(j << 1) + 0]) * (startY - scnPts[(j << 1) + 1]) / (scnPts[(j << 1) + 3] - scnPts[(j << 1) + 1]);
						break;
					}
					else if ((scnPts[(j << 1) + 1] - (centY + yDiff) >= 0) ^ (scnPts[(j << 1) + 3] - (centY + yDiff) >= 0))
					{
						startY = centY + yDiff;
						startX = scnPts[(j << 1) + 0] + (scnPts[(j << 1) + 2] - scnPts[(j << 1) + 0]) * (startY - scnPts[(j << 1) + 1]) / (scnPts[(j << 1) + 3] - scnPts[(j << 1) + 1]);
						break;
					}
					else if ((scnPts[(j << 1)] - (centX - xDiff) >= 0) ^ (scnPts[(j << 1) + 2] - (centX - xDiff) >= 0))
					{
						startX = centX - xDiff;
						startY = scnPts[(j << 1) + 1] + (scnPts[(j << 1) + 3] - scnPts[(j << 1) + 1]) * (startX - scnPts[(j << 1)]) / (scnPts[(j << 1) + 2] - scnPts[(j << 1)]);
						break;
					}
					else if ((scnPts[(j << 1)] - (centX + xDiff) >= 0) ^ (scnPts[(j << 1) + 2] - (centX + xDiff) >= 0))
					{
						startX = centX + xDiff;
						startY = scnPts[(j << 1) + 1] + (scnPts[(j << 1) + 3] - scnPts[(j << 1) + 1]) * (startX - scnPts[(j << 1)]) / (scnPts[(j << 1) + 2] - scnPts[(j << 1)]);
						break;
					}
				}

				j++;
			}
			if (j >= nPoints - 1)
			{
				j = nPoints - 2;
				startX = scnPts[(j << 1) + 2];
				startY = scnPts[(j << 1) + 3];
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
		UTF8Char *lbl = sbuff;
		UTF8Char l[2];
		UOSInt currInd;
		UOSInt lastInd;
		UOSInt cnt;
		cnt = lblSize;
		currX = startX;
		currY = startY;
		lastInd = (UOSInt)-1;
		currInd = j;

		while (cnt--)
		{
			if (lastInd != currInd)
			{
				lastInd = currInd;

				xDiff = scnPts[(currInd << 1) + 2] - scnPts[(currInd << 1) + 0];
				yDiff = scnPts[(currInd << 1) + 3] - scnPts[(currInd << 1) + 1];
				if (xDiff > 0)
				{
					axDiff = xDiff;
				}
				else
				{
					axDiff = -xDiff;
				}
				if (yDiff > 0)
				{
					ayDiff = yDiff;
				}
				else
				{
					ayDiff = -yDiff;
				}

				if (mode == 0)
				{
					nextX = scnPts[(currInd << 1) + 2];
					nextY = scnPts[(currInd << 1) + 3];
				}
				else
				{
					nextX = scnPts[(currInd << 1) + 0];
					nextY = scnPts[(currInd << 1) + 1];
				}
			}

			img->GetTextSize(font->font, {lbl, 1}, szThis);
			l[0] = lbl[0];
			l[1] = 0;
			if (font->fontType == 0)
			{
				img->DrawString(currX - (szThis[0] * 0.5), currY - (szThis[1] * 0.5), {l, 1}, font->font, (Media::DrawBrush*)font->other);
			}
			else
			{
				img->DrawStringB(currX - (szThis[0] * 0.5), currY - (szThis[1] * 0.5), {l, 1}, font->font, (Media::DrawBrush*)font->other, (UInt32)Double2Int32(font->thick));
			}

			found = false;
			if (mode == 0)
			{
				if (axDiff > ayDiff)
				{
					if (xDiff > 0)
					{
						if (currX + szThis[0] <= nextX)
						{
							currX += szThis[0];
							currY = scnPts[(currInd << 1) + 1] + (scnPts[(currInd << 1) + 3] - scnPts[(currInd << 1) + 1]) * (currX - scnPts[(currInd << 1)]) / (scnPts[(currInd << 1) + 2] - scnPts[(currInd << 1)]);
							found = true;
						}
						else
						{
							nextX = currX + szThis[0];
						}
					}
					else
					{
						if (currX - szThis[0] >= nextX)
						{
							currX -= szThis[0];
							currY = scnPts[(currInd << 1) + 1] + (scnPts[(currInd << 1) + 3] - scnPts[(currInd << 1) + 1]) * (currX - scnPts[(currInd << 1)]) / (scnPts[(currInd << 1) + 2] - scnPts[(currInd << 1)]);
							found = true;
						}
						else
						{
							nextX = currX - szThis[0];
						}
					}
					if (!found)
					{
						currInd++;
						while (currInd < nPoints - 1)
						{
							if (((scnPts[(currInd << 1)] - nextX > 0) ^ (scnPts[(currInd << 1) + 2] - nextX > 0)) || (scnPts[(currInd << 1)] == nextX) || (scnPts[(currInd << 1) + 2] == nextX))
							{
								currX = nextX;
								currY = scnPts[(currInd << 1) + 1] + (scnPts[(currInd << 1) + 3] - scnPts[(currInd << 1) + 1]) * (currX - scnPts[(currInd << 1)]) / (scnPts[(currInd << 1) + 2] - scnPts[(currInd << 1)]);
								found = true;
								break;
							}
							else if (((scnPts[(currInd << 1) + 1] - (currY - szThis[1]) > 0) ^ (scnPts[(currInd << 1) + 3] - (currY - szThis[1]) > 0)) || (scnPts[(currInd << 1) + 1] == (currY - szThis[1])) || (scnPts[(currInd << 1) + 3] == (currY - szThis[1])))
							{
								currY = currY - szThis[1];
								currX = scnPts[(currInd << 1) + 0] + (scnPts[(currInd << 1) + 2] - scnPts[(currInd << 1) + 0]) * (currY - scnPts[(currInd << 1) + 1]) / (scnPts[(currInd << 1) + 3] - scnPts[(currInd << 1) + 1]);
								found = true;
								break;
							}
							else if (((scnPts[(currInd << 1) + 1] - (currY + szThis[1]) > 0) ^ (scnPts[(currInd << 1) + 3] - (currY + szThis[1]) > 0)) || (scnPts[(currInd << 1) + 1] == (currY + szThis[1])) || (scnPts[(currInd << 1) + 3] == (currY + szThis[1])))
							{
								currY = currY + szThis[1];
								currX = scnPts[(currInd << 1) + 0] + (scnPts[(currInd << 1) + 2] - scnPts[(currInd << 1) + 0]) * (currY - scnPts[(currInd << 1) + 1]) / (scnPts[(currInd << 1) + 3] - scnPts[(currInd << 1) + 1]);
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
					if (yDiff > 0)
					{
						if (currY + szThis[1] <= nextY)
						{
							currY += szThis[1];
							currX = scnPts[(currInd << 1) + 0] + (scnPts[(currInd << 1) + 2] - scnPts[(currInd << 1) + 0]) * (currY - scnPts[(currInd << 1) + 1]) / (scnPts[(currInd << 1) + 3] - scnPts[(currInd << 1) + 1]);
							found = true;
						}
						else
						{
							nextY = currY + szThis[1];
						}
					}
					else
					{
						if (currY - szThis[1] >= nextY)
						{
							currY -= szThis[1];
							currX = scnPts[(currInd << 1) + 0] + (scnPts[(currInd << 1) + 2] - scnPts[(currInd << 1) + 0]) * (currY - scnPts[(currInd << 1) + 1]) / (scnPts[(currInd << 1) + 3] - scnPts[(currInd << 1) + 1]);
							found = true;
						}
						else
						{
							nextY = currY - szThis[1];
						}
					}
					if (!found)
					{
						currInd++;
						while (currInd < nPoints - 1)
						{
							if (((scnPts[(currInd << 1) + 1] - nextY > 0) ^ (scnPts[(currInd << 1) + 3] - nextY > 0)) || (scnPts[(currInd << 1) + 1] == nextY) || (scnPts[(currInd << 1) + 3] == nextY))
							{
								currY = nextY;
								currX = scnPts[(currInd << 1) + 0] + (scnPts[(currInd << 1) + 2] - scnPts[(currInd << 1) + 0]) * (currY - scnPts[(currInd << 1) + 1]) / (scnPts[(currInd << 1) + 3] - scnPts[(currInd << 1) + 1]);
								found = true;
								break;
							}
							else if (((scnPts[(currInd << 1) + 0] - (currX - szThis[0]) > 0) ^ (scnPts[(currInd << 1) + 2] - (currX - szThis[0]) > 0)) || (scnPts[(currInd << 1) + 0] == (currX - szThis[0])) || (scnPts[(currInd << 1) + 2] == (currX - szThis[0])))
							{
								currX = currX - szThis[0];
								currY = scnPts[(currInd << 1) + 1] + (scnPts[(currInd << 1) + 3] - scnPts[(currInd << 1) + 1]) * (currX - scnPts[(currInd << 1)]) / (scnPts[(currInd << 1) + 2] - scnPts[(currInd << 1)]);
								found = true;
								break;
							}
							else if (((scnPts[(currInd << 1) + 0] - (currX + szThis[0]) > 0) ^ (scnPts[(currInd << 1) + 2] - (currX + szThis[0]) > 0)) || (scnPts[(currInd << 1) + 0] == (currX + szThis[0])) || (scnPts[(currInd << 1) + 2] == (currX + szThis[0])))
							{
								currX = currX + szThis[0];
								currY = scnPts[(currInd << 1) + 1] + (scnPts[(currInd << 1) + 3] - scnPts[(currInd << 1) + 1]) * (currX - scnPts[(currInd << 1)]) / (scnPts[(currInd << 1) + 2] - scnPts[(currInd << 1)]);
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
				if (axDiff > ayDiff)
				{
					if (xDiff < 0)
					{
						if (currX + szThis[0] <= nextX)
						{
							currX += szThis[0];
							currY = scnPts[(currInd << 1) + 1] + (scnPts[(currInd << 1) + 3] - scnPts[(currInd << 1) + 1]) * (currX - scnPts[(currInd << 1)]) / (scnPts[(currInd << 1) + 2] - scnPts[(currInd << 1)]);
							found = true;
						}
						else
						{
							nextX = currX + szThis[0];
						}
					}
					else
					{
						if (currX - szThis[0] >= nextX)
						{
							currX -= szThis[0];
							currY = scnPts[(currInd << 1) + 1] + (scnPts[(currInd << 1) + 3] - scnPts[(currInd << 1) + 1]) * (currX - scnPts[(currInd << 1)]) / (scnPts[(currInd << 1) + 2] - scnPts[(currInd << 1)]);
							found = true;
						}
						else
						{
							nextX = currX - szThis[0];
						}
					}
					if (!found)
					{
						while (currInd > 0)
						{
							currInd--;
							if (((scnPts[(currInd << 1)] - nextX > 0) ^ (scnPts[(currInd << 1) + 2] - nextX > 0)) || (scnPts[(currInd << 1)] == nextX) || (scnPts[(currInd << 1) + 2] == nextX))
							{
								currX = nextX;
								currY = scnPts[(currInd << 1) + 1] + (scnPts[(currInd << 1) + 3] - scnPts[(currInd << 1) + 1]) * (currX - scnPts[(currInd << 1)]) / (scnPts[(currInd << 1) + 2] - scnPts[(currInd << 1)]);
								found = true;
								break;
							}
							else if (((scnPts[(currInd << 1) + 1] - (currY - szThis[1]) > 0) ^ (scnPts[(currInd << 1) + 3] - (currY - szThis[1]) > 0)) || (scnPts[(currInd << 1) + 1] == (currY - szThis[1])) || (scnPts[(currInd << 1) + 3] == (currY - szThis[1])))
							{
								currY = currY - szThis[1];
								currX = scnPts[(currInd << 1) + 0] + (scnPts[(currInd << 1) + 2] - scnPts[(currInd << 1) + 0]) * (currY - scnPts[(currInd << 1) + 1]) / (scnPts[(currInd << 1) + 3] - scnPts[(currInd << 1) + 1]);
								found = true;
								break;
							}
							else if (((scnPts[(currInd << 1) + 1] - (currY + szThis[1]) > 0) ^ (scnPts[(currInd << 1) + 3] - (currY + szThis[1]) > 0)) || (scnPts[(currInd << 1) + 1] == (currY + szThis[1])) || (scnPts[(currInd << 1) + 3] == (currY + szThis[1])))
							{
								currY = currY + szThis[1];
								currX = scnPts[(currInd << 1) + 0] + (scnPts[(currInd << 1) + 2] - scnPts[(currInd << 1) + 0]) * (currY - scnPts[(currInd << 1) + 1]) / (scnPts[(currInd << 1) + 3] - scnPts[(currInd << 1) + 1]);
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
					if (yDiff < 0)
					{
						if (currY + szThis[1] <= nextY)
						{
							currY += szThis[1];
							currX = scnPts[(currInd << 1) + 0] + (scnPts[(currInd << 1) + 2] - scnPts[(currInd << 1) + 0]) * (currY - scnPts[(currInd << 1) + 1]) / (scnPts[(currInd << 1) + 3] - scnPts[(currInd << 1) + 1]);
							found = true;
						}
						else
						{
							nextY = currY + szThis[1];
						}
					}
					else
					{
						if (currY - szThis[1] >= nextY)
						{
							currY -= szThis[1];
							currX = scnPts[(currInd << 1) + 0] + (scnPts[(currInd << 1) + 2] - scnPts[(currInd << 1) + 0]) * (currY - scnPts[(currInd << 1) + 1]) / (scnPts[(currInd << 1) + 3] - scnPts[(currInd << 1) + 1]);
							found = true;
						}
						else
						{
							nextY = currY - szThis[1];
						}
					}
					if (!found)
					{
						while (currInd > 0)
						{
							currInd--;
							if (((scnPts[(currInd << 1) + 1] - nextY > 0) ^ (scnPts[(currInd << 1) + 3] - nextY > 0)) || (scnPts[(currInd << 1) + 1] == nextY) || (scnPts[(currInd << 1) + 3] == nextY))
							{
								currY = nextY;
								currX = scnPts[(currInd << 1) + 0] + (scnPts[(currInd << 1) + 2] - scnPts[(currInd << 1) + 0]) * (currY - scnPts[(currInd << 1) + 1]) / (scnPts[(currInd << 1) + 3] - scnPts[(currInd << 1) + 1]);
								found = true;
								break;
							}
							else if (((scnPts[(currInd << 1) + 0] - (currX - szThis[0]) > 0) ^ (scnPts[(currInd << 1) + 2] - (currX - szThis[0]) > 0)) || (scnPts[(currInd << 1) + 0] == (currX - szThis[0])) || (scnPts[(currInd << 1) + 2] == (currX - szThis[0])))
							{
								currX = currX - szThis[0];
								currY = scnPts[(currInd << 1) + 1] + (scnPts[(currInd << 1) + 3] - scnPts[(currInd << 1) + 1]) * (currX - scnPts[(currInd << 1)]) / (scnPts[(currInd << 1) + 2] - scnPts[(currInd << 1)]);
								found = true;
								break;
							}
							else if (((scnPts[(currInd << 1) + 0] - (currX + szThis[0]) > 0) ^ (scnPts[(currInd << 1) + 2] - (currX + szThis[0]) > 0)) || (scnPts[(currInd << 1) + 0] == (currX + szThis[0])) || (scnPts[(currInd << 1) + 2] == (currX + szThis[0])))
							{
								currX = currX + szThis[0];
								currY = scnPts[(currInd << 1) + 1] + (scnPts[(currInd << 1) + 3] - scnPts[(currInd << 1) + 1]) * (currX - scnPts[(currInd << 1)]) / (scnPts[(currInd << 1) + 2] - scnPts[(currInd << 1)]);
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
		i++;
	}
}

void Map::MapConfig2TGen::DrawCharsL(Media::DrawImage *img, Text::CString str1, Double *mapPts, Int32 *scnPts, UOSInt nPoints, UInt32 thisPt, Double scaleN, Double scaleD, Data::ArrayList<MapFontStyle*> *fontStyle, Double *realBounds)
{
	UTF8Char sbuff[256];
	str1.ConcatTo(sbuff);
	UOSInt lblSize = str1.leng;
	Double centX = scnPts[thisPt << 1] + (scnPts[(thisPt << 1) + 2] - scnPts[(thisPt << 1)]) * scaleN / scaleD;
	Double centY = scnPts[(thisPt << 1) + 1] + (scnPts[(thisPt << 1) + 3] - scnPts[(thisPt << 1) + 1]) * scaleN / scaleD;
	Double currX;
	Double currY;
	Double nextX;
	Double nextY;
	Double startX = 0;
	Double startY = 0;
	Double xDiff;
	Double yDiff;
	Double axDiff;
	Double ayDiff;
	Double minX;
	Double maxX;
	Double minY;
	Double maxY;
	Double angleOfst;
	UOSInt i;
	UOSInt j;
	Double szThis[2];
	Double szLast[2];
	Int32 mode;
	UOSInt fntCount;
	Map::MapFontStyle *font;

	if (fontStyle == 0)
		return;

	minX = maxX = centX;
	minY = maxY = centY;

	fntCount = fontStyle->GetCount();
	font = (Map::MapFontStyle*)fontStyle->GetItem(0);
	if (str1.Equals(UTF8STRC("King Cho Road")))
	{
		i = 0;
	}

	xDiff = scnPts[(thisPt << 1) + 2] - scnPts[(thisPt << 1) + 0];
	yDiff = scnPts[(thisPt << 1) + 3] - scnPts[(thisPt << 1) + 1];
	if (xDiff > 0)
	{
		axDiff = xDiff;
	}
	else
	{
		axDiff = -xDiff;
	}
	if (yDiff > 0)
	{
		ayDiff = yDiff;
	}
	else
	{
		ayDiff = -yDiff;
	}

	if (xDiff > 0)
	{
		mode = 0;
		angleOfst = 0;
	}
	else
	{
		mode = 1;
		angleOfst = PI;
	}
/*	if (axDiff > ayDiff)
	{
		if (xDiff > 0)
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
		if (yDiff > 0)
		{
			mode = 0;
		}
		else
		{
			mode = 1;
		}
	}*/

	currX = centX;
	currY = centY;
	i = lblSize;
	j = thisPt;
	nextX = xDiff;
	nextY = yDiff;
	xDiff = 0;
	yDiff = 0;

	img->GetTextSize(font->font, str1, szThis);
	xDiff = szThis[0] * 0.5;
	yDiff = xDiff * xDiff;

	if (mode == 0)
	{
		while (j >= 0)
		{
			startX = scnPts[(j << 1) + 0] - centX;
			startY = scnPts[(j << 1) + 1] - centY;
			xDiff = (startX * startX) + (startY * startY);
			if (xDiff >= yDiff)
			{
				if (startX > 0)
				{
					axDiff = startX;
				}
				else
				{
					axDiff = -startX;
				}
				if (startY > 0)
				{
					ayDiff = startY;
				}
				else
				{
					ayDiff = -startY;
				}

				if (axDiff > ayDiff)
				{
					startX = centX + (startX * Math_Sqrt(yDiff) / Math_Sqrt(xDiff));
					startY = scnPts[(j << 1) + 1] + (scnPts[(j << 1) + 3] - scnPts[(j << 1) + 1]) * (startX - scnPts[(j << 1)]) / (scnPts[(j << 1) + 2] - scnPts[(j << 1)]);
				}
				else
				{
					startY = centY + (startY * Math_Sqrt(yDiff) / Math_Sqrt(xDiff));
					startX = scnPts[(j << 1) + 0] + (scnPts[(j << 1) + 2] - scnPts[(j << 1) + 0]) * (startY - scnPts[(j << 1) + 1]) / (scnPts[(j << 1) + 3] - scnPts[(j << 1) + 1]);
				}
				break;
			}
			if (j <= 0)
			{
				startX = scnPts[0];
				startY = scnPts[1];
				break;
			}
			j--;
		}
	}
	else
	{
		while (j < nPoints - 1)
		{
			startX = scnPts[(j << 1) + 2] - centX;
			startY = scnPts[(j << 1) + 3] - centY;
			xDiff = (startX * startX) + (startY * startY);
			if (xDiff >= yDiff)
			{
				if (startX > 0)
				{
					axDiff = startX;
				}
				else
				{
					axDiff = -startX;
				}
				if (startY > 0)
				{
					ayDiff = startY;
				}
				else
				{
					ayDiff = -startY;
				}

				if (axDiff > ayDiff)
				{
					startX = centX + (startX * Math_Sqrt(yDiff) / Math_Sqrt(xDiff));
					startY = scnPts[(j << 1) + 1] + (scnPts[(j << 1) + 3] - scnPts[(j << 1) + 1]) * (startX - scnPts[(j << 1)]) / (scnPts[(j << 1) + 2] - scnPts[(j << 1)]);
				}
				else
				{
					startY = centY + (startY * Math_Sqrt(yDiff) / Math_Sqrt(xDiff));
					startX = scnPts[(j << 1) + 0] + (scnPts[(j << 1) + 2] - scnPts[(j << 1) + 0]) * (startY - scnPts[(j << 1) + 1]) / (scnPts[(j << 1) + 3] - scnPts[(j << 1) + 1]);
				}
				break;
			}

			j++;
			if (j >= nPoints - 1)
			{
				j = nPoints - 2;
				startX = scnPts[(j << 1) + 2];
				startY = scnPts[(j << 1) + 3];
				break;
			}
		}
	}

	UOSInt startInd = j;
	img->SetTextAlign(Media::DrawEngine::DRAW_POS_CENTER);
	i = 0;
	while (i < fntCount)
	{
		font = (Map::MapFontStyle *)fontStyle->GetItem(i);
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

		szLast[0] = 0;

		lastX = currX = startX;
		lastY = currY = startY;
		j = startInd;
		UOSInt lastInd = j;

		angle = angleOfst - Math_ArcTan2((mapPts[(j << 1) + 1] - mapPts[(j << 1) + 3]), (mapPts[(j << 1) + 2] - mapPts[(j << 1) + 0]));
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

			img->GetTextSize(font->font, CSTRP(lbl, nextPos), szThis);
			dist = (szLast[0] + szThis[0]) * 0.5;
			nextX = currX + dist * cosAngle;
			nextY = currY - dist * sinAngle;
			if ( (((nextX > scnPts[(j << 1)]) ^ (nextX > scnPts[(j << 1) + 2])) || (nextX == scnPts[(j << 1)]) || (nextX == scnPts[(j << 1) + 2])) && (((nextY > scnPts[(j << 1) + 1]) ^ (nextY > scnPts[(j << 1) + 3])) || (nextY == scnPts[(j << 1) + 1]) || (nextY == scnPts[(j << 1) + 3])))
			{
				currX = nextX;
				currY = nextY;
			}
			else
			{
				xDiff = szLast[0] + szThis[0];
				yDiff = (xDiff * xDiff) * 0.25;

				if (mode == 0)
				{
					j++;
					while (j < nPoints - 1)
					{
						nextX = scnPts[(j << 1) + 2] - currX;
						nextY = scnPts[(j << 1) + 3] - currY;
						xDiff = (nextX * nextX) + (nextY * nextY);
						if (xDiff < yDiff)
						{
							j++;
						}
						else
						{
							if (nextX > 0)
							{
								axDiff = nextX;
							}
							else
							{
								axDiff = -nextX;
							}
							if (nextY > 0)
							{
								ayDiff = nextY;
							}
							else
							{
								ayDiff = -nextY;
							}

							if (axDiff > ayDiff)
							{
								if ((scnPts[(j << 1) + 0] < scnPts[(j << 1) + 2]) ^ (nextX > 0))
								{
									currX = currX - (nextX * Math_Sqrt(yDiff) / Math_Sqrt(xDiff));
								}
								else
								{
									currX = currX + (nextX * Math_Sqrt(yDiff) / Math_Sqrt(xDiff));
								}
								if (((currX > scnPts[(j << 1)]) ^ (currX > scnPts[(j << 1) + 2])) || (currX == scnPts[(j << 1)]) || (currX == scnPts[(j << 1) + 2]))
								{
								}
								else
								{
									currX = scnPts[(j << 1) + 0];
								}
								currY = scnPts[(j << 1) + 1] + (scnPts[(j << 1) + 3] - scnPts[(j << 1) + 1]) * (currX - scnPts[(j << 1)]) / (scnPts[(j << 1) + 2] - scnPts[(j << 1)]);
							}
							else
							{
								if ((scnPts[(j << 1) + 1] < scnPts[(j << 1) + 3]) ^ (nextY > 0))
								{
									currY = currY - (nextY * Math_Sqrt(yDiff) / Math_Sqrt(xDiff));
								}
								else
								{
									currY = currY + (nextY * Math_Sqrt(yDiff) / Math_Sqrt(xDiff));
								}
								if (((currY > scnPts[(j << 1) + 1]) ^ (currY > scnPts[(j << 1) + 3])) || (currY == scnPts[(j << 1) + 1]) || (currY == scnPts[(j << 1) + 3]))
								{
								}
								else
								{
									currY = scnPts[(j << 1) + 1];
								}
								currX = scnPts[(j << 1) + 0] + (scnPts[(j << 1) + 2] - scnPts[(j << 1) + 0]) * (currY - scnPts[(j << 1) + 1]) / (scnPts[(j << 1) + 3] - scnPts[(j << 1) + 1]);
							}
							break;
						}
					}
					if (j == nPoints - 1)
					{
						j--;

						currX = currX + (dist * cosAngle);
						currY = currY - (dist * sinAngle);
					}
				}
				else if (mode == 1)
				{
					while (j-- > 0)
					{
						nextX = scnPts[(j << 1) + 0] - currX;
						nextY = scnPts[(j << 1) + 1] - currY;
						xDiff = (nextX * nextX) + (nextY * nextY);
						if (xDiff < yDiff)
						{

						}
						else
						{
							if (nextX > 0)
							{
								axDiff = nextX;
							}
							else
							{
								axDiff = -nextX;
							}
							if (nextY > 0)
							{
								ayDiff = nextY;
							}
							else
							{
								ayDiff = -nextY;
							}

							if (axDiff > ayDiff)
							{
								if ((scnPts[(j << 1) + 0] < scnPts[(j << 1) + 2]) ^ (nextX > 0))
								{
									currX = currX + (nextX * Math_Sqrt(yDiff) / Math_Sqrt(xDiff));
								}
								else
								{
									currX = currX - (nextX * Math_Sqrt(yDiff) / Math_Sqrt(xDiff));
								}
								if (((currX > scnPts[(j << 1)]) ^ (currX > scnPts[(j << 1) + 2])) || (currX == scnPts[(j << 1)]) || (currX == scnPts[(j << 1) + 2]))
								{
								}
								else
								{
									currX = scnPts[(j << 1) + 2];
								}
								currY = scnPts[(j << 1) + 1] + (scnPts[(j << 1) + 3] - scnPts[(j << 1) + 1]) * (currX - scnPts[(j << 1)]) / (scnPts[(j << 1) + 2] - scnPts[(j << 1)]);
							}
							else
							{
								if ((scnPts[(j << 1) + 1] < scnPts[(j << 1) + 3]) ^ (nextY > 0))
								{
									currY = currY + (nextY * Math_Sqrt(yDiff) / Math_Sqrt(xDiff));
								}
								else
								{
									currY = currY - (nextY * Math_Sqrt(yDiff) / Math_Sqrt(xDiff));
								}
								if (((currY > scnPts[(j << 1) + 1]) ^ (currY > scnPts[(j << 1) + 3])) || (currY == scnPts[(j << 1) + 1]) || (currY == scnPts[(j << 1) + 3]))
								{
								}
								else
								{
									currY = scnPts[(j << 1) + 3];
								}
								currX = scnPts[(j << 1) + 0] + (scnPts[(j << 1) + 2] - scnPts[(j << 1) + 0]) * (currY - scnPts[(j << 1) + 1]) / (scnPts[(j << 1) + 3] - scnPts[(j << 1) + 1]);
							}
							break;
						}
					}
					if (j == (UOSInt)-1)
					{
						j = 0;
						currX = currX + (dist * cosAngle);
						currY = currY - (dist * sinAngle);
					}
				}

				if (j != lastInd)
				{
					lastInd = j;
					angle = angleOfst - Math_ArcTan2((mapPts[(j << 1) + 1] - mapPts[(j << 1) + 3]), (mapPts[(j << 1) + 2] - mapPts[(j << 1) + 0]));
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
				currX = lastX + (dist * lca);
				currY = lastY - (dist * lsa);

				Double xadd = szThis[0] * lca;
				Double yadd = szThis[0] * lsa;
				if (xadd < 0)
					xadd = -xadd;
				if (yadd < 0)
					yadd = -yadd;
				if ((currX - xadd) < minX)
				{
					minX = (currX - xadd);
				}
				if ((currX + xadd) > maxX)
				{
					maxX = (currX + xadd);
				}
				if ((currY - yadd) < minY)
				{
					minY = (currY - yadd);
				}
				if ((currY + yadd) > maxY)
				{
					maxY = (currY + yadd);
				}

				if (mode == 0)
				{
					if (font->fontType == 0)
					{
						img->DrawStringRot(currX, currY, CSTRP(lbl, nextPos), font->font, (Media::DrawBrush*)font->other, lastAngle);
					}
					else
					{
						img->DrawStringRotB(currX, currY, CSTRP(lbl, nextPos), font->font, (Media::DrawBrush*)font->other, lastAngle, (UInt32)Double2Int32(font->thick));
					}
				}
				else
				{
					if (font->fontType == 0)
					{
						img->DrawStringRot(currX, currY, CSTRP(lbl, nextPos), font->font, (Media::DrawBrush*)font->other, lastAngle);
					}
					else
					{
						img->DrawStringRotB(currX, currY, CSTRP(lbl, nextPos), font->font, (Media::DrawBrush*)font->other, lastAngle, (UInt32)Double2Int32(font->thick));
					}
				}
			}
			else
			{
				lastAngle = angleDegree;
				Double xadd = szThis[0] * cosAngle;
				Double yadd = szThis[0] * sinAngle;
				if (xadd < 0)
					xadd = -xadd;
				if (yadd < 0)
					yadd = -yadd;
				if ((currX - xadd) < minX)
				{
					minX = (currX - xadd);
				}
				if ((currX + xadd) > maxX)
				{
					maxX = (currX + xadd);
				}
				if ((currY - yadd) < minY)
				{
					minY = (currY - yadd);
				}
				if ((currY + yadd) > maxY)
				{
					maxY = (currY + yadd);
				}

				if (mode == 0)
				{
					if (font->fontType == 0)
					{
						img->DrawStringRot(currX, currY, CSTRP(lbl, nextPos), font->font, (Media::DrawBrush*)font->other, angleDegree);
					}
					else
					{
						img->DrawStringRotB(currX, currY, CSTRP(lbl, nextPos), font->font, (Media::DrawBrush*)font->other, angleDegree, (UInt32)Double2Int32(font->thick));
					}
				}
				else
				{
					if (font->fontType == 0)
					{
						img->DrawStringRot(currX, currY, CSTRP(lbl, nextPos), font->font, (Media::DrawBrush*)font->other, angleDegree);
					}
					else
					{
						img->DrawStringRotB(currX, currY, CSTRP(lbl, nextPos), font->font, (Media::DrawBrush*)font->other, angleDegree, (UInt32)Double2Int32(font->thick));
					}
				}

			}
			lastX = currX;
			lastY = currY;
			szLast[0] = szThis[0];
		}
		i++;
	}
	realBounds[0] = minX;
	realBounds[1] = minY;
	realBounds[2] = maxX;
	realBounds[3] = maxY;
}

void Map::MapConfig2TGen::GetCharsSize(Media::DrawImage *img, Double *size, Text::CString label, Data::ArrayList<MapFontStyle*> *fontStyle, Double scaleW, Double scaleH)
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

	Double szTmp[2];
	font = (Map::MapFontStyle*)fontStyle->GetItem(maxIndex);
	img->GetTextSize(font->font, label, szTmp);

	if (scaleH == 0)
	{
		size[0] = szTmp[0] + xSizeAdd;
		size[1] = szTmp[1] + ySizeAdd;

		return;
	}


	Double pt[8];

	if (scaleW < 0)
	{
		scaleW = -scaleW;
		scaleH = -scaleH;
	}
	Double degD = Math_ArcTan2(scaleH, scaleW);
	Double xPos = szTmp[0] + xSizeAdd;
	Double yPos = szTmp[1] + ySizeAdd;
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
	size[0] = maxX - minX;
	size[1] = maxY - minY;
}

UInt32 Map::MapConfig2TGen::ToColor(const UTF8Char *str)
{
	UInt32 v = (UInt32)Text::StrHex2Int32C(str);
	return 0xff000000 | ((v & 0xff) << 16) | (v & 0xff00) | ((v >> 16) & 0xff);
}

Map::IMapDrawLayer *Map::MapConfig2TGen::GetDrawLayer(Text::CString name, Data::ArrayList<Map::IMapDrawLayer*> *layerList, IO::Writer *errWriter)
{
	Map::CIPLayer2 *cip;
	UOSInt i = layerList->GetCount();
	while (i-- > 0)
	{
		Map::IMapDrawLayer *lyr;
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

void Map::MapConfig2TGen::DrawPoints(Media::DrawImage *img, MapLayerStyle *lyrs, Map::MapView *view, Bool *isLayerEmpty, Map::MapScheduler *sch, Media::DrawEngine *eng, Media::IImgResizer *resizer, Double *objBounds, UOSInt *objCnt, UOSInt maxObjCnt)
{
	Data::ArrayListInt64 *arri;
	Map::DrawObjectL *dobj;
	UOSInt imgW;
	UOSInt imgH;
	UOSInt i;
#ifdef NOSCH
	UOSInt j;
	Int32 pts[2];
	Double *objPtr = &objBounds[4 * *objCnt];
#endif
	void *session;

#ifndef NOSCH
	sch->SetDrawType(lyrs->lyr, Map::MapScheduler::MSDT_POINTS, 0, 0, lyrs->img, UOSInt2Double(lyrs->img->GetWidth()) * 0.5, UOSInt2Double(lyrs->img->GetHeight()) * 0.5, isLayerEmpty);
	sch->SetDrawObjs(objBounds, objCnt, maxObjCnt);
#endif
	NEW_CLASS(arri, Data::ArrayListInt64());
	lyrs->lyr->GetObjectIdsMapXY(arri, 0, view->GetLeftX() - (view->GetRightX() - view->GetLeftX()), view->GetTopY() - (view->GetBottomY() - view->GetTopY()), view->GetRightX() + (view->GetRightX() - view->GetLeftX()), view->GetBottomY() + (view->GetBottomY() - view->GetTopY()), true);
	if (arri->GetCount() <= 0)
	{
		DEL_CLASS(arri);
		return;
	}
	session = lyrs->lyr->BeginGetObject();
	Media::DrawImage *dimg;
	if (img->GetHDPI() != 96)
	{
		imgW = lyrs->img->GetWidth();
		imgH = lyrs->img->GetHeight();
		Media::DrawImage *gimg2 = lyrs->img;
		Media::DrawImage *gimg = eng->CreateImage32((UInt32)Double2Int32(UOSInt2Double(imgW) * img->GetHDPI() / 96.0), (UInt32)Double2Int32(UOSInt2Double(imgH) * img->GetHDPI() / 96.0), gimg2->GetAlphaType());
		gimg->SetAlphaType(gimg2->GetAlphaType());
		Bool revOrder;
		Bool revOrder2;
		UInt8 *bmpBits = gimg->GetImgBits(&revOrder);
		UInt8 *bmpBits2 = gimg2->GetImgBits(&revOrder2);
		resizer->Resize(bmpBits2, (OSInt)imgW << 2, UOSInt2Double(imgW), UOSInt2Double(imgH), 0, 0, bmpBits, Double2Int32(UOSInt2Double(imgW) * img->GetHDPI() / 96.0) << 2, (UInt32)Double2Int32(UOSInt2Double(imgW) * img->GetHDPI() / 96.0), (UInt32)Double2Int32(UOSInt2Double(imgH) * img->GetHDPI() / 96.0));
		gimg->GetImgBitsEnd(true);
		gimg2->GetImgBitsEnd(false);
		dimg = gimg;
		imgW = (UInt32)Double2Int32(UOSInt2Double(imgW) * img->GetHDPI() / 96.0) >> 1;
		imgH = (UInt32)Double2Int32(UOSInt2Double(imgH) * img->GetHDPI() / 96.0) >> 1;
#ifndef NOSCH
		sch->SetDrawType(lyrs->lyr, Map::MapScheduler::MSDT_POINTS, 0, 0, dimg, UOSInt2Double(dimg->GetWidth()) * 0.5, UOSInt2Double(dimg->GetHeight()) * 0.5, isLayerEmpty);
#endif
	}
	else
	{
		imgW = lyrs->img->GetWidth() >> 1;
		imgH = lyrs->img->GetHeight() >> 1;
		dimg = lyrs->img;
	}

	i = arri->GetCount();
	while (i-- > 0)
	{
		if ((dobj = lyrs->lyr->GetNewObjectById(session, arri->GetItem(i))) != 0)
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
			sch->Draw(dobj);
#endif
		}
	}

	lyrs->lyr->EndGetObject(session);
#ifndef NOSCH
	sch->WaitForFinish();
#endif
	if (img->GetHDPI() != 96)
	{
		eng->DeleteImage(dimg);
	}
	DEL_CLASS(arri);
}

void Map::MapConfig2TGen::DrawString(Media::DrawImage *img, MapLayerStyle *lyrs, Map::MapView *view, Data::ArrayList<MapFontStyle*> **fonts, MapLabels2 *labels, UOSInt maxLabels, UOSInt *labelCnt, Bool *isLayerEmpty)
{
	void *arr;
	Data::ArrayListInt64 *arri;
	UOSInt i;
	UInt32 j;
	Map::DrawObjectL *dobj;
	Double scaleW;
	Double scaleH;
	Math::Coord2DDbl pts;
	UTF8Char *sptr;
	UTF8Char *sptrEnd;
	UTF8Char lblStr[128];
	void *session;
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

	NEW_CLASS(arri, Data::ArrayListInt64());
	Double tmpSize = (view->GetRightX() - view->GetLeftX()) * 1.5;
	lyrs->lyr->GetObjectIdsMapXY(arri, &arr, view->GetLeftX() - tmpSize, view->GetTopY() - tmpSize, view->GetRightX() + tmpSize, view->GetBottomY() + tmpSize, false);
	session = lyrs->lyr->BeginGetObject();
	i = arri->GetCount();
	while (i-- > 0)
	{
		if ((dobj = lyrs->lyr->GetNewObjectById(session, arri->GetItem(i))) != 0)
		{
			if (lyrs->bkColor & SFLG_SMART)
			{
				UInt32 k;
				UInt32 maxSize;
				UInt32 maxPos;
				maxSize = dobj->nPoint - (maxPos = dobj->ptOfstArr[dobj->nPtOfst - 1]);
				k = dobj->nPtOfst;
				while (k-- > 1)
				{
					if ((dobj->ptOfstArr[k] - dobj->ptOfstArr[k - 1]) > maxSize)
						maxSize = (dobj->ptOfstArr[k] - (maxPos = dobj->ptOfstArr[k - 1]));
				}
				sptrEnd = lyrs->lyr->GetString(sptr = lblStr, sizeof(lblStr), arr, arri->GetItem(i), 0);
				if (AddLabel(labels, maxLabels, labelCnt, CSTRP(sptr, sptrEnd), maxSize, &dobj->pointArr[maxPos].x, lyrs->priority, lyrs->lyr->GetLayerType(), lyrs->style, lyrs->bkColor, view, UOSInt2Double(imgWidth) * view->GetHDPI() / view->GetDDPI(), (UOSInt2Double(imgHeight) * view->GetHDPI() / view->GetDDPI())))
				{
					lyrs->lyr->ReleaseObject(session, dobj);
				}
				else
				{
					lyrs->lyr->ReleaseObject(session, dobj);
				}
			}
			else if (lyrs->lyr->GetLayerType() == 3)
			{
				sptrEnd = lyrs->lyr->GetString(sptr = lblStr, sizeof(lblStr), arr, arri->GetItem(i), 0);
				if (dobj->nPoint & 1)
				{
					UOSInt k = dobj->nPoint >> 1;
					pts = dobj->pointArr[k];

					scaleW = dobj->pointArr[k + 1].x - dobj->pointArr[k - 1].x;
					scaleH = dobj->pointArr[k + 1].y - dobj->pointArr[k - 1].y;
				}
				else
				{
					UOSInt k = dobj->nPoint >> 1;
					pts = (dobj->pointArr[k - 1] + dobj->pointArr[k]) * 0.5;

					scaleW = dobj->pointArr[k].x - dobj->pointArr[k - 1].x;
					scaleH = dobj->pointArr[k].y - dobj->pointArr[k - 1].y;
				}

				if ((lyrs->bkColor & SFLG_ROTATE) == 0)
					scaleW = scaleH = 0;
				Double szThis[2];
				GetCharsSize(img, szThis, CSTRP(sptr, sptrEnd), fonts[lyrs->style], scaleW, scaleH);
				pts = view->MapXYToScnXY(pts);
				if ((pts.x + (szThis[0] * 0.5)) >= 0 && (pts.y + (szThis[1] * 0.5)) >= 0 && (pts.x - (szThis[0] * 0.5)) <= UOSInt2Double(view->GetScnWidth()) && (pts.y - (szThis[1] * 0.5)) <= UOSInt2Double(view->GetScnHeight()))
				{
					DrawChars(img, CSTRP(sptr, sptrEnd), pts.x, pts.y, scaleW, scaleH, fonts[lyrs->style], (lyrs->bkColor & SFLG_ALIGN) != 0);
				}
				lyrs->lyr->ReleaseObject(session, dobj);
			}
			else
			{
				Math::Coord2DDbl lastPt = {0, 0};
				Math::Coord2DDbl *pointPos = dobj->pointArr;
				sptrEnd = lyrs->lyr->GetString(sptr = lblStr, sizeof(lblStr), arr, arri->GetItem(i), 0);

				j = dobj->nPoint;
				while (j--)
				{
					lastPt += *pointPos;
					pointPos++;
				}

				pts = (lastPt / dobj->nPoint);
				Double szThis[2];
				GetCharsSize(img, szThis, CSTRP(sptr, sptrEnd), fonts[lyrs->style], 0, 0);
				pts = view->MapXYToScnXY(pts);
				if ((pts.x + (szThis[0] * 0.5)) >= 0 && (pts.y + (szThis[1] * 0.5)) >= 0 && (pts.x - (szThis[0] * 0.5)) <= UOSInt2Double(view->GetScnWidth()) && (pts.y - (szThis[1] * 0.5)) <= UOSInt2Double(view->GetScnHeight()))
				{
					DrawChars(img, CSTRP(sptr, sptrEnd), pts.x, pts.y, 0, 0, fonts[lyrs->style], (lyrs->bkColor & SFLG_ALIGN) != 0);
				}
				lyrs->lyr->ReleaseObject(session, dobj);
			}
		}
	}
	lyrs->lyr->EndGetObject(session);
	lyrs->lyr->ReleaseNameArr(arr);
	DEL_CLASS(arri);
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
			MemFree(labels[j].points);
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


Bool Map::MapConfig2TGen::AddLabel(MapLabels2 *labels, UOSInt maxLabel, UOSInt *labelCnt, Text::CString labelt, UOSInt nPoint, Double *points, Int32 priority, Int32 recType, UInt32 fontStyle, UInt32 flags, Map::MapView *view, Double xOfst, Double yOfst)
{
	Double size;
//	Int32 visibleSize;

	UOSInt i;
	UOSInt j;

	Double *ptPtr;

	Double scnX;
	Double scnY;
	Int32 found;

/*	Int32 left = view->GetLeft();
	Int32 right = view->GetRight();
	Int32 top = view->GetTop();
	Int32 bottom = view->GetBottom();
	Int32 mapPosX = view->GetCentX();
	Int32 mapPosY = view->GetCentY();*/

	if (recType == 1) //Point
	{
		Double xSum;
		Double ySum;

		found = 0;
		i = 0;
		while (i < *labelCnt)
		{
			if (recType == labels[i].shapeType)
			{
				if (labelt.Equals(labels[i].label))
				{
					found = 1;

					xSum = labels[i].scaleW * (Double)labels[i].totalSize;
					ySum = labels[i].scaleH * (Double)labels[i].totalSize;

					ptPtr = points;
					j = nPoint;
					while (j--)
					{
						xSum += *ptPtr++;
						ySum += *ptPtr++;
					}
					labels[i].totalSize += UOSInt2Double(nPoint);
					labels[i].scaleW = (xSum / labels[i].totalSize);
					labels[i].scaleH = (ySum / labels[i].totalSize);

					scnX = labels[i].scaleW - labels[i].xPos;
					scnY = labels[i].scaleH - labels[i].yPos;
					scnX = scnX * scnX + scnY * scnY;
					labels[i].currSize = scnX;

					ptPtr = points;
					j = nPoint;
					while (j--)
					{
						scnX = labels[i].scaleW - *ptPtr++;
						scnY = labels[i].scaleH - *ptPtr++;
						scnX = scnX * scnX + scnY * scnY;
						if (scnX < labels[i].currSize)
						{
							labels[i].xPos = ptPtr[-2];
							labels[i].yPos = ptPtr[-1];
							labels[i].fontStyle = fontStyle;
							labels[i].currSize = scnX;
							labels[i].xOfst = xOfst;
							labels[i].yOfst = yOfst;
							if (priority > labels[i].priority)
								labels[i].priority = priority;
						}
					}
					return true;
				}
			}
			i++;
		}

		if (found == 0 && nPoint > 0)
		{
			i = (UOSInt)-1;
			xSum = 0;
			ySum = 0;
			ptPtr = points;
			j = nPoint;
			while (j--)
			{
				xSum += *ptPtr++;
				ySum += *ptPtr++;
			}

			found = 1;

			i = NewLabel(labels, maxLabel, labelCnt, priority);
			if (i < 0)
			{
				return false;
			}

			labels[i].totalSize = UOSInt2Double(nPoint);
			labels[i].scaleW = (xSum / UOSInt2Double(nPoint));
			labels[i].scaleH = (ySum / UOSInt2Double(nPoint));
			labels[i].label = Text::String::New(labelt);
			labels[i].xPos = points[0];
			labels[i].yPos = points[1];
			labels[i].fontStyle = fontStyle;
			labels[i].priority = priority;

			scnX = labels[i].scaleW - points[0];
			scnY = labels[i].scaleH - points[1];
			scnX = scnX * scnX + scnY * scnY;
			labels[i].currSize = scnX;
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
				scnX = labels[i].scaleW - *ptPtr++;
				scnY = labels[i].scaleH - *ptPtr++;
				scnX = scnX * scnX + scnY * scnY;

				if (scnX < labels[i].currSize)
				{
					labels[i].xPos = ptPtr[-2];
					labels[i].yPos = ptPtr[-1];
					labels[i].currSize = scnX;
					if (priority > labels[i].priority)
						labels[i].priority = priority;
				}
			}
			return true;
		}
		return false;
	}
	else if (recType == 3) //lines
	{
		Double tmp;
		Double lastPtX;
		Double lastPtY = points[1];
		Double thisPtX;
		Double thisPtY;

		Int32 toUpdate;

//		visibleSize = 0;
		size = 0;

		i = 1;
		while (i < nPoint)
		{
			lastPtX = points[(i << 1) - 2];
			lastPtY = points[(i << 1) - 1];
			thisPtX = points[(i << 1) + 0];
			thisPtY = points[(i << 1) + 1];
			if (lastPtX > thisPtX)
			{
				tmp = lastPtX;
				lastPtX = thisPtX;
				thisPtX = tmp;
			}
			if (lastPtY > thisPtY)
			{
				tmp = lastPtY;
				lastPtY = thisPtY;
				thisPtY = tmp;
			}

			if ((thisPtY - lastPtY) > (thisPtX - lastPtX))
				size += thisPtY - lastPtY;
			else
				size += thisPtX - lastPtX;

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
		Double startX;
		Double startY;
		Double endX;
		Double endY;
		startX = points[0];
		startY = points[1];
		endX = points[(nPoint << 1) - 2];
		endY = points[(nPoint << 1) - 1];
		i = 0;

		while (i < *labelCnt)
		{
			if (recType == labels[i].shapeType)
			{
				if (labelt.Equals(labels[i].label))
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
					else if (labels[i].points[0] == endX && labels[i].points[1] == endY)
					{
//						wprintf(L"Shape: %s merged (%d + %d)\n", labelt, labels[i].nPoints, nPoint);
						UOSInt newSize = labels[i].nPoints + nPoint - 1;
						Double* newArr = MemAlloc(Double, newSize << 1);

						MemCopyNO(newArr, points, nPoint << 4);
						MemCopyNO(&newArr[nPoint << 1], &labels[i].points[2], (labels[i].nPoints - 1) << 4);

						startX = newArr[0];
						startY = newArr[1];
						endX = newArr[(newSize << 1) - 2];
						endY = newArr[(newSize << 1) - 1];

						MemFree(labels[i].points);
						labels[i].points = newArr;
						labels[i].nPoints = newSize;
						labels[i].currSize += size;//visibleSize;
						toUpdate = 0;
						foundInd = i;
					}
					else if (labels[i].points[(labels[i].nPoints << 1) - 2] == startX && labels[i].points[(labels[i].nPoints << 1) - 1] == startY)
					{
//						wprintf(L"Shape: %s merged (%d + %d)\n", labelt, labels[i].nPoints, nPoint);
						UOSInt newSize = labels[i].nPoints + nPoint - 1;
						Double* newArr = MemAlloc(Double, newSize << 1);

						MemCopyNO(newArr, labels[i].points, labels[i].nPoints << 4);
						MemCopyNO(&newArr[labels[i].nPoints << 1], &points[2], (nPoint - 1) << 4);

						startX = newArr[0];
						startY = newArr[1];
						endX = newArr[(newSize << 1) - 2];
						endY = newArr[(newSize << 1) - 1];

						MemFree(labels[i].points);
						labels[i].points = newArr;
						labels[i].nPoints = newSize;
						labels[i].currSize += size;//visibleSize;
						toUpdate = 0;
						foundInd = i;
					}
					else if (labels[i].points[0] == startX && labels[i].points[1] == startY)
					{
//						wprintf(L"Shape: %s inverse merged (%d + %d)\n", labelt, labels[i].nPoints, nPoint);
						UOSInt newSize = labels[i].nPoints + nPoint - 1;
						Double* newArr = MemAlloc(Double, newSize << 1);
						UOSInt k;
						UOSInt l;
						l = 0;
						k = labels[i].nPoints;
						while (k-- > 1)
						{
							newArr[l++] = labels[i].points[k << 1];
							newArr[l++] = labels[i].points[(k << 1) + 1];
						}
						MemCopyNO(&newArr[l], points, nPoint << 4);

						startX = newArr[0];
						startY = newArr[1];
						endX = newArr[(newSize << 1) - 2];
						endY = newArr[(newSize << 1) - 1];

						MemFree(labels[i].points);
						labels[i].points = newArr;
						labels[i].nPoints = newSize;
						labels[i].currSize += size;//visibleSize;
						toUpdate = 0;
						foundInd = i;
					}
					else if (labels[i].points[(labels[i].nPoints << 1) - 2] == endX && labels[i].points[(labels[i].nPoints << 1) - 1] == endY)
					{
//						wprintf(L"Shape: %s inverse merged (%d + %d)\n", labelt, labels[i].nPoints, nPoint);
						UOSInt newSize = labels[i].nPoints + nPoint - 1;
						Double* newArr = MemAlloc(Double, newSize << 1);
						MemCopyNO(newArr, labels[i].points, labels[i].nPoints << 4);
						UOSInt k;
						UOSInt l;
						l = labels[i].nPoints << 1;
						k = nPoint - 1;
						while (k-- > 0)
						{
							newArr[l++] = points[k << 1];
							newArr[l++] = points[(k << 1) + 1];
						}
						startX = newArr[0];
						startY = newArr[1];
						endX = newArr[(newSize << 1) - 2];
						endY = newArr[(newSize << 1) - 1];
						MemFree(labels[i].points);
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

			labels[i].label = Text::String::New(labelt);
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
				MemFree(labels[i].points);
			labels[i].points = ptPtr = MemAlloc(Double, nPoint * 2);
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
					if (labelt.Equals(labels[i].label))
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
			Double lastX;
			Double lastY;
			Double thisX;
			Double thisY;
			Double thisTX;
			Double thisTY;
			Double* outPts;
			UOSInt outPtCnt;
			Double sum;
			Double sumX;
			Double sumY;

			outPts = MemAlloc(Double, nPoint << 1);
			outPtCnt = 0;
			MemCopyNO(outPts, points, nPoint << 4);
			outPtCnt = nPoint;

			i = 0;
			sum = 0;
			sumX = sumY = 0;
			lastX = outPts[(outPtCnt << 1) - 2];
			lastY = outPts[(outPtCnt << 1) - 1];
			while (i < outPtCnt)
			{
				thisX = outPts[(i << 1)];
				thisY = outPts[(i << 1) + 1];

				sum += (lastX * thisY) - (lastY * thisX);

				lastX = thisX;
				lastY = thisY;
				i++;
			}
			if (sum != 0)
			{
				Double *finalPts;
				UInt32 finalCnt;
				Double maxX;
				Double maxY;
				Double minX;
				Double minY;
				finalCnt = 0;
				finalPts = MemAlloc(Double, outPtCnt << 1);
				sumX += maxX = minX = lastX = finalPts[0] = outPts[0];
				sumY += maxY = minY = lastY = finalPts[1] = outPts[1];
				finalCnt++;

				i = 2;
				while (i < outPtCnt)
				{
					thisX = outPts[(i << 1) - 2];
					thisY = outPts[(i << 1) - 1];
					if ((outPts[(i << 1)] - lastX) * (lastY - thisY) == (outPts[(i << 1) + 1] - lastY) * (lastX - thisX))
					{

					}
					else
					{
						sumX += finalPts[(finalCnt << 1)] = thisX;
						sumY += finalPts[(finalCnt << 1) + 1] = thisY;
						if (maxX < thisX)
							maxX = thisX;
						if (minX > thisX)
							minX = thisX;
						if (maxY < thisY)
							maxY = thisY;
						if (minY > thisY)
							minY = thisY;
						finalCnt++;
					}
					lastX = thisX;
					lastY = thisY;
					i++;
				}

				thisX = outPts[(outPtCnt << 1) - 2];
				thisY = outPts[(outPtCnt << 1) - 1];
				sumX += finalPts[(finalCnt << 1)] = thisX;
				sumY += finalPts[(finalCnt << 1) + 1] = thisY;
				finalCnt++;
				if (maxX < thisX)
					maxX = thisX;
				if (minX > thisX)
					minX = thisX;
				if (maxY < thisY)
					maxY = thisY;
				if (minY > thisY)
					minY = thisY;

				lastX = thisX;
				lastY = thisY;
				sum = 0;
				thisY = (maxY + minY) * 0.5;
				i = 0;
				while (i < finalCnt)
				{
					thisTX = finalPts[(i << 1)];
					thisTY = finalPts[(i << 1) + 1];
					if ((lastY >= thisY && thisTY < thisY) || (thisTY >= thisY && lastY < thisY))
					{
						thisX = lastX + (thisY - lastY) * (thisTX - lastX) / (thisTY - lastY);
						if (sum == 0)
						{
							minX = thisX;
							maxX = thisX;
						}
						else
						{
							if (thisX > maxX)
								maxX = thisX;
							if (thisX < minX)
								minX = thisX;
						}
						sum = 1;
					}
					lastX = thisTX;
					lastY = thisTY;
					i++;
				}


				MemFree(outPts);
				outPts = finalPts;
				outPtCnt = finalCnt;

				thisX = (maxX + minX) * 0.5;
				thisY = (maxY + minY) * 0.5;

				i = NewLabel(labels, maxLabel, labelCnt, priority);
				if (i < 0)
				{
					MemFree(outPts);
					return false;
				}

				labels[i].label = Text::String::New(labelt);
				labels[i].xPos = thisX;
				labels[i].yPos = thisY;
				labels[i].fontStyle = fontStyle;
				labels[i].scaleW = (maxX + minX) * 0.5; //& 1
				labels[i].scaleH = (maxY + minY) * 0.5; //& 1
				labels[i].priority = priority;

				labels[i].currSize = UOSInt2Double(outPtCnt);
				labels[i].totalSize = UOSInt2Double(outPtCnt);
				labels[i].nPoints = outPtCnt;
				labels[i].shapeType = 5;
				if (labels[i].points)
					MemFree(labels[i].points);
				labels[i].points = outPts;
				labels[i].flags = flags;
				return true;
			}
			else
			{
				MemFree(outPts);
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
	l.xPos = mapLabels[index].xPos;
	l.yPos = mapLabels[index].yPos;
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
	mapLabels[index].xPos = mapLabels[index2].xPos;
	mapLabels[index].yPos = mapLabels[index2].yPos;
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
	mapLabels[index2].xPos = l.xPos;
	mapLabels[index2].yPos = l.yPos;
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

Bool Map::MapConfig2TGen::LabelOverlapped(Double *points, UOSInt nPoints, Double tlx, Double tly, Double brx, Double bry)
{
	while (nPoints--)
	{
		if (points[(nPoints << 2) + 0] < brx && points[(nPoints << 2) + 2] > tlx && points[(nPoints << 2) + 1] < bry && points[(nPoints << 2) + 3] > tly) return true;
	}

	return false;
}

void Map::MapConfig2TGen::DrawLabels(Media::DrawImage *img, MapLabels2 *labels, UOSInt maxLabel, UOSInt *labelCnt, Map::MapView *view, Data::ArrayList<MapFontStyle*> **fonts, Media::DrawEngine *drawEng, Double *objBounds, UOSInt *objCnt)
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
		Double szThis[2];
		UOSInt currPt;

		Double tlx;
		Double tly;
		Double brx;
		Double bry;
		Double scnPtX;
		Double scnPtY;
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
				GetCharsSize(img, szThis, labels[i].label->ToCString(), fonts[labels[i].fontStyle], 0, 0);//labels[i].scaleW, labels[i].scaleH);
				Math::Coord2DDbl scnD = view->MapXYToScnXY(Math::Coord2DDbl(labels[i].xPos / labels[i].mapRate, labels[i].yPos / labels[i].mapRate));
				scnPtX = scnD.x;
				scnPtY = scnD.y;

				j = 1;
				if (labels[i].xOfst == 0)
				{
					tlx = scnPtX - (szThis[0] * 0.5);
					brx = tlx + szThis[0];
					tly = scnPtY - (szThis[1] * 0.5);
					bry = tly + szThis[1];

					j = LabelOverlapped(objBounds, currPt, tlx, tly, brx, bry);
				}
				if (j)
				{
					tlx = scnPtX + 1 + (labels[i].xOfst * 0.5);
					brx = tlx + szThis[0];
					tly = scnPtY - (szThis[1] * 0.5);
					bry = tly + szThis[1];

					j = LabelOverlapped(objBounds, currPt, tlx, tly, brx, bry);
				}
				if (j)
				{
					tlx = scnPtX - szThis[0] - 1 - (labels[i].xOfst * 0.5);
					brx = tlx + szThis[0];
					tly = scnPtY - (szThis[1] * 0.5);
					bry = tly + szThis[1];

					j = LabelOverlapped(objBounds, currPt, tlx, tly, brx, bry);
				}
				if (j)
				{
					tlx = scnPtX - (szThis[0] * 0.5);
					brx = tlx + szThis[0];
					tly = scnPtY - szThis[1] - 1 - (labels[i].yOfst * 0.5);
					bry = tly + szThis[1];

					j = LabelOverlapped(objBounds, currPt, tlx, tly, brx, bry);
				}
				if (j)
				{
					tlx = scnPtX - (szThis[0] * 0.5);
					brx = tlx + szThis[0];
					tly = scnPtY + 1 + (labels[i].yOfst * 0.5);
					bry = tly + szThis[1];

					j = LabelOverlapped(objBounds, currPt, tlx, tly, brx, bry);
				}

				if (!j)
				{
					DrawChars(img, labels[i].label->ToCString(), (tlx + brx) * 0.5, (tly + bry) * 0.5, 0, 0, fonts[labels[i].fontStyle], 0);

					objBounds[(currPt << 2)] = tlx;
					objBounds[(currPt << 2) + 1] = tly;
					objBounds[(currPt << 2) + 2] = brx;
					objBounds[(currPt << 2) + 3] = bry;

					log->AddString(labels[i].label->v, (tlx + brx) * 0.5, (tly + bry) * 0.5, 0, 0, labels[i].fontStyle, 0, &objBounds[(currPt << 2)]);

					currPt++;
				}
			}
			else if (labels[i].shapeType == 3)
			{
//				wprintf(L"Label: %s, nPoints: %d\n", labels[i].label, labels[i].nPoints);

				if (lastLbl)
				{
					if (!lastLbl->Equals(labels[i].label))
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
					lastLbl = labels[i].label->Clone();
				}
				else
				{
				//	MessageBoxW(NULL, L"Test", lastLbl, MB_OK);
				}

//				Int32 *points = (Int32*)MemAlloc(labels[i].nPoints << 3);
//				view->MapToScnXY(labels[i].points, points, labels[i].nPoints, 0, 0);
				Double minX;
				Double minY;
				Double maxX;
				Double maxY;
				Double xDiff;
				Double yDiff;
				Double scaleN;
				Double scaleD;
				Double lastX;
				Double lastY;
				Double thisX;
				Double thisY;
				UInt32 k;
//				Bool hasPoint;

				j = (labels[i].nPoints << 1) - 2;
				lastX = labels[i].points[j];
				lastY = labels[i].points[j + 1];
				maxX = minX = lastX;
				maxY = minY = lastY;
				while (j > 0)
				{
					j -= 2;
					thisX = labels[i].points[j];
					thisY = labels[i].points[j + 1];

					if (minX > thisX)
						minX = thisX;
					if (maxX < thisX)
						maxX = thisX;
					if (minY > thisY)
						minY = thisY;
					if (maxY < thisY)
						maxY = thisY;
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
				xDiff = maxX - minX;
				yDiff = maxY - minY;
				scaleN = 0;
				scaleD = 1;

				if (xDiff == 0 && yDiff == 0)
				{
					k = 0;
				}
				else
				{
					if (minX > maxX || minY > maxY)
					{
					//	wprintf(L"Error in handling label: %s\n", lastLbl);
					}
					if (xDiff > yDiff)
					{
						scnPtX = (maxX + minX) * 0.5;
						k = 0;
						while ((UOSInt)k < labels[i].nPoints - 1)
						{
							if (labels[i].points[k << 1] >= scnPtX && labels[i].points[(k << 1) + 2] <= scnPtX)
							{
								scaleD = labels[i].points[(k << 1) + 0] - labels[i].points[(k << 1) + 2];
								scaleN = (labels[i].points[k << 1] - scnPtX);
								break;
							}
							else if (labels[i].points[k << 1] <= scnPtX && labels[i].points[(k << 1) + 2] >= scnPtX)
							{
								scaleD = labels[i].points[(k << 1) + 2] - labels[i].points[(k << 1)];
								scaleN = scnPtX - labels[i].points[k << 1];
								break;
							}
							k++;
						}

					}
					else
					{
						scnPtY = (maxY + minY) * 0.5;
						k = 0;
						while ((UOSInt)k < labels[i].nPoints - 1)
						{
							if (labels[i].points[(k << 1) + 1] >= scnPtY && labels[i].points[(k << 1) + 3] <= scnPtY)
							{
								scaleD = labels[i].points[(k << 1) + 1] - labels[i].points[(k << 1) + 3];
								scaleN = (labels[i].points[(k << 1) + 1] - scnPtY);
								break;
							}
							else if (labels[i].points[(k << 1) + 1] <= scnPtY && labels[i].points[(k << 1) + 3] >= scnPtY)
							{
								scaleD = labels[i].points[(k << 1) + 3] - labels[i].points[(k << 1) + 1];
								scaleN = scnPtY - labels[i].points[(k << 1) + 1];
								break;
							}
							k++;
						}
					}
				}

				labels[i].scaleW = labels[i].points[(k << 1) + 2] - labels[i].points[(k << 1)];
				labels[i].scaleH = labels[i].points[(k << 1) + 3] - labels[i].points[(k << 1) + 1];
				GetCharsSize(img, szThis, labels[i].label->ToCString(), fonts[labels[i].fontStyle], labels[i].scaleW, labels[i].scaleH);
//				scnPtX = labels[i].points[(k << 1)] + MulDiv(labels[i].points[(k << 1) + 2] - labels[i].points[(k << 1)], scaleN, scaleD);
//				scnPtY = labels[i].points[(k << 1) + 1] + MulDiv(labels[i].points[(k << 1) + 3] - labels[i].points[(k << 1) + 1], scaleN, scaleD);
				Double dscnPtX;
				Double dscnPtY;
				Double dblArr[6];
				dblArr[0] = minX / labels[i].mapRate;
				dblArr[1] = minY / labels[i].mapRate;
				dblArr[2] = maxX / labels[i].mapRate;
				dblArr[3] = maxY / labels[i].mapRate;
				if (scaleD == 0)
				{
					dscnPtX = labels[i].points[(k << 1)];
					dscnPtY = labels[i].points[(k << 1) + 1];
				}
				else
				{
					dscnPtX = labels[i].points[(k << 1)] + (labels[i].points[(k << 1) + 2] - labels[i].points[(k << 1)]) / (Double)scaleD * scaleN;
					dscnPtY = labels[i].points[(k << 1) + 1] + (labels[i].points[(k << 1) + 3] - labels[i].points[(k << 1) + 1]) / (Double)scaleD * scaleN;
				}
				dblArr[4] = dscnPtX / labels[i].mapRate;
				dblArr[5] = dscnPtY / labels[i].mapRate;
				Int32 tmpArr[6];
/*				tmpArr[0] = minX;
				tmpArr[1] = minY;
				tmpArr[2] = maxX;
				tmpArr[3] = maxY;
				tmpArr[4] = scnPtX;
				tmpArr[5] = scnPtY;*/
				view->MapXYToScnXY(dblArr, tmpArr, 3, 0, 0);

				if ((tmpArr[2] - tmpArr[0]) < szThis[0] && (tmpArr[1] - tmpArr[3]) < szThis[1])
				{
					tlx = tmpArr[4] - (szThis[0] * 0.5);
					brx = tlx + szThis[0];
					tly = tmpArr[5] - (szThis[1] * 0.5);
					bry = tly + szThis[1];

					j = LabelOverlapped(objBounds, currPt, tlx, tly, brx, bry);
					if (j == 0)
					{
						DrawChars(img, labels[i].label->ToCString(), (tlx + brx) * 0.5, (tly + bry) * 0.5, labels[i].scaleW, labels[i].scaleH, fonts[labels[i].fontStyle], (labels[i].flags & SFLG_ALIGN) != 0);

						objBounds[(currPt << 2)] = tlx;
						objBounds[(currPt << 2) + 1] = tly;
						objBounds[(currPt << 2) + 2] = brx;
						objBounds[(currPt << 2) + 3] = bry;

						log->AddString(labels[i].label->v, (tlx + brx) * 0.5, (tly + bry) * 0.5, labels[i].scaleW, labels[i].scaleH, labels[i].fontStyle, (labels[i].flags & SFLG_ALIGN) != 0, &objBounds[(currPt << 2)]);

						currPt++;
					}
				}
				else
				{
					Double scnDiff = (maxX - minX) * 0.5;
					j = 1;
					while (j)
					{
						Math::Coord2DDbl tmpD = view->MapXYToScnXY(Math::Coord2DDbl(dscnPtX / labels[i].mapRate, dscnPtY / labels[i].mapRate));
						tmpArr[4] = Double2Int32(tmpD.x);
						tmpArr[5] = Double2Int32(tmpD.y);
						tlx = tmpArr[4] - (szThis[0] * 0.5);
						brx = tlx + szThis[0];
						tly = tmpArr[5] - (szThis[1] * 0.5);
						bry = tly + szThis[1];

						j = LabelOverlapped(objBounds, currPt, tlx, tly, brx, bry);
						if (j == 0)
						{
							break;
						}
						else
						{
							/////////////////////////////////////
							if (xDiff > yDiff)
							{
								if (scnDiff < 0)
								{
									dscnPtX += scnDiff - 1;
									if (dscnPtX <= minX)
									{
										scnDiff = -scnDiff;
										dscnPtX = ((minX + maxX) * 0.5) + scnDiff + 1;
									}
								}
								else
								{
									dscnPtX += scnDiff + 1;
									if (dscnPtX >= maxX)
									{
										scnDiff = scnDiff * 0.51;
										if (scnDiff < 30)
											break;
										scnDiff = -scnDiff;
										dscnPtX = ((minX + maxX) * 0.5) + scnDiff - 1;
									}
								}

								k = 0;
								while ((UOSInt)k < labels[i].nPoints - 1)
								{
									if (labels[i].points[k << 1] >= dscnPtX && labels[i].points[(k << 1) + 2] <= dscnPtX)
									{
										scaleD = labels[i].points[(k << 1) + 0] - labels[i].points[(k << 1) + 2];
										scaleN = (labels[i].points[k << 1] - dscnPtX);
										break;
									}
									else if (labels[i].points[k << 1] <= dscnPtX && labels[i].points[(k << 1) + 2] >= dscnPtX)
									{
										scaleD = labels[i].points[(k << 1) + 2] - labels[i].points[(k << 1)];
										scaleN = (dscnPtX - labels[i].points[k << 1]);
										break;
									}
									k++;
								}

							}
							else
							{
								if (scnDiff < 0)
								{
									dscnPtY += scnDiff - 1;
									if (dscnPtY <= minY)
									{
										scnDiff = -scnDiff;
										dscnPtY = ((minY + maxY) * 0.5) + scnDiff + 1;
									}
								}
								else
								{
									dscnPtY += scnDiff + 1;
									if (dscnPtY >= maxY)
									{
										scnDiff = scnDiff * 0.5;
										if (scnDiff < 30)
											break;
										scnDiff = -scnDiff;
										dscnPtY = ((minY + maxY) * 0.5) + scnDiff - 1;
									}
								}

								k = 0;
								while ((UOSInt)k < labels[i].nPoints - 1)
								{
									if (labels[i].points[(k << 1) + 1] >= dscnPtY && labels[i].points[(k << 1) + 3] <= dscnPtY)
									{
										scaleD = labels[i].points[(k << 1) + 1] - labels[i].points[(k << 1) + 3];
										scaleN = (labels[i].points[(k << 1) + 1] - dscnPtY);
										break;
									}
									else if (labels[i].points[(k << 1) + 1] <= dscnPtY && labels[i].points[(k << 1) + 3] >= dscnPtY)
									{
										scaleD = labels[i].points[(k << 1) + 3] - labels[i].points[(k << 1) + 1];
										scaleN = (dscnPtY - labels[i].points[(k << 1) + 1]);
										break;
									}
									k++;
								}
							}
							dscnPtX = labels[i].points[(k << 1)] + (labels[i].points[(k << 1) + 2] - labels[i].points[(k << 1)]) / (Double)scaleD *scaleN;
							dscnPtY = labels[i].points[(k << 1) + 1] + (labels[i].points[(k << 1) + 3] - labels[i].points[(k << 1) + 1]) / (Double)scaleD * scaleN;
							labels[i].scaleW = labels[i].points[(k << 1) + 2] - labels[i].points[(k << 1)];
							labels[i].scaleH = labels[i].points[(k << 1) + 3] - labels[i].points[(k << 1) + 1];
							GetCharsSize(img, szThis, labels[i].label->ToCString(), fonts[labels[i].fontStyle], labels[i].scaleW, labels[i].scaleH);
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
							if ((tmpV - LBLMINDIST) < bry && (tmpV + LBLMINDIST) > tly)
							{
								n++;
							}
							tmpV = thisPts[--m];
							if ((tmpV - LBLMINDIST) < brx && (tmpV + LBLMINDIST) > tlx)
							{
								n++;
							}
							n = (n != 2);
						}

						if (n)
						{
							Int32 *points;
							if ((labels[i].flags & SFLG_ALIGN) != 0)
							{
								Double realBounds[4];
								points = MemAlloc(Int32, labels[i].nPoints << 1);
								view->MapXYToScnXY(labels[i].points, points, labels[i].nPoints, 0, 0);
								DrawCharsLA(img, labels[i].label->ToCString(), labels[i].points, points, labels[i].nPoints, k, scaleN, scaleD, fonts[labels[i].fontStyle], realBounds);
								DrawCharsLA(img, labels[i].label->ToCString(), labels[i].points, points, labels[i].nPoints, k, scaleN, scaleD, fonts[labels[i].fontStyle], realBounds);
								MemFree(points);

								objBounds[(currPt << 2)] = realBounds[0];
								objBounds[(currPt << 2) + 1] = realBounds[1];
								objBounds[(currPt << 2) + 2] = realBounds[2];
								objBounds[(currPt << 2) + 3] = realBounds[3];
								currPt++;
							}
							else if ((labels[i].flags & SFLG_ROTATE) != 0)
							{
								Double realBounds[4];
								points = MemAlloc(Int32, labels[i].nPoints << 1);
								view->MapXYToScnXY(labels[i].points, points, labels[i].nPoints, 0, 0);
								DrawCharsL(img, labels[i].label->ToCString(), labels[i].points, points, labels[i].nPoints, k, scaleN, scaleD, fonts[labels[i].fontStyle], realBounds);
								log->AddStringL(labels[i].label->v, labels[i].points, points, labels[i].nPoints, k, scaleN, scaleD, labels[i].fontStyle, false, realBounds);
								MemFree(points);

								objBounds[(currPt << 2)] = realBounds[0];
								objBounds[(currPt << 2) + 1] = realBounds[1];
								objBounds[(currPt << 2) + 2] = realBounds[2];
								objBounds[(currPt << 2) + 3] = realBounds[3];
								currPt++;
							}
							else
							{
								DrawChars(img, labels[i].label->ToCString(), (tlx + brx) * 0.5, (tly + bry) * 0.5, labels[i].scaleW, labels[i].scaleH, fonts[labels[i].fontStyle], (labels[i].flags & SFLG_ALIGN) != 0);

								objBounds[(currPt << 2)] = tlx;
								objBounds[(currPt << 2) + 1] = tly;
								objBounds[(currPt << 2) + 2] = brx;
								objBounds[(currPt << 2) + 3] = bry;

								log->AddString(labels[i].label->v, (tlx + brx) * 0.5, (tly + bry) * 0.5, labels[i].scaleW, labels[i].scaleH, labels[i].fontStyle, (labels[i].flags & SFLG_ALIGN) != 0, &objBounds[(currPt << 2)]);

								currPt++;
							}

							thisPts[thisCnt++] = (tlx + brx) * 0.5;
							thisPts[thisCnt++] = (tly + bry) * 0.5;
						}
						j = (thisCnt < 10);
					}
				}
			}
			else if (labels[i].shapeType == 5)
			{
				GetCharsSize(img, szThis, labels[i].label->ToCString(), fonts[labels[i].fontStyle], 0, 0);//labels[i].scaleW, labels[i].scaleH);
				Math::Coord2DDbl scnD = view->MapXYToScnXY(Math::Coord2DDbl(labels[i].xPos / labels[i].mapRate, labels[i].yPos / labels[i].mapRate));
				scnPtX = scnD.x;
				scnPtY = scnD.y;

				j = 1;
				if (j)
				{
					tlx = scnPtX  - (szThis[0] * 0.5);
					brx = tlx + szThis[0];
					tly = scnPtY - (szThis[1] * 0.5);
					bry = tly + szThis[1];

					j = LabelOverlapped(objBounds, currPt, tlx, tly, brx, bry);
				}
				if (j)
				{
					tlx = scnPtX + 1;
					brx = tlx + szThis[0];
					tly = scnPtY - (szThis[1] * 0.5);
					bry = tly + szThis[1];

					j = LabelOverlapped(objBounds, currPt, tlx, tly, brx, bry);
				}
				if (j)
				{
					tlx = scnPtX - szThis[0] - 1;
					brx = tlx + szThis[0];
					tly = scnPtY - (szThis[1] * 0.5);
					bry = tly + szThis[1];

					j = LabelOverlapped(objBounds, currPt, tlx, tly, brx, bry);
				}
				if (j)
				{
					tlx = scnPtX - (szThis[0] * 0.5);
					brx = tlx + szThis[0];
					tly = scnPtY - szThis[1] - 1;
					bry = tly + szThis[1];

					j = LabelOverlapped(objBounds, currPt, tlx, tly, brx, bry);
				}
				if (j)
				{
					tlx = scnPtX - (szThis[0] * 0.5);
					brx = tlx + szThis[0];
					tly = scnPtY + 1;
					bry = tly + szThis[1];

					j = LabelOverlapped(objBounds, currPt, tlx, tly, brx, bry);
				}

				if (!j)
				{
					DrawChars(img, labels[i].label->ToCString(), (tlx + brx) * 0.5, (tly + bry) * 0.5, 0, 0, fonts[labels[i].fontStyle], 0);

					objBounds[(currPt << 2)] = tlx;
					objBounds[(currPt << 2) + 1] = tly;
					objBounds[(currPt << 2) + 2] = brx;
					objBounds[(currPt << 2) + 3] = bry;

					log->AddString(labels[i].label->v, (tlx + brx) * 0.5, (tly + bry) * 0.5, 0, 0, labels[i].fontStyle, 0, &objBounds[(currPt << 2)]);

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
			MemFree(labels[i].points);
	}
	if (lastLbl)
		lastLbl->Release();
}

void Map::MapConfig2TGen::LoadLabels(Media::DrawImage *img, Map::MapConfig2TGen::MapLabels2 *labels, UOSInt maxLabel, UOSInt *labelCnt, Map::MapView *view, Data::ArrayList<MapFontStyle*> **fonts, Media::DrawEngine *drawEng, Double *objBounds, UOSInt *objCnt, Text::CString fileName, Int32 xId, Int32 yId, Double xOfst, Double yOfst, IO::Stream *dbStream)
{
	IO::FileStream *fs = 0;
	IO::StreamReader *reader;
	UTF8Char c;
	UTF8Char sbuff[512];
	Text::PString strs[15];
	UTF8Char *sptr;
	UTF8Char *sptr2;
	UOSInt i;
	if (dbStream)
	{
		NEW_CLASS(reader, IO::StreamReader(dbStream));
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

		NEW_CLASS(fs, IO::FileStream(CSTRP(sbuff, sptr), IO::FileMode::ReadOnly, IO::FileShare::DenyAll, IO::FileStream::BufferType::Normal));
		if (fs->IsError())
		{
			DEL_CLASS(fs);
			return;
		}
		NEW_CLASS(reader, IO::StreamReader(fs));
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
			Text::String *label;
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
			Double *ptArr;
			Double *currPt;
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
				ptArr = MemAlloc(Double, nPoints << 1);
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

					currPt[0] = Text::StrToDouble(strs[0].v);
					currPt[1] = Text::StrToDouble(strs[1].v);
					currPt += 2;
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
						Int32 *scnPts;
						Double realBounds[4];
						scnPts = MemAlloc(Int32, nPoints << 1);
						view->MapXYToScnXY(ptArr, scnPts, nPoints, 0, 0);
						if (isAlign)
						{
							DrawCharsLA(img, label->ToCString(), ptArr, scnPts, nPoints, ptCurr, scaleN, scaleD, fonts[fontStyle], realBounds);
						}
						else
						{
							DrawCharsL(img, label->ToCString(), ptArr, scnPts, nPoints, ptCurr, scaleN, scaleD, fonts[fontStyle], realBounds);
						}
						MemFree(scnPts);
					}
				}

				MemFree(ptArr);
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

Map::MapConfig2TGen::MapConfig2TGen(Text::CString fileName, Media::DrawEngine *eng, Data::ArrayList<Map::IMapDrawLayer*> *layerList, Parser::ParserList *parserList, const UTF8Char *forceBase, IO::Writer *errWriter, Int32 maxScale, Int32 minScale)
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
		IO::StreamReader rdr(&fstm);
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
					IO::StmData::FileData *fd;
					IO::ParserType pt;
					sptr = fileName.ConcatTo(sbuff);
					sptr = IO::Path::AppendPath(sbuff, sptr, strs[4].ToCString());
					NEW_CLASS(fd, IO::StmData::FileData(CSTRP(sbuff, sptr), false));
					IO::ParsedObject *obj = parserList->ParseFile(fd, &pt);
					DEL_CLASS(fd);
					if (obj)
					{
						if (obj->GetParserType() == IO::ParserType::ImageList)
						{
							Media::ImageList *imgList = (Media::ImageList*)obj;
							if (imgList->GetCount() > 0)
							{
								imgList->ToStaticImage(0);
								Media::StaticImage *img = (Media::StaticImage*)imgList->GetImage(0, 0);
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
					if (currLayer->img == 0)
					{
						currLayer->img = this->drawEng->LoadImage(strs[4].ToCString());
					}
					if (currLayer->img == 0)
					{
						MemFree(currLayer);
					}
					else
					{
						layerNameEnd = strs[1].ConcatTo(baseDir);
						currLayer->lyr = GetDrawLayer(CSTRP(layerName, layerNameEnd), layerList, errWriter);
						if (currLayer->lyr == 0)
						{
							this->drawEng->DeleteImage(currLayer->img);
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
					if (currFont->fontName)
					{
						MemFree(currFont->fontName);
					}
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
			if (currLyr->img && currLyr->drawType == 10)
			{
				this->drawEng->DeleteImage(currLyr->img);
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

Media::DrawPen *Map::MapConfig2TGen::CreatePen(Media::DrawImage *img, UInt32 lineStyle, UOSInt lineLayer)
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

WChar *Map::MapConfig2TGen::DrawMap(Media::DrawImage *img, Map::MapView *view, Bool *isLayerEmpty, Map::MapScheduler *mapSch, Media::IImgResizer *resizer, Text::CString dbOutput, DrawParam *params)
{
//	Manage::HiResClock clk;
	UInt32 index;
	UInt32 index2;
	UOSInt layerCnt = this->drawList->GetCount();
	Data::ArrayList<MapFontStyle*> **myArrs;
	Data::ArrayList<MapFontStyle*> *fontArr;
	Map::IMapDrawLayer *lyr;
	Map::MapLayerStyle *lyrs;
	Map::DrawObjectL *dobj;
	Map::MapFontStyle *fnt;
	Map::MapFontStyle *fnt2;
	Media::DrawBrush *brush;
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
	Double *objBounds = MemAlloc(Double, this->nStr * 8);
	UOSInt objCnt = 0;

	thisScale = view->GetMapScale();
#ifndef NOSCH
	mapSch->SetMapView(view, img);
#endif

	brush = img->NewBrushARGB(this->bgColor);
	img->DrawRect(0, 0, UOSInt2Double(img->GetWidth()), UOSInt2Double(img->GetHeight()), 0, brush);
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
					fnt2->other = img->NewBrushARGB(fnt->color);
				}
				else if (fnt->fontType == 1)
				{
					fnt2->other = img->NewBrushARGB(fnt->color);
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
					fnt2->other = img->NewBrushARGB(fnt->color);
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
		void *session;
		lyrs = (Map::MapLayerStyle*)this->drawList->GetItem(index++);
		if (thisScale > lyrs->minScale && thisScale <= lyrs->maxScale)
		{
			if (lyrs->drawType == 7 && params->labelType != 0)
			{
				lyr = lyrs->lyr;
				if (lyr)
				{
					arr.Clear();
					lyr->GetObjectIdsMapXY(&arr, 0, view->GetLeftX(), view->GetTopY(), view->GetRightX(), view->GetBottomY(), true);

					if ((i = arr.GetCount()) > 0)
					{
#ifdef NOSCH
						Data::ArrayList *drawArr;
						NEW_CLASS(drawArr, Data::ArrayList());
#else
						mapSch->SetDrawType(lyr, Map::MapScheduler::MSDT_POLYGON, pen = CreatePen(img, lyrs->style, 0), brush = img->NewBrushARGB(lyrs->bkColor), 0, 0, 0, isLayerEmpty);
#endif

						session = lyr->BeginGetObject();
						lastId = -1;
						while (i-- > 0)
						{
							thisId = arr.GetItem(i);
							if (thisId != lastId)
							{
								lastId = thisId;
								if ((dobj = lyr->GetNewObjectById(session, thisId)) != 0)
								{
#ifndef NOSCH
									mapSch->Draw(dobj);
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
				lyr = lyrs->lyr;
				if (lyr)
				{
					arr.Clear();
					lyr->GetObjectIdsMapXY(&arr, 0, view->GetLeftX(), view->GetTopY(), view->GetRightX(), view->GetBottomY(), true);

					if ((i = arr.GetCount()) > 0)
					{
#ifdef NOSCH
						Data::ArrayList *drawArr;
						NEW_CLASS(drawArr, Data::ArrayList());
#else
						mapSch->SetDrawType(lyr, Map::MapScheduler::MSDT_POLYLINE, pen = CreatePen(img, lyrs->style, 0), 0, 0, 0, 0, isLayerEmpty);
#endif

						session = lyr->BeginGetObject();
						lastId = -1;
						while (i-- > 0)
						{
							thisId = arr.GetItem(i);
							if (thisId != lastId)
							{
								lastId = thisId;
								if ((dobj = lyr->GetNewObjectById(session, thisId)) != 0)
								{
#ifndef NOSCH
									mapSch->Draw(dobj);
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
					img->DelBrush((Media::DrawBrush*)fnt->other);
				}
				else if (fnt->fontType == 1)
				{
					img->DelBrush((Media::DrawBrush*)fnt->other);
				}
				else if (fnt->fontType == 2)
				{
					img->DelPen((Media::DrawPen*)fnt->other);
				}
				else if (fnt->fontType == 4)
				{
					img->DelFont(fnt->font);
					img->DelBrush((Media::DrawBrush*)fnt->other);
				}
				MemFree(fnt);

				j++;
			}
			DEL_CLASS(fontArr);
		}
	}
	MemFree(myArrs);
	MemFree(labels);
	MemFree(objBounds);

//	Double t = clk.GetTimeDiff();
//	printf("Time used: %d\n", (Int32)(t * 1000));
	return 0;
}

UInt32 Map::MapConfig2TGen::GetBGColor()
{
	return this->bgColor;
}

void Map::MapConfig2TGen::ReleaseLayers(Data::ArrayList<Map::IMapDrawLayer*> *layerList)
{
	Map::IMapDrawLayer *lyr;
	UOSInt i;
	i = layerList->GetCount();
	while (i-- > 0)
	{
		lyr = layerList->GetItem(i);
		DEL_CLASS(lyr);
	}
}
