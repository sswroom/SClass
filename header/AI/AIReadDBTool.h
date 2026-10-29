#ifndef _SM_AI_AIREADDBTOOL
#define _SM_AI_AIREADDBTOOL
#include "AI/AITool.h"
#include "DB/ReadingDB.h"
#include "Data/ArrayListStringNN.h"
#include "Data/QueryConditions.h"

namespace AI
{
	class AIReadDBTool : public AITool
	{
	private:
		NN<DB::ReadingDB> db;
		NN<Text::String> schemaName;

	public:
		AIReadDBTool(NN<DB::ReadingDB> db, Text::CStringNN schemaName);
		virtual ~AIReadDBTool();
		virtual Text::CStringNN GetName() const;
		virtual void GetDescription(NN<Text::StringBuilderUTF8> sb) const;
		virtual void GetParameters(NN<Net::OpenAIToolParameters> parameters) const;
		Text::CStringNN GetSchemaName() const;
		UIntOS QueryTableNames(NN<Data::ArrayListStringNN> names) const;
		Optional<DB::TableDef> GetTableDef(Text::CStringNN tableName) const;
		Optional<DB::DBReader> QueryTableData(Text::CStringNN tableName, Optional<Data::ArrayListStringNN> columnNames, UIntOS dataOfst, UIntOS maxCnt, Text::CString ordering, Optional<Data::QueryConditions> condition) const;
		void CloseReader(NN<DB::DBReader> reader) const;
		virtual Bool Invoke(NN<Data::VariObject> input, NN<Data::VariObject> output);
	};
}
#endif
