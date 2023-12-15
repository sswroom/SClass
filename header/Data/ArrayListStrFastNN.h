#ifndef _SM_DATA_ARRAYLISTSTRFASTNN
#define _SM_DATA_ARRAYLISTSTRFASTNN
#include "Data/SortableArrayListNN.h"
#include "Text/String.h"

namespace Data
{
	class ArrayListStrFastNN : public Data::SortableArrayListNN<Text::String>
	{
	public:
		ArrayListStrFastNN();
		ArrayListStrFastNN(UOSInt capacity);

		virtual NotNullPtr<Data::ArrayListNN<Text::String>> Clone() const;
		virtual OSInt Compare(NotNullPtr<Text::String> obj1, NotNullPtr<Text::String> obj2) const;
		OSInt SortedIndexOf(Text::CStringNN str) const;
	};
}
#endif
