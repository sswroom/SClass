#ifndef _SM_SSWR_AVIREAD_AVIRMSSQLCONNFORM
#define _SM_SSWR_AVIREAD_AVIRMSSQLCONNFORM
#include "DB/DBConn.h"
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
		class AVIRMSSQLConnForm : public UI::GUIForm
		{
		private:
			UI::GUILabel *lblDriver;
			UI::GUITextBox *txtDriver;
			UI::GUILabel *lblServer;
			UI::GUITextBox *txtServer;
			UI::GUIButton *btnPasteJDBC;
			UI::GUILabel *lblPort;
			UI::GUITextBox *txtPort;
			UI::GUILabel *lblDatabase;
			UI::GUITextBox *txtDatabase;
			UI::GUILabel *lblUser;
			UI::GUITextBox *txtUser;
			UI::GUILabel *lblPassword;
			UI::GUITextBox *txtPassword;
			UI::GUIButton *btnOK;
			UI::GUIButton *btnCancel;

			SSWR::AVIRead::AVIRCore *core;
			DB::DBConn *conn;
			Bool isError;

			static void __stdcall OnPasteJDBCClicked(void *userObj);
			static void __stdcall OnOKClicked(void *userObj);
			static void __stdcall OnCancelClicked(void *userObj);
		public:
			AVIRMSSQLConnForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRMSSQLConnForm();

			virtual void OnMonitorChanged();

			Bool IsDriverNotFound();
			DB::DBConn *GetDBConn();
		};
	}
}
#endif
