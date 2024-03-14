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
			NotNullPtr<UI::GUIPopupMenu> mnuTable;
			NotNullPtr<UI::GUIPopupMenu> mnuSchema;
			NotNullPtr<UI::GUIPopupMenu> mnuConn;
			NotNullPtr<UI::GUIMainMenu> mnuMain;

			NotNullPtr<UI::GUIListBox> lbConn;
			NotNullPtr<UI::GUIHSplitter> hspConn;
			NotNullPtr<UI::GUITabControl> tcMain;

			NotNullPtr<UI::GUITabPage> tpDatabase;
			NotNullPtr<UI::GUIListBox> lbDatabase;
			NotNullPtr<UI::GUIButton> btnDatabaseChange;
			NotNullPtr<UI::GUIButton> btnDatabaseDelete;
			NotNullPtr<UI::GUIButton> btnDatabaseNew;

			NotNullPtr<UI::GUITabPage> tpTable;
			NotNullPtr<UI::GUIPanel> pnlTable;
			NotNullPtr<UI::GUIListBox> lbSchema;
			NotNullPtr<UI::GUIHSplitter> hspSchema;
			NotNullPtr<UI::GUIListBox> lbTable;
			NotNullPtr<UI::GUIHSplitter> hspTable;
			NotNullPtr<UI::GUIListView> lvTable;
			NotNullPtr<UI::GUIVSplitter> vspTable;
			NotNullPtr<UI::GUIListView> lvTableResult;

			NotNullPtr<UI::GUITabPage> tpSQL;
			NotNullPtr<UI::GUIPanel> pnlSQL;
			NotNullPtr<UI::GUIPanel> pnlSQLCtrl;
			NotNullPtr<UI::GUIButton> btnSQLExec;
			NotNullPtr<UI::GUIButton> btnSQLFile;
			NotNullPtr<UI::GUITextBox> txtSQL;
			NotNullPtr<UI::GUIVSplitter> vspSQL;
			NotNullPtr<UI::GUIListView> lvSQLResult;

			NotNullPtr<UI::GUITabPage> tpMap;
			NotNullPtr<UI::GUIPanel> pnlMap;
			NotNullPtr<UI::GUIHSplitter> hspMap;
			UI::GUIMapControl *mapMain;
			NotNullPtr<UI::GUIPanel> pnlMapTable;
			NotNullPtr<UI::GUIVSplitter> vspMapRecord;
			NotNullPtr<UI::GUIListView> lvMapRecord;
			NotNullPtr<UI::GUIListBox> lbMapSchema;
			NotNullPtr<UI::GUIHSplitter> hspMapTable;
			NotNullPtr<UI::GUIListBox> lbMapTable;

			NotNullPtr<UI::GUITabPage> tpVariable;
			NotNullPtr<UI::GUIPanel> pnlVariable;
			NotNullPtr<UI::GUIButton> btnVariable;
			NotNullPtr<UI::GUIListView> lvVariable;

			NotNullPtr<UI::GUITabPage> tpSvrConn;
			NotNullPtr<UI::GUIPanel> pnlSvrConn;
			NotNullPtr<UI::GUIButton> btnSvrConn;
			NotNullPtr<UI::GUIButton> btnSvrConnKill;
			NotNullPtr<UI::GUIListView> lvSvrConn;

			NotNullPtr<UI::GUITabPage> tpLog;
			NotNullPtr<UI::GUIListBox> lbLog;
			NotNullPtr<UI::GUITextBox> txtLog;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			NotNullPtr<UI::ListBoxLogger> logger;
			Data::ArrayList<DB::DBManagerCtrl*> dbList;
			Optional<Net::SSLEngine> ssl;
			IO::LogTool log;
			DB::ReadingDB *currDB;
			Data::QueryConditions *currCond;
			NotNullPtr<Media::ColorManagerSess> colorSess;
			NotNullPtr<Map::MapEnv> mapEnv;
			NotNullPtr<Map::DBMapLayer> dbLayer;
			Math::Coord2D<OSInt> mapDownPos;
			Bool sqlFileMode;

			static void __stdcall OnConnSelChg(AnyType userObj);
			static Bool __stdcall OnConnRClicked(AnyType userObj, Math::Coord2D<OSInt> scnPos, MouseButton btn);
			static void __stdcall OnConnDblClicked(AnyType userObj);
			static void __stdcall OnSchemaSelChg(AnyType userObj);
			static Bool __stdcall OnSchemaRClicked(AnyType userObj, Math::Coord2D<OSInt> scnPos, MouseButton btn);
			static void __stdcall OnTableSelChg(AnyType userObj);
			static Bool __stdcall OnTableRClicked(AnyType userObj, Math::Coord2D<OSInt> scnPos, MouseButton btn);
			static void __stdcall OnMapSchemaSelChg(AnyType userObj);
			static void __stdcall OnMapTableSelChg(AnyType userObj);
			static Bool __stdcall OnMapMouseDown(AnyType userObj, Math::Coord2D<OSInt> scnPos, MouseButton button);
			static Bool __stdcall OnMapMouseUp(AnyType userObj, Math::Coord2D<OSInt> scnPos, MouseButton button);
			static void __stdcall OnDatabaseChangeClicked(AnyType userObj);
			static void __stdcall OnDatabaseDeleteClicked(AnyType userObj);
			static void __stdcall OnDatabaseNewClicked(AnyType userObj);
			static void __stdcall OnSQLExecClicked(AnyType userObj);
			static void __stdcall OnSQLFileClicked(AnyType userObj);
			static void __stdcall OnLayerUpdated(AnyType userObj);
			static void __stdcall OnVariableClicked(AnyType userObj);
			static void __stdcall OnSvrConnClicked(AnyType userObj);
			static void __stdcall OnSvrConnKillClicked(AnyType userObj);
			static void __stdcall OnFileHandler(AnyType userObj, Data::DataArray<NotNullPtr<Text::String>> files);

			void UpdateDatabaseList();
			void UpdateSchemaList();
			void UpdateTableList();
			void UpdateMapTableList();
			void UpdateTableData(Text::CString schemaName, Optional<Text::String> tableName);
			static void UpdateResult(NotNullPtr<DB::DBReader> r, NotNullPtr<UI::GUIListView> lv);
			void UpdateVariableList();
			void UpdateSvrConnList();
			void RunSQLFile(DB::ReadingDBTool *db, NotNullPtr<Text::String> fileName);

			Data::Class *CreateTableClass(Text::CString schemaName, Text::CStringNN tableName);
			void CopyTableCreate(DB::SQLType sqlType, Bool axisAware);
			void ExportTableData(DB::SQLType sqlType, Bool axisAware);
			void ExportTableCSV();
			void ExportTableSQLite();
			void ExportTableHTML();
			void ExportTablePList();
			void ExportTableXLSX();
			void ExportTableExcelXML();
		public:
			AVIRDBManagerForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRDBManagerForm();

			virtual void EventMenuClicked(UInt16 cmdId);
			virtual void OnMonitorChanged();

			void ConnAdd(NotNullPtr<DB::DBConn> conn);
		};
	}
}
#endif
