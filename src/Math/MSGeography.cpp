#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Math/MSGeography.h"
#include "Math/Geometry/LineString.h"
#include "Math/Geometry/MultiPolygon.h"
#include "Math/Geometry/Point.h"
#include "Math/Geometry/PointZ.h"
#include "Math/Geometry/Polygon.h"
#include "Math/Geometry/Polyline.h"

#define VERBOSE
#include <stdio.h>
// https://sqlprotocoldoc.blob.core.windows.net/productionsqlarchives/MS-SSCLRT/%5bMS-SSCLRT%5d.pdf

Optional<Math::Geometry::Vector2D> Math::MSGeography::ParseBinary(const UInt8 *buffPtr, UOSInt buffSize, OptOut<UInt32> srIdOut)
{
	if (buffSize < 6)
	{
		return 0;
	}
	UInt32 srid = ReadUInt32(&buffPtr[0]);
	srIdOut.Set(srid);
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
		else if (buffPtr[5] == 0x0D) //Point Z
		{
			if (buffSize < 30)
			{
				return 0;
			}
			Math::Geometry::Point *pt;
			NEW_CLASS(pt, Math::Geometry::PointZ(srid, ReadDouble(&buffPtr[6]), ReadDouble(&buffPtr[14]), ReadDouble(&buffPtr[22])));
			return pt;
		}
		else if (buffPtr[5] == 4 || buffPtr[5] == 0) //Shape 2D
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
			if (shapePtr[8] == 1)
			{
				if (nShapes != 1)
				{
					printf("MSGeography: Type 4-1 must be single shape\r\n");
					return 0;
				}
				if (nPoints == 0)
				{
					printf("MSGeography: Point empty\r\n");
					return 0;
				}
				else if (nPoints != 1)
				{
					printf("MSGeography: Type 4-1 must be single point\r\n");
					return 0;
				}
				Math::Geometry::Point *pt;
				NEW_CLASS(pt, Math::Geometry::Point(srid, ReadDouble(&pointPtr[0]), ReadDouble(&pointPtr[8])));
				return pt;
			}
			else if (shapePtr[8] == 2)
			{
				if (nShapes != 1)
				{
					printf("MSGeography: Type 4-2 must be single shape\r\n");
					return 0;
				}
				Math::Geometry::Polyline *pl;
				NN<Math::Geometry::LineString> lineString;
				UOSInt i;
				UOSInt j;
				UOSInt k;
				UOSInt l;
				Math::Coord2DDbl *points;
				NEW_CLASS(pl, Math::Geometry::Polyline(srid));
				if (nFigures <= 1)
				{
					NEW_CLASSNN(lineString, Math::Geometry::LineString(srid, nPoints, false, false));
					points = lineString->GetPointList(j);
					i = 0;
					while (i < j)
					{
						points[i] = Math::Coord2DDbl(ReadDouble(&pointPtr[i * 16]), ReadDouble(&pointPtr[i * 16 + 8]));
						i++;
					}
					pl->AddGeometry(lineString);
				}
				else
				{
					i = 0;
					while (i < nFigures)
					{
						l = ReadUInt32(&figurePtr[i * 5 + 1]);
						if (i + 1 == nFigures)
						{
							k = nPoints;
						}
						else
						{
							k = ReadUInt32(&figurePtr[i * 5 + 6]);
						}
						NEW_CLASSNN(lineString, Math::Geometry::LineString(srid, k - l, false, false));
						points = lineString->GetPointList(j);
						j = 0;
						while (l < k)
						{
							points[j] = Math::Coord2DDbl(ReadDouble(&pointPtr[l * 16]), ReadDouble(&pointPtr[l * 16 + 8]));
							l++;
							j++;
						}
						pl->AddGeometry(lineString);
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
				NN<Math::Geometry::LinearRing> lr;
				UOSInt i;
				UOSInt j;
				UOSInt k;
				UOSInt l;
				NEW_CLASS(pg, Math::Geometry::Polygon(srid));
				j = 0;
				i = 0;
				while (i < nFigures)
				{
					if (i + 1 >= nFigures)
						k = nPoints;
					else
						k = ReadUInt32(&figurePtr[i * 5 + 1]);
					NEW_CLASSNN(lr, Math::Geometry::LinearRing(srid, (k - j), false ,false));
					Math::Coord2DDbl *points = lr->GetPointList(l);
					l = 0;
					while (j < k)
					{
						points[l] = Math::Coord2DDbl(ReadDouble(&pointPtr[j * 16]), ReadDouble(&pointPtr[j * 16 + 8]));
						j++;
						l++;
					}
					pg->AddGeometry(lr);
					i++;
				}
				return pg;
			}
			else if (shapePtr[8] == 5) //Multilinestring
			{
				Math::Geometry::Polyline *pl;
				NN<Math::Geometry::LineString> lineString;
				UOSInt i;
				UOSInt j;
				UOSInt k;
				UOSInt l;
				Math::Coord2DDbl *points;
				NEW_CLASS(pl, Math::Geometry::Polyline(srid));
				if (nFigures <= 1)
				{
					NEW_CLASSNN(lineString, Math::Geometry::LineString(srid, nPoints, false, false));
					points = lineString->GetPointList(j);
					i = 0;
					while (i < j)
					{
						points[i] = Math::Coord2DDbl(ReadDouble(&pointPtr[i * 16]), ReadDouble(&pointPtr[i * 16 + 8]));
						i++;
					}
					pl->AddGeometry(lineString);
				}
				else
				{
					i = 0;
					while (i < nFigures)
					{
						l = ReadUInt32(&figurePtr[i * 5 + 1]);
						if (i + 1 == nFigures)
						{
							k = nPoints;
						}
						else
						{
							k = ReadUInt32(&figurePtr[i * 5 + 6]);
						}
						NEW_CLASSNN(lineString, Math::Geometry::LineString(srid, k - l, false, false));
						points = lineString->GetPointList(j);
						j = 0;
						while (l < k)
						{
							points[j] = Math::Coord2DDbl(ReadDouble(&pointPtr[l * 16]), ReadDouble(&pointPtr[l * 16 + 8]));
							l++;
							j++;
						}
						pl->AddGeometry(lineString);
						i++;
					}
				}
				return pl;
			}
			else if (shapePtr[8] == 6) //MultiPolygon
			{
				Math::Geometry::MultiPolygon *mpg;
				NN<Math::Geometry::Polygon> pg;
				NN<Math::Geometry::LinearRing> lr;
				UOSInt i;
				UOSInt j;
				UOSInt k;
				UOSInt l;
				NEW_CLASS(mpg, Math::Geometry::MultiPolygon(srid));
				if (nFigures == 0)
					nFigures = 1;
				i = 0;
				j = 0;
				while (i < nFigures)
				{
					i++;
					if (i == nFigures)
					{
						k = nPoints;
					}
					else
					{
						k = ReadUInt32(&figurePtr[i * 5 + 1]);
					}
					NEW_CLASSNN(pg, Math::Geometry::Polygon(srid));
					NEW_CLASSNN(lr, Math::Geometry::LinearRing(srid, (k - j), false, false));
					Math::Coord2DDbl *points = lr->GetPointList(l);
					l = 0;
					while (j < k)
					{
						points[l] = Math::Coord2DDbl(ReadDouble(&pointPtr[j * 16]), ReadDouble(&pointPtr[j * 16 + 8]));
						j++;
						l++;
					}
					pg->AddGeometry(lr);
					mpg->AddGeometry(pg);
				}
				return mpg;
			}
			else if (shapePtr[8] == 7) //GeometryCollection
			{
				if (nPoints == 0)
				{
					printf("MSGeography: GeometryCollection found\r\n");
				}
				else
				{
					printf("MSGeography: GeometryCollection not supported\r\n");
				}
			}
			else
			{
				printf("MSGeography: Type 4, Unsupported type %d\r\n", shapePtr[8]);
/*				Text::StringBuilderUTF8 sb;
				sb.AppendHexBuff(buffPtr, buffSize, ' ', Text::LineBreakType::CRLF);
				printf("%s\r\n", sb.ToString());*/
			}
		}
		else if (buffPtr[5] == 5) //Shape Z
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
			if (shapePtr[8] == 2)
			{
				if (nShapes != 1)
				{
					printf("MSGeography: Type 5-2, Multi shape not supported\r\n");
					return 0;
				}
				Math::Geometry::LineString *pl;
				UOSInt i;
				UOSInt j;
				NEW_CLASS(pl, Math::Geometry::LineString(srid, nPoints, true, false));
				Math::Coord2DDbl *points = pl->GetPointList(j);
				Double *zList = pl->GetZList(j);
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
				if (nShapes != 1)
				{
					printf("MSGeography: Type 5-3, Multi shape not supported\r\n");
					return 0;
				}
				Math::Geometry::Polygon *pg;
				NN<Math::Geometry::LinearRing> lr;
				UOSInt i;
				UOSInt j;
				UOSInt k;
				UOSInt l;
				const UInt8 *zPtr = pointPtr + nPoints * 16;
				if (nFigures == 0)
					nFigures = 1;
				NEW_CLASS(pg, Math::Geometry::Polygon(srid));
				i = 0;
				j = 0;
				while (i < nFigures)
				{
					i++;
					if (i >= nFigures)
						k = nPoints;
					else
						k = ReadUInt32(&figurePtr[i * 5 + 1]);
					NEW_CLASSNN(lr, Math::Geometry::LinearRing(srid, (k - j), true, false));
					Math::Coord2DDbl *points = lr->GetPointList(l);
					Double *zList = lr->GetZList(l);
					l = 0;
					while (j < k)
					{
						points[l] = Math::Coord2DDbl(ReadDouble(&pointPtr[j * 16]), ReadDouble(&pointPtr[j * 16 + 8]));
						zList[l] = ReadDouble(&zPtr[j * 8]);
						j++;
						l++;
					}
					pg->AddGeometry(lr);
				}
				return pg;
			}
			else if (shapePtr[8] == 5)
			{
				const UInt8 *zPtr = pointPtr + nPoints * 16;
				Math::Geometry::Polyline *pl;
				NN<Math::Geometry::LineString> lineString;
				UOSInt i;
				UOSInt j;
				UOSInt k;
				UOSInt l;
				Math::Coord2DDbl *points;
				Double *zArr;
				NEW_CLASS(pl, Math::Geometry::Polyline(srid));
				if (nFigures <= 1)
				{
					NEW_CLASSNN(lineString, Math::Geometry::LineString(srid, nPoints, true, false));
					points = lineString->GetPointList(j);
					zArr = lineString->GetZList(j);
					i = 0;
					while (i < j)
					{
						points[i] = Math::Coord2DDbl(ReadDouble(&pointPtr[i * 16]), ReadDouble(&pointPtr[i * 16 + 8]));
						zArr[i] = ReadDouble(&zPtr[i * 8]);
						i++;
					}
					pl->AddGeometry(lineString);
				}
				else
				{
					i = 0;
					while (i < nFigures)
					{
						l = ReadUInt32(&figurePtr[i * 5 + 1]);
						if (i + 1 == nFigures)
						{
							k = nPoints;
						}
						else
						{
							k = ReadUInt32(&figurePtr[i * 5 + 6]);
						}
						NEW_CLASSNN(lineString, Math::Geometry::LineString(srid, k - l, true, false));
						points = lineString->GetPointList(j);
						zArr = lineString->GetZList(j);
						j = 0;
						while (l < k)
						{
							points[j] = Math::Coord2DDbl(ReadDouble(&pointPtr[l * 16]), ReadDouble(&pointPtr[l * 16 + 8]));
							zArr[j] = ReadDouble(&zPtr[l * 8]);
							l++;
							j++;
						}
						pl->AddGeometry(lineString);
						i++;
					}
				}
				return pl;
			}
			else if (shapePtr[8] == 6)
			{
				Math::Geometry::MultiPolygon *mpg;
				NEW_CLASS(mpg, Math::Geometry::MultiPolygon(srid));
				shapePtr += 9;
				UOSInt thisFigure;
				UOSInt nextFigure = ReadUInt32(&shapePtr[4]);
				UOSInt thisPtOfst;
				UOSInt nextPtOfst = ReadUInt32(&figurePtr[nextFigure * 5 + 1]);
				UOSInt i;
				UOSInt j;
				const UInt8 *pointPtrTmp;
				i = 1;
				j = nShapes;
				while (i < j)
				{
					thisFigure = nextFigure;
					thisPtOfst = nextPtOfst;
					if (i + 1 >= j)
					{
						nextFigure = nFigures;
						nextPtOfst = nPoints;
					}
					else
					{
						nextFigure = ReadUInt32(&shapePtr[13]);
						nextPtOfst = ReadUInt32(&figurePtr[nextFigure * 5 + 1]);
					}
					NN<Math::Geometry::Polygon> pg;
					NN<Math::Geometry::LinearRing> lr;
					UOSInt k;
					UOSInt l;
					UOSInt m = thisFigure;
					UOSInt tmpV;
					pointPtrTmp = pointPtr + nPoints * 16;
					NEW_CLASSNN(pg, Math::Geometry::Polygon(srid));
					k = 0;
					while (m < nextFigure)
					{
						if (m + 1 >= nextFigure)
							l = nextFigure - thisFigure;
						else
							l = ReadUInt32(&figurePtr[(k + thisFigure) * 5 + 1]);
						NEW_CLASSNN(lr, Math::Geometry::LinearRing(srid, l - k, true, false));
						Math::Coord2DDbl *points = lr->GetPointList(tmpV);
						Double *zList = lr->GetZList(tmpV);
						while (k < l)
						{
							points[k] = Math::Coord2DDbl(ReadDouble(&pointPtr[(k + thisPtOfst) * 16]), ReadDouble(&pointPtr[(k + thisPtOfst) * 16 + 8]));
							zList[k] = ReadDouble(&pointPtrTmp[(k + thisPtOfst) * 8]);
							k++;
						}
						pg->AddGeometry(lr);
					}
					mpg->AddGeometry(pg);

					shapePtr += 9;
					i++;
				}
				return mpg;
			}
			else
			{
				printf("MSGeography: Type 5, Unsupported type %d\r\n", shapePtr[8]);
			}
		}
		else if (buffPtr[5] == 7) //Shape ZM
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
			ind = 10 + nPoints * 32;
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
					printf("MSGeography: Type 7-2, Multi shape not supported\r\n");
					return 0;
				}
				Math::Geometry::LineString *pl;
				UOSInt i;
				UOSInt j;
				NEW_CLASS(pl, Math::Geometry::LineString(srid, nPoints, true, true));
				Math::Coord2DDbl *points = pl->GetPointList(j);
				Double *zList = pl->GetZList(j);
				Double *mList = pl->GetMList(j);
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
				pointPtr += j * 8;
				i = 0;
				while (i < j)
				{
					mList[i] = ReadDouble(&pointPtr[i * 8]);
					i++;
				}
				if (nFigures > 1)
				{
					printf("MSGeography: Type 7, LineString ZM with nFigures > 1\r\n");
				}
				return pl;
			}
/*			else if (shapePtr[8] == 3)
			{
				if (nShapes != 1)
				{
					printf("MSGeography: Type 7, Multi shape not supported\r\n");
					Text::StringBuilderUTF8 sb;
					sb.AppendHexBuff(shapePtr, nShapes * 9, ' ', Text::LineBreakType::CRLF);
					printf("MSGeography: Type 7: shape buff: %s\r\n", sb.ToString());
					return 0;
				}
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
			}*/
			else if (shapePtr[8] == 5)
			{
				const UInt8 *zPtr = pointPtr + nPoints * 16;
				const UInt8 *mPtr = zPtr + nPoints * 8;
				Math::Geometry::Polyline *pl;
				NN<Math::Geometry::LineString> lineString;
				UOSInt i;
				UOSInt j;
				UOSInt k;
				UOSInt l;
				Math::Coord2DDbl *points;
				Double *zArr;
				Double *mArr;
				NEW_CLASS(pl, Math::Geometry::Polyline(srid));
				if (nFigures <= 1)
				{
					NEW_CLASSNN(lineString, Math::Geometry::LineString(srid, nPoints, true, true));
					points = lineString->GetPointList(j);
					zArr = lineString->GetZList(j);
					mArr = lineString->GetMList(j);
					i = 0;
					while (i < j)
					{
						points[i] = Math::Coord2DDbl(ReadDouble(&pointPtr[i * 16]), ReadDouble(&pointPtr[i * 16 + 8]));
						zArr[i] = ReadDouble(&zPtr[i * 8]);
						mArr[i] = ReadDouble(&mPtr[i * 8]);
						i++;
					}
					pl->AddGeometry(lineString);
				}
				else
				{
					i = 0;
					while (i < nFigures)
					{
						l = ReadUInt32(&figurePtr[i * 5 + 1]);
						if (i + 1 == nFigures)
						{
							k = nPoints;
						}
						else
						{
							k = ReadUInt32(&figurePtr[i * 5 + 6]);
						}
						NEW_CLASSNN(lineString, Math::Geometry::LineString(srid, k - l, true, true));
						points = lineString->GetPointList(j);
						zArr = lineString->GetZList(j);
						mArr = lineString->GetMList(j);
						j = 0;
						while (l < k)
						{
							points[j] = Math::Coord2DDbl(ReadDouble(&pointPtr[l * 16]), ReadDouble(&pointPtr[l * 16 + 8]));
							zArr[j] = ReadDouble(&zPtr[l * 8]);
							mArr[j] = ReadDouble(&mPtr[l * 8]);
							l++;
							j++;
						}
						pl->AddGeometry(lineString);
						i++;
					}
				}
				return pl;
			}
			else
			{
				printf("MSGeography: Type 7, Unsupported type %d\r\n", shapePtr[8]);
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
			Math::Coord2DDbl *points = pl->GetPointList(j);
			points[0] = Math::Coord2DDbl(ReadDouble(&buffPtr[6]), ReadDouble(&buffPtr[14]));
			points[1] = Math::Coord2DDbl(ReadDouble(&buffPtr[22]), ReadDouble(&buffPtr[30]));
			return pl;
		}
		else if (buffPtr[5] == 21) // LineString Z
		{
			if (buffSize < 54)
			{
				return 0;
			}
			Math::Geometry::LineString *pl;
			NEW_CLASS(pl, Math::Geometry::LineString(srid, 2, true, false));
			UOSInt j;
			Math::Coord2DDbl *points = pl->GetPointList(j);
			Double *zList = pl->GetZList(j);
			points[0] = Math::Coord2DDbl(ReadDouble(&buffPtr[6]), ReadDouble(&buffPtr[14]));
			points[1] = Math::Coord2DDbl(ReadDouble(&buffPtr[22]), ReadDouble(&buffPtr[30]));
			zList[0] = ReadDouble(&buffPtr[38]);
			zList[1] = ReadDouble(&buffPtr[46]);
			return pl;
		}
		else if (buffPtr[5] == 23) // LineString ZM
		{
			if (buffSize < 70)
			{
				return 0;
			}
			Math::Geometry::LineString *pl;
			NEW_CLASS(pl, Math::Geometry::LineString(srid, 2, true, true));
			UOSInt j;
			Math::Coord2DDbl *points = pl->GetPointList(j);
			Double *zList = pl->GetZList(j);
			Double *mList = pl->GetMList(j);
			points[0] = Math::Coord2DDbl(ReadDouble(&buffPtr[6]), ReadDouble(&buffPtr[14]));
			points[1] = Math::Coord2DDbl(ReadDouble(&buffPtr[22]), ReadDouble(&buffPtr[30]));
			zList[0] = ReadDouble(&buffPtr[38]);
			zList[1] = ReadDouble(&buffPtr[46]);
			mList[0] = ReadDouble(&buffPtr[54]);
			mList[1] = ReadDouble(&buffPtr[62]);
			return pl;
		}
		else
		{
			printf("MSGeography: Unsupported type %d\r\n", buffPtr[5]);
#if defined(VERBOSE)
			Text::StringBuilderUTF8 sb;
			sb.AppendHexBuff(buffPtr, buffSize, ' ', Text::LineBreakType::CRLF);
			printf("MSGeography: %s\r\n", sb.ToPtr());
#endif
		}
	}
	return 0;
}
