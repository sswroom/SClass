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
			NN<UI::GUIPanel> pnlMain;
			NN<UI::GUIListView> lvTables;
			NN<UI::GUILabel> lblDBType;
			NN<UI::GUIComboBox> cboDBType;
			NN<UI::GUICheckBox> chkAxisAware;
			NN<UI::GUILabel> lblSchema;
			NN<UI::GUITextBox> txtSchema;
			NN<UI::GUILabel> lblTable;
			NN<UI::GUITextBox> txtTable;
			NN<UI::GUIButton> btnExport;

			NN<SSWR::AVIRead::AVIRCore> core;
			NN<DB::ReadingDB> db;
			Text::CString schema;
			Text::CStringNN table;

			static void __stdcall OnTablesDblClk(AnyType userObj, UIntOS itemIndex);
			static void __stdcall OnExportClicked(AnyType userObj);
		public:
			AVIRDBExportForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<DB::ReadingDB> db, Text::CString schema, Text::CStringNN table);
			virtual ~AVIRDBExportForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
