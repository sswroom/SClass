#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Math/GeographicCoordinateSystem.h"
#include "Math/ProjectedCoordinateSystem.h"
#include "Text/MyString.h"

Math::CoordinateSystem::CoordinateSystem(Text::String *sourceName, UInt32 srid, const UTF8Char *csysName) : IO::ParsedObject(sourceName)
{
	this->csysName = Text::StrCopyNew(csysName);
	this->srid = srid;
}

Math::CoordinateSystem::CoordinateSystem(const UTF8Char *sourceName, UInt32 srid, const UTF8Char *csysName) : IO::ParsedObject(sourceName)
{
	this->csysName = Text::StrCopyNew(csysName);
	this->srid = srid;
}

Math::CoordinateSystem::~CoordinateSystem()
{
	Text::StrDelNew(this->csysName);
}

IO::ParserType Math::CoordinateSystem::GetParserType()
{
	return IO::ParserType::CoordinateSystem;
}

Bool Math::CoordinateSystem::Equals(Math::CoordinateSystem *csys)
{
	if (this == csys)
		return true;
	Math::CoordinateSystem::CoordinateSystemType cst = this->GetCoordSysType();
	if (cst != csys->GetCoordSysType())
		return false;
	if (cst == Math::CoordinateSystem::CoordinateSystemType::Geographic)
	{
		Math::GeographicCoordinateSystem *gcs1 = (Math::GeographicCoordinateSystem*)this;
		Math::GeographicCoordinateSystem *gcs2 = (Math::GeographicCoordinateSystem*)csys;
		return gcs1->GetEllipsoid()->Equals(gcs2->GetEllipsoid());
	}
	else if (cst == Math::CoordinateSystem::CoordinateSystemType::PointMapping)
	{
		return false;
	}
	else
	{
		Math::ProjectedCoordinateSystem *pcs1 = (Math::ProjectedCoordinateSystem*)this;
		Math::ProjectedCoordinateSystem *pcs2 = (Math::ProjectedCoordinateSystem*)csys;
		return pcs1->SameProjection(pcs2);
	}
}

const UTF8Char *Math::CoordinateSystem::GetCSysName()
{
	return this->csysName;
}

UInt32 Math::CoordinateSystem::GetSRID()
{
	return this->srid;
}

void Math::CoordinateSystem::ConvertXYZ(Math::CoordinateSystem *srcCoord, Math::CoordinateSystem *destCoord, Double srcX, Double srcY, Double srcZ, Double *destX, Double *destY, Double *destZ)
{
	if (srcCoord->IsProjected())
	{
		Math::ProjectedCoordinateSystem *pcs = (Math::ProjectedCoordinateSystem*)srcCoord;
		pcs->ToGeographicCoordinate(srcX, srcY, &srcX, &srcY);
		srcCoord = pcs->GetGeographicCoordinateSystem();
	}
	if (srcCoord->Equals(destCoord))
	{
		*destX = srcX;
		*destY = srcY;
		if (destZ)
			*destZ = srcZ;
		return;
	}
	((Math::GeographicCoordinateSystem*)srcCoord)->ToCartesianCoord(srcY, srcX, srcZ, &srcX, &srcY, &srcZ);

	if (destCoord->IsProjected())
	{
		Math::ProjectedCoordinateSystem *pcs = (Math::ProjectedCoordinateSystem*)destCoord;
		Math::GeographicCoordinateSystem *gcs = pcs->GetGeographicCoordinateSystem();
		gcs->FromCartesianCoord(srcX, srcY, srcZ, &srcY, &srcX, &srcZ);
		pcs->FromGeographicCoordinate(srcX, srcY, destX, destY);
		if (destZ)
			*destZ = srcZ;
	}
	else
	{
		Math::GeographicCoordinateSystem *gcs = (Math::GeographicCoordinateSystem*)destCoord;;
		gcs->FromCartesianCoord(srcX, srcY, srcZ, destY, destX, &srcZ);
		if (destZ)
			*destZ = srcZ;
	}
}

