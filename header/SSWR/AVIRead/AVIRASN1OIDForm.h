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
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;

			UI::GUITabControl *tcMain;
			
			NotNullPtr<UI::GUITabPage> tpSource;
			NotNullPtr<UI::GUIPanel> pnlSource;
			NotNullPtr<UI::GUIButton> btnConvert;
			UI::GUITextBox *txtSource;

			NotNullPtr<UI::GUITabPage> tpCPP;
			UI::GUITextBox *txtCPP;

			static void __stdcall OnConvertClicked(void *userObj);
		public:
			AVIRASN1OIDForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRASN1OIDForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
