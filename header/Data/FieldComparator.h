#ifndef _SM_DATA_FIELDCOMPARATOR
#define _SM_DATA_FIELDCOMPARATOR
#include "Data/ArrayList.h"
#include "Data/ArrayListStringNN.h"
#include "Data/Comparator.h"
#include "Data/VariObject.h"
#include "DB/DBUtil.h"
#include "Text/StringBuilderUTF8.h"

namespace Data
{
	class FieldComparator : public Comparator<NotNullPtr<Data::VariObject>>
	{
	private:
		Data::ArrayListStringNN fieldNames;
		Data::ArrayList<Int8> dirs;
	public:
		FieldComparator(Text::CString compareConds);
		virtual ~FieldComparator();

		virtual OSInt Compare(NotNullPtr<VariObject> a, NotNullPtr<VariObject> b) const;
		Bool IsValid();
		Bool ToOrderClause(NotNullPtr<Text::StringBuilderUTF8> sb, DB::SQLType sqlType);

		static OSInt Compare(VariItem *a, VariItem *b);
	};
}
#endif
