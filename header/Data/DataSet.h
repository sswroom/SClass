#ifndef _SM_DATA_DATASET
#define _SM_DATA_DATAEST
#include "Data/ArrayList.h"
#include "Data/VariItem.h"

namespace Data
{
	class DataSet
	{
	public:
		UnsafeArray<VariItem> items;
		UOSInt capacity;
		UOSInt itemCnt;

	public:
		DataSet();
		~DataSet();

		void AddItem(const VariItem& key, const VariItem& value);
		UOSInt GetCount() const { return this->itemCnt; }
	};
}
#endif
