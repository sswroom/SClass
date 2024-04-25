#ifndef _SM_DATA_DATAMODELUTIL
#define _SM_DATA_DATAMODELUTIL
#include "AnyType.h"
#include "Data/Class.h"
#include "DB/DBReader.h"

namespace Data
{
	class DataModelUtil
	{
	public:
		static Bool FillFromDBReader(NN<Data::Class> cls, AnyType value, NN<DB::DBReader> r);
	};
}
#endif
