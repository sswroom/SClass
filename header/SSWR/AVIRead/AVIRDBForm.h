#ifndef _SM_SSWR_AVIREAD_AVIRDBFORM
#define _SM_SSWR_AVIREAD_AVIRDBFORM
#include "DB/ReadingDB.h"
#include "DB/ReadingDBTool.h"
#include "IO/Writer.h"
#include "IO/LogTool.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "SSWR/AVIRead/AVIRDBManager.h"
#include "UI/GUIForm.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUIListBox.h"
#include "UI/GUIListView.h"
#include "UI/GUIMainMenu.h"
#include "UI/GUIPanel.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"
#include "UI/GUIVSplitter.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRDBForm : public UI::GUIForm, public AVIRDBManager
		{
		private:
			NN<UI::GUITabControl> tcDB;
			NN<UI::GUIVSplitter> vspDB;
			NN<UI::GUIListView> lvResult;

			NN<UI::GUITabPage> tpTable;
			NN<UI::GUIListBox> lbSchema;
			NN<UI::GUIHSplitter> hspSchema;
			NN<UI::GUIListBox> lbTable;
			NN<UI::GUIHSplitter> hspTable;
			NN<UI::GUIListView> lvTable;

			NN<UI::GUITabPage> tpSQL;
			NN<UI::GUIPanel> pnlSQLCtrl;
			NN<UI::GUIButton> btnSQL;
			NN<UI::GUITextBox> txtSQL;

			NN<UI::GUIMainMenu> mnuMain;

			NN<SSWR::AVIRead::AVIRCore> core;
			Data::ArrayListStringNN dbNames;
			NN<DB::ReadingDB> db;
			Bool needRelease;
			Optional<DB::ReadingDBTool> dbt;
			IO::LogTool log;
			Optional<Data::QueryConditions> currCond;

			Optional<IO::Writer> debugWriter;
			Optional<IO::LogHandler> logHdlr;

			static void __stdcall OnSchemaSelChg(AnyType userObj);
			static void __stdcall OnTableSelChg(AnyType userObj);
			static void __stdcall OnSQLClicked(AnyType userObj);

			void UpdateResult(NN<DB::DBReader> r);
			void CopyTableCreate(DB::SQLType sqlType, Bool axisAware);
			void ExportTableData(DB::SQLType sqlType, Bool axisAware);
			void ExportTableCSV();
			void ExportTableSQLite();
			void ExportTableHTML();
			void ExportTablePList();
			void ExportTableXLSX();
			void ExportTableExcelXML();
		public:
			AVIRDBForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<DB::ReadingDB> db, Bool needRelease);
			virtual ~AVIRDBForm();

			void UpdateSchemas();
			void UpdateTables();
			virtual void EventMenuClicked(UInt16 cmdId);
			virtual void OnMonitorChanged();

			virtual UIntOS GetDataSourceCount() const;
			virtual void GetDataSourceName(UIntOS index, NN<Text::StringBuilderUTF8> sb) const;
			virtual Optional<DB::ReadingDB> OpenDataSource(UIntOS index);
		};
	}
}
#endif
