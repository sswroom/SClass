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
void __stdcall OnGPSPos(void *userObj, Map::GPSTrack::GPSRecord3 *record, UOSInt sateCnt, Map::ILocationService::SateStatus *sates)
{
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("Lat = "));
	Text::SBAppendF64(&sb, record->pos.lat);
	sb.AppendC(UTF8STRC(", Lon = "));
	Text::SBAppendF64(&sb, record->pos.lon);
	sb.AppendC(UTF8STRC(", Altitude = "));
	Text::SBAppendF64(&sb, record->altitude);
	sb.AppendC(UTF8STRC(", Speed = "));
	Text::SBAppendF64(&sb, record->speed);
	sb.AppendC(UTF8STRC(", Heading = "));
	Text::SBAppendF64(&sb, record->heading);
	sb.AppendC(UTF8STRC(", nSateUsedGPS = "));
	sb.AppendI32(record->nSateUsedGPS);
	sb.AppendC(UTF8STRC(", nSateViewGPS = "));
	sb.AppendI32(record->nSateViewGPS);
	console->WriteLineC(sb.ToString(), sb.GetLength());
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
	sb.AppendC(UTF8STRC("Run using GPIO pin "));
	sb.AppendI32(pinNum);
	console->WriteLineC(sb.ToString(), sb.GetLength());

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
