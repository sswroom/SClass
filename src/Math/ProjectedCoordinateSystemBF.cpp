#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/ProjectedCoordinateSystemBF.h"

Math::ProjectedCoordinateSystemBF::ProjectedCoordinateSystemBF(Text::CStringNN name, Double falseEasting, Double falseNorthing, Double centralMeridian, Double latitudeOfOrigin, Double scaleFactor, Double latAdj, Double lonAdj, NN<Math::GeographicCoordinateSystemBF> gcs) : Math::CoordinateSystemBF(name)
{
	NEW_CLASSNN(this->falseEasting, Math::BigFloat(256, falseEasting));
	NEW_CLASSNN(this->falseNorthing, Math::BigFloat(256, falseNorthing));
	NEW_CLASSNN(this->centralMeridian, Math::BigFloat(256, centralMeridian));
	NEW_CLASSNN(this->latitudeOfOrigin, Math::BigFloat(256, latitudeOfOrigin));
	NEW_CLASSNN(this->scaleFactor, Math::BigFloat(256, scaleFactor));
	NEW_CLASSNN(this->latAdj, Math::BigFloat(256, latAdj));
	NEW_CLASSNN(this->lonAdj, Math::BigFloat(256, lonAdj));
	this->gcs = gcs;
}

Math::ProjectedCoordinateSystemBF::~ProjectedCoordinateSystemBF()
{
	this->gcs.Delete();
	this->lonAdj.Delete();
	this->latAdj.Delete();
	this->scaleFactor.Delete();
	this->latitudeOfOrigin.Delete();
	this->centralMeridian.Delete();
	this->falseNorthing.Delete();
	this->falseEasting.Delete();
}

void Math::ProjectedCoordinateSystemBF::CalSurfaceDistanceXY(NN<const Math::BigFloat> x1, NN<const Math::BigFloat> y1, NN<const Math::BigFloat> x2, NN<const Math::BigFloat> y2, NN<Math::BigFloat> dist, Math::Unit::Distance::DistanceUnit unit) const
{
	Math::BigFloat xDiff(x2);
	Math::BigFloat yDiff(y2);
	xDiff -= x1;
	yDiff -= y1;
	xDiff *= xDiff;
	yDiff *= yDiff;
	xDiff += yDiff;
	xDiff.ToSqrt();
	if (unit != Math::Unit::Distance::DU_METER)
	{
		yDiff = Math::Unit::Distance::GetUnitRatio(unit);
		xDiff *= yDiff;
	}
	dist.Ptr()[0] = xDiff;
}
/*
Double Math::ProjectedCoordinateSystem::CalPLDistance(Math::Polyline *pl, Math::Distance::DistanceUnit unit)
{
	IntOS nPoints;
	IntOS nParts;
	Int32 *parts;
	Double *points;
	parts = pl->GetPartList(&nParts);
	points = pl->GetPointList(&nPoints);
	IntOS i = nParts;
	IntOS j = nPoints;
	IntOS k;
	Double totalDist = 0;
	Bool hasLast;
	Double lastX;
	Double lastY;
	while (i-- > 0)
	{
		k = parts[i];
		hasLast = false;
		while (j-- > k)
		{
			if (hasLast)
			{
				totalDist += CalSurfaceDistanceXY(lastX, lastY, points[(j << 1)], points[(j << 1) + 1], unit);
			}
			hasLast = true;
			lastX = points[(j << 1)];
			lastY = points[(j << 1) + 1];
		}
		j++;
	}
	return totalDist;
}

Double Math::ProjectedCoordinateSystem::CalPLDistance3D(Math::Polyline3D *pl, Math::Distance::DistanceUnit unit)
{
	IntOS nPoints;
	IntOS nParts;
	IntOS nAlts;
	Int32 *parts;
	Double *points;
	Double *alts;
	parts = pl->GetPartList(&nParts);
	points = pl->GetPointList(&nPoints);
	alts = pl->GetAltitudeList(&nAlts);
	IntOS i = nParts;
	IntOS j = nPoints;
	IntOS k;
	Double dist;
	Double totalDist = 0;
	Bool hasLast;
	Double lastX;
	Double lastY;
	Double lastH;
	while (i-- > 0)
	{
		k = parts[i];
		hasLast = false;
		while (j-- > k)
		{
			if (hasLast)
			{
				dist = CalSurfaceDistanceXY(lastX, lastY, points[(j << 1)], points[(j << 1) + 1], unit);
				dist = Math_Sqrt(dist * dist + (alts[j] - lastH) * (alts[j] - lastH));
				totalDist += dist;
			}
			hasLast = true;
			lastX = points[(j << 1)];
			lastY = points[(j << 1) + 1];
			lastH = alts[j];
		}
		j++;
	}
	return totalDist;
}

Math::CoordinateSystem *Math::ProjectedCoordinateSystem::Clone()
{
	Math::CoordinateSystem *csys;
	NEW_CLASS(csys, Math::ProjectedCoordinateSystem(this->sourceName, this->falseEasting, this->falseNorthing, this->centralMeridian, this->latitudeOfOrigin, this->scaleFactor, (Math::GeographicCoordinateSystem*)this->gcs->Clone()));
	return csys;
}

Math::CoordinateSystem::CoordinateSystemType Math::ProjectedCoordinateSystem::GetCoordSysType()
{
	return Math::CoordinateSystem::CoordinateSystemType::PROJECTED;
}

Math::GeographicCoordinateSystem *Math::ProjectedCoordinateSystem::GetGeographicCoordinateSystem()
{
	return this->gcs;
}

void Math::ProjectedCoordinateSystem::ToGeographicCoordinate(Double projX, Double projY, Double *geoX, Double *geoY)
{
	Double aF = this->gcs->GetSemiMajorAxis() * this->scaleFactor;
	Double rLatL = (projY - this->falseNorthing) / aF + (this->latitudeOfOrigin * PI / 180.0);
	Double e = this->gcs->GetEccentricity();
	Double e2 = e * e;
	Double tmpV;
	while (true)
	{
		tmpV = projY - this->falseNorthing - this->CalcM(rLatL);
		rLatL = rLatL + tmpV / aF;
		if (tmpV < 0.00001)
			break;
	}
	Double sLat = Math_Sin(rLatL);
	Double tLat = Math_Tan(rLatL);
	Double tLat2 = tLat * tLat;
	Double tLat4 = tLat2 * tLat2;
	Double secLat = 1 / Math_Cos(rLatL);
	Double tmp = 1 - e2 * sLat * sLat;
	Double v = aF / Math_Sqrt(tmp);
	Double v3 = v * v * v;
	Double v5 = v3 * v * v;
	Double v7 = v5 * v * v;
	Double p = v * (1 - e2) / tmp;
	Double nb2 = v / p - 1;

	Double ser7 = tLat / (2 * p * v);
	Double ser8 = tLat / (24 * p * v3) * (5 + 3 * tLat2 + nb2 - 9 * tLat2 * nb2);
	Double ser9 = tLat / (720 * p * v5) * (61 + 90 * tLat2 + 45 * tLat4);
	Double ser10 = secLat / v;
	Double ser11 = secLat / (6 * v3) * (v / p + 2 * tLat2);
	Double ser12 = secLat / (120 * v5) * (5 + 28 * tLat2 + 24 * tLat4);
	Double ser12a = secLat / (5040 * v5 * v * v) * (61 + 662 * tLat2 + 1320 * tLat4 + 720 * tLat4 * tLat2);

	Double eDiff = projX - this->falseEasting;
	Double outX = (this->centralMeridian * PI / 180.0) + ser10 * eDiff - ser11 * Math_Pow(eDiff, 3) + ser12 * Math_Pow(eDiff, 5) - ser12a * Math_Pow(eDiff, 7);
	Double outY = rLatL - ser7 * Math_Pow(eDiff, 2) + ser8 * Math_Pow(eDiff, 4) - ser9 * Math_Pow(eDiff, 6);

	*geoX = (outX * 180.0 / PI);
	*geoY = (outY * 180.0 / PI);
}

void Math::ProjectedCoordinateSystem::FromGeographicCoordinate(Double geoX, Double geoY, Double *projX, Double *projY)
{
	Double rLat = geoY * PI / 180.0;
	Double rLon = geoX * PI / 180.0;
	Double rLon0 = this->centralMeridian * PI / 180;
	Double sLat = Math_Sin(rLat);
	Double a = gcs->GetSemiMajorAxis();
	Double e = gcs->GetEccentricity();
	Double e2 = e * e;
	Double tmp = 1 - e2 * sLat * sLat;
	Double v = a * this->scaleFactor / Math_Sqrt(tmp);
	Double p = v * (1 - e2) / tmp;
	Double nb2 = v / p - 1;
	Double m = this->CalcM(rLat);
	Double cLat = Math_Cos(rLat);
	Double tLat = Math_Tan(rLat);
	Double tLat2 = tLat * tLat;
	Double tLat4 = tLat2 * tLat2;
	Double cLat3 = cLat * cLat * cLat;
	Double cLat5 = cLat3 * cLat * cLat;
	
	Double ser1 = m + this->falseNorthing;
	Double ser2 = v * 0.5 * cLat * sLat;
	Double ser3 = v / 24 * sLat * cLat3 * (5 - tLat2 + 9 * nb2);
	Double ser3a = v / 720 * sLat * cLat5 * (61 - 58 * tLat2 + tLat4);
	Double ser4 = v * cLat;
	Double ser5 = v / 6 * cLat3 * (v / p - tLat2);
	Double ser6 = v / 120 * cLat5 * (5 - 18 * tLat2 + tLat4 + 14 * nb2 - 58 * tLat2 * nb2);
	Double dlon = rLon - rLon0;
	Double dlon2 = dlon * dlon;
	Double dlon4 = dlon2 * dlon2;

	*projX = this->falseEasting + ser4 * dlon + ser5 * dlon * dlon2 + ser6 * dlon * dlon4;
	*projY = ser1 + ser2 * dlon2 + ser3 * dlon4 + ser3a * dlon4 * dlon2;
}

Double Math::ProjectedCoordinateSystem::CalcM(Double rLat)
{
	Double a = gcs->GetSemiMajorAxis();
	Double b = gcs->GetSemiMinorAxis();
	Double n = (a - b) / (a + b);
	Double n2 = n * n;
	Double n3 = n2 * n;
	Double rLat0 = this->latitudeOfOrigin * PI / 180;
	Double m;
	m = (1 + n + 1.25 * n2 + 1.25 * n3) * (rLat - rLat0);
	m = m - (3 * n + 3 * n2  + 2.625 * n3) * Math_Sin(rLat - rLat0) * Math_Cos(rLat + rLat0);
	m = m + (1.875 * n2 + 1.875 * n3) * Math_Sin(2 * (rLat - rLat0)) * Math_Cos(2 * (rLat + rLat0));
	m = m - 35 / 24 * n3 * Math_Sin(3 * (rLat - rLat0)) * Math_Cos(3 * (rLat + rLat0));
	m = m * b * this->scaleFactor;
	return m;
}*/

Bool Math::ProjectedCoordinateSystemBF::SameProjection(NN<Math::ProjectedCoordinateSystemBF> csys) const
{
	if (this->falseEasting.Ptr()[0] != csys->falseEasting)
		return false;
	if (this->falseNorthing.Ptr()[0] != csys->falseNorthing)
		return false;
	if (this->centralMeridian.Ptr()[0] != csys->centralMeridian)
		return false;
	if (this->latitudeOfOrigin.Ptr()[0] != csys->latitudeOfOrigin)
		return false;
	if (this->scaleFactor.Ptr()[0] != csys->scaleFactor)
		return false;
	if (this->latAdj.Ptr()[0] != csys->latAdj)
		return false;
	if (this->lonAdj.Ptr()[0] != csys->lonAdj)
		return false;
	return this->gcs->Equals(csys->gcs);
}

Optional<Math::ProjectedCoordinateSystemBF> Math::ProjectedCoordinateSystemBF::CreateCoordinateSystem(Math::CoordinateSystemManager::ProjCoordSysType pcst)
{
//	Math::ProjectedCoordinateSystemBF *csys;
//	Math::GeographicCoordinateSystemBF *gcs;
/*	switch (pcst)
	{
	case Math::ProjectedCoordinateSystem::PCST_HK80:
		gcs = Math::GeographicCoordinateSystemBF::CreateCoordinateSystem(Math::GeographicCoordinateSystem::GCST_INTL1924);
		NEW_CLASS(csys, Math::ProjectedCoordinateSystemBF(L"HK 1980 Grid", 836694.05, 819069.80, 114.17855555555555555555555555556, 22.312133333333333333333333333333, 1, -0.00152777777777778, 0.00244444444444444, gcs));
		return csys;
	case Math::ProjectedCoordinateSystem::PCST_UK_NATIONAL_GRID:
		gcs = Math::GeographicCoordinateSystemBF::CreateCoordinateSystem(Math::GeographicCoordinateSystem::GCST_AIRY1830);
		NEW_CLASS(csys, Math::ProjectedCoordinateSystemBF(L"National Grid", 400000, -100000, -2, 49, 0.9996012717, 0, 0, gcs));
		return csys;
	case Math::ProjectedCoordinateSystem::PCST_IRISH_NATIONAL_GRID:
		gcs = Math::GeographicCoordinateSystemBF::CreateCoordinateSystem(Math::GeographicCoordinateSystem::GCST_AIRY1830M);
		NEW_CLASS(csys, Math::ProjectedCoordinateSystemBF(L"Irish National Grid", 200000, 250000, -8, 53.5, 1.000035, 0, 0, gcs));
		return csys;
	}*/
	return 0;
}
