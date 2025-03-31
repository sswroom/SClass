#ifndef _SM_SSWR_AVIREAD_AVIRWSDISCOVERYFORM
#define _SM_SSWR_AVIREAD_AVIRWSDISCOVERYFORM
#include "Net/WSDiscoveryClient.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRWSDiscoveryForm : public UI::GUIForm
		{
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			NN<Net::WSDiscoveryClient> wsd;

			NN<UI::GUIPanel> pnlFile;
			NN<UI::GUILabel> lblFile;
			NN<UI::GUITextBox> txtFile;
			NN<UI::GUIButton> btnBrowse;
			NN<UI::GUIListView> lvXML;

		public:
			AVIRWSDiscoveryForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRWSDiscoveryForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
