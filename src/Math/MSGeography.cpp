#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Math/MSGeography.h"
#include "Math/Geometry/Point.h"
#include "Math/Geometry/Polygon.h"

// https://sqlprotocoldoc.blob.core.windows.net/productionsqlarchives/MS-SSCLRT/%5bMS-SSCLRT%5d.pdf

Math::Geometry::Vector2D *Math::MSGeography::ParseBinary(const UInt8 *buffPtr, UOSInt buffSize, UInt32 *srIdOut)
{
	if (buffSize < 6)
	{
		return 0;
	}
	UInt32 srid = ReadUInt32(&buffPtr[0]);
	if (srIdOut)
	{
		*srIdOut = srid;
	}
	if (buffPtr[4] == 1 || buffPtr[4] == 2) //version 1 or 2
	{
		if (buffPtr[5] == 0x0C) //Point 2D
		{
			if (buffSize < 22)
			{
				return 0;
			}
			Math::Geometry::Point *pt;
			NEW_CLASS(pt, Math::Geometry::Point(srid, ReadDouble(&buffPtr[6]), ReadDouble(&buffPtr[14])));
			return pt;
		}
		else if (buffPtr[5] == 4) //Polygon 2D
		{
			UInt32 nPoints;
			UInt32 nFigures;
			UInt32 nShapes;
			const UInt8 *pointPtr;
			const UInt8 *figurePtr;
			const UInt8 *shapePtr;
			UOSInt ind;
			if (buffSize < 10)
			{
				return 0;
			}
			nPoints = ReadUInt32(&buffPtr[6]);
			pointPtr = &buffPtr[10];
			ind = 10 + nPoints * 16;
			if (buffSize < ind + 4)
			{
				return 0;
			}
			nFigures = ReadUInt32(&buffPtr[ind]);
			figurePtr = &buffPtr[ind + 4];
			ind += 4 + nFigures * 5;
			if (buffSize < ind + 4)
			{
				return 0;
			}
			nShapes = ReadUInt32(&buffPtr[ind]);
			shapePtr = &buffPtr[ind + 4];
			if (buffSize < ind + 4 + nShapes * 9)
			{
				return 0;
			}
			if (nShapes != 1)
			{
				return 0;
			}
			if (shapePtr[8] == 3)
			{
				Math::Geometry::Polygon *pg;
				UOSInt i;
				UOSInt j;
				NEW_CLASS(pg, Math::Geometry::Polygon(srid, nFigures, nPoints, false, false));
				Math::Coord2DDbl *points = pg->GetPointList(&j);
				i = 0;
				while (i < j)
				{
					points[i] = Math::Coord2DDbl(ReadDouble(&pointPtr[i * 16]), ReadDouble(&pointPtr[i * 16 + 8]));
					i++;
				}
				if (nFigures > 1)
				{
					UInt32 *ofstList = pg->GetPtOfstList(&j);
					i = 0;
					while (i < j)
					{
						ofstList[i] = ReadUInt32(&figurePtr[i * 5 + 1]);
						i++;
					}
				}
				return pg;
			}
		}
	}
	return 0;
}
