#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/CoordinateSystem.h"
#include "Math/Geometry/PointZ.h"

Math::Geometry::PointZ::PointZ(UInt32 srid, Double x, Double y, Double z) : Math::Geometry::Point(srid, x, y)
{
	this->z = z;
}

Math::Geometry::PointZ::~PointZ()
{
}

NN<Math::Geometry::Vector2D> Math::Geometry::PointZ::Clone() const
{
	NN<Math::Geometry::PointZ> pt;
	NEW_CLASSNN(pt, Math::Geometry::PointZ(this->srid, this->pos.x, this->pos.y, this->z));
	return pt;
}

Double Math::Geometry::PointZ::GetZ() const
{
	return this->z;
}

Bool Math::Geometry::PointZ::GetZBounds(OutParam<Double> min, OutParam<Double> max) const
{
	min.Set(this->z);
	max.Set(this->z);
	return true;
}

Math::Vector3 Math::Geometry::PointZ::GetPos3D() const
{
	return Math::Vector3(this->pos, this->z);
}

Bool Math::Geometry::PointZ::HasZ() const
{
	return true;
}

void Math::Geometry::PointZ::Convert(NN<Math::CoordinateConverter> converter)
{
	Math::Vector3 tmpPos = converter->Convert3D(Math::Vector3(this->pos, this->z));
	this->pos = tmpPos.GetXY();
	this->z = tmpPos.GetZ();
	this->srid = converter->GetOutputSRID();
}

Bool Math::Geometry::PointZ::Equals(NN<const Math::Geometry::Vector2D> vec, Bool sameTypeOnly, Bool nearlyVal) const
{
	if (vec->GetSRID() != this->srid)
	{
		return false;
	}
	if (vec->GetVectorType() == VectorType::Point && vec->HasZ())
	{
		const Math::Geometry::PointZ *pt = (const Math::Geometry::PointZ*)vec.Ptr();
		if (nearlyVal)
			return this->pos.EqualsNearly(pt->pos) &&
					Math::NearlyEqualsDbl(this->z, pt->z);
		else
			return this->pos == pt->pos && this->z == pt->z;
	}
	else
	{
		return false;
	}
}

