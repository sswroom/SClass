#ifndef _SM_SSWR_AVIREAD_AVIRWIFICAPTURELITEFORM
#define _SM_SSWR_AVIREAD_AVIRWIFICAPTURELITEFORM
#include "Data/FastMap.h"
#include "IO/FileStream.h"
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
		class AVIRWifiCaptureLiteForm : public UI::GUIForm
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
				UOSInt ieLen;
				UInt8 *ieBuff;
			} WifiLog;
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Net::WirelessLAN *wlan;
			Net::WirelessLAN::Interface *wlanInterf;
			OSInt wlanScan;
			Data::FastMap<UInt64, BSSStatus*> bssMap;
			Data::FastMap<UInt64, WifiLog*> wifiLogMap;
			Int64 lastTimeTick;

			NotNullPtr<UI::GUITabControl> tcMain;

			NotNullPtr<UI::GUITabPage> tpCurr;
			NotNullPtr<UI::GUIListView> lvCurrWifi;

			NotNullPtr<UI::GUITabPage> tpLogWifi;
			NotNullPtr<UI::GUIListView> lvLogWifi;
			NotNullPtr<UI::GUIPanel> pnlLogWifi;
			NotNullPtr<UI::GUIButton> btnLogWifiSave;
			NotNullPtr<UI::GUIButton> btnLogWifiSaveF;

			static void __stdcall OnTimerTick(void *userObj);
			static void __stdcall OnLogWifiDblClicked(void *userObj, UOSInt index);
			static void __stdcall OnLogWifiSaveClicked(void *userObj);
			static void __stdcall OnLogWifiSaveFClicked(void *userObj);
			static Bool __stdcall OnFormClosing(void *userObj, CloseReason reason);
		public:
			AVIRWifiCaptureLiteForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRWifiCaptureLiteForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
