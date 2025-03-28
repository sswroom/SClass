#include "Stdafx.h"
#include "Math/WKBWriter.h"
#include "Math/Geometry/Polygon.h"

#include <stdio.h>

Math::WKBWriter::WKBWriter(Bool isoMode)
{
	this->isoMode = isoMode;
}

Math::WKBWriter::~WKBWriter()
{
}

Bool Math::WKBWriter::Write(NN<IO::Stream> stm, NN<Math::Geometry::Vector2D> vec)
{
	UInt8 buff[64];
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt nPoint;
	UInt32 geomType;
	buff[0] = IS_BYTEORDER_LE;
	switch (vec->GetVectorType())
	{
	case Math::Geometry::Vector2D::VectorType::Polygon:
		{
			NN<Math::Geometry::Polygon> pg = NN<Math::Geometry::Polygon>::ConvertFrom(vec);
			NN<Math::Geometry::LinearRing> lr;
			UnsafeArray<Math::Coord2DDbl> points;
			geomType = 3;
			j = pg->GetCount();
			if (vec->HasZ())
			{
				if (this->isoMode)
				{
					geomType += 1000;
					if (vec->HasM())
					{
						geomType += 2000;
					}
				}
				else
				{
					geomType |= 0x80000000;
					if (vec->HasM())
					{
						geomType |= 0x40000000;
					}
				}
			}
			if (vec->GetSRID() != 0)
			{
				geomType |= 0x20000000;
				WriteNUInt32(&buff[1], geomType);
				WriteNUInt32(&buff[5], vec->GetSRID());
				WriteNUInt32(&buff[9], (UInt32)j);
				stm->Write(Data::ByteArrayR(buff, 13));
			}
			else
			{
				WriteNUInt32(&buff[1], geomType);
				WriteNUInt32(&buff[5], (UInt32)j);
				stm->Write(Data::ByteArrayR(buff, 9));
			}
			i = 0;
			while (i < j)
			{
				if (pg->GetItem(i).SetTo(lr))
				{
					points = lr->GetPointList(nPoint);
					if (!pg->HasZ())
					{
						WriteNUInt32(buff, (UInt32)nPoint);
						stm->Write(Data::ByteArrayR(buff, 4));
						stm->Write(Data::ByteArrayR(UnsafeArray<const UInt8>::ConvertFrom(points), nPoint * 16));
					}
					else
					{
						WriteNUInt32(buff, (UInt32)nPoint);
						stm->Write(Data::ByteArrayR(buff, 4));
						UnsafeArray<Double> zList;
						UnsafeArray<Double> mList;
						k = 0;
 						if (!pg->HasM())
						{
							if (lr->GetZList(nPoint).SetTo(zList))
							{
								while (k < nPoint)
								{
									WriteNDouble(&buff[0], points[k].x);
									WriteNDouble(&buff[8], points[k].y);
									WriteNDouble(&buff[16], zList[k]);
									stm->Write(Data::ByteArrayR(buff, 24));
									k++;
								}
							}
							else
							{
								while (k < nPoint)
								{
									WriteNDouble(&buff[0], points[k].x);
									WriteNDouble(&buff[8], points[k].y);
									WriteNDouble(&buff[16], NAN);
									stm->Write(Data::ByteArrayR(buff, 24));
									k++;
								}
							}
						}
						else
						{
							if (lr->GetZList(nPoint).SetTo(zList))
							{
								if (lr->GetMList(nPoint).SetTo(mList))
								{
									while (k < nPoint)
									{
										WriteNDouble(&buff[0], points[k].x);
										WriteNDouble(&buff[8], points[k].y);
										WriteNDouble(&buff[16], zList[k]);
										WriteNDouble(&buff[24], mList[k]);
										stm->Write(Data::ByteArrayR(buff, 32));
										k++;
									}
								}
								else
								{
									while (k < nPoint)
									{
										WriteNDouble(&buff[0], points[k].x);
										WriteNDouble(&buff[8], points[k].y);
										WriteNDouble(&buff[16], zList[k]);
										WriteNDouble(&buff[24], NAN);
										stm->Write(Data::ByteArrayR(buff, 32));
										k++;
									}
								}
							}
							else
							{
								if (lr->GetMList(nPoint).SetTo(mList))
								{
									while (k < nPoint)
									{
										WriteNDouble(&buff[0], points[k].x);
										WriteNDouble(&buff[8], points[k].y);
										WriteNDouble(&buff[16], NAN);
										WriteNDouble(&buff[24], mList[k]);
										stm->Write(Data::ByteArrayR(buff, 32));
										k++;
									}
								}
								else
								{
									while (k < nPoint)
									{
										WriteNDouble(&buff[0], points[k].x);
										WriteNDouble(&buff[8], points[k].y);
										WriteNDouble(&buff[16], NAN);
										WriteNDouble(&buff[24], NAN);
										stm->Write(Data::ByteArrayR(buff, 32));
										k++;
									}
								}
							}
						}

					}
				}
				else
				{
					WriteNUInt32(buff, 0);
					stm->Write(Data::ByteArrayR(buff, 4));
				}
				i++;
			}
			return true;
		}
	case Math::Geometry::Vector2D::VectorType::Point:
	case Math::Geometry::Vector2D::VectorType::LineString:
	case Math::Geometry::Vector2D::VectorType::MultiPoint:
	case Math::Geometry::Vector2D::VectorType::Polyline: //MultiLineString
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
	case Math::Geometry::Vector2D::VectorType::LinearRing:
	case Math::Geometry::Vector2D::VectorType::Image:
	case Math::Geometry::Vector2D::VectorType::String:
	case Math::Geometry::Vector2D::VectorType::Ellipse:
	case Math::Geometry::Vector2D::VectorType::PieArea:
	case Math::Geometry::Vector2D::VectorType::Unknown:
	default:	
		printf("WKBWriter: Unsupported type: %s\r\n", Math::Geometry::Vector2D::VectorTypeGetName(vec->GetVectorType()).v.Ptr());
		return false;
	}
}
