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

Int32 MyMain(NotNullPtr<Core::IProgControl> progCtrl)
{
	IO::OneWireGPIO *oneWire;
	IO::Device::DS18B20 *sensor;
	IO::ConsoleWriter console;
	Double temp;
	Text::StringBuilderUTF8 sb;
	UInt8 sensorId[7];
	UInt16 pinNum = 7;
	UOSInt argc;
	UTF8Char **argv = progCtrl->GetCommandLines(progCtrl, argc);
	if (argc >= 2)
	{
		Text::StrToUInt16(argv[1], pinNum);
	}
	Sync::ThreadUtil::SetPriority(Sync::ThreadUtil::TP_REALTIME);
	sb.AppendC(UTF8STRC("Run using GPIO pin "));
	sb.AppendI32(pinNum);
	console.WriteLineC(sb.ToString(), sb.GetLength());
	IO::GPIOControl gpioCtrl;
	IO::GPIOPin pin(gpioCtrl, pinNum);
	NEW_CLASS(oneWire, IO::OneWireGPIO(pin));
	NEW_CLASS(sensor, IO::Device::DS18B20(oneWire));
	if (gpioCtrl.IsError() || pin.IsError())
	{
		console.WriteLineC(UTF8STRC("Error in opening GPIO, root permission?"));
	}
	else if (!sensor->ReadSensorID(sensorId))
	{
		console.WriteLineC(UTF8STRC("Sensor not found"));
	}
	else
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("Sensor Id = "));
		sb.AppendHex8(sensorId[0]);
		sb.AppendC(UTF8STRC("-"));
		sb.AppendHexBuff(&sensorId[1], 6, 0, Text::LineBreakType::None);
		console.WriteLineC(sb.ToString(), sb.GetLength());

		while (true)
		{
			if (!sensor->ConvTemp())
			{
				console.WriteLineC(UTF8STRC("Error in converting temperature"));
			}
			else if (!sensor->ReadTemp(&temp))
			{
				console.WriteLineC(UTF8STRC("Error in reading temperature"));
			}
			else
			{
				sb.ClearStr();
				sb.AppendC(UTF8STRC("Temperature = "));
				sb.AppendDouble(temp);
				console.WriteLineC(sb.ToString(), sb.GetLength());
			}
			Sync::SimpleThread::Sleep(3000);
		}
	}

	DEL_CLASS(sensor);
	DEL_CLASS(oneWire);
	return 0;
}
