#ifndef _SM_DB_FOREIGNKEYDEF
#define _SM_DB_FOREIGNKEYDEF
#include "Data/ArrayListNN.hpp"
#include "Text/String.h"

namespace DB
{
	class ForeignKeyDef
	{
	public:
		struct ColMap
		{
			NN<Text::String> localCol;
			NN<Text::String> foreignCol;
		};
	private:
		Optional<Text::String> name;
		Optional<Text::String> foreignSchema;
		NN<Text::String> foreignTable;
		Data::ArrayListNN<ColMap> cols;

		static void FreeColMap(NN<ColMap> colMap);
	public:
		ForeignKeyDef(Text::CString name, Text::CString foreignSchema, Text::CStringNN foreignTable);
		ForeignKeyDef(Optional<Text::String> name, Optional<Text::String> foreignSchema, NN<Text::String> foreignTable);
		~ForeignKeyDef();

		Optional<Text::String> GetName() const;
		Optional<Text::String> GetForeignSchema() const;
		NN<Text::String> GetForeignTable() const;
		void AddCol(Text::CStringNN localCol, Text::CStringNN foreignCol);
		void AddCol(NN<Text::String> localCol, NN<Text::String> foreignCol);
		UIntOS GetColCnt() const;
		Optional<ColMap> GetCol(UIntOS index) const;
	};
}
#endif
