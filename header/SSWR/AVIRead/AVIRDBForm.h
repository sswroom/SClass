#ifndef _SM_SSWR_AVIREAD_AVIRDBFORM
#define _SM_SSWR_AVIREAD_AVIRDBFORM
#include "DB/ReadingDB.h"
#include "DB/ReadingDBTool.h"
#include "IO/Writer.h"
#include "IO/LogTool.h"
#include "SSWR/AVIRead/AVIRCore.h"
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
		class AVIRDBForm : public UI::GUIForm
		{
		private:
			NotNullPtr<UI::GUITabControl> tcDB;
			NotNullPtr<UI::GUIVSplitter> vspDB;
			NotNullPtr<UI::GUIListView> lvResult;

			NotNullPtr<UI::GUITabPage> tpTable;
			NotNullPtr<UI::GUIListBox> lbSchema;
			NotNullPtr<UI::GUIHSplitter> hspSchema;
			NotNullPtr<UI::GUIListBox> lbTable;
			NotNullPtr<UI::GUIHSplitter> hspTable;
			NotNullPtr<UI::GUIListView> lvTable;

			NotNullPtr<UI::GUITabPage> tpSQL;
			NotNullPtr<UI::GUIPanel> pnlSQLCtrl;
			NotNullPtr<UI::GUIButton> btnSQL;
			NotNullPtr<UI::GUITextBox> txtSQL;

			NotNullPtr<UI::GUIMainMenu> mnuMain;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Data::ArrayListStringNN dbNames;
			NotNullPtr<DB::ReadingDB> db;
			Bool needRelease;
			DB::ReadingDBTool *dbt;
			IO::LogTool log;
			Data::QueryConditions *currCond;

			IO::Writer *debugWriter;
			IO::LogHandler *logHdlr;

			static void __stdcall OnSchemaSelChg(void *userObj);
			static void __stdcall OnTableSelChg(void *userObj);
			static void __stdcall OnSQLClicked(void *userObj);

			void UpdateResult(NotNullPtr<DB::DBReader> r);
			void CopyTableCreate(DB::SQLType sqlType, Bool axisAware);
			void ExportTableData(DB::SQLType sqlType, Bool axisAware);
			void ExportTableCSV();
			void ExportTableSQLite();
			void ExportTableHTML();
			void ExportTablePList();
			void ExportTableXLSX();
			void ExportTableExcelXML();
		public:
			AVIRDBForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, NotNullPtr<DB::ReadingDB> db, Bool needRelease);
			virtual ~AVIRDBForm();

			void UpdateSchemas();
			void UpdateTables();
			virtual void EventMenuClicked(UInt16 cmdId);
			virtual void OnMonitorChanged();
		};
	}
}
#endif
