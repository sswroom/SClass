#ifndef _SM_SSWR_AVIREAD_AVIRWIFICAPTURELITEFORM
#define _SM_SSWR_AVIREAD_AVIRWIFICAPTURELITEFORM
#include "Data/Integer64Map.h"
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
				const UTF8Char *ssid;
				Int32 phyType;
				Int32 bssType;
			} BSSStatus;

			typedef struct
			{
				UInt8 mac[6];
				const UTF8Char *ssid;
				Int32 phyType;
				Double freq;
				const UTF8Char *manuf;
				const UTF8Char *model;
				const UTF8Char *serialNum;
				const UTF8Char *country;
				UInt8 ouis[3][3];
				Int64 neighbour[20];
				OSInt ieLen;
				UInt8 *ieBuff;
			} WifiLog;
		private:
			SSWR::AVIRead::AVIRCore *core;
			Net::WirelessLAN *wlan;
			Net::WirelessLAN::Interface *wlanInterf;
			OSInt wlanScan;
			Data::Integer64Map<BSSStatus*> *bssMap;
			Data::Integer64Map<WifiLog*> *wifiLogMap;
			Int64 lastTimeTick;

			UI::GUITabControl *tcMain;

			UI::GUITabPage *tpCurr;
			UI::GUIListView *lvCurrWifi;

			UI::GUITabPage *tpLogWifi;
			UI::GUIListView *lvLogWifi;
			UI::GUIPanel *pnlLogWifi;
			UI::GUIButton *btnLogWifiSave;
			UI::GUIButton *btnLogWifiSaveF;

			static void __stdcall OnTimerTick(void *userObj);
			static void __stdcall OnLogWifiDblClicked(void *userObj, OSInt index);
			static void __stdcall OnLogWifiSaveClicked(void *userObj);
			static void __stdcall OnLogWifiSaveFClicked(void *userObj);
			static Bool __stdcall OnFormClosing(void *userObj, CloseReason reason);
		public:
			AVIRWifiCaptureLiteForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRWifiCaptureLiteForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
