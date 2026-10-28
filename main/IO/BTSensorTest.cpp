#include "Stdafx.h"
#include "Core/ByteTool_C.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "IO/BTScanner.h"
#include "IO/RAWBTMonitor.h"
#include "Sync/Thread.h"
#include "Text/StringBuilderUTF8.h"

class BTSensorReader
{
private:
	IO::RAWBTMonitor monitor;
	Sync::Thread thread;
	IO::ConsoleWriter console;

	static void __stdcall ScanThread(NN<Sync::Thread> thread)
	{
		NN<BTSensorReader> me = thread->GetUserObj().GetNN<BTSensorReader>();
		UnsafeArray<UInt8> packet = MemAllocArr(UInt8, me->monitor.GetMTU());
		Int64 timeTicks;
		while (!thread->IsStopping())
		{
			UIntOS packetSize = me->monitor.NextPacket(packet, timeTicks);
			if (packetSize > 0)
			{
				me->ProcessPacket(Data::ByteArrayR(packet, packetSize));
			}
		}
		MemFreeArr(packet);
	}

	void ProcessAdvertisement(Data::ByteArrayR packet, UIntOS addrOfst, UIntOS dataOfst, UIntOS dataLen, Int8 rssi)
	{
		if (addrOfst + 6 > packet.GetSize() || dataOfst + dataLen > packet.GetSize())
			return;

		UInt8 mac[6];
		UIntOS i = 0;
		while (i < 6)
		{
			mac[i] = packet[addrOfst + 5 - i];
			i++;
		}

		UInt16 companyId = 0;
		Bool hasCompanyId = false;
		const UInt8 *manufacturerData = 0;
		UIntOS manufacturerLen = 0;
		const UInt8 *serviceData = 0;
		UIntOS serviceLen = 0;
		UInt16 serviceUuid = 0;
		i = dataOfst;
		UIntOS dataEnd = dataOfst + dataLen;
		while (i + 1 < dataEnd)
		{
			UInt8 fieldLen = packet[i];
			if (fieldLen == 0 || i + 1 + fieldLen > dataEnd)
				break;

			UInt8 fieldType = packet[i + 1];
			const UInt8 *fieldData = &packet[i + 2];
			UIntOS fieldDataLen = fieldLen - 1;
			if (fieldType == 0xff && fieldDataLen >= 2)
			{
				companyId = ReadLUInt16(&fieldData[0]);
				hasCompanyId = true;
				manufacturerData = &fieldData[2];
				manufacturerLen = fieldDataLen - 2;
			}
			else if (fieldType == 0x16 && fieldDataLen >= 2)
			{
				serviceUuid = ReadLUInt16(&fieldData[0]);
				serviceData = &fieldData[2];
				serviceLen = fieldDataLen - 2;
			}
			i += (UIntOS)fieldLen + 1;
		}

		// Meter Pro CO2 carries model/battery in FD3D service data and readings in manufacturer data.
		if (serviceData != 0 && serviceUuid == 0xfd3d && serviceLen >= 3 &&
			((serviceData[0] & 0x7f) == '5' || (serviceData[0] & 0x7f) == 0x15) &&
			hasCompanyId && companyId == 0x0969 && manufacturerLen >= 15)
		{
			Int32 temperatureWhole = manufacturerData[9] & 0x7f;
			Int32 temperatureFraction = manufacturerData[8] & 0x0f;
			Int32 temperatureSign = (manufacturerData[9] & 0x80) ? 1 : -1;
			Int32 humidity = manufacturerData[10] & 0x7f;
			UInt16 co2 = (UInt16)(((UInt16)manufacturerData[13] << 8) | manufacturerData[14]);

			Text::StringBuilderUTF8 sb;
			sb.AppendC(UTF8STRC("SwitchBot Meter Pro CO2 "));
			sb.AppendHexBuff(mac, 6, ':', Text::LineBreakType::None);
			sb.AppendC(UTF8STRC(" RSSI="));
			sb.AppendI32(rssi);
			sb.AppendC(UTF8STRC(" Temperature="));
			if (temperatureSign < 0)
				sb.AppendC(UTF8STRC("-"));
			sb.AppendI32(temperatureWhole);
			sb.AppendC(UTF8STRC("."));
			if (temperatureFraction < 10)
				sb.AppendC(UTF8STRC("0"));
			sb.AppendI32(temperatureFraction);
			sb.AppendC(UTF8STRC(" C Humidity="));
			sb.AppendI32(humidity);
			sb.AppendC(UTF8STRC("% CO2="));
			if (co2 <= 9999)
				sb.AppendU32(co2);
			else
				sb.AppendC(UTF8STRC("unavailable"));
			sb.AppendC(UTF8STRC(" ppm Battery="));
			sb.AppendI32(serviceData[2] & 0x7f);
			sb.AppendC(UTF8STRC("%"));
			this->console.WriteLine(sb.ToCString());
			return;
		}

		if (hasCompanyId || serviceData != 0)
		{
			Text::StringBuilderUTF8 sb;
			sb.AppendC(UTF8STRC("BLE advertiser "));
			sb.AppendHexBuff(mac, 6, ':', Text::LineBreakType::None);
			sb.AppendC(UTF8STRC(" RSSI="));
			sb.AppendI32(rssi);
			if (hasCompanyId)
			{
				sb.AppendC(UTF8STRC(" Company=0x"));
				sb.AppendHex16(companyId);
			}
			if (serviceData != 0)
			{
				sb.AppendC(UTF8STRC(" ServiceData="));
				sb.AppendHexBuff(serviceData, serviceLen, ' ', Text::LineBreakType::None);
			}
			this->console.WriteLine(sb.ToCString());
		}
	}

	void ProcessPacket(Data::ByteArrayR packet)
	{
		if (packet.GetSize() < 9 || ReadMUInt32(&packet[0]) != 1 || packet[4] != 4 || packet[5] != 0x3e)
			return;

		UIntOS eventEnd = (UIntOS)packet[6] + 7;
		if (eventEnd > packet.GetSize())
			eventEnd = packet.GetSize();
		UInt8 reportCount = packet[8];
		UIntOS pos = 9;
		if (packet[7] == 0x02)
		{
			while (reportCount-- > 0 && pos + 10 <= eventEnd)
			{
				UIntOS dataLen = packet[pos + 8];
				if (pos + 10 + dataLen > eventEnd)
					break;
				this->ProcessAdvertisement(packet, pos + 2, pos + 9, dataLen, (Int8)packet[pos + 9 + dataLen]);
				pos += 11 + dataLen;
			}
		}
		else if (packet[7] == 0x0d)
		{
			while (reportCount-- > 0 && pos + 24 <= eventEnd)
			{
				UIntOS dataLen = packet[pos + 23];
				if (pos + 24 + dataLen > eventEnd)
					break;
				this->ProcessAdvertisement(packet, pos + 3, pos + 24, dataLen, (Int8)packet[pos + 13]);
				pos += 24 + dataLen;
			}
		}
	}

public:
	BTSensorReader() : monitor(0), thread(ScanThread, this, CSTR("BTSensorReader"))
	{
	}

	Bool IsError()
	{
		return this->monitor.IsError();
	}

	Bool Start()
	{
		return this->thread.Start();
	}

	void Stop()
	{
		this->thread.BeginStop();
		this->monitor.Close();
		this->thread.WaitForEnd();
	}
};

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	IO::ConsoleWriter console;
	if (IO::RAWBTMonitor::GetDevCount() == 0)
	{
		console.WriteLine(CSTR("No Bluetooth adapter found"));
		return 1;
	}

	NN<IO::BTScanner> btscanner;
	if (!IO::BTScanner::CreateScanner().SetTo(btscanner))
	{
		console.WriteLine(CSTR("Unable to create BT scanner"));
		return 1;
	}

	NN<BTSensorReader> reader;
	NEW_CLASSNN(reader, BTSensorReader());
	if (reader->IsError())
	{
		console.WriteLine(CSTR("Unable to open the Bluetooth HCI monitor (check permissions)"));
		reader.Delete();
		btscanner->Close();
		btscanner.Delete();
		return 1;
	}

	btscanner->ScanOn();
	if (!reader->Start())
	{
		console.WriteLine(CSTR("Unable to start BLE scan reader"));
		btscanner->ScanOff();
		reader.Delete();
		btscanner->Close();
		btscanner.Delete();
		return 1;
	}

	console.WriteLine(CSTR("Reading nearby BLE advertisements. Press Ctrl-C to stop."));
	progCtrl->WaitForExit(progCtrl);
	reader->Stop();
	btscanner->ScanOff();
	btscanner->Close();
	reader.Delete();
	btscanner.Delete();
	return 0;
}
