#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "IO/FileStream.h"
#include "IO/GPIOPin.h"
#include "IO/IOPinCapture.h"
#include "IO/StreamWriter.h"
#include "IO/Device/DHT22.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	IO::GPIOControl *gpioCtrl;
	IO::GPIOPin *pin;
	IO::IOPinCapture *pinCapture;
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
	console.WriteLineC(sb.ToString(), sb.GetLength());
	NEW_CLASS(gpioCtrl, IO::GPIOControl());
	NEW_CLASS(pin, IO::GPIOPin(gpioCtrl, pinNum));
	if (gpioCtrl->IsError() || pin->IsError())
	{
		console.WriteLineC(UTF8STRC("Error in opening pin."));
	}
	else
	{
		NEW_CLASS(pinCapture, IO::IOPinCapture(pin));
		NEW_CLASS(dht22, IO::Device::DHT22(pinCapture));
		if (dht22->ReadData(&temp, &rh))
		{
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Temp = "));
			Text::SBAppendF64(&sb, temp);
			sb.AppendC(UTF8STRC(", RH = "));
			Text::SBAppendF64(&sb, rh);
			sb.AppendC(UTF8STRC("%"));
			console.WriteLineC(sb.ToString(), sb.GetLength());
		}
		else
		{
			console.WriteLineC(UTF8STRC("Error in reading from DHT22"));
		}

		Data::ArrayList<Double> times;
		Data::ArrayList<Int32> isHighs;
		IO::FileStream *fs;
		IO::StreamWriter *writer;
		UOSInt i;
		UOSInt j;
		if (pinCapture->GetCaptureData(&times, &isHighs) > 0)
		{
			NEW_CLASS(fs, IO::FileStream((const UTF8Char*)"Capture.csv", IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
			NEW_CLASS(writer, IO::StreamWriter(fs, 65001));
			writer->WriteLineC(UTF8STRC("Time, State"));
			i = 0;
			j = times.GetCount();
			while (i < j)
			{
				sb.ClearStr();
				Text::SBAppendF64(&sb, times.GetItem(i));
				sb.AppendC(UTF8STRC(", "));
				sb.AppendI32(isHighs.GetItem(i));
				writer->WriteLineC(sb.ToString(), sb.GetLength());
				i++;
			}		
			DEL_CLASS(writer);
			DEL_CLASS(fs);
		}

		DEL_CLASS(dht22);
		DEL_CLASS(pinCapture);
	}
	DEL_CLASS(pin);
	DEL_CLASS(gpioCtrl);
	return 0;
}
