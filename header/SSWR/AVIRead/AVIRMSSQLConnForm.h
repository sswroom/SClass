#ifndef _SM_SSWR_AVIREAD_AVIRMSSQLCONNFORM
#define _SM_SSWR_AVIREAD_AVIRMSSQLCONNFORM
#include "DB/DBConn.h"
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
		class AVIRMSSQLConnForm : public UI::GUIForm
		{
		private:
			NotNullPtr<UI::GUILabel> lblDriver;
			UI::GUITextBox *txtDriver;
			NotNullPtr<UI::GUILabel> lblServer;
			UI::GUITextBox *txtServer;
			NotNullPtr<UI::GUIButton> btnPasteJDBC;
			NotNullPtr<UI::GUILabel> lblPort;
			UI::GUITextBox *txtPort;
			UI::GUICheckBox *chkEncrypt;
			NotNullPtr<UI::GUILabel> lblDatabase;
			UI::GUITextBox *txtDatabase;
			NotNullPtr<UI::GUILabel> lblUser;
			UI::GUITextBox *txtUser;
			NotNullPtr<UI::GUILabel> lblPassword;
			UI::GUITextBox *txtPassword;
			NotNullPtr<UI::GUIButton> btnOK;
			NotNullPtr<UI::GUIButton> btnCancel;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Optional<DB::DBConn> conn;
			Bool isError;

			static void __stdcall OnPasteJDBCClicked(void *userObj);
			static void __stdcall OnOKClicked(void *userObj);
			static void __stdcall OnCancelClicked(void *userObj);
		public:
			AVIRMSSQLConnForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRMSSQLConnForm();

			virtual void OnMonitorChanged();

			Bool IsDriverNotFound();
			Optional<DB::DBConn> GetDBConn();
		};
	}
}
#endif
