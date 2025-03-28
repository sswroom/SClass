#ifndef _SM_SSWR_AVIREAD_AVIRMEDIAFORM
#define _SM_SSWR_AVIREAD_AVIRMEDIAFORM
#include "Net/MDNSClient.h"
#include "UI/GUIForm.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUIListBox.h"
#include "UI/GUIListView.h"
#include "UI/GUITextBox.h"
#include "UI/GUIVSplitter.h"
#include "SSWR/AVIRead/AVIRCore.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRMDNSForm : public UI::GUIForm
		{
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			NN<Net::MDNSClient> mdns;

			NN<UI::GUIListBox> lbDev;
			NN<UI::GUIHSplitter> hspDev;
			NN<UI::GUIListView> lvServices;
			NN<UI::GUIVSplitter> vspServices;
			NN<UI::GUITextBox> txtServices;
		public:
			AVIRMDNSForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRMDNSForm();
		
			virtual void OnMonitorChanged();
		};
	}
}
#endif
