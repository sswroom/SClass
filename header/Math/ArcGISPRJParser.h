#ifndef _SM_MATH_ARCGISPRJPARSER
#define _SM_MATH_ARCGISPRJPARSER
#include "Math/CSys/ArcGISCSys.h"
#include "Math/CSys/CoordinateSystem.h"

namespace Math
{
	class ArcGISPRJParser
	{
	private:
		Math::CSys::ArcGISCSys csys;

		static Bool ParsePRJString(UnsafeArray<UTF8Char> prjBuff, OutParam<UIntOS> strSize);
	public:
		ArcGISPRJParser();
		~ArcGISPRJParser();

		Optional<Math::CSys::CoordinateSystem> ParsePRJFile(Text::CStringNN fileName);
		Optional<Math::CSys::CoordinateSystem> ParsePRJBuff(Text::CStringNN sourceName, UnsafeArray<UTF8Char> prjBuff, UIntOS buffSize, OptOut<UIntOS> parsedSize);
	};
}
#endif
