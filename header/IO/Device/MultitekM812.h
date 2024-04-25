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
			MultitekM812(NN<IO::MODBUSMaster> modbus, UInt8 addr);
			virtual ~MultitekM812();

			Bool ReadVoltsL2_L1(OutParam<Double> volt);
			Bool ReadVoltsL3_L2(OutParam<Double> volt);
			Bool ReadVoltsL1_L3(OutParam<Double> volt);
			Bool ReadSysVoltsL1(OutParam<Double> volt);
			Bool ReadSysVoltsL2(OutParam<Double> volt);
			Bool ReadSysVoltsL3(OutParam<Double> volt);
			Bool ReadAmpsL1(OutParam<Double> amps);
			Bool ReadAmpsL2(OutParam<Double> amps);
			Bool ReadAmpsL3(OutParam<Double> amps);
			Bool ReadActivePower(OutParam<Double> watt);
			Bool ReadApparentPower(OutParam<Double> voltAmps);
			Bool ReadReactivePower(OutParam<Double> var);
			Bool ReadPowerFactor(OutParam<Double> ratio);
			Bool ReadTotalActiveEnergy(OutParam<Double> wh);
			Bool ReadTotalReactiveEnergy(OutParam<Double> varh);
			Bool ReadFrequency(OutParam<Double> hz);
			Bool ReadActivePowerL1(OutParam<Double> watt);
			Bool ReadActivePowerL2(OutParam<Double> watt);
			Bool ReadActivePowerL3(OutParam<Double> watt);
			Bool ReadReactivePowerL1(OutParam<Double> var);
			Bool ReadReactivePowerL2(OutParam<Double> var);
			Bool ReadReactivePowerL3(OutParam<Double> var);
			Bool ReadSysWDemand(OutParam<Double> watt);
			Bool ReadSysVADemand(OutParam<Double> voltAmps);
			Bool ReadSysADemand(OutParam<Double> amps);
			Bool ReadNeutralCurrent(OutParam<Double> amps);
			Bool ReadMaxWDemand(OutParam<Double> watt);
			Bool ReadMaxVADemand(OutParam<Double> voltAmps);
			Bool ReadMaxADemand(OutParam<Double> amps);
			Bool ReadTHD_V1(OutParam<Double> volt);
			Bool ReadTHD_V2(OutParam<Double> volt);
			Bool ReadTHD_V3(OutParam<Double> volt);
			Bool ReadTHD_I1(OutParam<Double> amps);
			Bool ReadTHD_I2(OutParam<Double> amps);
			Bool ReadTHD_I3(OutParam<Double> amps);

			Bool ReadSysV(OutParam<Double> volts);
			Bool ReadSysI(OutParam<Double> amps);
			Bool ReadSysType(OutParam<Int32> typ);
			Bool ReadSysPower(OutParam<Double> watt);
			Bool ReadDemandTime(OutParam<Int32> timeMin);
			Bool ReadDemandPeriod(OutParam<Int32> period);
			Bool ReadRelayDivisor(OutParam<Int32> divisor);
			Bool GetRelayPulseWidth(OutParam<Int32> width); //n * 20ms
		};
	}
}
#endif
