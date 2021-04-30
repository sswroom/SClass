#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Vector2D.h"

Math::Vector2D::Vector2D(UInt32 srid)
{
	this->srid = srid;
}

Math::Vector2D::~Vector2D()
{

}

UInt32 Math::Vector2D::GetSRID()
{
	return this->srid;
}

void Math::Vector2D::SetSRID(UInt32 srid)
{
	this->srid = srid;
}
