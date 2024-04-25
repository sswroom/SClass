#ifndef _SM_SSWR_AVIREAD_AVIRODBCSTRFORM
#define _SM_SSWR_AVIREAD_AVIRODBCSTRFORM
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
		class AVIRODBCStrForm : public UI::GUIForm
		{
		private:
			NN<UI::GUILabel> lblDriver;
			NN<UI::GUIComboBox> cboDriver;
			NN<UI::GUIButton> btnDriverInfo;
			NN<UI::GUILabel> lblConnStr;
			NN<UI::GUITextBox> txtConnStr;
			NN<UI::GUIButton> btnOK;
			NN<UI::GUIButton> btnCancel;

			NN<SSWR::AVIRead::AVIRCore> core;
			DB::DBConn *conn;

			static void __stdcall OnDriverSelChg(AnyType userObj);
			static void __stdcall OnDriverInfoClicked(AnyType userObj);
			static void __stdcall OnOKClicked(AnyType userObj);
			static void __stdcall OnCancelClicked(AnyType userObj);
		public:
			AVIRODBCStrForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRODBCStrForm();

			virtual void OnMonitorChanged();

			DB::DBConn *GetDBConn();
		};
	}
}
#endif
