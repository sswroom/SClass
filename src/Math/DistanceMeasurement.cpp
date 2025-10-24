#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/DistanceMeasurement.h"
#include "Math/Math_C.h"

Math::DistanceMeasurement::DistanceMeasurement(Double instX, Double instY, Double instZ, Double hAngleAdj, Optional<Math::ProjectedCoordinateSystem> csys)
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

void Math::DistanceMeasurement::FromXYZ(Double x, Double y, Double z, OutParam<Double> hAngle, OutParam<Double> vAngle, OutParam<Double> dist)
{
	Double xDiff = x - this->instX;
	Double yDiff = y - this->instY;
	Double zDiff = z - this->instZ;
	Double d;
	dist.Set(d = Math_Sqrt(xDiff * xDiff + yDiff * yDiff + zDiff * zDiff));
	vAngle.Set(Math_ArcCos(zDiff / d));
	Double hAngleV = Math_ArcTan2(xDiff, yDiff) + this->hAngleAdj;
	while (hAngleV < 0)
	{
		hAngleV += Math::PI + Math::PI;
	}
	hAngle.Set(hAngleV);
}

void Math::DistanceMeasurement::ToXYZ(Double hAngle, Double vAngle, Double dist, OutParam<Double> x, OutParam<Double> y, OutParam<Double> z)
{
	NN<Math::ProjectedCoordinateSystem> csys;
	if (!this->csys.SetTo(csys))
	{
		hAngle = hAngle - this->hAngleAdj;
		Double hDist = Math_Sin(vAngle) * dist;
		z.Set(Math_Cos(vAngle) * dist + this->instZ);
		x.Set(Math_Sin(hAngle) * hDist + this->instX);
		y.Set(Math_Cos(hAngle) * hDist + this->instY);
	}
	else
	{
		Double h;
		Double dX;
		Double dY;
		Double dZ;
		Double rDist;
		Double rhDist;
		hAngle = hAngle - this->hAngleAdj;
		NN<Math::GeographicCoordinateSystem> gsys = csys->GetGeographicCoordinateSystem();
		Math::Coord2DDbl latlon = csys->ToGeographicCoordinateRad(Math::Coord2DDbl(this->instX, this->instY));
		h = this->instZ;
		Math::Vector3 cxyz = gsys->ToCartesianCoordRad(Math::Vector3(latlon, h));

		dZ = cxyz.GetZ() + Math_Cos(hAngle) * Math_Sin(vAngle + latlon.GetLat()) * dist;
		rDist = dist * Math_Cos(hAngle) * Math_Cos(vAngle + latlon.GetLat());
		rhDist = dist * Math_Sin(hAngle) * Math_Cos(vAngle + latlon.GetLat());
		
		dX = cxyz.GetX() + rDist * Math_Cos(latlon.GetLon()) - rhDist * Math_Sin(latlon.GetLon());
		dY = cxyz.GetY() + rDist * Math_Sin(latlon.GetLon()) + rhDist * Math_Cos(latlon.GetLon());
		Math::Vector3 latlonh = gsys->FromCartesianCoordRad(Math::Vector3(dX, dY, dZ));
		Math::Coord2DDbl xy = csys->FromGeographicCoordinateRad(latlonh.GetXY());
		x.Set(xy.x);
		y.Set(xy.y);
		z.Set(latlonh.GetZ());
	}
}