#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/GeometryTool.h"
#include "Math/Geometry/CompoundCurve.h"

Math::Geometry::CompoundCurve::CompoundCurve(UInt32 srid) : Math::Geometry::MultiGeometry<Math::Geometry::LineString>(srid)
{
}

Math::Geometry::CompoundCurve::~CompoundCurve()
{
}

void Math::Geometry::CompoundCurve::AddGeometry(NN<LineString> geometry)
{
	VectorType t = geometry->GetVectorType();
	if (t == VectorType::CircularString || t == VectorType::LineString)
	{
		this->geometries.Add(geometry);
	}
	else
	{
		geometry.Delete();
	}
}

Math::Geometry::Vector2D::VectorType Math::Geometry::CompoundCurve::GetVectorType() const
{
	return Math::Geometry::Vector2D::VectorType::CompoundCurve;
}

NN<Math::Geometry::Vector2D> Math::Geometry::CompoundCurve::Clone() const
{
	NN<Math::Geometry::CompoundCurve> newObj;
	NEW_CLASSNN(newObj, Math::Geometry::CompoundCurve(this->srid));
	Data::ArrayIterator<NN<Math::Geometry::LineString>> it = this->Iterator();
	while (it.HasNext())
	{
		newObj->AddGeometry(NN<LineString>::ConvertFrom(it.Next()->Clone()));
	}
	return newObj;
}

UIntOS Math::Geometry::CompoundCurve::GetDrawPoints(NN<Data::ArrayListA<Math::Coord2DDbl>> ptList) const
{
	UIntOS ret = 0;
	NN<LineString> ls;
	UIntOS nPoint;
	UnsafeArray<const Math::Coord2DDbl> ptArr;
	Data::ArrayIterator<NN<LineString>> it = this->Iterator();
	while (it.HasNext())
	{
		ls = it.Next();
		ptArr = ls->GetPointListRead(nPoint);
		if (nPoint > 2 && (nPoint & 1) != 0 && ls->GetVectorType() == Math::Geometry::Vector2D::VectorType::CircularString)
		{
			UIntOS i = 2;
			while (i < nPoint)
			{
				if (ret > 0)
				{
					ptList->RemoveAt(ptList->GetCount() - 1);
					ret--;
				}
				ret += Math::GeometryTool::ArcToLine(ptArr[i - 2], ptArr[i - 1], ptArr[i], 2.5, ptList);
				i++;
			}
		}
		else
		{
			if (ret == 0 || ptList->GetItem(ptList->GetCount() - 1) != ptArr[0])
			{
				ret += nPoint;
				ptList->AddRange(ptArr, nPoint);
			}
			else
			{
				ret += nPoint - 1;
				ptList->AddRange(&ptArr[1], nPoint - 1);
			}
		}
	}
	return ret;
}

Optional<Math::Geometry::Vector2D> Math::Geometry::CompoundCurve::ToSimpleShape() const
{
	Data::ArrayListA<Math::Coord2DDbl> ptList;
	NN<Math::Geometry::LineString> ls;
	this->GetDrawPoints(ptList);
	NEW_CLASSNN(ls, Math::Geometry::LineString(this->srid, ptList.Arr(), ptList.GetCount(), nullptr, nullptr));
	return ls;
}
