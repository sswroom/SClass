#ifndef _SM_DATA_FIELDCOMPARATOR
#define _SM_DATA_FIELDCOMPARATOR
#include "Data/ArrayList.h"
#include "Data/Comparator.h"
#include "Data/VariObject.h"
#include "DB/DBUtil.h"
#include "Text/StringBuilderUTF.h"

namespace Data
{
	class FieldComparator : public Comparator<Data::VariObject*>
	{
	private:
		Data::ArrayList<const UTF8Char*> *fieldNames;
		Data::ArrayList<Int8> *dirs;
	public:
		FieldComparator(const UTF8Char *compareConds);
		virtual ~FieldComparator();

		virtual OSInt Compare(VariObject *a, VariObject *b);
		Bool IsValid();
		Bool ToOrderClause(Text::StringBuilderUTF *sb, DB::DBUtil::ServerType svrType);

		static OSInt Compare(VariItem *a, VariItem *b);
	};
}
#endif