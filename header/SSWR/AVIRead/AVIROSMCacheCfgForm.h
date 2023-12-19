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
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Optional<Net::SSLEngine> ssl;
			NotNullPtr<UI::GUILabel> lblMapType;
			NotNullPtr<UI::GUIComboBox> cboMapType;
			NotNullPtr<UI::GUILabel> lblHTTPPort;
			NotNullPtr<UI::GUITextBox> txtHTTPPort;

			NotNullPtr<UI::GUIButton> btnOK;
			NotNullPtr<UI::GUIButton> btnCancel;

			static void __stdcall OnOKClick(void *userObj);
			static void __stdcall OnCancelClick(void *userObj);
		public:
			AVIROSMCacheCfgForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIROSMCacheCfgForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
