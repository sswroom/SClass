#ifndef _SM_SSWR_AVIREAD_AVIRWIFICAPTUREFORM
#define _SM_SSWR_AVIREAD_AVIRWIFICAPTUREFORM
#include "Data/FastMap.h"
#include "IO/FileStream.h"
#include "IO/GPSNMEA.h"
#include "IO/MotionDetectorAccelerometer.h"
#include "IO/SensorManager.h"
#include "IO/Writer.h"
#include "Net/WirelessLAN.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "Sync/Mutex.h"
#include "UI/GUIButton.h"
#include "UI/GUICheckBox.h"
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
		class AVIRWifiCaptureForm : public UI::GUIForm
		{
		private:
			typedef struct
			{
				UInt8 mac[6];
				Double freq;
				NotNullPtr<Text::String> ssid;
				Int32 phyType;
				Int32 bssType;
			} BSSStatus;

			typedef struct
			{
				UInt8 mac[6];
				NotNullPtr<Text::String> ssid;
				Int32 phyType;
				Double freq;
				Text::String *manuf;
				Text::String *model;
				Text::String *serialNum;
				Optional<Text::String> country;
				UInt8 ouis[3][3];
				UInt64 neighbour[20];
			} WifiLog;
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			IO::SensorManager sensorMgr;
			IO::MotionDetectorAccelerometer *motion;
			Net::WirelessLAN *wlan;
			Net::WirelessLAN::Interface *wlanInterf;
			UOSInt wlanScan;
			Data::FastMap<UInt64, BSSStatus*> bssMap;
			Data::FastMap<UInt64, WifiLog*> wifiLogMap;
			Int32 lastMotion;
			Bool gpsChg;
			Data::TimeInstant currGPSTime;
			Math::Coord2DDbl currPos;
			Double currAlt;
			Bool currActive;
			Map::ILocationService *locSvc;
			Bool locSvcRel;
			IO::FileStream *captureFS;
			IO::Writer *captureWriter;
			Sync::Mutex captureMut;
			Int64 lastTimeTick;

			NotNullPtr<UI::GUITabControl> tcMain;

			NotNullPtr<UI::GUITabPage> tpStatus;
			NotNullPtr<UI::GUILabel> lblMotion;
			NotNullPtr<UI::GUITextBox> txtMotion;
			NotNullPtr<UI::GUILabel> lblGPS;
			NotNullPtr<UI::GUITextBox> txtGPS;
			NotNullPtr<UI::GUIButton> btnGPS;
			NotNullPtr<UI::GUILabel> lblGPSTime;
			NotNullPtr<UI::GUITextBox> txtGPSTime;
			NotNullPtr<UI::GUILabel> lblGPSLat;
			NotNullPtr<UI::GUITextBox> txtGPSLat;
			NotNullPtr<UI::GUILabel> lblGPSLon;
			NotNullPtr<UI::GUITextBox> txtGPSLon;
			NotNullPtr<UI::GUILabel> lblGPSAlt;
			NotNullPtr<UI::GUITextBox> txtGPSAlt;
			NotNullPtr<UI::GUILabel> lblGPSActive;
			NotNullPtr<UI::GUITextBox> txtGPSActive;
			NotNullPtr<UI::GUILabel> lblBattery;
			NotNullPtr<UI::GUITextBox> txtBattery;
			NotNullPtr<UI::GUILabel> lblCurrWifiCnt;
			NotNullPtr<UI::GUITextBox> txtCurrWifiCnt;
			NotNullPtr<UI::GUILabel> lblBSSCount;
			NotNullPtr<UI::GUITextBox> txtBSSCount;
			NotNullPtr<UI::GUIButton> btnCapture;

			NotNullPtr<UI::GUITabPage> tpCurr;
			NotNullPtr<UI::GUIListView> lvCurrWifi;

			NotNullPtr<UI::GUITabPage> tpLogWifi;
			NotNullPtr<UI::GUIListView> lvLogWifi;
			NotNullPtr<UI::GUIPanel> pnlLogWifi;
			NotNullPtr<UI::GUIButton> btnLogWifiSave;
			NotNullPtr<UI::GUIButton> btnLogWifiSaveF;

			static void __stdcall OnTimerTick(void *userObj);
			static void __stdcall OnGPSClicked(void *userObj);
			static void __stdcall OnCaptureClicked(void *userObj);
			static void __stdcall OnLogWifiDblClicked(void *userObj, UOSInt index);
			static void __stdcall OnLogWifiSaveClicked(void *userObj);
			static void __stdcall OnLogWifiSaveFClicked(void *userObj);
			static Bool __stdcall OnFormClosing(void *userObj, CloseReason reason);
			static void __stdcall OnGPSData(void *userObj, NotNullPtr<Map::GPSTrack::GPSRecord3> record, Data::DataArray<Map::ILocationService::SateStatus> sates);
		public:
			AVIRWifiCaptureForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRWifiCaptureForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
