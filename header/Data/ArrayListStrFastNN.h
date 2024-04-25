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

		virtual NN<Data::ArrayListNN<Text::String>> Clone() const;
		virtual OSInt Compare(NN<Text::String> obj1, NN<Text::String> obj2) const;
		OSInt SortedIndexOf(Text::CStringNN str) const;

		void FreeAll();
	};
}
#endif
