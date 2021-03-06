#ifndef _SM_SSWR_AVIREAD_AVIRDBMANAGERFORM
#define _SM_SSWR_AVIREAD_AVIRDBMANAGERFORM
#include "DB/DBTool.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIForm.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUIListBox.h"
#include "UI/GUIListView.h"
#include "UI/GUIMainMenu.h"
#include "UI/GUIPanel.h"
#include "UI/GUIPopupMenu.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUIVSplitter.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRDBManagerForm : public UI::GUIForm
		{
		private:
			UI::GUIPopupMenu *mnuTable;
			UI::GUIPopupMenu *mnuConn;
			UI::GUIMainMenu *mnuMain;

			UI::GUIListBox *lbConn;
			UI::GUIHSplitter *hspConn;
			UI::GUITabControl *tcMain;

			UI::GUITabPage *tpDetail;

			UI::GUITabPage *tpDatabase;
			UI::GUIListBox *lbDatabase;
			UI::GUIButton *btnDatabase;

			UI::GUITabPage *tpTable;
			UI::GUIPanel *pnlTable;
			UI::GUIListBox *lbTable;
			UI::GUIHSplitter *hspTable;
			UI::GUIListView *lvTable;
			UI::GUIVSplitter *vspTable;
			UI::GUIListView *lvTableResult;

			SSWR::AVIRead::AVIRCore *core;
			Data::ArrayList<DB::DBTool*> *dbList;
			IO::LogTool *log;
			DB::DBTool *currDB;

			static void __stdcall OnConnSelChg(void *userObj);
			static Bool __stdcall OnConnRClicked(void *userObj, OSInt scnX, OSInt scnY, MouseButton btn);
			static void __stdcall OnTableSelChg(void *userObj);
			static Bool __stdcall OnTableRClicked(void *userObj, OSInt scnX, OSInt scnY, MouseButton btn);
			static void __stdcall OnDatabaseClicked(void *userObj);

			void UpdateDatabaseList();
			void UpdateTableList();
			void UpdateTableData(const UTF8Char *tableName);
			void UpdateResult(DB::DBReader *r);

			void AppendJavaCol(Text::StringBuilderUTF *sb, DB::ColDef *col);
		public:
			AVIRDBManagerForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRDBManagerForm();

			virtual void EventMenuClicked(UInt16 cmdId);
			virtual void OnMonitorChanged();

			void ConnAdd(DB::DBConn *conn);
		};
	}
}
#endif
