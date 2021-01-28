#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayList.h"
#include "Data/ArrayListInt.h"
#include "IO/Stream.h"
#include "IO/FileStream.h"
#include "IO/IStreamData.h"
#include "IO/StmData/FileData.h"
#include "IO/IFileParser.h"
#include "IO/Path.h"
#include "Map/IMapDrawLayer.h"
#include "Map/MapView.h"
#include "Map/SPDLayer.h"
#include "Map/SPCFile.h"
#include "Math/Math.h"
#include "Manage/HiResClock.h"
#include "Media/DrawEngine.h"
#include "Media/GDIEngine.h"
#include "Media/FrameInfo.h"
#include "Media/IImgResizer.h"
#include "Media/Image.h"
#include "Parser/FileParser/BMPParser.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"
#include "Text/MyString.h"
#include <windows.h>

#define SFLG_ROTATE 1
#define SFLG_SMART 2
#define SFLG_ALIGN 4
#define LBLMINDIST 150
#ifdef _WIN32_WCE
#define SRCHCNT 1
#else
#define SRCHCNT 1
#endif

#define PI 3.14159265358979323846

void Map::SPCFile::DrawChars(Media::DrawImage *img, const WChar *str1, Int32 scnPosX, Int32 scnPosY, Int32 scaleW, Int32 scaleH, SPCFontStyle *fontStyle, Bool isAlign)
{
	WChar sbuff[256];
	Text::StrConcat(sbuff, str1);
	Double size[2];
	UInt16 absH;
//	Int32 fntCount;
//	Int32 i;
	Int32 j;
	Int32 k;
//	Int32 maxSize;
//	Int32 maxIndex;
	OSInt lblSize;

	if (fontStyle == 0)
		return;
	if (scaleW < 0)
	{
		scaleW = -scaleW;
		scaleH = -scaleH;
	}

	lblSize = Text::StrCharCnt(str1);
	img->GetTextSize(fontStyle->font, str1, lblSize, size);

	if (scaleH == 0) //scaleW == 0 && scaleH == 0)
	{
		img->SetTextAlign(Media::DrawEngine::DRAW_POS_TOPLEFT);

		if (fontStyle->buffSize > 0)
		{
			j = fontStyle->buffSize;
			Double px = scnPosX - (size[0] * 0.5);
			Double py = scnPosY - (size[1] * 0.5);
			while (j >= 0)
			{
				k = fontStyle->buffSize - j;
				while (k >= 0)
				{
					if (j | k)
					{
						img->DrawString(px + j, py + k, str1, fontStyle->font, fontStyle->brushBuff);
						img->DrawString(px + j, py - k, str1, fontStyle->font, fontStyle->brushBuff);
						img->DrawString(px - j, py + k, str1, fontStyle->font, fontStyle->brushBuff);
						img->DrawString(px - j, py - k, str1, fontStyle->font, fontStyle->brushBuff);
					}
					else
					{
						img->DrawString(px, py, str1, fontStyle->font, fontStyle->brush);
					}
					k--;
				}
				j--;
			}
		}
		else
		{
			img->DrawString(scnPosX - (size[0] * 0.5), scnPosY - (size[1] * 0.5), str1, fontStyle->font, fontStyle->brush);
		}

		return;
	}


	if (scaleH < 0)
		absH = (UInt16)-scaleH;
	else
		absH = (UInt16)scaleH;

	Double degD = Math::ArcTan(scaleH / (Double)scaleW);

	Int32 deg = (Int32) (Math::ArcTan(scaleH / (Double)scaleW) * 1800 / PI);
	while (deg < 0)
		deg += 3600;

	Int32 lastScaleW = scaleW;
	Int32 lastScaleH = scaleH;

	scaleW = lastScaleW;
	scaleH = lastScaleH;
//	Int32 pt[10];
//	Int32 xPos;
//	Int32 yPos;
//	double sVal;
//	double cVal;

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
		img->GetTextSize(fontStyle->font, str1, lblSize, szThis);

		if ((szThis[0] * absH) < ((szThis[1] * lblSize * scaleW)))
		{
			scaleW = -scaleW;
			startX = scnPosX - (tmp = (szThis[0] * 0.5));
			if (scaleW)
				startY = scnPosY - (szThis[1] * 0.5) - (Int16)((Int32)tmp * (Int32)scaleH / (Int32)scaleW);
			else
				startY = scnPosY - (szThis[1] * 0.5);
			type = 0;
		}
		else
		{
			scaleW = -scaleW;
			if (scaleH > 0)
			{
				startY = scnPosY - (tmp = ((szThis[1] * lblSize) * 0.5));
				startX = scnPosX - (Int16)((Int32)tmp * (Int32)scaleW / (Int32)scaleH);
			}
			else if (scaleH)
			{
				scaleW = -scaleW;
				scaleH = -scaleH;
				startY = scnPosY - (tmp = ((szThis[1] * lblSize) * 0.5));
				startX = scnPosX - (Int16)((Int32)tmp * (Int32)scaleW / (Int32)scaleH);
			}
			else
			{
				startY = scnPosY - (tmp = ((szThis[1] * lblSize) * 0.5));
				startX = scnPosX;
			}
			type = 1;
		}

		Double cHeight;
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
			img->GetTextSize(fontStyle->font, lbl, 1, szThis);

			if (type)
			{
				if (fontStyle->buffSize <= 0)
				{
					WChar l[2];
					l[0] = lbl[0];
					l[1] = 0;
					img->DrawString(startX + currX - (szThis[0] * 0.5), startY + currY, l, fontStyle->font, fontStyle->brush);
				}
				else
				{
					WChar l[2];
					j = fontStyle->buffSize;
					Double px = startX + currX - (szThis[0] * 0.5);
					Double py = startY + currY;
					l[0] = lbl[0];
					l[1] = 0;
					while (j >= 0)
					{
						k = fontStyle->buffSize - j;
						while (k >= 0)
						{
							if (j | k)
							{
								img->DrawString(px + j, py + k, l, fontStyle->font, fontStyle->brushBuff);
								img->DrawString(px + j, py - k, l, fontStyle->font, fontStyle->brushBuff);
								img->DrawString(px - j, py + k, l, fontStyle->font, fontStyle->brushBuff);
								img->DrawString(px - j, py - k, l, fontStyle->font, fontStyle->brushBuff);
							}
							else
							{
								img->DrawString(px, py, l, fontStyle->font, fontStyle->brush);
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
				if (fontStyle->buffSize <= 0)
				{
					WChar l[2];
					l[0] = lbl[0];
					l[1] = 0;
					img->DrawString(startX + currX, startY + currY, l, fontStyle->font, fontStyle->brush);
				}
				else
				{
					WChar l[2];
					j = fontStyle->buffSize;
					Double px = startX + currX;
					Double py = startY + currY;
					l[0] = lbl[0];
					l[1] = 0;
					while (j >= 0)
					{
						k = fontStyle->buffSize - j;
						while (k >= 0)
						{
							if (j | k)
							{
								img->DrawString(px + j, py + k, l, fontStyle->font, fontStyle->brushBuff);
								img->DrawString(px + j, py - k, l, fontStyle->font, fontStyle->brushBuff);
								img->DrawString(px - j, py + k, l, fontStyle->font, fontStyle->brushBuff);
								img->DrawString(px - j, py - k, l, fontStyle->font, fontStyle->brushBuff);
							}
							else
							{
								img->DrawString(px, py, l, fontStyle->font, fontStyle->brush);
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

		if (fontStyle->buffSize <= 0)
		{
			img->DrawStringRot(scnPosX, scnPosY, str1, fontStyle->font, fontStyle->brush, degD * 180.0 / PI);
		}
		else
		{
			j = fontStyle->buffSize;
			Int32 px = scnPosX;
			Int32 py = scnPosY;
			while (j >= 0)
			{
				k = fontStyle->buffSize;
				while (k >= 0)
				{
					if (j | k)
					{
						img->DrawStringRot(px + j, py + k, str1, fontStyle->font, fontStyle->brushBuff, degD * 180 / PI);
						img->DrawStringRot(px + j, py - k, str1, fontStyle->font, fontStyle->brushBuff, degD * 180 / PI);
						img->DrawStringRot(px - j, py + k, str1, fontStyle->font, fontStyle->brushBuff, degD * 180 / PI);
						img->DrawStringRot(px - j, py - k, str1, fontStyle->font, fontStyle->brushBuff, degD * 180 / PI);
					}
					else
					{
						img->DrawStringRot(px, py, str1, fontStyle->font, fontStyle->brush, degD * 180 / PI);
					}
					k--;
				}
				j--;
			}
		}
	}
}

void Map::SPCFile::GetCharsSize(Media::DrawImage *img, Int32 *size, const WChar *label, SPCFontStyle *fontStyle, Int32 scaleW, Int32 scaleH)
{
	Double szTmp[2];
	img->GetTextSize(fontStyle->font, label, Text::StrCharCnt(label), szTmp);

	if (scaleH == 0)
	{
		size[0] = Math::Double2Int(szTmp[0]);
		size[1] = Math::Double2Int(szTmp[1]);

		return;
	}


	Double pt[8];

	if (scaleW < 0)
	{
		scaleW = -scaleW;
		scaleH = -scaleH;
	}
	Double degD = Math::ArcTan(scaleH / (Double)scaleW);
	Double xPos = szTmp[0];
	Double yPos = szTmp[1];
	Double sVal;
	Double cVal;
	Double xs = ((xPos * 0.5) * (sVal = Math::Sin(degD)));
	Double ys = ((yPos * 0.5) * sVal);
	Double xc = ((xPos * 0.5) * (cVal = Math::Cos(degD)));
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
	size[0] = Math::Double2Int(maxX - minX);
	size[1] = Math::Double2Int(maxY - minY);
}

Int32 Map::SPCFile::ToColor(Int32 c)
{
	return 0xff000000 | ((c & 0xff) << 16) | (c & 0xff00) | ((c >> 16) & 0xff);
}

Map::IMapDrawLayer *Map::SPCFile::GetDrawLayer(const WChar *name, Data::ArrayList<Map::SPDLayer*> *layerList)
{
	Map::SPDLayer *cip;
	OSInt i = layerList->GetCount();
	while (i-- > 0)
	{
		cip = layerList->GetItem(i);
		if (IO::Path::FileNameCompare(name, cip->GetName()) == 0)
		{
			return cip;
		}
	}
	NEW_CLASS(cip, Map::SPDLayer(name));
	if (cip->IsError())
	{
		DEL_CLASS(cip);
		return 0;
	}
	layerList->Add(cip);
	return cip;
}

void Map::SPCFile::DrawPoints(Media::DrawImage *img, SPCLayerStyle *lyrs, Map::MapView *view, Bool *isLayerEmpty)
{
	Data::ArrayListInt64 *arri;
	Map::DrawObject *dobj;
	Int32 pts[2];
	Int32 imgW;
	Int32 imgH;
	OSInt i;
	void *session;

	NEW_CLASS(arri, Data::ArrayListInt64());
	lyrs->lyr->GetObjectIdsLatLon(arri, 0, view->GetLeftX(), view->GetTopY(), view->GetRightX(), view->GetBottomY(), false);
	session = lyrs->lyr->BeginGetObject();
	imgW = lyrs->img->GetWidth() >> 1;
	imgH = lyrs->img->GetHeight() >> 1;

	if ((i = arri->GetCount()) > 0)
	{
		*isLayerEmpty = false;
	}
	while (i-- > 0)
	{
		dobj = lyrs->lyr->GetObjectByIdN(session, arri->GetItem(i));
		view->IMapXYToScnXY(dobj->mapRate, dobj->points, pts, 1, 0, 0);
		img->DrawImagePt(lyrs->img, pts[0] - imgW, pts[1] - imgH);
		lyrs->lyr->ReleaseObject(session, dobj);
	}
	DEL_CLASS(arri);
}

void Map::SPCFile::DrawString(Media::DrawImage *img, SPCLayerStyle *lyrs, Int32 flags, Map::MapView *view, SPCFontStyle *fonts, SPCLabels *labels, Int32 maxLabels, Int32 *labelCnt, Bool *isLayerEmpty, Int32 *debug)
{
	void *nameArr;
	Data::ArrayListInt64 *arri;
	OSInt i;
	Int32 j;
	Map::DrawObject *dobj;
	Int32 scaleW;
	Int32 scaleH;
	Int32 pts[2];
	WChar *sptr;
	WChar lblStr[128];
	void *session;

	NEW_CLASS(arri, Data::ArrayListInt64());
	lyrs->lyr->GetObjectIdsLatLon(arri, &nameArr, view->GetLeftX(), view->GetTopY(), view->GetRightX(), view->GetBottomY(), false);
	session = lyrs->lyr->BeginGetObject();
	i = arri->GetCount();
	while (i-- > 0)
	{
		dobj = lyrs->lyr->GetObjectByIdN(session, arri->GetItem(i));
		if (dobj->nPoints > 0)
		{
			if (flags & SFLG_SMART)
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
				lyrs->lyr->GetString(sptr = lblStr, nameArr, i, 0);

				if (AddLabel(labels, maxLabels, labelCnt, sptr, maxSize, &dobj->points[maxPos << 1], 0, lyrs->lyr->GetLayerType(), lyrs->fontId, flags, view, debug, dobj->mapRate))
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
				lyrs->lyr->GetString(sptr = lblStr, nameArr, i, 0);
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

				if (view->InViewXY(pts[0] / dobj->mapRate, pts[1] / dobj->mapRate))
				{
					view->IMapXYToScnXY(dobj->mapRate, pts, pts, 1, 0, 0);

					if ((flags & SFLG_ROTATE) == 0)
						scaleW = scaleH = 0;
					DrawChars(img, sptr, pts[0], pts[1], scaleW, scaleH, &fonts[lyrs->fontId], (flags & SFLG_ALIGN) != 0);
				}
				lyrs->lyr->ReleaseObject(session, dobj);
			}
			else
			{
				Int64 lastPtX = 0;
				Int64 lastPtY = 0;
				Int32 *pointPos = dobj->points;
				lyrs->lyr->GetString(sptr = lblStr, nameArr, i, 0);
	//			sptr = (WChar*)arr->GetItem(i);

				j = dobj->nPoints;
				while (j--)
				{
					lastPtX += *pointPos++;
					lastPtY += *pointPos++;
				}

				pts[0] = (Int32)(lastPtX / dobj->nPoints);
				pts[1] = (Int32)(lastPtY / dobj->nPoints);
				if (view->InViewXY(pts[0] / dobj->mapRate, pts[1] / dobj->mapRate))
				{
					view->IMapXYToScnXY(dobj->mapRate, pts, pts, 1, 0, 0);
					DrawChars(img, sptr, pts[0], pts[1], 0, 0, &fonts[lyrs->fontId], (flags & SFLG_ALIGN) != 0);
				}
				lyrs->lyr->ReleaseObject(session, dobj);
			}
		}
	}
	lyrs->lyr->EndGetObject(session);
	lyrs->lyr->ReleaseNameArr(nameArr);
	DEL_CLASS(arri);
}

Int32 Map::SPCFile::NewLabel(SPCLabels *labels, Int32 maxLabel, Int32 *labelCnt, Int32 priority)
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


Bool Map::SPCFile::AddLabel(SPCLabels *labels, Int32 maxLabel, Int32 *labelCnt, WChar *labelt, Int32 nPoint, Int32 *points, Int32 priority, Int32 recType, Int32 fontStyle, Int32 flags, Map::MapView *view, Int32 *debug, Double mapRate)
{
	Int32 size;
	Int32 visibleSize;

	Int32 i;
	Int32 j;

	Int32 *pInt;

	Int32 scnX;
	Int32 scnY;
	Int32 found;

	Int32 left = Math::Double2Int(view->GetLeftX() / mapRate);
	Int32 right = Math::Double2Int(view->GetRightX() / mapRate);
	Int32 top = Math::Double2Int(view->GetTopY() / mapRate);
	Int32 bottom = Math::Double2Int(view->GetBottomY() / mapRate);
	Int32 mapPosX = Math::Double2Int(view->GetCenterX() / mapRate);
	Int32 mapPosY = Math::Double2Int(view->GetCenterY() / mapRate);

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
					labels[i].mapRate = mapRate;
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
						Int32* newArr = MemAlloc(Int32, newSize);
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
			labels[i].mapRate = mapRate;

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
				j = (nPoint - 1) << 1;
				if (flags & SFLG_ROTATE)
				{
					labels[i].scaleW = points[j] - points[j - 2];
					labels[i].scaleH = points[j + 1] - points[j - 1];
				}
				else
				{
					labels[i].scaleW = 0;
					labels[i].scaleH = 0;
				}

				labels[i].flags = flags;

				if (points[j] > points[j - 2])
				{
					thisPtX = lastPtX + visibleSize;
					thisPtY = points[j + 1] + MulDiv(thisPtX - points[j], points[j + 1] - points[j - 1], points[j] - points[j - 2]);
				}
				else if (points[j] < points[j - 2])
				{
					thisPtX = lastPtX - visibleSize;
					thisPtY = points[j + 1] + MulDiv(thisPtX - points[j], points[j + 1] - points[j - 1], points[j] - points[j - 2]);
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

void Map::SPCFile::SwapLabel(SPCLabels *mapLabels, Int32 index, Int32 index2)
{
	SPCLabels l;

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

Int32 Map::SPCFile::LabelOverlapped(Int32 *points, Int32 nPoints, Int32 tlx, Int32 tly, Int32 brx, Int32 bry)
{
	while (nPoints--)
	{
		if (points[(nPoints << 2) + 0] < brx && points[(nPoints << 2) + 2] > tlx && points[(nPoints << 2) + 1] < bry && points[(nPoints << 2) + 3] > tly) return 1;
	}

	return 0;
}

void Map::SPCFile::DrawLabels(Media::DrawImage *img, SPCLabels *labelArr, Int32 maxLabel, Int32 *labelCnt, Map::MapView *view, SPCFontStyle *fonts)
{
	////////////////////////////////////////////
	Int32 i;
	Int32 j;
	WChar *dest;
	WChar *lastLbl = 0;
	Double leftLon = view->GetLeftX();
	Double topLat = view->GetTopY();
	Double rightLon = view->GetRightX();
	Double bottomLat = view->GetBottomY();
	Int32 scnWidth = img->GetWidth();
	Int32 scnHeight = img->GetHeight();
//	Int32 scaleWidth = right - left;
//	Int32 scaleHeight = bottom - top;

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
		Double dscnPtX;
		Double dscnPtY;
		long thisPts[10];
		long thisCnt = 0;

		SPCLabels **labels;
		SPCLabels *lbl;

		const WChar *src;

		if (!(points = MemAlloc(Int32, *labelCnt * 4 * SRCHCNT)))
			return;

		labels = MemAlloc(SPCLabels*, *labelCnt);
		i = *labelCnt;
		while (i-- > 0)
		{
			labels[i] = &labelArr[i];
		}

		i = 0;
		j = *labelCnt;
		while (j--)
		{
			currPt = 0;
			i = 0;
			while (i < j)
			{
				if (labels[i]->priority < labels[i + 1]->priority)
				{
					lbl = labels[i];
					labels[i] = labels[i + 1];
					labels[i + 1] = lbl;
					//SwapLabel(labels, i, i + 1);
					currPt = 1;
				}
				else if (labels[i]->priority == labels[i + 1]->priority)
				{
					if (labels[i]->totalSize < labels[i + 1]->totalSize)
					{
						lbl = labels[i];
						labels[i] = labels[i + 1];
						labels[i + 1] = lbl;
						//SwapLabel(labels, i, i + 1);
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
			lbl = labels[i];
			GetCharsSize(img, szThis, lbl->label, &fonts[lbl->fontStyle], lbl->scaleW, lbl->scaleH);

			view->MapXYToScnXY(lbl->xPos / lbl->mapRate, lbl->yPos, &dscnPtX, &dscnPtY);
			scnPtX = Math::Double2Int(dscnPtX);
			scnPtY = Math::Double2Int(dscnPtY);

		//	labels[i].shapeType = 0;
			if (lbl->shapeType == 1)
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
					DrawChars(img, lbl->label, (tlx + brx) >> 1, (tly + bry) >> 1, 0, 0, &fonts[lbl->fontStyle], 0);

					points[(currPt << 2)] = tlx;
					points[(currPt << 2) + 1] = tly;
					points[(currPt << 2) + 2] = brx;
					points[(currPt << 2) + 3] = bry;
					currPt++;
				}
			}
			else if (lbl->shapeType == 3)
			{
				if (lastLbl)
				{
					src = lbl->label;
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
					src = lbl->label;
					while (*src++);
					dest = lastLbl = MemAlloc(WChar, src - lbl->label);
					src = lbl->label;
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

				if (!j && thisCnt < SRCHCNT)
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
						DrawChars(img, lbl->label, (tlx + brx) >> 1, (tly + bry) >> 1, lbl->scaleW, lbl->scaleH, &fonts[lbl->fontStyle], (lbl->flags & SFLG_ALIGN) != 0);

						points[(currPt << 2)] = tlx;
						points[(currPt << 2) + 1] = tly;
						points[(currPt << 2) + 2] = brx;
						points[(currPt << 2) + 3] = bry;
						currPt++;

						thisPts[thisCnt++] = (tlx + brx) >> 1;
						thisPts[thisCnt++] = (tly + bry) >> 1;
					}
					j = (thisCnt < SRCHCNT);
				}

				if (j)
				{
					Int32 k;
					Int32 l;
					Int32 *ptInt = lbl->points;

					k = 0;
					l = lbl->nPoints - 1;
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

						if (leftLon * lbl->mapRate < thisPtX && rightLon * lbl->mapRate > lastPtX && topLat * lbl->mapRate < thisPtY && bottomLat * lbl->mapRate > lastPtY)
						{
							Int32 left = Math::Double2Int(leftLon * lbl->mapRate);
							Int32 top = Math::Double2Int(topLat * lbl->mapRate);
							Int32 right = Math::Double2Int(rightLon * lbl->mapRate);
							Int32 bottom = Math::Double2Int(bottomLat * lbl->mapRate);
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

							lbl->scaleW = 0;
							lbl->scaleH = 0;

							scnPtX = (thisPtX + lastPtX) >> 1;
							scnPtY = (thisPtY + lastPtY) >> 1;
							if (lbl->flags & SFLG_ROTATE)
							{
								lbl->scaleW = ptInt[2] - ptInt[0];
								lbl->scaleH = ptInt[3] - ptInt[1];
								GetCharsSize(img, szThis, lbl->label, &fonts[lbl->fontStyle], lbl->scaleW, lbl->scaleH);
							}

							view->MapXYToScnXY(scnPtX / lbl->mapRate, scnPtY / lbl->mapRate, &dscnPtX, &dscnPtY);
							scnPtX = Math::Double2Int(dscnPtX);
							scnPtY = Math::Double2Int(dscnPtY);

							tlx = scnPtX - (szThis[0] >> 1);
							brx = tlx + szThis[0];
							tly = scnPtY - (szThis[1] >> 1);
							bry = tly + szThis[1];

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
								j = LabelOverlapped(points, currPt, tlx, tly, brx, bry);

								if (!j)
								{
									DrawChars(img, lbl->label, (tlx + brx) >> 1, (tly + bry) >> 1, lbl->scaleW, lbl->scaleH, &fonts[lbl->fontStyle], (lbl->flags & SFLG_ALIGN) != 0);

									points[(currPt << 2)] = tlx;
									points[(currPt << 2) + 1] = tly;
									points[(currPt << 2) + 2] = brx;
									points[(currPt << 2) + 3] = bry;
									currPt++;

									thisPts[thisCnt++] = (tlx + brx) >> 1;
									thisPts[thisCnt++] = (tly + bry) >> 1;
								}
								j = (thisCnt < SRCHCNT);
							}
						}

						ptInt += 2;
						k++;
					}
				}

				if (!j && false)
				{
					DrawChars(img, lbl->label,  (tlx + brx) >> 1, (tly + bry) >> 1, lbl->scaleW, lbl->scaleH, &fonts[lbl->fontStyle], (lbl->flags & SFLG_ALIGN) != 0);

					points[(currPt << 2)] = tlx;
					points[(currPt << 2) + 1] = tly;
					points[(currPt << 2) + 2] = brx;
					points[(currPt << 2) + 3] = bry;
					currPt++;
				}
			}
			else if (lbl->shapeType == 5)
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
					DrawChars(img, lbl->label, (tlx + brx) >> 1, (tly + bry) >> 1, 0, 0, &fonts[lbl->fontStyle], 0);

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
		MemFree(labels);
	}

	FreeLabels(labelArr, maxLabel, labelCnt);
	if (lastLbl)
		MemFree(lastLbl);
}

void Map::SPCFile::FreeLabels(SPCLabels *labels, Int32 maxLabel, Int32 *labelCnt)
{
	Int32 i;
	i = *labelCnt;
	while (i-- > 0)
	{
		Text::StrDelNew(labels[i].label);
		if (labels[i].points)
			MemFree(labels[i].points);
	}
}

Map::SPCFile::SPCFile(WChar *fileName, Media::DrawEngine *eng, Data::ArrayList<Map::SPDLayer*> *layerList)
{
	WChar layerName[512];
	WChar *sptr;
	IO::FileStream *fstm;
	Int32 i;
	Int32 j;
	Int32 k;
	SPCLayerStyle *currLayer;
	SPCLineLayer *lineLyr;

	Int32 dataBuff[128];
	Int32 nDir;
	Int32 nGrps;
	Int32 nLayers;

	WChar **dirs;
	WChar **grps;

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
		fstm->Read((UInt8*)dataBuff, 28);
		nDir = dataBuff[0];
		nGrps = dataBuff[1];
		this->nFont = dataBuff[2];
		this->nLine = dataBuff[3];
		nLayers = dataBuff[4];
		this->nStr = dataBuff[5];
		bgColor = ToColor(dataBuff[6]);

		dirs = MemAlloc(WChar*, nDir);
		dataBuff[1] = 0;
		i = 0;
		while (i < nDir)
		{
			fstm->Read((UInt8*)dataBuff, 6);
			j = (Int32)fstm->GetPosition();
			fstm->Seek(IO::SeekableStream::ST_BEGIN, dataBuff[0]);
			dirs[i] = MemAlloc(WChar, (dataBuff[1] >> 1) + 1);
			fstm->Read((UInt8*)dirs[i], dataBuff[1]);
			fstm->Seek(IO::SeekableStream::ST_BEGIN, j);
			dirs[i][dataBuff[1] >> 1] = 0;

			i++;
		}

		grps = MemAlloc(WChar*, nGrps);
		dataBuff[1] = 0;
		i = 0;
		while (i < nGrps)
		{
			fstm->Read((UInt8*)dataBuff, 6);
			j = (Int32)fstm->GetPosition();
			fstm->Seek(IO::SeekableStream::ST_BEGIN, dataBuff[0]);
			grps[i] = MemAlloc(WChar, (dataBuff[1] >> 1) + 1);
			fstm->Read((UInt8*)grps[i], dataBuff[1]);
			fstm->Seek(IO::SeekableStream::ST_BEGIN, j);
			grps[i][dataBuff[1] >> 1] = 0;

			i++;
		}

		this->fonts = MemAlloc(SPCFontStyle, this->nFont);
		i = 0;
		while (i < this->nFont)
		{
			fstm->Read((UInt8*)dataBuff, 24);
			this->fonts[i].fontSize = ((Int16*)dataBuff)[3];
			this->fonts[i].fontColor = ToColor(dataBuff[2]);
			this->fonts[i].buffSize = dataBuff[3];
			this->fonts[i].buffColor = ToColor(dataBuff[4]);
			this->fonts[i].fontStyle = dataBuff[5];
#ifdef NoPaid
			this->fonts[i].fontColor = 0xff7f7f7f;
#endif
			j = (Int32)fstm->GetPosition();
			fstm->Seek(IO::SeekableStream::ST_BEGIN, dataBuff[0]);
			this->fonts[i].fontName = MemAlloc(WChar, (((UInt16*)dataBuff)[2] >> 1) + 1);
			fstm->Read((UInt8*)this->fonts[i].fontName, ((UInt16*)dataBuff)[2]);
			fstm->Seek(IO::SeekableStream::ST_BEGIN, j);
			((WChar*)this->fonts[i].fontName)[((UInt16*)dataBuff)[2] >> 1] = 0;

			i++;
		}

		this->lines = MemAlloc(SPCLineStyle, this->nLine);
		i = 0;
		while (i < this->nLine)
		{
			NEW_CLASS(this->lines[i].layers, Data::ArrayList<SPCLineLayer*>());
			fstm->Read((UInt8*)dataBuff, 8);
			this->lines[i].lineStyle = dataBuff[0];
			j = dataBuff[1];
			while (j-- > 0)
			{
				lineLyr = MemAlloc(SPCLineLayer, 1);
				fstm->Read((UInt8*)dataBuff, 12);
				lineLyr->thickness = dataBuff[0];
				lineLyr->lineColor = ToColor(dataBuff[1]);
				lineLyr->othersLen = dataBuff[2];
				if (lineLyr->othersLen > 0)
				{
					lineLyr->others = MemAlloc(UInt8, k = lineLyr->othersLen);
					fstm->Read((UInt8*)dataBuff, lineLyr->othersLen << 2);
					while (k-- > 0)
					{
						lineLyr->others[k] = (UInt8)dataBuff[k];
					}
				}
				else
				{
					lineLyr->others = 0;
				}
				this->lines[i].layers->Add(lineLyr);
			}
			i++;
		}

		NEW_CLASS(this->drawList, Data::ArrayList<SPCLayerStyle*>());
		i = 0;
		while (i < nLayers)
		{
			dataBuff[2] = 0;
			fstm->Read((UInt8*)dataBuff, 10);
			fstm->Read((UInt8*)&dataBuff[3], 32);
			currLayer = MemAlloc(SPCLayerStyle, 1);
			currLayer->drawType = dataBuff[3];
			currLayer->minScale = dataBuff[8];
			currLayer->maxScale = dataBuff[9];
			currLayer->style = dataBuff[10];
			currLayer->fillColor = ToColor(dataBuff[7]);
			currLayer->lineId = dataBuff[6];
			currLayer->fontId = dataBuff[5];
			currLayer->grpId = dataBuff[4];
#ifdef _WIN32_WCE
			if (dirs[dataBuff[0]][1] == ':')
				sptr = Text::StrConcat(layerName, &dirs[dataBuff[0]][2]);
			else
				sptr = Text::StrConcat(layerName, dirs[dataBuff[0]]);
#else
			sptr = Text::StrConcat(layerName, dirs[dataBuff[0]]);
#endif
			*sptr++ = '\\';
			j = (Int32)fstm->GetPosition();
			fstm->Seek(IO::SeekableStream::ST_BEGIN, dataBuff[1]);
			fstm->Read((UInt8*)sptr, dataBuff[2]);
			fstm->Seek(IO::SeekableStream::ST_BEGIN, j);
			sptr[dataBuff[2] >> 1] = 0;
			NEW_CLASS(currLayer->lyr, Map::SPDLayer(layerName));

			currLayer->img = 0;
			if (((Map::SPDLayer*)currLayer->lyr)->IsError() == 0)
			{
                this->drawList->Add(currLayer);
			}
			else
			{
				DEL_CLASS((Map::SPDLayer*)currLayer->lyr);
				MemFree(currLayer);
			}
			i++;
		}

		i = nGrps;
		while (i-- > 0)
		{
			MemFree(grps[i]);
		}
		MemFree(grps);

		i = nDir;
		while (i-- > 0)
		{
			MemFree(dirs[i]);
		}
		MemFree(dirs);
	}
	DEL_CLASS(fstm);
}

Map::SPCFile::~SPCFile()
{
	Int32 i;
	OSInt j;
	Map::SPCLayerStyle *currLyr;

	if (this->lines)
	{
		i = nLine;
		while (i-- > 0)
		{
			j = this->lines[i].layers->GetCount();
			while (j-- > 0)
			{
				Map::SPCLineLayer *thisLine = (Map::SPCLineLayer*)this->lines[i].layers->GetItem(j);
				if (thisLine->others)
				{
					MemFree(thisLine->others);
				}
				MemFree(thisLine);
			}
			DEL_CLASS(this->lines[i].layers);
		}
		MemFree(this->lines);
		this->lines = 0;
	}
	if (this->fonts)
	{
		i = this->nFont;
		while (i-- > 0)
		{
			MemFree((WChar*)this->fonts[i].fontName);
		}
		MemFree(this->fonts);
	}

	if (this->drawList)
	{
		OSInt i = this->drawList->GetCount();
		while (i-- > 0)
		{
			currLyr = (Map::SPCLayerStyle*)this->drawList->GetItem(i);
			if (currLyr->img)
			{
				this->drawEng->DeleteImage(currLyr->img);
			}
			if (currLyr->lyr)
			{
				DEL_CLASS((SPDLayer*)currLyr->lyr);
			}
			MemFree(currLyr);
		}
		DEL_CLASS(this->drawList);
	}
	DEL_CLASS(drawMut);
}

Bool Map::SPCFile::IsError()
{
	return this->drawList == 0;
}

Media::DrawPen *Map::SPCFile::CreatePen(Media::DrawImage *img, Int32 lineStyle, Int32 lineLayer, Int32 scale, Int32 maxScale)
{
	if (lineStyle < 0 || lineStyle >= this->nLine)
	{
		return 0;
	}
	Map::SPCLineLayer *thisLine = (Map::SPCLineLayer*)this->lines[lineStyle].layers->GetItem(lineLayer);
	if (thisLine == 0)
		return 0;

	if (thisLine->others)
	{
		Media::DrawPen *pen = img->NewPenARGB(thisLine->lineColor, thisLine->thickness, thisLine->others, thisLine->othersLen);
		return pen;
	}
	else if (this->lines[lineStyle].lineStyle & 1)
	{
		Int32 i;
		i = maxScale / scale;
		if (i <= 0)
			i = 1;
		i += thisLine->thickness;
		return img->NewPenARGB(thisLine->lineColor, i, 0, 0);
	}
	else
	{
		return img->NewPenARGB(thisLine->lineColor, thisLine->thickness, 0, 0);
	}

	return 0;
}

Int32 Map::SPCFile::DrawMap(Media::DrawImage *img, Map::MapView *view, Bool *isLayerEmpty)
{
	Int32 index;
	Int32 index2;
	if (this->drawList == 0) return 0;
	OSInt layerCnt = this->drawList->GetCount();
	Data::ArrayListInt64 *arr;
	Map::SPCFontStyle *myArrs;
	Map::IMapDrawLayer *lyr;
	Map::SPCLayerStyle *lyrs;
	Map::DrawObject *dobj;
	Media::DrawBrush *brush;
	Media::DrawPen *pen;
	OSInt i;
	Int32 j;
	Int32 k;
	Int64 lastId;
	Int64 thisId;
	Int32 thisScale;
	Int32 labelCnt = 0;
	Int32 maxLabel = this->nStr;
	*isLayerEmpty = true;
	Map::SPCLabels *labels = MemAlloc(Map::SPCLabels, maxLabel);

	Double time;
	Double time2;

	NEW_CLASS(arr, Data::ArrayListInt64());
	thisScale = view->GetScale();

	brush = img->NewBrushARGB(this->bgColor);
	img->DrawRect(0, 0, img->GetWidth(), img->GetHeight(), 0, brush);
	img->DelBrush(brush);

	myArrs = MemAlloc(Map::SPCFontStyle, this->nFont);
	i = this->nFont;
	while (i-- > 0)
	{
		myArrs[i].buffColor = this->fonts[i].buffColor;
		myArrs[i].buffSize = this->fonts[i].buffSize;
		myArrs[i].fontColor = this->fonts[i].fontColor;
		myArrs[i].fontName = Text::StrCopyNew(this->fonts[i].fontName);
		myArrs[i].fontSize = this->fonts[i].fontSize;
		myArrs[i].fontStyle = this->fonts[i].fontStyle;
		myArrs[i].font = img->NewFontW(myArrs[i].fontName, myArrs[i].fontSize, myArrs[i].fontStyle);
//		myArrs[i].font = img->NewFontW(L"MS Ui Gothic", myArrs[i].fontSize, myArrs[i].fontStyle);
		myArrs[i].brush = img->NewBrushARGB(myArrs[i].fontColor);
		if (myArrs[i].buffSize > 0)
		{
			myArrs[i].brushBuff = img->NewBrushARGB(myArrs[i].buffColor);
		}
		else
		{
			myArrs[i].brushBuff = 0;
		}
	}

	Manage::HiResClock clk;
	index = 0;
	while (index < layerCnt)
	{
		void *session;
		lyrs = (Map::SPCLayerStyle*)this->drawList->GetItem(index++);
		if (thisScale > lyrs->minScale && thisScale <= lyrs->maxScale)
		{
			if (lyrs->style & 1)
			{
				if (lyrs->drawType == 2)
				{
					lyr = lyrs->lyr;
					if (lyr)
					{
						arr->Clear();
						lyr->GetObjectIdsLatLon(arr, 0, view->GetLeftX(), view->GetTopY(), view->GetRightX(), view->GetBottomY(), false);

						pen = CreatePen(img, lyrs->lineId, 0, view->GetScale(), lyrs->maxScale);
						brush = img->NewBrushARGB(lyrs->fillColor);

						session = lyr->BeginGetObject();
						if ((i = arr->GetCount()) > 0)
						{
							*isLayerEmpty = false;
						}
						lastId = -1;
						while (i-- > 0)
						{
							thisId = arr->GetItem(i);
							if (thisId != lastId)
							{
								lastId = thisId;
								dobj = lyr->GetObjectByIdN(session, thisId);
								view->IMapXYToScnXY(dobj->mapRate, dobj->points, dobj->points, dobj->nPoints, 0, 0);
								k = dobj->nParts;
								j = 1;
								while (j < k)
								{
									dobj->parts[j - 1] = dobj->parts[j] - dobj->parts[j - 1];
									j++;
								}
								dobj->parts[k - 1] = dobj->nPoints - dobj->parts[k - 1];

								img->DrawPolyPolygonI(dobj->points, dobj->parts, dobj->nParts, pen, brush);

								lyr->ReleaseObject(session, dobj);
							}
						}
						img->DelPen(pen);
						img->DelBrush(brush);

						if (pen)
						{
							index2 = 1;
							while ((pen = CreatePen(img, lyrs->lineId, index2++, view->GetScale(), lyrs->maxScale)) != 0)
							{
								i = arr->GetCount();
								lastId = -1;
								while (i-- > 0)
								{
									thisId = arr->GetItem(i);
									if (thisId != lastId)
									{
										lastId = thisId;
										dobj = lyr->GetObjectByIdN(session, thisId);
										view->IMapXYToScnXY(dobj->mapRate, dobj->points, dobj->points, dobj->nPoints, 0, 0);
										k = dobj->nParts;
										j = 1;
										while (j < k)
										{
											dobj->parts[j - 1] = dobj->parts[j] - dobj->parts[j - 1];
											j++;
										}
										dobj->parts[k - 1] = dobj->nPoints - dobj->parts[k - 1];

										img->DrawPolyPolygonI(dobj->points, dobj->parts, dobj->nParts, pen, 0);

										lyr->ReleaseObject(session, dobj);
									}
								}
								img->DelPen(pen);
							}
						}
						lyr->EndGetObject(session);
					}
				}
				else if (lyrs->drawType == 1)
				{
					lyr = lyrs->lyr;
					if (lyr)
					{
						arr->Clear();
						lyr->GetObjectIdsLatLon(arr, 0, view->GetLeftX(), view->GetTopY(), view->GetRightX(), view->GetBottomY(), false);

						pen = CreatePen(img, lyrs->lineId, 0, view->GetScale(), lyrs->maxScale);
						session = lyr->BeginGetObject();

						if ((i = arr->GetCount()) > 0)
						{
							*isLayerEmpty = false;
						}
						lastId = -1;
						while (i-- > 0)
						{
							thisId = arr->GetItem(i);
							if (thisId != lastId)
							{
								lastId = thisId;
								dobj = lyr->GetObjectByIdN(session, thisId);
								view->IMapXYToScnXY(dobj->mapRate, dobj->points, dobj->points, dobj->nPoints, 0, 0);
								k = dobj->nParts;
								j = 1;
								while (j < k)
								{
									img->DrawPolylineI(&dobj->points[dobj->parts[j-1] << 1], dobj->parts[j] - dobj->parts[j - 1], pen);
									j++;
								}
								img->DrawPolylineI(&dobj->points[dobj->parts[k-1] << 1], dobj->nPoints - dobj->parts[k - 1], pen);

								lyr->ReleaseObject(session, dobj);
							}
						}
						img->DelPen(pen);

						if (pen)
						{
							index2 = 1;
							while ((pen = CreatePen(img, lyrs->lineId, index2++, view->GetScale(), lyrs->maxScale)) != 0)
							{
								i = arr->GetCount();
								lastId = -1;
								while (i-- > 0)
								{
									thisId = arr->GetItem(i);
									if (thisId != lastId)
									{
										lastId = thisId;
										dobj = lyr->GetObjectByIdN(session, thisId);
										view->IMapXYToScnXY(dobj->mapRate, dobj->points, dobj->points, dobj->nPoints, 0, 0);
										k = dobj->nParts;
										j = 1;
										while (j < k)
										{
											img->DrawPolylineI(&dobj->points[dobj->parts[j-1] << 1], dobj->parts[j] - dobj->parts[j - 1], pen);
											j++;
										}
										img->DrawPolylineI(&dobj->points[dobj->parts[k-1] << 1], dobj->nPoints - dobj->parts[k - 1], pen);

										lyr->ReleaseObject(session, dobj);
									}
								}
								img->DelPen(pen);
							}
						}
						lyr->EndGetObject(session);
					}
				}
				else if (lyrs->drawType == 0)
				{
					DrawPoints(img, lyrs, view, isLayerEmpty);
				}
			}
			if (lyrs->style & 2)
			{
				Int32 flags;
				flags = 0;
				if (lyrs->style & 4)
					flags |= SFLG_SMART;
				if (lyrs->style & 8)
					flags |= SFLG_ROTATE;
				if (lyrs->style & 16)
					flags |= SFLG_ALIGN;
				DrawString(img, lyrs, flags, view, myArrs, labels, maxLabel, &labelCnt, isLayerEmpty, &flags);
			}
		}
	}
	time2 = clk.GetTimeDiff();

	clk.Start();
	DrawLabels(img, labels, maxLabel, &labelCnt, view, myArrs);
//	FreeLabels(labels, maxLabel, &labelCnt);
	time = clk.GetTimeDiff();

	i = this->nFont;
	while (i-- > 0)
	{
		Text::StrDelNew(myArrs[i].fontName);
		img->DelFont(myArrs[i].font);
		img->DelBrush(myArrs[i].brush);
		if (myArrs[i].brushBuff)
			img->DelBrush(myArrs[i].brushBuff);
	}
	MemFree(myArrs);
	MemFree(labels);

	DEL_CLASS(arr);
	return Math::Double2Int(time * 1000);
}

Int32 Map::SPCFile::GetBGColor()
{
	return this->bgColor;
}

WChar *Map::SPCFile::GetPGLabel(WChar *buff, Int32 mapX, Int32 mapY, Double mapRate)
{
	Int32 index;
	Double outLat;
	Double outLon;
	Map::SPCLayerStyle *lyrs;
	WChar *outPtr;
	OSInt layerCnt = this->drawList->GetCount();
	index = 0;
	while (index < layerCnt)
	{
		lyrs = (Map::SPCLayerStyle*)this->drawList->GetItem(index++);
		if (lyrs->drawType == 2 && (lyrs->style & 32) != 0)
		{
			if ((outPtr = lyrs->lyr->GetPGLabelLatLon(buff, mapY / mapRate, mapX / mapRate, &outLat, &outLon, 0)) != 0)
				return outPtr;
		}
	}
	return 0;
}

WChar *Map::SPCFile::GetPLLabel(WChar *buff, Int32 mapX, Int32 mapY, Int32 *lineX, Int32 *lineY, Double mapRate)
{
	Double lat = mapY / mapRate;
	Double lon = mapX / mapRate;
	Int32 index;
	WChar nearStr[256];
	Double nearLat;
	Double nearLon;
	Double nearDist;
	WChar thisStr[256];
	Double thisLat;
	Double thisLon;
	Double thisDist;
	Map::SPCLayerStyle *lyrs;
	OSInt layerCnt = this->drawList->GetCount();
	index = 0;
	nearLat = 0;
	nearLon = 0;
	nearDist = 0x7fffffff;
	while (index < layerCnt)
	{
		lyrs = (Map::SPCLayerStyle*)this->drawList->GetItem(index++);
		if (lyrs->drawType == 1 && (lyrs->style & 32) != 0)
		{
			if (lyrs->lyr->GetPLLabelLatLon(thisStr, lat, lon, &thisLat, &thisLon, 0))
			{
				thisDist = (thisLat - lat) * (thisLat - lat) + (thisLon - lon) * (thisLon - lon);
				if (thisDist < nearDist)
				{
					Text::StrConcat(nearStr, thisStr);
					nearDist = thisDist;
					nearLat = thisLat;
					nearLon = thisLon;
				}
			}
		}
	}

	if (nearLat == 0 && nearLon == 0)
		return 0;
	else
	{
		*lineX = Math::Double2Int(nearLon * mapRate);
		*lineY = Math::Double2Int(nearLat * mapRate);
		return Text::StrConcat(buff, nearStr);
	}
}

Int32 Map::SPCFile::GetLastError()
{
	return this->debug;
}
void Map::SPCFile::ReleaseLayers(Data::ArrayList<Map::SPDLayer*> *layerList)
{
	Map::SPDLayer *lyr;
	OSInt i;
	i = layerList->GetCount();
	while (i-- > 0)
	{
		lyr = layerList->GetItem(i);
		DEL_CLASS(lyr);
	}
}
