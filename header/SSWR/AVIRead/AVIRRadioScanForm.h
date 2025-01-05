#ifndef _SM_SSWR_AVIREAD_AVIRRADIOSCANFORM
#define _SM_SSWR_AVIREAD_AVIRRADIOSCANFORM
#include "IO/BTScanner.h"
#include "Net/WirelessLAN.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListView.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRRadioScanForm : public UI::GUIForm
		{
		private:
			NN<UI::GUITabControl> tcMain;

			NN<UI::GUITabPage> tpDashboard;
			NN<UI::GUIListView> lvDashboard;

			NN<UI::GUITabPage> tpWiFi;
			NN<UI::GUIPanel> pnlWiFi;
			NN<UI::GUIButton> btnWiFi;
			NN<UI::GUITextBox> txtWiFiDetail;
			NN<UI::GUIListView> lvWiFi;

			NN<UI::GUITabPage> tpBluetooth;
			NN<UI::GUIPanel> pnlBluetooth;
			NN<UI::GUIButton> btnBluetooth;
			NN<UI::GUIListView> lvBluetooth;

			NN<SSWR::AVIRead::AVIRCore> core;
			Net::WirelessLAN wlan;
			Optional<Net::WirelessLAN::Interface> wlanInterf;
			OSInt wlanScan;
			Int64 wlanLastTime;
			Data::ArrayListNN<Net::WirelessLAN::BSSInfo> wlanBSSList;

			Optional<IO::BTScanner> bt;
			Bool btUpdated;
			Int64 btMinTime;

			static void __stdcall OnTimerTick(AnyType userObj);
			static void __stdcall OnWiFiClicked(AnyType userObj);
			static void __stdcall OnWiFiSelChg(AnyType userObj);
			static void __stdcall OnBluetoothClicked(AnyType userObj);
			static void __stdcall OnDashboardDblClk(AnyType userObj, UOSInt index);
			static void __stdcall OnDeviceUpdated(NN<IO::BTScanLog::ScanRecord3> dev, IO::BTScanner::UpdateType updateType, AnyType userObj);
			void ToggleWiFi();
			void ToggleBT();
			UOSInt AppendBTList(NN<Data::FastMapNN<UInt64, IO::BTScanLog::ScanRecord3>> devMap, Int64 currTime, OutParam<Int64> minTime);
		public:
			AVIRRadioScanForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRRadioScanForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
