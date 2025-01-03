#ifndef _SM_SSWR_AVIREAD_AVIRWIFICAPTURELITEFORM
#define _SM_SSWR_AVIREAD_AVIRWIFICAPTURELITEFORM
#include "Data/FastMapNN.h"
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
				UOSInt ieLen;
				UInt8 *ieBuff;
			} WifiLog;
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			Net::WirelessLAN wlan;
			Optional<Net::WirelessLAN::Interface> wlanInterf;
			OSInt wlanScan;
			Data::FastMapNN<UInt64, BSSStatus> bssMap;
			Data::FastMapNN<UInt64, WifiLog> wifiLogMap;
			Int64 lastTimeTick;

			NN<UI::GUITabControl> tcMain;

			NN<UI::GUITabPage> tpCurr;
			NN<UI::GUIListView> lvCurrWifi;

			NN<UI::GUITabPage> tpLogWifi;
			NN<UI::GUIListView> lvLogWifi;
			NN<UI::GUIPanel> pnlLogWifi;
			NN<UI::GUIButton> btnLogWifiSave;
			NN<UI::GUIButton> btnLogWifiSaveF;

			static void __stdcall OnTimerTick(AnyType userObj);
			static void __stdcall OnLogWifiDblClicked(AnyType userObj, UOSInt index);
			static void __stdcall OnLogWifiSaveClicked(AnyType userObj);
			static void __stdcall OnLogWifiSaveFClicked(AnyType userObj);
			static Bool __stdcall OnFormClosing(AnyType userObj, CloseReason reason);
		public:
			AVIRWifiCaptureLiteForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRWifiCaptureLiteForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
