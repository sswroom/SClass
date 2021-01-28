#ifndef _SM_IO_POWERINFO
#define _SM_IO_POWERINFO

namespace IO
{
	class PowerInfo
	{
	public:
		typedef enum
		{
			ACS_UNKNOWN,
			ACS_OFF,
			ACS_ON
		} ACStatus;

		typedef struct
		{
			ACStatus acStatus;
			Bool hasBattery;
			Bool batteryCharging;
			Int32 batteryPercent;
			Int32 timeLeftSec;
			Double batteryVoltage; //V
			Double batteryChargeCurrent; //A
			Double batteryTemp; //degree C
		} PowerStatus;
	public:
		static Bool GetPowerStatus(PowerStatus *power);
	};
};

#endif
