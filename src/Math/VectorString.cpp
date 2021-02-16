#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/CoordinateSystem.h"
#include "Math/VectorString.h"
#include "Text/MyString.h"

Math::VectorString::VectorString(Int32 srid, const UTF8Char *s, Double x, Double y, Double angleDegree, Double buffSize, Media::DrawEngine::DrawPos align) : Vector2D(srid)
{
	this->s = Text::StrCopyNew(s);
	this->x = x;
	this->y = y;
	this->angleDegree = angleDegree;
	this->buffSize = buffSize;
	this->align = align;
}

Math::VectorString::~VectorString()
{
	Text::StrDelNew(this->s);
}

Math::Vector2D::VectorType Math::VectorString::GetVectorType()
{
	return Math::Vector2D::VT_STRING;
}

void Math::VectorString::GetCenter(Double *x, Double *y)
{
	*x = this->x;
	*y = this->y;
}

Math::Vector2D *Math::VectorString::Clone()
{
	Math::VectorString *vstr;
	NEW_CLASS(vstr, Math::VectorString(this->srid, this->s, this->x, this->y, this->angleDegree, this->buffSize, this->align));
	return vstr;
}

void Math::VectorString::GetBounds(Double *minX, Double *minY, Double *maxX, Double *maxY)
{
	*minX = this->x;
	*minY = this->y;
	*maxX = this->x;
	*maxY = this->y;
}

Double Math::VectorString::CalSqrDistance(Double x, Double y, Double *nearPtX, Double *nearPtY)
{
	Double diffX = x - this->x;
	Double diffY = y - this->y;
	*nearPtX = this->x;
	*nearPtY = this->y;
	return diffX * diffX + diffY * diffY;
}

Bool Math::VectorString::JoinVector(Math::Vector2D *vec)
{
	return false;
}

Bool Math::VectorString::Support3D()
{
	return false;
}

void Math::VectorString::ConvCSys(Math::CoordinateSystem *srcCSys, Math::CoordinateSystem *destCSys)
{
	Math::CoordinateSystem::ConvertXYZ(srcCSys, destCSys, this->x, this->y, 0, &this->x, &this->y, 0);
}

const UTF8Char *Math::VectorString::GetString()
{
	return this->s;
}

Double Math::VectorString::GetAngleDegree()
{
	return this->angleDegree;
}

Double Math::VectorString::GetBuffSize()
{
	return this->buffSize;
}

Media::DrawEngine::DrawPos Math::VectorString::GetTextAlign()
{
	return this->align;
}
