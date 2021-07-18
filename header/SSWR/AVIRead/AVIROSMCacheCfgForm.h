#ifndef _SM_SSWR_AVIREAD_AVIROSMCACHECFGFORM
#define _SM_SSWR_AVIREAD_AVIROSMCACHECFGFORM
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUITextBox.h"
#include "SSWR/AVIRead/AVIRCore.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIROSMCacheCfgForm : public UI::GUIForm
		{
		private:
			SSWR::AVIRead::AVIRCore *core;
			UI::GUILabel *lblMapType;
			UI::GUIComboBox *cboMapType;
			UI::GUILabel *lblHTTPPort;
			UI::GUITextBox *txtHTTPPort;

			UI::GUIButton *btnOK;
			UI::GUIButton *btnCancel;

			static void __stdcall OnOKClick(void *userObj);
			static void __stdcall OnCancelClick(void *userObj);
		public:
			AVIROSMCacheCfgForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIROSMCacheCfgForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
