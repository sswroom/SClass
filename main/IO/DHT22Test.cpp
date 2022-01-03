#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "IO/GPIOPin.h"
#include "IO/Device/DHT22.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	IO::GPIOControl *gpioCtrl;
	IO::GPIOPin *pin;
	IO::Device::DHT22 *dht22;
	IO::ConsoleWriter console;
	Double temp;
	Double rh;
	Text::StringBuilderUTF8 sb;
	UInt16 pinNum = 7;
	UOSInt argc;
	UTF8Char **argv = progCtrl->GetCommandLines(progCtrl, &argc);
	if (argc >= 2)
	{
		Text::StrToUInt16(argv[1], &pinNum);
	}
	Sync::Thread::SetPriority(Sync::Thread::TP_REALTIME);
	sb.AppendC(UTF8STRC("Run using GPIO pin "));
	sb.AppendI32(pinNum);
	console.WriteLine(sb.ToString());
	NEW_CLASS(gpioCtrl, IO::GPIOControl())
	NEW_CLASS(pin, IO::GPIOPin(gpioCtrl, pinNum));
	NEW_CLASS(dht22, IO::Device::DHT22(pin));
	if (gpioCtrl->IsError() || pin->IsError())
	{
		console.WriteLine((const UTF8Char*)"Error in opening GPIO, root permission?");
	}
	else
	{
		while (true)
		{
			if (dht22->ReadData(&temp, &rh))
			{
				sb.ClearStr();
				sb.AppendC(UTF8STRC("Temp = "));
				Text::SBAppendF64(&sb, temp);
				sb.AppendC(UTF8STRC(", RH = "));
				Text::SBAppendF64(&sb, rh);
				sb.AppendC(UTF8STRC("%"));
				console.WriteLine(sb.ToString());
			}
			else
			{
				console.WriteLine((const UTF8Char*)"Error in reading from DHT22");
			}
			Sync::Thread::Sleep(2000);
		}
	}
	DEL_CLASS(dht22);
	DEL_CLASS(pin);
	DEL_CLASS(gpioCtrl);
	return 0;
}
