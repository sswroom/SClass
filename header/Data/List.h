#ifndef _SM_DATA_LIST
#define _SM_DATA_LIST
#include "Data/ReadingList.h"
namespace Data
{
	template <class T> class List : ReadingList<T>
	{
	public:
		virtual ~List(){};

		virtual UOSInt Add(T val) = 0;
		virtual UOSInt AddRange(T *arr, UOSInt cnt) = 0;
		virtual Bool Remove(T val) = 0;
		virtual T RemoveAt(UOSInt index) = 0;
		virtual void Insert(UOSInt index, T val) = 0;
		virtual OSInt IndexOf(T val) = 0; //-1 = not found
		virtual void Clear() = 0;

		virtual UOSInt GetCount() = 0;
		virtual UOSInt GetCapacity() = 0;

		virtual T GetItem(UOSInt index) = 0;
		virtual void SetItem(UOSInt index, T val) = 0;
	};
}
#endif
