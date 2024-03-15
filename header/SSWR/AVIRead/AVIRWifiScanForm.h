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
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Net::WirelessLAN *wlan;
			Net::WirelessLAN::Interface *wlanInterf;

			NotNullPtr<UI::GUIPanel> pnlControl;
			NotNullPtr<UI::GUIButton> btnScan;
			NotNullPtr<UI::GUILabel> lblScanTime;
			NotNullPtr<UI::GUITextBox> txtScanTime;
			NotNullPtr<UI::GUILabel> lblResultTime;
			NotNullPtr<UI::GUITextBox> txtResultTime;
			NotNullPtr<UI::GUIListView> lvWifi;
			NotNullPtr<UI::GUIVSplitter> vspWifi;
			NotNullPtr<UI::GUITextBox> txtWifi;

			static void __stdcall OnScanClicked(AnyType userObj);
			static void __stdcall OnWifiSelChg(AnyType userObj);
			void WifiScan();
			void WifiClear();
		public:
			AVIRWifiScanForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRWifiScanForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
