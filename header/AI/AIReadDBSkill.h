#ifndef _SM_AI_AIREADDBSKILL
#define _SM_AI_AIREADDBSKILL
#include "AI/AISkill.h"
#include "DB/ReadingDB.h"
#include "Data/ArrayListStringNN.h"
#include "Data/QueryConditions.h"

namespace AI
{
	class AIReadDBSkill : public AISkill
	{
	private:
		NN<DB::ReadingDB> db;
		NN<Text::String> schemaName;

		static Bool GetUIntOSInput(NN<Data::VariObject> input, UnsafeArray<const UTF8Char> name, UIntOS defaultValue, OutParam<UIntOS> value);

	public:
		AIReadDBSkill(NN<DB::ReadingDB> db, Text::CStringNN schemaName);
		virtual ~AIReadDBSkill();
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
