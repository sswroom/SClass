#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/Sort/ArtificialQuickSortFunc.h"
#include "Map/DrawMapRenderer.h"
#include "Math/Math.h"
#include "Math/GeometryTool.h"
#include "Math/Geometry/VectorImage.h"
#include "Media/Resizer/LanczosResizer8_C8.h"
#include "Text/MyString.h"

#include <stdio.h>

#define IsDoorNum(a) Text::StrIsInt32(a.v)
#define LBLMINDIST 150

UOSInt Map::DrawMapRenderer::NewLabel(Map::DrawMapRenderer::MapLabels *labels, UOSInt maxLabel, UOSInt *labelCnt, Int32 priority)
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

void Map::DrawMapRenderer::SwapLabel(MapLabels *mapLabels, UOSInt index, UOSInt index2)
{
	MapLabels l;
	MemCopyNO(&l, &mapLabels[index], sizeof(MapLabels));
	MemCopyNO(&mapLabels[index], &mapLabels[index2], sizeof(MapLabels));
	MemCopyNO(&mapLabels[index2], &l, sizeof(MapLabels));
}

Bool Map::DrawMapRenderer::LabelOverlapped(Math::RectAreaDbl *points, UOSInt nPoints, Math::RectAreaDbl rect)
{
	while (nPoints--)
	{
		if (rect.OverlapOrTouch(points[nPoints]))
			return true;
	}

	return false;
}

Bool Map::DrawMapRenderer::AddLabel(MapLabels *labels, UOSInt maxLabel, UOSInt *labelCnt, Text::CString label, UOSInt nPoints, Math::Coord2DDbl *points, Int32 priority, Map::DrawLayerType recType, UOSInt fontStyle, Int32 flags, Map::MapView *view, OSInt xOfst, OSInt yOfst, Map::MapEnv::FontType fontType)
{
	Double size;
	Double visibleSize;

	UOSInt i;
	UOSInt j;

	Math::Coord2DDbl *ptPtr;
	Math::Coord2DDbl scnPos;
	Double scnSqrLen;
	Int32 found;
	if (label.leng == 0)
		return false;

	Math::RectAreaDbl rect = view->GetVerticalRect();
	Math::Coord2DDbl mapPos = view->GetCenter();

	if (recType == Map::DRAW_LAYER_POINT || recType == Map::DRAW_LAYER_POINT3D) //Point
	{
		found = 0;
		i = 0;
		while (i < *labelCnt)
		{
			if (recType == labels[i].layerType)
			{
				if (label.Equals(labels[i].label))
				{
					found = 1;

					ptPtr = points;
					j = nPoints;
					while (j--)
					{
						scnPos = mapPos - ptPtr[0];
						scnPos = scnPos * scnPos;
						scnSqrLen = scnPos.x + scnPos.y;
						if (scnSqrLen < labels[i].currSize)
						{
							labels[i].pos = ptPtr[0];
							labels[i].fontStyle = fontStyle;
							labels[i].fontType = fontType;
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
			i = 0;
			ptPtr = points;
			j = nPoints;
			while (j--)
			{
				if (rect.ContainPt(ptPtr->x, ptPtr->y))
				{
					found = 1;

					i = NewLabel(labels, maxLabel, labelCnt, priority);
					if (i == (UOSInt)-1)
					{
						return false;
					}

					labels[i].label = Text::String::New(label).Ptr();
					labels[i].pos = ptPtr[0];
					labels[i].fontStyle = fontStyle;
					labels[i].fontType = fontType;
					labels[i].scaleW = 0;
					labels[i].scaleH = 0;
					labels[i].priority = priority;

					scnPos = mapPos - ptPtr[0];
					scnPos = scnPos * scnPos;
					scnSqrLen = scnPos.x + scnPos.y;

					labels[i].currSize = scnSqrLen;
					labels[i].totalSize = UOSInt2Double(nPoints);
					labels[i].nPoints = 0;
					labels[i].layerType = recType;
					labels[i].points = 0;
					labels[i].flags = flags;
					labels[i].xOfst = xOfst;
					labels[i].yOfst = yOfst;

					ptPtr = points;
					j = nPoints;
					while (j--)
					{
						scnPos = mapPos - ptPtr[0];
						scnPos = scnPos * scnPos;
						scnSqrLen = scnPos.x + scnPos.y;
						if (scnSqrLen < labels[i].currSize)
						{
							labels[i].pos = ptPtr[0];
							labels[i].fontStyle = fontStyle;
							labels[i].fontType = fontType;
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
				ptPtr += 1;
			}
		}
		return false;
	}
	else if (recType == Map::DRAW_LAYER_POLYLINE || recType == Map::DRAW_LAYER_POLYLINE3D) //lines
	{
		Double tmp;
		Math::Coord2DDbl lastPt = points[0];
		Math::Coord2DDbl thisPt;
		UOSInt i;
		Int32 toUpdate;

		////////////////////////////
		Double left = rect.tl.x;
		Double top = rect.tl.y;
		Double right = rect.br.x;
		Double bottom = rect.br.y;

		visibleSize = 0;
		size = 0;

		i = 1;
		while (i < nPoints)
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
		UOSInt foundInd;
		Math::Coord2DDbl startPt;
		Math::Coord2DDbl endPt;
		startPt = points[0];
		endPt = points[nPoints - 1];
		i = 0;

		while (i < *labelCnt)
		{
			if (recType == labels[i].layerType)
			{
				if (label.Equals(labels[i].label))
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
						UOSInt newSize = labels[i].nPoints + nPoints - 1;
						Math::Coord2DDbl* newArr = MemAllocA(Math::Coord2DDbl, newSize);

						MemCopyNO(newArr, points, nPoints << 4);
						MemCopyNO(&newArr[nPoints], &labels[i].points[1], (labels[i].nPoints - 1) << 4);

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
						UOSInt newSize = labels[i].nPoints + nPoints - 1;
						Math::Coord2DDbl* newArr = MemAllocA(Math::Coord2DDbl, newSize);

						MemCopyNO(newArr, labels[i].points, labels[i].nPoints << 4);
						MemCopyNO(&newArr[labels[i].nPoints], &points[1], (nPoints - 1) << 4);

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
						UOSInt newSize = labels[i].nPoints + nPoints - 1;
						Math::Coord2DDbl* newArr = MemAllocA(Math::Coord2DDbl, newSize);
						UOSInt k;
						UOSInt l;
						l = 0;
						k = labels[i].nPoints;
						while (k-- > 1)
						{
							newArr[l] = labels[i].points[k];
							l++;
						}
						MemCopyNO(&newArr[l], points, nPoints << 4);

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
						UOSInt newSize = labels[i].nPoints + nPoints - 1;
						Math::Coord2DDbl* newArr = MemAllocA(Math::Coord2DDbl, newSize);
						MemCopyNO(newArr, labels[i].points, labels[i].nPoints << 4);
						UOSInt k;
						UOSInt l;
						l = labels[i].nPoints;
						k = nPoints - 1;
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

			if (i == (UOSInt)-1)
			{
				return false;
			}

			labels[i].layerType = recType;
			labels[i].fontStyle = fontStyle;
			labels[i].fontType = fontType;
			labels[i].totalSize = size;
			labels[i].currSize = visibleSize;
			labels[i].flags = flags;

			labels[i].label = Text::String::New(label).Ptr();
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
			j = labels[i].nPoints = nPoints;
			if (labels[i].points)
				MemFreeA(labels[i].points);
			labels[i].points = ptPtr = MemAllocA(Math::Coord2DDbl, nPoints);
			MemCopyNO(ptPtr, points, j << 4);
			return true;
		}
		return false;
	}
	else if (recType == Map::DRAW_LAYER_POLYGON) //polygon
	{
		if (nPoints <= 2)
			return false;

		found = 0;

		if (IsDoorNum(label))
		{
		}
		else
		{
			i = 0;
			while (i < *labelCnt)
			{
				if (recType == labels[i].layerType)
				{
					if (label.Equals(labels[i].label))
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
			////////////////////////////
			Double left = rect.tl.x;
			Double top = rect.tl.y;
			Double right = rect.br.x;
			Double bottom = rect.br.y;

			Math::Coord2DDbl lastPt;
			Math::Coord2DDbl thisPt;
			Math::Coord2DDbl thisT;
			Math::Coord2DDbl* outPts;
			UOSInt outPtCnt;
			Double sum;
			Math::Coord2DDbl sumVal;

			Math::Coord2DDbl *tmpPts;
			tmpPts = MemAllocA(Math::Coord2DDbl, nPoints << 1);
			outPtCnt = Math::GeometryTool::BoundPolygonY(points, nPoints, tmpPts, top, bottom, {0, 0});
			outPts = MemAllocA(Math::Coord2DDbl, nPoints << 1);
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
				UOSInt finalCnt;
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
						min = min.Min(thisPt);
						max = max.Max(thisPt);
						finalCnt++;
					}
					lastPt = thisPt;
					i++;
				}

				thisPt = outPts[outPtCnt - 1];
				sumVal += finalPts[finalCnt] = thisPt;
				finalCnt++;
				min = min.Min(thisPt);
				max = max.Max(thisPt);

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
							min = thisPt;
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

				thisPt = ((max + min) * 0.5);

				i = NewLabel(labels, maxLabel, labelCnt, priority);
				if (i == (UOSInt)-1)
				{
					MemFreeA(outPts);
					return false;
				}

				labels[i].label = Text::String::New(label).Ptr();
				labels[i].pos = thisPt;
				labels[i].fontStyle = fontStyle;
				labels[i].fontType = fontType;
				labels[i].scaleW = 0;
				labels[i].scaleH = 0;
				labels[i].priority = priority;

				labels[i].currSize = UOSInt2Double(outPtCnt);
				labels[i].totalSize = UOSInt2Double(outPtCnt);
				labels[i].nPoints = outPtCnt;
				labels[i].layerType = recType;
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

void Map::DrawMapRenderer::DrawLabels(NotNullPtr<Map::DrawMapRenderer::DrawEnv> denv)
{
	UOSInt i;
	UOSInt j;
	Bool overlapped;
	Text::String *lastLbl = 0;
//	Double leftLon = denv->view->GetLeftX();
//	Double topLat = denv->view->GetTopY();
//	Double rightLon = denv->view->GetRightX();
//	Double bottomLat = denv->view->GetBottomY();
	OSInt scnWidth = (OSInt)denv->img->GetWidth();
	OSInt scnHeight = (OSInt)denv->img->GetHeight();
//	Int32 scaleWidth = right - left;
//	Int32 scaleHeight = bottom - top;

	if (denv->labelCnt)
	{
		Math::Coord2DDbl szThis;
		UOSInt currPt;

		Math::RectAreaDbl rect;
		Math::Coord2DDbl scnPt;
		Math::Coord2DDbl dscnPos;
		OSInt thisPts[10];
		Int32 thisCnt = 0;

		i = 0;
		j = denv->labelCnt;
		while (j--)
		{
			currPt = 0;
			i = 0;
			while (i < j)
			{
				if (denv->labels[i].priority < denv->labels[i + 1].priority)
				{
					SwapLabel(denv->labels, i, i + 1);
					currPt = 1;
				}
				else if (denv->labels[i].priority == denv->labels[i + 1].priority)
				{
					if (denv->labels[i].totalSize < denv->labels[i + 1].totalSize)
					{
						SwapLabel(denv->labels, i, i + 1);
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

		currPt = denv->objCnt;
		i = 0;
		while (i < denv->labelCnt && currPt < denv->maxLabels)
		{

		//	labels[i].shapeType = 0;
			if (denv->labels[i].layerType == Map::DRAW_LAYER_POINT || denv->labels[i].layerType == Map::DRAW_LAYER_POINT3D)
			{
				GetCharsSize(denv, &szThis, denv->labels[i].label->ToCString(), denv->labels[i].fontType, denv->labels[i].fontStyle, 0, 0);//labels[i].scaleW, labels[i].scaleH);
				dscnPos = denv->view->MapXYToScnXY(denv->labels[i].pos);
				scnPt = dscnPos;

				overlapped = true;
				if (denv->labels[i].xOfst == 0)
				{
					rect.tl = scnPt - (szThis * 0.5);
					rect.br = rect.tl + szThis;

					overlapped = LabelOverlapped(denv->objBounds, currPt, rect);
				}
				if (overlapped)
				{
					rect.tl.x = scnPt.x + 1 + OSInt2Double(denv->labels[i].xOfst >> 1);
					rect.tl.y = scnPt.y - (szThis.y * 0.5);
					rect.br = rect.tl + szThis;

					overlapped = LabelOverlapped(denv->objBounds, currPt, rect);
				}
				if (overlapped)
				{
					rect.tl.x = scnPt.x - szThis.x - 1 - OSInt2Double(denv->labels[i].xOfst >> 1);
					rect.tl.y = scnPt.y - (szThis.y * 0.5);
					rect.br = rect.tl + szThis;

					overlapped = LabelOverlapped(denv->objBounds, currPt, rect);
				}
				if (overlapped)
				{
					rect.tl.x = scnPt.x - (szThis.x * 0.5);
					rect.tl.y = scnPt.y - szThis.y - 1 - OSInt2Double(denv->labels[i].yOfst >> 1);
					rect.br = rect.tl + szThis;

					overlapped = LabelOverlapped(denv->objBounds, currPt, rect);
				}
				if (overlapped)
				{
					rect.tl.x = scnPt.x - (szThis.x * 0.5);
					rect.tl.y = scnPt.y + 1 + OSInt2Double(denv->labels[i].yOfst >> 1);
					rect.br = rect.tl + szThis;

					overlapped = LabelOverlapped(denv->objBounds, currPt, rect);
				}

				if (!overlapped)
				{
					Math::Coord2DDbl center = rect.GetCenter();
					DrawChars(denv, denv->labels[i].label->ToCString(), center, 0, 0, denv->labels[i].fontType, denv->labels[i].fontStyle, 0);

					denv->objBounds[currPt] = rect;
					currPt++;
				}
			}
			else if (denv->labels[i].layerType == Map::DRAW_LAYER_POLYLINE || denv->labels[i].layerType == Map::DRAW_LAYER_POLYLINE3D)
			{

				if (lastLbl)
				{
					if (!lastLbl->Equals(denv->labels[i].label))
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
					lastLbl = denv->labels[i].label->Clone().Ptr();
				}
				else
				{
				//	MessageBoxW(NULL, L"Test", lastLbl, MB_OK);
				}

				Math::Coord2D<Int32> *points = MemAlloc(Math::Coord2D<Int32>, denv->labels[i].nPoints);
				denv->view->MapXYToScnXY(denv->labels[i].points, points, denv->labels[i].nPoints, Math::Coord2D<Int32>(0, 0));
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
				UOSInt k;
				Bool hasPoint;

				j = denv->labels[i].nPoints - 1;
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

					if (lastX > scnWidth)
					{
						if (thisX > scnWidth)
						{
							continue;
						}
						else
						{
							lastY = thisY + MulDivOS((lastY - thisY), (scnWidth - thisX), (lastX - thisX));
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
							lastY = thisY + MulDivOS(lastY - thisY, 0 - thisX, lastX - thisX);
							lastX = 0;
						}
					}

					if (thisX < 0)
					{
						thisY = lastY + MulDivOS(thisY - lastY, 0 - lastX, thisX - lastX);
						thisX = 0;
					}
					else if (thisX > scnWidth)
					{
						thisY = lastY + MulDivOS(thisY - lastY, scnWidth - lastX, thisX - lastX);
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
							lastX = thisX + MulDivOS(lastX - thisX, scnHeight - thisY, lastY - thisY);
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
							lastX = thisX + MulDivOS(lastX - thisX, 0 - thisY, lastY - thisY);
							lastY = 0;
						}
					}

					if (thisY < 0)
					{
						thisX = lastX + MulDivOS(thisX - lastX, 0 - lastY, thisY - lastY);
						thisY = 0;
					}
					else if (thisY > scnHeight)
					{
						thisX = lastX + MulDivOS(thisX - lastX, scnHeight - lastY, thisY - lastY);
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
					j = (UOSInt)((denv->labels[i].nPoints - 1) >> 1);
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

				if (diff.x > diff.y)
				{
					scnPt.x = OSInt2Double((maxX + minX) >> 1);
					k = 0;
					while (k < denv->labels[i].nPoints - 1)
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
							scaleN = scnPt.x - points[k ].x;
							break;
						}
						k++;
					}

				}
				else
				{
					scnPt.y = OSInt2Double((maxY + minY) >> 1);
					k = 0;
					while (k < denv->labels[i].nPoints - 1)
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
				scnPt.x = points[k].x + ((points[k + 1].x - points[k].x) * scaleN / scaleD);
				scnPt.y = points[k].y + ((points[k + 1].y - points[k].y) * scaleN / scaleD);
				if (denv->labels[i].flags & Map::MapEnv::SFLG_ROTATE)
				{
					denv->labels[i].scaleW = denv->labels[i].points[k + 1].x - denv->labels[i].points[k].y;
					denv->labels[i].scaleH = denv->labels[i].points[k + 1].y - denv->labels[i].points[k].y;
				}
				else
				{
					denv->labels[i].scaleW = 0;
					denv->labels[i].scaleH = 0;
				}
				GetCharsSize(denv, &szThis, denv->labels[i].label->ToCString(), denv->labels[i].fontType, denv->labels[i].fontStyle, denv->labels[i].scaleW, denv->labels[i].scaleH);
				if (OSInt2Double(diff.x) < szThis.x && OSInt2Double(diff.y) < szThis.y)
				{
					rect.tl = scnPt - (szThis * 0.5);
					rect.br = rect.tl + szThis;

					overlapped = LabelOverlapped(denv->objBounds, currPt, rect);
					if (!overlapped)
					{
						Math::Coord2DDbl center = rect.GetCenter();
						DrawChars(denv, denv->labels[i].label->ToCString(), center, denv->labels[i].scaleW, denv->labels[i].scaleH, denv->labels[i].fontType, denv->labels[i].fontStyle, (denv->labels[i].flags & Map::MapEnv::SFLG_ALIGN) != 0);

						denv->objBounds[currPt] = rect;
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
						rect.tl = scnPt - (szThis * 0.5);
						rect.br = rect.tl + szThis;

						overlapped = LabelOverlapped(denv->objBounds, currPt, rect);
						if (!overlapped || --tryCnt <= 0)
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
									scnPt.x += OSInt2Double(scnDiff) + 1;
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
								while (k < denv->labels[i].nPoints - 1)
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
								while (k < denv->labels[i].nPoints - 1)
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
							scnPt = points[k].ToDouble() + ((points[k + 1].ToDouble() - points[k].ToDouble()) * scaleN / scaleD);
							if (denv->labels[i].flags & Map::MapEnv::SFLG_ROTATE)
							{
								denv->labels[i].scaleW = denv->labels[i].points[k + 1].x - denv->labels[i].points[k].x;
								denv->labels[i].scaleH = denv->labels[i].points[k + 1].y - denv->labels[i].points[k].y;
							}
							else
							{
								denv->labels[i].scaleW = 0;
								denv->labels[i].scaleH = 0;
							}
							GetCharsSize(denv, &szThis, denv->labels[i].label->ToCString(), denv->labels[i].fontType, denv->labels[i].fontStyle, denv->labels[i].scaleW, denv->labels[i].scaleH);
						}
					}

					if (!overlapped && thisCnt < 10)
					{
						OSInt m;
						OSInt n = 1;
						OSInt tmpV;
						m = thisCnt;
						while (n && m > 0)
						{
							n = 0;
							tmpV = thisPts[--m];
							if (OSInt2Double(tmpV - LBLMINDIST) < rect.br.y && OSInt2Double(tmpV + LBLMINDIST) > rect.tl.y)
							{
								n++;
							}
							tmpV = thisPts[--m];
							if (OSInt2Double(tmpV - LBLMINDIST) < rect.br.x && OSInt2Double(tmpV + LBLMINDIST) > rect.tl.x)
							{
								n++;
							}
							n = (n != 2);
						}

						if (n)
						{
							Math::Coord2DDbl center = rect.GetCenter();
							if ((denv->labels[i].flags & Map::MapEnv::SFLG_ALIGN) != 0)
							{
								Math::RectAreaDbl realBounds;
								DrawCharsLA(denv, denv->labels[i].label->ToCString(), denv->labels[i].points, points, denv->labels[i].nPoints, k, scaleN, scaleD, denv->labels[i].fontType, denv->labels[i].fontStyle, &realBounds);

								denv->objBounds[currPt] = realBounds;
								currPt++;
							}
							else if ((denv->labels[i].flags & Map::MapEnv::SFLG_ROTATE) != 0)
							{
								Math::RectAreaDbl realBounds;
								DrawCharsL(denv, denv->labels[i].label->ToCString(), denv->labels[i].points, points, denv->labels[i].nPoints, k, scaleN, scaleD, denv->labels[i].fontType, denv->labels[i].fontStyle, &realBounds);

								denv->objBounds[currPt] = realBounds;
								currPt++;
							}
							else
							{
								DrawChars(denv, denv->labels[i].label->ToCString(), center, denv->labels[i].scaleW, denv->labels[i].scaleH, denv->labels[i].fontType, denv->labels[i].fontStyle, (denv->labels[i].flags & Map::MapEnv::SFLG_ALIGN) != 0);

								denv->objBounds[currPt] = rect;
								currPt++;
							}

							thisPts[thisCnt++] = Double2OSInt(center.x);
							thisPts[thisCnt++] = Double2OSInt(center.y);
						}
						j = (thisCnt < 10);
					}
				}
				MemFree(points);
			}
			else if (denv->labels[i].layerType == Map::DRAW_LAYER_POLYGON)
			{
				GetCharsSize(denv, &szThis, denv->labels[i].label->ToCString(), denv->labels[i].fontType, denv->labels[i].fontStyle, 0, 0);//labels[i].scaleW, labels[i].scaleH);
				dscnPos = denv->view->MapXYToScnXY(denv->labels[i].pos);
				scnPt = dscnPos;

				overlapped = true;
				if (overlapped)
				{
					rect.tl = scnPt  - (szThis * 0.5);
					rect.br = rect.tl + szThis;

					overlapped = LabelOverlapped(denv->objBounds, currPt, rect);
				}
				if (overlapped)
				{
					rect.tl.x = scnPt.x + 1;
					rect.tl.y = scnPt.y - (szThis.y * 0.5);
					rect.br = rect.tl + szThis;

					overlapped = LabelOverlapped(denv->objBounds, currPt, rect);
				}
				if (overlapped)
				{
					rect.tl.x = scnPt.x - szThis.x - 1;
					rect.tl.y = scnPt.y - (szThis.y * 0.5);
					rect.br = rect.tl + szThis;

					overlapped = LabelOverlapped(denv->objBounds, currPt, rect);
				}
				if (overlapped)
				{
					rect.tl.x = scnPt.x - (szThis.x * 0.5);
					rect.tl.y = scnPt.y - szThis.y - 1;
					rect.br = rect.tl + szThis;

					overlapped = LabelOverlapped(denv->objBounds, currPt, rect);
				}
				if (overlapped)
				{
					rect.tl.x = scnPt.x - (szThis.x * 0.5);
					rect.tl.y = scnPt.y + 1;
					rect.br = rect.tl + szThis;

					overlapped = LabelOverlapped(denv->objBounds, currPt, rect);
				}

				if (!overlapped)
				{
					Math::Coord2DDbl center = rect.GetCenter();
					DrawChars(denv, denv->labels[i].label->ToCString(), center, 0, 0, denv->labels[i].fontType, denv->labels[i].fontStyle, 0);

					denv->objBounds[currPt] = rect;
					currPt++;
				}
			}
			else
			{

			}

			i++;
		}

		denv->objCnt = currPt;
	}

	i = denv->labelCnt;
	while (i-- > 0)
	{
		denv->labels[i].label->Release();
		if (denv->labels[i].points)
			MemFreeA(denv->labels[i].points);
	}
	if (lastLbl)
		lastLbl->Release();
}

OSInt Map::DrawMapRenderer::VImgCompare(Math::Geometry::VectorImage *obj1, Math::Geometry::VectorImage *obj2)
{
	Bool type1 = obj1->IsScnCoord();
	Bool type2 = obj2->IsScnCoord();
	if (type1 != type2)
	{
		if (type1)
			return 1;
		else
			return -1;
	}
	Int32 zIndex1 = obj1->GetZIndex();
	Int32 zIndex2 = obj2->GetZIndex();
	if (zIndex1 > zIndex2)
	{
		return 1;
	}
	else if (zIndex1 < zIndex2)
	{
		return -1;
	}
	Math::Coord2DDbl coord1;
	Math::Coord2DDbl coord2;
	coord1 = obj1->GetCenter();
	coord2 = obj2->GetCenter();
	if (coord2.y > coord1.y)
	{
		return 1;
	}
	else if (coord1.y > coord2.y)
	{
		return -1;
	}
	else if (coord1.x > coord2.x)
	{
		return 1;
	}
	else if (coord1.x < coord2.x)
	{
		return -1;
	}
	else
	{
		return 0;
	}
}

void Map::DrawMapRenderer::DrawLayers(NotNullPtr<Map::DrawMapRenderer::DrawEnv> denv, Map::MapEnv::GroupItem *group)
{
	Map::MapEnv::LayerItem layer;
	Map::MapEnv::MapItem *item;

	Sync::MutexUsage mutUsage;
	denv->env->BeginUse(mutUsage);
	UOSInt i = 0;
	UOSInt j = denv->env->GetItemCount(group);

	while (i < j)
	{
		item = denv->env->GetItem(group, i);
		if (item->itemType == Map::MapEnv::IT_GROUP)
		{
			DrawLayers(denv, (Map::MapEnv::GroupItem*)item);
		}
		else if (item->itemType == Map::MapEnv::IT_LAYER)
		{
			Double scale = denv->view->GetMapScale();
			if (denv->env->GetLayerProp(layer, group, i))
			{
				if (layer.minScale <= scale && layer.maxScale >= scale)
				{
					Map::DrawLayerType layerType = layer.layer->GetLayerType();
					layer.layer->SetDispSize(denv->dispSize, denv->img->GetHDPI());
					if (layerType == Map::DRAW_LAYER_POLYLINE || layerType == Map::DRAW_LAYER_POLYLINE3D)
					{
						if ((layer.flags & Map::MapEnv::SFLG_HIDESHAPE) == 0)
						{
							if (layer.lineType == 0)
							{
								DrawShapes(denv, layer.layer, layer.lineStyle, layer.fillStyle, layer.lineThick, layer.lineColor);
							}
							else
							{
								DrawShapes(denv, layer.layer, (UOSInt)-1, layer.fillStyle, layer.lineThick, layer.lineColor);
							}

						}
						if (layer.flags & Map::MapEnv::SFLG_SHOWLABEL)
						{
							if (layer.fontType == Map::MapEnv::FontType::GlobalStyle)
							{
								if (layer.fontStyle < denv->fontStyleCnt)
								{
									DrawLabel(denv, layer.layer, layer.fontStyle, layer.labelCol, layer.priority, layer.flags, 0, 0, layer.fontType);
								}
							}
							else if (layer.fontType == Map::MapEnv::FontType::LayerStyle)
							{
								UOSInt fs = denv->layerFont.GetCount();
								Media::DrawFont *f = denv->img->NewFontPt(layer.fontName->ToCString(), layer.fontSizePt, Media::DrawEngine::DFS_NORMAL, 0);
								Media::DrawBrush *b = denv->img->NewBrushARGB(this->colorConv->ConvRGB8(layer.fontColor));
								denv->layerFont.Add(f);
								denv->layerFontColor.Add(b);
								DrawLabel(denv, layer.layer, fs, layer.labelCol, layer.priority, layer.flags, 0, 0, layer.fontType);
							}
						}
					}
					else if (layerType == Map::DRAW_LAYER_POLYGON)
					{
						if ((layer.flags & Map::MapEnv::SFLG_HIDESHAPE) == 0)
						{
							if (layer.lineType == 0)
							{
								DrawShapes(denv, layer.layer, layer.lineStyle, layer.fillStyle, layer.lineThick, layer.lineColor);
							}
							else
							{
								DrawShapes(denv, layer.layer, (UOSInt)-1, layer.fillStyle, layer.lineThick, layer.lineColor);
							}
						}
						if (layer.flags & Map::MapEnv::SFLG_SHOWLABEL)
						{
							if (layer.fontType == Map::MapEnv::FontType::GlobalStyle)
							{
								if (layer.fontStyle < denv->fontStyleCnt)
								{
									DrawLabel(denv, layer.layer, layer.fontStyle, layer.labelCol, layer.priority, layer.flags, 0, 0, layer.fontType);
								}
							}
							else if (layer.fontType == Map::MapEnv::FontType::LayerStyle)
							{
								UOSInt fs = denv->layerFont.GetCount();
								Media::DrawFont *f = denv->img->NewFontPt(layer.fontName->ToCString(), layer.fontSizePt, Media::DrawEngine::DFS_NORMAL, 0);
								Media::DrawBrush *b = denv->img->NewBrushARGB(this->colorConv->ConvRGB8(layer.fontColor));
								denv->layerFont.Add(f);
								denv->layerFontColor.Add(b);
								DrawLabel(denv, layer.layer, fs, layer.labelCol, layer.priority, layer.flags, 0, 0, layer.fontType);
							}
						}
					}
					else if (layerType == Map::DRAW_LAYER_POINT || layerType == Map::DRAW_LAYER_POINT3D)
					{
						if ((layer.flags & Map::MapEnv::SFLG_HIDESHAPE) == 0)
							DrawShapesPoint(denv, layer.layer, layer.imgIndex);
						if (layer.flags & Map::MapEnv::SFLG_SHOWLABEL)
						{
							Media::Image *pimg = 0;
							Double spotX;
							Double spotY;
							if (layer.layer->HasIconStyle())
							{
								pimg = layer.layer->GetIconStyleImg()->GetImage(0);
								spotX = OSInt2Double(layer.layer->GetIconStyleSpotX());
								spotY = OSInt2Double(layer.layer->GetIconStyleSpotY());
								if (pimg != 0 && (spotX == -1 || spotY == -1))
								{
									spotX = UOSInt2Double(pimg->info.dispSize.x) * 0.5;
									spotY = UOSInt2Double(pimg->info.dispSize.y) * 0.5;
								}
							}
							if (pimg == 0)
							{
								pimg = denv->env->GetImage(layer.imgIndex, 0);
								if (pimg)
								{
									spotX = UOSInt2Double(pimg->info.dispSize.x) * 0.5;
									spotY = UOSInt2Double(pimg->info.dispSize.y) * 0.5;
								}
								else
								{
									spotX = 0;
									spotY = 0;
								}
							}

							if (layer.fontType == Map::MapEnv::FontType::GlobalStyle)
							{
								if (layer.fontStyle < denv->fontStyleCnt)
								{
									if (pimg)
									{
										DrawLabel(denv, layer.layer, layer.fontStyle, layer.labelCol, layer.priority, layer.flags, (UOSInt)Double2Int32(UOSInt2Double(pimg->info.dispSize.x) * denv->img->GetHDPI() / pimg->info.hdpi), (UOSInt)Double2Int32(UOSInt2Double(pimg->info.dispSize.y) * denv->img->GetVDPI() / pimg->info.vdpi), layer.fontType);
									}
									else
									{
										DrawLabel(denv, layer.layer, layer.fontStyle, layer.labelCol, layer.priority, layer.flags, 0, 0, layer.fontType);
									}
								}
							}
							else if (layer.fontType == Map::MapEnv::FontType::LayerStyle)
							{
								UOSInt fs = denv->layerFont.GetCount();
								Media::DrawFont *f = denv->img->NewFontPt(layer.fontName->ToCString(), layer.fontSizePt, Media::DrawEngine::DFS_NORMAL, 0);
								Media::DrawBrush *b = denv->img->NewBrushARGB(this->colorConv->ConvRGB8(layer.fontColor));
								denv->layerFont.Add(f);
								denv->layerFontColor.Add(b);
								if (pimg)
								{
									DrawLabel(denv, layer.layer, fs, layer.labelCol, layer.priority, layer.flags, (UOSInt)Double2Int32(UOSInt2Double(pimg->info.dispSize.x) * denv->img->GetHDPI() / pimg->info.hdpi), (UOSInt)Double2Int32(UOSInt2Double(pimg->info.dispSize.y) * denv->img->GetVDPI() / pimg->info.vdpi), layer.fontType);
								}
								else
								{
									DrawLabel(denv, layer.layer, fs, layer.labelCol, layer.priority, layer.flags, 0, 0, layer.fontType);
								}
							}
						}
					}
					else if (layerType == Map::DRAW_LAYER_MIXED)
					{
						if ((layer.flags & Map::MapEnv::SFLG_HIDESHAPE) == 0)
						{
							if (layer.lineType == 0)
							{
								layer.layer->SetMixedData(Map::MapDrawLayer::MixedData::NonPointOnly);
								DrawShapes(denv, layer.layer, layer.lineStyle, layer.fillStyle, layer.lineThick, layer.lineColor);
							}
							else
							{
								layer.layer->SetMixedData(Map::MapDrawLayer::MixedData::NonPointOnly);
								DrawShapes(denv, layer.layer, (UOSInt)-1, layer.fillStyle, layer.lineThick, layer.lineColor);
							}
							layer.layer->SetMixedData(Map::MapDrawLayer::MixedData::PointOnly);
							DrawShapesPoint(denv, layer.layer, layer.imgIndex);
							layer.layer->SetMixedData(Map::MapDrawLayer::MixedData::AllData);
						}
						if (layer.flags & Map::MapEnv::SFLG_SHOWLABEL)
						{
							if (layer.fontType == Map::MapEnv::FontType::GlobalStyle)
							{
								if (layer.fontStyle < denv->fontStyleCnt)
								{
									layer.layer->SetMixedData(Map::MapDrawLayer::MixedData::NonPointOnly);
									DrawLabel(denv, layer.layer, layer.fontStyle, layer.labelCol, layer.priority, layer.flags, 0, 0, layer.fontType);
									layer.layer->SetMixedData(Map::MapDrawLayer::MixedData::PointOnly);
									DrawLabel(denv, layer.layer, layer.fontStyle, layer.labelCol, layer.priority, layer.flags, 0, 0, layer.fontType);
								}
							}
							else if (layer.fontType == Map::MapEnv::FontType::LayerStyle)
							{
								UOSInt fs = denv->layerFont.GetCount();
								Media::DrawFont *f = denv->img->NewFontPt(layer.fontName->ToCString(), layer.fontSizePt, Media::DrawEngine::DFS_NORMAL, 0);
								Media::DrawBrush *b = denv->img->NewBrushARGB(this->colorConv->ConvRGB8(layer.fontColor));
								denv->layerFont.Add(f);
								denv->layerFontColor.Add(b);
								layer.layer->SetMixedData(Map::MapDrawLayer::MixedData::NonPointOnly);
								DrawLabel(denv, layer.layer, fs, layer.labelCol, layer.priority, layer.flags, 0, 0, layer.fontType);
								layer.layer->SetMixedData(Map::MapDrawLayer::MixedData::PointOnly);
								DrawLabel(denv, layer.layer, fs, layer.labelCol, layer.priority, layer.flags, 0, 0, layer.fontType);
							}
						}
						layer.layer->SetMixedData(Map::MapDrawLayer::MixedData::AllData);
					}
					else if (layerType == Map::DRAW_LAYER_IMAGE)
					{
						DrawImageLayer(denv, layer.layer);
					}
				}
			}
		}

		i++;
	}
	mutUsage.EndUse();
}

void Map::DrawMapRenderer::DrawShapes(NotNullPtr<Map::DrawMapRenderer::DrawEnv> denv, Map::MapDrawLayer *layer, UOSInt lineStyle, UInt32 fillStyle, UOSInt lineThick, UInt32 lineColor)
{
	UOSInt i;
	Map::GetObjectSess *session;
	Media::DrawPen *p;
	Media::DrawBrush *b;
	Int64 lastId;
	Int64 thisId;
	UOSInt layerId = 0;
	Math::Geometry::Vector2D *vec;
	NotNullPtr<Math::CoordinateSystem> lyrCSys = layer->GetCoordinateSystem();
	NotNullPtr<Math::CoordinateSystem> envCSys = this->env->GetCoordinateSystem();
	denv->idArr.Clear();
	if (!lyrCSys->Equals(envCSys))
	{
		Math::Coord2DDbl tl;
		Math::Coord2DDbl br;
		Math::RectAreaDbl rect = denv->view->GetVerticalRect();
		tl = rect.tl;
		br = rect.GetBR();
		tl = Math::CoordinateSystem::ConvertXYZ(envCSys, lyrCSys, Math::Vector3(tl, 0)).GetXY();
		br = Math::CoordinateSystem::ConvertXYZ(envCSys, lyrCSys, Math::Vector3(br, 0)).GetXY();
		layer->GetObjectIdsMapXY(denv->idArr, 0, Math::RectAreaDbl(tl, br), true);

		if ((i = denv->idArr.GetCount()) > 0)
		{
			UInt32 color;
			UOSInt thick;
			UInt8 *pattern;
			UOSInt npattern;
			Bool found;
			found = denv->env->GetLineStyleLayer(lineStyle, layerId++, color, thick, pattern, npattern);
			if (!found)
			{
				thick = lineThick;
				color = lineColor;
				npattern = 0;
			}
			p = denv->img->NewPenARGB(this->colorConv->ConvRGB8(color), UOSInt2Double(thick) * denv->img->GetHDPI() / 96.0, pattern, npattern);
			b = denv->img->NewBrushARGB(this->colorConv->ConvRGB8(fillStyle));
			this->mapSch.SetDrawType(layer, p, b, 0, 0.0, 0.0, &denv->isLayerEmpty);

			session = layer->BeginGetObject();
			lastId = -1;
			while (i-- > 0)
			{
				thisId = denv->idArr.GetItem(i);
				if (thisId != lastId)
				{
					lastId = thisId;
					if ((vec = layer->GetNewVectorById(session, thisId)) != 0)
					{
						vec->ConvCSys(lyrCSys, envCSys);
						this->mapSch.Draw(vec);
					}
				}
			}
			layer->EndGetObject(session);

			if (found)
			{
				while (denv->env->GetLineStyleLayer(lineStyle, layerId++, color, thick, pattern, npattern))
				{
					p = denv->img->NewPenARGB(this->colorConv->ConvRGB8(color), UOSInt2Double(thick) * denv->img->GetHDPI() / 96.0, pattern, npattern);
					this->mapSch.DrawNextType(p, 0);
				}
			}
			this->mapSch.WaitForFinish();
		}
	}
	else
	{
		Math::Coord2DDbl tl;
		Math::Coord2DDbl br;
		Math::RectAreaDbl rect = denv->view->GetVerticalRect();
		tl = rect.tl;
		br = rect.br;
		layer->GetObjectIdsMapXY(denv->idArr, 0, Math::RectAreaDbl(tl, br), true);

		if ((i = denv->idArr.GetCount()) > 0)
		{
			UInt32 color;
			UOSInt thick;
			UInt8 *pattern;
			UOSInt npattern;
			Bool found;
			found = denv->env->GetLineStyleLayer(lineStyle, layerId++, color, thick, pattern, npattern);
			if (!found)
			{
				thick = lineThick;
				color = lineColor;
				npattern = 0;
			}
			p = denv->img->NewPenARGB(this->colorConv->ConvRGB8(color), UOSInt2Double(thick) * denv->img->GetHDPI() / 96.0, pattern, npattern);
			b = denv->img->NewBrushARGB(this->colorConv->ConvRGB8(fillStyle));
			this->mapSch.SetDrawType(layer, p, b, 0, 0.0, 0.0, &denv->isLayerEmpty);

			session = layer->BeginGetObject();
			lastId = -1;
			while (i-- > 0)
			{
				thisId = denv->idArr.GetItem(i);
				if (thisId != lastId)
				{
					lastId = thisId;
					if ((vec = layer->GetNewVectorById(session, thisId)) != 0)
					{
						this->mapSch.Draw(vec);
					}
				}
			}
			layer->EndGetObject(session);

			if (found)
			{
				while (denv->env->GetLineStyleLayer(lineStyle, layerId++, color, thick, pattern, npattern))
				{
					p = denv->img->NewPenARGB(this->colorConv->ConvRGB8(color), UOSInt2Double(thick) * denv->img->GetHDPI() / 96.0, pattern, npattern);
					this->mapSch.DrawNextType(p, 0);
				}
			}
			this->mapSch.WaitForFinish();
		}
	}
}

void Map::DrawMapRenderer::DrawShapesPoint(NotNullPtr<Map::DrawMapRenderer::DrawEnv> denv, Map::MapDrawLayer *layer, UOSInt imgIndex)
{
	Data::ArrayListInt64 arri;
	Math::Geometry::Vector2D *vec;
	UOSInt i;
	Map::GetObjectSess *session;
	Math::Coord2DDbl tl;
	Math::Coord2DDbl br;
	Math::RectAreaDbl rect = denv->view->GetVerticalRect();
	tl = rect.tl;
	br = rect.br;
	Double spotX;
	Double spotY;
	UOSInt maxLabel = denv->env->GetNString();

	Media::Image *img = 0;
	NotNullPtr<Media::Image> nnimg;
	UInt32 imgTimeMS = 0;
	if (layer->HasIconStyle())
	{
		img = layer->GetIconStyleImg()->GetImage(imgTimeMS);
		spotX = OSInt2Double(layer->GetIconStyleSpotX());
		spotY = OSInt2Double(layer->GetIconStyleSpotY());
		if (img != 0 && (spotX == -1 || spotY == -1))
		{
			spotX = UOSInt2Double(img->info.dispSize.x) * 0.5;
			spotY = UOSInt2Double(img->info.dispSize.y) * 0.5;
		}
	}
	if (!nnimg.Set(img))
	{
		img = denv->env->GetImage(imgIndex, imgTimeMS);
		if (!nnimg.Set(img))
			return;
		spotX = UOSInt2Double(nnimg->info.dispSize.x) * 0.5;
		spotY = UOSInt2Double(nnimg->info.dispSize.y) * 0.5;
	}
	if (imgTimeMS != 0)
	{
		if (denv->imgDurMS == 0)
		{
			denv->imgDurMS = imgTimeMS;
		}
		else if (denv->imgDurMS > imgTimeMS)
		{
			denv->imgDurMS = imgTimeMS;
		}
	}
	NotNullPtr<Math::CoordinateSystem> lyrCSys = layer->GetCoordinateSystem();
	NotNullPtr<Math::CoordinateSystem> envCSys = this->env->GetCoordinateSystem();
	if (!lyrCSys->Equals(envCSys))
	{
		tl = Math::CoordinateSystem::ConvertXYZ(envCSys, lyrCSys, Math::Vector3(tl, 0)).GetXY();
		br = Math::CoordinateSystem::ConvertXYZ(envCSys, lyrCSys, Math::Vector3(br, 0)).GetXY();
		layer->GetObjectIdsMapXY(arri, 0, Math::RectAreaDbl(tl - ((br - tl) * 0.5), br + ((br - tl) * 0.5)), true);
		if (arri.GetCount() <= 0)
		{
			return;
		}
		Media::DrawImage *dimg;
		NotNullPtr<Media::DrawImage> gimg;
		NotNullPtr<Media::StaticImage> simg;
		if (this->drawType == Map::DrawMapRenderer::DT_PIXELDRAW)
		{
			UInt32 newW = (UInt32)Double2Int32(UOSInt2Double(nnimg->info.dispSize.x) * denv->img->GetHDPI() / nnimg->info.hdpi);
			UInt32 newH = (UInt32)Double2Int32(UOSInt2Double(nnimg->info.dispSize.y) * denv->img->GetVDPI() / nnimg->info.vdpi);
			if (newW > nnimg->info.dispSize.x || newH > nnimg->info.dispSize.y)
			{
				this->resizer->SetTargetSize(Math::Size2D<UOSInt>(newW, newH));
				simg = NotNullPtr<Media::StaticImage>::ConvertFrom(nnimg);
				NotNullPtr<Media::StaticImage> img2;
				if (img2.Set(this->resizer->ProcessToNew(simg)))
				{
					spotX = spotX * denv->img->GetHDPI() / nnimg->info.hdpi;
					spotY = spotY * denv->img->GetVDPI() / nnimg->info.vdpi;
					dimg = this->eng->ConvImage(img2);
					img2.Delete();
				}
				else
				{
					dimg = this->eng->ConvImage(nnimg);
				}
			}
			else
			{
				dimg = this->eng->ConvImage(nnimg);
			}
		}
		else
		{
			dimg = this->eng->ConvImage(nnimg);
		}
		if (gimg.Set(dimg))
		{
			this->mapSch.SetDrawType(layer, 0, 0, dimg, spotX, spotY, &denv->isLayerEmpty);
			this->mapSch.SetDrawObjs(denv->objBounds, &denv->objCnt, maxLabel);
			session = layer->BeginGetObject();

			i = arri.GetCount();
			while (i-- > 0)
			{
				if ((vec = layer->GetNewVectorById(session, arri.GetItem(i))) != 0)
				{
					vec->ConvCSys(lyrCSys, envCSys);
					this->mapSch.Draw(vec);
				}
			}

			layer->EndGetObject(session);
			this->mapSch.WaitForFinish();
			this->eng->DeleteImage(gimg);
		}
	}
	else
	{
		layer->GetObjectIdsMapXY(arri, 0, Math::RectAreaDbl(tl - ((br - tl) * 0.5), br + ((br - tl) * 0.5)), true);
		if (arri.GetCount() <= 0)
		{
			return;
		}

		Media::DrawImage *dimg;
		NotNullPtr<Media::DrawImage> gimg;
		NotNullPtr<Media::StaticImage> simg;
		if (this->drawType == Map::DrawMapRenderer::DT_PIXELDRAW)
		{
			UInt32 newW = (UInt32)Double2Int32(UOSInt2Double(nnimg->info.dispSize.x) * denv->img->GetHDPI() / nnimg->info.hdpi);
			UInt32 newH = (UInt32)Double2Int32(UOSInt2Double(nnimg->info.dispSize.y) * denv->img->GetVDPI() / nnimg->info.vdpi);
			if (newW != nnimg->info.dispSize.x || newH != nnimg->info.dispSize.y)
			{
				this->resizer->SetTargetSize(Math::Size2D<UOSInt>(newW, newH));
				simg = NotNullPtr<Media::StaticImage>::ConvertFrom(nnimg);
				NotNullPtr<Media::StaticImage> img2;
				if (img2.Set(this->resizer->ProcessToNew(simg)))
				{
					spotX = spotX * denv->img->GetHDPI() / nnimg->info.hdpi;
					spotY = spotY * denv->img->GetVDPI() / nnimg->info.vdpi;
					dimg = this->eng->ConvImage(img2);
					img2.Delete();
				}
				else
				{
					dimg = this->eng->ConvImage(nnimg);
				}
			}
			else
			{
				dimg = this->eng->ConvImage(nnimg);
			}
		}
		else
		{
			dimg = this->eng->ConvImage(nnimg);
		}
		if (gimg.Set(dimg))
		{
			this->mapSch.SetDrawType(layer, 0, 0, dimg, spotX, spotY, &denv->isLayerEmpty);
			this->mapSch.SetDrawObjs(denv->objBounds, &denv->objCnt, maxLabel);
			session = layer->BeginGetObject();

			i = arri.GetCount();
			while (i-- > 0)
			{
				if ((vec = layer->GetNewVectorById(session, arri.GetItem(i))) != 0)
				{
					this->mapSch.Draw(vec);
				}
			}

			layer->EndGetObject(session);
			this->mapSch.WaitForFinish();
			this->eng->DeleteImage(gimg);
		}
	}
}

void Map::DrawMapRenderer::DrawLabel(NotNullPtr<DrawEnv> denv, Map::MapDrawLayer *layer, UOSInt fontStyle, UOSInt labelCol, Int32 priority, Int32 flags, UOSInt imgWidth, UOSInt imgHeight, Map::MapEnv::FontType fontType)
{
	Map::NameArray *arr;
	Data::ArrayListInt64 arri;
	UOSInt i;
	Math::Geometry::Vector2D *vec;
	Double scaleW;
	Double scaleH;
	Math::Coord2DDbl pts;
	UTF8Char *sptr;
	UTF8Char *sptrEnd;
	UTF8Char lblStr[256];
	Map::GetObjectSess *session;
	UOSInt maxLabel = denv->env->GetNString();
	Bool csysConv = false;;
	Math::Coord2DDbl tl;
	Math::Coord2DDbl br;
	Math::RectAreaDbl rect = denv->view->GetVerticalRect();
	tl = rect.tl;
	br = rect.br;
	NotNullPtr<Math::CoordinateSystem> lyrCSys = layer->GetCoordinateSystem();
	NotNullPtr<Math::CoordinateSystem> envCSys = this->env->GetCoordinateSystem();
	if (!lyrCSys->Equals(envCSys))
	{
		tl = Math::CoordinateSystem::ConvertXYZ(envCSys, lyrCSys, Math::Vector3(tl, 0)).GetXY();
		br = Math::CoordinateSystem::ConvertXYZ(envCSys, lyrCSys, Math::Vector3(br, 0)).GetXY();
		csysConv = true;
	}

	layer->GetObjectIdsMapXY(arri, &arr, Math::RectAreaDbl(tl, br), false);
	session = layer->BeginGetObject();
	i = arri.GetCount();
	while (i-- > 0)
	{
		if ((vec = layer->GetNewVectorById(session, arri.GetItem(i))) != 0)
		{
			sptrEnd = layer->GetString(sptr = lblStr, sizeof(lblStr), arr, arri.GetItem(i), labelCol);
			if (sptrEnd)
			{
				if (csysConv)
				{
					vec->ConvCSys(lyrCSys, envCSys);
				}
				if (flags & Map::MapEnv::SFLG_TRIM)
				{
					sptrEnd = Text::StrTrim(lblStr);
				}
				if (flags & Map::MapEnv::SFLG_CAPITAL)
				{
					Text::StrToCapital(lblStr, lblStr);
				}

				if (flags & Map::MapEnv::SFLG_SMART)
				{
					switch (vec->GetVectorType())
					{
					case Math::Geometry::Vector2D::VectorType::Point:
					{
						Math::Coord2DDbl pt = vec->GetCenter();
						AddLabel(denv->labels, maxLabel, &denv->labelCnt, CSTRP(sptr, sptrEnd), 1, &pt, priority, Map::DRAW_LAYER_POINT, fontStyle, flags, denv->view, (OSInt)imgWidth, (OSInt)imgHeight, fontType);
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
						if (vec->GetVectorType() == Math::Geometry::Vector2D::VectorType::Polygon)
						{
							AddLabel(denv->labels, maxLabel, &denv->labelCnt, CSTRP(sptr, sptrEnd), maxSize, &pointArr[maxPos], priority, Map::DRAW_LAYER_POLYGON, fontStyle, flags, denv->view, (OSInt)imgWidth, (OSInt)imgHeight, fontType);
						}
						else
						{
							AddLabel(denv->labels, maxLabel, &denv->labelCnt, CSTRP(sptr, sptrEnd), maxSize, &pointArr[maxPos], priority, Map::DRAW_LAYER_POLYLINE3D, fontStyle, flags, denv->view, (OSInt)imgWidth, (OSInt)imgHeight, fontType);
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
						printf("DrawMapRenderer.DrawLabelSmart: Unknown vector type\r\n");
						break;
					}
					DEL_CLASS(vec);
				}
				else
				{
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

						if (denv->view->InViewXY(pts))
						{
							pts = denv->view->MapXYToScnXY(pts);

							if ((flags & Map::MapEnv::SFLG_ROTATE) == 0)
								scaleW = scaleH = 0;
							DrawChars(denv, CSTRP(sptr, sptrEnd), pts, scaleW, scaleH, fontType, fontStyle, (flags & Map::MapEnv::SFLG_ALIGN) != 0);
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
						if (denv->view->InViewXY(pts))
						{
							pts = denv->view->MapXYToScnXY(pts);
							DrawChars(denv, CSTRP(sptr, sptrEnd), pts, 0, 0, fontType, fontStyle, (flags & Map::MapEnv::SFLG_ALIGN) != 0);
						}
						break;
					}
					case Math::Geometry::Vector2D::VectorType::Point:
					{
						pts = vec->GetCenter();
						if (denv->view->InViewXY(pts))
						{
							pts = denv->view->MapXYToScnXY(pts);
							DrawChars(denv, CSTRP(sptr, sptrEnd), pts, 0, 0, fontType, fontStyle, (flags & Map::MapEnv::SFLG_ALIGN) != 0);
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
						printf("DrawMapRenderer.DrawLabel: Unknown vector type\r\n");
						break;
					}
					DEL_CLASS(vec);
				}
			}
			else
			{
				DEL_CLASS(vec);
			}
		}
	}
	layer->EndGetObject(session);
	layer->ReleaseNameArr(arr);
}

void Map::DrawMapRenderer::DrawImageLayer(NotNullPtr<DrawEnv> denv, Map::MapDrawLayer *layer)
{
	Math::Geometry::Vector2D *vec;
	Math::Geometry::VectorImage *vimg;
	UOSInt i;
	UOSInt j;
	NotNullPtr<Math::CoordinateSystem> coord = layer->GetCoordinateSystem();
	Bool geoConv;
	Map::GetObjectSess *sess;
	geoConv = !denv->env->GetCoordinateSystem()->Equals(coord);

	Data::ArrayListInt64 arri;
	if (this->drawType == Map::DrawMapRenderer::DT_PIXELDRAW)
	{
		layer->SetCurrScale(denv->view->GetMapScale());
	}
	Math::Coord2DDbl tl;
	Math::Coord2DDbl br;
	Math::RectAreaDbl rect = denv->view->GetVerticalRect();
	tl = rect.tl;
	br = rect.br;
	if (geoConv)
	{
		tl = Math::CoordinateSystem::ConvertXYZ(denv->env->GetCoordinateSystem(), coord, Math::Vector3(tl, 0)).GetXY();
		br = Math::CoordinateSystem::ConvertXYZ(denv->env->GetCoordinateSystem(), coord, Math::Vector3(br, 0)).GetXY();
		layer->GetObjectIdsMapXY(arri, 0, Math::RectAreaDbl(tl, br), false);
	}
	else
	{
		layer->GetObjectIdsMapXY(arri, 0, Math::RectAreaDbl(tl, br), false);
	}
	Data::ArrayList<Math::Geometry::VectorImage *> imgList;
	sess = layer->BeginGetObject();
	i = 0;
	j = arri.GetCount();
	while (i < j)
	{
		vec = layer->GetNewVectorById(sess, arri.GetItem(i));
		if (vec)
		{
			if (vec->GetVectorType() == Math::Geometry::Vector2D::VectorType::Image)
			{
				imgList.Add((Math::Geometry::VectorImage*)vec);
			}
			else
			{
				DEL_CLASS(vec);
			}
		}
		i++;
	}
	layer->EndGetObject(sess);
	
	Data::Sort::ArtificialQuickSortFunc<Math::Geometry::VectorImage*>::Sort(imgList, VImgCompare);
	i = 0;
	j = imgList.GetCount();
	while (i < j)
	{
		Math::Coord2DDbl scnCoords[2];
		vimg = imgList.GetItem(i);
		if (vimg->IsScnCoord())
		{
			vimg->GetScreenBounds((UOSInt)denv->view->GetScnWidth(), (UOSInt)denv->view->GetScnHeight(), denv->view->GetHDPI(), denv->view->GetHDPI(), &scnCoords[0].x, &scnCoords[0].y, &scnCoords[1].x, &scnCoords[1].y);
		}
		else
		{
			Math::RectAreaDbl mapCoords = vimg->GetBounds();
			Double t = mapCoords.tl.y;
			mapCoords.tl.y = mapCoords.br.y;
			mapCoords.br.y = t;
			if (geoConv)
			{
				mapCoords.tl = Math::CoordinateSystem::ConvertXYZ(coord, denv->env->GetCoordinateSystem(), Math::Vector3(mapCoords.tl, 0)).GetXY();
				mapCoords.br = Math::CoordinateSystem::ConvertXYZ(coord, denv->env->GetCoordinateSystem(), Math::Vector3(mapCoords.br, 0)).GetXY();
			}
			scnCoords[0] = denv->view->MapXYToScnXY(mapCoords.tl);
			scnCoords[1] = denv->view->MapXYToScnXY(mapCoords.br);
		}
		UInt32 imgTimeMS;
		NotNullPtr<Media::StaticImage> simg;
		if (simg.Set(vimg->GetImage(scnCoords[1].x - scnCoords[0].x, scnCoords[1].y - scnCoords[0].y, imgTimeMS)))
		{
			DrawImageObject(denv, simg, scnCoords[0], scnCoords[1], vimg->GetSrcAlpha());
			if (imgTimeMS != 0)
			{
				if (denv->imgDurMS == 0)
				{
					denv->imgDurMS = imgTimeMS;
				}
				else if (denv->imgDurMS > imgTimeMS)
				{
					denv->imgDurMS = imgTimeMS;
				}
			}
		}

		DEL_CLASS(vimg);
		i++;
	}
}

void Map::DrawMapRenderer::DrawImageObject(NotNullPtr<DrawEnv> denv, NotNullPtr<Media::StaticImage> img, Math::Coord2DDbl scnTL, Math::Coord2DDbl scnBR, Double srcAlpha)
{
	UOSInt imgW;
	UOSInt imgH;
	Math::Coord2DDbl cimgPt2;
	Math::Coord2DDbl cimgPt;
	Double dimgW;
	Double dimgH;

	imgW = denv->img->GetWidth();
	imgH = denv->img->GetHeight();
	dimgW = UOSInt2Double(imgW);
	dimgH = UOSInt2Double(imgH);

	if (scnTL.x < scnBR.x && scnTL.y < scnBR.y)
	{
		if (this->drawType == DT_VECTORDRAW)
		{
			img->info.hdpi = UOSInt2Double(img->info.dispSize.x) * denv->img->GetHDPI() / (scnBR.x - scnTL.x);
			img->info.vdpi = UOSInt2Double(img->info.dispSize.y) * denv->img->GetVDPI() / (scnBR.y - scnTL.y);
			denv->img->DrawImagePt2(img, scnTL);
		}
		else
		{
			Math::Size2DDbl drawSize = scnBR - scnTL;
			if (dimgW > drawSize.x || dimgH > drawSize.y)
			{
				img->To32bpp();
				this->resizer->SetTargetSize(Math::Coord2D<UOSInt>::UOSIntFromDouble(scnBR) - Math::Coord2D<UOSInt>::UOSIntFromDouble(scnTL));
				this->resizer->SetResizeAspectRatio(Media::IImgResizer::RAR_IGNOREAR);
				NotNullPtr<Media::StaticImage> newImg;
				if (newImg.Set(this->resizer->ProcessToNew(img)))
				{
					if (srcAlpha >= 0 && srcAlpha <= 1)
					{
						newImg->MultiplyAlpha(srcAlpha);
					}
					newImg->info.hdpi = denv->img->GetHDPI();
					newImg->info.vdpi = denv->img->GetVDPI();
					denv->img->DrawImagePt2(newImg, scnTL);
					newImg.Delete();
				}
			}
			else
			{
				cimgPt2 = img->info.dispSize.ToDouble();
				cimgPt = Math::Coord2DDbl(0, 0);
				if (scnTL.x < 0)
				{
					cimgPt.x = cimgPt2.x * scnTL.x / (scnTL.x - scnBR.x);
					scnTL.x = 0;
				}
				if (scnTL.y < 0)
				{
					cimgPt.y = cimgPt2.y * scnTL.y / (scnTL.y - scnBR.y);
					scnTL.y = 0;
				}
				if (scnBR.x > dimgW)
				{
					cimgPt2.x = cimgPt.x + (cimgPt2.x - cimgPt.x) * (dimgW - scnTL.x) / (scnBR.x - scnTL.x);
					scnBR.x = dimgW;
				}
				if (scnBR.y > dimgH)
				{
					cimgPt2.y = cimgPt.y + (cimgPt2.y - cimgPt.y) * (dimgH - scnTL.y) / (scnBR.y - scnTL.y);
					scnBR.y = dimgH;
				}
				if (cimgPt.x == cimgPt2.x)
				{
					if (cimgPt2.x >= UOSInt2Double(img->info.dispSize.x))
					{
						cimgPt.x = cimgPt2.x - 1;
					}
					else
					{
						cimgPt2.x = cimgPt2.x + 1;
					}
				}
				if (cimgPt.y == cimgPt2.y)
				{
					if (cimgPt2.y >= UOSInt2Double(img->info.dispSize.y))
					{
						cimgPt.y = cimgPt2.y - 1;
					}
					else
					{
						cimgPt2.y++;
					}
				}

				this->resizer->SetTargetSize(Math::Coord2D<UOSInt>::UOSIntFromDouble(scnBR) - Math::Coord2D<UOSInt>::UOSIntFromDouble(scnTL));
				this->resizer->SetResizeAspectRatio(Media::IImgResizer::RAR_IGNOREAR);
				img->To32bpp();
				NotNullPtr<Media::StaticImage> newImg;
				if (cimgPt.x < cimgPt2.x && cimgPt.y < cimgPt2.y)
				{
					if (newImg.Set(this->resizer->ProcessToNewPartial(img, cimgPt, cimgPt2)))
					{
						if (srcAlpha >= 0 && srcAlpha <= 1)
						{
							newImg->MultiplyAlpha(srcAlpha);
						}
						NotNullPtr<Media::DrawImage> dimg;
						if (dimg.Set(this->eng->ConvImage(newImg)))
						{
							dimg->SetHDPI(denv->img->GetHDPI());
							dimg->SetVDPI(denv->img->GetVDPI());
							denv->img->DrawImagePt(dimg, scnTL);
							this->eng->DeleteImage(dimg);
						}
						newImg.Delete();
					}
				}
			}
		}
	}
}

void Map::DrawMapRenderer::GetCharsSize(NotNullPtr<DrawEnv> denv, Math::Coord2DDbl *size, Text::CString label, Map::MapEnv::FontType fontType, UOSInt fontStyle, Double scaleW, Double scaleH)
{
	Math::Size2DDbl szTmp;
	UOSInt buffSize;
	Media::DrawFont *df;
	if (fontType == Map::MapEnv::FontType::LayerStyle)
	{
		df = denv->layerFont.GetItem(fontStyle);
		buffSize = 0;
	}
	else
	{
		df = denv->fontStyles[fontStyle].font;
		buffSize = denv->fontStyles[fontStyle].buffSize;
	}
	szTmp = denv->img->GetTextSize(df, label);

	if (scaleH == 0)
	{
		size->x = (szTmp.x + UOSInt2Double(buffSize << 1));
		size->y = (szTmp.y + UOSInt2Double(buffSize << 1));

		return;
	}


	Double pt[8];

	if (scaleW < 0)
	{
		scaleW = -scaleW;
		scaleH = -scaleH;
	}
	Double degD = Math_ArcTan2(scaleH, scaleW);
	Double xPos;
	Double yPos;
	xPos = szTmp.x + UOSInt2Double(buffSize << 1);
	yPos = szTmp.y + UOSInt2Double(buffSize << 1);
	Double sVal;
	Double cVal;
	Double xs = (xPos * 0.5 * (sVal = Math_Sin(degD)));
	Double ys = (yPos * 0.5 * sVal);
	Double xc = (xPos * 0.5 * (cVal = Math_Cos(degD)));
	Double yc = (yPos * 0.5 * cVal);

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

void Map::DrawMapRenderer::DrawChars(NotNullPtr<DrawEnv> denv, Text::CStringNN str1, Math::Coord2DDbl scnPos, Double scaleW, Double scaleH, Map::MapEnv::FontType fontType, UOSInt fontStyle, Bool isAlign)
{
	Math::Size2DDbl size;
	UInt16 absH;
	Map::DrawMapRenderer::DrawFontStyle *font;
	Media::DrawFont *df;
	Media::DrawBrush *db;

	if (fontType == Map::MapEnv::FontType::GlobalStyle)
	{
		if (fontStyle >= denv->fontStyleCnt)
			return;
		font = &denv->fontStyles[fontStyle];
		df = font->font;
		db = font->fontBrush;
	}
	else if (fontType == Map::MapEnv::FontType::LayerStyle)
	{
		font = 0;
		df = denv->layerFont.GetItem(fontStyle);
		db = denv->layerFontColor.GetItem(fontStyle);
		if (df == 0 || db == 0)
			return;
	}
	else
	{
		return;
	}

	if (scaleW < 0)
	{
		scaleW = -scaleW;
		scaleH = -scaleH;
	}

	if (scaleH == 0)
	{
		scaleH = 0;
	}
	size = denv->img->GetTextSize(df, str1);

	if (scaleH == 0)
	{
		denv->img->SetTextAlign(Media::DrawEngine::DRAW_POS_TOPLEFT);

		if (font && font->buffSize > 0)
		{
			denv->img->DrawStringB(scnPos - (size * 0.5), str1, font->font, font->buffBrush, (UOSInt)Double2Int32(UOSInt2Double(font->buffSize) * denv->img->GetHDPI() / 96.0));
			denv->img->DrawString(scnPos - (size * 0.5), str1, df, db);
		}
		else
		{
			denv->img->DrawString(scnPos - (size * 0.5), str1, df, db);
		}
		return;
	}


	if (scaleH < 0)
		absH = (UInt16)-scaleH;
	else
		absH = (UInt16)scaleH;

	Double degD = Math_ArcTan2((Double)scaleH, (Double)scaleW);

	Double lastScaleW = scaleW;
	Double lastScaleH = scaleH;

	scaleW = lastScaleW;
	scaleH = lastScaleH;

	if (isAlign)
	{
		denv->img->SetTextAlign(Media::DrawEngine::DRAW_POS_TOPLEFT);
		Math::Coord2DDbl currPt = Math::Coord2DDbl(0, 0);
		Math::Coord2DDbl startPt;
		Double tmp;
		Int32 type;
		Math::Size2DDbl szThis;
		Double dlblSize = UOSInt2Double(str1.leng);
		szThis = denv->img->GetTextSize(df, str1);

		if ((szThis.x * absH) < (szThis.y * dlblSize * scaleW))
		{
			scaleW = -scaleW;
			startPt.x = scnPos.x - (tmp = szThis.x * 0.5);
			if (scaleW)
				startPt.y = scnPos.y - (szThis.y * 0.5) - (tmp * scaleH / scaleW);
			else
				startPt.y = scnPos.y - (szThis.y * 0.5);
			type = 0;
		}
		else
		{
			scaleW = -scaleW;
			if (scaleH > 0)
			{
				startPt.y = scnPos.y - (tmp = ((szThis.y * dlblSize) * 0.5));
				startPt.x = scnPos.x - (tmp * scaleW / scaleH);
			}
			else if (scaleH)
			{
				scaleW = -scaleW;
				scaleH = -scaleH;
				startPt.y = scnPos.y - (tmp = ((szThis.y * dlblSize) * 0.5));
				startPt.x = scnPos.x - (tmp * scaleW / scaleH);
			}
			else
			{
				startPt.y = scnPos.y - (tmp = ((szThis.y * dlblSize) * 0.5));
				startPt.x = scnPos.x;
			}
			type = 1;
		}

		UOSInt cnt;
		const UTF8Char *lbl = str1.v;

		if (font && font->buffSize > 0)
		{
			currPt.x = 0;
			currPt.y = 0;

			cnt = str1.leng;

			while (cnt--)
			{
				szThis = denv->img->GetTextSize(font->font, {lbl, 1});

				if (type)
				{
					UTF8Char l[2];
					l[0] = lbl[0];
					l[1] = 0;
					denv->img->DrawStringB(Math::Coord2DDbl(startPt.x + currPt.x - (szThis.x * 0.5), startPt.y + currPt.y), {l, 1}, font->font, font->buffBrush, (UOSInt)Double2Int32(UOSInt2Double(font->buffSize) * denv->img->GetHDPI() / 96.0));

					currPt.y += szThis.y;

					if (scaleH)
						currPt.x = (currPt.y * scaleW / scaleH);
				}
				else
				{
					UTF8Char l[2];
					l[0] = lbl[0];
					l[1] = 0;
					denv->img->DrawStringB(startPt + currPt, {l, 1}, font->font, font->buffBrush, (UOSInt)Double2Int32(UOSInt2Double(font->buffSize) * denv->img->GetHDPI() / 96.0));

					currPt.x += szThis.x;
					if (scaleW)
						currPt.y = (currPt.x * scaleH / scaleW);
				}
				lbl += 1;
			}
		}
		currPt.x = 0;
		currPt.y = 0;
		lbl = str1.v;

		cnt = str1.leng;

		while (cnt--)
		{
			szThis = denv->img->GetTextSize(df, {lbl, 1});

			if (type)
			{
				UTF8Char l[2];
				l[0] = lbl[0];
				l[1] = 0;
				denv->img->DrawString(Math::Coord2DDbl(startPt.x + currPt.x - (szThis.x * 0.5), startPt.y + currPt.y), {l, 1}, df, db);

				currPt.y += szThis.y;

				if (scaleH)
					currPt.x = (currPt.y * scaleW / scaleH);
			}
			else
			{
				UTF8Char l[2];
				l[0] = lbl[0];
				l[1] = 0;
				denv->img->DrawString(startPt + currPt, {l, 1}, df, db);

				currPt.x += szThis.x;
				if (scaleW)
					currPt.y = (currPt.x * scaleH / scaleW);
			}
			lbl += 1;
		}

	}
	else
	{
		denv->img->SetTextAlign(Media::DrawEngine::DRAW_POS_CENTER);
		if (font && font->buffSize > 0)
		{
			denv->img->DrawStringRotB(scnPos, str1, font->font, font->buffBrush, degD * 180 / Math::PI, font->buffSize);
		}
		denv->img->DrawStringRot(scnPos, str1, df, db, degD * 180 / Math::PI);
	}
}

void Map::DrawMapRenderer::DrawCharsL(NotNullPtr<Map::DrawMapRenderer::DrawEnv> denv, Text::CStringNN str1, Math::Coord2DDbl *mapPts, Math::Coord2D<Int32> *scnPts, UOSInt nPoints, UOSInt thisPt, Double scaleN, Double scaleD, Map::MapEnv::FontType fontType, UOSInt fontStyle, Math::RectAreaDbl *realBounds)
{
	UTF8Char sbuff[256];
	str1.ConcatTo(sbuff);
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
	UOSInt j;
	Math::Size2DDbl szThis;
	Math::Size2DDbl szLast;
	Int32 mode;
	Map::DrawMapRenderer::DrawFontStyle *font;
	Media::DrawFont *df;
	Media::DrawBrush *db;

	if (fontType == Map::MapEnv::FontType::GlobalStyle)
	{
		if (fontStyle >= denv->fontStyleCnt)
			return;
		font = &denv->fontStyles[fontStyle];
		df = font->font;
		db = font->fontBrush;
	}
	else if (fontType == Map::MapEnv::FontType::LayerStyle)
	{
		font = 0;
		df = denv->layerFont.GetItem(fontStyle);
		db = denv->layerFontColor.GetItem(fontStyle);
		if (df == 0 || db == 0)
			return;
	}
	else
	{
		return;
	}

	min = max = centPt;

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
		angleOfst = Math::PI;
	}

	currPt = centPt;
	j = thisPt;
	nextPt = diff;
	diff.x = 0;
	diff.y = 0;

	szThis = denv->img->GetTextSize(df, str1);
	diff.x = szThis.x * 0.5;
	diff.y = diff.x * diff.x;

	if (mode == 0)
	{
		while (j != (UOSInt)-1)
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
			if (j == (UOSInt)-1)
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

	if (j >= nPoints - 1)
	{
		j -= 1;
	}
	else if (j == (UOSInt)-1)
	{
		j = 0;
	}
	UOSInt startInd = j;
	denv->img->SetTextAlign(Media::DrawEngine::DRAW_POS_CENTER);

	////////////////////////////////
	UTF8Char *lbl = sbuff;
	UTF8Char *nextPos = lbl;
	UTF8Char nextChar = *lbl;
	Double angle;
	Double cosAngle;
	Double sinAngle;
	Double dist;
	Double angleDegree;
	Double lastAngle;
	Double lastX;
	Double lastY;
	UTF32Char u32c;

	szLast.x = 0;

	lastX = currPt.x = startX;
	lastY = currPt.y = startY;
	j = startInd;
	UOSInt lastInd = j;

	angle = angleOfst - Math_ArcTan2((mapPts[j].y - mapPts[j + 1].y), (mapPts[j + 1].x - mapPts[j].x));
	angleDegree = angle * 180.0 / Math::PI;
	cosAngle = Math_Cos(angle);
	sinAngle = Math_Sin(angle);
	lastAngle = angleDegree;

	while (nextChar)
	{
		lbl = nextPos;
		*lbl = nextChar;

		while (true)
		{
			nextChar = *nextPos;
			if (nextChar == 0)
				break;
			if (nextChar == ' ')
			{
				nextPos++;
				nextChar = *nextPos;
				*nextPos = 0;
				break;
			}
			else if (nextChar < 0x80)
			{
				nextPos++;
			}
			else if ((nextChar & 0xe0) == 0xc0)
			{
//				u32c = ((nextChar & 0x1f) << 6) | (nextPos[1] & 0x3f);
				nextPos += 2;
			}
			else if ((nextChar & 0xf0) == 0xe0)
			{
				u32c = (UTF32Char)(((nextChar & 0x0f) << 12) | ((nextPos[1] & 0x3f) << 6) | (nextPos[2] & 0x3f));
				nextPos += 3;
				if (u32c >= 0x3f00 && u32c <= 0x9f00)
				{
					nextChar = *nextPos;
					*nextPos = 0;
					break;
				}
			}
			else if ((nextChar & 0xf8) == 0xf0)
			{
				u32c = (((UTF32Char)nextChar & 0x7) << 18) | (((UTF32Char)nextPos[1] & 0x3f) << 12) | ((nextPos[2] & 0x3f) << 6) | (nextPos[3] & 0x3f);
				nextPos += 4;
				nextChar = *nextPos;
				*nextPos = 0;
				break;
			}
			else if ((nextChar & 0xfc) == 0xf8)
			{
				u32c = (((UTF32Char)nextChar & 0x3) << 24) | (((UTF32Char)nextPos[1] & 0x3f) << 18) | (((UTF32Char)nextPos[2] & 0x3f) << 12) | ((nextPos[3] & 0x3f) << 6) | (nextPos[4] & 0x3f);
				nextPos += 5;
				nextChar = *nextPos;
				*nextPos = 0;
				break;
			}
			else if ((nextChar & 0xfe) == 0xfc)
			{
				u32c = (((UTF32Char)nextChar & 0x1) << 30) | (((UTF32Char)nextPos[1] & 0x3f) << 24) | (((UTF32Char)nextPos[2] & 0x3f) << 18) | (((UTF32Char)nextPos[3] & 0x3f) << 12) | ((nextPos[4] & 0x3f) << 6) | (nextPos[5] & 0x3f);
				nextPos += 6;
				nextChar = *nextPos;
				*nextPos = 0;
				break;
			}
			else
			{
				nextPos++;
			}
			
		}

		szThis = denv->img->GetTextSize(df, CSTRP(lbl, nextPos));
		dist = (szLast.x + szThis.x) * 0.5;
		nextPt.x = currPt.x + (dist * cosAngle);
		nextPt.y = currPt.y - (dist * sinAngle);
		if ( (((nextPt.x > scnPts[j].x) ^ (nextPt.x > scnPts[j + 1].x)) || (nextPt.x == scnPts[j].x) || (nextPt.x == scnPts[j + 1].x)) && (((nextPt.y > scnPts[j].y) ^ (nextPt.y > scnPts[j + 1].y)) || (nextPt.y == scnPts[j].y) || (nextPt.y == scnPts[j + 1].y)))
		{
			currPt = nextPt;
		}
		else
		{
			diff.x = szLast.x + szThis.x;
			diff.y = (diff.x * diff.x) * 0.5;

			if (mode == 0)
			{
				j++;
				while (j < nPoints - 1)
				{
					nextPt = scnPts[j + 1].ToDouble() - currPt;
					diff.x = (nextPt.x * nextPt.x) + (nextPt.y * nextPt.y);
					if (diff.x < diff.y)
					{
						j++;
					}
					else
					{
						aDiff = nextPt.Abs();

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
					nextPt = scnPts[j].ToDouble() - currPt;
					diff.x = (nextPt.x * nextPt.x) + (nextPt.y * nextPt.y);
					if (diff.x < diff.y)
					{

					}
					else
					{
						aDiff = nextPt.Abs();

						if (aDiff.x > aDiff.y)
						{
							if ((scnPts[j].x < scnPts[j + 1].x) ^ (nextPt.x > 0))
							{
								currPt.x = currPt.x + nextPt.x * Math_Sqrt(diff.y) / Math_Sqrt(diff.x);
							}
							else
							{
								currPt.x = currPt.x - nextPt.x * Math_Sqrt(diff.y) / Math_Sqrt(diff.x);
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
								currPt.y = currPt.y + nextPt.y * Math_Sqrt(diff.y) / Math_Sqrt(diff.x);
							}
							else
							{
								currPt.y = currPt.y - nextPt.y * Math_Sqrt(diff.y) / Math_Sqrt(diff.x);
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
				angleDegree = angle * 180.0 / Math::PI;
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
			Double lsa = Math_Sin(lastAngle * Math::PI / 180.0);
			Double lca = Math_Cos(lastAngle * Math::PI / 180.0);
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
				if (font && font->buffSize > 0)
				{
					denv->img->DrawStringRotB(currPt, CSTRP(lbl, nextPos), df, font->buffBrush, lastAngle, font->buffSize);
				}
				denv->img->DrawStringRot(currPt, CSTRP(lbl, nextPos), df, db, lastAngle);
			}
			else
			{
				if (font && font->buffSize > 0)
				{
					denv->img->DrawStringRotB(currPt, CSTRP(lbl, nextPos), df, font->buffBrush, lastAngle, font->buffSize);
				}
				denv->img->DrawStringRot(currPt, CSTRP(lbl, nextPos), df, db, lastAngle);
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
				if (font && font->buffSize > 0)
				{
					denv->img->DrawStringRotB(currPt, CSTRP(lbl, nextPos), df, font->buffBrush, angleDegree, font->buffSize);
				}
				denv->img->DrawStringRot(currPt, CSTRP(lbl, nextPos), df, db, angleDegree);
			}
			else
			{
				if (font && font->buffSize > 0)
				{
					denv->img->DrawStringRotB(currPt, CSTRP(lbl, nextPos), df, font->buffBrush, angleDegree, font->buffSize);
				}
				denv->img->DrawStringRot(currPt, CSTRP(lbl, nextPos), df, db, angleDegree);
			}

		}
		lastX = currPt.x;
		lastY = currPt.y;
		szLast.x = szThis.x;
	}

	realBounds->tl = min;
	realBounds->br = max;
}

void Map::DrawMapRenderer::DrawCharsLA(NotNullPtr<DrawEnv> denv, Text::CStringNN str1, Math::Coord2DDbl *mapPts, Math::Coord2D<Int32> *scnPts, UOSInt nPoints, UOSInt thisPt, Double scaleN, Double scaleD, Map::MapEnv::FontType fontType, UOSInt fontStyle, Math::RectAreaDbl *realBounds)
{
	UTF8Char sbuff[256];
	UOSInt lblSize = str1.leng;
	str1.ConcatTo(sbuff);
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
	Bool found;
	Map::DrawMapRenderer::DrawFontStyle *font;
	Media::DrawFont *df;
	Media::DrawBrush *db;

	if (fontType == Map::MapEnv::FontType::GlobalStyle)
	{
		if (fontStyle >= denv->fontStyleCnt)
			return;
		font = &denv->fontStyles[fontStyle];
		df = font->font;
		db = font->fontBrush;
	}
	else if (fontType == Map::MapEnv::FontType::LayerStyle)
	{
		font = 0;
		df = denv->layerFont.GetItem(fontStyle);
		db = denv->layerFontColor.GetItem(fontStyle);
		if (df == 0 || db == 0)
			return;
	}
	else
	{
		return;
	}

	max = min = centPt;

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
			angleOfst = Math::PI;
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
			angleOfst = Math::PI;
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
		szThis = denv->img->GetTextSize(df, {&str1.v[i], 1});
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

	if (j >= nPoints - 1)
	{
		j -= 1;
	}
	else if (j == (UOSInt)-1)
	{
		j = 0;
	}
	UOSInt startInd = j;
	denv->img->SetTextAlign(Media::DrawEngine::DRAW_POS_CENTER);

	////////////////////////////////
	UTF8Char *lbl = sbuff;
	UTF8Char *nextPos = lbl;
	UTF8Char nextChar = *lbl;
	Double angle;
	Int32 angleDegree;
	Int32 lastAngle;
	UOSInt lastAInd;
	Math::Coord2DDbl lastPt;

	szLast = Math::Size2DDbl(0, 0);

	lastPt = currPt = startPt;
	j = startInd;

	angle = angleOfst - Math_ArcTan2((mapPts[j].y - mapPts[j + 1].y), (mapPts[j + 1].x - mapPts[j].x));
	angleDegree = Double2Int32(angle * 180.0 / Math::PI);
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

		szThis = denv->img->GetTextSize(df, CSTRP(lbl, nextPos));
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
					if (j == (UOSInt)-1)
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
				angleDegree = Double2Int32(angle * 180.0 / Math::PI);
				while (angleDegree < 0)
				{
					angleDegree += 360;
				}
			}
		}

		Int32 angleDiff;
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
			Math::Coord2DDbl scnPt1 = scnPts[lastAInd].ToDouble();
			Math::Coord2DDbl scnPt2 = scnPts[lastAInd + 1].ToDouble();
			Math::Coord2DDbl tempPt = scnPt1 + (scnPt2 - scnPt1) * ((nextPt - scnPt1) / (scnPt2 - scnPt1)).SwapXY();
			tempPt -= lastPt;
			if (tempPt.y < 0)
				tempPt.y = -tempPt.y;
			if (tempPt.x < 0)
				tempPt.x = -tempPt.x;
			if (tempPt.x <= (szLast.x + szThis.x) * 0.5)
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

		lastPt = currPt;
		if (mode == 0)
		{
			if (font && font->buffSize > 0)
			{
				denv->img->DrawStringB(currPt, CSTRP(lbl, nextPos), df, font->buffBrush, (UInt32)Double2Int32(UOSInt2Double(font->buffSize) * denv->img->GetHDPI() / 96.0));
			}
			denv->img->DrawString(currPt, CSTRP(lbl, nextPos), df, db);
		}
		else
		{
			if (font && font->buffSize > 0)
			{
				denv->img->DrawStringB(currPt, CSTRP(lbl, nextPos), df, font->buffBrush, (UInt32)Double2Int32(UOSInt2Double(font->buffSize) * denv->img->GetHDPI() / 96.0));
			}
			denv->img->DrawString(currPt, CSTRP(lbl, nextPos), df, db);
		}
		szLast = szThis;
	}

	realBounds->tl = min;
	realBounds->br = max;
}

Map::DrawMapRenderer::DrawMapRenderer(NotNullPtr<Media::DrawEngine> eng, Map::MapEnv *env, NotNullPtr<const Media::ColorProfile> color, Media::ColorManagerSess *colorSess, DrawType drawType)
{
	this->eng = eng;
	this->env = env;
	this->color.Set(color);
	this->colorSess = colorSess;
	this->lastLayerEmpty = true;
	this->drawType = drawType;
	Media::ColorProfile srcColor(Media::ColorProfile::CPT_SRGB);
	NEW_CLASS(this->resizer, Media::Resizer::LanczosResizer8_C8(3, 3, srcColor, this->color, colorSess, Media::AT_NO_ALPHA));
	NEW_CLASS(this->colorConv, Media::ColorConv(srcColor, this->color, colorSess));
}

Map::DrawMapRenderer::~DrawMapRenderer()
{
	DEL_CLASS(this->resizer);
	DEL_CLASS(this->colorConv);
}

void Map::DrawMapRenderer::DrawMap(NotNullPtr<Media::DrawImage> img, Map::MapView *view, UInt32 *imgDurMS)
{
	Map::DrawMapRenderer::DrawEnv denv;
	UOSInt i;
	Map::DrawMapRenderer::DrawFontStyle *font;

/*	Media::DrawBrush *b = img->NewBrushARGB(this->env->GetBGColor());
	img->DrawRect(0, 0, view->GetScnWidth(), view->GetScnHeight(), 0, b);
	img->DelBrush(b);*/

	this->mapSch.SetMapView(view, img);

	denv.env = env;
	denv.img = img;
	denv.view = view;
	denv.isLayerEmpty = true;
	denv.objBounds = MemAllocA(Math::RectAreaDbl, this->env->GetNString());
	denv.objCnt = 0;
	denv.labelCnt = 0;
	denv.labels = MemAllocA(Map::DrawMapRenderer::MapLabels, denv.maxLabels = this->env->GetNString());
	MemClear(denv.labels, denv.maxLabels * sizeof(Map::DrawMapRenderer::MapLabels));
	denv.fontStyleCnt = env->GetFontStyleCount();
	denv.fontStyles = MemAlloc(Map::DrawMapRenderer::DrawFontStyle, denv.fontStyleCnt);
	denv.imgDurMS = 0;
	denv.dispSize = Math::Size2DDbl(UOSInt2Double(img->GetWidth()), UOSInt2Double(img->GetHeight()));
	i = denv.fontStyleCnt;
	while (i-- > 0)
	{
		Text::String *fontName;
		Double fontSizePt;
		Bool bold;
		UInt32 fontColor;
		UOSInt buffSize;
		UInt32 buffColor;

		font = &denv.fontStyles[i];
		env->GetFontStyle(i, fontName, fontSizePt, bold, fontColor, buffSize, buffColor);
		font->font = img->NewFontPt(fontName->ToCString(), fontSizePt, bold?Media::DrawEngine::DFS_BOLD:Media::DrawEngine::DFS_NORMAL, 0);
		font->fontBrush = img->NewBrushARGB(this->colorConv->ConvRGB8(fontColor));
		font->buffSize = buffSize;
		if (buffSize > 0)
		{
			font->buffBrush = img->NewBrushARGB(this->colorConv->ConvRGB8(buffColor));
		}
		else
		{
			font->buffBrush = 0;
		}
	}

	this->DrawLayers(denv, 0);
	DrawLabels(denv);

	i = denv.fontStyleCnt;
	while (i-- > 0)
	{
		font = &denv.fontStyles[i];
		img->DelBrush(font->fontBrush);
		img->DelFont(font->font);
		if (font->buffBrush)
		{
			img->DelBrush(font->buffBrush);
		}
	}
	i = denv.layerFont.GetCount();
	while (i-- > 0)
	{
		img->DelFont(denv.layerFont.GetItem(i));
		img->DelBrush(denv.layerFontColor.GetItem(i));
	}
	MemFree(denv.fontStyles);
	MemFreeA(denv.objBounds);
	MemFreeA(denv.labels);

	this->lastLayerEmpty = denv.isLayerEmpty;
	if (imgDurMS)
	{
		*imgDurMS = denv.imgDurMS;
	}
}

void Map::DrawMapRenderer::SetUpdatedHandler(Map::MapRenderer::UpdatedHandler updHdlr, void *userObj)
{
}

Bool Map::DrawMapRenderer::GetLastsLayerEmpty()
{
	return this->lastLayerEmpty;
}

void Map::DrawMapRenderer::SetEnv(Map::MapEnv *env)
{
	this->env = env;
}

void Map::DrawMapRenderer::ColorUpdated()
{
	if (this->colorSess)
	{
		this->resizer->RGBParamChanged(this->colorSess->GetRGBParam());
		this->colorConv->RGBParamChanged(this->colorSess->GetRGBParam());
	}
}
