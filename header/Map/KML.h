#ifndef _SM_MAP_KML
#define _SM_MAP_KML
#include "IO/FileStream.h"
#include "Data/ArrayList.h"
#include "Data/ArrayListDbl.h"

namespace Map
{
	class KML
	{
	public:
		static Bool GenKML(IO::FileStream *fs, Data::ArrayList<const WChar*> *names, Data::ArrayListDbl *lats, Data::ArrayListDbl *lons);
	};
};
#endif
