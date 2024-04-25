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
			NN<UI::GUILabel> lblDSN;
			NN<UI::GUITextBox> txtDSN;
			NN<UI::GUILabel> lblUID;
			NN<UI::GUITextBox> txtUID;
			NN<UI::GUILabel> lblPWD;
			NN<UI::GUITextBox> txtPWD;
			NN<UI::GUIButton> btnOK;
			NN<UI::GUIButton> btnCancel;

			NN<SSWR::AVIRead::AVIRCore> core;
			DB::DBConn *conn;

			static void __stdcall OnOKClicked(AnyType userObj);
			static void __stdcall OnCancelClicked(AnyType userObj);
		public:
			AVIRODBCDSNForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRODBCDSNForm();

			virtual void OnMonitorChanged();

			DB::DBConn *GetDBConn();
		};
	}
}
#endif
