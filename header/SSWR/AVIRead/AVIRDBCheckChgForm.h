#ifndef _SM_SSWR_AVIREAD_AVIRDBCHECKCHGFORM
#define _SM_SSWR_AVIREAD_AVIRDBCHECKCHGFORM
#include "DB/DBConn.h"
#include "DB/ReadingDB.h"
#include "DB/SQLBuilder.h"
#include "IO/Stream.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUICheckBox.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUIGroupBox.h"
#include "UI/GUILabel.h"
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
				IO::Stream *stm;
				UOSInt totalCnt;
				Data::Timestamp startTime;
				Data::Timestamp lastUpdateTime;
				NotNullPtr<Text::StringBuilderUTF8> sbInsert;
				UOSInt nInsert;
				DB::DBConn *db;
			};
		private:
			NotNullPtr<UI::GUILabel> lblSchema;
			NotNullPtr<UI::GUITextBox> txtSchema;
			NotNullPtr<UI::GUILabel> lblTable;
			NotNullPtr<UI::GUITextBox> txtTable;
			NotNullPtr<UI::GUILabel> lblSrcFilter;
			NotNullPtr<UI::GUITextBox> txtSrcFilter;

			NotNullPtr<UI::GUIGroupBox> grpData;
			NotNullPtr<UI::GUICheckBox> chkNoHeader;
			NotNullPtr<UI::GUILabel> lblDataFile;
			NotNullPtr<UI::GUITextBox> txtDataFile;
			NotNullPtr<UI::GUIButton> btnDataFile;
			NotNullPtr<UI::GUILabel> lblDataTable;
			NotNullPtr<UI::GUIComboBox> cboDataTable;
			NotNullPtr<UI::GUILabel> lblKeyCol;
			NotNullPtr<UI::GUIComboBox> cboKeyCol;
			NotNullPtr<UI::GUILabel> lblNullCol;
			NotNullPtr<UI::GUIComboBox> cboNullCol;
			NotNullPtr<UI::GUILabel> lblAssignCol;
			NotNullPtr<UI::GUIButton> btnAssignCol;
			NotNullPtr<UI::GUILabel> lblDataFilter;
			NotNullPtr<UI::GUITextBox> txtDataFilter;
			NotNullPtr<UI::GUIButton> btnDataCheck;

			NotNullPtr<UI::GUILabel> lblDataFileRow;
			NotNullPtr<UI::GUITextBox> txtDataFileRow;
			NotNullPtr<UI::GUILabel> lblNoChg;
			NotNullPtr<UI::GUITextBox> txtNoChg;
			NotNullPtr<UI::GUILabel> lblUpdated;
			NotNullPtr<UI::GUITextBox> txtUpdated;
			NotNullPtr<UI::GUILabel> lblNewRow;
			NotNullPtr<UI::GUITextBox> txtNewRow;
			NotNullPtr<UI::GUILabel> lblDeletedRow;
			NotNullPtr<UI::GUITextBox> txtDeletedRow;

			NotNullPtr<UI::GUILabel> lblDBType;
			NotNullPtr<UI::GUIComboBox> cboDBType;
			NotNullPtr<UI::GUICheckBox> chkAxisAware;
			NotNullPtr<UI::GUICheckBox> chkMultiRow;
			NotNullPtr<UI::GUIButton> btnSQL;
			NotNullPtr<UI::GUIButton> btnExecute;

			NotNullPtr<UI::GUILabel> lblStatTime;
			NotNullPtr<UI::GUITextBox> txtStatTime;
			NotNullPtr<UI::GUILabel> lblStatus;
			NotNullPtr<UI::GUITextBox> txtStatus;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Data::ArrayList<UOSInt> colInd;
			DB::ReadingDB *db;
			Text::CString schema;
			Text::CStringNN table;

			DB::ReadingDB *dataFile;
			Bool dataFileNoHeader;
			Int8 dataFileTz;

			static void __stdcall OnDataFileClk(AnyType userObj);
			static void __stdcall OnFiles(AnyType userObj, Data::DataArray<NotNullPtr<Text::String>> files);
			static void __stdcall OnDataCheckClk(AnyType userObj);
			static void __stdcall OnSQLClicked(AnyType userObj);
			static void __stdcall OnExecuteClicked(AnyType userObj);
			static void __stdcall OnDataTableChg(AnyType userObj);
			static void __stdcall OnAssignColClicked(AnyType userObj);
			Bool LoadDataFile(Text::CStringNN fileName);
			Bool GetColIndex(NotNullPtr<Data::ArrayList<UOSInt>> colInd, NotNullPtr<DB::TableDef> destTable, Text::CString srcSchema, Text::CStringNN srcTable);
			Bool IsColIndexValid(NotNullPtr<Data::ArrayList<UOSInt>> colInd, NotNullPtr<DB::TableDef> destTable);
			Bool CheckDataFile();
			Bool GenerateSQL(DB::SQLType sqlType, Bool axisAware, NotNullPtr<SQLSession> sess);
			Bool NextSQL(Text::CStringNN sql, NotNullPtr<SQLSession> sess);
			void UpdateStatus(NotNullPtr<SQLSession> sess);
			static void __stdcall AppendCol(NotNullPtr<DB::SQLBuilder> sql, NotNullPtr<DB::ColDef> col, Optional<Text::String> s, Int8 tzQhr);
			Text::CString GetNullText();
			DB::SQLType GetDBSQLType();
		public:
			AVIRDBCheckChgForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, DB::ReadingDB *db, Text::CString schema, Text::CStringNN table);
			virtual ~AVIRDBCheckChgForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
