#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/CoordinateSystem.h"
#include "Math/Geometry/VectorString.h"
#include "Text/MyString.h"

Math::Geometry::VectorString::VectorString(UInt32 srid, NotNullPtr<Text::String> s, Math::Coord2DDbl pos, Double angleDegree, Double buffSize, Media::DrawEngine::DrawPos align) : Vector2D(srid)
{
	this->s = s->Clone();
	this->pos = pos;
	this->angleDegree = angleDegree;
	this->buffSize = buffSize;
	this->align = align;
}

Math::Geometry::VectorString::VectorString(UInt32 srid, Text::CStringNN s, Math::Coord2DDbl pos, Double angleDegree, Double buffSize, Media::DrawEngine::DrawPos align) : Vector2D(srid)
{
	this->s = Text::String::New(s);
	this->pos = pos;
	this->angleDegree = angleDegree;
	this->buffSize = buffSize;
	this->align = align;
}

Math::Geometry::VectorString::~VectorString()
{
	this->s->Release();
}

Math::Geometry::Vector2D::VectorType Math::Geometry::VectorString::GetVectorType() const
{
	return Math::Geometry::Vector2D::VectorType::String;
}

Math::Coord2DDbl Math::Geometry::VectorString::GetCenter() const
{
	return this->pos;
}

NotNullPtr<Math::Geometry::Vector2D> Math::Geometry::VectorString::Clone() const
{
	NotNullPtr<Math::Geometry::VectorString> vstr;
	NEW_CLASSNN(vstr, Math::Geometry::VectorString(this->srid, this->s, this->pos, this->angleDegree, this->buffSize, this->align));
	return vstr;
}

Math::RectAreaDbl Math::Geometry::VectorString::GetBounds() const
{
	return Math::RectAreaDbl(this->pos, this->pos);
}

Double Math::Geometry::VectorString::CalBoundarySqrDistance(Math::Coord2DDbl pt, OutParam<Math::Coord2DDbl> nearPt) const
{
	Math::Coord2DDbl diff = pt - this->pos;
	nearPt.Set(this->pos);
	diff = diff * diff;
	return diff.x + diff.y;
}

Bool Math::Geometry::VectorString::JoinVector(NotNullPtr<const Math::Geometry::Vector2D> vec)
{
	return false;
}

Bool Math::Geometry::VectorString::HasZ() const
{
	return false;
}

Bool Math::Geometry::VectorString::GetZBounds(OutParam<Double> min, OutParam<Double> max) const
{
	return false;
}

Bool Math::Geometry::VectorString::GetMBounds(OutParam<Double> min, OutParam<Double> max) const
{
	return false;
}

void Math::Geometry::VectorString::Convert(NotNullPtr<Math::CoordinateConverter> converter)
{
	this->pos = converter->Convert2D(this->pos);
	this->srid = converter->GetOutputSRID();
}

Bool Math::Geometry::VectorString::Equals(NotNullPtr<const Math::Geometry::Vector2D> vec, Bool sameTypeOnly, Bool nearlyVal) const
{
	if (vec->GetVectorType() != VectorType::String)
	{
		return false;
	}
	const VectorString *vstr = (const VectorString*)vec.Ptr();
	if (nearlyVal)
		return this->srid == vstr->srid &&
			this->pos.EqualsNearly(vstr->pos) &&
			this->align == vstr->align &&
			Math::NearlyEqualsDbl(this->angleDegree, vstr->angleDegree) &&
			this->buffSize == vstr->buffSize &&
			this->s->Equals(vstr->s);
	else
		return this->srid == vstr->srid &&
			this->pos == vstr->pos &&
			this->align == vstr->align &&
			this->angleDegree == vstr->angleDegree &&
			this->buffSize == vstr->buffSize &&
			this->s->Equals(vstr->s);
}

UOSInt Math::Geometry::VectorString::GetCoordinates(NotNullPtr<Data::ArrayListA<Math::Coord2DDbl>> coordList) const
{
	coordList->Add(this->pos);
	return 1;
}

Bool Math::Geometry::VectorString::InsideOrTouch(Math::Coord2DDbl coord) const
{
	return coord == this->pos;
}

void Math::Geometry::VectorString::SwapXY()
{
	this->pos = this->pos.SwapXY();
}

void Math::Geometry::VectorString::MultiplyCoordinatesXY(Double v)
{
	this->pos = this->pos * v;
}

UOSInt Math::Geometry::VectorString::GetPointCount() const
{
	return 1;
}

NotNullPtr<Text::String> Math::Geometry::VectorString::GetString() const
{
	return this->s;
}

Double Math::Geometry::VectorString::GetAngleDegree() const
{
	return this->angleDegree;
}

Double Math::Geometry::VectorString::GetBuffSize() const
{
	return this->buffSize;
}

Media::DrawEngine::DrawPos Math::Geometry::VectorString::GetTextAlign() const
{
	return this->align;
}
