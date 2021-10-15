#ifndef _SM_SSWR_AVIREAD_AVIRASN1DATAFORM
#define _SM_SSWR_AVIREAD_AVIRASN1DATAFORM
#include "Net/ASN1Data.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIForm.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRASN1DataForm : public UI::GUIForm
		{
		private:
			SSWR::AVIRead::AVIRCore *core;
			Net::ASN1Data *asn1;

			UI::GUIMainMenu *mnuMain;

			UI::GUITabControl *tcMain;

			UI::GUITabPage *tpDesc;
			UI::GUITextBox *txtDesc;
			
			UI::GUITabPage *tpASN1;
			UI::GUITextBox *txtASN1;

		public:
			AVIRASN1DataForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, Net::ASN1Data *asn1);
			virtual ~AVIRASN1DataForm();

			virtual void OnMonitorChanged();
			virtual void EventMenuClicked(UInt16 cmdId);
		};
	}
}
#endif
