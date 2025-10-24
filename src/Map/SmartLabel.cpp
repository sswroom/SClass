#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Sync/Event.h"
#include "IO/Stream.h"
#include "Map/MapView.h"
#include "Map/SmartLabel.h"
#include "Math/Math_C.h"
#include "Media/DrawEngine.h"

#ifndef _WIN32_WCE
#include <windows.h>
#endif

#define LBLMINDIST 150

typedef enum
{
	PointType,
	PolylineType,
	PolygonType
} ShapeType;

typedef struct
{
	const WChar *label;
	Int32 xPos;
	Int32 yPos;
	Int32 fontStyle;
	Int32 scaleW;
	Int32 scaleH;
	Int32 priority;
	Int32 totalSize;
	Double currSize;
	Double mapRate;
	Int32 nPoints;
	ShapeType shapeType;
	Int32 *points;
	Map::SmartLabel::LabelFlags flags;
} MapLabels;

typedef struct
{
	Int32 nLabels;
	Map::MapView *view;
	Map::SmartLabel::DrawFunc func1;
	Map::SmartLabel::SizeFunc func2;
	void *userObj;
	MapLabels *labels;
	Int32 labelCnt;
} LabelSession;

Int32 Map::SmartLabel::NewLabel(void *sess, Int32 priority)
{
	Int32 minPriority;
	Int32 i;
	Int32 j;
	Int32 k;

	LabelSession *ses = (LabelSession*)sess;
	MapLabels *labels = ses->labels;

	if (ses->labelCnt >= ses->nLabels)
	{
		i = ses->labelCnt;
		minPriority = ses->labels[0].priority;
		j = -1;
		k = ses->labels[0].totalSize;
		while (i--)
		{
			if (ses->labels[i].priority < minPriority)
			{
				minPriority = ses->labels[i].priority;
				j = i;
				k = ses->labels[i].totalSize;
			}
			else if (ses->labels[i].priority == minPriority)
			{
				if (ses->labels[i].totalSize < k)
				{
					j = i;
					k = ses->labels[i].totalSize;
				}
			}
		}
		if (j < 0)
			return j;
		if (ses->labels[j].label)
			Text::StrDelNew(ses->labels[j].label);
		if (ses->labels[j].points)
			MemFree(ses->labels[j].points);
		ses->labels[j].label = 0;
		ses->labels[j].points = 0;
		ses->labels[j].priority = priority;
		return j;
	}
	else
	{
		i = ses->labelCnt++;
		labels[i].priority = priority;
		labels[i].label = 0;
		labels[i].points = 0;
		return i;
	}
}

void Map::SmartLabel::SwapLabel(void *sess, Int32 index, Int32 index2)
{
	LabelSession *ses = (LabelSession*)sess;
	MapLabels *labels = ses->labels;
	MapLabels l;

	l.label = labels[index].label;
	l.xPos = labels[index].xPos;
	l.yPos = labels[index].yPos;
	l.fontStyle = labels[index].fontStyle;
	l.scaleW = labels[index].scaleW;
	l.scaleH = labels[index].scaleH;
	l.priority = labels[index].priority;
	l.totalSize = labels[index].totalSize;
	l.currSize = labels[index].currSize;
	l.nPoints = labels[index].nPoints;
	l.shapeType = labels[index].shapeType;
	l.points = labels[index].points;
	l.flags = labels[index].flags;
	
	labels[index].label = labels[index2].label;
	labels[index].xPos = labels[index2].xPos;
	labels[index].yPos = labels[index2].yPos;
	labels[index].fontStyle = labels[index2].fontStyle;
	labels[index].scaleW = labels[index2].scaleW;
	labels[index].scaleH = labels[index2].scaleH;
	labels[index].priority = labels[index2].priority;
	labels[index].totalSize = labels[index2].totalSize;
	labels[index].currSize = labels[index2].currSize;
	labels[index].nPoints = labels[index2].nPoints;
	labels[index].shapeType = labels[index2].shapeType;
	labels[index].points = labels[index2].points;
	labels[index].flags = labels[index2].flags;

	labels[index2].label = l.label;
	labels[index2].xPos = l.xPos;
	labels[index2].yPos = l.yPos;
	labels[index2].fontStyle = l.fontStyle;
	labels[index2].scaleW = l.scaleW;
	labels[index2].scaleH = l.scaleH;
	labels[index2].priority = l.priority;
	labels[index2].totalSize = l.totalSize;
	labels[index2].currSize = l.currSize;
	labels[index2].nPoints = l.nPoints;
	labels[index2].shapeType = l.shapeType;
	labels[index2].points = l.points;
	labels[index2].flags = l.flags;}

Int32 Map::SmartLabel::LabelOverlapped(Int32 *points, Int32 nPoints, Int32 tlx, Int32 tly, Int32 brx, Int32 bry)
{
	while (nPoints--)
	{
		if (points[(nPoints << 2) + 0] < brx && points[(nPoints << 2) + 2] > tlx && points[(nPoints << 2) + 1] < bry && points[(nPoints << 2) + 3] > tly) return 1;
	}

	return 0;
}

Map::SmartLabel::SmartLabel()
{
}

Map::SmartLabel::~SmartLabel()
{
}

void *Map::SmartLabel::BeginDraw(Int32 nLabels, MapView *view, DrawFunc func1, SizeFunc func2, void *userObj)
{
	LabelSession *ses;
	ses = MemAlloc(LabelSession, 1);
	ses->nLabels = nLabels;
	ses->view = view;
	ses->func1 = func1;
	ses->func2 = func2;
	ses->userObj = userObj;
	ses->labels = MemAlloc(MapLabels, nLabels);
	ses->labelCnt = 0;

	return ses;
}

void Map::SmartLabel::EndDraw(void *sess)
{
	LabelSession *ses = (LabelSession*)sess;
	Int32 i = ses->labelCnt;
	while (i-- > 0)
	{
/////////////////////////////
	}
	MemFree(ses->labels);
	MemFree(ses);
}

void Map::SmartLabel::DrawImage(void *sess, Media::DrawImage *img)
{
	LabelSession *ses = (LabelSession*)sess;
	Int32 i;
	Int32 j;
	WChar *dest;
	WChar *lastLbl = 0;
	Double leftLon = ses->view->GetLeftX();
	Double topLat = ses->view->GetTopY();
	Double rightLon = ses->view->GetRightX();
	Double bottomLat = ses->view->GetBottomY();
	Int32 scnWidth = img->GetWidth();
	Int32 scnHeight = img->GetHeight();

	if (ses->labelCnt)
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

		if (!(points = MemAlloc(Int32, ses->labelCnt * 20)))
			return;

		i = 0;
		j = ses->labelCnt;
		while (j--)
		{
			currPt = 0;
			i = 0;
			while (i < j)
			{
				if (ses->labels[i].priority < ses->labels[i + 1].priority)
				{
					SwapLabel(ses, i, i + 1);
					currPt = 1;
				}
				else if (ses->labels[i].priority == ses->labels[i + 1].priority)
				{
					if (ses->labels[i].totalSize < ses->labels[i + 1].totalSize)
					{
						SwapLabel(ses, i, i + 1);
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
		while (i < ses->labelCnt)
		{
			Double scnXD;
			Double scnYD;
//			GetCharsSize(img, szThis, ses->labels[i].label, fonts[ses->labels[i].fontStyle], ses->labels[i].scaleW, ses->labels[i].scaleH);
			ses->func2(ses->userObj, ses->labels[i].label, ses->labels[i].scaleW, ses->labels[i].scaleH, ses->labels[i].fontStyle, &szThis[0], &szThis[1], ses->labels[i].flags);
			ses->view->MapXYToScnXY(ses->labels[i].xPos / ses->labels[i].mapRate, ses->labels[i].yPos / ses->labels[i].mapRate, &scnXD, &scnYD);
			scnPtX = Double2Int32(scnXD);
			scnPtY = Double2Int32(scnYD);

			
		//	labels[i].shapeType = 0;
			if (ses->labels[i].shapeType == PointType)
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
					ses->func1(ses->userObj, ses->labels[i].label, (tlx + brx) >> 1, (tly + bry) >> 1, 0, 0, ses->labels[i].fontStyle, ses->labels[i].flags);
//					DrawChars(img, ses->labels[i].label, (tlx + brx) >> 1, (tly + bry) >> 1, 0, 0, fonts[ses->labels[i].fontStyle], 0);

					points[(currPt << 2)] = tlx;
					points[(currPt << 2) + 1] = tly;
					points[(currPt << 2) + 2] = brx;
					points[(currPt << 2) + 3] = bry;
					currPt++;
				}
			}
			else if (ses->labels[i].shapeType == PolylineType)
			{

				if (lastLbl)
				{
					src = ses->labels[i].label;
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
					src = ses->labels[i].label;
					while (*src++);
					dest = lastLbl = MemAlloc(WChar, src - ses->labels[i].label);
					src = ses->labels[i].label;
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
						ses->func1(ses->userObj, ses->labels[i].label, (tlx + brx) >> 1, (tly + bry) >> 1, ses->labels[i].scaleW, ses->labels[i].scaleH, ses->labels[i].fontStyle, ses->labels[i].flags);
						//DrawChars(img, ses->labels[i].label, (tlx + brx) >> 1, (tly + bry) >> 1, ses->labels[i].scaleW, ses->labels[i].scaleH, fonts[ses->labels[i].fontStyle], (ses->labels[i].flags & LF_ALIGN) != 0);

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
					Int32 *ptInt = ses->labels[i].points;

					k = 0;
					l = ses->labels[i].nPoints - 1;
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

						if (leftLon * ses->labels[i].mapRate < thisPtX && rightLon * ses->labels[i].mapRate > lastPtX && topLat * ses->labels[i].mapRate < thisPtY && bottomLat * ses->labels[i].mapRate > lastPtY)
						{
							lastPtX = ptInt[0];
							lastPtY = ptInt[1];
							thisPtX = ptInt[2];
							thisPtY = ptInt[3];
							if (leftLon * ses->labels[i].mapRate > lastPtX)
							{
								lastPtY += MulDiv(Double2Int32(leftLon * ses->labels[i].mapRate - lastPtX), thisPtY - lastPtY, thisPtX - lastPtX);
								lastPtX = Double2Int32(leftLon * ses->labels[i].mapRate);
							}
							else if (lastPtX > rightLon * ses->labels[i].mapRate)
							{
								lastPtY += MulDiv(Double2Int32(rightLon * ses->labels[i].mapRate - lastPtX), thisPtY - lastPtY, thisPtX - lastPtX);
								lastPtX = Double2Int32(rightLon * ses->labels[i].mapRate);
							}

							if (topLat * ses->labels[i].mapRate > lastPtY)
							{
								lastPtX += MulDiv(Double2Int32(topLat * ses->labels[i].mapRate - lastPtY), thisPtX - lastPtX, thisPtY - lastPtY);
								lastPtY = Double2Int32(topLat * ses->labels[i].mapRate);
							}
							else if (lastPtY > bottomLat * ses->labels[i].mapRate)
							{
								lastPtX += MulDiv(Double2Int32(bottomLat * ses->labels[i].mapRate - lastPtY), thisPtX - lastPtX, thisPtY - lastPtY);
								lastPtY = Double2Int32(bottomLat * ses->labels[i].mapRate);
							}

							if (thisPtX < leftLon * ses->labels[i].mapRate)
							{
								thisPtY += MulDiv(Double2Int32(leftLon * ses->labels[i].mapRate - thisPtX), thisPtY - lastPtY, thisPtX - lastPtX);
								thisPtX = Double2Int32(leftLon * ses->labels[i].mapRate);
							}
							else if (rightLon * ses->labels[i].mapRate < thisPtX)
							{
								thisPtY += MulDiv(Double2Int32(rightLon * ses->labels[i].mapRate - thisPtX), thisPtY - lastPtY, thisPtX - lastPtX);
								thisPtX = Double2Int32(rightLon * ses->labels[i].mapRate);
							}

							if (topLat * ses->labels[i].mapRate > thisPtY)
							{
								thisPtX += MulDiv(Double2Int32(topLat * ses->labels[i].mapRate - thisPtY), thisPtX - lastPtX, thisPtY - lastPtY);
								thisPtY = Double2Int32(topLat * ses->labels[i].mapRate);
							}
							else if (bottomLat * ses->labels[i].mapRate < thisPtY)
							{
								thisPtX += MulDiv(Double2Int32(bottomLat * ses->labels[i].mapRate - thisPtY), thisPtX - lastPtX, thisPtY - lastPtY);
								thisPtY = Double2Int32(bottomLat * ses->labels[i].mapRate);
							}

							ses->labels[i].scaleW = 0;
							ses->labels[i].scaleH = 0;

							scnPtX = (thisPtX + lastPtX) >> 1;
							scnPtY = (thisPtY + lastPtY) >> 1;
							if (ses->labels[i].flags & LF_ROTATE)
							{
								ses->labels[i].scaleW = ptInt[2] - ptInt[0];
								ses->labels[i].scaleH = ptInt[3] - ptInt[1];
							}

							ses->view->MapXYToScnXY(scnPtX / ses->labels[i].mapRate, scnPtY / ses->labels[i].mapRate, &scnXD, &scnYD);
							scnPtX = Double2Int32(scnXD);
							scnPtY = Double2Int32(scnYD);

//							GetCharsSize(img, szThis, ses->labels[i].label, fonts[ses->labels[i].fontStyle], ses->labels[i].scaleW, ses->labels[i].scaleH);
							ses->func2(ses->userObj, ses->labels[i].label, ses->labels[i].scaleW, ses->labels[i].scaleH, ses->labels[i].fontStyle, &szThis[0], &szThis[1], ses->labels[i].flags);

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
								ses->func1(ses->userObj, ses->labels[i].label, (tlx + brx) >> 1, (tly + bry) >> 1, ses->labels[i].scaleW, ses->labels[i].scaleH, ses->labels[i].fontStyle, ses->labels[i].flags);
								//DrawChars(img, ses->labels[i].label, (tlx + brx) >> 1, (tly + bry) >> 1, ses->labels[i].scaleW, ses->labels[i].scaleH, fonts[ses->labels[i].fontStyle], (ses->labels[i].flags & LF_ALIGN) != 0);

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
					ses->func1(ses->userObj, ses->labels[i].label, (tlx + brx) >> 1, (tly + bry) >> 1, ses->labels[i].scaleW, ses->labels[i].scaleH, ses->labels[i].fontStyle, ses->labels[i].flags);
//					DrawChars(img, ses->labels[i].label,  (tlx + brx) >> 1, (tly + bry) >> 1, ses->labels[i].scaleW, ses->labels[i].scaleH, fonts[ses->labels[i].fontStyle], (ses->labels[i].flags & LF_ALIGN) != 0);

					points[(currPt << 2)] = tlx;
					points[(currPt << 2) + 1] = tly;
					points[(currPt << 2) + 2] = brx;
					points[(currPt << 2) + 3] = bry;
					currPt++;
				}
			}
			else if (ses->labels[i].shapeType == PolygonType)
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
					ses->func1(ses->userObj, ses->labels[i].label, (tlx + brx) >> 1, (tly + bry) >> 1, 0, 0, ses->labels[i].fontStyle, ses->labels[i].flags);
//					DrawChars(img, ses->labels[i].label, (tlx + brx) >> 1, (tly + bry) >> 1, 0, 0, fonts[ses->labels[i].fontStyle], 0);

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

//	printf("Free labels\n");
	i = ses->labelCnt;
	while (i-- > 0)
	{
		Text::StrDelNew(ses->labels[i].label);
		if (ses->labels[i].points)
			MemFree(ses->labels[i].points);
	}
	if (lastLbl)
		MemFree(lastLbl);
}

Bool Map::SmartLabel::AddPointLabel(void *sess, Int32 priority, Int32 fontStyle, WChar *label, Int32 *points, Int32 nPoints, LabelFlags flags, Double mapRate)
{
	LabelSession *ses = (LabelSession*)sess;
	Int32 found;
	Int32 i;
	Int32 j;
	Int32 *pInt;
	Double scnX;
	Double scnY;
	Double mapPosLon = ses->view->GetCenterX();
	Double mapPosLat = ses->view->GetCenterY();

	found = 0;
	i = 0;
	while (i < ses->labelCnt)
	{
		if (ses->labels[i].shapeType == PointType)
		{
			if (Text::StrEquals(ses->labels[i].label, label))
			{
				found = 1;

				pInt = points;
				j = nPoints;
				while (j--)
				{
					scnX = mapPosLon - *pInt++ / mapRate;
					scnY = mapPosLat - *pInt++ / mapRate;
					scnX = scnX * scnX + scnY * scnY;
					if (scnX < ses->labels[i].currSize)
					{
						ses->labels[i].xPos = pInt[-2];
						ses->labels[i].yPos = pInt[-1];
						ses->labels[i].fontStyle = fontStyle;
						ses->labels[i].scaleW = 0;
						ses->labels[i].scaleH = 0;
						ses->labels[i].currSize = scnX;
						ses->labels[i].mapRate = mapRate;
						if (priority > ses->labels[i].priority)
							ses->labels[i].priority = priority;
					}
					ses->labels[i].totalSize++;
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
		j = nPoints;
		while (j--)
		{
			if (ses->view->InViewXY(pInt[1] / mapRate, pInt[0] / mapRate))
			{
				found = 1;

				i = NewLabel(ses, priority);
				if (i < 0)
				{
					return false;
				}

				ses->labels[i].label = Text::StrCopyNew(label);
				ses->labels[i].xPos = pInt[0];
				ses->labels[i].yPos = pInt[1];
				ses->labels[i].fontStyle = fontStyle;
				ses->labels[i].scaleW = 0;
				ses->labels[i].scaleH = 0;
				ses->labels[i].priority = priority;

				scnX = mapPosLon - pInt[0] / mapRate;
				scnY = mapPosLat - pInt[1] / mapRate;
				scnX = scnX * scnX + scnY * scnY;

				ses->labels[i].currSize = scnX;
				ses->labels[i].mapRate = mapRate;
				ses->labels[i].totalSize = nPoints;
				ses->labels[i].nPoints = 0;
				ses->labels[i].shapeType = PointType;
				ses->labels[i].points = 0;
				ses->labels[i].flags = (Map::SmartLabel::LabelFlags)0;

				pInt = points;
				j = nPoints;
				while (j--)
				{
					scnX = mapPosLon - *pInt++ / mapRate;
					scnY = mapPosLat - *pInt++ / mapRate;
					scnX = scnX * scnX + scnY * scnY;
					if (scnX < ses->labels[i].currSize)
					{
						ses->labels[i].xPos = pInt[-2];
						ses->labels[i].yPos = pInt[-1];
						ses->labels[i].fontStyle = fontStyle;
						ses->labels[i].scaleW = 0;
						ses->labels[i].scaleH = 0;
						ses->labels[i].currSize = scnX;
						if (priority > ses->labels[i].priority)
							ses->labels[i].priority = priority;
					}
				}

				return true;
			}
			pInt += 2;
		}
	}
	return false;
}

Bool Map::SmartLabel::AddPolylineLabel(void *sess, Int32 priority, Int32 fontStyle, WChar *label, Int32 *points, Int32 nPoints, LabelFlags flags, Double mapRate)
{
	LabelSession *ses = (LabelSession*)sess;
	Int32 tmp;
	Int32 lastPtX;
	Int32 lastPtY = points[1];
	Int32 thisPtX;
	Int32 thisPtY;

	Int32 toUpdate;
	Int32 i;
	Int32 j;

	Int32 *pInt;
	Int32 visibleSize = 0;
	Int32 size = 0;

	Int32 left = Double2Int32(ses->view->GetLeftX() * mapRate);
	Int32 top = Double2Int32(ses->view->GetTopY() * mapRate);
	Int32 right = Double2Int32(ses->view->GetRightX() * mapRate);
	Int32 bottom = Double2Int32(ses->view->GetBottomY() * mapRate);

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
	Int32 found = 0;
	Int32 totalSize = 0;
	Int32 foundInd;
	i = 0;

	while (i < ses->labelCnt)
	{
		if (ses->labels[i].shapeType == PolylineType)
		{
			if (Text::StrEquals(ses->labels[i].label, label))
			{
				found++;

				if (totalSize == 0)
				{
					ses->labels[i].totalSize += size;
					totalSize = ses->labels[i].totalSize;
				}
				else
				{
					ses->labels[i].totalSize = totalSize;
				}


				if (priority > ses->labels[i].priority)
					ses->labels[i].priority = priority;
				else
					priority = ses->labels[i].priority;

				if (ses->labels[i].currSize < visibleSize)
				{
					toUpdate = 1;
				}

				if (found >= 2)
				{
				}
				else if (ses->labels[i].points[0] == points[(nPoints << 1) - 2] && ses->labels[i].points[1] == points[(nPoints << 1) - 1])
				{
					Int32 newSize = ses->labels[i].nPoints + nPoints - 1;
					Int32* newArr = MemAlloc(Int32, newSize << 1);
					Int32 k;
					Int32 l;
					l = 0;
					k = 0;
					while (k < ses->labels[i].nPoints)
					{
						newArr[l++] = ses->labels[i].points[k << 1];
						newArr[l++] = ses->labels[i].points[(k << 1) + 1];
						k++;
					}
					k = 1;
					while (k < nPoints)
					{
						newArr[l++] = points[k << 1];
						newArr[l++] = points[(k << 1) + 1];
						k++;
					}
					MemFree(ses->labels[i].points);
					ses->labels[i].points = newArr;
					ses->labels[i].nPoints = newSize;
					toUpdate = 1;
					foundInd = i;
				}
				else if (ses->labels[i].points[(ses->labels[i].nPoints << 1) - 2] == points[0] && ses->labels[i].points[(ses->labels[i].nPoints << 1) - 1] == points[1])
				{
					Int32 newSize = ses->labels[i].nPoints + nPoints - 1;
					Int32* newArr = MemAlloc(Int32, newSize << 1);
					Int32 k;
					Int32 l;
					l = 0;
					k = 0;
					while (k < nPoints)
					{
						newArr[l++] = points[k << 1];
						newArr[l++] = points[(k << 1) + 1];
						k++;
					}
					k = 1;
					while (k < ses->labels[i].nPoints)
					{
						newArr[l++] = ses->labels[i].points[k << 1];
						newArr[l++] = ses->labels[i].points[(k << 1) + 1];
						k++;
					}
					MemFree(ses->labels[i].points);
					ses->labels[i].points = newArr;
					ses->labels[i].nPoints = newSize;
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
		i = NewLabel(ses, priority);

		if (i < 0)
			return false;

		ses->labels[i].shapeType = PolylineType;
		ses->labels[i].fontStyle = fontStyle;
		ses->labels[i].totalSize = size;
		ses->labels[i].currSize = visibleSize;
		ses->labels[i].mapRate = mapRate;

		ses->labels[i].label = Text::StrCopyNew(label);
		ses->labels[i].points = 0;

		toUpdate = 1;
	}
	else if (toUpdate)
	{
	}

	if (toUpdate)
	{
		/////////////////////////////////////////////////////////////////////////////////
		j = ses->labels[i].nPoints = nPoints;
		if (ses->labels[i].points)
			MemFree(ses->labels[i].points);
		ses->labels[i].points = pInt = MemAlloc(Int32, nPoints * 2);
		ses->labels[i].mapRate = mapRate;
		j = j << 1;
		long k = 0;
		while (k < j)
			*pInt++ = points[k++];

		visibleSize = visibleSize >> 1;

		j = 1;
		while (j < nPoints)
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
						if (flags & LF_ROTATE)
						{
							ses->labels[i].scaleW = points[(j << 1)] - points[(j << 1) - 2];
							ses->labels[i].scaleH = points[(j << 1) + 1] - points[(j << 1) - 1];
						}
						else
						{
							ses->labels[i].scaleW = 0;
							ses->labels[i].scaleH = 0;
						}

						ses->labels[i].flags = flags;

						if (points[(j << 1) + 1] > points[(j << 1) - 1])
						{
							thisPtY = lastPtY + visibleSize;
						}
						else
						{
							thisPtY = lastPtY - visibleSize;
						}
						thisPtX = points[(j << 1) - 2] + MulDiv(thisPtY - points[(j << 1) - 1], points[(j << 1)] - points[(j << 1) - 2], points[(j << 1) + 1] - points[(j << 1) - 1]);

						ses->labels[i].xPos = thisPtX;
						ses->labels[i].yPos = thisPtY;
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
						if (flags & LF_ROTATE)
						{
							ses->labels[i].scaleW = points[(j << 1)] - points[(j << 1) - 2];
							ses->labels[i].scaleH = points[(j << 1) + 1] - points[(j << 1) - 1];
						}
						else
						{
							ses->labels[i].scaleW = 0;
							ses->labels[i].scaleH = 0;
						}

						ses->labels[i].flags = flags;

						if (points[(j << 1)] > points[(j << 1) - 2])
						{
							thisPtX = lastPtX + visibleSize;
						}
						else
						{
							thisPtX = lastPtX - visibleSize;
						}
						thisPtY = points[(j << 1) + 1] + MulDiv(thisPtX - points[(j << 1)], points[(j << 1) + 1] - points[(j << 1) - 1], points[(j << 1)] - points[(j << 1) - 2]);

						ses->labels[i].xPos = thisPtX;
						ses->labels[i].yPos = thisPtY;
						visibleSize = -1;
						break;
					}
				}
			}
			j++;
		}
		if (visibleSize > 0)
		{
			j = nPoints - 1;
			if (flags & LF_ROTATE)
			{
				ses->labels[i].scaleW = points[(j << 1)] - points[(j << 1) - 2];
				ses->labels[i].scaleH = points[(j << 1) + 1] - points[(j << 1) - 1];
			}
			else
			{
				ses->labels[i].scaleW = 0;
				ses->labels[i].scaleH = 0;
			}

			ses->labels[i].flags = flags;

			if (points[(j << 1)] > points[(j << 1) - 2])
			{
				thisPtX = lastPtX + visibleSize;
			}
			else
			{
				thisPtX = lastPtX - visibleSize;
			}
			thisPtY = points[(j << 1) + 1] + MulDiv(thisPtX - points[(j << 1)], points[(j << 1) + 1] - points[(j << 1) - 1], points[(j << 1)] - points[(j << 1) - 2]);

			ses->labels[i].xPos = thisPtX;
			ses->labels[i].yPos = thisPtY;
			visibleSize = -1;
		}
		return true;
	}
	return false;
}

Bool Map::SmartLabel::AddPolygonLabel(void *sess, Int32 priority, Int32 fontStyle, WChar *label, Int32 *points, Int32 nPoints, LabelFlags flags, Double mapRate)
{
	Int32 found;
	Int32 i;

	LabelSession *ses = (LabelSession*)sess;

	Int32 left = Double2Int32(ses->view->GetLeftX() * mapRate);
	Int32 top = Double2Int32(ses->view->GetTopY() * mapRate);
	Int32 right = Double2Int32(ses->view->GetRightX() * mapRate);
	Int32 bottom = Double2Int32(ses->view->GetBottomY() * mapRate);

	if (nPoints <= 2)
		return false;

	found = 0;
	i = 0;
	while (i < ses->labelCnt)
	{
		if (PolygonType == ses->labels[i].shapeType)
		{
			if (Text::StrEquals(ses->labels[i].label, label))
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
		outPts = MemAlloc(Int32, nPoints << 2);
		outPtCnt = 0;
		startIndex = -1;
		i = 0;
		while (i < nPoints)
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
		i = (startIndex + 1) % nPoints;
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

			i = (i + 1) % nPoints;
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

			i = NewLabel(ses, priority);
			if (i < 0)
			{
				MemFree(outPts);
				return false;
			}

			ses->labels[i].label = Text::StrCopyNew(label);
			ses->labels[i].xPos = thisX;
			ses->labels[i].yPos = thisY;
			ses->labels[i].fontStyle = fontStyle;
			ses->labels[i].scaleW = 0;
			ses->labels[i].scaleH = 0;
			ses->labels[i].priority = priority;

			ses->labels[i].currSize = outPtCnt;
			ses->labels[i].totalSize = outPtCnt;
			ses->labels[i].mapRate = mapRate;
			ses->labels[i].nPoints = outPtCnt;
			ses->labels[i].shapeType = PolygonType;
			if (ses->labels[i].points)
				MemFree(ses->labels[i].points);
			ses->labels[i].points = outPts;
			ses->labels[i].flags = flags;
			return true;
		}
		else
		{
			MemFree(outPts);
		}
	}
	return false;
}
