#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/MotionDetectorAccelerometer.h"

IO::MotionDetectorAccelerometer::MotionDetectorAccelerometer(NN<IO::SensorAccelerometer> acc, Bool toRelease)
{
	this->acc = acc;
	this->toRelease = toRelease;
	this->currAcc = Math::Vector3(0, 0, 0);
	this->currMoving = false;
}

IO::MotionDetectorAccelerometer::~MotionDetectorAccelerometer()
{
	if (this->toRelease)
	{
		this->acc.Delete();
	}
}

Bool IO::MotionDetectorAccelerometer::UpdateStatus()
{
	Math::Vector3 thisAcc;
	Math::Vector3 diff;
	if (!this->acc->ReadAcceleration(thisAcc))
	{
		return false;
	}
	if (this->currAcc.IsZero())
	{
		this->currAcc = thisAcc;
		this->currMoving = false;
		return true;
	}
	diff = (this->currAcc - thisAcc).Abs();
	this->currMoving = ((diff.val[0] + diff.val[1] + diff.val[2]) >= 0.05);
	this->currAcc = thisAcc;
	return true;
}

Math::Vector3 IO::MotionDetectorAccelerometer::GetValues() const
{
	return this->currAcc;
}

Bool IO::MotionDetectorAccelerometer::IsMovving()
{
	return this->currMoving;
}
