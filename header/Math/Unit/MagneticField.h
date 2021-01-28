#ifndef _SM_MATH_UNIT_MAGNETICFIELD
#define _SM_MATH_UNIT_MAGNETICFIELD
namespace Math
{
	namespace Unit
	{
		class MagneticField
		{
		public:
			typedef enum
			{
				MFU_TESLA,
				MFU_GAUSS,
				MFU_MICROTESLA,
				MFU_MILLITESLA
			} MagneticFieldUnit;
		public:
			static Double GetUnitRatio(MagneticFieldUnit unit);
			static const UTF8Char *GetUnitShortName(MagneticFieldUnit unit);
			static const UTF8Char *GetUnitName(MagneticFieldUnit unit);
			static Double Convert(MagneticFieldUnit fromUnit, MagneticFieldUnit toUnit, Double fromValue);
		};
	}
}
#endif
