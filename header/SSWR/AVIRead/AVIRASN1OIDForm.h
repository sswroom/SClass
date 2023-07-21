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
			SSWR::AVIRead::AVIRCore *core;

			UI::GUITabControl *tcMain;
			
			UI::GUITabPage *tpSource;
			UI::GUIPanel *pnlSource;
			UI::GUIButton *btnConvert;
			UI::GUITextBox *txtSource;

			UI::GUITabPage *tpCPP;
			UI::GUITextBox *txtCPP;

			static void __stdcall OnConvertClicked(void *userObj);
		public:
			AVIRASN1OIDForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRASN1OIDForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
