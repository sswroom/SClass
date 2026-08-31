#ifndef _SM_DB_SQLINSERTCOMMAND
#define _SM_DB_SQLINSERTCOMMAND
#include "Data/ArrayListStringNN.h"
#include "Data/ArrayListNN.hpp"
#include "DB/SQL/SQLCommand.h"
#include "DB/SQL/SQLObjectPath.h"

namespace DB
{
	namespace SQL
	{
		class SQLInsertCommand : public SQLCommand
		{
		private:
			NN<SQLObjectPath> objectPath;
			Data::ArrayListStringNN columnList;
			Data::ArrayListNN<Data::ArrayListNN<SQLValue>> rowList;

			static void __stdcall FreeRow(NN<Data::ArrayListNN<SQLValue>> row);
		public:
			SQLInsertCommand(NN<SQLObjectPath> objPath);
			virtual ~SQLInsertCommand();

			virtual CommandType GetCommandType() const;
			void AddColumn(Text::CStringNN columnName);
			void AddRow(NN<Data::ArrayListNN<SQLValue>> row);
			NN<SQLObjectPath> GetObjectPath() const;
			UIntOS GetColumnCount() const;
			Optional<Text::String> GetColumn(UIntOS index) const;
			UIntOS GetRowCount() const;
			Optional<Data::ArrayListNN<SQLValue>> GetRow(UIntOS index) const;
		};
	}
}
#endif
