#ifndef _SM_SSWR_AVIREAD_AVIROLEDBFORM
#define _SM_SSWR_AVIREAD_AVIROLEDBFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIROLEDBForm : public UI::GUIForm
		{
		private:
			NN<UI::GUILabel> lblConnStr;
			NN<UI::GUITextBox> txtConnStr;
			NN<UI::GUIButton> btnOK;
			NN<UI::GUIButton> btnCancel;

			NN<SSWR::AVIRead::AVIRCore> core;
			DB::DBConn *conn;

			static void __stdcall OnOKClicked(AnyType userObj);
			static void __stdcall OnCancelClicked(AnyType userObj);
		public:
			AVIROLEDBForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIROLEDBForm();

			virtual void OnMonitorChanged();

			DB::DBConn *GetDBConn();
		};
	}
}
#endif
