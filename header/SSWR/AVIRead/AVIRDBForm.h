#ifndef _SM_SSWR_AVIREAD_AVIRDBFORM
#define _SM_SSWR_AVIREAD_AVIRDBFORM
#include "DB/ReadingDB.h"
#include "DB/ReadingDBTool.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIForm.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUIListBox.h"
#include "UI/GUIListView.h"
#include "UI/GUIMainMenu.h"
#include "UI/GUIPanel.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUIVSplitter.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRDBForm : public UI::GUIForm
		{
		private:
			UI::GUITabControl *tcDB;
			UI::GUITabPage *tpTable;
			UI::GUITabPage *tpSQL;
			UI::GUIVSplitter *vspDB;
			UI::GUIListView *lvResult;

			UI::GUIListBox *lbSchema;
			UI::GUIHSplitter *hspSchema;
			UI::GUIListBox *lbTable;
			UI::GUIHSplitter *hspTable;
			UI::GUIListView *lvTable;

			UI::GUIMainMenu *mnuMain;

			SSWR::AVIRead::AVIRCore *core;
			Data::ArrayList<const UTF8Char*> dbNames;
			DB::ReadingDB *db;
			Bool needRelease;
			DB::ReadingDBTool *dbt;
			IO::LogTool log;

			static void __stdcall OnSchemaSelChg(void *userObj);
			static void __stdcall OnTableSelChg(void *userObj);

			void UpdateResult(DB::DBReader *r);
			Data::Class *CreateTableClass(Text::CString schemaName, Text::CString tableName);
		public:
			AVIRDBForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, DB::ReadingDB *db, Bool needRelease);
			virtual ~AVIRDBForm();

			void UpdateSchemas();
			void UpdateTables();
			virtual void EventMenuClicked(UInt16 cmdId);
			virtual void OnMonitorChanged();
		};
	}
}
#endif
