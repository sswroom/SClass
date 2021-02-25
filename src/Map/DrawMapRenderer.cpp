#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/Sort/ArtificialQuickSort.h"
#include "Map/DrawMapRenderer.h"
#include "Math/Math.h"
#include "Math/Geometry.h"
#include "Math/VectorImage.h"
#include "Media/Resizer/LanczosResizer8_C8.h"
#include "Text/MyString.h"

#define IsDoorNum(a) Text::StrIsInt32(a)
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
		if (j == (UOSInt)-1)
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

void Map::DrawMapRenderer::SwapLabel(MapLabels *mapLabels, OSInt index, OSInt index2)
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

Bool Map::DrawMapRenderer::AddLabel(MapLabels *labels, UOSInt maxLabel, UOSInt *labelCnt, const UTF8Char *label, UOSInt nPoints, Double *points, Int32 priority, Map::DrawLayerType recType, UOSInt fontStyle, Int32 flags, Map::MapView *view, OSInt xOfst, OSInt yOfst)
{
	Double size;
	Double visibleSize;

	UOSInt i;
	UOSInt j;

	Double *ptPtr;

	Double scnX;
	Double scnY;
	Int32 found;
	if (label == 0 || label[0] == 0)
		return false;

	Double left = view->GetLeftX();
	Double right = view->GetRightX();
	Double top = view->GetTopY();
	Double bottom = view->GetBottomY();
	Double mapPosLat = view->GetCenterY();
	Double mapPosLon = view->GetCenterX();

	if (recType == Map::DRAW_LAYER_POINT || recType == Map::DRAW_LAYER_POINT3D) //Point
	{
		found = 0;
		i = 0;
		while (i < *labelCnt)
		{
			if (recType == labels[i].layerType)
			{
				if (Text::StrCompare(labels[i].label, label) == 0)
				{
					found = 1;

					ptPtr = points;
					j = nPoints;
					while (j--)
					{
						scnX = mapPosLon - *ptPtr++;
						scnY = mapPosLat - *ptPtr++;
						scnX = scnX * scnX + scnY * scnY;
						if (scnX < labels[i].currSize)
						{
							labels[i].lon = ptPtr[-2];
							labels[i].lat = ptPtr[-1];
							labels[i].fontStyle = fontStyle;
							labels[i].scaleW = 0;
							labels[i].scaleH = 0;
							labels[i].currSize = scnX;
							labels[i].xOfst = xOfst;
							labels[i].yOfst = yOfst;
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
			ptPtr = points;
			j = nPoints;
			while (j--)
			{
				if (ptPtr[0] >= left && ptPtr[0] < right && ptPtr[1] >= top && ptPtr[1] < bottom)
				{
					found = 1;

					i = NewLabel(labels, maxLabel, labelCnt, priority);
					if (i == (UOSInt)-1)
					{
						return false;
					}

					labels[i].label = Text::StrCopyNew(label);
					labels[i].lon = ptPtr[0];
					labels[i].lat = ptPtr[1];
					labels[i].fontStyle = fontStyle;
					labels[i].scaleW = 0;
					labels[i].scaleH = 0;
					labels[i].priority = priority;

					scnX = mapPosLon - ptPtr[0];
					scnY = mapPosLat - ptPtr[1];
					scnX = scnX * scnX + scnY * scnY;

					labels[i].currSize = scnX;
					labels[i].totalSize = Math::OSInt2Double(nPoints);
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
						scnX = mapPosLon - *ptPtr++;
						scnY = mapPosLat - *ptPtr++;
						scnX = scnX * scnX + scnY * scnY;
						if (scnX < labels[i].currSize)
						{
							labels[i].lon = ptPtr[-2];
							labels[i].lat = ptPtr[-1];
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
				ptPtr += 2;
			}
		}
		return false;
	}
	else if (recType == Map::DRAW_LAYER_POLYLINE || recType == Map::DRAW_LAYER_POLYLINE3D) //lines
	{
		Double tmp;
		Double lastPtX;
		Double lastPtY = points[1];
		Double thisPtX;
		Double thisPtY;
		UOSInt i;
		Int32 toUpdate;

		visibleSize = 0;
		size = 0;

		i = 1;
		while (i < nPoints)
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
					lastPtY += (left - lastPtX) * (thisPtY - lastPtY) / (thisPtX - lastPtX);
					lastPtX = left;
				}
				if (top > lastPtY)
				{
					lastPtX += (top - lastPtY) * (thisPtX - lastPtX) / (thisPtY - lastPtY);
					lastPtY = top;
				}
				if (right < thisPtX)
				{
					thisPtY += (right - lastPtX) * (thisPtY - lastPtY) / (thisPtX - lastPtX);
					thisPtX = right;
				}
				if (bottom < thisPtY)
				{
					thisPtX += (bottom - lastPtY) * (thisPtX - lastPtX) / (thisPtY - lastPtY);
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
		Double totalSize = 0;
		OSInt foundInd;
		Double startX;
		Double startY;
		Double endX;
		Double endY;
		startX = points[0];
		startY = points[1];
		endX = points[(nPoints << 1) - 2];
		endY = points[(nPoints << 1) - 1];
		i = 0;

		while (i < *labelCnt)
		{
			if (recType == labels[i].layerType)
			{
				if (Text::StrCompare(labels[i].label, label) == 0)
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
						OSInt newSize = labels[i].nPoints + nPoints - 1;
						Double* newArr = MemAlloc(Double, newSize << 1);

						MemCopyNO(newArr, points, nPoints << 4);
						MemCopyNO(&newArr[nPoints << 1], &labels[i].points[2], (labels[i].nPoints - 1) << 4);

						startX = newArr[0];
						startY = newArr[1];
						endX = newArr[(newSize << 1) - 2];
						endY = newArr[(newSize << 1) - 1];

						MemFree(labels[i].points);
						labels[i].points = newArr;
						labels[i].nPoints = newSize;
						labels[i].currSize += visibleSize;
						toUpdate = 0;
						foundInd = i;
					}
					else if (labels[i].points[(labels[i].nPoints << 1) - 2] == startX && labels[i].points[(labels[i].nPoints << 1) - 1] == startY)
					{
//						wprintf(L"Shape: %s merged (%d + %d)\n", labelt, labels[i].nPoints, nPoint);
						OSInt newSize = labels[i].nPoints + nPoints - 1;
						Double* newArr = MemAlloc(Double, newSize << 1);

						MemCopyNO(newArr, labels[i].points, labels[i].nPoints << 4);
						MemCopyNO(&newArr[labels[i].nPoints << 1], &points[2], (nPoints - 1) << 4);

						startX = newArr[0];
						startY = newArr[1];
						endX = newArr[(newSize << 1) - 2];
						endY = newArr[(newSize << 1) - 1];

						MemFree(labels[i].points);
						labels[i].points = newArr;
						labels[i].nPoints = newSize;
						labels[i].currSize += visibleSize;
						toUpdate = 0;
						foundInd = i;
					}
					else if (labels[i].points[0] == startX && labels[i].points[1] == startY)
					{
//						wprintf(L"Shape: %s inverse merged (%d + %d)\n", labelt, labels[i].nPoints, nPoint);
						OSInt newSize = labels[i].nPoints + nPoints - 1;
						Double* newArr = MemAlloc(Double, newSize << 1);
						OSInt k;
						OSInt l;
						l = 0;
						k = labels[i].nPoints;
						while (k-- > 1)
						{
							newArr[l++] = labels[i].points[k << 1];
							newArr[l++] = labels[i].points[(k << 1) + 1];
						}
						MemCopyNO(&newArr[l], points, nPoints << 4);

						startX = newArr[0];
						startY = newArr[1];
						endX = newArr[(newSize << 1) - 2];
						endY = newArr[(newSize << 1) - 1];

						MemFree(labels[i].points);
						labels[i].points = newArr;
						labels[i].nPoints = newSize;
						labels[i].currSize += visibleSize;
						toUpdate = 0;
						foundInd = i;
					}
					else if (labels[i].points[(labels[i].nPoints << 1) - 2] == endX && labels[i].points[(labels[i].nPoints << 1) - 1] == endY)
					{
//						wprintf(L"Shape: %s inverse merged (%d + %d)\n", labelt, labels[i].nPoints, nPoint);
						OSInt newSize = labels[i].nPoints + nPoints - 1;
						Double* newArr = MemAlloc(Double, newSize << 1);
						MemCopyNO(newArr, labels[i].points, labels[i].nPoints << 4);
						OSInt k;
						OSInt l;
						l = labels[i].nPoints << 1;
						k = nPoints - 1;
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
			labels[i].totalSize = size;
			labels[i].currSize = visibleSize;
			labels[i].flags = flags;

			labels[i].label = Text::StrCopyNew(label);
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
				MemFree(labels[i].points);
			labels[i].points = ptPtr = MemAlloc(Double, nPoints * 2);
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
					if (Text::StrCompare(labels[i].label, label) == 0)
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

			Double *tmpPts;
			tmpPts = MemAlloc(Double, nPoints << 2);
			outPtCnt = Math::Geometry::BoundPolygonY(points, nPoints, tmpPts, top, bottom, 0, 0);
			outPts = MemAlloc(Double, nPoints << 2);
			outPtCnt = Math::Geometry::BoundPolygonX(tmpPts, outPtCnt, outPts, left, right, 0, 0);
			MemFree(tmpPts);

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
				UOSInt finalCnt;
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

				thisX = ((maxX + minX) * 0.5);
				thisY = ((maxY + minY) * 0.5);

				i = NewLabel(labels, maxLabel, labelCnt, priority);
				if (i == (UOSInt)-1)
				{
					MemFree(outPts);
					return false;
				}

				labels[i].label = Text::StrCopyNew(label);
				labels[i].lon = thisX;
				labels[i].lat = thisY;
				labels[i].fontStyle = fontStyle;
				labels[i].scaleW = 0;
				labels[i].scaleH = 0;
				labels[i].priority = priority;

				labels[i].currSize = Math::OSInt2Double(outPtCnt);
				labels[i].totalSize = Math::OSInt2Double(outPtCnt);
				labels[i].nPoints = outPtCnt;
				labels[i].layerType = recType;
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

void Map::DrawMapRenderer::DrawLabels(Map::DrawMapRenderer::DrawEnv *denv)
{
	UOSInt i;
	UOSInt j;
	Bool overlapped;
	const UTF8Char *lastLbl = 0;
//	Double leftLon = denv->view->GetLeftX();
//	Double topLat = denv->view->GetTopY();
//	Double rightLon = denv->view->GetRightX();
//	Double bottomLat = denv->view->GetBottomY();
	OSInt scnWidth = denv->img->GetWidth();
	OSInt scnHeight = denv->img->GetHeight();
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
		Double dscnX;
		Double dscnY;
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
				GetCharsSize(denv, szThis, denv->labels[i].label, denv->labels[i].fontStyle, 0, 0);//labels[i].scaleW, labels[i].scaleH);
				denv->view->MapXYToScnXY(denv->labels[i].lon, denv->labels[i].lat, &dscnX, &dscnY);
				scnPtX = dscnX;
				scnPtY = dscnY;

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
					tlx = scnPtX + 1 + (denv->labels[i].xOfst >> 1);
					brx = tlx + szThis[0];
					tly = scnPtY - (szThis[1] * 0.5);
					bry = tly + szThis[1];

					overlapped = LabelOverlapped(denv->objBounds, currPt, tlx, tly, brx, bry);
				}
				if (overlapped)
				{
					tlx = scnPtX - szThis[0] - 1 - (denv->labels[i].xOfst >> 1);
					brx = tlx + szThis[0];
					tly = scnPtY - (szThis[1] * 0.5);
					bry = tly + szThis[1];

					overlapped = LabelOverlapped(denv->objBounds, currPt, tlx, tly, brx, bry);
				}
				if (overlapped)
				{
					tlx = scnPtX - (szThis[0] * 0.5);
					brx = tlx + szThis[0];
					tly = scnPtY - szThis[1] - 1 - (denv->labels[i].yOfst >> 1);
					bry = tly + szThis[1];

					overlapped = LabelOverlapped(denv->objBounds, currPt, tlx, tly, brx, bry);
				}
				if (overlapped)
				{
					tlx = scnPtX - (szThis[0] * 0.5);
					brx = tlx + szThis[0];
					tly = scnPtY + 1 + (denv->labels[i].yOfst >> 1);
					bry = tly + szThis[1];

					overlapped = LabelOverlapped(denv->objBounds, currPt, tlx, tly, brx, bry);
				}

				if (!overlapped)
				{
					DrawChars(denv, denv->labels[i].label, (tlx + brx) * 0.5, (tly + bry) * 0.5, 0, 0, denv->labels[i].fontStyle, 0);

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
					if (Text::StrCompare(lastLbl, denv->labels[i].label) != 0)
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
						Text::StrDelNew(lastLbl);
					lastLbl = Text::StrCopyNew(denv->labels[i].label);
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
					j = (denv->labels[i].nPoints - 1) & ~1;
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
					scnPtX = Math::OSInt2Double((maxX + minX) >> 1);
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
					scnPtY = Math::OSInt2Double((maxY + minY) >> 1);
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
					denv->labels[i].scaleW = denv->labels[i].points[(k << 1) + 2] - denv->labels[i].points[(k << 1)];
					denv->labels[i].scaleH = denv->labels[i].points[(k << 1) + 3] - denv->labels[i].points[(k << 1) + 1];
				}
				else
				{
					denv->labels[i].scaleW = 0;
					denv->labels[i].scaleH = 0;
				}
				GetCharsSize(denv, szThis, denv->labels[i].label, denv->labels[i].fontStyle, denv->labels[i].scaleW, denv->labels[i].scaleH);
				if (xDiff < szThis[0] && yDiff < szThis[1])
				{
					tlx = scnPtX - (szThis[0] * 0.5);
					brx = tlx + szThis[0];
					tly = scnPtY - (szThis[1] * 0.5);
					bry = tly + szThis[1];

					overlapped = LabelOverlapped(denv->objBounds, currPt, tlx, tly, brx, bry);
					if (!overlapped)
					{
						DrawChars(denv, denv->labels[i].label, (tlx + brx) * 0.5, (tly + bry) * 0.5, denv->labels[i].scaleW, denv->labels[i].scaleH, denv->labels[i].fontStyle, (denv->labels[i].flags & Map::MapEnv::SFLG_ALIGN) != 0);

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
									scnPtX += scnDiff - 1;
									if (scnPtX <= minX)
									{
										scnDiff = -scnDiff;
										scnPtX = Math::OSInt2Double(((minX + maxX) >> 1) + scnDiff + 1);
									}
								}
								else
								{
									scnPtX += scnDiff + 1;
									if (scnPtX >= maxX)
									{
										scnDiff = scnDiff >> 1;
										if (scnDiff < 30)
											break;
										scnDiff = -scnDiff;
										scnPtX = Math::OSInt2Double(((minX + maxX) >> 1) + scnDiff - 1);
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
									scnPtY += scnDiff - 1;
									if (scnPtY <= minY)
									{
										scnDiff = -scnDiff;
										scnPtY = Math::OSInt2Double(((minY + maxY) >> 1) + scnDiff + 1);
									}
								}
								else
								{
									scnPtY += scnDiff + 1;
									if (scnPtY >= maxY)
									{
										scnDiff = scnDiff >> 1;
										if (scnDiff < 30)
											break;
										scnDiff = -scnDiff;
										scnPtY = Math::OSInt2Double(((minY + maxY) >> 1) + scnDiff - 1);
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
								denv->labels[i].scaleW = denv->labels[i].points[(k << 1) + 2] - denv->labels[i].points[(k << 1)];
								denv->labels[i].scaleH = denv->labels[i].points[(k << 1) + 3] - denv->labels[i].points[(k << 1) + 1];
							}
							else
							{
								denv->labels[i].scaleW = 0;
								denv->labels[i].scaleH = 0;
							}
							GetCharsSize(denv, szThis, denv->labels[i].label, denv->labels[i].fontStyle, denv->labels[i].scaleW, denv->labels[i].scaleH);
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
							if ((denv->labels[i].flags & Map::MapEnv::SFLG_ALIGN) != 0)
							{
								Double realBounds[4];
								DrawCharsLA(denv, denv->labels[i].label, denv->labels[i].points, points, denv->labels[i].nPoints, k, scaleN, scaleD, denv->labels[i].fontStyle, realBounds);

								denv->objBounds[(currPt << 2)] = realBounds[0];
								denv->objBounds[(currPt << 2) + 1] = realBounds[1];
								denv->objBounds[(currPt << 2) + 2] = realBounds[2];
								denv->objBounds[(currPt << 2) + 3] = realBounds[3];
								currPt++;
							}
							else if ((denv->labels[i].flags & Map::MapEnv::SFLG_ROTATE) != 0)
							{
								Double realBounds[4];
								DrawCharsL(denv, denv->labels[i].label, denv->labels[i].points, points, denv->labels[i].nPoints, k, scaleN, scaleD, denv->labels[i].fontStyle, realBounds);

								denv->objBounds[(currPt << 2)] = realBounds[0];
								denv->objBounds[(currPt << 2) + 1] = realBounds[1];
								denv->objBounds[(currPt << 2) + 2] = realBounds[2];
								denv->objBounds[(currPt << 2) + 3] = realBounds[3];
								currPt++;
							}
							else
							{
								DrawChars(denv, denv->labels[i].label, (tlx + brx) * 0.5, (tly + bry) * 0.5, denv->labels[i].scaleW, denv->labels[i].scaleH, denv->labels[i].fontStyle, (denv->labels[i].flags & Map::MapEnv::SFLG_ALIGN) != 0);

								denv->objBounds[(currPt << 2)] = tlx;
								denv->objBounds[(currPt << 2) + 1] = tly;
								denv->objBounds[(currPt << 2) + 2] = brx;
								denv->objBounds[(currPt << 2) + 3] = bry;
								currPt++;
							}

							thisPts[thisCnt++] = Math::Double2Int32((tlx + brx) * 0.5);
							thisPts[thisCnt++] = Math::Double2Int32((tly + bry) * 0.5);
						}
						j = (thisCnt < 10);
					}
				}
				MemFree(points);
			}
			else if (denv->labels[i].layerType == Map::DRAW_LAYER_POLYGON)
			{
				GetCharsSize(denv, szThis, denv->labels[i].label, denv->labels[i].fontStyle, 0, 0);//labels[i].scaleW, labels[i].scaleH);
				denv->view->MapXYToScnXY(denv->labels[i].lon, denv->labels[i].lat, &dscnX, &dscnY);
				scnPtX = dscnX;
				scnPtY = dscnY;

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
					DrawChars(denv, denv->labels[i].label, (tlx + brx) * 0.5, (tly + bry) * 0.5, 0, 0, denv->labels[i].fontStyle, 0);

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
		Text::StrDelNew(denv->labels[i].label);
		if (denv->labels[i].points)
			MemFree(denv->labels[i].points);
	}
	if (lastLbl)
		Text::StrDelNew(lastLbl);
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
	Double x1;
	Double y1;
	Double x2;
	Double y2;
	vimg1->GetCenter(&x1, &y1);
	vimg2->GetCenter(&x2, &y2);
	if (y2 > y1)
	{
		return 1;
	}
	else if (y1 > y2)
	{
		return -1;
	}
	else if (x1 > x2)
	{
		return 1;
	}
	else if (x1 < x2)
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
	OSInt i = 0;
	OSInt j = denv->env->GetItemCount(group);

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
								DrawPLLayer(denv, layer.layer, -1, layer.lineThick, layer.lineColor);
							}

						}
						if (layer.flags & Map::MapEnv::SFLG_SHOWLABEL)
						{
							if (layer.fontType == 0)
							{
								if (layer.fontStyle < denv->fontStyleCnt)
								{
									DrawLabel(denv, layer.layer, layer.fontStyle, layer.labelCol, layer.priority, layer.flags, 0, 0, layerType);
								}
							}
							else if (layer.fontType == 1)
							{
								OSInt fs = denv->layerFont->GetCount();
								Media::DrawFont *f = denv->img->NewFontPt(layer.fontName, layer.fontSizePt, Media::DrawEngine::DFS_NORMAL, 0);
								Media::DrawBrush *b = denv->img->NewBrushARGB(this->colorConv->ConvRGB8(layer.fontColor));
								denv->layerFont->Add(f);
								denv->layerFontColor->Add(b);
								DrawLabel(denv, layer.layer, -fs - 1, layer.labelCol, layer.priority, layer.flags, 0, 0, layerType);
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
								DrawPGLayer(denv, layer.layer, -1, layer.fillStyle, layer.lineThick, layer.lineColor);
							}
						}
						if (layer.flags & Map::MapEnv::SFLG_SHOWLABEL)
						{
							if (layer.fontType == 0)
							{
								if (layer.fontStyle < denv->fontStyleCnt)
								{
									DrawLabel(denv, layer.layer, layer.fontStyle, layer.labelCol, layer.priority, layer.flags, 0, 0, layerType);
								}
							}
							else if (layer.fontType == 1)
							{
								OSInt fs = denv->layerFont->GetCount();
								Media::DrawFont *f = denv->img->NewFontPt(layer.fontName, layer.fontSizePt, Media::DrawEngine::DFS_NORMAL, 0);
								Media::DrawBrush *b = denv->img->NewBrushARGB(this->colorConv->ConvRGB8(layer.fontColor));
								denv->layerFont->Add(f);
								denv->layerFontColor->Add(b);
								DrawLabel(denv, layer.layer, -fs - 1, layer.labelCol, layer.priority, layer.flags, 0, 0, layerType);
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
							Int32 imgDurMS = 0;
							if (layer.layer->HasIconStyle())
							{
								pimg = layer.layer->GetIconStyleImg()->GetImage(0, &imgDurMS);
								spotX = Math::OSInt2Double(layer.layer->GetIconStyleSpotX());
								spotY = Math::OSInt2Double(layer.layer->GetIconStyleSpotY());
								if (pimg != 0 && (spotX == -1 || spotY == -1))
								{
									spotX = pimg->info->dispWidth * 0.5;
									spotY = pimg->info->dispHeight * 0.5;
								}
							}
							if (pimg == 0)
							{
								pimg = denv->env->GetImage(layer.imgIndex, &imgDurMS);
								if (pimg)
								{
									spotX = pimg->info->dispWidth * 0.5;
									spotY = pimg->info->dispHeight * 0.5;
								}
								else
								{
									spotX = 0;
									spotY = 0;
								}
							}

							if (layer.fontType == 0)
							{
								if (layer.fontStyle < denv->fontStyleCnt)
								{
									if (pimg)
									{
										DrawLabel(denv, layer.layer, layer.fontStyle, layer.labelCol, layer.priority, layer.flags, Math::Double2Int32(pimg->info->dispWidth * denv->img->GetHDPI() / pimg->info->hdpi), Math::Double2Int32(pimg->info->dispHeight * denv->img->GetVDPI() / pimg->info->vdpi), layerType);
									}
									else
									{
										DrawLabel(denv, layer.layer, layer.fontStyle, layer.labelCol, layer.priority, layer.flags, 0, 0, layerType);
									}
								}
							}
							else if (layer.fontType == 1)
							{
								OSInt fs = denv->layerFont->GetCount();
								Media::DrawFont *f = denv->img->NewFontPt(layer.fontName, layer.fontSizePt, Media::DrawEngine::DFS_NORMAL, 0);
								Media::DrawBrush *b = denv->img->NewBrushARGB(this->colorConv->ConvRGB8(layer.fontColor));
								denv->layerFont->Add(f);
								denv->layerFontColor->Add(b);
								if (pimg)
								{
									DrawLabel(denv, layer.layer, -fs - 1, layer.labelCol, layer.priority, layer.flags, Math::Double2Int32(pimg->info->dispWidth * denv->img->GetHDPI() / pimg->info->hdpi), Math::Double2Int32(pimg->info->dispHeight * denv->img->GetVDPI() / pimg->info->vdpi), layerType);
								}
								else
								{
									DrawLabel(denv, layer.layer, -fs - 1, layer.labelCol, layer.priority, layer.flags, 0, 0, layerType);
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
								DrawPLLayer(denv, layer.layer, -1, layer.lineThick, layer.lineColor);
								layer.layer->SetMixedType(Map::DRAW_LAYER_POLYGON);
								DrawPGLayer(denv, layer.layer, -1, layer.fillStyle, layer.lineThick, layer.lineColor);
							}
						}
						if (layer.flags & Map::MapEnv::SFLG_SHOWLABEL)
						{
							if (layer.fontType == 0)
							{
								layer.layer->SetMixedType(Map::DRAW_LAYER_POLYLINE3D);
								if (layer.fontStyle < denv->fontStyleCnt)
								{
									DrawLabel(denv, layer.layer, layer.fontStyle, layer.labelCol, layer.priority, layer.flags, 0, 0, Map::DRAW_LAYER_POLYLINE3D);
								}
								layer.layer->SetMixedType(Map::DRAW_LAYER_POLYGON);
								if (layer.fontStyle < denv->fontStyleCnt)
								{
									DrawLabel(denv, layer.layer, layer.fontStyle, layer.labelCol, layer.priority, layer.flags, 0, 0, Map::DRAW_LAYER_POLYGON);
								}
							}
							else if (layer.fontType == 1)
							{
								OSInt fs = denv->layerFont->GetCount();
								Media::DrawFont *f = denv->img->NewFontPt(layer.fontName, layer.fontSizePt, Media::DrawEngine::DFS_NORMAL, 0);
								Media::DrawBrush *b = denv->img->NewBrushARGB(this->colorConv->ConvRGB8(layer.fontColor));
								denv->layerFont->Add(f);
								denv->layerFontColor->Add(b);
								layer.layer->SetMixedType(Map::DRAW_LAYER_POLYLINE3D);
								DrawLabel(denv, layer.layer, -fs - 1, layer.labelCol, layer.priority, layer.flags, 0, 0, Map::DRAW_LAYER_POLYLINE3D);
								layer.layer->SetMixedType(Map::DRAW_LAYER_POLYGON);
								DrawLabel(denv, layer.layer, -fs - 1, layer.labelCol, layer.priority, layer.flags, 0, 0, Map::DRAW_LAYER_POLYGON);
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

void Map::DrawMapRenderer::DrawPLLayer(Map::DrawMapRenderer::DrawEnv *denv, Map::IMapDrawLayer *layer, UOSInt lineStyle, UOSInt lineThick, Int32 lineColor)
{
	OSInt i;
	OSInt j;
	OSInt k;
	void *session;
	Media::DrawPen *p;
	Int64 lastId;
	Int64 thisId;
	Int32 layerId = 0;
	Map::DrawObjectL *dobj;
	Math::CoordinateSystem *lyrCSys = layer->GetCoordinateSystem();
	Math::CoordinateSystem *envCSys = this->env->GetCoordinateSystem();

	denv->idArr->Clear();
	if (lyrCSys != 0 && envCSys != 0 && !lyrCSys->Equals(envCSys))
	{
		Double x1;
		Double y1;
		Double x2;
		Double y2;
		x1 = denv->view->GetLeftX();
		y1 = denv->view->GetTopY();
		x2 = denv->view->GetRightX();
		y2 = denv->view->GetBottomY();
		Math::CoordinateSystem::ConvertXYZ(envCSys, lyrCSys, x1, y1, 0, &x1, &y1, 0);
		Math::CoordinateSystem::ConvertXYZ(envCSys, lyrCSys, x2, y2, 0, &x2, &y2, 0);
		layer->GetObjectIdsMapXY(denv->idArr, 0, x1, y1, x2, y2, true);

		if ((i = denv->idArr->GetCount()) > 0)
		{
			Int32 color;
			OSInt thick;
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
			p = denv->img->NewPenARGB(this->colorConv->ConvRGB8(color), thick * denv->img->GetHDPI() / 96.0, pattern, npattern);
			//pen = CreatePen(img, lyrs->style, 0, osSize);
			this->mapSch->SetDrawType(layer, Map::MapScheduler::MSDT_POLYLINE, p, 0, 0, 0.0, 0.0, &denv->isLayerEmpty);

			session = layer->BeginGetObject();
			lastId = -1;
			j = i;
			i = 0;
			while (i < j)
			{
				thisId = denv->idArr->GetItem(i);
				if (thisId != lastId)
				{
					lastId = thisId;
					if ((dobj = layer->GetObjectByIdD(session, thisId)) != 0)
					{
						k = dobj->nPoint;
						while (k-- > 0)
						{
							Math::CoordinateSystem::ConvertXYZ(lyrCSys, envCSys, dobj->pointArr[(k << 1)], dobj->pointArr[(k << 1) + 1], 0, &dobj->pointArr[(k << 1)], &dobj->pointArr[(k << 1) + 1], 0);
						}
						this->mapSch->Draw(dobj);
					}
				}
				i++;
			}
			layer->EndGetObject(session);

			if (found)
			{
				while (denv->env->GetLineStyleLayer(lineStyle, layerId++, &color, &thick, &pattern, &npattern))
				{
					p = denv->img->NewPenARGB(this->colorConv->ConvRGB8(color), thick * denv->img->GetHDPI() / 96.0, pattern, npattern);
					mapSch->DrawNextType(p, 0);
				}
			}
			mapSch->WaitForFinish();
		}
	}
	else
	{
		layer->GetObjectIdsMapXY(denv->idArr, 0, denv->view->GetLeftX(), denv->view->GetTopY(), denv->view->GetRightX(), denv->view->GetBottomY(), true);

		if ((i = denv->idArr->GetCount()) > 0)
		{
			Int32 color;
			OSInt thick;
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
			p = denv->img->NewPenARGB(this->colorConv->ConvRGB8(color), thick * denv->img->GetHDPI() / 96.0, pattern, npattern);
			//pen = CreatePen(img, lyrs->style, 0, osSize);
			this->mapSch->SetDrawType(layer, Map::MapScheduler::MSDT_POLYLINE, p, 0, 0, 0.0, 0.0, &denv->isLayerEmpty);

			session = layer->BeginGetObject();
			lastId = -1;
			j = i;
			i = 0;
			while (i < j)
			{
				thisId = denv->idArr->GetItem(i);
				if (thisId != lastId)
				{
					lastId = thisId;
					if ((dobj = layer->GetObjectByIdD(session, thisId)) != 0)
					{
						this->mapSch->Draw(dobj);
					}
				}
				i++;
			}
			layer->EndGetObject(session);

			if (found)
			{
				while (denv->env->GetLineStyleLayer(lineStyle, layerId++, &color, &thick, &pattern, &npattern))
				{
					p = denv->img->NewPenARGB(this->colorConv->ConvRGB8(color), thick * denv->img->GetHDPI() / 96.0, pattern, npattern);
					mapSch->DrawNextType(p, 0);
				}
			}
			mapSch->WaitForFinish();
		}
	}
}

void Map::DrawMapRenderer::DrawPGLayer(Map::DrawMapRenderer::DrawEnv *denv, Map::IMapDrawLayer *layer, UOSInt lineStyle, Int32 fillStyle, UOSInt lineThick, Int32 lineColor)
{
	OSInt i;
	OSInt j;
	void *session;
	Media::DrawPen *p;
	Media::DrawBrush *b;
	Int64 lastId;
	Int64 thisId;
	Int32 layerId = 0;
	Map::DrawObjectL *dobj;
	Math::CoordinateSystem *lyrCSys = layer->GetCoordinateSystem();
	Math::CoordinateSystem *envCSys = this->env->GetCoordinateSystem();
	denv->idArr->Clear();
	if (lyrCSys != 0 && envCSys != 0 && !lyrCSys->Equals(envCSys))
	{
		Double x1;
		Double y1;
		Double x2;
		Double y2;
		x1 = denv->view->GetLeftX();
		y1 = denv->view->GetTopY();
		x2 = denv->view->GetRightX();
		y2 = denv->view->GetBottomY();
		Math::CoordinateSystem::ConvertXYZ(envCSys, lyrCSys, x1, y1, 0, &x1, &y1, 0);
		Math::CoordinateSystem::ConvertXYZ(envCSys, lyrCSys, x2, y2, 0, &x2, &y2, 0);
		layer->GetObjectIdsMapXY(denv->idArr, 0, x1, y1, x2, y2, true);

		if ((i = denv->idArr->GetCount()) > 0)
		{
			Int32 color;
			OSInt thick;
			UInt8 *pattern;
			UOSInt npattern;
			Bool found;
			found = denv->env->GetLineStyleLayer(lineStyle, layerId++, &color, &thick, &pattern, &npattern);
			p = denv->img->NewPenARGB(this->colorConv->ConvRGB8(color), thick * denv->img->GetHDPI() / 96.0, pattern, npattern);
			b = denv->img->NewBrushARGB(this->colorConv->ConvRGB8(fillStyle));
			this->mapSch->SetDrawType(layer, Map::MapScheduler::MSDT_POLYGON, p, b, 0, 0.0, 0.0, &denv->isLayerEmpty);

			session = layer->BeginGetObject();
			lastId = -1;
			while (i-- > 0)
			{
				thisId = denv->idArr->GetItem(i);
				if (thisId != lastId)
				{
					lastId = thisId;
					if ((dobj = layer->GetObjectByIdD(session, thisId)) != 0)
					{
						j = dobj->nPoint;
						while (j-- > 0)
						{
							Math::CoordinateSystem::ConvertXYZ(lyrCSys, envCSys, dobj->pointArr[(j << 1)], dobj->pointArr[(j << 1) + 1], 0, &dobj->pointArr[(j << 1)], &dobj->pointArr[(j << 1) + 1], 0);
						}
						this->mapSch->Draw(dobj);
					}
				}
			}
			layer->EndGetObject(session);

			if (found)
			{
				while (denv->env->GetLineStyleLayer(lineStyle, layerId++, &color, &thick, &pattern, &npattern))
				{
					p = denv->img->NewPenARGB(this->colorConv->ConvRGB8(color), thick * denv->img->GetHDPI() / 96.0, pattern, npattern);
					mapSch->DrawNextType(p, 0);
				}
			}
			mapSch->WaitForFinish();
		}
	}
	else
	{
		layer->GetObjectIdsMapXY(denv->idArr, 0, denv->view->GetLeftX(), denv->view->GetTopY(), denv->view->GetRightX(), denv->view->GetBottomY(), true);

		if ((i = denv->idArr->GetCount()) > 0)
		{
			Int32 color;
			OSInt thick;
			UInt8 *pattern;
			UOSInt npattern;
			Bool found;
			found = denv->env->GetLineStyleLayer(lineStyle, layerId++, &color, &thick, &pattern, &npattern);
			p = denv->img->NewPenARGB(this->colorConv->ConvRGB8(color), thick * denv->img->GetHDPI() / 96.0, pattern, npattern);
			b = denv->img->NewBrushARGB(this->colorConv->ConvRGB8(fillStyle));
			this->mapSch->SetDrawType(layer, Map::MapScheduler::MSDT_POLYGON, p, b, 0, 0.0, 0.0, &denv->isLayerEmpty);

			session = layer->BeginGetObject();
			lastId = -1;
			while (i-- > 0)
			{
				thisId = denv->idArr->GetItem(i);
				if (thisId != lastId)
				{
					lastId = thisId;
					if ((dobj = layer->GetObjectByIdD(session, thisId)) != 0)
					{
						this->mapSch->Draw(dobj);
					}
				}
			}
			layer->EndGetObject(session);

			if (found)
			{
				while (denv->env->GetLineStyleLayer(lineStyle, layerId++, &color, &thick, &pattern, &npattern))
				{
					p = denv->img->NewPenARGB(this->colorConv->ConvRGB8(color), thick * denv->img->GetHDPI() / 96.0, pattern, npattern);
					mapSch->DrawNextType(p, 0);
				}
			}
			mapSch->WaitForFinish();
		}
	}
}

void Map::DrawMapRenderer::DrawPTLayer(Map::DrawMapRenderer::DrawEnv *denv, Map::IMapDrawLayer *layer, UOSInt imgIndex)
{
	Data::ArrayListInt64 *arri;
	Map::DrawObjectL *dobj;
	OSInt i;
	OSInt k;
	void *session;
	Double leftLon = denv->view->GetLeftX();
	Double topLat = denv->view->GetTopY();
	Double rightLon = denv->view->GetRightX();
	Double bottomLat = denv->view->GetBottomY();
	Double spotX;
	Double spotY;
	UOSInt maxLabel = denv->env->GetNString();

	Media::Image *img = 0;
	Int32 imgTimeMS = 0;
	if (layer->HasIconStyle())
	{
		img = layer->GetIconStyleImg()->GetImage(0, &imgTimeMS);
		spotX = Math::OSInt2Double(layer->GetIconStyleSpotX());
		spotY = Math::OSInt2Double(layer->GetIconStyleSpotY());
		if (img != 0 && (spotX == -1 || spotY == -1))
		{
			spotX = img->info->dispWidth * 0.5;
			spotY = img->info->dispHeight * 0.5;
		}
	}
	if (img == 0)
	{
		img = denv->env->GetImage(imgIndex, &imgTimeMS);
		if (img == 0)
			return;
		spotX = img->info->dispWidth * 0.5;
		spotY = img->info->dispHeight * 0.5;
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
	NEW_CLASS(arri, Data::ArrayListInt64());
	if (lyrCSys != 0 && envCSys != 0 && !lyrCSys->Equals(envCSys))
	{
		Math::CoordinateSystem::ConvertXYZ(envCSys, lyrCSys, leftLon, topLat, 0, &leftLon, &topLat, 0);
		Math::CoordinateSystem::ConvertXYZ(envCSys, lyrCSys, rightLon, bottomLat, 0, &rightLon, &bottomLat, 0);
		layer->GetObjectIdsMapXY(arri, 0, leftLon - ((rightLon - leftLon) * 0.5), topLat - ((bottomLat - topLat) * 0.5), rightLon + ((rightLon - leftLon) * 0.5), bottomLat + ((bottomLat - topLat) * 0.5), true);
		if (arri->GetCount() <= 0)
		{
			DEL_CLASS(arri);
			return;
		}
		Media::DrawImage *dimg;
		if (this->drawType == Map::DrawMapRenderer::DT_PIXELDRAW)
		{
			UInt32 newW = Math::Double2Int32(img->info->dispWidth * denv->img->GetHDPI() / img->info->hdpi);
			UInt32 newH = Math::Double2Int32(img->info->dispHeight * denv->img->GetVDPI() / img->info->vdpi);
			if (newW > img->info->dispWidth || newH > img->info->dispHeight)
			{
				this->resizer->SetTargetWidth(newW);
				this->resizer->SetTargetHeight(newH);
				Media::StaticImage *img2 = this->resizer->ProcessToNew((Media::StaticImage*)img);
				if (img2)
				{
					spotX = spotX * denv->img->GetHDPI() / img->info->hdpi;
					spotY = spotY * denv->img->GetVDPI() / img->info->vdpi;
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

		this->mapSch->SetDrawType(layer, Map::MapScheduler::MSDT_POINTS, 0, 0, dimg, spotX, spotY, &denv->isLayerEmpty);
		this->mapSch->SetDrawObjs(denv->objBounds, &denv->objCnt, maxLabel);
		session = layer->BeginGetObject();

		i = arri->GetCount();
		while (i-- > 0)
		{
			if ((dobj = layer->GetObjectByIdD(session, arri->GetItem(i))) != 0)
			{
				k = dobj->nPoint;
				while (k-- > 0)
				{
					Math::CoordinateSystem::ConvertXYZ(lyrCSys, envCSys, dobj->pointArr[(k << 1)], dobj->pointArr[(k << 1) + 1], 0, &dobj->pointArr[(k << 1)], &dobj->pointArr[(k << 1) + 1], 0);
				}
				this->mapSch->Draw(dobj);
			}
		}

		layer->EndGetObject(session);
		this->mapSch->WaitForFinish();
		this->eng->DeleteImage(dimg);
	}
	else
	{
		layer->GetObjectIdsMapXY(arri, 0, leftLon - ((rightLon - leftLon) * 0.5), topLat - ((bottomLat - topLat) * 0.5), rightLon + ((rightLon - leftLon) * 0.5), bottomLat + ((bottomLat - topLat) * 0.5), true);
		if (arri->GetCount() <= 0)
		{
			DEL_CLASS(arri);
			return;
		}

		Media::DrawImage *dimg;
		if (this->drawType == Map::DrawMapRenderer::DT_PIXELDRAW)
		{
			UInt32 newW = Math::Double2Int32(img->info->dispWidth * denv->img->GetHDPI() / img->info->hdpi);
			UInt32 newH = Math::Double2Int32(img->info->dispHeight * denv->img->GetVDPI() / img->info->vdpi);
			if (newW > img->info->dispWidth || newH > img->info->dispHeight)
			{
				this->resizer->SetTargetWidth(newW);
				this->resizer->SetTargetHeight(newH);
				Media::StaticImage *img2 = this->resizer->ProcessToNew((Media::StaticImage*)img);
				if (img2)
				{
					spotX = spotX * denv->img->GetHDPI() / img->info->hdpi;
					spotY = spotY * denv->img->GetVDPI() / img->info->vdpi;
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

		this->mapSch->SetDrawType(layer, Map::MapScheduler::MSDT_POINTS, 0, 0, dimg, spotX, spotY, &denv->isLayerEmpty);
		this->mapSch->SetDrawObjs(denv->objBounds, &denv->objCnt, maxLabel);
		session = layer->BeginGetObject();

		i = arri->GetCount();
		while (i-- > 0)
		{
			if ((dobj = layer->GetObjectByIdD(session, arri->GetItem(i))) != 0)
			{
				this->mapSch->Draw(dobj);
			}
		}

		layer->EndGetObject(session);
		this->mapSch->WaitForFinish();
		this->eng->DeleteImage(dimg);
	}
	DEL_CLASS(arri);
}

void Map::DrawMapRenderer::DrawLabel(DrawEnv *denv, Map::IMapDrawLayer *layer, UOSInt fontStyle, Int32 labelCol, Int32 priority, Int32 flags, UOSInt imgWidth, UOSInt imgHeight, Map::DrawLayerType layerType)
{
	void *arr;
	Data::ArrayListInt64 *arri;
	OSInt i;
	Int32 j;
	OSInt k;
	Map::DrawObjectL *dobj;
	Double scaleW;
	Double scaleH;
	Double pts[2];
	UTF8Char *sptr;
	UTF8Char lblStr[256];
	void *session;
	Int32 maxLabel = (Int32)denv->env->GetNString();
	Double x1;
	Double y1;
	Double x2;
	Double y2;
	Bool csysConv = false;;
	x1 = denv->view->GetLeftX();
	y1 = denv->view->GetTopY();
	x2 = denv->view->GetRightX();
	y2 = denv->view->GetBottomY();
	Math::CoordinateSystem *lyrCSys = layer->GetCoordinateSystem();
	Math::CoordinateSystem *envCSys = this->env->GetCoordinateSystem();
	if (lyrCSys != 0 && envCSys != 0 && !lyrCSys->Equals(envCSys))
	{
		Math::CoordinateSystem::ConvertXYZ(envCSys, lyrCSys, x1, y1, 0, &x1, &y1, 0);
		Math::CoordinateSystem::ConvertXYZ(envCSys, lyrCSys, x2, y2, 0, &x2, &y2, 0);
		csysConv = true;
	}

	NEW_CLASS(arri, Data::ArrayListInt64());
	layer->GetObjectIdsMapXY(arri, &arr, x1, y1, x2, y2, false);
	session = layer->BeginGetObject();
	i = arri->GetCount();
	while (i-- > 0)
	{
		if ((dobj = layer->GetObjectByIdD(session, arri->GetItem(i))) != 0)
		{
			layer->GetString(sptr = lblStr, sizeof(lblStr), arr, arri->GetItem(i), labelCol);

			if (csysConv)
			{
				k = dobj->nPoint;
				while (k-- > 0)
				{
					Math::CoordinateSystem::ConvertXYZ(lyrCSys, envCSys, dobj->pointArr[(k << 1)], dobj->pointArr[(k << 1) + 1], 0, &dobj->pointArr[(k << 1)], &dobj->pointArr[(k << 1) + 1], 0);
				}
			}
			if (flags & Map::MapEnv::SFLG_TRIM)
			{
				Text::StrTrim(lblStr);
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
				if (AddLabel(denv->labels, maxLabel, &denv->labelCnt, sptr, maxSize, &dobj->pointArr[maxPos << 1], priority, layerType, fontStyle, flags, denv->view, imgWidth, imgHeight))
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
					pts[0] = dobj->pointArr[dobj->nPoint - 1];
					pts[1] = dobj->pointArr[dobj->nPoint];

					scaleW = dobj->pointArr[dobj->nPoint + 1] - dobj->pointArr[dobj->nPoint - 3];
					scaleH = dobj->pointArr[dobj->nPoint + 2] - dobj->pointArr[dobj->nPoint - 2];
				}
				else
				{
					pts[0] = (dobj->pointArr[dobj->nPoint - 2] + dobj->pointArr[dobj->nPoint]) * 0.5;
					pts[1] = (dobj->pointArr[dobj->nPoint - 1] + dobj->pointArr[dobj->nPoint + 1]) * 0.5;

					scaleW = dobj->pointArr[dobj->nPoint] - dobj->pointArr[dobj->nPoint - 2];
					scaleH = dobj->pointArr[dobj->nPoint + 1] - dobj->pointArr[dobj->nPoint - 1];
				}

				if (denv->view->InViewXY(pts[0], pts[1]))
				{
					denv->view->MapXYToScnXY(pts[0], pts[1], &pts[0], &pts[1]);

					if ((flags & Map::MapEnv::SFLG_ROTATE) == 0)
						scaleW = scaleH = 0;
					DrawChars(denv, sptr, pts[0], pts[1], scaleW, scaleH, fontStyle, (flags & Map::MapEnv::SFLG_ALIGN) != 0);
				}
				layer->ReleaseObject(session, dobj);
			}
			else if (layerType == Map::DRAW_LAYER_POLYGON)
			{
				Math::Geometry::GetPolygonCenter(dobj->nPtOfst, dobj->nPoint, dobj->ptOfstArr, dobj->pointArr, &pts[0], &pts[1]);
				if (denv->view->InViewXY(pts[0], pts[1]))
				{
					denv->view->MapXYToScnXY(pts[0], pts[1], &pts[0], &pts[1]);
					DrawChars(denv, sptr, pts[0], pts[1], 0, 0, fontStyle, (flags & Map::MapEnv::SFLG_ALIGN) != 0);
				}
				layer->ReleaseObject(session, dobj);
			}
			else
			{
				Double lastPtX = 0;
				Double lastPtY = 0;
				Double *pointPos = dobj->pointArr;

				j = dobj->nPoint;
				while (j--)
				{
					lastPtX += *pointPos++;
					lastPtY += *pointPos++;
				}

				pts[0] = (lastPtX / dobj->nPoint);
				pts[1] = (lastPtY / dobj->nPoint);
				if (denv->view->InViewXY(pts[0], pts[1]))
				{
					denv->view->MapXYToScnXY(pts[0], pts[1], &pts[0], &pts[1]);
					DrawChars(denv, sptr, pts[0], pts[1], 0, 0, fontStyle, (flags & Map::MapEnv::SFLG_ALIGN) != 0);
				}
				layer->ReleaseObject(session, dobj);
			}
		}
	}
	layer->EndGetObject(session);
	layer->ReleaseNameArr(arr);
	DEL_CLASS(arri);
}

void Map::DrawMapRenderer::DrawImageLayer(DrawEnv *denv, Map::IMapDrawLayer *layer)
{
	Data::ArrayListInt64 *arri;
	Math::Vector2D *vec;
	Math::VectorImage *vimg;
	UOSInt i;
	UOSInt j;
	Math::CoordinateSystem *coord = layer->GetCoordinateSystem();
	Bool geoConv;
	void *sess;
	geoConv = !denv->env->GetCoordinateSystem()->Equals(coord);

	NEW_CLASS(arri, Data::ArrayListInt64());
	if (this->drawType == Map::DrawMapRenderer::DT_PIXELDRAW)
	{
		layer->SetCurrScale(denv->view->GetMapScale());
	}
	if (geoConv)
	{
		Double x1 = denv->view->GetLeftX();
		Double y1 = denv->view->GetTopY();
		Double x2 = denv->view->GetRightX();
		Double y2 = denv->view->GetBottomY();
		Math::CoordinateSystem::ConvertXYZ(denv->env->GetCoordinateSystem(), coord, x1, y1, 0, &x1, &y1, 0);
		Math::CoordinateSystem::ConvertXYZ(denv->env->GetCoordinateSystem(), coord, x2, y2, 0, &x2, &y2, 0);
		layer->GetObjectIdsMapXY(arri, 0, x1, y1, x2, y2, false);
	}
	else
	{
		layer->GetObjectIdsMapXY(arri, 0, denv->view->GetLeftX(), denv->view->GetTopY(), denv->view->GetRightX(), denv->view->GetBottomY(), false);
	}
	Data::ArrayList<Math::VectorImage *> imgList;
	sess = layer->BeginGetObject();
	i = 0;
	j = arri->GetCount();
	while (i < j)
	{
		vec = layer->GetVectorById(sess, arri->GetItem(i));
		if (vec)
		{
			if (vec->GetVectorType() == Math::Vector2D::VT_IMAGE)
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
	ArtificialQuickSort_SortCmp(arr, VImgCompare, 0, j - 1);
	i = 0;
	j = imgList.GetCount();
	while (i < j)
	{
		Double minX;
		Double minY;
		Double maxX;
		Double maxY;
		Double scnX;
		Double scnY;
		Double scnX2;
		Double scnY2;
		vimg = imgList.GetItem(i);
		if (vimg->IsScnCoord())
		{
			vimg->GetScreenBounds(denv->view->GetScnWidth(), denv->view->GetScnHeight(), denv->view->GetHDPI(), denv->view->GetHDPI(), &scnX, &scnY, &scnX2, &scnY2);
		}
		else
		{
			vimg->GetBounds(&minX, &minY, &maxX, &maxY);
			if (geoConv)
			{
				Math::CoordinateSystem::ConvertXYZ(coord, denv->env->GetCoordinateSystem(), minX, maxY, 0, &minX, &maxY, 0);
				Math::CoordinateSystem::ConvertXYZ(coord, denv->env->GetCoordinateSystem(), maxX, minY, 0, &maxX, &minY, 0);
			}
			denv->view->MapXYToScnXY(minX, maxY, &scnX, &scnY);
			denv->view->MapXYToScnXY(maxX, minY, &scnX2, &scnY2);
		}
		Int32 imgTimeMS;
		Media::StaticImage *simg = vimg->GetImage(scnX2 - scnX, scnY2 - scnY, &imgTimeMS);
		DrawImageObject(denv, simg, scnX, scnY, scnX2, scnY2, vimg->GetSrcAlpha());
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
	DEL_CLASS(arri);
}

void Map::DrawMapRenderer::DrawImageObject(DrawEnv *denv, Media::StaticImage *img, Double scnX1, Double scnY1, Double scnX2, Double scnY2, Double srcAlpha)
{
	OSInt imgW;
	OSInt imgH;
	Double cimgX2;
	Double cimgY2;
	Double cimgX;
	Double cimgY;

	imgW = denv->img->GetWidth();
	imgH = denv->img->GetHeight();

	if (img != 0 && scnX1 < scnX2 && scnY1 < scnY2)
	{
		if (this->drawType == DT_VECTORDRAW)
		{
			img->info->hdpi = img->info->dispWidth * denv->img->GetHDPI() / (scnX2 - scnX1);
			img->info->vdpi = img->info->dispHeight * denv->img->GetVDPI() / (scnY2 - scnY1);
			denv->img->DrawImagePt2(img, scnX1, scnY1);
		}
		else
		{
			Double drawW = scnX2 - scnX1;
			Double drawH = scnY2 - scnY1;
			if (imgW > drawW || imgH > drawH)
			{
				img->To32bpp();
				this->resizer->SetTargetWidth(Math::Double2Int32(scnX2) - Math::Double2Int32(scnX1));
				this->resizer->SetTargetHeight(Math::Double2Int32(scnY2) - Math::Double2Int32(scnY1));
				this->resizer->SetResizeAspectRatio(Media::IImgResizer::RAR_IGNOREAR);
				Media::StaticImage *newImg = this->resizer->ProcessToNew(img);
				if (newImg)
				{
					if (srcAlpha >= 0 && srcAlpha <= 1)
					{
						newImg->MultiplyAlpha(srcAlpha);
					}
					newImg->info->hdpi = denv->img->GetHDPI();
					newImg->info->vdpi = denv->img->GetVDPI();
					denv->img->DrawImagePt2(newImg, scnX1, scnY1);
					DEL_CLASS(newImg);
				}
			}
			else
			{
				cimgX2 = Math::OSInt2Double(img->info->dispWidth);
				cimgY2 = Math::OSInt2Double(img->info->dispHeight);
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
				if (scnX2 > imgW)
				{
					cimgX2 = cimgX + (cimgX2 - cimgX) * (imgW - scnX1) / (scnX2 - scnX1);
					scnX2 = Math::OSInt2Double(imgW);
				}
				if (scnY2 > imgH)
				{
					cimgY2 = cimgY + (cimgY2 - cimgY) * (imgH - scnY1) / (scnY2 - scnY1);
					scnY2 = Math::OSInt2Double(imgH);
				}
				if (cimgX == cimgX2)
				{
					if (cimgX2 >= img->info->dispWidth)
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
					if (cimgY2 >= img->info->dispHeight)
					{
						cimgY = cimgY2 - 1;
					}
					else
					{
						cimgY2++;
					}
				}

				this->resizer->SetTargetWidth(Math::Double2Int32(scnX2) - Math::Double2Int32(scnX1));
				this->resizer->SetTargetHeight(Math::Double2Int32(scnY2) - Math::Double2Int32(scnY1));
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

void Map::DrawMapRenderer::GetCharsSize(DrawEnv *denv, Double *size, const UTF8Char *label, UOSInt fontStyle, Double scaleW, Double scaleH)
{
	Double szTmp[2];
	denv->img->GetTextSize(denv->fontStyles[fontStyle].font, label, Text::StrCharCnt(label), szTmp);

	if (scaleH == 0)
	{
		size[0] = (szTmp[0] + (denv->fontStyles[fontStyle].buffSize << 1));
		size[1] = (szTmp[1] + (denv->fontStyles[fontStyle].buffSize << 1));

		return;
	}


	Double pt[8];

	if (scaleW < 0)
	{
		scaleW = -scaleW;
		scaleH = -scaleH;
	}
	Double degD = Math::ArcTan2(scaleH, scaleW);
	Double xPos;
	Double yPos;
	xPos = szTmp[0] + (denv->fontStyles[fontStyle].buffSize << 1);
	yPos = szTmp[1] + (denv->fontStyles[fontStyle].buffSize << 1);
	Double sVal;
	Double cVal;
	Double xs = (xPos * 0.5 * (sVal = Math::Sin(degD)));
	Double ys = (yPos * 0.5 * sVal);
	Double xc = (xPos * 0.5 * (cVal = Math::Cos(degD)));
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

void Map::DrawMapRenderer::DrawChars(DrawEnv *denv, const UTF8Char *str1, Double scnPosX, Double scnPosY, Double scaleW, Double scaleH, UOSInt fontStyle, Bool isAlign)
{
	Double size[2];
	UInt16 absH;
	OSInt lblSize;
	Map::DrawMapRenderer::DrawFontStyle *font;
	Media::DrawFont *df;
	Media::DrawBrush *db;

	if (fontStyle >= denv->fontStyleCnt)
		return;
	if (scaleW < 0)
	{
		scaleW = -scaleW;
		scaleH = -scaleH;
	}

	lblSize = Text::StrCharCnt(str1);
	font = &denv->fontStyles[fontStyle];
	df = font->font;
	db = font->fontBrush;

	if (scaleH == 0)
	{
		scaleH = 0;
	}
	denv->img->GetTextSize(df, str1, lblSize, size);

	if (scaleH == 0)
	{
		denv->img->SetTextAlign(Media::DrawEngine::DRAW_POS_TOPLEFT);

		if (font && font->buffSize > 0)
		{
			denv->img->DrawStringB(scnPosX - (size[0] * 0.5), scnPosY - (size[1] * 0.5), str1, font->font, font->buffBrush, Math::Double2Int32(font->buffSize * denv->img->GetHDPI() / 96.0));
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

	Double degD = Math::ArcTan2((Double)scaleH, (Double)scaleW);

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
		denv->img->GetTextSize(df, str1, lblSize, szThis);

		if ((szThis[0] * absH) < (szThis[1] * lblSize * scaleW))
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
				startY = scnPosY - (tmp = ((szThis[1] * lblSize) * 0.5));
				startX = scnPosX - (tmp * scaleW / scaleH);
			}
			else if (scaleH)
			{
				scaleW = -scaleW;
				scaleH = -scaleH;
				startY = scnPosY - (tmp = ((szThis[1] * lblSize) * 0.5));
				startX = scnPosX - (tmp * scaleW / scaleH);
			}
			else
			{
				startY = scnPosY - (tmp = ((szThis[1] * lblSize) * 0.5));
				startX = scnPosX;
			}
			type = 1;
		}

		OSInt cnt;
		const UTF8Char *lbl = str1;

		if (font && font->buffSize > 0)
		{
			currX = 0;
			currY = 0;

			cnt = lblSize;

			while (cnt--)
			{
				denv->img->GetTextSize(font->font, lbl, 1, szThis);

				if (type)
				{
					UTF8Char l[2];
					l[0] = lbl[0];
					l[1] = 0;
					denv->img->DrawStringB(startX + currX - (szThis[0] * 0.5), startY + currY, l, font->font, font->buffBrush, Math::Double2Int32(font->buffSize * denv->img->GetHDPI() / 96.0));

					currY += szThis[1];

					if (scaleH)
						currX = (currY * scaleW / scaleH);
				}
				else
				{
					UTF8Char l[2];
					l[0] = lbl[0];
					l[1] = 0;
					denv->img->DrawStringB(startX + currX, startY + currY, l, font->font, font->buffBrush, Math::Double2Int32(font->buffSize * denv->img->GetHDPI() / 96.0));

					currX += szThis[0];
					if (scaleW)
						currY = (currX * scaleH / scaleW);
				}
				lbl += 1;
			}
		}
		currX = 0;
		currY = 0;
		lbl = str1;

		cnt = lblSize;

		while (cnt--)
		{
			denv->img->GetTextSize(df, lbl, 1, szThis);

			if (type)
			{
				UTF8Char l[2];
				l[0] = lbl[0];
				l[1] = 0;
				denv->img->DrawString(startX + currX - (szThis[0] * 0.5), startY + currY, l, df, db);

				currY += szThis[1];

				if (scaleH)
					currX = (currY * scaleW / scaleH);
			}
			else
			{
				UTF8Char l[2];
				l[0] = lbl[0];
				l[1] = 0;
				denv->img->DrawString(startX + currX, startY + currY, l, df, db);

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

void Map::DrawMapRenderer::DrawCharsL(Map::DrawMapRenderer::DrawEnv *denv, const UTF8Char *str1, Double *mapPts, Int32 *scnPts, UOSInt nPoints, UOSInt thisPt, Double scaleN, Double scaleD, UOSInt fontStyle, Double *realBounds)
{
	UTF8Char sbuff[256];
	OSInt lblSize = Text::StrConcat(sbuff, str1) - sbuff;
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

	if (fontStyle >= denv->fontStyleCnt)
		return;
	font = &denv->fontStyles[fontStyle];
	df = font->font;
	db = font->fontBrush;

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

	denv->img->GetTextSize(df, str1, lblSize, szThis);
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
					startX = centX + (startX * Math::Sqrt(yDiff) / Math::Sqrt(xDiff));
					startY = scnPts[(j << 1) + 1] + (scnPts[(j << 1) + 3] - scnPts[(j << 1) + 1]) * (startX - scnPts[(j << 1)]) / (scnPts[(j << 1) + 2] - scnPts[(j << 1)]);
				}
				else
				{
					startY = centY + (startY * Math::Sqrt(yDiff) / Math::Sqrt(xDiff));
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
					startX = centX + (startX * Math::Sqrt(yDiff) / Math::Sqrt(xDiff));
					startY = scnPts[(j << 1) + 1] + (scnPts[(j << 1) + 3] - scnPts[(j << 1) + 1]) * (startX - scnPts[(j << 1)]) / (scnPts[(j << 1) + 2] - scnPts[(j << 1)]);
				}
				else
				{
					startY = centY + (startY * Math::Sqrt(yDiff) / Math::Sqrt(xDiff));
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

	angle = angleOfst - Math::ArcTan2((mapPts[(j << 1) + 1] - mapPts[(j << 1) + 3]), (mapPts[(j << 1) + 2] - mapPts[(j << 1) + 0]));
	angleDegree = angle * 180.0 / Math::PI;
	cosAngle = Math::Cos(angle);
	sinAngle = Math::Sin(angle);
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
				u32c = ((nextChar & 0x0f) << 12) | ((nextPos[1] & 0x3f) << 6) | (nextPos[2] & 0x3f);
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

		denv->img->GetTextSize(df, lbl, nextPos - lbl, szThis);
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
								currX = currX - (nextX * Math::Sqrt(yDiff) / Math::Sqrt(xDiff));
							}
							else
							{
								currX = currX + (nextX * Math::Sqrt(yDiff) / Math::Sqrt(xDiff));
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
								currY = currY - (nextY * Math::Sqrt(yDiff) / Math::Sqrt(xDiff));
							}
							else
							{
								currY = currY + (nextY * Math::Sqrt(yDiff) / Math::Sqrt(xDiff));
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
								currX = currX + nextX * Math::Sqrt(yDiff) / Math::Sqrt(xDiff);
							}
							else
							{
								currX = currX - nextX * Math::Sqrt(yDiff) / Math::Sqrt(xDiff);
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
								currY = currY + nextY * Math::Sqrt(yDiff) / Math::Sqrt(xDiff);
							}
							else
							{
								currY = currY - nextY * Math::Sqrt(yDiff) / Math::Sqrt(xDiff);
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
				angle = angleOfst - Math::ArcTan2((mapPts[(j << 1) + 1] - mapPts[(j << 1) + 3]), (mapPts[(j << 1) + 2] - mapPts[(j << 1) + 0]));
				angleDegree = angle * 180.0 / Math::PI;
				cosAngle = Math::Cos(angle);
				sinAngle = Math::Sin(angle);
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
			Double lsa = Math::Sin(lastAngle * Math::PI / 180.0);
			Double lca = Math::Cos(lastAngle * Math::PI / 180.0);
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
					denv->img->DrawStringRotB(currX, currY, lbl, df, font->buffBrush, lastAngle, font->buffSize);
				}
				denv->img->DrawStringRot(currX, currY, lbl, df, db, lastAngle);
			}
			else
			{
				if (font && font->buffSize > 0)
				{
					denv->img->DrawStringRotB(currX, currY, lbl, df, font->buffBrush, lastAngle, font->buffSize);
				}
				denv->img->DrawStringRot(currX, currY, lbl, df, db, lastAngle);
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
					denv->img->DrawStringRotB(currX, currY, lbl, df, font->buffBrush, angleDegree, font->buffSize);
				}
				denv->img->DrawStringRot(currX, currY, lbl, df, db, angleDegree);
			}
			else
			{
				if (font && font->buffSize > 0)
				{
					denv->img->DrawStringRotB(currX, currY, lbl, df, font->buffBrush, angleDegree, font->buffSize);
				}
				denv->img->DrawStringRot(currX, currY, lbl, df, db, angleDegree);
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

void Map::DrawMapRenderer::DrawCharsLA(DrawEnv *denv, const UTF8Char *str1, Double *mapPts, Int32 *scnPts, UOSInt nPoints, UOSInt thisPt, Double scaleN, Double scaleD, UOSInt fontStyle, Double *realBounds)
{
	UTF8Char sbuff[256];
	UOSInt lblSize = Text::StrConcat(sbuff, str1) - sbuff;
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

	if (fontStyle >= denv->fontStyleCnt)
		return;
	font = &denv->fontStyles[fontStyle];
	df = font->font;
	db = font->fontBrush;

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
		denv->img->GetTextSize(df, &str1[i], 1, szThis);
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
	OSInt startInd = j;
	denv->img->SetTextAlign(Media::DrawEngine::DRAW_POS_CENTER);

	////////////////////////////////
	UTF8Char *lbl = sbuff;
	UTF8Char *nextPos = lbl;
	UTF8Char nextChar = *lbl;
	Double angle;
	Int32 angleDegree;
	Int32 lastAngle;
	OSInt lastAInd;
	Double lastX;
	Double lastY;

	szLast[0] = 0;
	szLast[1] = 0;

	lastX = currX = startX;
	lastY = currY = startY;
	j = startInd;

	angle = angleOfst - Math::ArcTan2((mapPts[(j << 1) + 1] - mapPts[(j << 1) + 3]), (mapPts[(j << 1) + 2] - mapPts[(j << 1) + 0]));
	angleDegree = Math::Double2Int32(angle * 180.0 / Math::PI);
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

		denv->img->GetTextSize(df, lbl, nextPos - lbl, szThis);
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

				angle = angleOfst - Math::ArcTan2((mapPts[(j << 1) + 1] - mapPts[(j << 1) + 3]), (mapPts[(j << 1) + 2] - mapPts[(j << 1) + 0]));
				angleDegree = Math::Double2Int32(angle * 180.0 / Math::PI);
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
				denv->img->DrawStringB(currX, currY, lbl, df, font->buffBrush, Math::Double2Int32(font->buffSize * denv->img->GetHDPI() / 96.0));
			}
			denv->img->DrawString(currX, currY, lbl, df, db);
		}
		else
		{
			if (font && font->buffSize > 0)
			{
				denv->img->DrawStringB(currX, currY, lbl, df, font->buffBrush, Math::Double2Int32(font->buffSize * denv->img->GetHDPI() / 96.0));
			}
			denv->img->DrawString(currX, currY, lbl, df, db);
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
	NEW_CLASS(this->mapSch, Map::MapScheduler());
	NEW_CLASS(this->resizer, Media::Resizer::LanczosResizer8_C8(3, 3, &srcColor, &this->color, colorSess, Media::AT_NO_ALPHA));
	NEW_CLASS(this->colorConv, Media::ColorConv(&srcColor, &this->color, colorSess));
}

Map::DrawMapRenderer::~DrawMapRenderer()
{
	DEL_CLASS(this->resizer);
	DEL_CLASS(this->mapSch);
	DEL_CLASS(this->colorConv);
}

void Map::DrawMapRenderer::DrawMap(Media::DrawImage *img, Map::MapView *view, Int32 *imgDurMS)
{
	Map::DrawMapRenderer::DrawEnv denv;
	OSInt i;
	Map::DrawMapRenderer::DrawFontStyle *font;

/*	Media::DrawBrush *b = img->NewBrushARGB(this->env->GetBGColor());
	img->DrawRect(0, 0, view->GetScnWidth(), view->GetScnHeight(), 0, b);
	img->DelBrush(b);*/

	this->mapSch->SetMapView(view, img);

	denv.env = env;
	denv.img = img;
	denv.view = view;
	denv.isLayerEmpty = true;
	denv.objBounds = MemAlloc(Double, this->env->GetNString() << 2);
	denv.objCnt = 0;
	denv.labelCnt = 0;
	denv.labels = MemAlloc(Map::DrawMapRenderer::MapLabels, denv.maxLabels = (Int32)this->env->GetNString());
	denv.fontStyleCnt = env->GetFontStyleCount();
	denv.fontStyles = MemAlloc(Map::DrawMapRenderer::DrawFontStyle, denv.fontStyleCnt);
	denv.imgDurMS = 0;
	NEW_CLASS(denv.layerFont, Data::ArrayList<Media::DrawFont*>());
	NEW_CLASS(denv.layerFontColor, Data::ArrayList<Media::DrawBrush*>());
	i = denv.fontStyleCnt;
	while (i-- > 0)
	{
		const UTF8Char *fontName;
		Double fontSizePt;
		Bool bold;
		Int32 fontColor;
		UOSInt buffSize;
		Int32 buffColor;

		font = &denv.fontStyles[i];
		env->GetFontStyle(i, &fontName, &fontSizePt, &bold, &fontColor, &buffSize, &buffColor);
		font->font = img->NewFontPt(fontName, fontSizePt, bold?Media::DrawEngine::DFS_BOLD:Media::DrawEngine::DFS_NORMAL, 0);
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
	NEW_CLASS(denv.idArr, Data::ArrayListInt64());

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
	i = denv.layerFont->GetCount();
	while (i-- > 0)
	{
		img->DelFont(denv.layerFont->GetItem(i));
		img->DelBrush(denv.layerFontColor->GetItem(i));
	}
	DEL_CLASS(denv.layerFont);
	DEL_CLASS(denv.layerFontColor);
	MemFree(denv.fontStyles);
	DEL_CLASS(denv.idArr);
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
