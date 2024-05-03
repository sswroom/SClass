#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "IO/FileStream.h"
#include "IO/GPIOPin.h"
#include "IO/IOPinCapture.h"
#include "IO/StreamWriter.h"
#include "IO/Device/DHT22.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Writer.h"

Int32 MyMain(NN<Core::IProgControl> progCtrl)
{
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
	console.WriteLine(sb.ToCString());
	IO::GPIOControl gpioCtrl;
	IO::GPIOPin pin(gpioCtrl, pinNum);
	if (gpioCtrl.IsError() || pin.IsError())
	{
		console.WriteLine(CSTR("Error in opening pin."));
	}
	else
	{
		IO::IOPinCapture pinCapture(pin);
		IO::Device::DHT22 dht22(pinCapture);
		if (dht22.ReadData(temp, rh))
		{
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Temp = "));
			sb.AppendDouble(temp);
			sb.AppendC(UTF8STRC(", RH = "));
			sb.AppendDouble(rh);
			sb.AppendC(UTF8STRC("%"));
			console.WriteLine(sb.ToCString());
		}
		else
		{
			console.WriteLine(CSTR("Error in reading from DHT22"));
		}

		Data::ArrayList<Double> times;
		Data::ArrayList<Int32> isHighs;
		UOSInt i;
		UOSInt j;
		if (pinCapture.GetCaptureData(&times, &isHighs) > 0)
		{
			IO::FileStream fs(CSTR("Capture.csv"), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
			Text::UTF8Writer writer(fs);
			writer.WriteLine(CSTR("Time, State"));
			i = 0;
			j = times.GetCount();
			while (i < j)
			{
				sb.ClearStr();
				sb.AppendDouble(times.GetItem(i));
				sb.AppendC(UTF8STRC(", "));
				sb.AppendI32(isHighs.GetItem(i));
				writer.WriteLine(sb.ToCString());
				i++;
			}		
		}
	}
	return 0;
}
