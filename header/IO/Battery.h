//need setupapi.lib
#ifndef _SM_IO_BATTERY
#define _SM_IO_BATTERY

namespace IO
{
	class Battery
	{
	public:
		enum ACPowerStatus
		{
			ACPWR_OFFLINE = 0,
			ACPWR_ONLINE = 1,
			ACPWR_UNK = 255
		};
		enum BatteryFlag
		{
			BATTFLG_HIGH = 1,
			BATTFLG_LOW = 2,
			BATTFLG_CRITICAL = 4,
			BATTFLG_CHARGING = 8,
			BATTFLG_NOBATTERY = 128,
			BATTFLG_UNK = 255
		};

	private:
		void *hand;

		Battery(void *hand);
		~Battery();

	public:
		Int32 GetBatteryCount();
		Battery *GetBattery(Int32 index);
		static ACPowerStatus GetACLineStatus();
		static BatteryFlag GetBatteryFlag();
		static Int32 GetBatteryPercent();
		static Int32 GetBatteryTimeSecond();
		static Int32 GetBatteryChargeTimeSecond();
	};
};
#endif
