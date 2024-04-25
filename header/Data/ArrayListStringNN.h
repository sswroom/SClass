#ifndef _SM_DATA_ARRAYLISTSTRINGNN
#define _SM_DATA_ARRAYLISTSTRINGNN
#include "Data/SortableArrayListNN.h"
#include "Text/String.h"

namespace Data
{
	class ArrayListStringNN : public Data::SortableArrayListNN<Text::String>
	{
	public:
		ArrayListStringNN();
		ArrayListStringNN(UOSInt capacity);

		virtual NN<Data::ArrayListNN<Text::String>> Clone() const;
		virtual OSInt Compare(NN<Text::String> obj1, NN<Text::String> obj2) const;
		virtual OSInt SortedIndexOfC(Text::CStringNN val) const;
		UOSInt IndexOfC(Text::CStringNN val) const;

		NN<Text::String> JoinString() const;
		void FreeAll();
	};
}
#endif
