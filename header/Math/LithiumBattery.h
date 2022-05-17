#ifndef _SM_MATH_LITHIUMBATTERY
#define _SM_MATH_LITHIUMBATTERY
namespace Math
{
	class LithiumBattery
	{
	public:
		static constexpr Double voltageMax = 4.2;
		static constexpr Double voltageShutdown = 3.0;
	public:
		static Double PercentToVoltage(Double percent)
		{
			return (voltageMax - voltageShutdown) * percent * 0.01 + voltageShutdown;
		}

		static Int32 PercentToMilliVolt(Int32 percent)
		{
			return (4200 - 3000) * percent / 100 + 3000;
		}
	};
}
#endif
