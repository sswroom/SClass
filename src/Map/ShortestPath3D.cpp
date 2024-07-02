#include "Stdafx.h"
#include "Map/ShortestPath3D.h"
#include "Math/CoordinateSystemConverter.h"

void Map::ShortestPath3D::FreeLineInfo(NN<LineInfo> lineInfo)
{
	lineInfo->vec.Delete();
	lineInfo.Delete();
}

void Map::ShortestPath3D::FreeAreaInfo(NN<AreaInfo> areaInfo)
{
	areaInfo->nodes.DeleteAll();
	areaInfo.Delete();
}

NN<Map::ShortestPath3D::AreaInfo> Map::ShortestPath3D::GetArea(Math::Coord2DDbl pos)
{
	OSInt areaX = (OSInt)(pos.x / this->searchDist);
	OSInt areaY = (OSInt)(pos.y / this->searchDist);
	OSInt i = 0;
	OSInt j = (OSInt)this->areas.GetCount() - 1;
	OSInt k;
	NN<AreaInfo> areaInfo;
	while (i <= j)
	{
		k = (i + j) >> 1;
		areaInfo = this->areas.GetItemNoCheck((UOSInt)k);
		if (areaInfo->x > areaX)
		{
			j = k - 1;
		}
		else if (areaInfo->x < areaX)
		{
			i = k + 1;
		}
		else if (areaInfo->y > areaY)
		{
			j = k - 1;
		}
		else if (areaInfo->y < areaY)
		{
			i = k + 1;
		}
		else
		{
			return areaInfo;
		}
	}
	NEW_CLASSNN(areaInfo, AreaInfo());
	areaInfo->x = areaX;
	areaInfo->y = areaY;
	this->areas.Insert((UOSInt)i, areaInfo);
	return areaInfo;

}

NN<Map::ShortestPath3D::NodeInfo> Map::ShortestPath3D::GetNode(Math::Coord2DDbl pos, Double z)
{
	NN<AreaInfo> areaInfo = GetArea(pos);
	OSInt i = 0;
	OSInt j = (OSInt)areaInfo->nodes.GetCount() - 1;
	OSInt k;
	NN<NodeInfo> nodeInfo;
	while (i <= j)
	{
		k = (i + j) >> 1;
		nodeInfo = areaInfo->nodes.GetItemNoCheck((UOSInt)k);
		if (nodeInfo->pos.x > pos.x)
		{
			j = k - 1;
		}
		else if (nodeInfo->pos.x < pos.x)
		{
			i = k + 1;
		}
		else if (nodeInfo->pos.y > pos.y)
		{
			j = k - 1;
		}
		else if (nodeInfo->pos.y < pos.y)
		{
			i = k + 1;
		}
		else if (nodeInfo->z > z)
		{
			j = k - 1;
		}
		else if (nodeInfo->z < z)
		{
			i = k + 1;
		}
		else
		{
			return nodeInfo;
		}
	}
	NEW_CLASSNN(nodeInfo, NodeInfo());
	nodeInfo->pos = pos;
	nodeInfo->z = z;
	areaInfo->nodes.Insert((UOSInt)i, nodeInfo);
	return nodeInfo;
}

void Map::ShortestPath3D::AddVector(NN<Math::Geometry::Vector2D> vec)
{
	Math::Geometry::Vector2D::VectorType type = vec->GetVectorType();
	NN<Math::Geometry::Polyline> pl;
	NN<Math::Geometry::LineString> ls;
	NN<LineInfo> lineInfo;
	NN<NodeInfo> nodeInfo;
	UnsafeArray<Math::Coord2DDbl> ptList;
	UnsafeArray<Double> zList;
	UOSInt nPoints;
	UOSInt i;
	UOSInt j;
	switch (type)
	{
	case Math::Geometry::Vector2D::VectorType::Polyline:
		pl = NN<Math::Geometry::Polyline>::ConvertFrom(vec);
		i = 0;
		j = pl->GetCount();
		while (i < j)
		{
			if (pl->GetItem(i).SetTo(ls))
			{
				this->AddVector(ls->Clone());
			}
			i++;
		}
		vec.Delete();
		break;
	case Math::Geometry::Vector2D::VectorType::LineString:
		ls = NN<Math::Geometry::LineString>::ConvertFrom(vec);
		NEW_CLASSNN(lineInfo, LineInfo());
		ptList = ls->GetPointList(nPoints);
		lineInfo->startPos = ptList[0];
		lineInfo->endPos = ptList[nPoints - 1];
		if (ls->GetZList(nPoints).SetTo(zList))
		{
			lineInfo->startZ = zList[0];
			lineInfo->endZ = zList[nPoints - 1];
		}
		else
		{
			lineInfo->startZ = 0;
			lineInfo->endZ = 0;
		}
		lineInfo->index = this->lines.GetCount();
		lineInfo->networkId = 0;
		lineInfo->vec = ls;
		this->lines.Add(lineInfo);
		nodeInfo = this->GetNode(lineInfo->startPos, lineInfo->startZ);
		nodeInfo->lines.Add(lineInfo);
		nodeInfo = this->GetNode(lineInfo->endPos, lineInfo->endZ);
		nodeInfo->lines.Add(lineInfo);
		break;
	case Math::Geometry::Vector2D::VectorType::Unknown:
	case Math::Geometry::Vector2D::VectorType::Point:
	case Math::Geometry::Vector2D::VectorType::Polygon:
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
	case Math::Geometry::Vector2D::VectorType::LinearRing:
	case Math::Geometry::Vector2D::VectorType::Image:
	case Math::Geometry::Vector2D::VectorType::String:
	case Math::Geometry::Vector2D::VectorType::Ellipse:
	case Math::Geometry::Vector2D::VectorType::PieArea:
	default:
		printf("Unsupported vector type: %s\r\n", Math::Geometry::Vector2D::VectorTypeGetName(type).v.Ptr());
		vec.Delete();
		break;
	}
}

Map::ShortestPath3D::ShortestPath3D(NN<Math::CoordinateSystem> csys, Double searchDist)
{
	this->csys = csys;
	this->searchDist = searchDist;
}

Map::ShortestPath3D::~ShortestPath3D()
{
	this->csys.Delete();
	this->areas.FreeAll(FreeAreaInfo);
	this->lines.FreeAll(FreeLineInfo);
}

void Map::ShortestPath3D::AddLayer(NN<Map::MapDrawLayer> layer)
{
	NN<Math::CoordinateSystem> csys = layer->GetCoordinateSystem();
	Optional<Map::NameArray> nameArr;
	Data::ArrayListInt64 idArr;
	NN<Math::Geometry::Vector2D> vec;
	NN<Map::GetObjectSess> sess = layer->BeginGetObject();
	layer->GetAllObjectIds(idArr, nameArr);
	UOSInt i = 0;
	UOSInt j = idArr.GetCount();
	while (i < j)
	{
		if (layer->GetNewVectorById(sess, idArr.GetItem(i)).SetTo(vec))
		{
			if (csys->Equals(this->csys))
			{
				this->AddVector(vec);
			}
			else
			{
				Math::CoordinateSystemConverter converter(csys, this->csys);
				vec->Convert(converter);
				this->AddVector(vec);
			}
		}
		i++;
	}
	layer->EndGetObject(sess);
}
