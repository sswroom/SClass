#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "IO/GPIOPin.h"
#include "IO/Device/AM2315GPIO.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	IO::GPIOPin *sdaPin;
	IO::GPIOPin *sclPin;
	IO::Device::AM2315GPIO *am2315;
	IO::ConsoleWriter console;
	IO::GPIOControl *gpioCtrl;
	Double temp;
	Double rh;
	Text::StringBuilderUTF8 sb;
	UInt16 pinNum1 = 15;
	UInt16 pinNum2 = 14;
	UOSInt argc;
	UTF8Char **argv = progCtrl->GetCommandLines(progCtrl, &argc);
	if (argc >= 3)
	{
		Text::StrToUInt16(argv[1], &pinNum1);
		Text::StrToUInt16(argv[1], &pinNum2);
	}
	Sync::Thread::SetPriority(Sync::Thread::TP_REALTIME);
	sb.AppendC(UTF8STRC("Run using GPIO pin "));
	sb.AppendI32(pinNum1);
	sb.AppendC(UTF8STRC(" (SDA) and "));
	sb.AppendI32(pinNum2);
	sb.AppendC(UTF8STRC(" (SCL)"));
	console.WriteLine(sb.ToString());
	NEW_CLASS(gpioCtrl, IO::GPIOControl())
	NEW_CLASS(sdaPin, IO::GPIOPin(gpioCtrl, pinNum1));
	NEW_CLASS(sclPin, IO::GPIOPin(gpioCtrl, pinNum2));
	NEW_CLASS(am2315, IO::Device::AM2315GPIO(sdaPin, sclPin));
	if (gpioCtrl->IsError() || sdaPin->IsError() || sclPin->IsError())
	{
		console.WriteLine((const UTF8Char*)"Error in opening GPIO, root permission?");
	}
	else
	{
		while (true)
		{
			if (am2315->ReadTemperature(&temp))
			{
				sb.ClearStr();
				sb.AppendC(UTF8STRC("Temp = "));
				Text::SBAppendF64(&sb, temp);
				console.WriteLine(sb.ToString());

				if (am2315->ReadRH(&rh))
				{
					sb.ClearStr();
					sb.AppendC(UTF8STRC("RH = "));
					Text::SBAppendF64(&sb, rh);
					sb.AppendC(UTF8STRC("%"));
					console.WriteLine(sb.ToString());
				}
				else
				{
					console.WriteLine((const UTF8Char*)"Error in reading from AM2315");
				}
			}
			else
			{
				console.WriteLine((const UTF8Char*)"Error in reading from AM2315");
			}
			Sync::Thread::Sleep(2000);
		}
	}
	DEL_CLASS(am2315);
	DEL_CLASS(sdaPin);
	DEL_CLASS(sclPin);
	DEL_CLASS(gpioCtrl);
	return 0;
}
