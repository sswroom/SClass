#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/MotionDetectorAccelerometer.h"

IO::MotionDetectorAccelerometer::MotionDetectorAccelerometer(IO::SensorAccelerometer *acc, Bool toRelease)
{
	this->acc = acc;
	this->toRelease = toRelease;
	this->currX = 0;
	this->currY = 0;
	this->currZ = 0;
	this->currMoving = false;
}

IO::MotionDetectorAccelerometer::~MotionDetectorAccelerometer()
{
	if (this->toRelease)
	{
		DEL_CLASS(this->acc);
	}
}

Bool IO::MotionDetectorAccelerometer::UpdateStatus()
{
	Double thisX;
	Double thisY;
	Double thisZ;
	Double diffX;
	Double diffY;
	Double diffZ;
	if (!this->acc->ReadAcceleration(&thisX, &thisY, &thisZ))
	{
		return false;
	}
	if (this->currX == 0 && this->currY == 0 && this->currZ == 0)
	{
		this->currX = thisX;
		this->currY = thisY;
		this->currZ = thisZ;
		this->currMoving = false;
		return true;
	}
	diffX = this->currX - thisX;
	diffY = this->currY - thisY;
	diffZ = this->currZ - thisZ;
	if (diffX < 0)
		diffX = -diffX;
	if (diffY < 0)
		diffY = -diffY;
	if (diffZ < 0)
		diffZ = -diffZ;
	this->currMoving = ((diffX + diffY + diffZ) >= 0.05);
	this->currX = thisX;
	this->currY = thisY;
	this->currZ = thisZ;
	return true;
}

void IO::MotionDetectorAccelerometer::GetValues(Double *x, Double *y, Double *z)
{
	*x = this->currX;
	*y = this->currY;
	*z = this->currZ;
}

Bool IO::MotionDetectorAccelerometer::IsMovving()
{
	return this->currMoving;
}
