#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Data/ByteTool.h"
#include "IO/MODBUSRTUMaster.h"
#include "IO/SerialPort.h"
#include "IO/Device/SDM120M.h"
#include "Sync/ThreadUtil.h"
#include <stdio.h>

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	NN<IO::SerialPort> port;
	IO::Device::SDM120M *sdm120m;
	Double dval;
	Int32 ival;
	UInt32 portNum = 33;
	UInt32 baudRate = 9600;
	UInt8 addr = 2;
	UOSInt argc;
	UnsafeArray<UnsafeArray<UTF8Char>> argv = progCtrl->GetCommandLines(progCtrl, argc);
	if (argc >= 3)
	{
		Text::StrToUInt32(argv[1], portNum);
		Text::StrToUInt32(argv[2], baudRate);
	}
	if (argc >= 4)
	{
		Text::StrToUInt8(argv[3], addr);
	}

	NEW_CLASSNN(port, IO::SerialPort(portNum, baudRate, IO::SerialPort::PARITY_NONE, false));
	if (port->IsError())
	{
		printf("Error in opeining serial port\r\n");
	}
	else
	{
		IO::MODBUSRTUMaster modbus(port);
		NEW_CLASS(sdm120m, IO::Device::SDM120M(modbus, addr));

		if (sdm120m->ReadVoltage(dval))
		{
			printf("Voltage = %lfV\r\n", dval);
		}
		if (sdm120m->ReadCurrent(dval))
		{
			printf("Current = %lfA\r\n", dval);
		}
		if (sdm120m->ReadActivePower(dval))
		{
			printf("Active Power = %lfW\r\n", dval);
		}
		if (sdm120m->ReadApparentPower(dval))
		{
			printf("Apparent Power = %lfVA\r\n", dval);
		}
		if (sdm120m->ReadReactivePower(dval))
		{
			printf("Reactive Power = %lfVAr\r\n", dval);
		}
		if (sdm120m->ReadPowerFactor(dval))
		{
			printf("Power Factor = %lf\r\n", dval);
		}
		if (sdm120m->ReadPhaseAngle(dval))
		{
			printf("Phase Angle = %lfdegree\r\n", dval);
		}
		if (sdm120m->ReadFrequency(dval))
		{
			printf("Frequency = %lfHz\r\n", dval);
		}
		if (sdm120m->ReadImportActiveEnergy(dval))
		{
			printf("Import Active Energy = %lfkWh\r\n", dval);
		}
		if (sdm120m->ReadExportActiveEnergy(dval))
		{
			printf("Export Active Energy = %lfkWh\r\n", dval);
		}
		if (sdm120m->ReadImportReactiveEnergy(dval))
		{
			printf("Import Reactive Energy = %lfkVArh\r\n", dval);
		}
		if (sdm120m->ReadExportReactiveEnergy(dval))
		{
			printf("Export Reactive Energy = %lfkVArh\r\n", dval);
		}
		if (sdm120m->ReadTotalActiveEnergy(dval))
		{
			printf("Total Active Energy = %lfkWh\r\n", dval);
		}
		if (sdm120m->ReadTotalReactiveEnergy(dval))
		{
			printf("Total Reactive Energy = %lfkVArh\r\n", dval);
		}

		if (sdm120m->ReadRelayPulseWidth(ival))
		{
			printf("Relay Pulse Width = %dms\r\n", ival);
		}
		if (sdm120m->ReadParityStop(ival))
		{
			printf("Parity Stop = %d\r\n", ival);
		}
		if (sdm120m->ReadNetworkNode(ival))
		{
			printf("Network Node = %d\r\n", ival);
		}
		if (sdm120m->ReadBaudRate(ival))
		{
			printf("Baud Rate = %d\r\n", ival);
		}

		if (sdm120m->SetNetworkNode(2))
		{
			printf("Network Node set to 2\r\n");
		}
/*		if (sdm120m->SetRelayPulseWidth(60))
		{
			printf("Relay Pulse Width set to 60ms\r\n");
		}
		if (sdm120m->SetBaudRate(9600))
		{
			printf("Baud Rate set to 9600\r\n");
		}*/

		DEL_CLASS(sdm120m);
	}
	port.Delete();
	return 0;
}
