#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "IO/Device/BME280.h"
#include "Sync/SimpleThread.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

Int32 MyMain(NN<Core::IProgControl> progCtrl)
{
	IO::Device::BME280 *bme280;
	IO::ConsoleWriter console;
	NN<IO::I2CChannel> channel;
	if (!IO::Device::BME280::CreateDefChannel(1).SetTo(channel))
	{
		console.WriteLine(CSTR("I2C bus not found"));
	}
	else
	{
		NEW_CLASS(bme280, IO::Device::BME280(channel, true));
		if (bme280->IsError())
		{
			console.WriteLine(CSTR("BME280 not found"));
		}
		else
		{
			Sync::SimpleThread::Sleep(1000);
			OSInt i = 10;
			while (i-- > 0)
			{
				Double temp;
				Double rh;
				Double pressure;
				if (!bme280->ReadAll(temp, rh, pressure))
				{
					console.WriteLine(CSTR("Error in reading all"));
				}
				else
				{
					Text::StringBuilderUTF8 sb;
					sb.AppendC(UTF8STRC("Temp = "));
					sb.AppendDouble(temp);
					sb.AppendC(UTF8STRC(", RH = "));
					sb.AppendDouble(rh);
					sb.AppendC(UTF8STRC(", Pressure = "));
					sb.AppendDouble(pressure);
					console.WriteLine(sb.ToCString());
				}
				Sync::SimpleThread::Sleep(2000);
			}
		}
		DEL_CLASS(bme280);
	}
	return 0;
}
