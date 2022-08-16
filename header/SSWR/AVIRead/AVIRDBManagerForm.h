#ifndef _SM_SSWR_AVIREAD_AVIRDBMANAGERFORM
#define _SM_SSWR_AVIREAD_AVIRDBMANAGERFORM
#include "DB/DBTool.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIForm.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUIListBox.h"
#include "UI/GUIListView.h"
#include "UI/GUIMainMenu.h"
#include "UI/GUIMapControl.h"
#include "UI/GUIPanel.h"
#include "UI/GUIPopupMenu.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"
#include "UI/GUIVSplitter.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRDBManagerForm : public UI::GUIForm
		{
		private:
			UI::GUIPopupMenu *mnuTable;
			UI::GUIPopupMenu *mnuSchema;
			UI::GUIPopupMenu *mnuConn;
			UI::GUIMainMenu *mnuMain;

			UI::GUIListBox *lbConn;
			UI::GUIHSplitter *hspConn;
			UI::GUITabControl *tcMain;

			UI::GUITabPage *tpDatabase;
			UI::GUIListBox *lbDatabase;
			UI::GUIButton *btnDatabaseChange;
			UI::GUIButton *btnDatabaseDelete;
			UI::GUIButton *btnDatabaseNew;

			UI::GUITabPage *tpTable;
			UI::GUIPanel *pnlTable;
			UI::GUIListBox *lbSchema;
			UI::GUIHSplitter *hspSchema;
			UI::GUIListBox *lbTable;
			UI::GUIHSplitter *hspTable;
			UI::GUIListView *lvTable;
			UI::GUIVSplitter *vspTable;
			UI::GUIListView *lvTableResult;

			UI::GUITabPage *tpSQL;
			UI::GUIPanel *pnlSQL;
			UI::GUIPanel *pnlSQLCtrl;
			UI::GUIButton *btnSQLExec;
			UI::GUITextBox *txtSQL;
			UI::GUIVSplitter *vspSQL;
			UI::GUIListView *lvSQLResult;

			UI::GUITabPage *tpMap;
			UI::GUIPanel *pnlMap;
			UI::GUIHSplitter *hspMap;
			UI::GUIMapControl *mapMain;
			UI::GUIPanel *pnlMapTable;
			UI::GUIVSplitter *vspMapRecord;
			UI::GUIListView *lvMapRecord;
			UI::GUIListBox *lbMapSchema;
			UI::GUIHSplitter *hspMapTable;
			UI::GUIListBox *lbMapTable;

			SSWR::AVIRead::AVIRCore *core;
			Data::ArrayList<DB::DBTool*> dbList;
			Net::SSLEngine *ssl;
			IO::LogTool log;
			DB::DBTool *currDB;
			Media::ColorManagerSess *colorSess;
			Map::MapEnv *mapEnv;

			static void __stdcall OnConnSelChg(void *userObj);
			static Bool __stdcall OnConnRClicked(void *userObj, Math::Coord2D<OSInt> scnPos, MouseButton btn);
			static void __stdcall OnSchemaSelChg(void *userObj);
			static Bool __stdcall OnSchemaRClicked(void *userObj, Math::Coord2D<OSInt> scnPos, MouseButton btn);
			static void __stdcall OnTableSelChg(void *userObj);
			static Bool __stdcall OnTableRClicked(void *userObj, Math::Coord2D<OSInt> scnPos, MouseButton btn);
			static void __stdcall OnMapSchemaSelChg(void *userObj);
			static void __stdcall OnDatabaseChangeClicked(void *userObj);
			static void __stdcall OnDatabaseDeleteClicked(void *userObj);
			static void __stdcall OnDatabaseNewClicked(void *userObj);
			static void __stdcall OnSQLExecClicked(void *userObj);
			static void __stdcall OnLayerUpdated(void *userObj);

			void UpdateDatabaseList();
			void UpdateSchemaList();
			void UpdateTableList();
			void UpdateMapTableList();
			void UpdateTableData(Text::CString schemaName, Text::String *tableName);
			static void UpdateResult(DB::DBReader *r, UI::GUIListView *lv);

			Data::Class *CreateTableClass(Text::CString schemaName, Text::CString tableName);
			void CopyTableCreate(DB::DBUtil::ServerType svrType);
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
