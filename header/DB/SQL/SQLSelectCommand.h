#ifndef _SM_DB_SQLSELECTCOMMAND
#define _SM_DB_SQLSELECTCOMMAND
#include "Data/ArrayListNN.hpp"
#include "DB/SQL/SQLCommand.h"
#include "DB/SQL/SQLValue.h"

namespace DB
{
	namespace SQL
	{
		class SQLSelectCommand : public SQLCommand
		{
		private:
			struct ColumnInfo
			{
				NN<SQLValue> value;
				Optional<Text::String> alias;
			};
		private:
			Data::ArrayListNN<ColumnInfo> columnList;

			static void __stdcall FreeColumnInfo(NN<ColumnInfo> v);
		public:
			SQLSelectCommand();
			virtual ~SQLSelectCommand();

			virtual CommandType GetCommandType() const;
			void AddColumn(NN<SQLValue> value, Text::CString alias);
			UIntOS GetColumnCount() const;
			Optional<SQLValue> GetColumn(UIntOS index) const;
			Optional<Text::String> GetColumnAlias(UIntOS index) const;
		};
	}
}
#endif
