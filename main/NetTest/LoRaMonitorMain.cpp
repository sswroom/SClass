#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "Net/LoRaMonitorCore.h"
#include "Net/OSSocketFactory.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	IO::ConsoleWriter console;
	Net::OSSocketFactory sockf(true);
	Net::LoRaMonitorCore lora(sockf, 1700, 6080);
	if (!lora.IsError())
	{
		console.WriteLine(CSTR("Started LoRa Monitor"));
		progCtrl->WaitForExit(progCtrl);
		console.WriteLine(CSTR("Stopping LoRa Monitor"));
	}
	return 0;
}
