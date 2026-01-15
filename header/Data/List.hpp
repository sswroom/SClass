#ifndef _SM_DATA_LIST
#define _SM_DATA_LIST
#include "Data/ReadingList.hpp"
namespace Data
{
	template <class T> class List : public ReadingList<T>
	{
	public:
		virtual ~List(){};

		virtual UIntOS Add(T val) = 0;
		virtual UIntOS AddRange(UnsafeArray<const T> arr, UIntOS cnt) = 0;
		virtual Bool Remove(T val) = 0;
		virtual T RemoveAt(UIntOS index) = 0;
		virtual void Insert(UIntOS index, T val) = 0;
		virtual UIntOS IndexOf(T val) const = 0; //INVALID_INDEX = not found
		virtual void Clear() = 0;

		virtual UIntOS GetCount() const = 0;
		virtual UIntOS GetCapacity() const = 0;

		virtual T GetItem(UIntOS index) const = 0;
		virtual void SetItem(UIntOS index, T val) = 0;
	};
}
#endif
