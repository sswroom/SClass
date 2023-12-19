#ifndef _SM_SSWR_AVIREAD_AVIRASMCONVFORM
#define _SM_SSWR_AVIREAD_AVIRASMCONVFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUIPanel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRAsmConvForm : public UI::GUIForm
		{
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;

			NotNullPtr<UI::GUIPanel> pnlCtrl;
			NotNullPtr<UI::GUIButton> btnConv;
			NotNullPtr<UI::GUIButton> btnConv2;
			NotNullPtr<UI::GUITextBox> txtIntelAsm;
			NotNullPtr<UI::GUIHSplitter> hspAsm;
			NotNullPtr<UI::GUITextBox> txtGNUAsm;

			static void __stdcall OnConvClicked(void *userObj);
			static void __stdcall OnConv2Clicked(void *userObj);
			void ConvAsm();
		public:
			AVIRAsmConvForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRAsmConvForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
