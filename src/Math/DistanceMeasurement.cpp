#include "stdafx.h"
#include "MyMemory.h"
#include "Math/DistanceMeasurement.h"
#include "Math/Math.h"

Math::DistanceMeasurement::DistanceMeasurement(Double instX, Double instY, Double instZ, Double hAngleAdj, Math::ProjectedCoordinateSystem *csys)
{
	this->instX = instX;
	this->instY = instY;
	this->instZ = instZ;
	this->hAngleAdj = hAngleAdj;
	this->csys = csys;
}

Math::DistanceMeasurement::~DistanceMeasurement()
{
}

void Math::DistanceMeasurement::FromXYZ(Double x, Double y, Double z, Double *hAngle, Double *vAngle, Double *dist)
{
	Double xDiff = x - this->instX;
	Double yDiff = y - this->instY;
	Double zDiff = z - this->instZ;
	*dist = Math::Sqrt(xDiff * xDiff + yDiff * yDiff + zDiff * zDiff);
	*vAngle = Math::ArcCos(zDiff / *dist);
	Double hAngleV = Math::ArcTan2(xDiff, yDiff) + this->hAngleAdj;
	while (hAngleV < 0)
	{
		hAngleV += Math::PI + Math::PI;
	}
	*hAngle = hAngleV;
}

void Math::DistanceMeasurement::ToXYZ(Double hAngle, Double vAngle, Double dist, Double *x, Double *y, Double *z)
{
	if (this->csys == 0)
	{
		hAngle = hAngle - this->hAngleAdj;
		Double hDist = Math::Sin(vAngle) * dist;
		*z = Math::Cos(vAngle) * dist + this->instZ;
		*x = Math::Sin(hAngle) * hDist + this->instX;
		*y = Math::Cos(hAngle) * hDist + this->instY;
	}
	else
	{
		Double lat;
		Double lon;
		Double h;
		Double cX;
		Double cY;
		Double cZ;
		Double dX;
		Double dY;
		Double dZ;
		Double rDist;
		Double rhDist;
		hAngle = hAngle - this->hAngleAdj;
		Math::GeographicCoordinateSystem *gsys = this->csys->GetGeographicCoordinateSystem();
		this->csys->ToGeographicCoordinate(this->instX, this->instY, &lon, &lat);
		h = this->instZ;
		gsys->ToCartesianCoord(lat, lon, h, &cX, &cY, &cZ);
		lat = lat * Math::PI / 180.0;
		lon = lon * Math::PI / 180.0;

		dZ = cZ + Math::Cos(hAngle) * Math::Sin(vAngle + lat) * dist;
		rDist = dist * Math::Cos(hAngle) * Math::Cos(vAngle + lat);
		rhDist = dist * Math::Sin(hAngle) * Math::Cos(vAngle + lat);
		
		dX = cX + rDist * Math::Cos(lon) - rhDist * Math::Sin(lon);
		dY = cY + rDist * Math::Sin(lon) + rhDist * Math::Cos(lon);
		gsys->FromCartesianCoord(dX, dY, dZ, &lat, &lon, &h);
		this->csys->FromGeographicCoordinate(lon, lat, x, y);
		*z = h;
	}
}