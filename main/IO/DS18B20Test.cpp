#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "IO/GPIOPin.h"
#include "IO/OneWireGPIO.h"
#include "IO/Device/DS18B20.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	NN<IO::OneWireGPIO> oneWire;
	NN<IO::Device::DS18B20> sensor;
	IO::ConsoleWriter console;
	Double temp;
	Text::StringBuilderUTF8 sb;
	UInt8 sensorId[7];
	UInt16 pinNum = 7;
	UIntOS argc;
	UnsafeArray<UnsafeArray<UTF8Char>> argv = progCtrl->GetCommandLines(progCtrl, argc);
	if (argc >= 2)
	{
		Text::StrToUInt16(argv[1], pinNum);
	}
	Sync::ThreadUtil::SetPriority(Sync::ThreadUtil::TP_REALTIME);
	sb.AppendC(UTF8STRC("Run using GPIO pin "));
	sb.AppendI32(pinNum);
	console.WriteLine(sb.ToCString());
	IO::GPIOControl gpioCtrl;
	IO::GPIOPin pin(gpioCtrl, pinNum);
	NEW_CLASSNN(oneWire, IO::OneWireGPIO(pin));
	NEW_CLASSNN(sensor, IO::Device::DS18B20(oneWire));
	if (gpioCtrl.IsError() || pin.IsError())
	{
		console.WriteLine(CSTR("Error in opening GPIO, root permission?"));
	}
	else if (!sensor->ReadSensorID(sensorId))
	{
		console.WriteLine(CSTR("Sensor not found"));
	}
	else
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("Sensor Id = "));
		sb.AppendHex8(sensorId[0]);
		sb.AppendC(UTF8STRC("-"));
		sb.AppendHexBuff(&sensorId[1], 6, 0, Text::LineBreakType::None);
		console.WriteLine(sb.ToCString());

		while (true)
		{
			if (!sensor->ConvTemp())
			{
				console.WriteLine(CSTR("Error in converting temperature"));
			}
			else if (!sensor->ReadTemp(temp))
			{
				console.WriteLine(CSTR("Error in reading temperature"));
			}
			else
			{
				sb.ClearStr();
				sb.AppendC(UTF8STRC("Temperature = "));
				sb.AppendDouble(temp);
				console.WriteLine(sb.ToCString());
			}
			Sync::SimpleThread::Sleep(3000);
		}
	}

	sensor.Delete();
	oneWire.Delete();
	return 0;
}
