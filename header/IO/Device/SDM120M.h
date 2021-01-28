#ifndef _SM_IO_DEVICE_SDM120M
#define _SM_IO_DEVICE_SDM120M
#include "IO/MODBUSDevice.h"

namespace IO
{
	namespace Device
	{
		class SDM120M : public IO::MODBUSDevice
		{
		public:
			SDM120M(IO::MODBUSMaster *modbus, UInt8 addr);
			virtual ~SDM120M();

			Bool ReadVoltage(Double *volt);
			Bool ReadCurrent(Double *amps);
			Bool ReadActivePower(Double *watt);
			Bool ReadApparentPower(Double *voltAmps);
			Bool ReadReactivePower(Double *var);
			Bool ReadPowerFactor(Double *ratio);
			Bool ReadPhaseAngle(Double *degree); //CT: no this field
			Bool ReadFrequency(Double *hz);
			Bool ReadImportActiveEnergy(Double *kwh);
			Bool ReadExportActiveEnergy(Double *kwh);
			Bool ReadImportReactiveEnergy(Double *kvarh); //CT: no this field
			Bool ReadExportReactiveEnergy(Double *kvarh); //CT: no this field
			Bool ReadTotalActiveEnergy(Double *kwh);
			Bool ReadTotalReactiveEnergy(Double *kvarh); //CT: no this field

			Bool ReadRelayPulseWidth(Int32 *ms); //CT: no this field
			Bool ReadParityStop(Int32 *parity); //CT: no this field
			Bool ReadNetworkNode(Int32 *addr);
			Bool ReadBaudRate(Int32 *baudRate);

			Bool SetRelayPulseWidth(Single ms); //CT: no this field
			Bool SetNetworkNode(UInt8 id);
			Bool SetBaudRate(Int32 baudRate);

			static void GetDataEntries(UInt8 addr, MODBUSDataEntry dataHdlr, void *userObj);
		};
	}
}
#endif
