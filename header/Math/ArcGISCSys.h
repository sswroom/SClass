#ifndef _SM_MATH_ARCGISCSYS
#define _SM_MATH_ARCGISCSYS
#include "Data/FastStringMap.h"
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
			UOSInt nameLen;
			const UTF8Char *projWKT;
			UOSInt projWKTLen;
		} CSysInfo;

	private:
		static CSysInfo geogList[];
		static CSysInfo projList[];

		Data::FastStringMap<CSysInfo*> geogMap;
		Data::FastStringMap<CSysInfo*> projMap;

	public:
		ArcGISCSys();
		~ArcGISCSys();

		UInt32 GuessSRIDGeog(Text::CStringNN name);
		UInt32 GuessSRIDProj(Text::CStringNN name);
	};
}
#endif
