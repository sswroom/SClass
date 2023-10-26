#ifndef _SM_DATA_OBJECTGETTER
#define _SM_DATA_OBJECTGETTER
#include "Data/VariItem.h"

namespace Data
{
	class ObjectGetter
	{
	public:
		virtual ~ObjectGetter() {};

		virtual NotNullPtr<VariItem> GetNewItem(Text::CStringNN name) = 0;
	};
}
#endif
