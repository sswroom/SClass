#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "IO/GPIOPin.h"
#include "IO/Device/AM2315GPIO.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

Int32 MyMain(NotNullPtr<Core::IProgControl> progCtrl)
{
	IO::GPIOPin *sdaPin;
	IO::GPIOPin *sclPin;
	IO::Device::AM2315GPIO *am2315;
	IO::ConsoleWriter console;
	Double temp;
	Double rh;
	Text::StringBuilderUTF8 sb;
	UInt16 pinNum1 = 15;
	UInt16 pinNum2 = 14;
	UOSInt argc;
	UTF8Char **argv = progCtrl->GetCommandLines(progCtrl, argc);
	if (argc >= 3)
	{
		Text::StrToUInt16(argv[1], pinNum1);
		Text::StrToUInt16(argv[1], pinNum2);
	}
	Sync::ThreadUtil::SetPriority(Sync::ThreadUtil::TP_REALTIME);
	sb.AppendC(UTF8STRC("Run using GPIO pin "));
	sb.AppendI32(pinNum1);
	sb.AppendC(UTF8STRC(" (SDA) and "));
	sb.AppendI32(pinNum2);
	sb.AppendC(UTF8STRC(" (SCL)"));
	console.WriteLineC(sb.ToString(), sb.GetLength());
	IO::GPIOControl gpioCtrl;
	NEW_CLASS(sdaPin, IO::GPIOPin(gpioCtrl, pinNum1));
	NEW_CLASS(sclPin, IO::GPIOPin(gpioCtrl, pinNum2));
	NEW_CLASS(am2315, IO::Device::AM2315GPIO(sdaPin, sclPin));
	if (gpioCtrl.IsError() || sdaPin->IsError() || sclPin->IsError())
	{
		console.WriteLineC(UTF8STRC("Error in opening GPIO, root permission?"));
	}
	else
	{
		while (true)
		{
			if (am2315->ReadTemperature(&temp))
			{
				sb.ClearStr();
				sb.AppendC(UTF8STRC("Temp = "));
				sb.AppendDouble(temp);
				console.WriteLineC(sb.ToString(), sb.GetLength());

				if (am2315->ReadRH(&rh))
				{
					sb.ClearStr();
					sb.AppendC(UTF8STRC("RH = "));
					sb.AppendDouble(rh);
					sb.AppendC(UTF8STRC("%"));
					console.WriteLineC(sb.ToString(), sb.GetLength());
				}
				else
				{
					console.WriteLineC(UTF8STRC("Error in reading from AM2315"));
				}
			}
			else
			{
				console.WriteLineC(UTF8STRC("Error in reading from AM2315"));
			}
			Sync::SimpleThread::Sleep(2000);
		}
	}
	DEL_CLASS(am2315);
	DEL_CLASS(sdaPin);
	DEL_CLASS(sclPin);
	return 0;
}
