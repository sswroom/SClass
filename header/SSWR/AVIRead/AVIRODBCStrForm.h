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
			UI::GUILabel *lblDriver;
			UI::GUIComboBox *cboDriver;
			UI::GUIButton *btnDriverInfo;
			UI::GUILabel *lblConnStr;
			UI::GUITextBox *txtConnStr;
			UI::GUIButton *btnOK;
			UI::GUIButton *btnCancel;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			DB::DBConn *conn;

			static void __stdcall OnDriverSelChg(void *userObj);
			static void __stdcall OnDriverInfoClicked(void *userObj);
			static void __stdcall OnOKClicked(void *userObj);
			static void __stdcall OnCancelClicked(void *userObj);
		public:
			AVIRODBCStrForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRODBCStrForm();

			virtual void OnMonitorChanged();

			DB::DBConn *GetDBConn();
		};
	}
}
#endif
