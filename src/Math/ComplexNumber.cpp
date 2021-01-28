#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/ComplexNumber.h"

Math::ComplexNumber::ComplexNumber()
{
	this->realPart = 0.0;
	this->imaginaryPart = 0.0;
}

Math::ComplexNumber::ComplexNumber(Double val)
{
	this->realPart = val;
	this->imaginaryPart = 0.0;
}

Math::ComplexNumber::ComplexNumber(Double realPart, Double imaginaryPart)
{
	this->realPart = realPart;
	this->imaginaryPart = imaginaryPart;
}

Math::ComplexNumber::ComplexNumber(const ComplexNumber &val)
{
	this->realPart = val.realPart;
	this->imaginaryPart = val.imaginaryPart;
}

Math::ComplexNumber::~ComplexNumber()
{
}

const Math::ComplexNumber Math::ComplexNumber::operator +(const Math::ComplexNumber &val)
{
	return Math::ComplexNumber(this->realPart + val.realPart, this->imaginaryPart + val.imaginaryPart);
}

const Math::ComplexNumber Math::ComplexNumber::operator -(const Math::ComplexNumber &val)
{
	return Math::ComplexNumber(this->realPart - val.realPart, this->imaginaryPart - val.imaginaryPart);
}

const Math::ComplexNumber Math::ComplexNumber::operator *(const Math::ComplexNumber &val)
{
	return Math::ComplexNumber(this->realPart * val.realPart - this->imaginaryPart * val.imaginaryPart, this->realPart * val.imaginaryPart + this->imaginaryPart * val.realPart);
}

Double Math::ComplexNumber::GetRealPart()
{
	return this->realPart;
}

Double Math::ComplexNumber::GetImaginaryPart()
{
	return this->imaginaryPart;
}
