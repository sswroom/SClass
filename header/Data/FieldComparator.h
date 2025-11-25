#ifndef _SM_DATA_FIELDCOMPARATOR
#define _SM_DATA_FIELDCOMPARATOR
#include "Data/ArrayList.hpp"
#include "Data/ArrayListStringNN.h"
#include "Data/Comparator.hpp"
#include "Data/VariObject.h"
#include "DB/DBUtil.h"
#include "Text/StringBuilderUTF8.h"

namespace Data
{
	class FieldComparator : public Comparator<NN<Data::VariObject>>
	{
	private:
		Data::ArrayListStringNN fieldNames;
		Data::ArrayList<Int8> dirs;
	public:
		FieldComparator(Text::CString compareConds);
		virtual ~FieldComparator();

		virtual OSInt Compare(NN<VariObject> a, NN<VariObject> b) const;
		Bool IsValid();
		Bool ToOrderClause(NN<Text::StringBuilderUTF8> sb, DB::SQLType sqlType);

		static OSInt Compare(Optional<VariItem> a, Optional<VariItem> b);
	};
}
#endif
