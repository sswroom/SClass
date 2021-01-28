#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayList.h"
#include "Data/ArrayListInt.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"
#include "IO/Stream.h"
#include "IO/FileStream.h"
#include "IO/StreamReader.h"
#include "IO/IStreamData.h"
#include "IO/StmData/FileData.h"
#include "IO/FileParser.h"
#include "IO/Path.h"
#include "IO/Console.h"
#include "Media/DrawEngine.h"
#include "Media/GDIEngine.h"
#include "Media/FrameInfo.h"
#include "Media/IImgResizer.h"
#include "Media/StaticImage.h"
#include "Media/ImageList.h"
#include "Map/IMapDrawLayer.h"
#include "Map/MapView.h"
#include "Map/MapConfig.h"
#include "Map/CIPLayer.h"
#include <math.h>
#include <windows.h>
#include <stdio.h>
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

void Map::MapConfig::DrawChars(Media::DrawImage *img, const WChar *str1, Int32 scnPosX, Int32 scnPosY, Int32 scaleW, Int32 scaleH, Data::ArrayList<MapFontStyle*> *fontStyle, Bool isAlign)
{
	WChar sbuff[256];
	Text::StrConcat(sbuff, str1);
	Int32 size[2];
	UInt16 absH;
	OSInt fntCount;
	Int32 i;
	Int32 maxSize;
	Int32 maxIndex;
	OSInt lblSize;
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
			if (maxSize < font->fontSize)
			{
				maxSize = font->fontSize;
				maxIndex = i;
			}
		}
		i++;
	}

	if (maxSize == 0)
	{
		return;
	}

	lblSize = Text::StrCharCnt(str1);
	font = (Map::MapFontStyle*)fontStyle->GetItem(maxIndex);
	if (scaleH == 0)
	{
		scaleH = 0;
	}
	img->GetTextSize(font->font, str1, lblSize, size);

	if (scaleH == 0) //scaleW == 0 && scaleH == 0)
	{
		Int32 rcLeft;
		Int32 rcRight;
		Int32 rcTop;
		Int32 rcBottom;
		img->SetTextAlign(Media::DrawEngine::DRAW_POS_TOPLEFT);

		i = 0;
		while (i < fntCount)
		{
			font = (Map::MapFontStyle*)fontStyle->GetItem(i);
			if (font->fontType == 1)
			{
				Media::DrawPen *p = 0;
				Media::DrawBrush *b = 0;
				rcLeft = scnPosX - ((size[0] + font->fontSize) >> 1);
				rcRight = rcLeft + size[0] + font->fontSize;
				rcTop = scnPosY - ((size[1] + font->fontSize) >> 1);
				rcBottom = rcTop + size[1] + font->fontSize;

				b = (Media::DrawBrush *)font->other;
				if (i < fntCount - 1)
				{
					font2 = (Map::MapFontStyle*)fontStyle->GetItem(i + 1);
					if (font2->fontType == 2 && font2->fontSize == font->fontSize)
					{
						i++;
						p = (Media::DrawPen*)font2->other;
						rcRight += font2->thick >> 1;
						rcBottom += font2->thick >> 1;
					}
				}

				img->DrawRect(rcLeft, rcTop, rcRight - rcLeft, rcBottom - rcTop, p, b);
			}
			else if (font->fontType == 2)
			{
				Int32 pt[10];
//				Media::DrawPen *p = 0;
				rcLeft = scnPosX - ((size[0] + font->fontSize) >> 1);
				rcRight = rcLeft + size[0] + font->fontSize;
				rcTop = scnPosY - ((size[1] + font->fontSize) >> 1);
				rcBottom = rcTop + size[1] + font->fontSize;

				pt[0] = rcLeft;
				pt[1] = rcTop;
				pt[2] = rcRight;
				pt[3] = rcTop;
				pt[4] = rcRight;
				pt[5] = rcBottom;
				pt[6] = rcLeft;
				pt[7] = rcBottom;
				pt[8] = pt[0];
				pt[9] = pt[1];

				img->DrawPolyline(pt, 5, (Media::DrawPen*)font->other);
			}
			else if (font->fontType == 0)
			{
				img->DrawStringW(scnPosX - (size[0]  >> 1), scnPosY - (size[1] >> 1), str1, font->font, (Media::DrawBrush*)font->other);
			}
			else if (font->fontType == 4)
			{
//				SelectObject(map->MapHDC, fonts[i].font);
//				SetTextColor(map->MapHDC, fonts[i].color);
//				SetBkMode(map->MapHDC, TRANSPARENT);
				Int32 j = font->thick;
				Int32 k;
				Int32 px = scnPosX - (size[0] >> 1);
				Int32 py = scnPosY - (size[1] >> 1);
				while (j >= 0)
				{
					k = font->thick - j;
					while (k >= 0)
					{
						if (j | k)
						{
							img->DrawStringW(px + j, py + k, str1, font->font, (Media::DrawBrush*)font->other);
							img->DrawStringW(px + j, py - k, str1, font->font, (Media::DrawBrush*)font->other);
							img->DrawStringW(px - j, py + k, str1, font->font, (Media::DrawBrush*)font->other);
							img->DrawStringW(px - j, py - k, str1, font->font, (Media::DrawBrush*)font->other);
						}
						else
						{
							img->DrawStringW(px, py, str1, font->font, (Media::DrawBrush*)font->other);
						}
						k--;
					}
					j--;
				}
			}
			i++;
		}

		return;
	}


	if (scaleH < 0)
		absH = (UInt16)-scaleH;
	else
		absH = (UInt16)scaleH;

	double degD = atan(scaleH / (double)scaleW);

	long deg = (long) (atan(scaleH / (double)scaleW) * 1800 / PI);
	while (deg < 0)
		deg += 3600;

	Int32 lastScaleW = scaleW;
	Int32 lastScaleH = scaleH;

	i = 0;
	while (i < fntCount)
	{
		Media::DrawBrush *b;
		Media::DrawPen *p;

		scaleW = lastScaleW;
		scaleH = lastScaleH;
		Int32 pt[10];
		Int32 xPos;
		Int32 yPos;
		double sVal;
		double cVal;

		font = (Map::MapFontStyle *)fontStyle->GetItem(i);

		if (font->fontType == 1)
		{
			xPos = size[0] + font->fontSize ;
			yPos = size[1] + font->fontSize;
			Int32 xs = (Int32) ((xPos >> 1) * (sVal = sin(degD)));
			Int32 ys = (Int32) ((yPos >> 1) * sVal);
			Int32 xc = (Int32) ((xPos >> 1) * (cVal = cos(degD)));
			Int32 yc = (Int32) ((yPos >> 1) * cVal);

			pt[0] = scnPosX - xc - ys;
			pt[1] = scnPosY + xs - yc;
			pt[2] = scnPosX + xc - ys;
			pt[3] = scnPosY - xs - yc;
			pt[4] = scnPosX + xc + ys;
			pt[5] = scnPosY - xs + yc;
			pt[6] = scnPosX - xc + ys;
			pt[7] = scnPosY + xs + yc;


			p = 0;
			b = (Media::DrawBrush*)font->other;

			if (i < fntCount - 1)
			{
				font2 = (Map::MapFontStyle *)fontStyle->GetItem(i + 1);
				if (font2->fontType == 2 && font2->fontSize == font->fontSize)
				{
					i++;
					p = (Media::DrawPen*)font2->other;
				}
			}
			img->DrawPolygon(pt, 4, p, b);
		}
		else if (font->fontType == 2)
		{
			xPos = size[0] + font->fontSize;
			yPos = size[1] + font->fontSize;
			Int32 xs = (Int32) ((xPos >> 1) * (sVal = sin(degD)));
			Int32 ys = (Int32) ((yPos >> 1) * sVal);
			Int32 xc = (Int32) ((xPos >> 1) * (cVal = cos(degD)));
			Int32 yc = (Int32) ((yPos >> 1) * cVal);

			pt[0] = scnPosX - xc - ys;
			pt[1] = scnPosY + xs - yc;
			pt[2] = scnPosX + xc - ys;
			pt[3] = scnPosY - xs - yc;
			pt[4] = scnPosX + xc + ys;
			pt[5] = scnPosY - xs + yc;
			pt[6] = scnPosX - xc + ys;
			pt[7] = scnPosY + xs + yc;
			pt[8] = pt[0];
			pt[9] = pt[1];

			img->DrawPolyline(pt, 5, (Media::DrawPen*)font->other);
		}
		else if (font->fontType == 0 || font->fontType == 4)
		{
			if (isAlign)
			{
				img->SetTextAlign(Media::DrawEngine::DRAW_POS_TOPLEFT);
				Int32 currX = 0;
				Int32 currY = 0;
				Int32 startX;
				Int32 startY;
				Int32 tmp;
				Int32 type;
				Int32 szThis[2];
				img->GetTextSize(font->font, str1, lblSize, szThis);

				if ((szThis[0] * absH) < (szThis[1] * lblSize * scaleW))
				{
					scaleW = -scaleW;
					startX = scnPosX - (tmp = (szThis[0] >> 1));
					if (scaleW)
						startY = scnPosY - (szThis[1] >> 1) - (Int16)((Int32)tmp * (Int32)scaleH / (Int32)scaleW);
					else
						startY = scnPosY - (szThis[1] >> 1);
					type = 0;
				}
				else
				{
					scaleW = -scaleW;
					if (scaleH > 0)
					{
						startY = scnPosY - (tmp = (Int32)((szThis[1] * lblSize) >> 1));
						startX = scnPosX - (Int16)((Int32)tmp * (Int32)scaleW / (Int32)scaleH);
					}
					else if (scaleH)
					{
						scaleW = -scaleW;
						scaleH = -scaleH;
						startY = scnPosY - (tmp = (Int32)((szThis[1] * lblSize) >> 1));
						startX = scnPosX - (Int16)((Int32)tmp * (Int32)scaleW / (Int32)scaleH);
					}
					else
					{
						startY = scnPosY - (tmp = (Int32)((szThis[1] * lblSize) >> 1));
						startX = scnPosX;
					}
					type = 1;
				}

				Int32 cHeight;
				if (scaleH < 0)
					cHeight = szThis[1];
				else
					cHeight = -szThis[1];

				currX = 0;
				currY = 0;

				OSInt cnt;
				WChar *lbl = sbuff;
				cnt = lblSize;

				while (cnt--)
				{
					img->GetTextSize(font->font, lbl, 1, szThis);

					if (type)
					{
						if (font->fontType == 0)
						{
							WChar l[2];
							l[0] = lbl[0];
							l[1] = 0;
							img->DrawStringW(startX + currX - (szThis[0] >> 1), startY + currY, l, font->font, (Media::DrawBrush*)font->other);
						}
						else
						{
							WChar l[2];
							Int32 j = font->thick;
							Int32 k;
							Int32 px = startX + currX - (szThis[0] >> 1);
							Int32 py = startY + currY;
							l[0] = lbl[0];
							l[1] = 0;
							while (j >= 0)
							{
								k = font->thick - j;
								while (k >= 0)
								{
									if (j | k)
									{
										img->DrawStringW(px + j, py + k, l, font->font, (Media::DrawBrush*)font->other);
										img->DrawStringW(px + j, py - k, l, font->font, (Media::DrawBrush*)font->other);
										img->DrawStringW(px - j, py + k, l, font->font, (Media::DrawBrush*)font->other);
										img->DrawStringW(px - j, py - k, l, font->font, (Media::DrawBrush*)font->other);
									}
									else
									{
										img->DrawStringW(px, py, l, font->font, (Media::DrawBrush*)font->other);
									}
									k--;
								}
								j--;
							}
						}

						currY += szThis[1];

						if (scaleH)
							currX = (Int16)((Int32)currY * (Int32)scaleW / (Int32)scaleH);
					}
					else
					{
						if (font->fontType == 0)
						{
							WChar l[2];
							l[0] = lbl[0];
							l[1] = 0;
							img->DrawStringW(startX + currX, startY + currY, l, font->font, (Media::DrawBrush*)font->other);
						}
						else
						{
							WChar l[2];
							Int32 j = font->thick;
							Int32 k;
							Int32 px = startX + currX;
							Int32 py = startY + currY;
							l[0] = lbl[0];
							l[1] = 0;
							while (j >= 0)
							{
								k = font->thick - j;
								while (k >= 0)
								{
									if (j | k)
									{
										img->DrawStringW(px + j, py + k, l, font->font, (Media::DrawBrush*)font->other);
										img->DrawStringW(px + j, py - k, l, font->font, (Media::DrawBrush*)font->other);
										img->DrawStringW(px - j, py + k, l, font->font, (Media::DrawBrush*)font->other);
										img->DrawStringW(px - j, py - k, l, font->font, (Media::DrawBrush*)font->other);
									}
									else
									{
										img->DrawStringW(px, py, l, font->font, (Media::DrawBrush*)font->other);
									}
									k--;
								}
								j--;
							}
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
			//	img->DrawStringRotW(scnPosX, scnPosY, str1, font->font, (Media::DrawBrush*)font->other, (Int32)(degD * 180 / PI));

				if (font->fontType == 0)
				{
					img->DrawStringRotW(scnPosX, scnPosY, str1, font->font, (Media::DrawBrush*)font->other, (Int32)(degD * 180 / PI));
				}
				else if (font->fontType == 4)
				{
					Int32 j = font->thick;
					Int32 k;
					Int32 px = scnPosX;
					Int32 py = scnPosY;
					while (j >= 0)
					{
						k = font->thick - j;
						while (k >= 0)
						{
							if (j | k)
							{
								img->DrawStringRotW(px + j, py + k, str1, font->font, (Media::DrawBrush*)font->other, (Int32)(degD * 180 / PI));
								img->DrawStringRotW(px + j, py - k, str1, font->font, (Media::DrawBrush*)font->other, (Int32)(degD * 180 / PI));
								img->DrawStringRotW(px - j, py + k, str1, font->font, (Media::DrawBrush*)font->other, (Int32)(degD * 180 / PI));
								img->DrawStringRotW(px - j, py - k, str1, font->font, (Media::DrawBrush*)font->other, (Int32)(degD * 180 / PI));
							}
							else
							{
								img->DrawStringRotW(px, py, str1, font->font, (Media::DrawBrush*)font->other, (Int32)(degD * 180 / PI));
							}
							k--;
						}
						j--;
					}
				}
			}
		}
		i++;
	}
}

void Map::MapConfig::GetCharsSize(Media::DrawImage *img, Int32 *size, const WChar *label, Data::ArrayList<MapFontStyle*> *fontStyle, Int32 scaleW, Int32 scaleH)
{
	Int32 xSizeAdd = 0;
	Int32 ySizeAdd = 0;

	Map::MapFontStyle *font;
	OSInt fntCount;

	Int32 i;
	Int32 maxSize;
	Int32 maxIndex;

	fntCount = fontStyle->GetCount();;

	maxSize = 0;
	maxIndex = 0;
	i = 0;
	while (i < fntCount)
	{
		font = (Map::MapFontStyle*)fontStyle->GetItem(i);
		if (font->fontType == 0)
		{
			if (maxSize < font->fontSize)
			{
				maxSize = font->fontSize;
				maxIndex = i;
			}
		}
		else if (font->fontType == 4)
		{
			if (maxSize < font->fontSize + (font->thick << 1))
			{
				maxSize = font->fontSize + (font->thick << 1);
				maxIndex = i;
			}
		}
		else if (font->fontType == 1 || font->fontType == 2)
		{
			if (xSizeAdd < (font->fontSize + font->thick))
			{
				xSizeAdd = (font->fontSize + font->thick);
			}
			if (ySizeAdd < (font->fontSize + font->thick))
			{
				ySizeAdd = (font->fontSize + font->thick);
			}
		}
		i++;
	}

	if (maxSize == 0)
	{
		return;
	}

	Int32 szTmp[2];
	font = (Map::MapFontStyle*)fontStyle->GetItem(maxIndex);
	img->GetTextSize(font->font, label, Text::StrCharCnt(label), szTmp);

	if (scaleH == 0)
	{
		size[0] = szTmp[0] + xSizeAdd;
		size[1] = szTmp[1] + ySizeAdd;

		return;
	}


	Int32 pt[8];

	if (scaleW < 0)
	{
		scaleW = -scaleW;
		scaleH = -scaleH;
	}
	double degD = atan(scaleH / (double)scaleW);
	Int32 xPos = szTmp[0] + xSizeAdd;
	Int32 yPos = szTmp[1] + ySizeAdd;
	double sVal;
	double cVal;
	Int32 xs = (Int32) ((xPos >> 1) * (sVal = sin(degD)));
	Int32 ys = (Int32) ((yPos >> 1) * sVal);
	Int32 xc = (Int32) ((xPos >> 1) * (cVal = cos(degD)));
	Int32 yc = (Int32) ((yPos >> 1) * cVal);

	pt[0] = -xc - ys;
	pt[1] = xs - yc;
	pt[2] = xc - ys;
	pt[3] = -xs - yc;
	pt[4] = xc + ys;
	pt[5] = -xs + yc;
	pt[6] = -xc + ys;
	pt[7] = xs + yc;

	Int32 maxX = pt[0];
	Int32 minX = pt[0];
	Int32 maxY = pt[1];
	Int32 minY = pt[1];
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

Int32 Map::MapConfig::ToColor(WChar *str)
{
	Int32 v = Text::StrHex2Int(str);
	return 0xff000000 | ((v & 0xff) << 16) | (v & 0xff00) | ((v >> 16) & 0xff);
}

Map::IMapDrawLayer *Map::MapConfig::GetDrawLayer(WChar *name, Data::ArrayList<Map::IMapDrawLayer*> *layerList)
{
	Map::CIPLayer *cip;
	OSInt i = layerList->GetCount();
	while (i-- > 0)
	{
		cip = (Map::CIPLayer*)layerList->GetItem(i);
		if (IO::Path::FileNameCompare(name, cip->GetName()) == 0)
		{
			return cip;
		}
	}
	NEW_CLASS(cip, Map::CIPLayer(name));
	if (cip->IsError())
	{
		DEL_CLASS(cip);
		return 0;
	}
	layerList->Add(cip);
	return cip;
}

void Map::MapConfig::DrawPoints(Media::DrawImage *img, MapLayerStyle *lyrs, Map::MapView *view, Bool *isLayerEmpty, Map::MapScheduler *sch)
{
	Data::ArrayListInt *arri;
	Map::DrawObject *dobj;
	Int32 imgW;
	Int32 imgH;
	OSInt i;
#ifdef NOSCH
	OSInt j;
	Int32 pts[2];
#endif
	void *session;

#ifndef NOSCH
	sch->SetDrawType(lyrs->lyr, Map::MapScheduler::MSDT_POINTS, 0, 0, lyrs->img, isLayerEmpty);
#endif
	NEW_CLASS(arri, Data::ArrayListInt());
	lyrs->lyr->GetObjectIds(arri, 0, view->GetLeft(), view->GetTop(), view->GetRight(), view->GetBottom(), true);
	session = lyrs->lyr->BeginGetObject();
	imgW = lyrs->img->GetWidth() >> 1;
	imgH = lyrs->img->GetHeight() >> 1;

	i = arri->GetCount();
	while (i-- > 0)
	{
		dobj = lyrs->lyr->GetObjectById(session, arri->GetItem(i));
#ifdef NOSCH
		j = dobj->nPoints;
		while (j-- > 0)
		{
			if (view->MapToScnXY(&dobj->points[j << 1], pts, 1, 0, 0))
				*isLayerEmpty = false;
			img->DrawImagePt(lyrs->img, pts[0] - imgW, pts[1] - imgH);
		}
		lyrs->lyr->ReleaseObject(session, dobj);
#else
		sch->Draw(dobj);
#endif
	}
	lyrs->lyr->EndGetObject(session);
#ifndef NOSCH
	sch->WaitForFinish();
#endif
	DEL_CLASS(arri);
}

void Map::MapConfig::DrawString(Media::DrawImage *img, MapLayerStyle *lyrs, Map::MapView *view, Data::ArrayList<MapFontStyle*> **fonts, MapLabels *labels, Int32 maxLabels, Int32 *labelCnt, Bool *isLayerEmpty)
{
	void *arr;
	Data::ArrayListInt *arri;
	OSInt i;
	Int32 j;
	Map::DrawObject *dobj;
	Int32 scaleW;
	Int32 scaleH;
	Int32 pts[2];
	WChar *sptr;
	WChar lblStr[128];
	void *session;

//	NEW_CLASS(arr, Data::ArrayList());
	NEW_CLASS(arri, Data::ArrayListInt());
	lyrs->lyr->GetObjectIds(arri, &arr, view->GetLeft(), view->GetTop(), view->GetRight(), view->GetBottom(), false);
	session = lyrs->lyr->BeginGetObject();
	i = arri->GetCount();
	while (i-- > 0)
	{
		dobj = lyrs->lyr->GetObjectById(session, arri->GetItem(i));

		if (lyrs->bkColor & SFLG_SMART)
		{
			UInt32 k;
			Int32 maxSize;
			Int32 maxPos;
			maxSize = dobj->nPoints - (maxPos = dobj->parts[dobj->nParts - 1]);
			k = dobj->nParts;
			while (k-- > 1)
			{
				if ((dobj->parts[k] - dobj->parts[k - 1]) > maxSize)
					maxSize = (dobj->parts[k] - (maxPos = dobj->parts[k - 1]));
			}
			lyrs->lyr->GetString(sptr = lblStr, arr, i, 0);
//			sptr = (WChar*)arr->GetItem(i);
			if (AddLabel(labels, maxLabels, labelCnt, sptr, maxSize, &dobj->points[maxPos << 1], lyrs->priority, lyrs->lyr->GetLayerType(), lyrs->style, lyrs->bkColor, view))
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
			lyrs->lyr->GetString(sptr = lblStr, arr, i, 0);
//			sptr = (WChar*)arr->GetItem(i);
			if (dobj->nPoints & 1)
			{
				pts[0] = dobj->points[dobj->nPoints - 1];
				pts[1] = dobj->points[dobj->nPoints];

				scaleW = dobj->points[dobj->nPoints + 1] - dobj->points[dobj->nPoints - 3];
				scaleH = dobj->points[dobj->nPoints + 2] - dobj->points[dobj->nPoints - 2];
			}
			else
			{
				pts[0] = (dobj->points[dobj->nPoints - 2] + dobj->points[dobj->nPoints]) >> 1;
				pts[1] = (dobj->points[dobj->nPoints - 1] + dobj->points[dobj->nPoints + 1]) >> 1;

				scaleW = dobj->points[dobj->nPoints] - dobj->points[dobj->nPoints - 2];
				scaleH = dobj->points[dobj->nPoints + 1] - dobj->points[dobj->nPoints - 1];
			}

			if (view->InView(pts[0], pts[1]))
			{
				view->MapToScnXY(pts, pts, 1, 0, 0);

				if ((lyrs->bkColor & SFLG_ROTATE) == 0)
					scaleW = scaleH = 0;
				DrawChars(img, sptr, pts[0], pts[1], scaleW, scaleH, fonts[lyrs->style], (lyrs->bkColor & SFLG_ALIGN) != 0);
			}
			lyrs->lyr->ReleaseObject(session, dobj);
		}
		else
		{
			Int64 lastPtX = 0;
			Int64 lastPtY = 0;
			Int32 *pointPos = dobj->points;
			lyrs->lyr->GetString(sptr = lblStr, arr, i, 0);
//			sptr = (WChar*)arr->GetItem(i);

			j = dobj->nPoints;
			while (j--)
			{
				lastPtX += *pointPos++;
				lastPtY += *pointPos++;
			}

			pts[0] = (Int32)(lastPtX / dobj->nPoints);
			pts[1] = (Int32)(lastPtY / dobj->nPoints);
			if (view->InView(pts[0], pts[1]))
			{
				view->MapToScnXY(pts, pts, 1, 0, 0);
				DrawChars(img, sptr, pts[0], pts[1], 0, 0, fonts[lyrs->style], (lyrs->bkColor & SFLG_ALIGN) != 0);
			}
			lyrs->lyr->ReleaseObject(session, dobj);
		}

	}
	lyrs->lyr->EndGetObject(session);
	lyrs->lyr->ReleaseNameArr(arr);
	DEL_CLASS(arri);
}

Int32 Map::MapConfig::NewLabel(MapLabels *labels, Int32 maxLabel, Int32 *labelCnt, Int32 priority)
{
	Int32 minPriority;
	Int32 i;
	Int32 j;
	Int32 k;

	if (*labelCnt >= maxLabel)
	{
		i = *labelCnt;
		minPriority = labels[0].priority;
		j = -1;
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
		if (j < 0)
			return j;
		if (labels[j].label)
			Text::StrDelNew(labels[j].label);
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


Bool Map::MapConfig::AddLabel(MapLabels *labels, Int32 maxLabel, Int32 *labelCnt, WChar *labelt, Int32 nPoint, Int32 *points, Int32 priority, Int32 recType, Int32 fontStyle, Int32 flags, Map::MapView *view)
{
	Int32 size;
	Int32 visibleSize;

	Int32 i;
	Int32 j;

	Int32 *pInt;

	Int32 scnX;
	Int32 scnY;
	Int32 found;

	Int32 left = view->GetLeft();
	Int32 right = view->GetRight();
	Int32 top = view->GetTop();
	Int32 bottom = view->GetBottom();
	Int32 mapPosX = view->GetCentX();
	Int32 mapPosY = view->GetCentY();

	if (recType == 1) //Point
	{
		found = 0;
		i = 0;
		while (i < *labelCnt)
		{
			if (recType == labels[i].shapeType)
			{
				if (Text::StrCompare(labels[i].label, labelt) == 0)
				{
					found = 1;

					pInt = points;
					j = nPoint;
					while (j--)
					{
						scnX = mapPosX - *pInt++;
						scnY = mapPosY - *pInt++;
						scnX = scnX * scnX + scnY * scnY;
						if (scnX < labels[i].currSize)
						{
							labels[i].xPos = pInt[-2];
							labels[i].yPos = pInt[-1];
							labels[i].fontStyle = fontStyle;
							labels[i].scaleW = 0;
							labels[i].scaleH = 0;
							labels[i].currSize = scnX;
							if (priority > labels[i].priority)
								labels[i].priority = priority;
						}
						labels[i].totalSize++;
					}
					return true;
				}
			}
			i++;
		}

		if (found == 0)
		{
			i = -1;
			pInt = points;
			j = nPoint;
			while (j--)
			{
				if (pInt[0] >= left && pInt[0] < right && pInt[1] >= top && pInt[1] < bottom)
				{
					found = 1;

					i = NewLabel(labels, maxLabel, labelCnt, priority);
					if (i < 0)
					{
						return false;
					}

					labels[i].label = Text::StrCopyNew(labelt);
					labels[i].xPos = pInt[0];
					labels[i].yPos = pInt[1];
					labels[i].fontStyle = fontStyle;
					labels[i].scaleW = 0;
					labels[i].scaleH = 0;
					labels[i].priority = priority;

					scnX = mapPosX - pInt[0];
					scnY = mapPosY - pInt[1];
					scnX = scnX * scnX + scnY * scnY;

					labels[i].currSize = scnX;
					labels[i].totalSize = nPoint;
					labels[i].nPoints = 0;
					labels[i].shapeType = 1;
					labels[i].points = 0;
					labels[i].flags = flags;

					pInt = points;
					j = nPoint;
					while (j--)
					{
						scnX = mapPosX - *pInt++;
						scnY = mapPosY - *pInt++;
						scnX = scnX * scnX + scnY * scnY;
						if (scnX < labels[i].currSize)
						{
							labels[i].xPos = pInt[-2];
							labels[i].yPos = pInt[-1];
							labels[i].fontStyle = fontStyle;
							labels[i].scaleW = 0;
							labels[i].scaleH = 0;
							labels[i].currSize = scnX;
							if (priority > labels[i].priority)
								labels[i].priority = priority;
						}
					}

					return true;
				}
				pInt += 2;
			}
		}
		return false;
	}
	else if (recType == 3) //lines
	{
		Int32 tmp;
		Int32 lastPtX;
		Int32 lastPtY = points[1];
		Int32 thisPtX;
		Int32 thisPtY;

		Int32 toUpdate;

		visibleSize = 0;
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

			if (left < thisPtX && right > lastPtX && top < thisPtY && bottom > lastPtY)
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

			}
			i++;
		}

		toUpdate = 0;
		found = 0;
		long totalSize = 0;
		long foundInd;
		i = 0;

		while (i < *labelCnt)
		{
			if (recType == labels[i].shapeType)
			{
				if (Text::StrCompare(labels[i].label, labelt) == 0)
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

					if (labels[i].currSize < visibleSize)
					{
						toUpdate = 1;
					}

					if (found >= 2)
					{
					}
					else if (labels[i].points[0] == points[(nPoint << 1) - 2] && labels[i].points[1] == points[(nPoint << 1) - 1])
					{
						Int32 newSize = labels[i].nPoints + nPoint - 1;
						Int32* newArr = MemAlloc(Int32, newSize << 1);
						Int32 k;
						Int32 l;
						l = 0;
						k = 0;
						while (k < labels[i].nPoints)
						{
							newArr[l++] = labels[i].points[k << 1];
							newArr[l++] = labels[i].points[(k << 1) + 1];
							k++;
						}
						k = 1;
						while (k < nPoint)
						{
							newArr[l++] = points[k << 1];
							newArr[l++] = points[(k << 1) + 1];
							k++;
						}
						MemFree(labels[i].points);
						labels[i].points = newArr;
						labels[i].nPoints = newSize;
						toUpdate = 1;
						foundInd = i;
					}
					else if (labels[i].points[(labels[i].nPoints << 1) - 2] == points[0] && labels[i].points[(labels[i].nPoints << 1) - 1] == points[1])
					{
						Int32 newSize = labels[i].nPoints + nPoint - 1;
						Int32* newArr = MemAlloc(Int32, newSize << 1);
						Int32 k;
						Int32 l;
						l = 0;
						k = 0;
						while (k < nPoint)
						{
							newArr[l++] = points[k << 1];
							newArr[l++] = points[(k << 1) + 1];
							k++;
						}
						k = 1;
						while (k < labels[i].nPoints)
						{
							newArr[l++] = labels[i].points[k << 1];
							newArr[l++] = labels[i].points[(k << 1) + 1];
							k++;
						}
						MemFree(labels[i].points);
						labels[i].points = newArr;
						labels[i].nPoints = newSize;
						toUpdate = 1;
						foundInd = i;
					}
					else
					{
						found = 0;
					}
				}
			}
			i++;
		}
		if (found)
		{
			i = foundInd;
		}
		if (found == 0 && visibleSize > 0)
		{
			i = NewLabel(labels, maxLabel, labelCnt, priority);

			if (i < 0)
				return false;

			labels[i].shapeType = recType;
			labels[i].fontStyle = fontStyle;
			labels[i].totalSize = size;
			labels[i].currSize = visibleSize;

			labels[i].label = Text::StrCopyNew(labelt);
			labels[i].points = 0;

			toUpdate = 1;
		}
		else if (toUpdate)
		{
		}

		if (toUpdate)
		{
			/////////////////////////////////////////////////////////////////////////////////
			j = labels[i].nPoints = nPoint;
			if (labels[i].points)
				MemFree(labels[i].points);
			labels[i].points = pInt = MemAlloc(Int32, nPoint * 2);
			j = j << 1;
			long k = 0;
			while (k < j)
				*pInt++ = points[k++];

			visibleSize = visibleSize >> 1;

			j = 1;
			while (j < nPoint)
			{
				lastPtX = points[(j << 1) - 2];
				lastPtY = points[(j << 1) - 1];
				thisPtX = points[(j << 1) + 0];
				thisPtY = points[(j << 1) + 1];
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

				if (left < thisPtX && right > lastPtX && top < thisPtY && bottom > lastPtY)
				{
					lastPtX = points[(j << 1) - 2];
					lastPtY = points[(j << 1) - 1];
					thisPtX = points[(j << 1) + 0];
					thisPtY = points[(j << 1) + 1];
					if (left > lastPtX)
					{
						lastPtY += MulDiv(left - lastPtX, thisPtY - lastPtY, thisPtX - lastPtX);
						lastPtX = left;
					}
					else if (lastPtX > right)
					{
						lastPtY += MulDiv(right - lastPtX, thisPtY - lastPtY, thisPtX - lastPtX);
						lastPtX = right;
					}

					if (top > lastPtY)
					{
						lastPtX += MulDiv(top - lastPtY, thisPtX - lastPtX, thisPtY - lastPtY);
						lastPtY = top;
					}
					else if (lastPtY > bottom)
					{
						lastPtX += MulDiv(bottom - lastPtY, thisPtX - lastPtX, thisPtY - lastPtY);
						lastPtY = bottom;
					}

					if (thisPtX < left)
					{
						thisPtY += MulDiv(left - thisPtX, thisPtY - lastPtY, thisPtX - lastPtX);
						thisPtX = left;
					}
					else if (right < thisPtX)
					{
						thisPtY += MulDiv(right - thisPtX, thisPtY - lastPtY, thisPtX - lastPtX);
						thisPtX = right;
					}

					if (top > thisPtY)
					{
						thisPtX += MulDiv(top - thisPtY, thisPtX - lastPtX, thisPtY - lastPtY);
						thisPtY = top;
					}
					else if (bottom < thisPtY)
					{
						thisPtX += MulDiv(bottom - thisPtY, thisPtX - lastPtX, thisPtY - lastPtY);
						thisPtY = bottom;
					}

					Int32 diffX = (thisPtX - lastPtX);
					Int32 diffY = (thisPtY - lastPtY);
					if (diffX < 0) diffX = -diffX;
					if (diffY < 0) diffY = -diffY;

					if (diffY > diffX)
					{
						if (visibleSize > diffY)
						{
							visibleSize -= diffY;
						}
						else
						{
							if (flags & SFLG_ROTATE)
							{
								labels[i].scaleW = points[(j << 1)] - points[(j << 1) - 2];
								labels[i].scaleH = points[(j << 1) + 1] - points[(j << 1) - 1];
							}
							else
							{
								labels[i].scaleW = 0;
								labels[i].scaleH = 0;
							}

							labels[i].flags = flags;

							if (points[(j << 1) + 1] > points[(j << 1) - 1])
							{
								thisPtY = lastPtY + visibleSize;
							}
							else
							{
								thisPtY = lastPtY - visibleSize;
							}
							thisPtX = points[(j << 1) - 2] + MulDiv(thisPtY - points[(j << 1) - 1], points[(j << 1)] - points[(j << 1) - 2], points[(j << 1) + 1] - points[(j << 1) - 1]);

							labels[i].xPos = thisPtX;
							labels[i].yPos = thisPtY;
							visibleSize = -1;
							break;
						}
					}
					else
					{
						if (visibleSize > diffX)
						{
							visibleSize -= diffX;
						}
						else
						{
							if (flags & SFLG_ROTATE)
							{
								labels[i].scaleW = points[(j << 1)] - points[(j << 1) - 2];
								labels[i].scaleH = points[(j << 1) + 1] - points[(j << 1) - 1];
							}
							else
							{
								labels[i].scaleW = 0;
								labels[i].scaleH = 0;
							}

							labels[i].flags = flags;

							if (points[(j << 1)] > points[(j << 1) - 2])
							{
								thisPtX = lastPtX + visibleSize;
							}
							else
							{
								thisPtX = lastPtX - visibleSize;
							}
							thisPtY = points[(j << 1) + 1] + MulDiv(thisPtX - points[(j << 1)], points[(j << 1) + 1] - points[(j << 1) - 1], points[(j << 1)] - points[(j << 1) - 2]);

							labels[i].xPos = thisPtX;
							labels[i].yPos = thisPtY;
							visibleSize = -1;
							break;
						}
					}
				}
				j++;
			}
			if (visibleSize > 0)
			{
				j = nPoint - 1;
				if (flags & SFLG_ROTATE)
				{
					labels[i].scaleW = points[(j << 1)] - points[(j << 1) - 2];
					labels[i].scaleH = points[(j << 1) + 1] - points[(j << 1) - 1];
				}
				else
				{
					labels[i].scaleW = 0;
					labels[i].scaleH = 0;
				}

				labels[i].flags = flags;

				if (points[(j << 1)] > points[(j << 1) - 2])
				{
					thisPtX = lastPtX + visibleSize;
					thisPtY = points[(j << 1) + 1] + MulDiv(thisPtX - points[(j << 1)], points[(j << 1) + 1] - points[(j << 1) - 1], points[(j << 1)] - points[(j << 1) - 2]);
				}
				else if (points[(j << 1)] < points[(j << 1) - 2])
				{
					thisPtX = lastPtX - visibleSize;
					thisPtY = points[(j << 1) + 1] + MulDiv(thisPtX - points[(j << 1)], points[(j << 1) + 1] - points[(j << 1) - 1], points[(j << 1)] - points[(j << 1) - 2]);
				}
				else
				{
					thisPtX = lastPtX;
					thisPtY = lastPtY;
				}

				labels[i].xPos = thisPtX;
				labels[i].yPos = thisPtY;
				visibleSize = -1;
			}
			return true;
		}
		return false;
	}
	else if (recType == 5) //polygon
	{
		if (nPoint <= 2)
			return false;

		found = 0;
		i = 0;
		while (i < *labelCnt)
		{
			if (recType == labels[i].shapeType)
			{
				if (Text::StrCompare(labels[i].label, labelt) == 0)
				{
					found = 1;
					break;
				}
			}
			i++;
		}

		if (found == 0)
		{
			Int32 startIndex;
			Int32 lastX;
			Int32 lastY;
			Int32 lastTX;
			Int32 lastTY;
			Int32 thisX;
			Int32 thisY;
			Int32 thisTX;
			Int32 thisTY;
			Int32* outPts;
			Int32 outPtCnt;
			Int64 sum;
			Int64 sumX;
			Int64 sumY;

			Int32 crossList[8];
			Int32 crossCnt;
			Int32 inCnt;
			Int32 tmp;
			Int32 tmp2;
			outPts = MemAlloc(Int32, nPoint << 2);
			outPtCnt = 0;
			startIndex = -1;
			i = 0;
			while (i < nPoint)
			{
				lastX = points[i << 1];
				lastY = points[(i << 1) + 1];
				if (lastX >= left && lastX < right && lastY >= top && lastY < bottom)
				{
					lastTX = lastX;
					lastTY = lastY;
					startIndex = i;
					break;
				}
				i++;
			}
			if (startIndex == -1)
			{
				startIndex = 0;
				lastTX = lastX = points[0];
				lastTY = lastY = points[1];
				if (lastTX < left) lastTX = left;
				if (lastTX >= right) lastTX = right;
				if (lastTY < top) lastTY = top;
				if (lastTY >= bottom) lastTY = bottom;
			}
			outPts[(outPtCnt << 1)] = lastTX;
			outPts[(outPtCnt << 1)+1] = lastTY;
			outPtCnt++;

			Int32 prevTX = lastTX;
			Int32 prevTY = lastTY;
			i = (startIndex + 1) % nPoint;
			while (i != startIndex)
			{
				thisX = points[(i << 1)];
				thisY = points[(i << 1) + 1];

				crossCnt = 0;
				inCnt = 0;
				if ((lastX < left && thisX >= left) || (lastX >= left && thisX < left))
				{
					crossList[(crossCnt << 1)] = left;
					lastTY = crossList[(crossCnt << 1) + 1] = (lastY - thisY) * (left - lastX) / (lastX - thisX) + lastY;
					if (lastTY >= top && lastTY <= bottom)
						inCnt++;
					crossCnt++;
				}

				if ((lastX < right && thisX >= right) || (lastX >= right && thisX < right))
				{
					crossList[(crossCnt << 1)] = right;
					lastTY = crossList[(crossCnt << 1) + 1] = (lastY - thisY) * (right - lastX) / (lastX - thisX) + lastY;
					if (lastTY >= top && lastTY <= bottom)
						inCnt++;
					crossCnt++;
				}

				if ((lastY < top && thisY >= top) || (lastY >= top && thisY < top))
				{
					lastTX = crossList[(crossCnt << 1)] = (lastX - thisX) * (top - lastY) / (lastY - thisY) + lastX;
					crossList[(crossCnt << 1) + 1] = top;
					if (lastTX >= left && lastTX <= right)
						inCnt++;
					crossCnt++;
				}

				if ((lastY < bottom && thisY >= bottom) || (lastY >= bottom && thisY < bottom))
				{
					lastTX = crossList[(crossCnt << 1)] = (lastX - thisX) * (bottom - lastY) / (lastY - thisY) + lastX;
					crossList[(crossCnt << 1) + 1] = bottom;
					if (lastTX >= left && lastTX <= right)
						inCnt++;
					crossCnt++;
				}

				if (crossCnt == 0)
				{
					lastTX = thisX;
					lastTY = thisY;
					if (lastTX < left) lastTX = left;
					if (lastTX >= right) lastTX = right;
					if (lastTY < top) lastTY = top;
					if (lastTY >= bottom) lastTY = bottom;

					if ((prevTX != lastTX) || (prevTY != lastTY))
					{
						outPts[(outPtCnt << 1)] = lastTX;
						outPts[(outPtCnt << 1)+1] = lastTY;
						outPtCnt++;
						prevTX = lastTX;
						prevTY = lastTY;
					}
				}
				else if (crossCnt == 1)
				{
					lastTX = crossList[0];
					lastTY = crossList[1];
					if (lastTX < left) lastTX = left;
					if (lastTX >= right) lastTX = right;
					if (lastTY < top) lastTY = top;
					if (lastTY >= bottom) lastTY = bottom;

					if ((prevTX != lastTX) || (prevTY != lastTY))
					{
						outPts[(outPtCnt << 1)] = lastTX;
						outPts[(outPtCnt << 1)+1] = lastTY;
						outPtCnt++;
						prevTX = lastTX;
						prevTY = lastTY;
					}

					lastTX = thisX;
					lastTY = thisY;
					if (lastTX < left) lastTX = left;
					if (lastTX >= right) lastTX = right;
					if (lastTY < top) lastTY = top;
					if (lastTY >= bottom) lastTY = bottom;

					if ((prevTX != lastTX) || (prevTY != lastTY))
					{
						outPts[(outPtCnt << 1)] = lastTX;
						outPts[(outPtCnt << 1)+1] = lastTY;
						outPtCnt++;
						prevTX = lastTX;
						prevTY = lastTY;
					}

				}
				else
				{
					tmp2 = crossCnt;
					while (tmp2 > 0)
					{
						tmp = 1;
						while (tmp < tmp2)
						{
							lastTX = crossList[(tmp << 1) - 2] - lastX;
							lastTY = crossList[(tmp << 1) - 1] - lastY;

							thisTX = crossList[(tmp << 1) ] - lastX;
							thisTY = crossList[(tmp << 1) + 1] - lastY;

							if (((__int64)lastTX * lastTX + (__int64)lastTY * lastTY) > ((__int64)thisTX * thisTX + (__int64)thisTY + thisTY))
							{
								lastTX = crossList[(tmp << 1) - 2];
								lastTY = crossList[(tmp << 1) - 1];
								crossList[(tmp << 1) - 2] = crossList[(tmp << 1) ];
								crossList[(tmp << 1) - 1] = crossList[(tmp << 1) + 1];
								crossList[(tmp << 1)] = lastTX;
								crossList[(tmp << 1) + 1] = lastTY;
							}
							tmp++;
						}
						tmp2--;
					}
					if (inCnt == 0)
					{
						lastTX = crossList[0];
						lastTY = crossList[1];
						if (lastTX < left) lastTX = left;
						if (lastTX >= right) lastTX = right;
						if (lastTY < top) lastTY = top;
						if (lastTY >= bottom) lastTY = bottom;

						if ((prevTX != lastTX) || (prevTY != lastTY))
						{
							outPts[(outPtCnt << 1)] = lastTX;
							outPts[(outPtCnt << 1)+1] = lastTY;
							outPtCnt++;
							prevTX = lastTX;
							prevTY = lastTY;
						}


						lastTX = thisX;
						lastTY = thisY;
						if (lastTX < left) lastTX = left;
						if (lastTX >= right) lastTX = right;
						if (lastTY < top) lastTY = top;
						if (lastTY >= bottom) lastTY = bottom;

						if ((prevTX != lastTX) || (prevTY != lastTY))
						{
							outPts[(outPtCnt << 1)] = lastTX;
							outPts[(outPtCnt << 1)+1] = lastTY;
							outPtCnt++;
							prevTX = lastTX;
							prevTY = lastTY;
						}

					}
					else if (inCnt == 1)
					{
						tmp = 0;
						while (tmp < crossCnt)
						{
							lastTX = crossList[(tmp << 1)];
							lastTY = crossList[(tmp << 1) + 1];
							if (lastTX >= left && lastTX <= right && lastTY >= top && lastTY <= bottom)
							{
								if (lastTX < left) lastTX = left;
								if (lastTX >= right) lastTX = right;
								if (lastTY < top) lastTY = top;
								if (lastTY >= bottom) lastTY = bottom;

								if ((prevTX != lastTX) || (prevTY != lastTY))
								{
									outPts[(outPtCnt << 1)] = lastTX;
									outPts[(outPtCnt << 1)+1] = lastTY;
									outPtCnt++;
									prevTX = lastTX;
									prevTY = lastTY;
								}
								break;
							}
							tmp++;
						}

						lastTX = thisX;
						lastTY = thisY;
						if (lastTX < left) lastTX = left;
						if (lastTX >= right) lastTX = right;
						if (lastTY < top) lastTY = top;
						if (lastTY >= bottom) lastTY = bottom;

						if ((prevTX != lastTX) || (prevTY != lastTY))
						{
							outPts[(outPtCnt << 1)] = lastTX;
							outPts[(outPtCnt << 1)+1] = lastTY;
							outPtCnt++;
							prevTX = lastTX;
							prevTY = lastTY;
						}
					}
					else if (inCnt == 2)
					{
						tmp = 0;
						while (tmp < crossCnt)
						{
							lastTX = crossList[(tmp << 1)];
							lastTY = crossList[(tmp << 1) + 1];
							if (lastTX >= left && lastTX <= right && lastTY >= top && lastTY <= bottom)
							{
								if (lastTX < left) lastTX = left;
								if (lastTX >= right) lastTX = right;
								if (lastTY < top) lastTY = top;
								if (lastTY >= bottom) lastTY = bottom;

								if ((prevTX != lastTX) || (prevTY != lastTY))
								{
									outPts[(outPtCnt << 1)] = lastTX;
									outPts[(outPtCnt << 1)+1] = lastTY;
									outPtCnt++;
									prevTX = lastTX;
									prevTY = lastTY;
								}
							}
							tmp++;
						}
					}
					else
					{
						//MessageBox(NULL, "Error", "Internal", MB_OK);
					}
				}


				found = 1;
				while (found && (outPtCnt > 3))
				{
					found = 0;
					outPtCnt--;
					if ((outPts[((outPtCnt - 1) << 1)] == outPts[((outPtCnt - 2) << 1)]) && (outPts[((outPtCnt - 2) << 1)] == outPts[((outPtCnt - 3) << 1)]))
					{
						outPts[((outPtCnt - 2) << 1) + 1] = outPts[((outPtCnt - 1) << 1) + 1];
						outPts[((outPtCnt - 1) << 1)] = outPts[((outPtCnt - 0) << 1)];
						outPts[((outPtCnt - 1) << 1) + 1] = outPts[((outPtCnt - 0) << 1) + 1];
						outPtCnt--;
						found = 1;
					}
					if ((outPts[((outPtCnt - 1) << 1) + 1] == outPts[((outPtCnt - 2) << 1) + 1]) && (outPts[((outPtCnt - 2) << 1) + 1] == outPts[((outPtCnt - 3) << 1) + 1]))
					{
						outPts[((outPtCnt - 2) << 1)] = outPts[((outPtCnt - 1) << 1)];
						outPts[((outPtCnt - 1) << 1)] = outPts[((outPtCnt - 0) << 1)];
						outPts[((outPtCnt - 1) << 1) + 1] = outPts[((outPtCnt - 0) << 1) + 1];
						outPtCnt--;
						found = 1;
					}

					outPtCnt++;
					if ((outPts[((outPtCnt - 1) << 1)] == outPts[((outPtCnt - 2) << 1)]) && (outPts[((outPtCnt - 2) << 1)] == outPts[((outPtCnt - 3) << 1)]))
					{
						outPts[((outPtCnt - 2) << 1) + 1] = outPts[((outPtCnt - 1) << 1) + 1];
						outPtCnt--;
						found = 1;
					}
					if ((outPts[((outPtCnt - 1) << 1) + 1] == outPts[((outPtCnt - 2) << 1) + 1]) && (outPts[((outPtCnt - 2) << 1) + 1] == outPts[((outPtCnt - 3) << 1) + 1]))
					{
						outPts[((outPtCnt - 2) << 1)] = outPts[((outPtCnt - 1) << 1)];
						outPtCnt--;
						found = 1;
					}

				}

				lastX = thisX;
				lastY = thisY;

				i = (i + 1) % nPoint;
			}

			i = 0;
			sum = 0;
			sumX = sumY = 0;
			lastX = outPts[(outPtCnt << 1) - 2];
			lastY = outPts[(outPtCnt << 1) - 1];
			while (i < outPtCnt)
			{
				thisX = outPts[(i << 1)];
				thisY = outPts[(i << 1) + 1];

				sum += ((__int64)lastX * thisY) - ((__int64)lastY * thisX);

				lastX = thisX;
				lastY = thisY;
				i++;
			}
			if (sum != 0)
			{
				Int32 *finalPts;
				Int32 finalCnt;
				Int32 maxX;
				Int32 maxY;
				Int32 minX;
				Int32 minY;
				finalCnt = 0;
				finalPts = MemAlloc(Int32, outPtCnt << 1);
				sumX += maxX = minX = lastX = finalPts[0] = outPts[0];
				sumY += maxY = minY = lastY = finalPts[1] = outPts[1];
				finalCnt++;

				i = 2;
				while (i < outPtCnt)
				{
					thisX = outPts[(i << 1) - 2];
					thisY = outPts[(i << 1) - 1];
					if ((Int64)(outPts[(i << 1)] - lastX) * (lastY - thisY) == (Int64)(outPts[(i << 1) + 1] - lastY) * (lastX - thisX))
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
				thisY = (maxY + minY) >> 1;
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

				thisX = (maxX + minX) >> 1;
				thisY = (maxY + minY) >> 1;

				i = NewLabel(labels, maxLabel, labelCnt, priority);
				if (i < 0)
				{
					MemFree(outPts);
					return false;
				}

				labels[i].label = Text::StrCopyNew(labelt);
				labels[i].xPos = thisX;
				labels[i].yPos = thisY;
				labels[i].fontStyle = fontStyle;
				labels[i].scaleW = 0;
				labels[i].scaleH = 0;
				labels[i].priority = priority;

				labels[i].currSize = outPtCnt;
				labels[i].totalSize = outPtCnt;
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

void Map::MapConfig::SwapLabel(MapLabels *mapLabels, Int32 index, Int32 index2)
{
	MapLabels l;

	l.label = mapLabels[index].label;
	l.xPos = mapLabels[index].xPos;
	l.yPos = mapLabels[index].yPos;
	l.fontStyle = mapLabels[index].fontStyle;
	l.scaleW = mapLabels[index].scaleW;
	l.scaleH = mapLabels[index].scaleH;
	l.priority = mapLabels[index].priority;
	l.totalSize = mapLabels[index].totalSize;
	l.currSize = mapLabels[index].currSize;
	l.nPoints = mapLabels[index].nPoints;
	l.shapeType = mapLabels[index].shapeType;
	l.points = mapLabels[index].points;
	l.flags = mapLabels[index].flags;

	mapLabels[index].label = mapLabels[index2].label;
	mapLabels[index].xPos = mapLabels[index2].xPos;
	mapLabels[index].yPos = mapLabels[index2].yPos;
	mapLabels[index].fontStyle = mapLabels[index2].fontStyle;
	mapLabels[index].scaleW = mapLabels[index2].scaleW;
	mapLabels[index].scaleH = mapLabels[index2].scaleH;
	mapLabels[index].priority = mapLabels[index2].priority;
	mapLabels[index].totalSize = mapLabels[index2].totalSize;
	mapLabels[index].currSize = mapLabels[index2].currSize;
	mapLabels[index].nPoints = mapLabels[index2].nPoints;
	mapLabels[index].shapeType = mapLabels[index2].shapeType;
	mapLabels[index].points = mapLabels[index2].points;
	mapLabels[index].flags = mapLabels[index2].flags;

	mapLabels[index2].label = l.label;
	mapLabels[index2].xPos = l.xPos;
	mapLabels[index2].yPos = l.yPos;
	mapLabels[index2].fontStyle = l.fontStyle;
	mapLabels[index2].scaleW = l.scaleW;
	mapLabels[index2].scaleH = l.scaleH;
	mapLabels[index2].priority = l.priority;
	mapLabels[index2].totalSize = l.totalSize;
	mapLabels[index2].currSize = l.currSize;
	mapLabels[index2].nPoints = l.nPoints;
	mapLabels[index2].shapeType = l.shapeType;
	mapLabels[index2].points = l.points;
	mapLabels[index2].flags = l.flags;
}

Int32 Map::MapConfig::LabelOverlapped(Int32 *points, Int32 nPoints, Int32 tlx, Int32 tly, Int32 brx, Int32 bry)
{
	while (nPoints--)
	{
		if (points[(nPoints << 2) + 0] < brx && points[(nPoints << 2) + 2] > tlx && points[(nPoints << 2) + 1] < bry && points[(nPoints << 2) + 3] > tly) return 1;
	}

	return 0;
}

void Map::MapConfig::DrawLabels(Media::DrawImage *img, MapLabels *labels, Int32 maxLabel, Int32 *labelCnt, Map::MapView *view, Data::ArrayList<MapFontStyle*> **fonts)
{
	Int32 i;
	Int32 j;
	WChar *dest;
	WChar *lastLbl = 0;
	Int32 left = view->GetLeft();
	Int32 top = view->GetTop();
	Int32 right = view->GetRight();
	Int32 bottom = view->GetBottom();
	Int32 scnWidth = img->GetWidth();
	Int32 scnHeight = img->GetHeight();
	Int32 scaleWidth = right - left;
	Int32 scaleHeight = bottom - top;

	if (*labelCnt)
	{
		Int32* points;
		Int32 currPt;
		Int32 szThis[2];

		Int32 tlx;
		Int32 tly;
		Int32 brx;
		Int32 bry;
		Int32 scnPtX;
		Int32 scnPtY;
		long thisPts[10];
		long thisCnt = 0;

		const WChar *src;

		if (!(points = MemAlloc(Int32, *labelCnt * 20)))
			return;

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

		currPt = 0;
		i = 0;
		while (i < *labelCnt)
		{
			GetCharsSize(img, szThis, labels[i].label, fonts[labels[i].fontStyle], labels[i].scaleW, labels[i].scaleH);

			scnPtX = MulDiv(labels[i].xPos - left, scnWidth, scaleWidth);
			scnPtY = MulDiv(bottom - labels[i].yPos, scnHeight, scaleHeight);

		//	labels[i].shapeType = 0;
			if (labels[i].shapeType == 1)
			{
				j = 1;
				if (j)
				{
					tlx = scnPtX + 1;
					brx = tlx + szThis[0];
					tly = scnPtY - (szThis[1] >> 1);
					bry = tly + szThis[1];

					j = LabelOverlapped(points, currPt, tlx, tly, brx, bry);
				}
				if (j)
				{
					tlx = scnPtX - szThis[0] - 1;
					brx = tlx + szThis[0];
					tly = scnPtY - (szThis[1] >> 1);
					bry = tly + szThis[1];

					j = LabelOverlapped(points, currPt, tlx, tly, brx, bry);
				}
				if (j)
				{
					tlx = scnPtX - (szThis[0] >> 1);
					brx = tlx + szThis[0];
					tly = scnPtY - szThis[1] - 1;
					bry = tly + szThis[1];

					j = LabelOverlapped(points, currPt, tlx, tly, brx, bry);
				}
				if (j)
				{
					tlx = scnPtX - (szThis[0] >> 1);
					brx = tlx + szThis[0];
					tly = scnPtY + 1;
					bry = tly + szThis[1];

					j = LabelOverlapped(points, currPt, tlx, tly, brx, bry);
				}

				if (!j)
				{
					DrawChars(img, labels[i].label, (tlx + brx) >> 1, (tly + bry) >> 1, 0, 0, fonts[labels[i].fontStyle], 0);

					points[(currPt << 2)] = tlx;
					points[(currPt << 2) + 1] = tly;
					points[(currPt << 2) + 2] = brx;
					points[(currPt << 2) + 3] = bry;
					currPt++;
				}
			}
			else if (labels[i].shapeType == 3)
			{
				if (lastLbl)
				{
					src = labels[i].label;
					dest = lastLbl;
					while (*src++)
					{
						if (src[-1] != *dest++)
						{
							break;
						}
					}
					if (src[-1] || *dest)
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
						MemFree(lastLbl);
					src = labels[i].label;
					while (*src++);
					dest = lastLbl = MemAlloc(WChar, (src - labels[i].label));
					src = labels[i].label;
					while (*src++)
						*dest++ = src[-1];
					*dest++ = 0;
				}
				else
				{
				//	MessageBoxW(NULL, L"Test", lastLbl, MB_OK);
				}

				j = 1;
				if (j)
				{
					tlx = scnPtX - (szThis[0] >> 1);
					brx = tlx + szThis[0];
					tly = scnPtY - (szThis[1] >> 1);
					bry = tly + szThis[1];

					j = LabelOverlapped(points, currPt, tlx, tly, brx, bry);
				}

				if (!j && thisCnt < 10)
				{
					long m;
					long n = 1;
					long tmpV;
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
						DrawChars(img, labels[i].label, (tlx + brx) >> 1, (tly + bry) >> 1, labels[i].scaleW, labels[i].scaleH, fonts[labels[i].fontStyle], (labels[i].flags & SFLG_ALIGN) != 0);

						points[(currPt << 2)] = tlx;
						points[(currPt << 2) + 1] = tly;
						points[(currPt << 2) + 2] = brx;
						points[(currPt << 2) + 3] = bry;
						currPt++;

						thisPts[thisCnt++] = (tlx + brx) >> 1;
						thisPts[thisCnt++] = (tly + bry) >> 1;
					}
					j = (thisCnt < 10);
				}

				if (j)
				{
					Int32 k;
					Int32 l;
					Int32 *ptInt = labels[i].points;

					k = 0;
					l = labels[i].nPoints - 1;
					while (j && k < l)
					{
						Int32 lastPtX;
						Int32 lastPtY;
						Int32 thisPtX;
						Int32 thisPtY;
						Int32 tmp;
						lastPtX = ptInt[0];
						lastPtY = ptInt[1];
						thisPtX = ptInt[2];
						thisPtY = ptInt[3];
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

						if (left < thisPtX && right > lastPtX && top < thisPtY && bottom > lastPtY)
						{
							lastPtX = ptInt[0];
							lastPtY = ptInt[1];
							thisPtX = ptInt[2];
							thisPtY = ptInt[3];
							if (left > lastPtX)
							{
								lastPtY += MulDiv(left - lastPtX, thisPtY - lastPtY, thisPtX - lastPtX);
								lastPtX = left;
							}
							else if (lastPtX > right)
							{
								lastPtY += MulDiv(right - lastPtX, thisPtY - lastPtY, thisPtX - lastPtX);
								lastPtX = right;
							}

							if (top > lastPtY)
							{
								lastPtX += MulDiv(top - lastPtY, thisPtX - lastPtX, thisPtY - lastPtY);
								lastPtY = top;
							}
							else if (lastPtY > bottom)
							{
								lastPtX += MulDiv(bottom - lastPtY, thisPtX - lastPtX, thisPtY - lastPtY);
								lastPtY = bottom;
							}

							if (thisPtX < left)
							{
								thisPtY += MulDiv(left - thisPtX, thisPtY - lastPtY, thisPtX - lastPtX);
								thisPtX = left;
							}
							else if (right < thisPtX)
							{
								thisPtY += MulDiv(right - thisPtX, thisPtY - lastPtY, thisPtX - lastPtX);
								thisPtX = right;
							}

							if (top > thisPtY)
							{
								thisPtX += MulDiv(top - thisPtY, thisPtX - lastPtX, thisPtY - lastPtY);
								thisPtY = top;
							}
							else if (bottom < thisPtY)
							{
								thisPtX += MulDiv(bottom - thisPtY, thisPtX - lastPtX, thisPtY - lastPtY);
								thisPtY = bottom;
							}

							labels[i].scaleW = 0;
							labels[i].scaleH = 0;

							scnPtX = (thisPtX + lastPtX) >> 1;
							scnPtY = (thisPtY + lastPtY) >> 1;
							if (labels[i].flags & SFLG_ROTATE)
							{
								labels[i].scaleW = ptInt[2] - ptInt[0];
								labels[i].scaleH = ptInt[3] - ptInt[1];
							}

							scnPtX = MulDiv(scnPtX - (Int32)left, scnWidth, scaleWidth);
							scnPtY = MulDiv(bottom - scnPtY, scnHeight, scaleHeight);

							GetCharsSize(img, szThis, labels[i].label, fonts[labels[i].fontStyle], labels[i].scaleW, labels[i].scaleH);

							tlx = scnPtX - (szThis[0] >> 1);
							brx = tlx + szThis[0];
							tly = scnPtY - (szThis[1] >> 1);
							bry = tly + szThis[1];

							j = LabelOverlapped(points, currPt, tlx, tly, brx, bry);
						}


						if (!j && thisCnt < 10)
						{
							long m;
							long n = 1;
							long tmpV;
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
								DrawChars(img, labels[i].label, (tlx + brx) >> 1, (tly + bry) >> 1, labels[i].scaleW, labels[i].scaleH, fonts[labels[i].fontStyle], (labels[i].flags & SFLG_ALIGN) != 0);

								points[(currPt << 2)] = tlx;
								points[(currPt << 2) + 1] = tly;
								points[(currPt << 2) + 2] = brx;
								points[(currPt << 2) + 3] = bry;
								currPt++;

								thisPts[thisCnt++] = (tlx + brx) >> 1;
								thisPts[thisCnt++] = (tly + bry) >> 1;
							}
							j = (thisCnt < 10);
						}

						ptInt += 2;
						k++;
					}
				}

				if (!j && false)
				{
					DrawChars(img, labels[i].label,  (tlx + brx) >> 1, (tly + bry) >> 1, labels[i].scaleW, labels[i].scaleH, fonts[labels[i].fontStyle], (labels[i].flags & SFLG_ALIGN) != 0);

					points[(currPt << 2)] = tlx;
					points[(currPt << 2) + 1] = tly;
					points[(currPt << 2) + 2] = brx;
					points[(currPt << 2) + 3] = bry;
					currPt++;
				}
			}
			else if (labels[i].shapeType == 5)
			{
				j = 1;
				if (j)
				{
					tlx = scnPtX  - (szThis[0] >> 1);
					brx = tlx + szThis[0];
					tly = scnPtY - (szThis[1] >> 1);
					bry = tly + szThis[1];

					j = LabelOverlapped(points, currPt, tlx, tly, brx, bry);
				}
				if (j)
				{
					tlx = scnPtX + 1;
					brx = tlx + szThis[0];
					tly = scnPtY - (szThis[1] >> 1);
					bry = tly + szThis[1];

					j = LabelOverlapped(points, currPt, tlx, tly, brx, bry);
				}
				if (j)
				{
					tlx = scnPtX - szThis[0] - 1;
					brx = tlx + szThis[0];
					tly = scnPtY - (szThis[1] >> 1);
					bry = tly + szThis[1];

					j = LabelOverlapped(points, currPt, tlx, tly, brx, bry);
				}
				if (j)
				{
					tlx = scnPtX - (szThis[0] >> 1);
					brx = tlx + szThis[0];
					tly = scnPtY - szThis[1] - 1;
					bry = tly + szThis[1];

					j = LabelOverlapped(points, currPt, tlx, tly, brx, bry);
				}
				if (j)
				{
					tlx = scnPtX - (szThis[0] >> 1);
					brx = tlx + szThis[0];
					tly = scnPtY + 1;
					bry = tly + szThis[1];

					j = LabelOverlapped(points, currPt, tlx, tly, brx, bry);
				}

				if (!j)
				{
					DrawChars(img, labels[i].label, (tlx + brx) >> 1, (tly + bry) >> 1, 0, 0, fonts[labels[i].fontStyle], 0);

					points[(currPt << 2)] = tlx;
					points[(currPt << 2) + 1] = tly;
					points[(currPt << 2) + 2] = brx;
					points[(currPt << 2) + 3] = bry;
					currPt++;
				}
			}
			else
			{

			}

			i++;
		}

		MemFree(points);
	}

	i = *labelCnt;
	while (i-- > 0)
	{
		Text::StrDelNew(labels[i].label);
		if (labels[i].points)
			MemFree(labels[i].points);
	}
	if (lastLbl)
		MemFree(lastLbl);
}

Map::MapConfig::MapConfig(const WChar *fileName, Media::DrawEngine *eng, Data::ArrayList<Map::IMapDrawLayer*> *layerList, Parser::ParserList *parserList, WChar *forceBase)
{
	WChar lineBuff[1024];
	WChar layerName[512];
	WChar *baseDir = layerName;
	WChar *strs[10];
	WChar *sptr;
	IO::FileStream *fstm;
	IO::StreamReader *rdr;
	Int32 i;
	Int32 j;
	MapLineStyle *currLine;
	MapFontStyle *currFont;
	MapLayerStyle *currLayer;

	this->drawEng = eng;
	this->inited = false;
	NEW_CLASS(drawMut, Sync::Mutex());

	NEW_CLASS(fstm, IO::FileStream(fileName, IO::FileStream::FILE_MODE_READONLY, IO::FileStream::FILE_SHARE_DENY_NONE));
	if (fstm->IsError())
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
		NEW_CLASS(rdr, IO::StreamReader(fstm));
		if (forceBase)
		{
			baseDir = Text::StrConcat(layerName, forceBase);
		}
		while (rdr->ReadLine(lineBuff, 1023))
		{
			OSInt strCnt;
			Int32 lyrType;
			strCnt = Text::StrSplitTrim(strs, 10, lineBuff, ',');

			lyrType = Text::StrToInt32(strs[0]);
			switch (lyrType)
			{
			case 1:
				this->bgColor = ToColor(strs[1]);
				this->nLine = Text::StrToInt32(strs[2]);
				this->nFont = Text::StrToInt32(strs[3]);
				this->nStr = Text::StrToInt32(strs[4]);
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
					baseDir = Text::StrConcat(layerName, strs[1]);
				}
				break;
			case 3:
				i = Text::StrToInt32(strs[1]);
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
					currLine->lineType = Text::StrToInt32(strs[2]);
					currLine->lineWidth = Text::StrToInt32(strs[3]);
					currLine->color = ToColor(strs[4]);
					currLine->styles = 0;
					this->lines[i]->Add(currLine);
				}
				else
				{
					j = 6;
					while (j < strCnt)
					{
						strs[j++][-1] = ',';
					}
					sptr = strs[strCnt-1];
					while (*sptr++);
					currLine = (MapLineStyle*)MemAlloc(MapLineStyle, 1);
					currLine->lineType = Text::StrToInt32(strs[2]);
					currLine->lineWidth = Text::StrToInt32(strs[3]);
					currLine->color = ToColor(strs[4]);
					currLine->styles = MemAlloc(WChar, sptr - strs[5]);
					Text::StrConcat(currLine->styles, strs[5]);
					this->lines[i]->Add(currLine);
				}
				break;
			case 5:
				i = Text::StrToInt32(strs[1]);
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
				currFont->fontType = Text::StrToInt32(strs[2]);
				sptr = strs[3];
				while (*sptr++);
				currFont->fontName = MemAlloc(WChar, sptr - strs[3]);
				Text::StrConcat(currFont->fontName, strs[3]);
				currFont->fontSize = (Text::StrToInt32(strs[4]) * 3) >> 2;
				currFont->thick = Text::StrToInt32(strs[5]);
				currFont->color = ToColor(strs[6]);
				this->fonts[i]->Add(currFont);
				break;
			case 0:
				break;
			case 13:
				break;
			case 6:
				currLayer = MemAlloc(MapLayerStyle, 1);
				currLayer->drawType = 6;
				currLayer->minScale = Text::StrToInt32(strs[2]);
				currLayer->maxScale = Text::StrToInt32(strs[3]);
				currLayer->img = 0;
				Text::StrConcat(baseDir, strs[1]);
				currLayer->lyr = GetDrawLayer(layerName, layerList);
				if (currLayer->lyr == 0)
				{
					MemFree(currLayer);
				}
				else
				{
					currLayer->style = Text::StrToInt32(strs[4]);
					currLayer->bkColor = 0;
					this->drawList->Add(currLayer);
				}
				break;
			case 7:
				currLayer = MemAlloc(MapLayerStyle, 1);
				currLayer->drawType = 7;
				currLayer->minScale = Text::StrToInt32(strs[2]);
				currLayer->maxScale = Text::StrToInt32(strs[3]);
				currLayer->img = 0;
				Text::StrConcat(baseDir, strs[1]);
				currLayer->lyr = GetDrawLayer(layerName, layerList);
				if (currLayer->lyr == 0)
				{
					MemFree(currLayer);
				}
				else
				{
					currLayer->style = Text::StrToInt32(strs[4]);
					currLayer->bkColor = ToColor(strs[5]);
					this->drawList->Add(currLayer);
				}
				break;
			case 8:
//					minScale = Text::StrToInt32(strs[2]);
//					maxScale = Text::StrToInt32(strs[3]);
				break;
			case 9:
				currLayer = MemAlloc(MapLayerStyle, 1);
				currLayer->drawType = 9;
				currLayer->minScale = Text::StrToInt32(strs[2]);
				currLayer->maxScale = Text::StrToInt32(strs[3]);
				currLayer->img = 0;
				Text::StrConcat(baseDir, strs[1]);
				currLayer->lyr = GetDrawLayer(layerName, layerList);
				if (currLayer->lyr == 0)
				{
					MemFree(currLayer);
				}
				else
				{
					currLayer->priority = Text::StrToInt32(strs[4]);
					currLayer->style = Text::StrToInt32(strs[5]);
					currLayer->bkColor = Text::StrToInt32(strs[6]);
					this->drawList->Add(currLayer);
				}
				break;
			case 10:
				currLayer = MemAlloc(MapLayerStyle, 1);
				currLayer->drawType = 10;
				currLayer->minScale = Text::StrToInt32(strs[2]);
				currLayer->maxScale = Text::StrToInt32(strs[3]);
				currLayer->img = 0;
				{
					IO::StmData::FileData *fd;
					IO::ParsedObject::ParserType pt;
					NEW_CLASS(fd, IO::StmData::FileData(strs[4]));
					IO::ParsedObject *obj = parserList->ParseFile(fd, &pt);
					DEL_CLASS(fd);
					if (obj)
					{
						if (obj->GetParserType() == IO::ParsedObject::PT_IMAGE_LIST_PARSER)
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
				}
				if (currLayer->img == 0)
				{
					currLayer->img = this->drawEng->LoadImageW(strs[4]);
				}
				if (currLayer->img == 0)
				{
					MemFree(currLayer);
				}
				else
				{
					Text::StrConcat(baseDir, strs[1]);
					currLayer->lyr = GetDrawLayer(layerName, layerList);
					if (currLayer->lyr == 0)
					{
						this->drawEng->DeleteImage(currLayer->img);
						MemFree(currLayer);
					}
					else
					{
						this->drawList->Add(currLayer);
					}
				}
				break;
			default:
				break;
			}
		}
		DEL_CLASS(rdr);
	}
	DEL_CLASS(fstm);
}

Map::MapConfig::~MapConfig()
{
	OSInt i;
	OSInt j;
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
		OSInt i = this->drawList->GetCount();
		while (i-- > 0)
		{
			currLyr = (Map::MapLayerStyle*)this->drawList->GetItem(i);
			if (currLyr->img)
			{
				this->drawEng->DeleteImage(currLyr->img);
			}
			MemFree(currLyr);
		}
		DEL_CLASS(this->drawList);
	}
	DEL_CLASS(drawMut);
}

Bool Map::MapConfig::IsError()
{
	return this->drawList == 0;
}

Media::DrawPen *Map::MapConfig::CreatePen(Media::DrawImage *img, Int32 lineStyle, Int32 lineLayer)
{
	if (lineStyle < 0 || lineStyle >= this->nLine)
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
		return img->NewPenARGB(thisLine->color, thisLine->lineWidth, 0, 0);
	}
	else if (thisLine->lineType == 1)
	{
		return img->NewPenARGB(thisLine->color, thisLine->lineWidth, 0, 0);
//		lines[index].nStyle = lines[index].width;
//		lines[index].pen = map->img->NewPenARGB(lines[index].color, lines[index].width, 0, 0);
	}
	else if (thisLine->lineType == 2)
	{
		UInt8 *pattern = MemAlloc(UInt8, Text::StrCharCnt(thisLine->styles));
		Int32 currVal;
		WChar *currCh;
		Int32 i;

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
				pattern[i++] = currVal;
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
		Media::DrawPen *pen = img->NewPenARGB(thisLine->color, thisLine->lineWidth, pattern, i);
		MemFree(pattern);
		return pen;
	}
	return 0;
}

void Map::MapConfig::DrawMap(Media::DrawImage *img, Map::MapView *view, Bool *isLayerEmpty, Map::MapScheduler *mapSch)
{
	Int32 index;
	Int32 index2;
	OSInt layerCnt = this->drawList->GetCount();
	Data::ArrayListInt *arr;
	Data::ArrayList<Map::MapFontStyle*> **myArrs;
	Data::ArrayList<Map::MapFontStyle*> *fontArr;
	Map::IMapDrawLayer *lyr;
	Map::MapLayerStyle *lyrs;
	Map::DrawObject *dobj;
	Map::MapFontStyle *fnt;
	Map::MapFontStyle *fnt2;
	Media::DrawBrush *brush;
	Media::DrawPen *pen;
	OSInt i;
	OSInt j;
	OSInt k;
	Int32 lastId;
	Int32 thisId;
	Int32 thisScale;
	Int32 labelCnt = 0;
	Int32 maxLabel = this->nStr;
	*isLayerEmpty = true;
	Map::MapLabels *labels = MemAlloc(Map::MapLabels, maxLabel);

	NEW_CLASS(arr, Data::ArrayListInt());
	thisScale = view->GetScale();
#ifndef NOSCH
	mapSch->SetMapView(view, img);
#endif

	brush = img->NewBrushARGB(this->bgColor);
	img->DrawRect(0, 0, img->GetWidth(), img->GetHeight(), 0, brush);
	img->DelBrush(brush);

	myArrs = MemAlloc(Data::ArrayList<Map::MapFontStyle*>*, this->nFont);
	i = this->nFont;
	while (i-- > 0)
	{
		fontArr = this->fonts[i];
		if (fontArr)
		{
			j = 0;
			k = fontArr->GetCount();
			NEW_CLASS(myArrs[i], Data::ArrayList<Map::MapFontStyle*>());
			while (j < k)
			{
				fnt = (Map::MapFontStyle*)fontArr->GetItem(j);
				fnt2 = MemAlloc(Map::MapFontStyle, 1);
				fnt2->color = fnt->color;
				fnt2->fontName = fnt->fontName;
				fnt2->fontSize = fnt->fontSize;
				fnt2->fontType = fnt->fontType;
				fnt2->thick = fnt->thick;
				if (fnt->fontType == 0)
				{
					Int32 s = 0;
					if (fnt->thick > 0)
						s = s | DFS_BOLD;
					fnt2->font = img->NewFontW(fnt->fontName, fnt->fontSize, s);
					fnt2->other = img->NewBrushARGB(fnt->color);
				}
				else if (fnt->fontType == 1)
				{
					fnt2->other = img->NewBrushARGB(fnt->color);
				}
				else if (fnt->fontType == 2)
				{
					fnt2->other = img->NewPenARGB(fnt->color, fnt->thick, 0, 0);
				}
				else if (fnt->fontType == 4)
				{
					fnt2->font = img->NewFontW(fnt->fontName, fnt->fontSize, 0);
					fnt2->other = img->NewBrushARGB(fnt->color);
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
			if (lyrs->drawType == 7)
			{
				lyr = lyrs->lyr;
				if (lyr)
				{
					arr->Clear();
					lyr->GetObjectIds(arr, 0, view->GetLeft(), view->GetTop(), view->GetRight(), view->GetBottom(), true);

					if ((i = arr->GetCount()) > 0)
					{
#ifdef NOSCH
						Data::ArrayList *drawArr;
						NEW_CLASS(drawArr, Data::ArrayList());
#endif
						mapSch->SetDrawType(lyr, Map::MapScheduler::MSDT_POLYGON, pen = CreatePen(img, lyrs->style, 0), brush = img->NewBrushARGB(lyrs->bkColor), 0, isLayerEmpty);

						session = lyr->BeginGetObject();
						lastId = -1;
						while (i-- > 0)
						{
							thisId = arr->GetItem(i);
							if (thisId != lastId)
							{
								lastId = thisId;
								dobj = lyr->GetObjectById(session, thisId);
#ifndef NOSCH
								mapSch->Draw(dobj);
#else
								if (view->MapToScnXY(dobj->points, dobj->points, dobj->nPoints, 0, 0))
									*isLayerEmpty = false;
								drawArr->Add(dobj);
#endif
							}
						}
						lyr->EndGetObject(session);
#ifdef NOSCH
						pen = CreatePen(img, lyrs->style, 0);
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
							while (pen = CreatePen(img, lyrs->style, index2++))
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
			else if (lyrs->drawType == 6)
			{
				lyr = lyrs->lyr;
				if (lyr)
				{
					arr->Clear();
					lyr->GetObjectIds(arr, 0, view->GetLeft(), view->GetTop(), view->GetRight(), view->GetBottom(), true);

					if ((i = arr->GetCount()) > 0)
					{
#ifdef NOSCH
						Data::ArrayList *drawArr;
						NEW_CLASS(drawArr, Data::ArrayList());
#else
						mapSch->SetDrawType(lyr, Map::MapScheduler::MSDT_POLYLINE, pen = CreatePen(img, lyrs->style, 0), 0, 0, isLayerEmpty);
#endif

						session = lyr->BeginGetObject();
						lastId = -1;
						while (i-- > 0)
						{
							thisId = arr->GetItem(i);
							if (thisId != lastId)
							{
								lastId = thisId;
								dobj = lyr->GetObjectById(session, thisId);
#ifndef NOSCH
								mapSch->Draw(dobj);
#else
								if (view->MapToScnXY(dobj->points, dobj->points, dobj->nPoints, 0, 0))
									*isLayerEmpty = false;
								drawArr->Add(dobj);
#endif
							}
						}
						lyr->EndGetObject(session);

#ifdef NOSCH
						index2 = 0;
						while (pen = CreatePen(img, lyrs->style, index2))
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
				DrawString(img, lyrs, view, myArrs, labels, maxLabel, &labelCnt, isLayerEmpty);
			}
			else if (lyrs->drawType == 10)
			{
				DrawPoints(img, lyrs, view, isLayerEmpty, mapSch);
			}
		}
	}

//	drawMut->Lock();
	DrawLabels(img, labels, maxLabel, &labelCnt, view, myArrs);
//	drawMut->Unlock();

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

	DEL_CLASS(arr);
}

void Map::MapConfig::ReleaseLayers(Data::ArrayList<Map::IMapDrawLayer*> *layerList)
{
	Map::CIPLayer *lyr;
	OSInt i;
	i = layerList->GetCount();
	while (i-- > 0)
	{
		lyr = (Map::CIPLayer *)layerList->GetItem(i);
		DEL_CLASS(lyr);
	}
}
