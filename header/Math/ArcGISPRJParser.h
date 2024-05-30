#ifndef _SM_MATH_ARCGISPRJPARSER
#define _SM_MATH_ARCGISPRJPARSER
#include "Math/ArcGISCSys.h"
#include "Math/CoordinateSystem.h"

namespace Math
{
	class ArcGISPRJParser
	{
	private:
		Math::ArcGISCSys csys;

		static Bool ParsePRJString(UnsafeArray<UTF8Char> prjBuff, OutParam<UOSInt> strSize);
	public:
		ArcGISPRJParser();
		~ArcGISPRJParser();

		Optional<Math::CoordinateSystem> ParsePRJFile(Text::CStringNN fileName);
		Optional<Math::CoordinateSystem> ParsePRJBuff(Text::CStringNN sourceName, UnsafeArray<UTF8Char> prjBuff, UOSInt buffSize, OptOut<UOSInt> parsedSize);
	};
}
#endif
