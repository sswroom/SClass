#ifndef _SM_MATH_ARCGISCSYS
#define _SM_MATH_ARCGISCSYS
#include "Data/FastStringMapNN.hpp"
#include "Text/CString.h"

namespace Math
{
	class ArcGISCSys
	{
	public:
		typedef struct
		{
			UInt32 srid;
			const UTF8Char *name;
			UIntOS nameLen;
			const UTF8Char *projWKT;
			UIntOS projWKTLen;
		} CSysInfo;

	private:
		static CSysInfo geogList[];
		static CSysInfo projList[];

		Data::FastStringMapNN<CSysInfo> geogMap;
		Data::FastStringMapNN<CSysInfo> projMap;

	public:
		ArcGISCSys();
		~ArcGISCSys();

		UInt32 GuessSRIDGeog(Text::CStringNN name);
		UInt32 GuessSRIDProj(Text::CStringNN name);
	};
}
#endif
