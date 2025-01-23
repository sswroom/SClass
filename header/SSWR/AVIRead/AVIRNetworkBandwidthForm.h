#ifndef _SM_SSWR_AVIREAD_AVIRNETWORKBANDWIDTHFORM
#define _SM_SSWR_AVIREAD_AVIRNETWORKBANDWIDTHFORM
#include "Net/BandwidthLogger.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRNetworkBandwidthForm : public UI::GUIForm
		{
		private:
			struct DNSResult
			{
				UInt32 ip;
				NN<Text::String> name;
			};
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			Net::BandwidthLogger logger;

			NN<UI::GUIPanel> pnlControl;
			NN<UI::GUIButton> btnLog;
			NN<UI::GUITextBox> txtLog;
			NN<UI::GUIListView> lvDetail;

			static void __stdcall OnLogClicked(AnyType userObj);
			static void __stdcall OnTimerTick(AnyType userObj);
		public:
			AVIRNetworkBandwidthForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRNetworkBandwidthForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
