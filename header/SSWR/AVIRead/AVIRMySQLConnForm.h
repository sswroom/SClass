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
			NN<UI::GUILabel> lblServer;
			NN<UI::GUITextBox> txtServer;
			NN<UI::GUILabel> lblPort;
			NN<UI::GUITextBox> txtPort;
			NN<UI::GUILabel> lblUID;
			NN<UI::GUITextBox> txtUID;
			NN<UI::GUILabel> lblPWD;
			NN<UI::GUITextBox> txtPWD;
			NN<UI::GUILabel> lblDatabase;
			NN<UI::GUITextBox> txtDatabase;
			NN<UI::GUIButton> btnOK;
			NN<UI::GUIButton> btnCancel;

			NN<SSWR::AVIRead::AVIRCore> core;
			Optional<DB::DBConn> conn;

			static void __stdcall OnOKClicked(AnyType userObj);
			static void __stdcall OnCancelClicked(AnyType userObj);
		public:
			AVIRMySQLConnForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRMySQLConnForm();

			virtual void OnMonitorChanged();

			Optional<DB::DBConn> GetDBConn();
		};
	}
}
#endif
