#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Math/MSGeography.h"
#include "Math/Geometry/LineString.h"
#include "Math/Geometry/Point.h"
#include "Math/Geometry/PointZ.h"
#include "Math/Geometry/Polygon.h"

#include <stdio.h>
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
		else if (buffPtr[5] == 0x0D) //Point 3D
		{
			if (buffSize < 30)
			{
				return 0;
			}
			Math::Geometry::Point *pt;
			NEW_CLASS(pt, Math::Geometry::PointZ(srid, ReadDouble(&buffPtr[6]), ReadDouble(&buffPtr[14]), ReadDouble(&buffPtr[22])));
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
			if (shapePtr[8] == 2)
			{
				if (nShapes != 1)
				{
					printf("MSGeography: Type 4-2 must be single shape\r\n");
					return 0;
				}
				Math::Geometry::Polyline *pl;
				UOSInt i;
				UOSInt j;
				NEW_CLASS(pl, Math::Geometry::Polyline(srid, nFigures, nPoints, false, false));
				Math::Coord2DDbl *points = pl->GetPointList(&j);
				i = 0;
				while (i < j)
				{
					points[i] = Math::Coord2DDbl(ReadDouble(&pointPtr[i * 16]), ReadDouble(&pointPtr[i * 16 + 8]));
					i++;
				}
				if (nFigures > 1)
				{
					UInt32 *ofstList = pl->GetPtOfstList(&j);
					i = 0;
					while (i < j)
					{
						ofstList[i] = ReadUInt32(&figurePtr[i * 5 + 1]);
						i++;
					}
				}
				return pl;
			}
			else if (shapePtr[8] == 3)
			{
				if (nShapes != 1)
				{
					printf("MSGeography: Type 4-3 must be single shape\r\n");
					return 0;
				}
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
			else if (shapePtr[8] == 5) //Multilinestring
			{
				Math::Geometry::Polyline *pl;
				UOSInt i;
				UOSInt j;
				NEW_CLASS(pl, Math::Geometry::Polyline(srid, nFigures, nPoints, false, false));
				Math::Coord2DDbl *points = pl->GetPointList(&j);
				i = 0;
				while (i < j)
				{
					points[i] = Math::Coord2DDbl(ReadDouble(&pointPtr[i * 16]), ReadDouble(&pointPtr[i * 16 + 8]));
					i++;
				}
				if (nFigures > 1)
				{
					UInt32 *ofstList = pl->GetPtOfstList(&j);
					i = 0;
					while (i < j)
					{
						ofstList[i] = ReadUInt32(&figurePtr[i * 5 + 1]);
						i++;
					}
				}
				return pl;
			}
			else
			{
				printf("MSGeography: Type 4, Unsupported type %d\r\n", shapePtr[8]);
/*				Text::StringBuilderUTF8 sb;
				sb.AppendHexBuff(buffPtr, buffSize, ' ', Text::LineBreakType::CRLF);
				printf("%s\r\n", sb.ToString());*/
			}
		}
		else if (buffPtr[5] == 5) //Polygon 3D
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
			ind = 10 + nPoints * 24;
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
			if (shapePtr[8] == 2)
			{
				Math::Geometry::LineString *pl;
				UOSInt i;
				UOSInt j;
				NEW_CLASS(pl, Math::Geometry::LineString(srid, nPoints, true, false));
				Math::Coord2DDbl *points = pl->GetPointList(&j);
				Double *zList = pl->GetZList(&j);
				i = 0;
				while (i < j)
				{
					points[i] = Math::Coord2DDbl(ReadDouble(&pointPtr[i * 16]), ReadDouble(&pointPtr[i * 16 + 8]));
					i++;
				}
				pointPtr += j * 16;
				i = 0;
				while (i < j)
				{
					zList[i] = ReadDouble(&pointPtr[i * 8]);
					i++;
				}
				if (nFigures > 1)
				{
					printf("MSGeography: Type 5, LineString 3D with nFigures > 1\r\n");
				}
				return pl;
			}
			else if (shapePtr[8] == 3)
			{
				Math::Geometry::Polygon *pg;
				UOSInt i;
				UOSInt j;
				NEW_CLASS(pg, Math::Geometry::Polygon(srid, nFigures, nPoints, true, false));
				Math::Coord2DDbl *points = pg->GetPointList(&j);
				Double *zList = pg->GetZList(&j);
				i = 0;
				while (i < j)
				{
					points[i] = Math::Coord2DDbl(ReadDouble(&pointPtr[i * 16]), ReadDouble(&pointPtr[i * 16 + 8]));
					i++;
				}
				pointPtr += j * 16;
				i = 0;
				while (i < j)
				{
					zList[i] = ReadDouble(&pointPtr[i * 8]);
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
			else
			{
				printf("MSGeography: Type 5, Unsupported type %d\r\n", shapePtr[8]);
			}
		}
		else if (buffPtr[5] == 20) // LineString
		{
			if (buffSize < 38)
			{
				printf("MSGeography: Type 20, buffSize too short: %d\r\n", (UInt32)buffSize);
				return 0;
			}
			Math::Geometry::LineString *pl;
			NEW_CLASS(pl, Math::Geometry::LineString(srid, 2, false, false));
			UOSInt j;
			Math::Coord2DDbl *points = pl->GetPointList(&j);
			points[0] = Math::Coord2DDbl(ReadDouble(&buffPtr[6]), ReadDouble(&buffPtr[14]));
			points[1] = Math::Coord2DDbl(ReadDouble(&buffPtr[22]), ReadDouble(&buffPtr[30]));
			return pl;
		}
		else if (buffPtr[5] == 21) // LineString 3D
		{
			if (buffSize < 54)
			{
				return 0;
			}
			Math::Geometry::LineString *pl;
			NEW_CLASS(pl, Math::Geometry::LineString(srid, 2, true, false));
			UOSInt j;
			Math::Coord2DDbl *points = pl->GetPointList(&j);
			Double *zList = pl->GetZList(&j);
			points[0] = Math::Coord2DDbl(ReadDouble(&buffPtr[6]), ReadDouble(&buffPtr[14]));
			points[1] = Math::Coord2DDbl(ReadDouble(&buffPtr[22]), ReadDouble(&buffPtr[30]));
			zList[0] = ReadDouble(&buffPtr[38]);
			zList[1] = ReadDouble(&buffPtr[46]);
			return pl;
		}
		else
		{
			printf("MSGeography: Unsupported type %d\r\n", buffPtr[5]);
		}
	}
	return 0;
}
