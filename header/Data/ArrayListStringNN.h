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

		virtual NotNullPtr<Data::ArrayListNN<Text::String>> Clone() const;
		virtual OSInt Compare(NotNullPtr<Text::String> obj1, NotNullPtr<Text::String> obj2) const;
		virtual OSInt SortedIndexOfC(Text::CStringNN val) const;

		NotNullPtr<Text::String> JoinString() const;
	};
}
#endif
