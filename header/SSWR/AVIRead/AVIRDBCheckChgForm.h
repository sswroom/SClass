#ifndef _SM_SSWR_AVIREAD_AVIRDBCHECKCHGFORM
#define _SM_SSWR_AVIREAD_AVIRDBCHECKCHGFORM
#include "DB/ReadingDB.h"
#include "IO/Stream.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUICheckBox.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
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
			UI::GUILabel *lblSchema;
			UI::GUITextBox *txtSchema;
			UI::GUILabel *lblTable;
			UI::GUITextBox *txtTable;
			UI::GUILabel *lblKeyCol;
			UI::GUIComboBox *cboKeyCol;
			UI::GUILabel *lblNullCol;
			UI::GUIComboBox *cboNullCol;
			UI::GUICheckBox *chkNoHeader;
			UI::GUICheckBox *chkLocalTZ;
			UI::GUILabel *lblCSV;
			UI::GUITextBox *txtCSV;
			UI::GUIButton *btnBrowse;
			UI::GUILabel *lblCSVRow;
			UI::GUITextBox *txtCSVRow;
			UI::GUILabel *lblNoChg;
			UI::GUITextBox *txtNoChg;
			UI::GUILabel *lblUpdated;
			UI::GUITextBox *txtUpdated;
			UI::GUILabel *lblNewRow;
			UI::GUITextBox *txtNewRow;
			UI::GUILabel *lblDeletedRow;
			UI::GUITextBox *txtDeletedRow;

			UI::GUILabel *lblDBType;
			UI::GUIComboBox *cboDBType;
			UI::GUICheckBox *chkAxisAware;
			UI::GUICheckBox *chkMultiRow;
			UI::GUIButton *btnSQL;
			UI::GUIButton *btnExecute;

			UI::GUILabel *lblStatTime;
			UI::GUITextBox *txtStatTime;
			UI::GUILabel *lblStatus;
			UI::GUITextBox *txtStatus;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			DB::ReadingDB *db;
			Text::CString schema;
			Text::CString table;

			static void __stdcall OnBrowseClk(void *userObj);
			static void __stdcall OnFiles(void *userObj, NotNullPtr<Text::String> *files, UOSInt nFiles);
			static void __stdcall OnSQLClicked(void *userObj);
			static void __stdcall OnExecuteClicked(void *userObj);
			Bool LoadCSV(Text::CStringNN fileName);
			Bool GenerateSQL(Text::CStringNN csvFileName, DB::SQLType sqlType, Bool axisAware, SQLSession *sess);
			Bool NextSQL(Text::CString sql, SQLSession *sess);
			void UpdateStatus(SQLSession *sess);
			static void __stdcall AppendCol(DB::SQLBuilder *sql, DB::DBUtil::ColType colType, Text::String *s, Int8 tzQhr);
			Text::CString GetNullText();
		public:
			AVIRDBCheckChgForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, DB::ReadingDB *db, Text::CString schema, Text::CString table);
			virtual ~AVIRDBCheckChgForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
