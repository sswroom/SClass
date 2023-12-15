#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Geometry/CompoundCurve.h"

Math::Geometry::CompoundCurve::CompoundCurve(UInt32 srid) : Math::Geometry::MultiGeometry<Math::Geometry::LineString>(srid)
{
}

Math::Geometry::CompoundCurve::~CompoundCurve()
{
}

void Math::Geometry::CompoundCurve::AddGeometry(NotNullPtr<LineString> geometry)
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

NotNullPtr<Math::Geometry::Vector2D> Math::Geometry::CompoundCurve::Clone() const
{
	NotNullPtr<Math::Geometry::CompoundCurve> newObj;
	NEW_CLASSNN(newObj, Math::Geometry::CompoundCurve(this->srid));
	Data::ArrayIterator<NotNullPtr<Math::Geometry::LineString>> it = this->Iterator();
	while (it.HasNext())
	{
		newObj->AddGeometry(NotNullPtr<LineString>::ConvertFrom(it.Next()->Clone()));
	}
	return newObj;
}

UOSInt Math::Geometry::CompoundCurve::GetDrawPoints(NotNullPtr<Data::ArrayListA<Math::Coord2DDbl>> ptList)
{
	UOSInt ret = 0;
	NotNullPtr<LineString> ls;
	UOSInt nPoint;
	const Math::Coord2DDbl *ptArr;
	Data::ArrayIterator<NotNullPtr<LineString>> it = this->Iterator();
	while (it.HasNext())
	{
		ls = it.Next();
		ptArr = ls->GetPointListRead(nPoint);
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
	return ret;
}
