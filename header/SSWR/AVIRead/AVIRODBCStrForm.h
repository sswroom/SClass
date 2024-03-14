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
			NotNullPtr<UI::GUILabel> lblDriver;
			NotNullPtr<UI::GUIComboBox> cboDriver;
			NotNullPtr<UI::GUIButton> btnDriverInfo;
			NotNullPtr<UI::GUILabel> lblConnStr;
			NotNullPtr<UI::GUITextBox> txtConnStr;
			NotNullPtr<UI::GUIButton> btnOK;
			NotNullPtr<UI::GUIButton> btnCancel;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			DB::DBConn *conn;

			static void __stdcall OnDriverSelChg(AnyType userObj);
			static void __stdcall OnDriverInfoClicked(AnyType userObj);
			static void __stdcall OnOKClicked(AnyType userObj);
			static void __stdcall OnCancelClicked(AnyType userObj);
		public:
			AVIRODBCStrForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRODBCStrForm();

			virtual void OnMonitorChanged();

			DB::DBConn *GetDBConn();
		};
	}
}
#endif
