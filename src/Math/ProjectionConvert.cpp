#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/ProjectionConvert.h"

Math::ProjectionConvert::ProjectionConvert()
{
	NEW_CLASSNN(this->originNorthing, Math::BigFloat(256));
	NEW_CLASSNN(this->originEasting, Math::BigFloat(256));
	NEW_CLASSNN(this->originLatitude, Math::BigFloat(256));
	NEW_CLASSNN(this->originLongitude, Math::BigFloat(256));
	NEW_CLASSNN(this->meridianScale, Math::BigFloat(256));
	NEW_CLASSNN(this->meridianOrigin, Math::BigFloat(256));
	NEW_CLASSNN(this->curvPrimeVertical, Math::BigFloat(256));
	NEW_CLASSNN(this->curvMerdian, Math::BigFloat(256));
	NEW_CLASSNN(this->refEllipsoid, Math::BigFloat(256));
	NEW_CLASSNN(this->eccentricity, Math::BigFloat(256));

	*this->originNorthing.Ptr() = CSTR("0");
	*this->originEasting.Ptr() = CSTR("500000");
	*this->originLatitude.Ptr() = CSTR("0");
	*this->originLongitude.Ptr() = CSTR("117");
	*this->meridianScale.Ptr() = CSTR("0.9996");
	*this->meridianOrigin.Ptr() = CSTR("0");
	*this->curvPrimeVertical.Ptr() = CSTR("6381309.467");
	*this->curvMerdian.Ptr() = CSTR("6344897.718");
	*this->refEllipsoid.Ptr() = CSTR("6378137");
	*this->eccentricity.Ptr() = CSTR("0.006694379989");
}

Math::ProjectionConvert::~ProjectionConvert()
{
	this->originNorthing.Delete();
	this->originEasting.Delete();
	this->originLatitude.Delete();
	this->originLongitude.Delete();
	this->meridianScale.Delete();
	this->meridianOrigin.Delete();
	this->curvPrimeVertical.Delete();
	this->curvMerdian.Delete();
	this->refEllipsoid.Delete();
	this->eccentricity.Delete();
}

NN<Math::BigFloat> Math::ProjectionConvert::CalMeridian(NN<Math::BigFloat> meridianDist, NN<Math::BigFloat> latRad)
{
	Math::BigFloat tmpVal(meridianDist->GetSize());
	Math::BigFloat tmpVal2(meridianDist->GetSize());
	Math::BigFloat tmpVal3(meridianDist->GetSize());
	Math::BigFloat e4(meridianDist->GetSize());
	e4 = this->eccentricity;
	e4 *= this->eccentricity;

	*meridianDist.Ptr() = latRad;
	tmpVal = 1;
	tmpVal2 = this->eccentricity;
	tmpVal2 /= 4;
	tmpVal -= tmpVal2;
	tmpVal2 = e4;
	tmpVal2 *= 3;
	tmpVal2 /= 64;
	tmpVal -= tmpVal2;
	*meridianDist.Ptr() *= tmpVal;

	tmpVal = e4;
	tmpVal /= 4;
	tmpVal += this->eccentricity;
	tmpVal *= 3;
	tmpVal /= 8;
	tmpVal2 = latRad;
	tmpVal2 *= 2;
	tmpVal3.SetSin(tmpVal2);
	*meridianDist.Ptr() -= tmpVal3;

	tmpVal = e4;
	tmpVal *= 15;
	tmpVal /= 256;
	tmpVal2 = latRad;
	tmpVal2 *= 4;
	tmpVal3.SetSin(tmpVal2);
	*meridianDist.Ptr() += tmpVal3;
	*meridianDist.Ptr() *= this->refEllipsoid;
	return meridianDist;
}
