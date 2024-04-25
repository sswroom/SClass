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
			NN<SSWR::AVIRead::AVIRCore> core;
			Optional<Net::SSLEngine> ssl;
			NN<UI::GUILabel> lblMapType;
			NN<UI::GUIComboBox> cboMapType;
			NN<UI::GUILabel> lblHTTPPort;
			NN<UI::GUITextBox> txtHTTPPort;

			NN<UI::GUIButton> btnOK;
			NN<UI::GUIButton> btnCancel;

			static void __stdcall OnOKClick(AnyType userObj);
			static void __stdcall OnCancelClick(AnyType userObj);
		public:
			AVIROSMCacheCfgForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIROSMCacheCfgForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
