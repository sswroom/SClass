#ifndef _SM_AI_AIREADDBTOOL
#define _SM_AI_AIREADDBTOOL
#include "AI/AITool.h"
#include "DB/ReadingDB.h"
#include "Data/ArrayListStringNN.h"
#include "Data/ChartPlotter.h"
#include "Data/QueryConditions.h"
#include "Media/DrawEngine.h"

namespace AI
{
	class AIReadDBTool : public AITool
	{
	private:
		NN<DB::ReadingDB> db;
		NN<Text::String> schemaName;
		Optional<Text::String> filePath;
		Bool allowXlsx;
		UIntOS maxToolSize;
		Optional<Media::DrawEngine> drawEngine;

		static Bool IsNumeric(DB::DBUtil::ColType type);
		static Bool SafeFilePart(Text::CStringNN name);
		static void ChartResult(NN<Data::VariObject> output, Text::CStringNN message);
		static Bool SaveChart(NN<Media::DrawEngine> engine, NN<Data::ChartPlotter> chart, Text::CStringNN fileName);
		static void MakeChart(NN<Media::DrawEngine> engine, NN<DB::DBReader> r, Text::CStringNN sessionId, Text::CStringNN path, NN<Data::VariObject> input, NN<Data::VariObject> output);
	public:
		AIReadDBTool(NN<DB::ReadingDB> db, Text::CStringNN schemaName, Text::CString filePath, Bool allowXlsx, Optional<Media::DrawEngine> drawEngine);
		virtual ~AIReadDBTool();
		virtual Text::CStringNN GetName() const;
		virtual void GetDescription(NN<Text::StringBuilderUTF8> sb) const;
		virtual void GetParameters(NN<Net::OpenAIToolParameters> parameters) const;
		Text::CStringNN GetSchemaName() const;
		UIntOS QueryTableNames(NN<Data::ArrayListStringNN> names) const;
		Optional<DB::TableDef> GetTableDef(Text::CStringNN tableName) const;
		Optional<DB::DBReader> QueryTableData(Text::CStringNN tableName, Optional<Data::ArrayListStringNN> columnNames, UIntOS dataOfst, UIntOS maxCnt, Text::CString ordering, Optional<Data::QueryConditions> condition) const;
		void CloseReader(NN<DB::DBReader> reader) const;
		virtual Bool Invoke(Text::CStringNN sessionId, NN<Data::VariObject> input, NN<Data::VariObject> output);
		virtual void SetMaxToolSize(UIntOS maxToolSize);
	};
}
#endif
