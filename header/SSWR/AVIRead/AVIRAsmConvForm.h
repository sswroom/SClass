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
			NN<SSWR::AVIRead::AVIRCore> core;

			NN<UI::GUIPanel> pnlCtrl;
			NN<UI::GUIButton> btnConv;
			NN<UI::GUIButton> btnConv2;
			NN<UI::GUITextBox> txtIntelAsm;
			NN<UI::GUIHSplitter> hspAsm;
			NN<UI::GUITextBox> txtGNUAsm;

			static void __stdcall OnConvClicked(AnyType userObj);
			static void __stdcall OnConv2Clicked(AnyType userObj);
			void ConvAsm();
		public:
			AVIRAsmConvForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRAsmConvForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
