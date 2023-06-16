#include "Stdafx.h"
#include "IO/Device/HCSR04.h"
#include "Manage/HiResClock.h"
#include "Sync/ThreadUtil.h"

IO::Device::HCSR04::HCSR04(IO::GPIOControl *gpio, OSInt trigPin, OSInt echoPin)
{
	this->gpio = gpio;
	this->trigPin = trigPin;
	this->echoPin = echoPin;

	this->gpio->SetPinOutput(this->trigPin, true);
	this->gpio->SetPinOutput(this->echoPin, false);
	this->gpio->SetPinState(this->trigPin, false);
	Sync::SimpleThread::Sleepus(2);
}

IO::Device::HCSR04::~HCSR04()
{

}

Bool IO::Device::HCSR04::ReadTime(Int32 *t)
{
	Int64 t0;
	Int64 t1;
	Int64 t2;
	UOSInt i;
	Manage::HiResClock clk;
	t0 = clk.GetTimeDiffus();
	clk.Start();
	this->gpio->SetPinState(this->trigPin, true);
	Sync::SimpleThread::Sleepus(10);
	this->gpio->SetPinState(this->trigPin, false);

	t1 = 0;
	t2 = 0;
	i = 30000;
	while (i > 0)
	{
		if (this->gpio->IsPinHigh(this->echoPin))
		{
			t1 = clk.GetTimeDiffus();
			break;
		}
		i--;
	}
	while (i > 0)
	{
		if (!this->gpio->IsPinHigh(this->echoPin))
		{
			t2 = clk.GetTimeDiffus();
			break;
		}
		i--;
	}
	if (t2 == 0)
		return false;
	*t = (Int32)(t2 - t1 - t0);
	return true;
}

Int32 IO::Device::HCSR04::ConvDistanceMM_RoomTemp(Int32 t)
{
	return (Int32)(t * 0.3462 * 0.5);
}
