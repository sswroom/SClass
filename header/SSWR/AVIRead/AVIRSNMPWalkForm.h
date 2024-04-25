#ifndef _SM_SSWR_AVIREAD_AVIRSNMPWALKFORM
#define _SM_SSWR_AVIREAD_AVIRSNMPWALKFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUITextBox.h"
#include "UI/ListBoxLogger.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRSNMPWalkForm : public UI::GUIForm
		{
		private:
			NN<SSWR::AVIRead::AVIRCore> core;

			NN<UI::GUIPanel> pnlRequest;
			NN<UI::GUILabel> lblAgent;
			NN<UI::GUITextBox> txtAgent;
			NN<UI::GUIListView> lvResults;

		public:
			AVIRSNMPWalkForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<const Net::SocketUtil::AddressInfo> addr, NN<Text::String> community);
			virtual ~AVIRSNMPWalkForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
