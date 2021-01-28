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
	Int32 pinNum = 7;
	OSInt argc;
	UTF8Char **argv = progCtrl->GetCommandLines(progCtrl, &argc);
	if (argc >= 2)
	{
		Text::StrToInt32(argv[1], &pinNum);
	}
	Sync::Thread::SetPriority(Sync::Thread::TP_REALTIME);
	sb.Append((const UTF8Char*)"Run using GPIO pin ");
	sb.AppendI32(pinNum);
	console.WriteLine(sb.ToString());
	NEW_CLASS(gpioCtrl, IO::GPIOControl());
	NEW_CLASS(pin, IO::GPIOPin(gpioCtrl, pinNum));
	if (gpioCtrl->IsError() || pin->IsError())
	{
		console.WriteLine((const UTF8Char*)"Error in opening pin.");
	}
	else
	{
		NEW_CLASS(pinCapture, IO::IOPinCapture(pin));
		NEW_CLASS(dht22, IO::Device::DHT22(pinCapture));
		if (dht22->ReadData(&temp, &rh))
		{
			sb.ClearStr();
			sb.Append((const UTF8Char*)"Temp = ");
			Text::SBAppendF64(&sb, temp);
			sb.Append((const UTF8Char*)", RH = ");
			Text::SBAppendF64(&sb, rh);
			sb.Append((const UTF8Char*)"%");
			console.WriteLine(sb.ToString());
		}
		else
		{
			console.WriteLine((const UTF8Char*)"Error in reading from DHT22");
		}

		Data::ArrayList<Double> times;
		Data::ArrayList<Int32> isHighs;
		IO::FileStream *fs;
		IO::StreamWriter *writer;
		OSInt i;
		OSInt j;
		if (pinCapture->GetCaptureData(&times, &isHighs) > 0)
		{
			NEW_CLASS(fs, IO::FileStream((const UTF8Char*)"Capture.csv", IO::FileStream::FILE_MODE_CREATE, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
			NEW_CLASS(writer, IO::StreamWriter(fs, 65001));
			writer->WriteLine((const UTF8Char*)"Time, State");
			i = 0;
			j = times.GetCount();
			while (i < j)
			{
				sb.ClearStr();
				Text::SBAppendF64(&sb, times.GetItem(i));
				sb.Append((const UTF8Char*)", ");
				sb.AppendI32(isHighs.GetItem(i));
				writer->WriteLine(sb.ToString());
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
