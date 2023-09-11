#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListInt32.h"
#include "Data/ArrayListDbl.h"
#include "Data/IComparable.h"
#include "Math/CoordinateSystemManager.h"
#include "Math/GeometryTool.h"
#include "Math/Math.h"

void Math::GeometryTool::RotateACW(Int32 *ptOut, Int32 *ptIn, UOSInt nPoint, Double centX, Double centY, Double angleRad)
{
	Double sd = Math_Sin(angleRad);
	Double cd = Math_Cos(angleRad);

	while (nPoint-- > 0)
	{
		Double x = ptIn[0] - centX;
		Double y = ptIn[1] - centY;
		ptOut[0] = Double2Int32(centX + x * cd + y * sd);
		ptOut[1] = Double2Int32(centY - x * sd + y * cd);
		ptOut += 2;
		ptIn += 2;
	}
}

void Math::GeometryTool::RotateACW(Double *ptOut, Double *ptIn, UOSInt nPoint, Double centX, Double centY, Double angleRad)
{
	Double sd = Math_Sin(angleRad);
	Double cd = Math_Cos(angleRad);

	while (nPoint-- > 0)
	{
		Double x = ptIn[0] - centX;
		Double y = ptIn[1] - centY;
		ptOut[0] = centX + x * cd + y * sd;
		ptOut[1] = centY - x * sd + y * cd;
		ptOut += 2;
		ptIn += 2;
	}
}

UOSInt Math::GeometryTool::BoundPolygonY(const Int32 *points, UOSInt nPoints, Int32 *pointOut, OSInt minY, OSInt maxY, OSInt ofstX, OSInt ofstY)
{
	Int32 *pointsCurr = pointOut;
	OSInt lastX;
	OSInt lastY;
	OSInt thisX;
	OSInt thisY;
	OSInt yStat;
	UOSInt i;

	lastX = points[(nPoints << 1) - 2];
	lastY = points[(nPoints << 1) - 1];

	if (lastY <= minY)
	{
		yStat = -1;
	}
	else if (lastY >= maxY)
	{
		yStat = 1;
	}
	else
	{
		yStat = 0;
	}

	i = nPoints;
	while (i-- > 0)
	{
		thisX = points[0];
		thisY = points[1];
		points += 2;

		if (thisY <= minY)
		{
			if (yStat == 0)
			{
				pointsCurr[0] = (Int32)(ofstX + lastX + MulDivOS(thisX - lastX, minY - lastY, thisY - lastY));
				pointsCurr[1] = (Int32)(ofstY + minY);
				pointsCurr += 2;

				lastX = thisX;
				lastY = thisY;
			}
			else if (yStat == -1)
			{
				lastX = thisX;
				lastY = thisY;
			}
			else
			{
				pointsCurr[0] = (Int32)(ofstX + lastX + MulDivOS(thisX - lastX, maxY - lastY, thisY - lastY));
				pointsCurr[1] = (Int32)(ofstY + maxY);
				pointsCurr[2] = (Int32)(ofstX + lastX + MulDivOS(thisX - lastX, minY - lastY, thisY - lastY));
				pointsCurr[3] = (Int32)(ofstY + minY);
				pointsCurr += 4;

				lastX = thisX;
				lastY = thisY;
			}

			yStat = -1;
		}
		else if (thisY >= maxY)
		{
			if (yStat == 0)
			{
				pointsCurr[0] = (Int32)(ofstX + lastX + MulDivOS(thisX - lastX, maxY - lastY, thisY - lastY));
				pointsCurr[1] = (Int32)(ofstY + maxY);
				pointsCurr += 2;

				lastX = thisX;
				lastY = thisY;
			}
			else if (yStat == -1)
			{
				pointsCurr[0] = (Int32)(ofstX + lastX + MulDivOS(thisX - lastX, minY - lastY, thisY - lastY));
				pointsCurr[1] = (Int32)(ofstY + minY);
				pointsCurr[2] = (Int32)(ofstX + lastX + MulDivOS(thisX - lastX, maxY - lastY, thisY - lastY));
				pointsCurr[3] = (Int32)(ofstY + maxY);
				pointsCurr += 4;

				lastX = thisX;
				lastY = thisY;
			}
			else
			{
				lastX = thisX;
				lastY = thisY;
			}

			yStat = 1;
		}
		else
		{
			if (yStat == 0)
			{
				if (thisX != lastX || thisY != lastY)
				{
					pointsCurr[0] = (Int32)(ofstX + (lastX = thisX));
					pointsCurr[1] = (Int32)(ofstY + (lastY = thisY));
					pointsCurr += 2;
				}
			}
			else if (yStat == -1)
			{
				pointsCurr[0] = (Int32)(ofstX + lastX + MulDivOS(thisX - lastX, minY - lastY, thisY - lastY));
				pointsCurr[1] = (Int32)(ofstY + minY);
				pointsCurr[2] = (Int32)(ofstX + thisX);
				pointsCurr[3] = (Int32)(ofstY + thisY);
				pointsCurr += 4;

				lastX = thisX;
				lastY = thisY;
			}
			else
			{
				pointsCurr[0] = (Int32)(ofstX + lastX + MulDivOS(thisX - lastX, maxY - lastY, thisY - lastY));
				pointsCurr[1] = (Int32)(ofstY + maxY);
				pointsCurr[2] = (Int32)(ofstX + thisX);
				pointsCurr[3] = (Int32)(ofstY + thisY);
				pointsCurr += 4;

				lastX = thisX;
				lastY = thisY;
			}
			yStat = 0;
		}
	}

	return (UOSInt)((pointsCurr - pointOut) >> 1);
}

UOSInt Math::GeometryTool::BoundPolygonX(const Int32 *points, UOSInt nPoints, Int32 *pointOut, OSInt minX, OSInt maxX, OSInt ofstX, OSInt ofstY)
{
	Int32 *pointsCurr = pointOut;
	Int32 lastX;
	Int32 lastY;
	Int32 thisX;
	Int32 thisY;
	Int32 xStat;
	UOSInt i;

	lastX = points[(nPoints << 1) - 2];
	lastY = points[(nPoints << 1) - 1];

	if (lastX <= minX)
	{
		xStat = -1;
	}
	else if (lastX >= maxX)
	{
		xStat = 1;
	}
	else
	{
		xStat = 0;
	}

	i = nPoints;
	while (i-- > 0)
	{
		thisX = points[0];
		thisY = points[1];
		points += 2;

		if (thisX <= minX)
		{
			if (xStat == 0)
			{
				pointsCurr[0] = (Int32)(ofstX + minX);
				pointsCurr[1] = (Int32)(ofstY + lastY + MulDivOS(thisY - lastY, minX - lastX, thisX - lastX));
				pointsCurr += 2;

				lastX = thisX;
				lastY = thisY;
			}
			else if (xStat == -1)
			{
				lastX = thisX;
				lastY = thisY;
			}
			else
			{
				pointsCurr[0] = (Int32)(ofstX + maxX);
				pointsCurr[1] = (Int32)(ofstY + lastY + MulDivOS(thisY - lastY, maxX - lastX, thisX - lastX));
				pointsCurr[2] = (Int32)(ofstX + minX);
				pointsCurr[3] = (Int32)(ofstY + lastY + MulDivOS(thisY - lastY, minX - lastX, thisX - lastX));
				pointsCurr += 4;

				lastX = thisX;
				lastY = thisY;
			}

			xStat = -1;
		}
		else if (thisX >= maxX)
		{
			if (xStat == 0)
			{
				pointsCurr[0] = (Int32)(ofstX + maxX);
				pointsCurr[1] = (Int32)(ofstY + lastY + MulDivOS(thisY - lastY, maxX - lastX, thisX - lastX));
				pointsCurr += 2;

				lastX = thisX;
				lastY = thisY;
			}
			else if (xStat == -1)
			{
				pointsCurr[0] = (Int32)(ofstX + minX);
				pointsCurr[1] = (Int32)(ofstY + lastY + MulDivOS(thisY - lastY, minX - lastX, thisX - lastX));
				pointsCurr[2] = (Int32)(ofstX + maxX);
				pointsCurr[3] = (Int32)(ofstY + lastY + MulDivOS(thisY - lastY, maxX - lastX, thisX - lastX));
				pointsCurr += 4;

				lastX = thisX;
				lastY = thisY;
			}
			else
			{
				lastX = thisX;
				lastY = thisY;
			}

			xStat = 1;
		}
		else
		{
			if (xStat == 0)
			{
				if (thisX != lastX || thisY != lastY)
				{
					pointsCurr[0] = (Int32)(ofstX + (lastX = thisX));
					pointsCurr[1] = (Int32)(ofstY + (lastY = thisY));
					pointsCurr += 2;
				}
			}
			else if (xStat == -1)
			{
				pointsCurr[0] = (Int32)(ofstX + minX);
				pointsCurr[1] = (Int32)(ofstY + lastY + MulDivOS(thisY - lastY, minX - lastX, thisX - lastX));
				pointsCurr[2] = (Int32)(ofstX + thisX);
				pointsCurr[3] = (Int32)(ofstY + thisY);
				pointsCurr += 4;

				lastX = thisX;
				lastY = thisY;
			}
			else
			{
				pointsCurr[0] = (Int32)(ofstX + maxX);
				pointsCurr[1] = (Int32)(ofstY + lastY + MulDivOS(thisY - lastY, maxX - lastX, thisX - lastX));
				pointsCurr[2] = (Int32)(ofstX + thisX);
				pointsCurr[3] = (Int32)(ofstY + thisY);
				pointsCurr += 4;

				lastX = thisX;
				lastY = thisY;
			}
			xStat = 0;
		}
	}

	return (UOSInt)((pointsCurr - pointOut) >> 1);
}

UOSInt Math::GeometryTool::BoundPolygonY(const Math::Coord2DDbl *points, UOSInt nPoints, Math::Coord2DDbl *pointOut, Double minY, Double maxY, Math::Coord2DDbl ofst)
{
	Math::Coord2DDbl *pointsCurr = pointOut;
	Math::Coord2DDbl lastPt;
	Math::Coord2DDbl thisPt;
	OSInt yStat;
	UOSInt i;

	lastPt = points[nPoints - 1];

	if (lastPt.y <= minY)
	{
		yStat = -1;
	}
	else if (lastPt.y >= maxY)
	{
		yStat = 1;
	}
	else
	{
		yStat = 0;
	}

	i = nPoints;
	while (i-- > 0)
	{
		thisPt = points[0];
		points += 1;

		if (thisPt.y <= minY)
		{
			if (yStat == 0)
			{
				pointsCurr[0].x = (ofst.x + lastPt.x + (thisPt.x - lastPt.x) * (minY - lastPt.y) / (thisPt.y - lastPt.y));
				pointsCurr[0].y = (ofst.y + minY);
				pointsCurr += 1;

				lastPt = thisPt;
			}
			else if (yStat == -1)
			{
				lastPt = thisPt;
			}
			else
			{
				pointsCurr[0].x = (ofst.x + lastPt.x + (thisPt.x - lastPt.x) * (maxY - lastPt.y) / (thisPt.y - lastPt.y));
				pointsCurr[0].y = (ofst.y + maxY);
				pointsCurr[1].x = (ofst.x + lastPt.x + (thisPt.x - lastPt.x) * (minY - lastPt.y) / (thisPt.y - lastPt.y));
				pointsCurr[1].y = (ofst.y + minY);
				pointsCurr += 2;

				lastPt = thisPt;
			}

			yStat = -1;
		}
		else if (thisPt.y >= maxY)
		{
			if (yStat == 0)
			{
				pointsCurr[0].x = (ofst.x + lastPt.x + (thisPt.x - lastPt.x) * (maxY - lastPt.y) / (thisPt.y - lastPt.y));
				pointsCurr[0].y = (ofst.y + maxY);
				pointsCurr += 1;

				lastPt = thisPt;
			}
			else if (yStat == -1)
			{
				pointsCurr[0].x = (ofst.x + lastPt.x + (thisPt.x - lastPt.x) * (minY - lastPt.y) / (thisPt.y - lastPt.y));
				pointsCurr[0].y = (ofst.y + minY);
				pointsCurr[1].x = (ofst.x + lastPt.x + (thisPt.x - lastPt.x) * (maxY - lastPt.y) / (thisPt.y - lastPt.y));
				pointsCurr[1].y = (ofst.y + maxY);
				pointsCurr += 2;

				lastPt = thisPt;
			}
			else
			{
				lastPt = thisPt;
			}

			yStat = 1;
		}
		else
		{
			if (yStat == 0)
			{
				if (thisPt != lastPt)
				{
					pointsCurr[0] = ofst + thisPt;
					pointsCurr += 1;
					lastPt = thisPt;
				}
			}
			else if (yStat == -1)
			{
				pointsCurr[0].x = (ofst.x + lastPt.x + (thisPt.x - lastPt.x) * (minY - lastPt.y) / (thisPt.y - lastPt.y));
				pointsCurr[0].y = (ofst.y + minY);
				pointsCurr[1] = (ofst + thisPt);
				pointsCurr += 2;

				lastPt = thisPt;
			}
			else
			{
				pointsCurr[0].x = (ofst.x + lastPt.x + (thisPt.x - lastPt.x) * (maxY - lastPt.y) / (thisPt.y - lastPt.y));
				pointsCurr[0].y = (ofst.y + maxY);
				pointsCurr[1] = ofst + thisPt;
				pointsCurr += 2;

				lastPt = thisPt;
			}
			yStat = 0;
		}
	}

	return (UOSInt)((pointsCurr - pointOut) >> 1);
}

UOSInt Math::GeometryTool::BoundPolygonX(const Math::Coord2DDbl *points, UOSInt nPoints, Math::Coord2DDbl *pointOut, Double minX, Double maxX, Math::Coord2DDbl ofst)
{
	Math::Coord2DDbl *pointsCurr = pointOut;
	Math::Coord2DDbl lastPt;
	Math::Coord2DDbl thisPt;
	Double xStat;
	UOSInt i;

	lastPt = points[nPoints - 1];

	if (lastPt.x <= minX)
	{
		xStat = -1;
	}
	else if (lastPt.x >= maxX)
	{
		xStat = 1;
	}
	else
	{
		xStat = 0;
	}

	i = nPoints;
	while (i-- > 0)
	{
		thisPt = points[0];
		points += 1;

		if (thisPt.x <= minX)
		{
			if (xStat == 0)
			{
				pointsCurr[0].x = (ofst.x + minX);
				pointsCurr[0].y = (ofst.y + lastPt.y + (thisPt.y - lastPt.y) * (minX - lastPt.x) / (thisPt.x - lastPt.x));
				pointsCurr += 1;

				lastPt = thisPt;
			}
			else if (xStat == -1)
			{
				lastPt = thisPt;
			}
			else
			{
				pointsCurr[0].x = (ofst.x + maxX);
				pointsCurr[0].y = (ofst.y + lastPt.y + (thisPt.y - lastPt.y) * (maxX - lastPt.x) / (thisPt.x - lastPt.x));
				pointsCurr[1].x = (ofst.x + minX);
				pointsCurr[1].y = (ofst.y + lastPt.y + (thisPt.y - lastPt.y) * (minX - lastPt.x) / (thisPt.x - lastPt.x));
				pointsCurr += 2;

				lastPt = thisPt;
			}

			xStat = -1;
		}
		else if (thisPt.x >= maxX)
		{
			if (xStat == 0)
			{
				pointsCurr[0].x = (ofst.x + maxX);
				pointsCurr[0].y = (ofst.y + lastPt.y + (thisPt.y - lastPt.y) * (maxX - lastPt.x) / (thisPt.x - lastPt.x));
				pointsCurr += 1;

				lastPt = thisPt;
			}
			else if (xStat == -1)
			{
				pointsCurr[0].x = (ofst.x + minX);
				pointsCurr[0].y = (ofst.y + lastPt.y + (thisPt.y - lastPt.y) * (minX - lastPt.x) / (thisPt.x - lastPt.x));
				pointsCurr[1].x = (ofst.x + maxX);
				pointsCurr[1].y = (ofst.y + lastPt.y + (thisPt.y - lastPt.y) * (maxX - lastPt.x) / (thisPt.x - lastPt.x));
				pointsCurr += 2;

				lastPt = thisPt;
			}
			else
			{
				lastPt = thisPt;
			}

			xStat = 1;
		}
		else
		{
			if (xStat == 0)
			{
				if (thisPt != lastPt)
				{
					pointsCurr[0] = ofst + thisPt;
					pointsCurr += 1;
					lastPt = thisPt;
				}
			}
			else if (xStat == -1)
			{
				pointsCurr[0].x = (ofst.x + minX);
				pointsCurr[0].y = (ofst.y + lastPt.y + (thisPt.y - lastPt.y) * (minX - lastPt.x) / (thisPt.x - lastPt.x));
				pointsCurr[1] = ofst + thisPt;
				pointsCurr += 2;

				lastPt = thisPt;
			}
			else
			{
				pointsCurr[0].x = (ofst.x + maxX);
				pointsCurr[0].y = (ofst.y + lastPt.y + (thisPt.y - lastPt.y) * (maxX - lastPt.x) / (thisPt.x - lastPt.x));
				pointsCurr[1] = ofst + thisPt;
				pointsCurr += 2;

				lastPt = thisPt;
			}
			xStat = 0;
		}
	}

	return (UOSInt)((pointsCurr - pointOut) >> 1);
}

Bool Math::GeometryTool::InPolygon(Int32 *points, UOSInt nPoints, Int32 ptX, Int32 ptY)
{
	Int32 firstX;
	Int32 firstY;
	Int32 lastX;
	Int32 lastY;
	UOSInt j;
	UOSInt k;
	Int32 leftCnt = 0;
	k = 1;
	firstX = points[0];
	firstY = points[1];
	lastX = firstX;
	lastY = firstY;

	while (k < nPoints)
	{
		j = 0;
		if (lastY > ptY)
			j += 1;
		if (points[(k << 1) + 1] > ptY)
			j += 1;

		if (j == 1)
		{
			if (ptX > lastX - MulDiv32(lastX - points[(k << 1)], lastY - ptY, lastY - points[(k << 1) + 1]))
			{
				leftCnt++;
			}
		}
		lastX = points[(k << 1)];
		lastY = points[(k << 1) + 1];
		k += 1;
	}

	j = 0;
	if (lastY > ptY)
		j += 1;
	if (firstY > ptY)
		j += 1;

	if (j == 1)
	{
		if (ptX > lastX - MulDiv32(lastX - firstX, lastY - ptY, lastY - firstY))
		{
			leftCnt++;
		}
	}

	return (leftCnt & 1) != 0;
}
/*
int                                //   1=inside, 0=outside
inpoly(                            // is target point inside a 2D polygon?
unsigned int poly[][2],            //   polygon points, [0]=x, [1]=y
int npoints,                       //   number of points in polygon
unsigned int xt,                   //   x (horizontal) of target point
unsigned int yt)                   //   y (vertical) of target point
{
     unsigned int xnew,ynew;
     unsigned int xold,yold;
     unsigned int x1,y1;
     unsigned int x2,y2;
     int i;
     int inside=0;

     if (npoints < 3) {
          return(0);
     }
     xold=poly[npoints-1][0];
     yold=poly[npoints-1][1];
     for (i=0 ; i < npoints ; i++) {
          xnew=poly[i][0];
          ynew=poly[i][1];
          if (xnew > xold) {
               x1=xold;
               x2=xnew;
               y1=yold;
               y2=ynew;
          }
          else {
               x1=xnew;
               x2=xold;
               y1=ynew;
               y2=yold;
          }
          if ((xnew < xt) == (xt <= xold)         // edge "open" at left end x
           && ((long)yt-(long)y1)*(long)(x2-x1)
            < ((long)y2-(long)y1)*(long)(xt-x1)) {
               inside=!inside;
          }
          xold=xnew;
          yold=ynew;
     }
     return(inside);
}
*/

void Math::GeometryTool::PtNearPline(Int32 *points, UOSInt nPoints, OSInt ptX, OSInt ptY, Int32 *nearPtX, Int32 *nearPtY)
{
	UOSInt i = nPoints - 1;
	Double calH;
	Double calW;
	Double calX = 0;
	Double calY = 0;
	Double calBase;
	Double dist = (Double)0x7fffffff;

	while (i-- > 0)
	{
		calH = points[(i << 1) + 1] - (Double)points[(i << 1) + 3];
		calW = points[(i << 1) + 0] - (Double)points[(i << 1) + 2];

		if (calH == 0)
		{
			calX = (Double)ptX;
		}
		else
		{
			calX = (calBase = (calW * calW)) * OSInt2Double(ptX);
			calBase += calH * calH;
			calX += calH * calH * (points[(i << 1) + 0]);
			calX += (OSInt2Double(ptY) - (Double)points[(i << 1) + 1]) * calH * calW;
			calX /= calBase;
		}

		if (calW == 0)
		{
			calY = (Double)ptY;
		}
		else
		{
			calY = ((calX - (Double)(points[(i << 1) + 0])) * calH / calW) + (Double)points[(i << 1) + 1];
		}

		if (calW < 0)
		{
			if (points[(i << 1) + 0] > calX)
				continue;
			if (points[(i << 1) + 2] < calX)
				continue;
		}
		else
		{
			if (points[(i << 1) + 0] < calX)
				continue;
			if (points[(i << 1) + 2] > calX)
				continue;
		}

		if (calH < 0)
		{
			if (points[(i << 1) + 1] > calY)
				continue;
			if (points[(i << 1) + 3] < calY)
				continue;
		}
		else
		{
			if (points[(i << 1) + 1] < calY)
				continue;
			if (points[(i << 1) + 3] > calY)
				continue;
		}

		calH = OSInt2Double(ptY) - calY;
		calW = OSInt2Double(ptX) - calX;
		calBase = calW * calW + calH * calH;
		if (calBase < dist)
		{
			dist = calBase;
			*nearPtX = (Int32)calX;
			*nearPtY = (Int32)calY;
		}
	}

	i = nPoints;
	while (i-- > 0)
	{
		calH = OSInt2Double(ptY) - (Double)points[(i << 1) + 1];
		calW = OSInt2Double(ptX) - (Double)points[(i << 1)];
		calBase = calW * calW + calH * calH;
		if (calBase < dist)
		{
			dist = calBase;
			*nearPtX = (Int32)calX;
			*nearPtY = (Int32)calY;
		}
	}
}

Double Math::GeometryTool::SphereDistDeg(Double lat1, Double lon1, Double lat2, Double lon2, Double radius)
{
	Double rLat1;
	Double rLat2;
	Double rLon1;
	Double rLon2;
	Double scale = Math::PI / 180.0;
	Double tmpV;
	if (lat1 == lat2 && lon1 == lon2)
		return 0;
	rLat1 = lat1 * scale;
	rLon1 = lon1 * scale;
	rLat2 = lat2 * scale;
	rLon2 = lon2 * scale;
	Double cLat1 = Math_Cos(rLat1);
	Double cLat2 = Math_Cos(rLat2);

	tmpV = cLat1 * Math_Cos(rLon1) * cLat2 * Math_Cos(rLon2) + cLat1 * Math_Sin(rLon1) * cLat2 * Math_Sin(rLon2) + Math_Sin(rLat1) * Math_Sin(rLat2);
	if (tmpV >= 1)
		return 0;
	return Math_ArcCos(tmpV) * radius;
}

void Math::GeometryTool::GetPolygonCenter(UOSInt nParts, UOSInt nPoints, UInt32 *parts, Int32 *points, Int32 *outPtX, Int32 *outPtY)
{
	Int32 minX;
	Int32 maxX;
	Int32 minY;
	Int32 maxY;
	Int32 centY;
	Int32 lastX;
	Int32 lastY;
	Int32 thisX;
	Int32 thisY;
	Int32 tempX;
	UOSInt k;
	UOSInt j;
	UOSInt i = nPoints;
	if (i <= 0)
	{
		*outPtX = 0;
		*outPtY = 0;
		return;
	}
	Data::ArrayListInt32 ptArr(4);

	maxX = minX = points[0];
	maxY = minY = points[1];
	while (i-- > 1)
	{
		if (points[(i << 1) + 0] > maxX)
		{
			maxX = points[(i << 1) + 0];
		}
		if (points[(i << 1) + 0] < minX)
		{
			minX = points[(i << 1) + 0];
		}
		if (points[(i << 1) + 1] > maxY)
		{
			maxY = points[(i << 1) + 1];
		}
		if (points[(i << 1) + 1] < minY)
		{
			minY = points[(i << 1) + 1];
		}
	}
	centY = (maxY + minY) >> 1;

	i = nPoints;
	j = nParts;
	while (j-- > 0)
	{
		k = parts[j];
		lastX = points[(k << 1) + 0];
		lastY = points[(k << 1) + 1];
		while (i-- > k)
		{
			thisX = points[(i << 1) + 0];
			thisY = points[(i << 1) + 1];

			if ((lastY >= centY && thisY < centY) || (thisY >= centY && lastY < centY))
			{
				tempX = lastX + MulDiv32(centY - lastY, thisX - lastX, thisY - lastY);
				ptArr.SortedInsert(tempX);
			}
			lastX = thisX;
			lastY = thisY;
		}

		i = k;
	}
	j = ptArr.GetCount();
	if ((j & 1) == 1 || j == 0)
	{
		*outPtX = 0;
		*outPtY = 0;
		return;
	}
	k = 0;
	i = 0;
	while (i < j)
	{
		k += (UOSInt)(ptArr.GetItem(i + 1) - ptArr.GetItem(i));
		i += 2;
	}

	k = k >> 1;
	i = 0;
	while (i < j)
	{
		lastX = ptArr.GetItem(i);
		thisX = ptArr.GetItem(i + 1);
		if ((thisX - lastX) > (OSInt)k)
		{
			*outPtX = (Int32)(lastX + (OSInt)k);
			*outPtY = centY;
			return;
		}
		k -= (UOSInt)(thisX - lastX);
		i += 2;
	}
	*outPtX = 0;
	*outPtY = 0;
	return;
}

Math::Coord2DDbl Math::GeometryTool::GetPolygonCenter(UOSInt nParts, UOSInt nPoints, UInt32 *parts, Math::Coord2DDbl *points)
{
	Double minX;
	Double maxX;
	Double minY;
	Double maxY;
	Double centY;
	Math::Coord2DDbl lastCoord;
	Math::Coord2DDbl thisCoord;
	Double tempX;
	Double sum;
	UOSInt k;
	UOSInt j;
	UOSInt i = nPoints;
	if (i <= 0)
	{
		return Math::Coord2DDbl(0, 0);
	}
	Data::ArrayListDbl ptArr(4);

	maxX = minX = points[0].x;
	maxY = minY = points[0].y;
	while (i-- > 1)
	{
		if (points[i].x > maxX)
		{
			maxX = points[i].x;
		}
		if (points[i].x < minX)
		{
			minX = points[i].x;
		}
		if (points[i].y > maxY)
		{
			maxY = points[i].y;
		}
		if (points[i].y < minY)
		{
			minY = points[i].y;
		}
	}
	centY = (maxY + minY) * 0.5;

	i = nPoints;
	j = nParts;
	while (j-- > 0)
	{
		k = parts[j];
		lastCoord = points[k];
		while (i-- > k)
		{
			thisCoord = points[i];

			if ((lastCoord.y >= centY && thisCoord.y < centY) || (thisCoord.y >= centY && lastCoord.y < centY))
			{
				tempX = lastCoord.x + (centY - lastCoord.y) * (thisCoord.x - lastCoord.x) / (thisCoord.y - lastCoord.y);
				ptArr.SortedInsert(tempX);
			}
			lastCoord = thisCoord;
		}

		i = k;
	}
	j = ptArr.GetCount();
	if ((j & 1) == 1 || j == 0)
	{
		return Math::Coord2DDbl(0, 0);
	}
	sum = 0;
	i = 0;
	while (i < j)
	{
		sum += ptArr.GetItem(i + 1) - ptArr.GetItem(i);
		i += 2;
	}

	sum = sum * 0.5;
	i = 0;
	while (i < j)
	{
		lastCoord.x = ptArr.GetItem(i);
		thisCoord.x = ptArr.GetItem(i + 1);
		if ((thisCoord.x - lastCoord.x) > sum)
		{
			return Math::Coord2DDbl(lastCoord.x + sum, centY);
		}
		sum -= thisCoord.x - lastCoord.x;
		i += 2;
	}
	return Math::Coord2DDbl(0, 0);
}

Double Math::GeometryTool::CalcMaxDistanceFromPoint(Math::Coord2DDbl pt, Math::Geometry::Vector2D *vec, Math::Unit::Distance::DistanceUnit unit)
{
	Double maxDist = -1;
	Math::Coord2DDbl maxPt = pt;
	Double thisDist;
	Data::ArrayListA<Math::Coord2DDbl> coords;
	UOSInt i = vec->GetCoordinates(coords);
	while (i-- > 0)
	{
		thisDist = coords.GetItem(i).CalcLengTo(pt);
		if (thisDist > maxDist)
		{
			maxDist = thisDist;
			maxPt = coords.GetItem(i);
		}
	}
	Math::CoordinateSystem *csys = Math::CoordinateSystemManager::SRCreateCSys(vec->GetSRID());
	if (csys)
	{
		thisDist = csys->CalSurfaceDistanceXY(pt, maxPt, unit);
		DEL_CLASS(csys);
		return thisDist;
	}
	else
	{
		return maxDist;
	}
}

Math::Coord2DDbl Math::GeometryTool::MercatorToProject(Math::Coord2DDbl pt)
{
	Double a = 6378137.0;
	return Math::Coord2DDbl(pt.GetLon() * a, a * Math_Ln(Math_Tan(Math::PI * 0.25 + pt.GetLat() * 0.5)));
}

void Math::GeometryTool::CalcHVAngleRad(Math::Coord2DDbl ptCurr, Math::Coord2DDbl ptNext, Double heightCurr, Double heightNext, Double *hAngle, Double *vAngle)
{
	Math::Coord2DDbl projCurr = MercatorToProject(ptCurr);
	Math::Coord2DDbl projDiff = MercatorToProject(ptNext) - projCurr;
	Double len = Math_Sqrt(projDiff.x * projDiff.x + projDiff.y * projDiff.y);
	if (len == 0)
	{
		*hAngle = 0;
		if (heightNext > heightCurr)
			*vAngle = Math::PI;
		else if (heightNext < heightCurr)
			*vAngle = -Math::PI;
		else
			*vAngle = 0;
	}
	else
	{
		*hAngle = Math_ArcTan2(projDiff.x, projDiff.y);
		*vAngle = Math_ArcTan2(heightNext - heightCurr, len);
		if (*hAngle < 0)
		{
			*hAngle += 2 * Math::PI;
		}
	}
}

void Math::GeometryTool::CalcHVAngleDeg(Math::Coord2DDbl ptCurr, Math::Coord2DDbl ptNext, Double heightCurr, Double heightNext, Double *hAngle, Double *vAngle)
{
	CalcHVAngleRad(ptCurr * (Math::PI / 180.0), ptNext * (Math::PI / 180.0), heightCurr, heightNext, hAngle, vAngle);
	*hAngle = *hAngle * 180 / Math::PI;
	*vAngle = *vAngle * 180 / Math::PI;
}

Math::Geometry::Polygon *Math::GeometryTool::CreateCircularPolygonWGS84(Math::Coord2DDbl pt, Double radiusMeter, UOSInt nPoints)
{
	Math::Geometry::Polygon *pg = 0;
	NotNullPtr<Math::CoordinateSystem> csys4326;
	NotNullPtr<Math::CoordinateSystem> csys3857;
	if (csys4326.Set(Math::CoordinateSystemManager::SRCreateCSys(4326)))
	{
		if (csys3857.Set(Math::CoordinateSystemManager::SRCreateCSys(3857)))
		{
			Math::Vector3 outPos = CoordinateSystem::ConvertXYZ(csys4326, csys3857, Math::Vector3(pt, 0));
			NEW_CLASS(pg, Math::Geometry::Polygon(3857, 1, nPoints + 1, false, false));
			UOSInt pgNPt;
			Math::Coord2DDbl *ptArr = pg->GetPointList(pgNPt);
			Double pi2 = Math::PI * 2;
			Double angle;
			UOSInt i = 0;
			while (i < nPoints)
			{
				angle = UOSInt2Double(i) * pi2 / UOSInt2Double(nPoints);
				ptArr[i].x = outPos.GetX() + Math_Sin(angle) * radiusMeter;
				ptArr[i].y = outPos.GetY() + Math_Cos(angle) * radiusMeter;
				i++;
			}
			ptArr[nPoints] = ptArr[0];
			pg->ConvCSys(csys3857, csys4326);
			csys3857.Delete();
		}
		csys4326.Delete();
	}
	return pg;
}
