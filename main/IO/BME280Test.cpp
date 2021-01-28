#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "IO/Device/BME280.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	IO::Device::BME280 *bme280;
	IO::ConsoleWriter console;
	IO::I2CChannel *channel = IO::Device::BME280::CreateDefChannel(1);
	if (channel == 0)
	{
		console.WriteLine((const UTF8Char*)"I2C bus not found");
	}
	else
	{
		NEW_CLASS(bme280, IO::Device::BME280(channel, true));
		if (bme280->IsError())
		{
			console.WriteLine((const UTF8Char*)"BME280 not found");
		}
		else
		{
			Sync::Thread::Sleep(1000);
			OSInt i = 10;
			while (i-- > 0)
			{
				Double temp;
				Double rh;
				Double pressure;
				if (!bme280->ReadAll(&temp, &rh, &pressure))
				{
					console.WriteLine((const UTF8Char*)"Error in reading all");
				}
				else
				{
					Text::StringBuilderUTF8 sb;
					sb.Append((const UTF8Char*)"Temp = ");
					Text::SBAppendF64(&sb, temp);
					sb.Append((const UTF8Char*)", RH = ");
					Text::SBAppendF64(&sb, rh);
					sb.Append((const UTF8Char*)", Pressure = ");
					Text::SBAppendF64(&sb, pressure);
					console.WriteLine(sb.ToString());
				}
				Sync::Thread::Sleep(2000);
			}
		}
		DEL_CLASS(bme280);
	}
	return 0;
}
