#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Math/MSGeography.h"
#include "Math/Point.h"
#include "Math/Polygon.h"

// https://sqlprotocoldoc.blob.core.windows.net/productionsqlarchives/MS-SSCLRT/%5bMS-SSCLRT%5d.pdf
/*
Driver={ODBC Driver 17 for SQL Server};Server=tcp:47.52.40.6,11433;uid=rpisdbadm;pwd=1q2w3e4r!Q@W#E$R;database=rpis_sde
https://gis.ectrak.com.hk:6443/arcgis/rest/services/TmpTest/GeoTest/MapServer

POLYGON ((50.497600000351667 10.510199999436736, 74.660500000230968 51.707599999383092, 53.830400000326335 45.041899999603629, 29.204499999992549 18.471999999135733, 50.497600000351667 10.510199999436736))
16 09 00 00 01 04 05 00 00 00 00 00 58 5B B1 3F 
49 40 00 00 30 EF 38 05 25 40 00 00 CB A1 45 AA 
52 40 00 00 04 A3 92 DA 49 40 00 00 16 8C 4A EA 
4A 40 00 00 AC FA 5C 85 46 40 00 00 AC 1C 5A 34 
3D 40 00 00 F0 FD D4 78 32 40 00 00 58 5B B1 3F 
49 40 00 00 30 EF 38 05 25 40 01 00 00 00 02 00 
00 00 00 01 00 00 00 FF FF FF FF 00 00 00 00 03
*/

Math::Vector2D *Math::MSGeography::ParseBinary(const UInt8 *buffPtr, UOSInt buffSize, UInt32 *srIdOut)
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
			Math::Point *pt;
			NEW_CLASS(pt, Math::Point(srid, ReadDouble(&buffPtr[6]), ReadDouble(&buffPtr[14])));
			return pt;
		}
		else if (buffPtr[5] == 4) //Polygon 2D
		{
			UInt32 nPoints;
			UInt32 nFigures;
			UInt32 nShapes;
			const UInt8 *pointPtr;
			//const UInt8 *figurePtr;
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
			//figurePtr = &buffPtr[ind + 4];
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
				Math::Polygon *pg;
				UOSInt i;
				UOSInt j;
				NEW_CLASS(pg, Math::Polygon(srid, 1, nPoints));
				Double *points = pg->GetPointList(&j);
				i = 0;
				while (i < j)
				{
					points[i * 2] = ReadDouble(&pointPtr[i * 16]);
					points[i * 2 + 1] = ReadDouble(&pointPtr[i * 16 + 8]);
					i++;
				}
				return pg;
			}
		}
	}
	return 0;
}
