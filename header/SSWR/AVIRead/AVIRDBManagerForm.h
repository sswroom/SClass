#ifndef _SM_SSWR_AVIREAD_AVIRDBMANAGERFORM
#define _SM_SSWR_AVIREAD_AVIRDBMANAGERFORM
#include "DB/DBManagerCtrl.h"
#include "Map/DBMapLayer.h"
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
#include "UI/ListBoxLogger.h"

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
			UI::GUIButton *btnSQLFile;
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

			UI::GUITabPage *tpVariable;
			UI::GUIPanel *pnlVariable;
			UI::GUIButton *btnVariable;
			UI::GUIListView *lvVariable;

			UI::GUITabPage *tpSvrConn;
			UI::GUIPanel *pnlSvrConn;
			UI::GUIButton *btnSvrConn;
			UI::GUIButton *btnSvrConnKill;
			UI::GUIListView *lvSvrConn;

			UI::GUITabPage *tpLog;
			UI::GUIListBox *lbLog;
			UI::GUITextBox *txtLog;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			UI::ListBoxLogger *logger;
			Data::ArrayList<DB::DBManagerCtrl*> dbList;
			Net::SSLEngine *ssl;
			IO::LogTool log;
			DB::ReadingDB *currDB;
			NotNullPtr<Media::ColorManagerSess> colorSess;
			Map::MapEnv *mapEnv;
			Map::DBMapLayer *dbLayer;
			Math::Coord2D<OSInt> mapDownPos;

			static void __stdcall OnConnSelChg(void *userObj);
			static Bool __stdcall OnConnRClicked(void *userObj, Math::Coord2D<OSInt> scnPos, MouseButton btn);
			static void __stdcall OnConnDblClicked(void *userObj);
			static void __stdcall OnSchemaSelChg(void *userObj);
			static Bool __stdcall OnSchemaRClicked(void *userObj, Math::Coord2D<OSInt> scnPos, MouseButton btn);
			static void __stdcall OnTableSelChg(void *userObj);
			static Bool __stdcall OnTableRClicked(void *userObj, Math::Coord2D<OSInt> scnPos, MouseButton btn);
			static void __stdcall OnMapSchemaSelChg(void *userObj);
			static void __stdcall OnMapTableSelChg(void *userObj);
			static Bool __stdcall OnMapMouseDown(void *userObj, Math::Coord2D<OSInt> scnPos, MouseButton button);
			static Bool __stdcall OnMapMouseUp(void *userObj, Math::Coord2D<OSInt> scnPos, MouseButton button);
			static void __stdcall OnDatabaseChangeClicked(void *userObj);
			static void __stdcall OnDatabaseDeleteClicked(void *userObj);
			static void __stdcall OnDatabaseNewClicked(void *userObj);
			static void __stdcall OnSQLExecClicked(void *userObj);
			static void __stdcall OnSQLFileClicked(void *userObj);
			static void __stdcall OnLayerUpdated(void *userObj);
			static void __stdcall OnVariableClicked(void *userObj);
			static void __stdcall OnSvrConnClicked(void *userObj);
			static void __stdcall OnSvrConnKillClicked(void *userObj);
			static void __stdcall OnFileHandler(void *userObj, NotNullPtr<Text::String> *files, UOSInt nFiles);

			void UpdateDatabaseList();
			void UpdateSchemaList();
			void UpdateTableList();
			void UpdateMapTableList();
			void UpdateTableData(Text::CString schemaName, Text::String *tableName);
			static void UpdateResult(DB::DBReader *r, UI::GUIListView *lv);
			void UpdateVariableList();
			void UpdateSvrConnList();

			Data::Class *CreateTableClass(Text::CString schemaName, Text::CString tableName);
			void CopyTableCreate(DB::SQLType sqlType, Bool axisAware);
			void ExportTableData(DB::SQLType sqlType, Bool axisAware);
			void ExportTableCSV();
		public:
			AVIRDBManagerForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRDBManagerForm();

			virtual void EventMenuClicked(UInt16 cmdId);
			virtual void OnMonitorChanged();

			void ConnAdd(DB::DBConn *conn);
		};
	}
}
#endif
