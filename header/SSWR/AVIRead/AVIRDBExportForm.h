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
			NotNullPtr<UI::GUIPanel> pnlMain;
			NotNullPtr<UI::GUIListView> lvTables;
			NotNullPtr<UI::GUILabel> lblDBType;
			NotNullPtr<UI::GUIComboBox> cboDBType;
			NotNullPtr<UI::GUICheckBox> chkAxisAware;
			NotNullPtr<UI::GUILabel> lblSchema;
			NotNullPtr<UI::GUITextBox> txtSchema;
			NotNullPtr<UI::GUILabel> lblTable;
			NotNullPtr<UI::GUITextBox> txtTable;
			NotNullPtr<UI::GUIButton> btnExport;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			DB::ReadingDB *db;
			Text::CString schema;
			Text::CString table;

			static void __stdcall OnTablesDblClk(void *userObj, UOSInt itemIndex);
			static void __stdcall OnExportClicked(void *userObj);
		public:
			AVIRDBExportForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, DB::ReadingDB *db, Text::CString schema, Text::CString table);
			virtual ~AVIRDBExportForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
