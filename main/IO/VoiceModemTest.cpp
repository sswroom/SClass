#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Data/ArrayListNative.hpp"
#include "IO/ConsoleWriter.h"
#include "IO/ModemController.h"
#include "IO/SerialPort.h"
#include "IO/Device/RockwellModemController.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	UIntOS i;
	UIntOS j;
	IO::ConsoleWriter console;
	Text::StringBuilderUTF8 sb;
	Data::ArrayListNative<UIntOS> ports;
	Data::ArrayListNative<IO::SerialPort::SerialPortType> portTypes;
	IO::SerialPort::GetAvailablePorts(ports, &portTypes);
	console.WriteLine(CSTR("Available Serial Ports:"));
	i = 0;
	j = ports.GetCount();
	while (i < j)
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("Port "));
		sb.AppendUIntOS(ports.GetItem(i));
		sb.AppendC(UTF8STRC(" - "));
		sb.Append(IO::SerialPort::SerialPortTypeGetName(portTypes.GetItem(i)));
		console.WriteLine(sb.ToCString());
		i++;
	}
	UIntOS portNum = ports.GetItem(0);

	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	UIntOS argc;
	UnsafeArray<UnsafeArray<UTF8Char>> argv = progCtrl->GetCommandLines(progCtrl, argc);
	if (argc >= 2)
	{
		Text::StrToUIntOS(argv[1], portNum);
	}
	console.WriteLine();
	sb.ClearStr();
	sb.AppendC(UTF8STRC("Trying Port "));
	sb.AppendUIntOS(portNum);
	console.WriteLine(sb.ToCString());

	NN<IO::SerialPort> port;
	NEW_CLASSNN(port, IO::SerialPort(portNum, 115200, IO::SerialPort::PARITY_NONE, true));
	if (port->IsError())
	{
		console.WriteLine(CSTR("Error in opening the port"));
	}
	else
	{
		NN<IO::ATCommandChannel> channel;
		NN<IO::Device::RockwellModemController> modem;
		Int32 v;
		NEW_CLASSNN(channel, IO::ATCommandChannel(port, false));
		NEW_CLASSNN(modem, IO::Device::RockwellModemController(channel, false));
		if (modem->VoiceGetManufacturer(sbuff).SetTo(sptr))
		{
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Manufacturer: "));
			sb.AppendP(sbuff, sptr);
			console.WriteLine(sb.ToCString());
		}
		else
		{
			console.WriteLine(CSTR("Manufacturer: Error in getting the value"));
		}

		if (modem->VoiceGetModel(sbuff).SetTo(sptr))
		{
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Model: "));
			sb.AppendP(sbuff, sptr);
			console.WriteLine(sb.ToCString());
		}
		else
		{
			console.WriteLine(CSTR("Model: Error in getting the value"));
		}

		if (modem->VoiceGetRevision(sbuff).SetTo(sptr))
		{
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Revision: "));
			sb.AppendP(sbuff, sptr);
			console.WriteLine(sb.ToCString());
		}
		else
		{
			console.WriteLine(CSTR("Revision: Error in getting the value"));
		}

		if (modem->VoiceGetBaudRate(v))
		{
			sb.ClearStr();
			sb.AppendC(UTF8STRC("BaudRate: "));
			sb.AppendI32(v);
			sb.AppendC(UTF8STRC("bps"));
			console.WriteLine(sb.ToCString());
		}
		else
		{
			console.WriteLine(CSTR("BaudRate: Error in getting the value"));
		}

		IO::Device::RockwellModemController::CallerIDType callerIDType;
		if (modem->VoiceGetCallerIDType(callerIDType))
		{
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Caller ID Type: "));
			sb.Append(IO::Device::RockwellModemController::CallerIDTypeGetName(callerIDType));
			console.WriteLine(sb.ToCString());
		}
		else
		{
			console.WriteLine(CSTR("Caller ID Type: Error in getting the value"));
		}

		IO::Device::RockwellModemController::VoiceType voiceType;
		if (modem->VoiceGetType(voiceType))
		{
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Voice Type: "));
			sb.Append(IO::Device::RockwellModemController::VoiceTypeGetName(voiceType));
			console.WriteLine(sb.ToCString());
		}
		else
		{
			console.WriteLine(CSTR("Voice Type: Error in getting the value"));
		}

		if (modem->VoiceGetBufferSize(v))
		{
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Buffer Size: "));
			sb.AppendI32(v);
			console.WriteLine(sb.ToCString());
		}
		else
		{
			console.WriteLine(CSTR("Buffer Size: Error in getting the value"));
		}

		if (modem->VoiceGetBitsPerSample(v))
		{
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Bits Per Sample: "));
			sb.AppendI32(v);
			console.WriteLine(sb.ToCString());
		}
		else
		{
			console.WriteLine(CSTR("Bits Per Sample: Error in getting the value"));
		}

		if (modem->VoiceGetToneDur(v))
		{
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Tone Duration: "));
			sb.AppendDouble(v / 1000.0);
			sb.AppendC(UTF8STRC("s"));
			console.WriteLine(sb.ToCString());
		}
		else
		{
			console.WriteLine(CSTR("Tone Duration: Error in getting the value"));
		}

		if (modem->VoiceGetCompression(sbuff).SetTo(sptr))
		{
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Compression: "));
			sb.AppendP(sbuff, sptr);
			console.WriteLine(sb.ToCString());
		}
		else
		{
			console.WriteLine(CSTR("Compression: Error in getting the value"));
		}

		IO::Device::RockwellModemController::VoiceLineType voiceLineType;
		if (modem->VoiceGetVoiceLineType(voiceLineType))
		{
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Voice Line Type: "));
			sb.Append(IO::Device::RockwellModemController::VoiceLineTypeGetName(voiceLineType));
			console.WriteLine(sb.ToCString());
		}
		else
		{
			console.WriteLine(CSTR("Voice Line Type: Error in getting the value"));
		}

		modem.Delete();
		channel.Delete();
	}
	port.Delete();
	return 0;
}
