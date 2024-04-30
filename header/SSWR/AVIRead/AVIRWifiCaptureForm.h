#ifndef _SM_SSWR_AVIREAD_AVIRWIFICAPTUREFORM
#define _SM_SSWR_AVIREAD_AVIRWIFICAPTUREFORM
#include "Data/FastMapNN.h"
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
				NN<Text::String> ssid;
				Int32 phyType;
				Int32 bssType;
			} BSSStatus;

			typedef struct
			{
				UInt8 mac[6];
				NN<Text::String> ssid;
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
			NN<SSWR::AVIRead::AVIRCore> core;
			IO::SensorManager sensorMgr;
			IO::MotionDetectorAccelerometer *motion;
			Net::WirelessLAN *wlan;
			Optional<Net::WirelessLAN::Interface> wlanInterf;
			UOSInt wlanScan;
			Data::FastMapNN<UInt64, BSSStatus> bssMap;
			Data::FastMapNN<UInt64, WifiLog> wifiLogMap;
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

			NN<UI::GUITabControl> tcMain;

			NN<UI::GUITabPage> tpStatus;
			NN<UI::GUILabel> lblMotion;
			NN<UI::GUITextBox> txtMotion;
			NN<UI::GUILabel> lblGPS;
			NN<UI::GUITextBox> txtGPS;
			NN<UI::GUIButton> btnGPS;
			NN<UI::GUILabel> lblGPSTime;
			NN<UI::GUITextBox> txtGPSTime;
			NN<UI::GUILabel> lblGPSLat;
			NN<UI::GUITextBox> txtGPSLat;
			NN<UI::GUILabel> lblGPSLon;
			NN<UI::GUITextBox> txtGPSLon;
			NN<UI::GUILabel> lblGPSAlt;
			NN<UI::GUITextBox> txtGPSAlt;
			NN<UI::GUILabel> lblGPSActive;
			NN<UI::GUITextBox> txtGPSActive;
			NN<UI::GUILabel> lblBattery;
			NN<UI::GUITextBox> txtBattery;
			NN<UI::GUILabel> lblCurrWifiCnt;
			NN<UI::GUITextBox> txtCurrWifiCnt;
			NN<UI::GUILabel> lblBSSCount;
			NN<UI::GUITextBox> txtBSSCount;
			NN<UI::GUIButton> btnCapture;

			NN<UI::GUITabPage> tpCurr;
			NN<UI::GUIListView> lvCurrWifi;

			NN<UI::GUITabPage> tpLogWifi;
			NN<UI::GUIListView> lvLogWifi;
			NN<UI::GUIPanel> pnlLogWifi;
			NN<UI::GUIButton> btnLogWifiSave;
			NN<UI::GUIButton> btnLogWifiSaveF;

			static void __stdcall OnTimerTick(AnyType userObj);
			static void __stdcall OnGPSClicked(AnyType userObj);
			static void __stdcall OnCaptureClicked(AnyType userObj);
			static void __stdcall OnLogWifiDblClicked(AnyType userObj, UOSInt index);
			static void __stdcall OnLogWifiSaveClicked(AnyType userObj);
			static void __stdcall OnLogWifiSaveFClicked(AnyType userObj);
			static Bool __stdcall OnFormClosing(AnyType userObj, CloseReason reason);
			static void __stdcall OnGPSData(AnyType userObj, NN<Map::GPSTrack::GPSRecord3> record, Data::DataArray<Map::ILocationService::SateStatus> sates);
		public:
			AVIRWifiCaptureForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRWifiCaptureForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
