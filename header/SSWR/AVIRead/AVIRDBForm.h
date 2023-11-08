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
			UI::GUITabControl *tcDB;
			UI::GUIVSplitter *vspDB;
			UI::GUIListView *lvResult;

			UI::GUITabPage *tpTable;
			UI::GUIListBox *lbSchema;
			UI::GUIHSplitter *hspSchema;
			UI::GUIListBox *lbTable;
			UI::GUIHSplitter *hspTable;
			UI::GUIListView *lvTable;

			UI::GUITabPage *tpSQL;
			UI::GUIPanel *pnlSQLCtrl;
			UI::GUIButton *btnSQL;
			UI::GUITextBox *txtSQL;

			UI::GUIMainMenu *mnuMain;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Data::ArrayListNN<Text::String> dbNames;
			NotNullPtr<DB::ReadingDB> db;
			Bool needRelease;
			DB::ReadingDBTool *dbt;
			IO::LogTool log;

			IO::Writer *debugWriter;
			IO::LogHandler *logHdlr;

			static void __stdcall OnSchemaSelChg(void *userObj);
			static void __stdcall OnTableSelChg(void *userObj);
			static void __stdcall OnSQLClicked(void *userObj);

			void UpdateResult(NotNullPtr<DB::DBReader> r);
			Data::Class *CreateTableClass(Text::CString schemaName, Text::CString tableName);
			void CopyTableCreate(DB::SQLType sqlType, Bool axisAware);
			void ExportTableData(DB::SQLType sqlType, Bool axisAware);
			void ExportTableCSV();
			void ExportTableSQLite();
		public:
			AVIRDBForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, NotNullPtr<DB::ReadingDB> db, Bool needRelease);
			virtual ~AVIRDBForm();

			void UpdateSchemas();
			void UpdateTables();
			virtual void EventMenuClicked(UInt16 cmdId);
			virtual void OnMonitorChanged();
		};
	}
}
#endif
