#ifndef _SM_SSWR_AVIREAD_AVIRRADIOSCANFORM
#define _SM_SSWR_AVIREAD_AVIRRADIOSCANFORM
#include "IO/ATCommandChannel.h"
#include "IO/BTScanner.h"
#include "IO/GSMModemController.h"
#include "IO/HuaweiGSMModemController.h"
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

			NN<UI::GUITabPage> tpGNSS;
			NN<UI::GUIPanel> pnlGNSS;
			NN<UI::GUILabel> lblGNSSPort;
			NN<UI::GUIComboBox> cboGNSSPort;
			NN<UI::GUIButton> btnGNSSPort;
			NN<UI::GUILabel> lblGNSSBaudRate;
			NN<UI::GUIComboBox> cboGNSSBaudRate;
			NN<UI::GUILabel> lblGNSSParity;
			NN<UI::GUIComboBox> cboGNSSParity;
			NN<UI::GUILabel> lblGNSSFlowControl;
			NN<UI::GUICheckBox> chkGNSSFlowControl;
			NN<UI::GUIButton> btnGNSS;
			NN<UI::GUITabControl> tcGNSS;
			NN<UI::GUITabPage> tpGNSSSatellite;
			NN<UI::GUIListView> lvGNSSSatellite;
			NN<UI::GUITabPage> tpGNSSLocation;
			NN<UI::GUILabel> lblGNSSTime;
			NN<UI::GUITextBox> txtGNSSTime;
			NN<UI::GUILabel> lblGNSSLatitude;
			NN<UI::GUITextBox> txtGNSSLatitude;
			NN<UI::GUILabel> lblGNSSLongitude;
			NN<UI::GUITextBox> txtGNSSLongitude;
			NN<UI::GUILabel> lblGNSSAltitude;
			NN<UI::GUITextBox> txtGNSSAltitude;
			NN<UI::GUILabel> lblGNSSSpeed;
			NN<UI::GUITextBox> txtGNSSSpeed;
			NN<UI::GUILabel> lblGNSSHeading;
			NN<UI::GUITextBox> txtGNSSHeading;
			NN<UI::GUILabel> lblGNSSValid;
			NN<UI::GUITextBox> txtGNSSValid;
			NN<UI::GUILabel> lblGNSSNSateUsed;
			NN<UI::GUITextBox> txtGNSSNSateUsed;
			NN<UI::GUILabel> lblGNSSNSateUsedGPS;
			NN<UI::GUITextBox> txtGNSSNSateUsedGPS;
			NN<UI::GUILabel> lblGNSSNSateUsedSBAS;
			NN<UI::GUITextBox> txtGNSSNSateUsedSBAS;
			NN<UI::GUILabel> lblGNSSNSateUsedGLO;
			NN<UI::GUITextBox> txtGNSSNSateUsedGLO;
			NN<UI::GUILabel> lblGNSSNSateViewGPS;
			NN<UI::GUITextBox> txtGNSSNSateViewGPS;
			NN<UI::GUILabel> lblGNSSNSateViewGLO;
			NN<UI::GUITextBox> txtGNSSNSateViewGLO;
			NN<UI::GUILabel> lblGNSSNSateViewGA;
			NN<UI::GUITextBox> txtGNSSNSateViewGA;
			NN<UI::GUILabel> lblGNSSNSateViewQZSS;
			NN<UI::GUITextBox> txtGNSSNSateViewQZSS;
			NN<UI::GUILabel> lblGNSSNSateViewBD;
			NN<UI::GUITextBox> txtGNSSNSateViewBD;

			NN<UI::GUITabPage> tpCellular;
			NN<UI::GUIPanel> pnlCellular;
			NN<UI::GUILabel> lblCellularPort;
			NN<UI::GUIComboBox> cboCellularPort;
			NN<UI::GUIButton> btnCellularPort;
			NN<UI::GUILabel> lblCellularBaudRate;
			NN<UI::GUIComboBox> cboCellularBaudRate;
			NN<UI::GUILabel> lblCellularParity;
			NN<UI::GUIComboBox> cboCellularParity;
			NN<UI::GUILabel> lblCellularFlowControl;
			NN<UI::GUICheckBox> chkCellularFlowControl;
			NN<UI::GUIButton> btnCellular;
			NN<UI::GUITabControl> tcCellular;
			NN<UI::GUITabPage> tpCellularInfo;
			NN<UI::GUILabel> lblCellularManu;
			NN<UI::GUITextBox> txtCellularManu;
			NN<UI::GUILabel> lblCellularModel;
			NN<UI::GUITextBox> txtCellularModel;
			NN<UI::GUILabel> lblCellularVer;
			NN<UI::GUITextBox> txtCellularVer;
			NN<UI::GUILabel> lblCellularIMEI;
			NN<UI::GUITextBox> txtCellularIMEI;
			NN<UI::GUILabel> lblCellularIMSI;
			NN<UI::GUITextBox> txtCellularIMSI;
			NN<UI::GUILabel> lblCellularTECharset;
			NN<UI::GUITextBox> txtCellularTECharset;
			NN<UI::GUILabel> lblCellularOperator;
			NN<UI::GUITextBox> txtCellularOperator;
			NN<UI::GUILabel> lblCellularRegStatus;
			NN<UI::GUITextBox> txtCellularRegStatus;
			NN<UI::GUILabel> lblCellularLAC;
			NN<UI::GUITextBox> txtCellularLAC;
			NN<UI::GUILabel> lblCellularCI;
			NN<UI::GUITextBox> txtCellularCI;
			NN<UI::GUILabel> lblCellularACT;
			NN<UI::GUITextBox> txtCellularACT;
			NN<UI::GUILabel> lblCellularSignalQuality;
			NN<UI::GUITextBox> txtCellularSignalQuality;

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
			Map::GPSTrack::GPSRecord3 gnssRecCurr;
			Sync::Mutex gnssRecMut;
			Bool gnssRecUpdated;
			UOSInt gnssRecSateCnt;
			Map::ILocationService::SateStatus gnssRecSates[32];
			Int64 gnssLastUpdateTime;

			Sync::Event cellularEvt;
			Optional<IO::Stream> cellularPort;
			Optional<IO::ATCommandChannel> cellularChannel;
			Optional<IO::GSMModemController> cellularModem;
			Optional<IO::HuaweiGSMModemController> cellularHuawei;
			Bool cellularToStop;
			Bool cellularRunning;
			Bool cellularInitStrs;
			Optional<Text::String> cellularModemManu;
			Optional<Text::String> cellularModemModel;
			Optional<Text::String> cellularModemVer;
			Optional<Text::String> cellularIMEI;
			Optional<Text::String> cellularHuaweiICCID;
			Bool cellularTECharsetUpd;
			Optional<Text::String> cellularTECharset;
			Bool cellularSIMChanged;
			Bool cellularSIMInfoUpdated;
			Optional<Text::String> cellularIMSI;
			Bool cellularRegNetUpdated;
			IO::GSMModemController::NetworkResult cellularRegNetN;
			IO::GSMModemController::RegisterStatus cellularRegNetStat;
			UInt16 cellularRegNetLAC;
			UInt32 cellularRegNetCI;
			IO::GSMModemController::AccessTech cellularRegNetACT;
			Bool cellularSignalUpdated;
			IO::GSMModemController::RSSI cellularSignalQuality;
			Bool cellularOperUpdated;
			Optional<Text::String> cellularOperName;
			Data::Timestamp cellularOperNextTime;

			static UInt32 __stdcall CellularThread(AnyType userObj);
			static void __stdcall OnTimerTick(AnyType userObj);
			static void __stdcall OnWiFiClicked(AnyType userObj);
			static void __stdcall OnWiFiSelChg(AnyType userObj);
			static void __stdcall OnBluetoothClicked(AnyType userObj);
			static void __stdcall OnDashboardDblClk(AnyType userObj, UOSInt index);
			static void __stdcall OnDeviceUpdated(NN<IO::BTScanLog::ScanRecord3> dev, IO::BTScanner::UpdateType updateType, AnyType userObj);
			static void __stdcall OnGNSSPortClicked(AnyType userObj);
			static void __stdcall OnGNSSClicked(AnyType userObj);
			static void __stdcall OnGNSSLocationUpdated(AnyType userObj, NN<Map::GPSTrack::GPSRecord3> record, Data::DataArray<Map::ILocationService::SateStatus> sates);
			static void __stdcall OnCellularPortClicked(AnyType userObj);
			static void __stdcall OnCellularClicked(AnyType userObj);
			void ToggleWiFi();
			void ToggleBT();
			void ToggleGNSS();
			void ToggleCellular();
			UOSInt AppendBTList(NN<Data::FastMapNN<UInt64, IO::BTScanLog::ScanRecord3>> devMap, Int64 currTime, OutParam<Int64> minTime);
			Bool CloseCellular();
		public:
			AVIRRadioScanForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRRadioScanForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
