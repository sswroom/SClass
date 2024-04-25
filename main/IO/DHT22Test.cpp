#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "IO/GPIOPin.h"
#include "IO/Device/DHT22.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

Int32 MyMain(NN<Core::IProgControl> progCtrl)
{
	NN<IO::GPIOPin> pin;
	IO::Device::DHT22 *dht22;
	IO::ConsoleWriter console;
	Double temp;
	Double rh;
	Text::StringBuilderUTF8 sb;
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
	NEW_CLASSNN(pin, IO::GPIOPin(gpioCtrl, pinNum));
	NEW_CLASS(dht22, IO::Device::DHT22(pin));
	if (gpioCtrl.IsError() || pin->IsError())
	{
		console.WriteLineC(UTF8STRC("Error in opening GPIO, root permission?"));
	}
	else
	{
		while (true)
		{
			if (dht22->ReadData(temp, rh))
			{
				sb.ClearStr();
				sb.AppendC(UTF8STRC("Temp = "));
				sb.AppendDouble(temp);
				sb.AppendC(UTF8STRC(", RH = "));
				sb.AppendDouble(rh);
				sb.AppendC(UTF8STRC("%"));
				console.WriteLineC(sb.ToString(), sb.GetLength());
			}
			else
			{
				console.WriteLineC(UTF8STRC("Error in reading from DHT22"));
			}
			Sync::SimpleThread::Sleep(2000);
		}
	}
	DEL_CLASS(dht22);
	pin.Delete();
	return 0;
}
