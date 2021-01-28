#ifndef _SM_IO_DEVICE_MULTITEKM812
#define _SM_IO_DEVICE_MULTITEKM812
#include "IO/MODBUSDevice.h"

namespace IO
{
	namespace Device
	{
		class MultitekM812 : public IO::MODBUSDevice
		{
		public:
			MultitekM812(IO::MODBUSMaster *modbus, UInt8 addr);
			virtual ~MultitekM812();

			Bool ReadVoltsL2_L1(Double *volt);
			Bool ReadVoltsL3_L2(Double *volt);
			Bool ReadVoltsL1_L3(Double *volt);
			Bool ReadSysVoltsL1(Double *volt);
			Bool ReadSysVoltsL2(Double *volt);
			Bool ReadSysVoltsL3(Double *volt);
			Bool ReadAmpsL1(Double *amps);
			Bool ReadAmpsL2(Double *amps);
			Bool ReadAmpsL3(Double *amps);
			Bool ReadActivePower(Double *watt);
			Bool ReadApparentPower(Double *voltAmps);
			Bool ReadReactivePower(Double *var);
			Bool ReadPowerFactor(Double *ratio);
			Bool ReadTotalActiveEnergy(Double *wh);
			Bool ReadTotalReactiveEnergy(Double *varh);
			Bool ReadFrequency(Double *hz);
			Bool ReadActivePowerL1(Double *watt);
			Bool ReadActivePowerL2(Double *watt);
			Bool ReadActivePowerL3(Double *watt);
			Bool ReadReactivePowerL1(Double *var);
			Bool ReadReactivePowerL2(Double *var);
			Bool ReadReactivePowerL3(Double *var);
			Bool ReadSysWDemand(Double *watt);
			Bool ReadSysVADemand(Double *voltAmps);
			Bool ReadSysADemand(Double *amps);
			Bool ReadNeutralCurrent(Double *amps);
			Bool ReadMaxWDemand(Double *watt);
			Bool ReadMaxVADemand(Double *voltAmps);
			Bool ReadMaxADemand(Double *amps);
			Bool ReadTHD_V1(Double *volt);
			Bool ReadTHD_V2(Double *volt);
			Bool ReadTHD_V3(Double *volt);
			Bool ReadTHD_I1(Double *amps);
			Bool ReadTHD_I2(Double *amps);
			Bool ReadTHD_I3(Double *amps);

			Bool ReadSysV(Double *volts);
			Bool ReadSysI(Double *amps);
			Bool ReadSysType(Int32 *typ);
			Bool ReadSysPower(Double *watt);
			Bool ReadDemandTime(Int32 *timeMin);
			Bool ReadDemandPeriod(Int32 *period);
			Bool ReadRelayDivisor(Int32 *divisor);
			Bool GetRelayPulseWidth(Int32 *width); //n * 20ms
		};
	}
}
#endif
