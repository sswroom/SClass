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
	*dist = Math_Sqrt(xDiff * xDiff + yDiff * yDiff + zDiff * zDiff);
	*vAngle = Math_ArcCos(zDiff / *dist);
	Double hAngleV = Math_ArcTan2(xDiff, yDiff) + this->hAngleAdj;
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
		Double hDist = Math_Sin(vAngle) * dist;
		*z = Math_Cos(vAngle) * dist + this->instZ;
		*x = Math_Sin(hAngle) * hDist + this->instX;
		*y = Math_Cos(hAngle) * hDist + this->instY;
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

		dZ = cZ + Math_Cos(hAngle) * Math_Sin(vAngle + lat) * dist;
		rDist = dist * Math_Cos(hAngle) * Math_Cos(vAngle + lat);
		rhDist = dist * Math_Sin(hAngle) * Math_Cos(vAngle + lat);
		
		dX = cX + rDist * Math_Cos(lon) - rhDist * Math_Sin(lon);
		dY = cY + rDist * Math_Sin(lon) + rhDist * Math_Cos(lon);
		gsys->FromCartesianCoord(dX, dY, dZ, &lat, &lon, &h);
		this->csys->FromGeographicCoordinate(lon, lat, x, y);
		*z = h;
	}
}