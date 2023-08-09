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
			SDM120M(NotNullPtr<IO::MODBUSMaster> modbus, UInt8 addr);
			virtual ~SDM120M();

			Bool ReadVoltage(OutParam<Double> volt);
			Bool ReadCurrent(OutParam<Double> amps);
			Bool ReadActivePower(OutParam<Double> watt);
			Bool ReadApparentPower(OutParam<Double> voltAmps);
			Bool ReadReactivePower(OutParam<Double> var);
			Bool ReadPowerFactor(OutParam<Double> ratio);
			Bool ReadPhaseAngle(OutParam<Double> degree); //CT: no this field
			Bool ReadFrequency(OutParam<Double> hz);
			Bool ReadImportActiveEnergy(OutParam<Double> kwh);
			Bool ReadExportActiveEnergy(OutParam<Double> kwh);
			Bool ReadImportReactiveEnergy(OutParam<Double> kvarh); //CT: no this field
			Bool ReadExportReactiveEnergy(OutParam<Double> kvarh); //CT: no this field
			Bool ReadTotalActiveEnergy(OutParam<Double> kwh);
			Bool ReadTotalReactiveEnergy(OutParam<Double> kvarh); //CT: no this field

			Bool ReadRelayPulseWidth(OutParam<Int32> ms); //CT: no this field
			Bool ReadParityStop(OutParam<Int32> parity); //CT: no this field
			Bool ReadNetworkNode(OutParam<Int32> addr);
			Bool ReadBaudRate(OutParam<Int32> baudRate);

			Bool SetRelayPulseWidth(Single ms); //CT: no this field
			Bool SetNetworkNode(UInt8 id);
			Bool SetBaudRate(Int32 baudRate);

			static void GetDataEntries(UInt8 addr, MODBUSDataEntry dataHdlr, void *userObj);
		};
	}
}
#endif
