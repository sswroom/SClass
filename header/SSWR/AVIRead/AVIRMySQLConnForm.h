#ifndef _SM_SSWR_AVIREAD_AVIRMYSQLCONNFORM
#define _SM_SSWR_AVIREAD_AVIRMYSQLCONNFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRMySQLConnForm : public UI::GUIForm
		{
		private:
			UI::GUILabel *lblServer;
			UI::GUITextBox *txtServer;
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
			AVIRMySQLConnForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRMySQLConnForm();

			virtual void OnMonitorChanged();

			DB::DBConn *GetDBConn();
		};
	}
}
#endif
