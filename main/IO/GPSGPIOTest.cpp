#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "IO/FileStream.h"
#include "IO/GPIOPin.h"
#include "IO/GPSNMEA.h"
#include "IO/NullIOPin.h"
#include "IO/RS232GPIO.h"
#include "IO/StreamReader.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

IO::ConsoleWriter *console;
void __stdcall OnGPSPos(void *userObj, Map::GPSTrack::GPSRecord *record)
{
	Text::StringBuilderUTF8 sb;
	sb.Append((const UTF8Char*)"Lat = ");
	Text::SBAppendF64(&sb, record->lat);
	sb.Append((const UTF8Char*)", Lon = ");
	Text::SBAppendF64(&sb, record->lon);
	sb.Append((const UTF8Char*)", Altitude = ");
	Text::SBAppendF64(&sb, record->altitude);
	sb.Append((const UTF8Char*)", Speed = ");
	Text::SBAppendF64(&sb, record->speed);
	sb.Append((const UTF8Char*)", Heading = ");
	Text::SBAppendF64(&sb, record->heading);
	sb.Append((const UTF8Char*)", nSateUsed = ");
	sb.AppendI32(record->nSateUsed);
	sb.Append((const UTF8Char*)", nSateView = ");
	sb.AppendI32(record->nSateView);
	console->WriteLine(sb.ToString());
}

Int32 MyMain(Core::IProgControl *progCtrl)
{
	IO::GPIOControl *gpio;
	IO::RS232GPIO *port;
	IO::GPSNMEA *gps;
	Text::StringBuilderUTF8 sb;
	UInt16 pinNum = 7;
	UOSInt argc;
	NEW_CLASS(console, IO::ConsoleWriter());
	UTF8Char **argv = progCtrl->GetCommandLines(progCtrl, &argc);
	if (argc >= 2)
	{
		Text::StrToUInt16(argv[1], &pinNum);
	}
	Sync::Thread::SetPriority(Sync::Thread::TP_REALTIME);
	sb.Append((const UTF8Char*)"Run using GPIO pin ");
	sb.AppendI32(pinNum);
	console->WriteLine(sb.ToString());

	NEW_CLASS(gpio, IO::GPIOControl());
	NEW_CLASS(port, IO::RS232GPIO(gpio, pinNum, 255, 9600));
	NEW_CLASS(gps, IO::GPSNMEA(port, false));
	gps->RegisterLocationHandler(OnGPSPos, 0);

	progCtrl->WaitForExit(progCtrl);

	DEL_CLASS(gps);
	DEL_CLASS(port);
	DEL_CLASS(gpio);
	DEL_CLASS(console);
	return 0;
}
