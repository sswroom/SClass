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
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

IO::ConsoleWriter *console;
void __stdcall OnGPSPos(AnyType userObj, NN<Map::GPSTrack::GPSRecord3> record, Data::DataArray<Map::LocationService::SateStatus> sates)
{
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("Lat = "));
	sb.AppendDouble(record->pos.GetLat());
	sb.AppendC(UTF8STRC(", Lon = "));
	sb.AppendDouble(record->pos.GetLon());
	sb.AppendC(UTF8STRC(", Altitude = "));
	sb.AppendDouble(record->altitude);
	sb.AppendC(UTF8STRC(", Speed = "));
	sb.AppendDouble(record->speed);
	sb.AppendC(UTF8STRC(", Heading = "));
	sb.AppendDouble(record->heading);
	sb.AppendC(UTF8STRC(", nSateUsedGPS = "));
	sb.AppendI32(record->nSateUsedGPS);
	sb.AppendC(UTF8STRC(", nSateViewGPS = "));
	sb.AppendI32(record->nSateViewGPS);
	console->WriteLine(sb.ToCString());
}

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	NN<IO::GPIOControl> gpio;
	NN<IO::RS232GPIO> port;
	IO::GPSNMEA *gps;
	Text::StringBuilderUTF8 sb;
	UInt16 pinNum = 7;
	UOSInt argc;
	NEW_CLASS(console, IO::ConsoleWriter());
	UnsafeArray<UnsafeArray<UTF8Char>> argv = progCtrl->GetCommandLines(progCtrl, argc);
	if (argc >= 2)
	{
		Text::StrToUInt16(argv[1], pinNum);
	}
	Sync::ThreadUtil::SetPriority(Sync::ThreadUtil::TP_REALTIME);
	sb.AppendC(UTF8STRC("Run using GPIO pin "));
	sb.AppendI32(pinNum);
	console->WriteLine(sb.ToCString());

	NEW_CLASSNN(gpio, IO::GPIOControl());
	NEW_CLASSNN(port, IO::RS232GPIO(gpio, pinNum, 255, 9600));
	NEW_CLASS(gps, IO::GPSNMEA(port, false));
	gps->RegisterLocationHandler(OnGPSPos, 0);

	progCtrl->WaitForExit(progCtrl);

	DEL_CLASS(gps);
	port.Delete();
	gpio.Delete();
	DEL_CLASS(console);
	return 0;
}
