#include "stdafx.h"
#include "MyMemory.h"
#include "Math/ProjectionConvert.h"

Math::ProjectionConvert::ProjectionConvert()
{
	NEW_CLASS(this->originNorthing, Math::BigFloat(256));
	NEW_CLASS(this->originEasting, Math::BigFloat(256));
	NEW_CLASS(this->originLatitude, Math::BigFloat(256));
	NEW_CLASS(this->originLongitude, Math::BigFloat(256));
	NEW_CLASS(this->meridianScale, Math::BigFloat(256));
	NEW_CLASS(this->meridianOrigin, Math::BigFloat(256));
	NEW_CLASS(this->curvPrimeVertical, Math::BigFloat(256));
	NEW_CLASS(this->curvMerdian, Math::BigFloat(256));
	NEW_CLASS(this->refEllipsoid, Math::BigFloat(256));
	NEW_CLASS(this->eccentricity, Math::BigFloat(256));

	*this->originNorthing = L"0";
	*this->originEasting = L"500000";
	*this->originLatitude = L"0";
	*this->originLongitude = L"117";
	*this->meridianScale = L"0.9996";
	*this->meridianOrigin = L"0";
	*this->curvPrimeVertical = L"6381309.467";
	*this->curvMerdian = L"6344897.718";
	*this->refEllipsoid = L"6378137";
	*this->eccentricity = L"0.006694379989";
}

Math::ProjectionConvert::~ProjectionConvert()
{
	DEL_CLASS(this->originNorthing);
	DEL_CLASS(this->originEasting);
	DEL_CLASS(this->originLatitude);
	DEL_CLASS(this->originLongitude);
	DEL_CLASS(this->meridianScale);
	DEL_CLASS(this->meridianOrigin);
	DEL_CLASS(this->curvPrimeVertical);
	DEL_CLASS(this->curvMerdian);
	DEL_CLASS(this->refEllipsoid);
	DEL_CLASS(this->eccentricity);
}

Math::BigFloat *Math::ProjectionConvert::CalMeridian(Math::BigFloat *meridianDist, Math::BigFloat *latRad)
{
	Math::BigFloat tmpVal(meridianDist->GetSize());
	Math::BigFloat tmpVal2(meridianDist->GetSize());
	Math::BigFloat tmpVal3(meridianDist->GetSize());
	Math::BigFloat e4(meridianDist->GetSize());
	e4 = this->eccentricity;
	e4 *= this->eccentricity;

	*meridianDist = latRad;
	tmpVal = 1;
	tmpVal2 = this->eccentricity;
	tmpVal2 /= 4;
	tmpVal -= &tmpVal2;
	tmpVal2 = &e4;
	tmpVal2 *= 3;
	tmpVal2 /= 64;
	tmpVal -= &tmpVal2;
	*meridianDist *= &tmpVal;

	tmpVal = e4;
	tmpVal /= 4;
	tmpVal += this->eccentricity;
	tmpVal *= 3;
	tmpVal /= 8;
	tmpVal2 = latRad;
	tmpVal2 *= 2;
	tmpVal3.SetSin(&tmpVal2);
	*meridianDist -= &tmpVal3;

	tmpVal = e4;
	tmpVal *= 15;
	tmpVal /= 256;
	tmpVal2 = latRad;
	tmpVal2 *= 4;
	tmpVal3.SetSin(&tmpVal2);
	*meridianDist += &tmpVal3;
	*meridianDist *= this->refEllipsoid;
	return meridianDist;
}
