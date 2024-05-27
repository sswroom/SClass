#ifndef _SM_SSWR_AVIREAD_AVIRDBMANAGERFORM
#define _SM_SSWR_AVIREAD_AVIRDBMANAGERFORM
#include "Data/ArrayListNN.h"
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
			NN<UI::GUIPopupMenu> mnuTable;
			NN<UI::GUIPopupMenu> mnuSchema;
			NN<UI::GUIPopupMenu> mnuConn;
			NN<UI::GUIMainMenu> mnuMain;

			NN<UI::GUIListBox> lbConn;
			NN<UI::GUIHSplitter> hspConn;
			NN<UI::GUITabControl> tcMain;

			NN<UI::GUITabPage> tpDatabase;
			NN<UI::GUIListBox> lbDatabase;
			NN<UI::GUIButton> btnDatabaseChange;
			NN<UI::GUIButton> btnDatabaseDelete;
			NN<UI::GUIButton> btnDatabaseNew;

			NN<UI::GUITabPage> tpTable;
			NN<UI::GUIPanel> pnlTable;
			NN<UI::GUIListBox> lbSchema;
			NN<UI::GUIHSplitter> hspSchema;
			NN<UI::GUIListBox> lbTable;
			NN<UI::GUIHSplitter> hspTable;
			NN<UI::GUIListView> lvTable;
			NN<UI::GUIVSplitter> vspTable;
			NN<UI::GUIListView> lvTableResult;

			NN<UI::GUITabPage> tpSQL;
			NN<UI::GUIPanel> pnlSQL;
			NN<UI::GUIPanel> pnlSQLCtrl;
			NN<UI::GUIButton> btnSQLExec;
			NN<UI::GUIButton> btnSQLFile;
			NN<UI::GUITextBox> txtSQL;
			NN<UI::GUIVSplitter> vspSQL;
			NN<UI::GUIListView> lvSQLResult;

			NN<UI::GUITabPage> tpMap;
			NN<UI::GUIPanel> pnlMap;
			NN<UI::GUIHSplitter> hspMap;
			UI::GUIMapControl *mapMain;
			NN<UI::GUIPanel> pnlMapTable;
			NN<UI::GUIVSplitter> vspMapRecord;
			NN<UI::GUIListView> lvMapRecord;
			NN<UI::GUIListBox> lbMapSchema;
			NN<UI::GUIHSplitter> hspMapTable;
			NN<UI::GUIListBox> lbMapTable;

			NN<UI::GUITabPage> tpVariable;
			NN<UI::GUIPanel> pnlVariable;
			NN<UI::GUIButton> btnVariable;
			NN<UI::GUIListView> lvVariable;

			NN<UI::GUITabPage> tpSvrConn;
			NN<UI::GUIPanel> pnlSvrConn;
			NN<UI::GUIButton> btnSvrConn;
			NN<UI::GUIButton> btnSvrConnKill;
			NN<UI::GUIListView> lvSvrConn;

			NN<UI::GUITabPage> tpLog;
			NN<UI::GUIListBox> lbLog;
			NN<UI::GUITextBox> txtLog;

			NN<SSWR::AVIRead::AVIRCore> core;
			NN<UI::ListBoxLogger> logger;
			Data::ArrayListNN<DB::DBManagerCtrl> dbList;
			Optional<Net::SSLEngine> ssl;
			IO::LogTool log;
			Optional<DB::ReadingDB> currDB;
			Data::QueryConditions *currCond;
			NN<Media::ColorManagerSess> colorSess;
			NN<Map::MapEnv> mapEnv;
			NN<Map::DBMapLayer> dbLayer;
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
			static void __stdcall OnFileHandler(AnyType userObj, Data::DataArray<NN<Text::String>> files);

			void UpdateDatabaseList();
			void UpdateSchemaList();
			void UpdateTableList();
			void UpdateMapTableList();
			void UpdateTableData(Text::CString schemaName, Optional<Text::String> tableName);
			static void UpdateResult(NN<DB::DBReader> r, NN<UI::GUIListView> lv);
			void UpdateVariableList();
			void UpdateSvrConnList();
			void RunSQLFile(NN<DB::ReadingDBTool> db, NN<Text::String> fileName);

			Optional<Data::Class> CreateTableClass(Text::CString schemaName, Text::CStringNN tableName);
			void CopyTableCreate(DB::SQLType sqlType, Bool axisAware);
			void ExportTableData(DB::SQLType sqlType, Bool axisAware);
			void ExportTableCSV();
			void ExportTableSQLite();
			void ExportTableHTML();
			void ExportTablePList();
			void ExportTableXLSX();
			void ExportTableExcelXML();
		public:
			AVIRDBManagerForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRDBManagerForm();

			virtual void EventMenuClicked(UInt16 cmdId);
			virtual void OnMonitorChanged();

			void ConnAdd(NN<DB::DBConn> conn);
		};
	}
}
#endif
