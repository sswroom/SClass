#ifndef _SM_SSWR_AVIREAD_AVIRDBCHECKCHGFORM
#define _SM_SSWR_AVIREAD_AVIRDBCHECKCHGFORM
#include "DB/ReadingDB.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
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
			UI::GUILabel *lblSchema;
			UI::GUITextBox *txtSchema;
			UI::GUILabel *lblTable;
			UI::GUITextBox *txtTable;
			UI::GUILabel *lblCSV;
			UI::GUITextBox *txtCSV;
			UI::GUIButton *btnBrowse;
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
			UI::GUIButton *btnSQL;

			SSWR::AVIRead::AVIRCore *core;
			DB::ReadingDB *db;
			Text::CString schema;
			Text::CString table;

			static void __stdcall OnBrowseClk(void *userObj);
			static void __stdcall OnFiles(void *userObj, Text::String **files, UOSInt nFiles);
			static void __stdcall OnSQLClicked(void *userObj);
			Bool LoadCSV(Text::CString fileName);
			Bool GenerateSQL(Text::CString csvFileName, Text::CString sqlFileName, DB::DBUtil::SQLType sqlType);
			static void __stdcall AppendCol(DB::SQLBuilder *sql, DB::DBUtil::ColType colType, Text::String *s);
		public:
			AVIRDBCheckChgForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, DB::ReadingDB *db, Text::CString schema, Text::CString table);
			virtual ~AVIRDBCheckChgForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
