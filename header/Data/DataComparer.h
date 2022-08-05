#ifndef _SM_DATA_DATACOMPARER
#define _SM_DATA_DATACOMPARER
#include "Data/UUID.h"
#include "Math/Geometry/Vector2D.h"
#include "Text/CString.h"
namespace Data
{
	class DataComparer
	{
	public:
		static OSInt Compare(const UTF8Char *val1, const UTF8Char *val2);
		static OSInt Compare(Single val1, Single val2);
		static OSInt Compare(Double val1, Double val2);
		static OSInt Compare(Int32 val1, Int32 val2);
		static OSInt Compare(UInt32 val1, UInt32 val2);
		static OSInt Compare(Int64 val1, Int64 val2);
		static OSInt Compare(UInt64 val1, UInt64 val2);
		static OSInt Compare(Bool val1, Bool val2);
		static OSInt Compare(const UInt8 *val1, UOSInt cnt1, const UInt8 *val2, UOSInt cnt2);
		static OSInt Compare(Math::Geometry::Vector2D *val1, Math::Geometry::Vector2D *val2);
		static OSInt Compare(Data::UUID *val1, Data::UUID *val2);
		static OSInt Compare(Text::CString val1, Text::CString val2);

		static Bool NearlyEquals(Double val1, Double val2);
	};
}
#endif
