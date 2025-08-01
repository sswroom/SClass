#ifndef _SM_DATA_DATACOMPARER
#define _SM_DATA_DATACOMPARER
#include "Data/UUID.h"
#include "Math/Geometry/Vector2D.h"
#include "Text/CString.h"
#include "Text/String.h"

namespace Data
{
	template <class T> class ReadonlyArray;
	class DataComparer
	{
	public:
		static OSInt Compare(UnsafeArray<const UTF8Char> val1, UnsafeArray<const UTF8Char> val2);
		static OSInt Compare(Single val1, Single val2);
		static OSInt Compare(Double val1, Double val2);
		static OSInt Compare(Int32 val1, Int32 val2);
		static OSInt Compare(UInt32 val1, UInt32 val2);
		static OSInt Compare(Int64 val1, Int64 val2);
		static OSInt Compare(UInt64 val1, UInt64 val2);
		static OSInt Compare(Bool val1, Bool val2);
		static OSInt Compare(const UInt8 *val1, UOSInt cnt1, const UInt8 *val2, UOSInt cnt2);
		static OSInt Compare(Math::Geometry::Vector2D *val1, Math::Geometry::Vector2D *val2);
		static OSInt Compare(NN<Math::Geometry::Vector2D> val1, NN<Math::Geometry::Vector2D> val2);
		static OSInt Compare(Optional<Data::UUID> val1, Optional<Data::UUID> val2);
		static OSInt Compare(NN<Data::UUID> val1, NN<Data::UUID> val2);
		static OSInt Compare(Text::CString val1, Text::CString val2);
		static OSInt Compare(Text::String *val1, Text::String *val2);
		static OSInt Compare(NN<Text::String> val1, NN<Text::String> val2);
		static OSInt Compare(Data::ReadonlyArray<UInt8> *val1, Data::ReadonlyArray<UInt8> *val2);

		static Bool NearlyEquals(Double val1, Double val2);
	};
}
#endif
