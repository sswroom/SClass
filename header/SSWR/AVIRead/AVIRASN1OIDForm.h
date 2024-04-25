#ifndef _SM_SSWR_AVIREAD_AVIRASN1OIDFORM
#define _SM_SSWR_AVIREAD_AVIRASN1OIDFORM
#include "Net/ASN1MIB.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUIPanel.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRASN1OIDForm : public UI::GUIForm
		{
		private:
			NN<SSWR::AVIRead::AVIRCore> core;

			NN<UI::GUITabControl> tcMain;
			
			NN<UI::GUITabPage> tpSource;
			NN<UI::GUIPanel> pnlSource;
			NN<UI::GUIButton> btnConvert;
			NN<UI::GUITextBox> txtSource;

			NN<UI::GUITabPage> tpCPP;
			NN<UI::GUITextBox> txtCPP;

			static void __stdcall OnConvertClicked(AnyType userObj);
		public:
			AVIRASN1OIDForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRASN1OIDForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
