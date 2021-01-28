#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Map/ProjectionCoord.h"

Map::ProjectionCoord::ProjectionCoord(Double originLat, Double originLon, Double radius, Double flattening, Double falseEasting, Double falseNorthing)
{
	this->r = radius;
	this->flattening = flattening;
	this->pr = Math::Ellipsoid::CalcPolarAxis(radius, flattening);
	this->originLat = originLat;
	this->originLon = originLon;
	this->falseEasting = falseEasting;
	this->falseNorthing = falseNorthing;
	Double x;
	Double y;
	Double z;
	Geo2Project(originLat, originLon, 0, &x, &y, &z);
	this->falseEasting -= x;
	this->falseNorthing -= y;
}

Map::ProjectionCoord::~ProjectionCoord()
{
}

void Map::ProjectionCoord::Project2Geo(Double x, Double y, Double z, Double *latDeg, Double *lonDeg, Double *h)
{
	Double latRad = 0;
	Double lonRad = 0;

	///////////////////////////////////////
	*latDeg = latRad * 180 / Math::PI;
	*lonDeg = lonRad * 180 / Math::PI;
}

void Map::ProjectionCoord::Geo2Project(Double latDeg, Double lonDeg, Double h, Double *x, Double *y, Double *z)
{
/*	Double latRad = latDeg * Math::PI / 180;
	Double lonRad = lonDeg * Math::PI / 180;

	Double clat = Math::Cos(latRad);
	Double clon = Math::Cos(lonRad);
	Double slat = Math::Sin(latRad);
	Double slon = Math::Sin(lonRad);
	
	Double angular = Math::ArcCos(this->pr / this->r);
	Double tmp = slat * Math::Sin(angular);
	Double N = this->r / Math::Sqrt(1 - tmp * tmp);
	*x = (N + h) * clat * clon + falseEasting;
	*y = (N + h) * clat * slon + falseNorthing;
	*z = (this->pr * this->pr / (this->r * this->r) * N + h) * slat;*/
	///////////////////////////////////////////
}
