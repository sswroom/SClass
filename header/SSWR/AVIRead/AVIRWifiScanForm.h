#ifndef _SM_SSWR_AVIREAD_AVIRWIFISCANFORM
#define _SM_SSWR_AVIREAD_AVIRWIFISCANFORM
#include "Net/WirelessLAN.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUITextBox.h"
#include "UI/GUIVSplitter.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRWifiScanForm : public UI::GUIForm
		{
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			Net::WirelessLAN wlan;
			Optional<Net::WirelessLAN::Interface> wlanInterf;
			Data::ArrayListNN<Net::WirelessLAN::BSSInfo> bssList;

			NN<UI::GUIPanel> pnlControl;
			NN<UI::GUIButton> btnScan;
			NN<UI::GUILabel> lblScanTime;
			NN<UI::GUITextBox> txtScanTime;
			NN<UI::GUILabel> lblResultTime;
			NN<UI::GUITextBox> txtResultTime;
			NN<UI::GUIListView> lvWifi;
			NN<UI::GUIVSplitter> vspWifi;
			NN<UI::GUITextBox> txtWifi;

			static void __stdcall OnScanClicked(AnyType userObj);
			static void __stdcall OnWifiSelChg(AnyType userObj);
			void WifiScan();
			void WifiClear();
		public:
			AVIRWifiScanForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRWifiScanForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
