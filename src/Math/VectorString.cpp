#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/CoordinateSystem.h"
#include "Math/VectorString.h"
#include "Text/MyString.h"

Math::VectorString::VectorString(UInt32 srid, Text::String *s, Math::Coord2DDbl pos, Double angleDegree, Double buffSize, Media::DrawEngine::DrawPos align) : Vector2D(srid)
{
	this->s = s->Clone();
	this->pos = pos;
	this->angleDegree = angleDegree;
	this->buffSize = buffSize;
	this->align = align;
}

Math::VectorString::VectorString(UInt32 srid, Text::CString s, Math::Coord2DDbl pos, Double angleDegree, Double buffSize, Media::DrawEngine::DrawPos align) : Vector2D(srid)
{
	this->s = Text::String::New(s);
	this->pos = pos;
	this->angleDegree = angleDegree;
	this->buffSize = buffSize;
	this->align = align;
}

Math::VectorString::~VectorString()
{
	this->s->Release();
}

Math::Vector2D::VectorType Math::VectorString::GetVectorType() const
{
	return Math::Vector2D::VectorType::String;
}

Math::Coord2DDbl Math::VectorString::GetCenter() const
{
	return this->pos;
}

Math::Vector2D *Math::VectorString::Clone() const
{
	Math::VectorString *vstr;
	NEW_CLASS(vstr, Math::VectorString(this->srid, this->s, this->pos, this->angleDegree, this->buffSize, this->align));
	return vstr;
}

void Math::VectorString::GetBounds(Math::RectAreaDbl *bounds) const
{
	*bounds = Math::RectAreaDbl(this->pos, this->pos);
}

Double Math::VectorString::CalSqrDistance(Math::Coord2DDbl pt, Math::Coord2DDbl *nearPt) const
{
	Math::Coord2DDbl diff = pt - this->pos;
	*nearPt = this->pos;
	diff = diff * diff;
	return diff.x + diff.y;
}

Bool Math::VectorString::JoinVector(Math::Vector2D *vec)
{
	return false;
}

Bool Math::VectorString::HasZ() const
{
	return false;
}

void Math::VectorString::ConvCSys(Math::CoordinateSystem *srcCSys, Math::CoordinateSystem *destCSys)
{
	Math::CoordinateSystem::ConvertXYZ(srcCSys, destCSys, this->pos.x, this->pos.y, 0, &this->pos.x, &this->pos.y, 0);
}

Bool Math::VectorString::Equals(Math::Vector2D *vec) const
{
	if (vec == 0 || vec->GetVectorType() != VectorType::String)
	{
		return false;
	}
	VectorString *vstr = (VectorString*)vec;
	return this->srid == vstr->srid &&
		this->pos == vstr->pos &&
		this->align == vstr->align &&
		this->angleDegree == vstr->angleDegree &&
		this->buffSize == vstr->buffSize &&
		this->s->Equals(vstr->s);
}

Text::String *Math::VectorString::GetString() const
{
	return this->s;
}

Double Math::VectorString::GetAngleDegree() const
{
	return this->angleDegree;
}

Double Math::VectorString::GetBuffSize() const
{
	return this->buffSize;
}

Media::DrawEngine::DrawPos Math::VectorString::GetTextAlign() const
{
	return this->align;
}
