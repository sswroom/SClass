#ifndef _SM_SSWR_AVIREAD_AVIRODBCDSNFORM
#define _SM_SSWR_AVIREAD_AVIRODBCDSNFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRODBCDSNForm : public UI::GUIForm
		{
		private:
			UI::GUILabel *lblDSN;
			UI::GUITextBox *txtDSN;
			UI::GUILabel *lblUID;
			UI::GUITextBox *txtUID;
			UI::GUILabel *lblPWD;
			UI::GUITextBox *txtPWD;
			UI::GUIButton *btnOK;
			UI::GUIButton *btnCancel;

			SSWR::AVIRead::AVIRCore *core;
			DB::DBConn *conn;

			static void __stdcall OnOKClicked(void *userObj);
			static void __stdcall OnCancelClicked(void *userObj);
		public:
			AVIRODBCDSNForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRODBCDSNForm();

			virtual void OnMonitorChanged();

			DB::DBConn *GetDBConn();
		};
	}
}
#endif
