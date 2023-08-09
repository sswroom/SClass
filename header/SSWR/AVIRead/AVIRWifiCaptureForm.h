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
				Text::String *country;
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

			UI::GUITabControl *tcMain;

			UI::GUITabPage *tpStatus;
			UI::GUILabel *lblMotion;
			UI::GUITextBox *txtMotion;
			UI::GUILabel *lblGPS;
			UI::GUITextBox *txtGPS;
			UI::GUIButton *btnGPS;
			UI::GUILabel *lblGPSTime;
			UI::GUITextBox *txtGPSTime;
			UI::GUILabel *lblGPSLat;
			UI::GUITextBox *txtGPSLat;
			UI::GUILabel *lblGPSLon;
			UI::GUITextBox *txtGPSLon;
			UI::GUILabel *lblGPSAlt;
			UI::GUITextBox *txtGPSAlt;
			UI::GUILabel *lblGPSActive;
			UI::GUITextBox *txtGPSActive;
			UI::GUILabel *lblBattery;
			UI::GUITextBox *txtBattery;
			UI::GUILabel *lblCurrWifiCnt;
			UI::GUITextBox *txtCurrWifiCnt;
			UI::GUILabel *lblBSSCount;
			UI::GUITextBox *txtBSSCount;
			UI::GUIButton *btnCapture;

			UI::GUITabPage *tpCurr;
			UI::GUIListView *lvCurrWifi;

			UI::GUITabPage *tpLogWifi;
			UI::GUIListView *lvLogWifi;
			UI::GUIPanel *pnlLogWifi;
			UI::GUIButton *btnLogWifiSave;
			UI::GUIButton *btnLogWifiSaveF;

			static void __stdcall OnTimerTick(void *userObj);
			static void __stdcall OnGPSClicked(void *userObj);
			static void __stdcall OnCaptureClicked(void *userObj);
			static void __stdcall OnLogWifiDblClicked(void *userObj, UOSInt index);
			static void __stdcall OnLogWifiSaveClicked(void *userObj);
			static void __stdcall OnLogWifiSaveFClicked(void *userObj);
			static Bool __stdcall OnFormClosing(void *userObj, CloseReason reason);
			static void __stdcall OnGPSData(void *userObj, NotNullPtr<Map::GPSTrack::GPSRecord3> record, Data::DataArray<Map::ILocationService::SateStatus> sates);
		public:
			AVIRWifiCaptureForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRWifiCaptureForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
