#ifndef _SM_DATA_OBJECTGETTER
#define _SM_DATA_OBJECTGETTER
#include "Data/VariItem.h"

namespace Data
{
	class ObjectGetter
	{
	public:
		virtual ~ObjectGetter() {};

		virtual VariItem *GetNewItem(const UTF8Char *name) = 0;
	};
}
#endif