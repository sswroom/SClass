#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/Sort/ArtificialQuickSort.h"
#include "Map/DrawMapRenderer.h"
#include "Math/Math.h"
#include "Math/Geometry.h"
#include "Math/VectorImage.h"
#include "Media/Resizer/LanczosResizer8_C8.h"
#include "Text/MyString.h"

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

void Map::DrawMapRenderer::SwapLabel(MapLabels *mapLabels, UOSInt index, UOSInt index2)
{
	MapLabels l;
	MemCopyNO(&l, &mapLabels[index], sizeof(MapLabels));
	MemCopyNO(&mapLabels[index], &mapLabels[index2], sizeof(MapLabels));
	MemCopyNO(&mapLabels[index2], &l, sizeof(MapLabels));
}

Bool Map::DrawMapRenderer::LabelOverlapped(Double *points, UOSInt nPoints, Double tlx, Double tly, Double brx, Double bry)
{
	while (nPoints--)
	{
		if (points[(nPoints << 2) + 0] < brx && points[(nPoints << 2) + 2] > tlx && points[(nPoints << 2) + 1] < bry && points[(nPoints << 2) + 3] > tly)
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

					labels[i].label = Text::String::New(label);
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
				ptPtr += 2;
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
						Math::Coord2DDbl* newArr = MemAlloc(Math::Coord2DDbl, newSize);

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

			labels[i].label = Text::String::New(label);
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
			outPtCnt = Math::Geometry::BoundPolygonY(points, nPoints, tmpPts, top, bottom, {0, 0});
			outPts = MemAllocA(Math::Coord2DDbl, nPoints << 1);
			outPtCnt = Math::Geometry::BoundPolygonX(tmpPts, outPtCnt, outPts, left, right, {0, 0});
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

				labels[i].label = Text::String::New(label);
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

void Map::DrawMapRenderer::DrawLabels(Map::DrawMapRenderer::DrawEnv *denv)
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
		Double szThis[2];
		UOSInt currPt;

		Double tlx;
		Double tly;
		Double brx;
		Double bry;
		Double scnPtX;
		Double scnPtY;
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
				GetCharsSize(denv, szThis, denv->labels[i].label->ToCString(), denv->labels[i].fontType, denv->labels[i].fontStyle, 0, 0);//labels[i].scaleW, labels[i].scaleH);
				dscnPos = denv->view->MapXYToScnXY(denv->labels[i].pos);
				scnPtX = dscnPos.x;
				scnPtY = dscnPos.y;

				overlapped = true;
				if (denv->labels[i].xOfst == 0)
				{
					tlx = scnPtX - (szThis[0] * 0.5);
					brx = tlx + szThis[0];
					tly = scnPtY - (szThis[1] * 0.5);
					bry = tly + szThis[1];

					overlapped = LabelOverlapped(denv->objBounds, currPt, tlx, tly, brx, bry);
				}
				if (overlapped)
				{
					tlx = scnPtX + 1 + OSInt2Double(denv->labels[i].xOfst >> 1);
					brx = tlx + szThis[0];
					tly = scnPtY - (szThis[1] * 0.5);
					bry = tly + szThis[1];

					overlapped = LabelOverlapped(denv->objBounds, currPt, tlx, tly, brx, bry);
				}
				if (overlapped)
				{
					tlx = scnPtX - szThis[0] - 1 - OSInt2Double(denv->labels[i].xOfst >> 1);
					brx = tlx + szThis[0];
					tly = scnPtY - (szThis[1] * 0.5);
					bry = tly + szThis[1];

					overlapped = LabelOverlapped(denv->objBounds, currPt, tlx, tly, brx, bry);
				}
				if (overlapped)
				{
					tlx = scnPtX - (szThis[0] * 0.5);
					brx = tlx + szThis[0];
					tly = scnPtY - szThis[1] - 1 - OSInt2Double(denv->labels[i].yOfst >> 1);
					bry = tly + szThis[1];

					overlapped = LabelOverlapped(denv->objBounds, currPt, tlx, tly, brx, bry);
				}
				if (overlapped)
				{
					tlx = scnPtX - (szThis[0] * 0.5);
					brx = tlx + szThis[0];
					tly = scnPtY + 1 + OSInt2Double(denv->labels[i].yOfst >> 1);
					bry = tly + szThis[1];

					overlapped = LabelOverlapped(denv->objBounds, currPt, tlx, tly, brx, bry);
				}

				if (!overlapped)
				{
					DrawChars(denv, denv->labels[i].label->ToCString(), (tlx + brx) * 0.5, (tly + bry) * 0.5, 0, 0, denv->labels[i].fontType, denv->labels[i].fontStyle, 0);

					denv->objBounds[(currPt << 2)] = tlx;
					denv->objBounds[(currPt << 2) + 1] = tly;
					denv->objBounds[(currPt << 2) + 2] = brx;
					denv->objBounds[(currPt << 2) + 3] = bry;
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
					lastLbl = denv->labels[i].label->Clone();
				}
				else
				{
				//	MessageBoxW(NULL, L"Test", lastLbl, MB_OK);
				}

				Int32 *points = MemAlloc(Int32, denv->labels[i].nPoints << 1);
				denv->view->MapXYToScnXY(denv->labels[i].points, points, denv->labels[i].nPoints, 0, 0);
				OSInt minX = 0;
				OSInt minY = 0;
				OSInt maxX = 0;
				OSInt maxY = 0;
				OSInt xDiff;
				OSInt yDiff;
				Double scaleN;
				Double scaleD;
				OSInt lastX;
				OSInt lastY;
				OSInt thisX;
				OSInt thisY;
				UOSInt k;
				Bool hasPoint;

				j = (denv->labels[i].nPoints << 1) - 2;
				lastX = points[j];
				lastY = points[j + 1];
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
					j = (UOSInt)((denv->labels[i].nPoints - 1) & (UOSInt)~1);
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
				}
				xDiff = maxX - minX;
				yDiff = maxY - minY;
				scaleN = 0;
				scaleD = 1;

				if (xDiff > yDiff)
				{
					scnPtX = OSInt2Double((maxX + minX) >> 1);
					k = 0;
					while (k < denv->labels[i].nPoints - 1)
					{
						if (points[k << 1] >= scnPtX && points[(k << 1) + 2] <= scnPtX)
						{
							scaleD = points[(k << 1) + 0] - points[(k << 1) + 2];
							scaleN = (points[k << 1] - scnPtX);
							break;
						}
						else if (points[k << 1] <= scnPtX && points[(k << 1) + 2] >= scnPtX)
						{
							scaleD = points[(k << 1) + 2] - points[(k << 1)];
							scaleN = scnPtX - points[k << 1];
							break;
						}
						k++;
					}

				}
				else
				{
					scnPtY = OSInt2Double((maxY + minY) >> 1);
					k = 0;
					while (k < denv->labels[i].nPoints - 1)
					{
						if (points[(k << 1) + 1] >= scnPtY && points[(k << 1) + 3] <= scnPtY)
						{
							scaleD = points[(k << 1) + 1] - points[(k << 1) + 3];
							scaleN = (points[(k << 1) + 1] - scnPtY);
							break;
						}
						else if (points[(k << 1) + 1] <= scnPtY && points[(k << 1) + 3] >= scnPtY)
						{
							scaleD = points[(k << 1) + 3] - points[(k << 1) + 1];
							scaleN = scnPtY - points[(k << 1) + 1];
							break;
						}
						k++;
					}
				}
				scnPtX = points[(k << 1)] + ((points[(k << 1) + 2] - points[(k << 1)]) * scaleN / scaleD);
				scnPtY = points[(k << 1) + 1] + ((points[(k << 1) + 3] - points[(k << 1) + 1]) * scaleN / scaleD);
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
				GetCharsSize(denv, szThis, denv->labels[i].label->ToCString(), denv->labels[i].fontType, denv->labels[i].fontStyle, denv->labels[i].scaleW, denv->labels[i].scaleH);
				if (OSInt2Double(xDiff) < szThis[0] && OSInt2Double(yDiff) < szThis[1])
				{
					tlx = scnPtX - (szThis[0] * 0.5);
					brx = tlx + szThis[0];
					tly = scnPtY - (szThis[1] * 0.5);
					bry = tly + szThis[1];

					overlapped = LabelOverlapped(denv->objBounds, currPt, tlx, tly, brx, bry);
					if (!overlapped)
					{
						DrawChars(denv, denv->labels[i].label->ToCString(), (tlx + brx) * 0.5, (tly + bry) * 0.5, denv->labels[i].scaleW, denv->labels[i].scaleH, denv->labels[i].fontType, denv->labels[i].fontStyle, (denv->labels[i].flags & Map::MapEnv::SFLG_ALIGN) != 0);

						denv->objBounds[(currPt << 2)] = tlx;
						denv->objBounds[(currPt << 2) + 1] = tly;
						denv->objBounds[(currPt << 2) + 2] = brx;
						denv->objBounds[(currPt << 2) + 3] = bry;
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
						tlx = scnPtX - (szThis[0] * 0.5);
						brx = tlx + szThis[0];
						tly = scnPtY - (szThis[1] * 0.5);
						bry = tly + szThis[1];

						overlapped = LabelOverlapped(denv->objBounds, currPt, tlx, tly, brx, bry);
						if (!overlapped || --tryCnt <= 0)
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
									scnPtX += OSInt2Double(scnDiff) - 1;
									if (scnPtX <= OSInt2Double(minX))
									{
										scnDiff = -scnDiff;
										scnPtX = OSInt2Double(((minX + maxX) >> 1) + scnDiff + 1);
									}
								}
								else
								{
									scnPtX += OSInt2Double(scnDiff) + 1;
									if (scnPtX >= OSInt2Double(maxX))
									{
										scnDiff = scnDiff >> 1;
										if (scnDiff < 30)
											break;
										scnDiff = -scnDiff;
										scnPtX = OSInt2Double(((minX + maxX) >> 1) + scnDiff - 1);
									}
								}

								k = 0;
								while (k < denv->labels[i].nPoints - 1)
								{
									if (points[k << 1] >= scnPtX && points[(k << 1) + 2] <= scnPtX)
									{
										scaleD = points[(k << 1) + 0] - points[(k << 1) + 2];
										scaleN = (points[k << 1] - scnPtX);
										break;
									}
									else if (points[k << 1] <= scnPtX && points[(k << 1) + 2] >= scnPtX)
									{
										scaleD = points[(k << 1) + 2] - points[(k << 1)];
										scaleN = scnPtX - points[k << 1];
										break;
									}
									k++;
								}

							}
							else
							{
								if (scnDiff < 0)
								{
									scnPtY += OSInt2Double(scnDiff - 1);
									if (scnPtY <= OSInt2Double(minY))
									{
										scnDiff = -scnDiff;
										scnPtY = OSInt2Double(((minY + maxY) >> 1) + scnDiff + 1);
									}
								}
								else
								{
									scnPtY += OSInt2Double(scnDiff + 1);
									if (scnPtY >= OSInt2Double(maxY))
									{
										scnDiff = scnDiff >> 1;
										if (scnDiff < 30)
											break;
										scnDiff = -scnDiff;
										scnPtY = OSInt2Double(((minY + maxY) >> 1) + scnDiff - 1);
									}
								}

								k = 0;
								while (k < denv->labels[i].nPoints - 1)
								{
									if (points[(k << 1) + 1] >= scnPtY && points[(k << 1) + 3] <= scnPtY)
									{
										scaleD = points[(k << 1) + 1] - points[(k << 1) + 3];
										scaleN = (points[(k << 1) + 1] - scnPtY);
										break;
									}
									else if (points[(k << 1) + 1] <= scnPtY && points[(k << 1) + 3] >= scnPtY)
									{
										scaleD = points[(k << 1) + 3] - points[(k << 1) + 1];
										scaleN = scnPtY - points[(k << 1) + 1];
										break;
									}
									k++;
								}
							}
							scnPtX = points[(k << 1)] + ((points[(k << 1) + 2] - points[(k << 1)]) * scaleN / scaleD);
							scnPtY = points[(k << 1) + 1] + ((points[(k << 1) + 3] - points[(k << 1) + 1]) * scaleN / scaleD);
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
							GetCharsSize(denv, szThis, denv->labels[i].label->ToCString(), denv->labels[i].fontType, denv->labels[i].fontStyle, denv->labels[i].scaleW, denv->labels[i].scaleH);
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
							if (OSInt2Double(tmpV - LBLMINDIST) < bry && OSInt2Double(tmpV + LBLMINDIST) > tly)
							{
								n++;
							}
							tmpV = thisPts[--m];
							if (OSInt2Double(tmpV - LBLMINDIST) < brx && OSInt2Double(tmpV + LBLMINDIST) > tlx)
							{
								n++;
							}
							n = (n != 2);
						}

						if (n)
						{
							if ((denv->labels[i].flags & Map::MapEnv::SFLG_ALIGN) != 0)
							{
								Double realBounds[4];
								DrawCharsLA(denv, denv->labels[i].label->ToCString(), denv->labels[i].points, points, denv->labels[i].nPoints, k, scaleN, scaleD, denv->labels[i].fontType, denv->labels[i].fontStyle, realBounds);

								denv->objBounds[(currPt << 2)] = realBounds[0];
								denv->objBounds[(currPt << 2) + 1] = realBounds[1];
								denv->objBounds[(currPt << 2) + 2] = realBounds[2];
								denv->objBounds[(currPt << 2) + 3] = realBounds[3];
								currPt++;
							}
							else if ((denv->labels[i].flags & Map::MapEnv::SFLG_ROTATE) != 0)
							{
								Double realBounds[4];
								DrawCharsL(denv, denv->labels[i].label->ToCString(), denv->labels[i].points, points, denv->labels[i].nPoints, k, scaleN, scaleD, denv->labels[i].fontType, denv->labels[i].fontStyle, realBounds);

								denv->objBounds[(currPt << 2)] = realBounds[0];
								denv->objBounds[(currPt << 2) + 1] = realBounds[1];
								denv->objBounds[(currPt << 2) + 2] = realBounds[2];
								denv->objBounds[(currPt << 2) + 3] = realBounds[3];
								currPt++;
							}
							else
							{
								DrawChars(denv, denv->labels[i].label->ToCString(), (tlx + brx) * 0.5, (tly + bry) * 0.5, denv->labels[i].scaleW, denv->labels[i].scaleH, denv->labels[i].fontType, denv->labels[i].fontStyle, (denv->labels[i].flags & Map::MapEnv::SFLG_ALIGN) != 0);

								denv->objBounds[(currPt << 2)] = tlx;
								denv->objBounds[(currPt << 2) + 1] = tly;
								denv->objBounds[(currPt << 2) + 2] = brx;
								denv->objBounds[(currPt << 2) + 3] = bry;
								currPt++;
							}

							thisPts[thisCnt++] = Double2Int32((tlx + brx) * 0.5);
							thisPts[thisCnt++] = Double2Int32((tly + bry) * 0.5);
						}
						j = (thisCnt < 10);
					}
				}
				MemFree(points);
			}
			else if (denv->labels[i].layerType == Map::DRAW_LAYER_POLYGON)
			{
				GetCharsSize(denv, szThis, denv->labels[i].label->ToCString(), denv->labels[i].fontType, denv->labels[i].fontStyle, 0, 0);//labels[i].scaleW, labels[i].scaleH);
				dscnPos = denv->view->MapXYToScnXY(denv->labels[i].pos);
				scnPtX = dscnPos.x;
				scnPtY = dscnPos.y;

				overlapped = true;
				if (overlapped)
				{
					tlx = scnPtX  - (szThis[0] * 0.5);
					brx = tlx + szThis[0];
					tly = scnPtY - (szThis[1] * 0.5);
					bry = tly + szThis[1];

					overlapped = LabelOverlapped(denv->objBounds, currPt, tlx, tly, brx, bry);
				}
				if (overlapped)
				{
					tlx = scnPtX + 1;
					brx = tlx + szThis[0];
					tly = scnPtY - (szThis[1] * 0.5);
					bry = tly + szThis[1];

					overlapped = LabelOverlapped(denv->objBounds, currPt, tlx, tly, brx, bry);
				}
				if (overlapped)
				{
					tlx = scnPtX - szThis[0] - 1;
					brx = tlx + szThis[0];
					tly = scnPtY - (szThis[1] * 0.5);
					bry = tly + szThis[1];

					overlapped = LabelOverlapped(denv->objBounds, currPt, tlx, tly, brx, bry);
				}
				if (overlapped)
				{
					tlx = scnPtX - (szThis[0] * 0.5);
					brx = tlx + szThis[0];
					tly = scnPtY - szThis[1] - 1;
					bry = tly + szThis[1];

					overlapped = LabelOverlapped(denv->objBounds, currPt, tlx, tly, brx, bry);
				}
				if (overlapped)
				{
					tlx = scnPtX - (szThis[0] * 0.5);
					brx = tlx + szThis[0];
					tly = scnPtY + 1;
					bry = tly + szThis[1];

					overlapped = LabelOverlapped(denv->objBounds, currPt, tlx, tly, brx, bry);
				}

				if (!overlapped)
				{
					DrawChars(denv, denv->labels[i].label->ToCString(), (tlx + brx) * 0.5, (tly + bry) * 0.5, 0, 0, denv->labels[i].fontType, denv->labels[i].fontStyle, 0);

					denv->objBounds[(currPt << 2)] = tlx;
					denv->objBounds[(currPt << 2) + 1] = tly;
					denv->objBounds[(currPt << 2) + 2] = brx;
					denv->objBounds[(currPt << 2) + 3] = bry;
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
			MemFree(denv->labels[i].points);
	}
	if (lastLbl)
		lastLbl->Release();
}

OSInt Map::DrawMapRenderer::VImgCompare(void *obj1, void *obj2)
{
	Math::VectorImage *vimg1 = (Math::VectorImage*)obj1;
	Math::VectorImage *vimg2 = (Math::VectorImage*)obj2;
	Bool type1 = vimg1->IsScnCoord();
	Bool type2 = vimg2->IsScnCoord();
	if (type1 != type2)
	{
		if (type1)
			return 1;
		else
			return -1;
	}
	Int32 zIndex1 = vimg1->GetZIndex();
	Int32 zIndex2 = vimg2->GetZIndex();
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
	coord1 = vimg1->GetCenter();
	coord2 = vimg2->GetCenter();
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

void Map::DrawMapRenderer::DrawLayers(Map::DrawMapRenderer::DrawEnv *denv, Map::MapEnv::GroupItem *group)
{
	Map::MapEnv::LayerItem layer;
	Map::MapEnv::MapItem *item;
	Sync::MutexUsage mutUsage(0);
	denv->env->BeginUse(&mutUsage);
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
			if (denv->env->GetLayerProp(&layer, group, i))
			{
				if (layer.minScale <= scale && layer.maxScale >= scale)
				{
					Map::DrawLayerType layerType = layer.layer->GetLayerType();
					if (layerType == Map::DRAW_LAYER_POLYLINE || layerType == Map::DRAW_LAYER_POLYLINE3D)
					{
						if ((layer.flags & Map::MapEnv::SFLG_HIDESHAPE) == 0)
						{
							if (layer.lineType == 0)
							{
								DrawPLLayer(denv, layer.layer, layer.lineStyle, layer.lineThick, layer.lineColor);
							}
							else
							{
								DrawPLLayer(denv, layer.layer, (UOSInt)-1, layer.lineThick, layer.lineColor);
							}

						}
						if (layer.flags & Map::MapEnv::SFLG_SHOWLABEL)
						{
							if (layer.fontType == Map::MapEnv::FontType::GlobalStyle)
							{
								if (layer.fontStyle < denv->fontStyleCnt)
								{
									DrawLabel(denv, layer.layer, layer.fontStyle, layer.labelCol, layer.priority, layer.flags, 0, 0, layerType, layer.fontType);
								}
							}
							else if (layer.fontType == Map::MapEnv::FontType::LayerStyle)
							{
								UOSInt fs = denv->layerFont.GetCount();
								Media::DrawFont *f = denv->img->NewFontPt(layer.fontName->ToCString(), layer.fontSizePt, Media::DrawEngine::DFS_NORMAL, 0);
								Media::DrawBrush *b = denv->img->NewBrushARGB(this->colorConv->ConvRGB8(layer.fontColor));
								denv->layerFont.Add(f);
								denv->layerFontColor.Add(b);
								DrawLabel(denv, layer.layer, fs, layer.labelCol, layer.priority, layer.flags, 0, 0, layerType, layer.fontType);
							}
						}
					}
					else if (layerType == Map::DRAW_LAYER_POLYGON)
					{
						if ((layer.flags & Map::MapEnv::SFLG_HIDESHAPE) == 0)
						{
							if (layer.lineType == 0)
							{
								DrawPGLayer(denv, layer.layer, layer.lineStyle, layer.fillStyle, layer.lineThick, layer.lineColor);
							}
							else
							{
								DrawPGLayer(denv, layer.layer, (UOSInt)-1, layer.fillStyle, layer.lineThick, layer.lineColor);
							}
						}
						if (layer.flags & Map::MapEnv::SFLG_SHOWLABEL)
						{
							if (layer.fontType == Map::MapEnv::FontType::GlobalStyle)
							{
								if (layer.fontStyle < denv->fontStyleCnt)
								{
									DrawLabel(denv, layer.layer, layer.fontStyle, layer.labelCol, layer.priority, layer.flags, 0, 0, layerType, layer.fontType);
								}
							}
							else if (layer.fontType == Map::MapEnv::FontType::LayerStyle)
							{
								UOSInt fs = denv->layerFont.GetCount();
								Media::DrawFont *f = denv->img->NewFontPt(layer.fontName->ToCString(), layer.fontSizePt, Media::DrawEngine::DFS_NORMAL, 0);
								Media::DrawBrush *b = denv->img->NewBrushARGB(this->colorConv->ConvRGB8(layer.fontColor));
								denv->layerFont.Add(f);
								denv->layerFontColor.Add(b);
								DrawLabel(denv, layer.layer, fs, layer.labelCol, layer.priority, layer.flags, 0, 0, layerType, layer.fontType);
							}
						}
					}
					else if (layerType == Map::DRAW_LAYER_POINT || layerType == Map::DRAW_LAYER_POINT3D)
					{
						if ((layer.flags & Map::MapEnv::SFLG_HIDESHAPE) == 0)
							DrawPTLayer(denv, layer.layer, layer.imgIndex);
						if (layer.flags & Map::MapEnv::SFLG_SHOWLABEL)
						{
							Media::Image *pimg = 0;
							Double spotX;
							Double spotY;
							UInt32 imgDurMS = 0;
							if (layer.layer->HasIconStyle())
							{
								pimg = layer.layer->GetIconStyleImg()->GetImage(&imgDurMS);
								spotX = OSInt2Double(layer.layer->GetIconStyleSpotX());
								spotY = OSInt2Double(layer.layer->GetIconStyleSpotY());
								if (pimg != 0 && (spotX == -1 || spotY == -1))
								{
									spotX = UOSInt2Double(pimg->info.dispWidth) * 0.5;
									spotY = UOSInt2Double(pimg->info.dispHeight) * 0.5;
								}
							}
							if (pimg == 0)
							{
								pimg = denv->env->GetImage(layer.imgIndex, &imgDurMS);
								if (pimg)
								{
									spotX = UOSInt2Double(pimg->info.dispWidth) * 0.5;
									spotY = UOSInt2Double(pimg->info.dispHeight) * 0.5;
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
										DrawLabel(denv, layer.layer, layer.fontStyle, layer.labelCol, layer.priority, layer.flags, (UOSInt)Double2Int32(UOSInt2Double(pimg->info.dispWidth) * denv->img->GetHDPI() / pimg->info.hdpi), (UOSInt)Double2Int32(UOSInt2Double(pimg->info.dispHeight) * denv->img->GetVDPI() / pimg->info.vdpi), layerType, layer.fontType);
									}
									else
									{
										DrawLabel(denv, layer.layer, layer.fontStyle, layer.labelCol, layer.priority, layer.flags, 0, 0, layerType, layer.fontType);
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
									DrawLabel(denv, layer.layer, fs, layer.labelCol, layer.priority, layer.flags, (UOSInt)Double2Int32(UOSInt2Double(pimg->info.dispWidth) * denv->img->GetHDPI() / pimg->info.hdpi), (UOSInt)Double2Int32(UOSInt2Double(pimg->info.dispHeight) * denv->img->GetVDPI() / pimg->info.vdpi), layerType, layer.fontType);
								}
								else
								{
									DrawLabel(denv, layer.layer, fs, layer.labelCol, layer.priority, layer.flags, 0, 0, layerType, layer.fontType);
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
								layer.layer->SetMixedType(Map::DRAW_LAYER_POLYLINE3D);
								DrawPLLayer(denv, layer.layer, layer.lineStyle, layer.lineThick, layer.lineColor);
								layer.layer->SetMixedType(Map::DRAW_LAYER_POLYGON);
								DrawPGLayer(denv, layer.layer, layer.lineStyle, layer.fillStyle, layer.lineThick, layer.lineColor);
							}
							else
							{
								layer.layer->SetMixedType(Map::DRAW_LAYER_POLYLINE3D);
								DrawPLLayer(denv, layer.layer, (UOSInt)-1, layer.lineThick, layer.lineColor);
								layer.layer->SetMixedType(Map::DRAW_LAYER_POLYGON);
								DrawPGLayer(denv, layer.layer, (UOSInt)-1, layer.fillStyle, layer.lineThick, layer.lineColor);
							}
						}
						if (layer.flags & Map::MapEnv::SFLG_SHOWLABEL)
						{
							if (layer.fontType == Map::MapEnv::FontType::GlobalStyle)
							{
								layer.layer->SetMixedType(Map::DRAW_LAYER_POLYLINE3D);
								if (layer.fontStyle < denv->fontStyleCnt)
								{
									DrawLabel(denv, layer.layer, layer.fontStyle, layer.labelCol, layer.priority, layer.flags, 0, 0, Map::DRAW_LAYER_POLYLINE3D, layer.fontType);
								}
								layer.layer->SetMixedType(Map::DRAW_LAYER_POLYGON);
								if (layer.fontStyle < denv->fontStyleCnt)
								{
									DrawLabel(denv, layer.layer, layer.fontStyle, layer.labelCol, layer.priority, layer.flags, 0, 0, Map::DRAW_LAYER_POLYGON, layer.fontType);
								}
							}
							else if (layer.fontType == Map::MapEnv::FontType::LayerStyle)
							{
								UOSInt fs = denv->layerFont.GetCount();
								Media::DrawFont *f = denv->img->NewFontPt(layer.fontName->ToCString(), layer.fontSizePt, Media::DrawEngine::DFS_NORMAL, 0);
								Media::DrawBrush *b = denv->img->NewBrushARGB(this->colorConv->ConvRGB8(layer.fontColor));
								denv->layerFont.Add(f);
								denv->layerFontColor.Add(b);
								layer.layer->SetMixedType(Map::DRAW_LAYER_POLYLINE3D);
								DrawLabel(denv, layer.layer, fs, layer.labelCol, layer.priority, layer.flags, 0, 0, Map::DRAW_LAYER_POLYLINE3D, layer.fontType);
								layer.layer->SetMixedType(Map::DRAW_LAYER_POLYGON);
								DrawLabel(denv, layer.layer, fs, layer.labelCol, layer.priority, layer.flags, 0, 0, Map::DRAW_LAYER_POLYGON, layer.fontType);
							}
						}
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

void Map::DrawMapRenderer::DrawPLLayer(Map::DrawMapRenderer::DrawEnv *denv, Map::IMapDrawLayer *layer, UOSInt lineStyle, UOSInt lineThick, UInt32 lineColor)
{
	UOSInt i;
	UOSInt j;
	UOSInt k;
	void *session;
	Media::DrawPen *p;
	Int64 lastId;
	Int64 thisId;
	UOSInt layerId = 0;
	Map::DrawObjectL *dobj;
	Math::CoordinateSystem *lyrCSys = layer->GetCoordinateSystem();
	Math::CoordinateSystem *envCSys = this->env->GetCoordinateSystem();

	denv->idArr.Clear();
	if (lyrCSys != 0 && envCSys != 0 && !lyrCSys->Equals(envCSys))
	{
		Math::Coord2DDbl tl;
		Math::Coord2DDbl br;
		Math::RectAreaDbl rect = denv->view->GetVerticalRect();
		tl = rect.tl;
		br = rect.br;
		Math::CoordinateSystem::ConvertXYZ(envCSys, lyrCSys, tl.x, tl.y, 0, &tl.x, &tl.y, 0);
		Math::CoordinateSystem::ConvertXYZ(envCSys, lyrCSys, br.x, br.y, 0, &br.x, &br.y, 0);
		layer->GetObjectIdsMapXY(&denv->idArr, 0, Math::RectAreaDbl(tl, br), true);

		if ((i = denv->idArr.GetCount()) > 0)
		{
			UInt32 color;
			UOSInt thick;
			UInt8 *pattern;
			UOSInt npattern;
			Bool found;
			found = denv->env->GetLineStyleLayer(lineStyle, layerId++, &color, &thick, &pattern, &npattern);
			if (!found)
			{
				thick = lineThick;
				color = lineColor;
				npattern = 0;
			}
			p = denv->img->NewPenARGB(this->colorConv->ConvRGB8(color), UOSInt2Double(thick) * denv->img->GetHDPI() / 96.0, pattern, npattern);
			//pen = CreatePen(img, lyrs->style, 0, osSize);
			this->mapSch.SetDrawType(layer, Map::MapScheduler::MSDT_POLYLINE, p, 0, 0, 0.0, 0.0, &denv->isLayerEmpty);

			session = layer->BeginGetObject();
			lastId = -1;
			j = i;
			i = 0;
			while (i < j)
			{
				thisId = denv->idArr.GetItem(i);
				if (thisId != lastId)
				{
					lastId = thisId;
					if ((dobj = layer->GetNewObjectById(session, thisId)) != 0)
					{
						k = dobj->nPoint;
						while (k-- > 0)
						{
							Math::CoordinateSystem::ConvertXYZ(lyrCSys, envCSys, dobj->pointArr[k].x, dobj->pointArr[k].y, 0, &dobj->pointArr[k].x, &dobj->pointArr[k].y, 0);
						}
						this->mapSch.Draw(dobj);
					}
				}
				i++;
			}
			layer->EndGetObject(session);

			if (found)
			{
				while (denv->env->GetLineStyleLayer(lineStyle, layerId++, &color, &thick, &pattern, &npattern))
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
		br = rect.GetBR();
		layer->GetObjectIdsMapXY(&denv->idArr, 0, Math::RectAreaDbl(tl, br), true);

		if ((i = denv->idArr.GetCount()) > 0)
		{
			UInt32 color;
			UOSInt thick;
			UInt8 *pattern;
			UOSInt npattern;
			Bool found;
			found = denv->env->GetLineStyleLayer(lineStyle, layerId++, &color, &thick, &pattern, &npattern);
			if (!found)
			{
				thick = lineThick;
				color = lineColor;
				npattern = 0;
			}
			p = denv->img->NewPenARGB(this->colorConv->ConvRGB8(color), UOSInt2Double(thick) * denv->img->GetHDPI() / 96.0, pattern, npattern);
			//pen = CreatePen(img, lyrs->style, 0, osSize);
			this->mapSch.SetDrawType(layer, Map::MapScheduler::MSDT_POLYLINE, p, 0, 0, 0.0, 0.0, &denv->isLayerEmpty);

			session = layer->BeginGetObject();
			lastId = -1;
			j = i;
			i = 0;
			while (i < j)
			{
				thisId = denv->idArr.GetItem(i);
				if (thisId != lastId)
				{
					lastId = thisId;
					if ((dobj = layer->GetNewObjectById(session, thisId)) != 0)
					{
						this->mapSch.Draw(dobj);
					}
				}
				i++;
			}
			layer->EndGetObject(session);

			if (found)
			{
				while (denv->env->GetLineStyleLayer(lineStyle, layerId++, &color, &thick, &pattern, &npattern))
				{
					p = denv->img->NewPenARGB(this->colorConv->ConvRGB8(color), UOSInt2Double(thick) * denv->img->GetHDPI() / 96.0, pattern, npattern);
					this->mapSch.DrawNextType(p, 0);
				}
			}
			this->mapSch.WaitForFinish();
		}
	}
}

void Map::DrawMapRenderer::DrawPGLayer(Map::DrawMapRenderer::DrawEnv *denv, Map::IMapDrawLayer *layer, UOSInt lineStyle, UInt32 fillStyle, UOSInt lineThick, UInt32 lineColor)
{
	UOSInt i;
	UOSInt j;
	void *session;
	Media::DrawPen *p;
	Media::DrawBrush *b;
	Int64 lastId;
	Int64 thisId;
	UOSInt layerId = 0;
	Map::DrawObjectL *dobj;
	Math::CoordinateSystem *lyrCSys = layer->GetCoordinateSystem();
	Math::CoordinateSystem *envCSys = this->env->GetCoordinateSystem();
	denv->idArr.Clear();
	if (lyrCSys != 0 && envCSys != 0 && !lyrCSys->Equals(envCSys))
	{
		Math::Coord2DDbl tl;
		Math::Coord2DDbl br;
		Math::RectAreaDbl rect = denv->view->GetVerticalRect();
		tl = rect.tl;
		br = rect.GetBR();
		Math::CoordinateSystem::ConvertXYZ(envCSys, lyrCSys, tl.x, tl.y, 0, &tl.x, &tl.y, 0);
		Math::CoordinateSystem::ConvertXYZ(envCSys, lyrCSys, br.x, br.y, 0, &br.x, &br.y, 0);
		layer->GetObjectIdsMapXY(&denv->idArr, 0, Math::RectAreaDbl(tl, br), true);

		if ((i = denv->idArr.GetCount()) > 0)
		{
			UInt32 color;
			UOSInt thick;
			UInt8 *pattern;
			UOSInt npattern;
			Bool found;
			found = denv->env->GetLineStyleLayer(lineStyle, layerId++, &color, &thick, &pattern, &npattern);
			p = denv->img->NewPenARGB(this->colorConv->ConvRGB8(color), UOSInt2Double(thick) * denv->img->GetHDPI() / 96.0, pattern, npattern);
			b = denv->img->NewBrushARGB(this->colorConv->ConvRGB8(fillStyle));
			this->mapSch.SetDrawType(layer, Map::MapScheduler::MSDT_POLYGON, p, b, 0, 0.0, 0.0, &denv->isLayerEmpty);

			session = layer->BeginGetObject();
			lastId = -1;
			while (i-- > 0)
			{
				thisId = denv->idArr.GetItem(i);
				if (thisId != lastId)
				{
					lastId = thisId;
					if ((dobj = layer->GetNewObjectById(session, thisId)) != 0)
					{
						j = dobj->nPoint;
						while (j-- > 0)
						{
							Math::CoordinateSystem::ConvertXYZ(lyrCSys, envCSys, dobj->pointArr[j].x, dobj->pointArr[j].y, 0, &dobj->pointArr[j].x, &dobj->pointArr[j].y, 0);
						}
						this->mapSch.Draw(dobj);
					}
				}
			}
			layer->EndGetObject(session);

			if (found)
			{
				while (denv->env->GetLineStyleLayer(lineStyle, layerId++, &color, &thick, &pattern, &npattern))
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
		layer->GetObjectIdsMapXY(&denv->idArr, 0, Math::RectAreaDbl(tl, br), true);

		if ((i = denv->idArr.GetCount()) > 0)
		{
			UInt32 color;
			UOSInt thick;
			UInt8 *pattern;
			UOSInt npattern;
			Bool found;
			found = denv->env->GetLineStyleLayer(lineStyle, layerId++, &color, &thick, &pattern, &npattern);
			p = denv->img->NewPenARGB(this->colorConv->ConvRGB8(color), UOSInt2Double(thick) * denv->img->GetHDPI() / 96.0, pattern, npattern);
			b = denv->img->NewBrushARGB(this->colorConv->ConvRGB8(fillStyle));
			this->mapSch.SetDrawType(layer, Map::MapScheduler::MSDT_POLYGON, p, b, 0, 0.0, 0.0, &denv->isLayerEmpty);

			session = layer->BeginGetObject();
			lastId = -1;
			while (i-- > 0)
			{
				thisId = denv->idArr.GetItem(i);
				if (thisId != lastId)
				{
					lastId = thisId;
					if ((dobj = layer->GetNewObjectById(session, thisId)) != 0)
					{
						this->mapSch.Draw(dobj);
					}
				}
			}
			layer->EndGetObject(session);

			if (found)
			{
				while (denv->env->GetLineStyleLayer(lineStyle, layerId++, &color, &thick, &pattern, &npattern))
				{
					p = denv->img->NewPenARGB(this->colorConv->ConvRGB8(color), UOSInt2Double(thick) * denv->img->GetHDPI() / 96.0, pattern, npattern);
					this->mapSch.DrawNextType(p, 0);
				}
			}
			this->mapSch.WaitForFinish();
		}
	}
}

void Map::DrawMapRenderer::DrawPTLayer(Map::DrawMapRenderer::DrawEnv *denv, Map::IMapDrawLayer *layer, UOSInt imgIndex)
{
	Data::ArrayListInt64 arri;
	Map::DrawObjectL *dobj;
	UOSInt i;
	UOSInt k;
	void *session;
	Math::Coord2DDbl tl;
	Math::Coord2DDbl br;
	Math::RectAreaDbl rect = denv->view->GetVerticalRect();
	tl = rect.tl;
	br = rect.br;
	Double spotX;
	Double spotY;
	UOSInt maxLabel = denv->env->GetNString();

	Media::Image *img = 0;
	UInt32 imgTimeMS = 0;
	if (layer->HasIconStyle())
	{
		img = layer->GetIconStyleImg()->GetImage(&imgTimeMS);
		spotX = OSInt2Double(layer->GetIconStyleSpotX());
		spotY = OSInt2Double(layer->GetIconStyleSpotY());
		if (img != 0 && (spotX == -1 || spotY == -1))
		{
			spotX = UOSInt2Double(img->info.dispWidth) * 0.5;
			spotY = UOSInt2Double(img->info.dispHeight) * 0.5;
		}
	}
	if (img == 0)
	{
		img = denv->env->GetImage(imgIndex, &imgTimeMS);
		if (img == 0)
			return;
		spotX = UOSInt2Double(img->info.dispWidth) * 0.5;
		spotY = UOSInt2Double(img->info.dispHeight) * 0.5;
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
	Math::CoordinateSystem *lyrCSys = layer->GetCoordinateSystem();
	Math::CoordinateSystem *envCSys = this->env->GetCoordinateSystem();
	if (lyrCSys != 0 && envCSys != 0 && !lyrCSys->Equals(envCSys))
	{
		Math::CoordinateSystem::ConvertXYZ(envCSys, lyrCSys, tl.x, tl.y, 0, &tl.x, &tl.y, 0);
		Math::CoordinateSystem::ConvertXYZ(envCSys, lyrCSys, br.x, br.y, 0, &br.x, &br.y, 0);
		layer->GetObjectIdsMapXY(&arri, 0, Math::RectAreaDbl(tl - ((br - tl) * 0.5), br + ((br - tl) * 0.5)), true);
		if (arri.GetCount() <= 0)
		{
			return;
		}
		Media::DrawImage *dimg;
		if (this->drawType == Map::DrawMapRenderer::DT_PIXELDRAW)
		{
			UInt32 newW = (UInt32)Double2Int32(UOSInt2Double(img->info.dispWidth) * denv->img->GetHDPI() / img->info.hdpi);
			UInt32 newH = (UInt32)Double2Int32(UOSInt2Double(img->info.dispHeight) * denv->img->GetVDPI() / img->info.vdpi);
			if (newW > img->info.dispWidth || newH > img->info.dispHeight)
			{
				this->resizer->SetTargetWidth(newW);
				this->resizer->SetTargetHeight(newH);
				Media::StaticImage *img2 = this->resizer->ProcessToNew((Media::StaticImage*)img);
				if (img2)
				{
					spotX = spotX * denv->img->GetHDPI() / img->info.hdpi;
					spotY = spotY * denv->img->GetVDPI() / img->info.vdpi;
					dimg = this->eng->ConvImage(img2);
					DEL_CLASS(img2);
				}
				else
				{
					dimg = this->eng->ConvImage(img);
				}
			}
			else
			{
				dimg = this->eng->ConvImage(img);
			}
		}
		else
		{
			dimg = this->eng->ConvImage(img);
		}

		this->mapSch.SetDrawType(layer, Map::MapScheduler::MSDT_POINTS, 0, 0, dimg, spotX, spotY, &denv->isLayerEmpty);
		this->mapSch.SetDrawObjs(denv->objBounds, &denv->objCnt, maxLabel);
		session = layer->BeginGetObject();

		i = arri.GetCount();
		while (i-- > 0)
		{
			if ((dobj = layer->GetNewObjectById(session, arri.GetItem(i))) != 0)
			{
				k = dobj->nPoint;
				while (k-- > 0)
				{
					Math::CoordinateSystem::ConvertXYZ(lyrCSys, envCSys, dobj->pointArr[k].x, dobj->pointArr[k].y, 0, &dobj->pointArr[k].x, &dobj->pointArr[k].y, 0);
				}
				this->mapSch.Draw(dobj);
			}
		}

		layer->EndGetObject(session);
		this->mapSch.WaitForFinish();
		this->eng->DeleteImage(dimg);
	}
	else
	{
		layer->GetObjectIdsMapXY(&arri, 0, Math::RectAreaDbl(tl - ((br - tl) * 0.5), br + ((br - tl) * 0.5)), true);
		if (arri.GetCount() <= 0)
		{
			return;
		}

		Media::DrawImage *dimg;
		if (this->drawType == Map::DrawMapRenderer::DT_PIXELDRAW)
		{
			UInt32 newW = (UInt32)Double2Int32(UOSInt2Double(img->info.dispWidth) * denv->img->GetHDPI() / img->info.hdpi);
			UInt32 newH = (UInt32)Double2Int32(UOSInt2Double(img->info.dispHeight) * denv->img->GetVDPI() / img->info.vdpi);
			if (newW != img->info.dispWidth || newH != img->info.dispHeight)
			{
				this->resizer->SetTargetWidth(newW);
				this->resizer->SetTargetHeight(newH);
				Media::StaticImage *img2 = this->resizer->ProcessToNew((Media::StaticImage*)img);
				if (img2)
				{
					spotX = spotX * denv->img->GetHDPI() / img->info.hdpi;
					spotY = spotY * denv->img->GetVDPI() / img->info.vdpi;
					dimg = this->eng->ConvImage(img2);
					DEL_CLASS(img2);
				}
				else
				{
					dimg = this->eng->ConvImage(img);
				}
			}
			else
			{
				dimg = this->eng->ConvImage(img);
			}
		}
		else
		{
			dimg = this->eng->ConvImage(img);
		}

		this->mapSch.SetDrawType(layer, Map::MapScheduler::MSDT_POINTS, 0, 0, dimg, spotX, spotY, &denv->isLayerEmpty);
		this->mapSch.SetDrawObjs(denv->objBounds, &denv->objCnt, maxLabel);
		session = layer->BeginGetObject();

		i = arri.GetCount();
		while (i-- > 0)
		{
			if ((dobj = layer->GetNewObjectById(session, arri.GetItem(i))) != 0)
			{
				this->mapSch.Draw(dobj);
			}
		}

		layer->EndGetObject(session);
		this->mapSch.WaitForFinish();
		this->eng->DeleteImage(dimg);
	}
}

void Map::DrawMapRenderer::DrawLabel(DrawEnv *denv, Map::IMapDrawLayer *layer, UOSInt fontStyle, UOSInt labelCol, Int32 priority, Int32 flags, UOSInt imgWidth, UOSInt imgHeight, Map::DrawLayerType layerType, Map::MapEnv::FontType fontType)
{
	void *arr;
	Data::ArrayListInt64 arri;
	UOSInt i;
	UInt32 j;
	UOSInt k;
	Map::DrawObjectL *dobj;
	Double scaleW;
	Double scaleH;
	Math::Coord2DDbl pts;
	UTF8Char *sptr;
	UTF8Char *sptrEnd;
	UTF8Char lblStr[256];
	void *session;
	UOSInt maxLabel = denv->env->GetNString();
	Bool csysConv = false;;
	Math::Coord2DDbl tl;
	Math::Coord2DDbl br;
	Math::RectAreaDbl rect = denv->view->GetVerticalRect();
	tl = rect.tl;
	br = rect.br;
	Math::CoordinateSystem *lyrCSys = layer->GetCoordinateSystem();
	Math::CoordinateSystem *envCSys = this->env->GetCoordinateSystem();
	if (lyrCSys != 0 && envCSys != 0 && !lyrCSys->Equals(envCSys))
	{
		Math::CoordinateSystem::ConvertXYZ(envCSys, lyrCSys, tl.x, tl.y, 0, &tl.x, &tl.y, 0);
		Math::CoordinateSystem::ConvertXYZ(envCSys, lyrCSys, br.x, br.y, 0, &br.x, &br.y, 0);
		csysConv = true;
	}

	layer->GetObjectIdsMapXY(&arri, &arr, Math::RectAreaDbl(tl, br), false);
	session = layer->BeginGetObject();
	i = arri.GetCount();
	while (i-- > 0)
	{
		if ((dobj = layer->GetNewObjectById(session, arri.GetItem(i))) != 0)
		{
			sptrEnd = layer->GetString(sptr = lblStr, sizeof(lblStr), arr, arri.GetItem(i), labelCol);
			if (sptrEnd)
			{
				if (csysConv)
				{
					k = dobj->nPoint;
					while (k-- > 0)
					{
						Math::CoordinateSystem::ConvertXYZ(lyrCSys, envCSys, dobj->pointArr[k].x, dobj->pointArr[k].y, 0, &dobj->pointArr[k].x, &dobj->pointArr[k].y, 0);
					}
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
					UInt32 k;
					UInt32 maxSize;
					UInt32 maxPos;
					if (dobj->nPtOfst == 0)
					{
						maxSize = dobj->nPoint;
						maxPos = 0;
					}
					else
					{
						maxSize = dobj->nPoint - (maxPos = dobj->ptOfstArr[dobj->nPtOfst - 1]);
						k = dobj->nPtOfst;
						while (k-- > 1)
						{
							if ((dobj->ptOfstArr[k] - dobj->ptOfstArr[k - 1]) > maxSize)
								maxSize = (dobj->ptOfstArr[k] - (maxPos = dobj->ptOfstArr[k - 1]));
						}
					}
					if (AddLabel(denv->labels, maxLabel, &denv->labelCnt, CSTRP(sptr, sptrEnd), maxSize, &dobj->pointArr[maxPos], priority, layerType, fontStyle, flags, denv->view, (OSInt)imgWidth, (OSInt)imgHeight, fontType))
					{
						layer->ReleaseObject(session, dobj);
					}
					else
					{
						layer->ReleaseObject(session, dobj);
					}
				}
				else if (layerType == Map::DRAW_LAYER_POLYLINE || layerType == Map::DRAW_LAYER_POLYLINE3D)
				{
					if (dobj->nPoint & 1)
					{
						UOSInt l = dobj->nPoint >> 1;
						pts = dobj->pointArr[l];

						scaleW = dobj->pointArr[l + 1].x - dobj->pointArr[l - 1].x;
						scaleH = dobj->pointArr[l + 1].y - dobj->pointArr[l - 1].y;
					}
					else
					{
						UOSInt l = dobj->nPoint >> 1;
						pts.x = (dobj->pointArr[l - 1].x + dobj->pointArr[l].x) * 0.5;
						pts.y = (dobj->pointArr[l - 1].y + dobj->pointArr[l].y) * 0.5;

						scaleW = dobj->pointArr[l].x - dobj->pointArr[l - 1].x;
						scaleH = dobj->pointArr[l].y - dobj->pointArr[l - 1].y;
					}

					if (denv->view->InViewXY(pts))
					{
						pts = denv->view->MapXYToScnXY(pts);

						if ((flags & Map::MapEnv::SFLG_ROTATE) == 0)
							scaleW = scaleH = 0;
						DrawChars(denv, CSTRP(sptr, sptrEnd), pts.x, pts.y, scaleW, scaleH, fontType, fontStyle, (flags & Map::MapEnv::SFLG_ALIGN) != 0);
					}
					layer->ReleaseObject(session, dobj);
				}
				else if (layerType == Map::DRAW_LAYER_POLYGON)
				{
					pts = Math::Geometry::GetPolygonCenter(dobj->nPtOfst, dobj->nPoint, dobj->ptOfstArr, dobj->pointArr);
					if (denv->view->InViewXY(pts))
					{
						pts = denv->view->MapXYToScnXY(pts);
						DrawChars(denv, CSTRP(sptr, sptrEnd), pts.x, pts.y, 0, 0, fontType, fontStyle, (flags & Map::MapEnv::SFLG_ALIGN) != 0);
					}
					layer->ReleaseObject(session, dobj);
				}
				else
				{
					Double lastPtX = 0;
					Double lastPtY = 0;
					Math::Coord2DDbl *pointPos = dobj->pointArr;

					j = dobj->nPoint;
					while (j--)
					{
						lastPtX += pointPos->x;
						lastPtY += pointPos->y;
						pointPos++;
					}

					pts.x = (lastPtX / dobj->nPoint);
					pts.y = (lastPtY / dobj->nPoint);
					if (denv->view->InViewXY(pts))
					{
						pts = denv->view->MapXYToScnXY(pts);
						DrawChars(denv, CSTRP(sptr, sptrEnd), pts.x, pts.y, 0, 0, fontType, fontStyle, (flags & Map::MapEnv::SFLG_ALIGN) != 0);
					}
					layer->ReleaseObject(session, dobj);
				}
			}
			else
			{
				layer->ReleaseObject(session, dobj);
			}
		}
	}
	layer->EndGetObject(session);
	layer->ReleaseNameArr(arr);
}

void Map::DrawMapRenderer::DrawImageLayer(DrawEnv *denv, Map::IMapDrawLayer *layer)
{
	Math::Vector2D *vec;
	Math::VectorImage *vimg;
	UOSInt i;
	UOSInt j;
	Math::CoordinateSystem *coord = layer->GetCoordinateSystem();
	Bool geoConv;
	void *sess;
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
		Math::CoordinateSystem::ConvertXYZ(denv->env->GetCoordinateSystem(), coord, tl.x, tl.y, 0, &tl.x, &tl.y, 0);
		Math::CoordinateSystem::ConvertXYZ(denv->env->GetCoordinateSystem(), coord, br.x, br.y, 0, &br.x, &br.y, 0);
		layer->GetObjectIdsMapXY(&arri, 0, Math::RectAreaDbl(tl, br), false);
	}
	else
	{
		layer->GetObjectIdsMapXY(&arri, 0, Math::RectAreaDbl(tl, br), false);
	}
	Data::ArrayList<Math::VectorImage *> imgList;
	sess = layer->BeginGetObject();
	i = 0;
	j = arri.GetCount();
	while (i < j)
	{
		vec = layer->GetNewVectorById(sess, arri.GetItem(i));
		if (vec)
		{
			if (vec->GetVectorType() == Math::Vector2D::VectorType::Image)
			{
				imgList.Add((Math::VectorImage*)vec);
			}
			else
			{
				DEL_CLASS(vec);
			}
		}
		i++;
	}
	layer->EndGetObject(sess);
	
	void **arr = (void**)imgList.GetArray(&j);
	ArtificialQuickSort_SortCmp(arr, VImgCompare, 0, (OSInt)j - 1);
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
			Math::RectAreaDbl mapCoords;
			vimg->GetBounds(&mapCoords);
			Double t = mapCoords.tl.y;
			mapCoords.tl.y = mapCoords.br.y;
			mapCoords.br.y = t;
			if (geoConv)
			{
				Math::CoordinateSystem::ConvertXYZ(coord, denv->env->GetCoordinateSystem(), mapCoords.tl.x, mapCoords.tl.y, 0, &mapCoords.tl.x, &mapCoords.tl.y, 0);
				Math::CoordinateSystem::ConvertXYZ(coord, denv->env->GetCoordinateSystem(), mapCoords.br.x, mapCoords.br.y, 0, &mapCoords.br.x, &mapCoords.br.y, 0);
			}
			scnCoords[0] = denv->view->MapXYToScnXY(mapCoords.tl);
			scnCoords[1] = denv->view->MapXYToScnXY(mapCoords.br);
		}
		UInt32 imgTimeMS;
		Media::StaticImage *simg = vimg->GetImage(scnCoords[1].x - scnCoords[0].x, scnCoords[1].y - scnCoords[0].y, &imgTimeMS);
		DrawImageObject(denv, simg, scnCoords[0].x, scnCoords[0].y, scnCoords[1].x, scnCoords[1].y, vimg->GetSrcAlpha());
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

		DEL_CLASS(vimg);
		i++;
	}
}

void Map::DrawMapRenderer::DrawImageObject(DrawEnv *denv, Media::StaticImage *img, Double scnX1, Double scnY1, Double scnX2, Double scnY2, Double srcAlpha)
{
	UOSInt imgW;
	UOSInt imgH;
	Double cimgX2;
	Double cimgY2;
	Double cimgX;
	Double cimgY;
	Double dimgW;
	Double dimgH;

	imgW = denv->img->GetWidth();
	imgH = denv->img->GetHeight();
	dimgW = UOSInt2Double(imgW);
	dimgH = UOSInt2Double(imgH);

	if (img != 0 && scnX1 < scnX2 && scnY1 < scnY2)
	{
		if (this->drawType == DT_VECTORDRAW)
		{
			img->info.hdpi = UOSInt2Double(img->info.dispWidth) * denv->img->GetHDPI() / (scnX2 - scnX1);
			img->info.vdpi = UOSInt2Double(img->info.dispHeight) * denv->img->GetVDPI() / (scnY2 - scnY1);
			denv->img->DrawImagePt2(img, scnX1, scnY1);
		}
		else
		{
			Double drawW = scnX2 - scnX1;
			Double drawH = scnY2 - scnY1;
			if (dimgW > drawW || dimgH > drawH)
			{
				img->To32bpp();
				this->resizer->SetTargetWidth((UOSInt)(Double2Int32(scnX2) - Double2Int32(scnX1)));
				this->resizer->SetTargetHeight((UOSInt)(Double2Int32(scnY2) - Double2Int32(scnY1)));
				this->resizer->SetResizeAspectRatio(Media::IImgResizer::RAR_IGNOREAR);
				Media::StaticImage *newImg = this->resizer->ProcessToNew(img);
				if (newImg)
				{
					if (srcAlpha >= 0 && srcAlpha <= 1)
					{
						newImg->MultiplyAlpha(srcAlpha);
					}
					newImg->info.hdpi = denv->img->GetHDPI();
					newImg->info.vdpi = denv->img->GetVDPI();
					denv->img->DrawImagePt2(newImg, scnX1, scnY1);
					DEL_CLASS(newImg);
				}
			}
			else
			{
				cimgX2 = UOSInt2Double(img->info.dispWidth);
				cimgY2 = UOSInt2Double(img->info.dispHeight);
				cimgX = 0;
				cimgY = 0;
				if (scnX1 < 0)
				{
					cimgX = cimgX2 * scnX1 / (scnX1 - scnX2);
					scnX1 = 0;
				}
				if (scnY1 < 0)
				{
					cimgY = cimgY2 * scnY1 / (scnY1 - scnY2);
					scnY1 = 0;
				}
				if (scnX2 > dimgW)
				{
					cimgX2 = cimgX + (cimgX2 - cimgX) * (dimgW - scnX1) / (scnX2 - scnX1);
					scnX2 = dimgW;
				}
				if (scnY2 > dimgH)
				{
					cimgY2 = cimgY + (cimgY2 - cimgY) * (dimgH - scnY1) / (scnY2 - scnY1);
					scnY2 = dimgH;
				}
				if (cimgX == cimgX2)
				{
					if (cimgX2 >= UOSInt2Double(img->info.dispWidth))
					{
						cimgX = cimgX2 - 1;
					}
					else
					{
						cimgX2 = cimgX2 + 1;
					}
				}
				if (cimgY == cimgY2)
				{
					if (cimgY2 >= UOSInt2Double(img->info.dispHeight))
					{
						cimgY = cimgY2 - 1;
					}
					else
					{
						cimgY2++;
					}
				}

				this->resizer->SetTargetWidth((UOSInt)(Double2Int32(scnX2) - Double2Int32(scnX1)));
				this->resizer->SetTargetHeight((UOSInt)(Double2Int32(scnY2) - Double2Int32(scnY1)));
				this->resizer->SetResizeAspectRatio(Media::IImgResizer::RAR_IGNOREAR);
				img->To32bpp();
				Media::StaticImage *newImg = 0;
				if (cimgX < cimgX2 && cimgY < cimgY2)
				{
					newImg = this->resizer->ProcessToNewPartial(img, cimgX, cimgY, cimgX2, cimgY2);
				}
				if (newImg)
				{
					if (srcAlpha >= 0 && srcAlpha <= 1)
					{
						newImg->MultiplyAlpha(srcAlpha);
					}
					Media::DrawImage *dimg = this->eng->ConvImage(newImg);
					if (dimg)
					{
						dimg->SetHDPI(denv->img->GetHDPI());
						dimg->SetVDPI(denv->img->GetVDPI());
						denv->img->DrawImagePt(dimg, scnX1, scnY1);
						this->eng->DeleteImage(dimg);
					}
					DEL_CLASS(newImg);
				}
			}
		}
	}
}

void Map::DrawMapRenderer::GetCharsSize(DrawEnv *denv, Double *size, Text::CString label, Map::MapEnv::FontType fontType, UOSInt fontStyle, Double scaleW, Double scaleH)
{
	Double szTmp[2];
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
	denv->img->GetTextSize(df, label, szTmp);

	if (scaleH == 0)
	{
		size[0] = (szTmp[0] + UOSInt2Double(buffSize << 1));
		size[1] = (szTmp[1] + UOSInt2Double(buffSize << 1));

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
	xPos = szTmp[0] + UOSInt2Double(buffSize << 1);
	yPos = szTmp[1] + UOSInt2Double(buffSize << 1);
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
	size[0] = maxX - minX;
	size[1] = maxY - minY;
}

void Map::DrawMapRenderer::DrawChars(DrawEnv *denv, Text::CString str1, Double scnPosX, Double scnPosY, Double scaleW, Double scaleH, Map::MapEnv::FontType fontType, UOSInt fontStyle, Bool isAlign)
{
	Double size[2];
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
	denv->img->GetTextSize(df, str1, size);

	if (scaleH == 0)
	{
		denv->img->SetTextAlign(Media::DrawEngine::DRAW_POS_TOPLEFT);

		if (font && font->buffSize > 0)
		{
			denv->img->DrawStringB(scnPosX - (size[0] * 0.5), scnPosY - (size[1] * 0.5), str1, font->font, font->buffBrush, (UOSInt)Double2Int32(UOSInt2Double(font->buffSize) * denv->img->GetHDPI() / 96.0));
			denv->img->DrawString(scnPosX - (size[0] * 0.5), scnPosY - (size[1] * 0.5), str1, df, db);
		}
		else
		{
			denv->img->DrawString(scnPosX - (size[0] * 0.5), scnPosY - (size[1] * 0.5), str1, df, db);
		}
		return;
	}


	if (scaleH < 0)
		absH = (UInt16)-scaleH;
	else
		absH = (UInt16)scaleH;

	Double degD = Math_ArcTan2((Double)scaleH, (Double)scaleW);

	Int32 deg = (Int32) (degD * 1800 / Math::PI);
	while (deg < 0)
		deg += 3600;

	Double lastScaleW = scaleW;
	Double lastScaleH = scaleH;

	scaleW = lastScaleW;
	scaleH = lastScaleH;

	if (isAlign)
	{
		denv->img->SetTextAlign(Media::DrawEngine::DRAW_POS_TOPLEFT);
		Double currX = 0;
		Double currY = 0;
		Double startX;
		Double startY;
		Double tmp;
		Int32 type;
		Double szThis[2];
		Double dlblSize = UOSInt2Double(str1.leng);
		denv->img->GetTextSize(df, str1, szThis);

		if ((szThis[0] * absH) < (szThis[1] * dlblSize * scaleW))
		{
			scaleW = -scaleW;
			startX = scnPosX - (tmp = szThis[0] * 0.5);
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
				startY = scnPosY - (tmp = ((szThis[1] * dlblSize) * 0.5));
				startX = scnPosX - (tmp * scaleW / scaleH);
			}
			else if (scaleH)
			{
				scaleW = -scaleW;
				scaleH = -scaleH;
				startY = scnPosY - (tmp = ((szThis[1] * dlblSize) * 0.5));
				startX = scnPosX - (tmp * scaleW / scaleH);
			}
			else
			{
				startY = scnPosY - (tmp = ((szThis[1] * dlblSize) * 0.5));
				startX = scnPosX;
			}
			type = 1;
		}

		UOSInt cnt;
		const UTF8Char *lbl = str1.v;

		if (font && font->buffSize > 0)
		{
			currX = 0;
			currY = 0;

			cnt = str1.leng;

			while (cnt--)
			{
				denv->img->GetTextSize(font->font, {lbl, 1}, szThis);

				if (type)
				{
					UTF8Char l[2];
					l[0] = lbl[0];
					l[1] = 0;
					denv->img->DrawStringB(startX + currX - (szThis[0] * 0.5), startY + currY, {l, 1}, font->font, font->buffBrush, (UOSInt)Double2Int32(UOSInt2Double(font->buffSize) * denv->img->GetHDPI() / 96.0));

					currY += szThis[1];

					if (scaleH)
						currX = (currY * scaleW / scaleH);
				}
				else
				{
					UTF8Char l[2];
					l[0] = lbl[0];
					l[1] = 0;
					denv->img->DrawStringB(startX + currX, startY + currY, {l, 1}, font->font, font->buffBrush, (UOSInt)Double2Int32(UOSInt2Double(font->buffSize) * denv->img->GetHDPI() / 96.0));

					currX += szThis[0];
					if (scaleW)
						currY = (currX * scaleH / scaleW);
				}
				lbl += 1;
			}
		}
		currX = 0;
		currY = 0;
		lbl = str1.v;

		cnt = str1.leng;

		while (cnt--)
		{
			denv->img->GetTextSize(df, {lbl, 1}, szThis);

			if (type)
			{
				UTF8Char l[2];
				l[0] = lbl[0];
				l[1] = 0;
				denv->img->DrawString(startX + currX - (szThis[0] * 0.5), startY + currY, {l, 1}, df, db);

				currY += szThis[1];

				if (scaleH)
					currX = (currY * scaleW / scaleH);
			}
			else
			{
				UTF8Char l[2];
				l[0] = lbl[0];
				l[1] = 0;
				denv->img->DrawString(startX + currX, startY + currY, {l, 1}, df, db);

				currX += szThis[0];
				if (scaleW)
					currY = (currX * scaleH / scaleW);
			}
			lbl += 1;
		}

	}
	else
	{
		denv->img->SetTextAlign(Media::DrawEngine::DRAW_POS_CENTER);
		if (font && font->buffSize > 0)
		{
			denv->img->DrawStringRotB(scnPosX, scnPosY, str1, font->font, font->buffBrush, degD * 180 / Math::PI, font->buffSize);
		}
		denv->img->DrawStringRot(scnPosX, scnPosY, str1, df, db, degD * 180 / Math::PI);
	}
}

void Map::DrawMapRenderer::DrawCharsL(Map::DrawMapRenderer::DrawEnv *denv, Text::CString str1, Math::Coord2DDbl *mapPts, Int32 *scnPts, UOSInt nPoints, UOSInt thisPt, Double scaleN, Double scaleD, Map::MapEnv::FontType fontType, UOSInt fontStyle, Double *realBounds)
{
	UTF8Char sbuff[256];
	str1.ConcatTo(sbuff);
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
	UOSInt j;
	Double szThis[2];
	Double szLast[2];
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

	minX = maxX = centX;
	minY = maxY = centY;

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
		angleOfst = Math::PI;
	}

	currX = centX;
	currY = centY;
	j = thisPt;
	nextX = xDiff;
	nextY = yDiff;
	xDiff = 0;
	yDiff = 0;

	denv->img->GetTextSize(df, str1, szThis);
	xDiff = szThis[0] * 0.5;
	yDiff = xDiff * xDiff;

	if (mode == 0)
	{
		while (j != (UOSInt)-1)
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
			if (j == (UOSInt)-1)
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

	szLast[0] = 0;

	lastX = currX = startX;
	lastY = currY = startY;
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

		denv->img->GetTextSize(df, CSTRP(lbl, nextPos), szThis);
		dist = (szLast[0] + szThis[0]) * 0.5;
		nextX = currX + (dist * cosAngle);
		nextY = currY - (dist * sinAngle);
		if ( (((nextX > scnPts[(j << 1)]) ^ (nextX > scnPts[(j << 1) + 2])) || (nextX == scnPts[(j << 1)]) || (nextX == scnPts[(j << 1) + 2])) && (((nextY > scnPts[(j << 1) + 1]) ^ (nextY > scnPts[(j << 1) + 3])) || (nextY == scnPts[(j << 1) + 1]) || (nextY == scnPts[(j << 1) + 3])))
		{
			currX = nextX;
			currY = nextY;
		}
		else
		{
			xDiff = szLast[0] + szThis[0];
			yDiff = (xDiff * xDiff) * 0.5;

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
								currX = currX + nextX * Math_Sqrt(yDiff) / Math_Sqrt(xDiff);
							}
							else
							{
								currX = currX - nextX * Math_Sqrt(yDiff) / Math_Sqrt(xDiff);
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
								currY = currY + nextY * Math_Sqrt(yDiff) / Math_Sqrt(xDiff);
							}
							else
							{
								currY = currY - nextY * Math_Sqrt(yDiff) / Math_Sqrt(xDiff);
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
				if (font && font->buffSize > 0)
				{
					denv->img->DrawStringRotB(currX, currY, CSTRP(lbl, nextPos), df, font->buffBrush, lastAngle, font->buffSize);
				}
				denv->img->DrawStringRot(currX, currY, CSTRP(lbl, nextPos), df, db, lastAngle);
			}
			else
			{
				if (font && font->buffSize > 0)
				{
					denv->img->DrawStringRotB(currX, currY, CSTRP(lbl, nextPos), df, font->buffBrush, lastAngle, font->buffSize);
				}
				denv->img->DrawStringRot(currX, currY, CSTRP(lbl, nextPos), df, db, lastAngle);
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
				if (font && font->buffSize > 0)
				{
					denv->img->DrawStringRotB(currX, currY, CSTRP(lbl, nextPos), df, font->buffBrush, angleDegree, font->buffSize);
				}
				denv->img->DrawStringRot(currX, currY, CSTRP(lbl, nextPos), df, db, angleDegree);
			}
			else
			{
				if (font && font->buffSize > 0)
				{
					denv->img->DrawStringRotB(currX, currY, CSTRP(lbl, nextPos), df, font->buffBrush, angleDegree, font->buffSize);
				}
				denv->img->DrawStringRot(currX, currY, CSTRP(lbl, nextPos), df, db, angleDegree);
			}

		}
		lastX = currX;
		lastY = currY;
		szLast[0] = szThis[0];
	}

	realBounds[0] = minX;
	realBounds[1] = minY;
	realBounds[2] = maxX;
	realBounds[3] = maxY;
}

void Map::DrawMapRenderer::DrawCharsLA(DrawEnv *denv, Text::CString str1, Math::Coord2DDbl *mapPts, Int32 *scnPts, UOSInt nPoints, UOSInt thisPt, Double scaleN, Double scaleD, Map::MapEnv::FontType fontType, UOSInt fontStyle, Double *realBounds)
{
	UTF8Char sbuff[256];
	UOSInt lblSize = str1.leng;
	str1.ConcatTo(sbuff);
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

	maxX = minX = centX;
	maxY = minY = centY;

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
			angleOfst = Math::PI;
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
			angleOfst = Math::PI;
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
		denv->img->GetTextSize(df, {&str1.v[i], 1}, szThis);
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
	Double lastX;
	Double lastY;

	szLast[0] = 0;
	szLast[1] = 0;

	lastX = currX = startX;
	lastY = currY = startY;
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

		denv->img->GetTextSize(df, CSTRP(lbl, nextPos), szThis);
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
				Double tempY = scnPts[(j << 1) + 1] + (scnPts[(j << 1) + 3] - scnPts[(j << 1) + 1]) * (nextX - scnPts[(j << 1)]) / (scnPts[(j << 1) + 2] - scnPts[(j << 1)]);
				tempY -= currY;
				if (tempY < 0)
					tempY = -tempY;
				if (tempY > (szLast[1] + szThis[1]) * 0.5)
				{
					currY = nextY;
					currX = scnPts[(j << 1) + 0] + (scnPts[(j << 1) + 2] - scnPts[(j << 1) + 0]) * (currY - scnPts[(j << 1) + 1]) / (scnPts[(j << 1) + 3] - scnPts[(j << 1) + 1]);
				}
				else
				{
					currX = nextX;
					currY = scnPts[(j << 1) + 1] + (scnPts[(j << 1) + 3] - scnPts[(j << 1) + 1]) * (nextX - scnPts[(j << 1)]) / (scnPts[(j << 1) + 2] - scnPts[(j << 1)]);
				}
				break;
			}
			else if (((nextY > scnPts[(j << 1) + 1]) ^ (nextY > scnPts[(j << 1) + 3])) || (nextY == scnPts[(j << 1) + 1]) || (nextY == scnPts[(j << 1) + 3]))
			{
				currY = nextY;
				currX = scnPts[(j << 1) + 0] + (scnPts[(j << 1) + 2] - scnPts[(j << 1) + 0]) * (currY - scnPts[(j << 1) + 1]) / (scnPts[(j << 1) + 3] - scnPts[(j << 1) + 1]);
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

						Double tempY = scnPts[(j << 1) + 1] + (scnPts[(j << 1) + 3] - scnPts[(j << 1) + 1]) * (nextX - scnPts[(j << 1)]) / (scnPts[(j << 1) + 2] - scnPts[(j << 1)]);
						tempY -= currY;
						if (tempY < 0)
							tempY = -tempY;
						if (tempY > (szLast[1] + szThis[1]) * 0.5)
						{
							currY = nextY;
							currX = scnPts[(j << 1) + 0] + (scnPts[(j << 1) + 2] - scnPts[(j << 1) + 0]) * (currY - scnPts[(j << 1) + 1]) / (scnPts[(j << 1) + 3] - scnPts[(j << 1) + 1]);
						}
						else
						{
							currX = nextX;
							currY = scnPts[(j << 1) + 1] + (scnPts[(j << 1) + 3] - scnPts[(j << 1) + 1]) * (nextX - scnPts[(j << 1)]) / (scnPts[(j << 1) + 2] - scnPts[(j << 1)]);
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

						Double tempY = scnPts[(j << 1) + 1] + (scnPts[(j << 1) + 3] - scnPts[(j << 1) + 1]) * (nextX - scnPts[(j << 1)]) / (scnPts[(j << 1) + 2] - scnPts[(j << 1)]);
						tempY -= currY;
						if (tempY < 0)
							tempY = -tempY;
						if (tempY > (szLast[1] + szThis[1]) * 0.5)
						{
							currY = nextY;
							currX = scnPts[(j << 1) + 0] + (scnPts[(j << 1) + 2] - scnPts[(j << 1) + 0]) * (currY - scnPts[(j << 1) + 1]) / (scnPts[(j << 1) + 3] - scnPts[(j << 1) + 1]);
						}
						else
						{
							currX = nextX;
							currY = scnPts[(j << 1) + 1] + (scnPts[(j << 1) + 3] - scnPts[(j << 1) + 1]) * (nextX - scnPts[(j << 1)]) / (scnPts[(j << 1) + 2] - scnPts[(j << 1)]);
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
			Double tempY = scnPts[(lastAInd << 1) + 1] + (scnPts[(lastAInd << 1) + 3] - scnPts[(lastAInd << 1) + 1]) * (nextX - scnPts[(lastAInd << 1)]) / (scnPts[(lastAInd << 1) + 2] - scnPts[(lastAInd << 1)]);
			Double tempX = scnPts[(lastAInd << 1) + 0] + (scnPts[(lastAInd << 1) + 2] - scnPts[(lastAInd << 1) + 0]) * (nextY - scnPts[(lastAInd << 1) + 1]) / (scnPts[(lastAInd << 1) + 3] - scnPts[(lastAInd << 1) + 1]);
			tempY -= lastY;
			tempX -= lastX;
			if (tempY < 0)
				tempY = -tempY;
			if (tempX < 0)
				tempX = -tempX;
			if (tempX <= (szLast[0] + szThis[0]) * 0.5)
			{
				currY = nextY;
				currX = scnPts[(lastAInd << 1) + 0] + (scnPts[(lastAInd << 1) + 2] - scnPts[(lastAInd << 1) + 0]) * (nextY - scnPts[(lastAInd << 1) + 1]) / (scnPts[(lastAInd << 1) + 3] - scnPts[(lastAInd << 1) + 1]);
			}
			else
			{
				currX = nextX;
				currY = scnPts[(lastAInd << 1) + 1] + (scnPts[(lastAInd << 1) + 3] - scnPts[(lastAInd << 1) + 1]) * (nextX - scnPts[(lastAInd << 1)]) / (scnPts[(lastAInd << 1) + 2] - scnPts[(lastAInd << 1)]);
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

		lastX = currX;
		lastY = currY;
		if (mode == 0)
		{
			if (font && font->buffSize > 0)
			{
				denv->img->DrawStringB(currX, currY, CSTRP(lbl, nextPos), df, font->buffBrush, (UInt32)Double2Int32(UOSInt2Double(font->buffSize) * denv->img->GetHDPI() / 96.0));
			}
			denv->img->DrawString(currX, currY, CSTRP(lbl, nextPos), df, db);
		}
		else
		{
			if (font && font->buffSize > 0)
			{
				denv->img->DrawStringB(currX, currY, CSTRP(lbl, nextPos), df, font->buffBrush, (UInt32)Double2Int32(UOSInt2Double(font->buffSize) * denv->img->GetHDPI() / 96.0));
			}
			denv->img->DrawString(currX, currY, CSTRP(lbl, nextPos), df, db);
		}
		szLast[0] = szThis[0];
		szLast[1] = szThis[1];
	}

	realBounds[0] = minX;
	realBounds[1] = minY;
	realBounds[2] = maxX;
	realBounds[3] = maxY;
}

Map::DrawMapRenderer::DrawMapRenderer(Media::DrawEngine *eng, Map::MapEnv *env, const Media::ColorProfile *color, Media::ColorManagerSess *colorSess, DrawType drawType)
{
	this->eng = eng;
	this->env = env;
	this->color.Set(color);
	this->colorSess = colorSess;
	this->lastLayerEmpty = true;
	this->drawType = drawType;
	Media::ColorProfile srcColor(Media::ColorProfile::CPT_SRGB);
	NEW_CLASS(this->resizer, Media::Resizer::LanczosResizer8_C8(3, 3, &srcColor, &this->color, colorSess, Media::AT_NO_ALPHA));
	NEW_CLASS(this->colorConv, Media::ColorConv(&srcColor, &this->color, colorSess));
}

Map::DrawMapRenderer::~DrawMapRenderer()
{
	DEL_CLASS(this->resizer);
	DEL_CLASS(this->colorConv);
}

void Map::DrawMapRenderer::DrawMap(Media::DrawImage *img, Map::MapView *view, UInt32 *imgDurMS)
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
	denv.objBounds = MemAlloc(Double, this->env->GetNString() << 2);
	denv.objCnt = 0;
	denv.labelCnt = 0;
	denv.labels = MemAlloc(Map::DrawMapRenderer::MapLabels, denv.maxLabels = this->env->GetNString());
	denv.fontStyleCnt = env->GetFontStyleCount();
	denv.fontStyles = MemAlloc(Map::DrawMapRenderer::DrawFontStyle, denv.fontStyleCnt);
	denv.imgDurMS = 0;
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
		env->GetFontStyle(i, &fontName, &fontSizePt, &bold, &fontColor, &buffSize, &buffColor);
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

	this->DrawLayers(&denv, 0);
	DrawLabels(&denv);

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
	MemFree(denv.objBounds);
	MemFree(denv.labels);

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
