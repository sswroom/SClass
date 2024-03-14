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
			NotNullPtr<UI::GUILabel> lblDSN;
			NotNullPtr<UI::GUITextBox> txtDSN;
			NotNullPtr<UI::GUILabel> lblUID;
			NotNullPtr<UI::GUITextBox> txtUID;
			NotNullPtr<UI::GUILabel> lblPWD;
			NotNullPtr<UI::GUITextBox> txtPWD;
			NotNullPtr<UI::GUIButton> btnOK;
			NotNullPtr<UI::GUIButton> btnCancel;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			DB::DBConn *conn;

			static void __stdcall OnOKClicked(AnyType userObj);
			static void __stdcall OnCancelClicked(AnyType userObj);
		public:
			AVIRODBCDSNForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRODBCDSNForm();

			virtual void OnMonitorChanged();

			DB::DBConn *GetDBConn();
		};
	}
}
#endif
