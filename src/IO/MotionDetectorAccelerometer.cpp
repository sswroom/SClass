#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/MotionDetectorAccelerometer.h"

IO::MotionDetectorAccelerometer::MotionDetectorAccelerometer(NN<IO::SensorAccelerometer> acc, Bool toRelease)
{
	this->acc = acc;
	this->toRelease = toRelease;
	this->currAcc = Math::Vector3D(0, 0, 0);
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
	Math::Vector3D thisAcc;
	Math::Vector3D diff;
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
	this->currMoving = ((diff.xy.x + diff.xy.y + diff.zn.x) >= 0.05);
	this->currAcc = thisAcc;
	return true;
}

Math::Vector3D IO::MotionDetectorAccelerometer::GetValues() const
{
	return this->currAcc;
}

Bool IO::MotionDetectorAccelerometer::IsMoving()
{
	return this->currMoving;
}
