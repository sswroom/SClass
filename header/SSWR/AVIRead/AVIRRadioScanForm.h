#ifndef _SM_SSWR_AVIREAD_AVIRRADIOSCANFORM
#define _SM_SSWR_AVIREAD_AVIRRADIOSCANFORM
#include "IO/BTScanner.h"
#include "Map/ILocationService.h"
#include "Net/WirelessLAN.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUICheckBox.h"
#include "UI/GUIComboBox.h"
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

			NN<UI::GUITabPage> tpGPS;
			NN<UI::GUIPanel> pnlGPS;
			NN<UI::GUILabel> lblGPSPort;
			NN<UI::GUIComboBox> cboGPSPort;
			NN<UI::GUIButton> btnGPSPort;
			NN<UI::GUILabel> lblGPSBaudRate;
			NN<UI::GUIComboBox> cboGPSBaudRate;
			NN<UI::GUILabel> lblGPSParity;
			NN<UI::GUIComboBox> cboGPSParity;
			NN<UI::GUILabel> lblGPSFlowControl;
			NN<UI::GUICheckBox> chkGPSFlowControl;
			NN<UI::GUIButton> btnGPS;
			NN<UI::GUITabControl> tcGPS;
			NN<UI::GUITabPage> tpGPSSatellite;
			NN<UI::GUIListView> lvGPSSatellite;
			NN<UI::GUITabPage> tpGPSLocation;
			NN<UI::GUILabel> lblGPSTime;
			NN<UI::GUITextBox> txtGPSTime;
			NN<UI::GUILabel> lblGPSLatitude;
			NN<UI::GUITextBox> txtGPSLatitude;
			NN<UI::GUILabel> lblGPSLongitude;
			NN<UI::GUITextBox> txtGPSLongitude;
			NN<UI::GUILabel> lblGPSAltitude;
			NN<UI::GUITextBox> txtGPSAltitude;
			NN<UI::GUILabel> lblGPSSpeed;
			NN<UI::GUITextBox> txtGPSSpeed;
			NN<UI::GUILabel> lblGPSHeading;
			NN<UI::GUITextBox> txtGPSHeading;
			NN<UI::GUILabel> lblGPSValid;
			NN<UI::GUITextBox> txtGPSValid;
			NN<UI::GUILabel> lblGPSNSateUsed;
			NN<UI::GUITextBox> txtGPSNSateUsed;
			NN<UI::GUILabel> lblGPSNSateUsedGPS;
			NN<UI::GUITextBox> txtGPSNSateUsedGPS;
			NN<UI::GUILabel> lblGPSNSateUsedSBAS;
			NN<UI::GUITextBox> txtGPSNSateUsedSBAS;
			NN<UI::GUILabel> lblGPSNSateUsedGLO;
			NN<UI::GUITextBox> txtGPSNSateUsedGLO;
			NN<UI::GUILabel> lblGPSNSateViewGPS;
			NN<UI::GUITextBox> txtGPSNSateViewGPS;
			NN<UI::GUILabel> lblGPSNSateViewGLO;
			NN<UI::GUITextBox> txtGPSNSateViewGLO;
			NN<UI::GUILabel> lblGPSNSateViewGA;
			NN<UI::GUITextBox> txtGPSNSateViewGA;
			NN<UI::GUILabel> lblGPSNSateViewQZSS;
			NN<UI::GUITextBox> txtGPSNSateViewQZSS;
			NN<UI::GUILabel> lblGPSNSateViewBD;
			NN<UI::GUITextBox> txtGPSNSateViewBD;

			NN<SSWR::AVIRead::AVIRCore> core;
			Net::WirelessLAN wlan;
			Optional<Net::WirelessLAN::Interface> wlanInterf;
			OSInt wlanScan;
			Int64 wlanLastTime;
			Data::ArrayListNN<Net::WirelessLAN::BSSInfo> wlanBSSList;

			Optional<IO::BTScanner> bt;
			Bool btUpdated;
			Int64 btMinTime;

			Optional<Map::ILocationService> locSvc;
			Map::GPSTrack::GPSRecord3 gpsRecCurr;
			Sync::Mutex gpsRecMut;
			Bool gpsRecUpdated;
			UOSInt gpsRecSateCnt;
			Map::ILocationService::SateStatus gpsRecSates[32];
			Int64 gpsLastUpdateTime;

			static void __stdcall OnTimerTick(AnyType userObj);
			static void __stdcall OnWiFiClicked(AnyType userObj);
			static void __stdcall OnWiFiSelChg(AnyType userObj);
			static void __stdcall OnBluetoothClicked(AnyType userObj);
			static void __stdcall OnDashboardDblClk(AnyType userObj, UOSInt index);
			static void __stdcall OnDeviceUpdated(NN<IO::BTScanLog::ScanRecord3> dev, IO::BTScanner::UpdateType updateType, AnyType userObj);
			static void __stdcall OnGPSPortClicked(AnyType userObj);
			static void __stdcall OnGPSClicked(AnyType userObj);
			static void __stdcall OnGPSLocationUpdated(AnyType userObj, NN<Map::GPSTrack::GPSRecord3> record, Data::DataArray<Map::ILocationService::SateStatus> sates);
			void ToggleWiFi();
			void ToggleBT();
			void ToggleGPS();
			UOSInt AppendBTList(NN<Data::FastMapNN<UInt64, IO::BTScanLog::ScanRecord3>> devMap, Int64 currTime, OutParam<Int64> minTime);
		public:
			AVIRRadioScanForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRRadioScanForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
