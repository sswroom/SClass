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

NotNullPtr<Math::Geometry::Vector2D> Math::Geometry::PointZ::Clone() const
{
	NotNullPtr<Math::Geometry::PointZ> pt;
	NEW_CLASSNN(pt, Math::Geometry::PointZ(this->srid, this->pos.x, this->pos.y, this->z));
	return pt;
}

Double Math::Geometry::PointZ::GetZ() const
{
	return this->z;
}

Math::Vector3 Math::Geometry::PointZ::GetPos3D() const
{
	return Math::Vector3(this->pos, this->z);
}

Bool Math::Geometry::PointZ::HasZ() const
{
	return true;
}

void Math::Geometry::PointZ::ConvCSys(NotNullPtr<const Math::CoordinateSystem> srcCSys, NotNullPtr<const Math::CoordinateSystem> destCSys)
{
	Math::Vector3 tmpPos = Math::CoordinateSystem::ConvertXYZ(srcCSys, destCSys, Math::Vector3(this->pos, this->z));
	this->pos = tmpPos.GetXY();
	this->z = tmpPos.GetZ();
	this->srid = destCSys->GetSRID();
}

Bool Math::Geometry::PointZ::Equals(NotNullPtr<const Math::Geometry::Vector2D> vec) const
{
	if (vec->GetSRID() != this->srid)
	{
		return false;
	}
	if (vec->GetVectorType() == VectorType::Point && vec->HasZ())
	{
		const Math::Geometry::PointZ *pt = (const Math::Geometry::PointZ*)vec.Ptr();
		return this->pos == pt->pos && this->z == pt->z;
	}
	else
	{
		return false;
	}
}

Bool Math::Geometry::PointZ::EqualsNearly(NotNullPtr<const Math::Geometry::Vector2D> vec) const
{
	if (vec->GetSRID() != this->srid)
	{
		return false;
	}
	if (vec->GetVectorType() == VectorType::Point && vec->HasZ() && !vec->HasM())
	{
		const Math::Geometry::PointZ *pt = (const Math::Geometry::PointZ*)vec.Ptr();
		return this->pos.EqualsNearly(pt->pos) &&
				Math::NearlyEqualsDbl(this->z, pt->z);
	}
	else
	{
		return false;
	}
}
