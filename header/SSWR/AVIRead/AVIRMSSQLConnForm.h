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
			NN<UI::GUILabel> lblDriver;
			NN<UI::GUITextBox> txtDriver;
			NN<UI::GUILabel> lblServer;
			NN<UI::GUITextBox> txtServer;
			NN<UI::GUIButton> btnPasteJDBC;
			NN<UI::GUILabel> lblPort;
			NN<UI::GUITextBox> txtPort;
			NN<UI::GUICheckBox> chkEncrypt;
			NN<UI::GUILabel> lblDatabase;
			NN<UI::GUITextBox> txtDatabase;
			NN<UI::GUILabel> lblUser;
			NN<UI::GUITextBox> txtUser;
			NN<UI::GUILabel> lblPassword;
			NN<UI::GUITextBox> txtPassword;
			NN<UI::GUIButton> btnOK;
			NN<UI::GUIButton> btnCancel;

			NN<SSWR::AVIRead::AVIRCore> core;
			Optional<DB::DBConn> conn;
			Bool isError;

			static void __stdcall OnPasteJDBCClicked(AnyType userObj);
			static void __stdcall OnOKClicked(AnyType userObj);
			static void __stdcall OnCancelClicked(AnyType userObj);
		public:
			AVIRMSSQLConnForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRMSSQLConnForm();

			virtual void OnMonitorChanged();

			Bool IsDriverNotFound();
			Optional<DB::DBConn> GetDBConn();
		};
	}
}
#endif
