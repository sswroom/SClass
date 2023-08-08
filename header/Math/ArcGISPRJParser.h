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

		static Bool ParsePRJString(UTF8Char *prjBuff, UOSInt *strSize);
	public:
		ArcGISPRJParser();
		~ArcGISPRJParser();

		Math::CoordinateSystem *ParsePRJFile(Text::CStringNN fileName);
		Math::CoordinateSystem *ParsePRJBuff(Text::CString sourceName, UTF8Char *prjBuff, UOSInt buffSize, UOSInt *parsedSize);
	};
}
#endif
