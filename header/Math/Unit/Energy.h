#ifndef _SM_MATH_UNIT_ENERGY
#define _SM_MATH_UNIT_ENERGY
namespace Math
{
	namespace Unit
	{
		class Energy
		{
		public:
			typedef enum
			{
				EU_JOULE,
				EU_WATTHOUR,
				EU_KILOWATTHOUR,

				EU_FIRST = EU_JOULE,
				EU_LAST = EU_KILOWATTHOUR
			} EnergyUnit;
		public:
			static Double GetUnitRatio(EnergyUnit unit);
			static const UTF8Char *GetUnitShortName(EnergyUnit unit);
			static const UTF8Char *GetUnitName(EnergyUnit unit);
			static Double Convert(EnergyUnit fromUnit, EnergyUnit toUnit, Double fromValue);
		};
	}
}
#endif
