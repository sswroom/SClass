#ifndef _SM_SSWR_AVIREAD_AVIRPOSTGRESQLFORM
#define _SM_SSWR_AVIREAD_AVIRPOSTGRESQLFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRPostgreSQLForm : public UI::GUIForm
		{
		private:
			UI::GUILabel *lblServer;
			UI::GUITextBox *txtServer;
			UI::GUILabel *lblPort;
			UI::GUITextBox *txtPort;
			UI::GUILabel *lblUID;
			UI::GUITextBox *txtUID;
			UI::GUILabel *lblPWD;
			UI::GUITextBox *txtPWD;
			UI::GUILabel *lblDatabase;
			UI::GUITextBox *txtDatabase;
			UI::GUIButton *btnOK;
			UI::GUIButton *btnCancel;

			SSWR::AVIRead::AVIRCore *core;
			DB::DBConn *conn;

			static void __stdcall OnOKClicked(void *userObj);
			static void __stdcall OnCancelClicked(void *userObj);
		public:
			AVIRPostgreSQLForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRPostgreSQLForm();

			virtual void OnMonitorChanged();

			DB::DBConn *GetDBConn();
		};
	}
}
#endif
