#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "IO/Device/DHT22.h"
#include "Manage/HiResClock.h"
#include "Sync/Thread.h"
//#include <wchar.h>
//#include <stdio.h>

IO::Device::DHT22::DHT22(IO::IOPin *pin)
{
	this->pin = pin;
}

IO::Device::DHT22::~DHT22()
{
}

Bool IO::Device::DHT22::ReadData(Double *temp, Double *rh)
{
	Bool laststate = true;
	UInt32 counter = 0;
	UInt8 dhtBuff[5];
	OSInt i;
	OSInt j;
	UInt8 mask;
	Manage::HiResClock clk;
	Double t;

	dhtBuff[0] = 0;
	dhtBuff[1] = 0;
	dhtBuff[2] = 0;
	dhtBuff[3] = 0;
	dhtBuff[4] = 0;

	this->pin->SetPinOutput(true);
	this->pin->SetPinState(false);
	Sync::Thread::Sleep(18);
	this->pin->SetPinState(true);
	Sync::Thread::Sleepus(40);
	this->pin->SetPinOutput(false);
	clk.Start();

	i = 0;
	j = 0;
	mask = 128;
	t = 0;
	while (j < 5)
	{
		counter = 0;
		while (true)
		{
			if (this->pin->IsPinHigh() != laststate)
			{
				laststate = !laststate;
				break;
			}
			counter++;
			Sync::Thread::Sleepus( 1 );
			if ( counter == 255 )
			{
				break;
			}
		}
 
		if ( counter == 255 )
			break;
		t = clk.GetTimeDiff();
		clk.Start();
  
		/* ignore first 3 transitions */
		if ( (i >= 3) && (!laststate))
		{
			if (t > 0.000050) //26 - 28 us = 0, 70 us = 1
			{
				dhtBuff[j] |= mask;
			}
			mask = (UInt8)(mask >> 1);
			if (mask == 0)
			{
				mask = 128;
				j++;
			}
		}
		i++;
	}

//	printf("Test: i = %d, j = %d, mask = %d, t = %lf\r\n", i, j, mask, t);
//	printf("%X %X %X %X %X\r\n", dhtBuff[0], dhtBuff[1], dhtBuff[2], dhtBuff[3], dhtBuff[4]);

	if ((j == 5) && (dhtBuff[4] == ( (dhtBuff[0] + dhtBuff[1] + dhtBuff[2] + dhtBuff[3]) & 0xFF) ) )
	{
		Double rhV = ReadMInt16(&dhtBuff[0]) * 0.1;
		Double tempV = ReadMInt16(&dhtBuff[2]) * 0.1;
		if (rhV >= 0 && rhV <= 100 && tempV >= -40.0 && tempV <= 80)
		{
			*rh = rhV;
			*temp = tempV;
			return true;
		}
		return false;
	}
	else
	{
		return false;
	}
}

