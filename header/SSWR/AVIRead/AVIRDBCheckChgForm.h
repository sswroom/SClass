#ifndef _SM_SSWR_AVIREAD_AVIRDBCHECKCHGFORM
#define _SM_SSWR_AVIREAD_AVIRDBCHECKCHGFORM
#include "DB/DBConn.h"
#include "DB/ReadingDB.h"
#include "DB/SQLBuilder.h"
#include "IO/Stream.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "SSWR/AVIRead/AVIRDBManager.h"
#include "UI/GUIButton.h"
#include "UI/GUICheckBox.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUIGroupBox.h"
#include "UI/GUILabel.h"
#include "UI/GUIRadioButton.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRDBCheckChgForm : public UI::GUIForm
		{
		private:
			struct SQLSession
			{
				UOSInt mode;
				Optional<IO::Stream> stm;
				UOSInt totalCnt;
				Data::Timestamp startTime;
				Data::Timestamp lastUpdateTime;
				NN<Text::StringBuilderUTF8> sbInsert;
				UOSInt nInsert;
				Optional<DB::DBConn> db;
			};
		private:
			NN<UI::GUILabel> lblSchema;
			NN<UI::GUITextBox> txtSchema;
			NN<UI::GUILabel> lblTable;
			NN<UI::GUITextBox> txtTable;
			NN<UI::GUILabel> lblSrcFilter;
			NN<UI::GUITextBox> txtSrcFilter;

			NN<UI::GUIGroupBox> grpData;
			NN<UI::GUICheckBox> chkNoHeader;
			NN<UI::GUICheckBox> chkCSVUTCTime;
			NN<UI::GUIRadioButton> radDataFile;
			NN<UI::GUITextBox> txtDataFile;
			NN<UI::GUIRadioButton> radDataConn;
			NN<UI::GUIComboBox> cboDataConn;
			NN<UI::GUIButton> btnDataFile;
			NN<UI::GUILabel> lblDataTable;
			NN<UI::GUIComboBox> cboDataTable;
			NN<UI::GUILabel> lblKeyCol;
			NN<UI::GUIComboBox> cboKeyCol1;
			NN<UI::GUIComboBox> cboKeyCol2;
			NN<UI::GUILabel> lblNullCol;
			NN<UI::GUIComboBox> cboNullCol;
			NN<UI::GUILabel> lblAssignCol;
			NN<UI::GUIButton> btnAssignCol;
			NN<UI::GUILabel> lblDataFilter;
			NN<UI::GUITextBox> txtDataFilter;
			NN<UI::GUIButton> btnDataCheck;

			NN<UI::GUILabel> lblDataFileRow;
			NN<UI::GUITextBox> txtDataFileRow;
			NN<UI::GUILabel> lblNoChg;
			NN<UI::GUITextBox> txtNoChg;
			NN<UI::GUILabel> lblUpdated;
			NN<UI::GUITextBox> txtUpdated;
			NN<UI::GUILabel> lblNewRow;
			NN<UI::GUITextBox> txtNewRow;
			NN<UI::GUILabel> lblDeletedRow;
			NN<UI::GUITextBox> txtDeletedRow;

			NN<UI::GUILabel> lblDBType;
			NN<UI::GUIComboBox> cboDBType;
			NN<UI::GUICheckBox> chkAxisAware;
			NN<UI::GUICheckBox> chkMultiRow;
			NN<UI::GUIButton> btnSQL;
			NN<UI::GUIButton> btnExecute;

			NN<UI::GUILabel> lblStatTime;
			NN<UI::GUITextBox> txtStatTime;
			NN<UI::GUILabel> lblStatus;
			NN<UI::GUITextBox> txtStatus;

			NN<SSWR::AVIRead::AVIRCore> core;
			Data::ArrayList<UOSInt> colInd;
			Data::ArrayListStringNN colStr;
			NN<DB::ReadingDB> db;
			Text::CString schema;
			Text::CStringNN table;
			NN<SSWR::AVIRead::AVIRDBManager> dbMgr;
			Bool inited;

			Optional<DB::ReadingDB> relDataFile;
			Bool dataFileNoHeader;

			Optional<DB::ReadingDB> dataConn;
			Int8 connTz;

			static void __stdcall OnDataFileClk(AnyType userObj);
			static void __stdcall OnFiles(AnyType userObj, Data::DataArray<NN<Text::String>> files);
			static void __stdcall OnDataCheckClk(AnyType userObj);
			static void __stdcall OnSQLClicked(AnyType userObj);
			static void __stdcall OnExecuteClicked(AnyType userObj);
			static void __stdcall OnDataTableChg(AnyType userObj);
			static void __stdcall OnAssignColClicked(AnyType userObj);
			static void __stdcall OnDataConnSelChg(AnyType userObj, Bool newState);
			static void __stdcall OnDataConnCboSelChg(AnyType userObj);
			Optional<Text::String> GetNewText(UOSInt colIndex);
			NN<Text::String> GetNewTextNN(UOSInt colIndex);
			Bool LoadDataFile(Text::CStringNN fileName);
			Bool InitConn(NN<DB::ReadingDB> conn, Int8 connTz);
			Bool GetColIndex(NN<Data::ArrayList<UOSInt>> colInd, NN<DB::TableDef> destTable, Text::CString srcSchema, Text::CStringNN srcTable);
			Bool IsColIndexValid(NN<Data::ArrayList<UOSInt>> colInd, NN<DB::TableDef> destTable);
			Bool CheckDataFile();
			Bool GenerateSQL(DB::SQLType sqlType, Bool axisAware, NN<SQLSession> sess);
			Bool NextSQL(Text::CStringNN sql, NN<SQLSession> sess);
			void UpdateStatus(NN<SQLSession> sess);
			static void __stdcall AppendCol(NN<DB::SQLBuilder> sql, NN<DB::ColDef> col, Optional<Text::String> s, Int8 tzQhr, UInt32 srid);
			Text::CStringNN GetNullText();
			DB::SQLType GetDBSQLType();
		public:
			AVIRDBCheckChgForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<DB::ReadingDB> db, Text::CString schema, Text::CStringNN table, NN<SSWR::AVIRead::AVIRDBManager> dbMgr);
			virtual ~AVIRDBCheckChgForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
