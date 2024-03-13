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
			NotNullPtr<UI::GUILabel> lblServer;
			NotNullPtr<UI::GUITextBox> txtServer;
			NotNullPtr<UI::GUILabel> lblPort;
			NotNullPtr<UI::GUITextBox> txtPort;
			NotNullPtr<UI::GUILabel> lblUID;
			NotNullPtr<UI::GUITextBox> txtUID;
			NotNullPtr<UI::GUILabel> lblPWD;
			NotNullPtr<UI::GUITextBox> txtPWD;
			NotNullPtr<UI::GUILabel> lblDatabase;
			NotNullPtr<UI::GUITextBox> txtDatabase;
			NotNullPtr<UI::GUIButton> btnOK;
			NotNullPtr<UI::GUIButton> btnCancel;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			DB::DBConn *conn;

			static void __stdcall OnOKClicked(void *userObj);
			static void __stdcall OnCancelClicked(void *userObj);
		public:
			AVIRPostgreSQLForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRPostgreSQLForm();

			virtual void OnMonitorChanged();

			DB::DBConn *GetDBConn();
		};
	}
}
#endif
