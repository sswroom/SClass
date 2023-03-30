#ifndef _SM_SSWR_AVIREAD_AVIRDBEXPORTFORM
#define _SM_SSWR_AVIREAD_AVIRDBEXPORTFORM
#include "DB/ReadingDB.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUICheckBox.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRDBExportForm : public UI::GUIForm
		{
		private:
			UI::GUIPanel *pnlMain;
			UI::GUIListView *lvTables;
			UI::GUILabel *lblDBType;
			UI::GUIComboBox *cboDBType;
			UI::GUICheckBox *chkAxisAware;
			UI::GUILabel *lblSchema;
			UI::GUITextBox *txtSchema;
			UI::GUILabel *lblTable;
			UI::GUITextBox *txtTable;
			UI::GUIButton *btnExport;

			SSWR::AVIRead::AVIRCore *core;
			DB::ReadingDB *db;
			Text::CString schema;
			Text::CString table;

			static void __stdcall OnTablesDblClk(void *userObj, UOSInt itemIndex);
			static void __stdcall OnExportClicked(void *userObj);
		public:
			AVIRDBExportForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, DB::ReadingDB *db, Text::CString schema, Text::CString table);
			virtual ~AVIRDBExportForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
