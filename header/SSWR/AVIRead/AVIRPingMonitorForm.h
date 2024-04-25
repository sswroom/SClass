#ifndef _SM_SSWR_AVIREAD_AVIRPINGMONITORFORM
#define _SM_SSWR_AVIREAD_AVIRPINGMONITORFORM

#include "IO/LogTool.h"
#include "Net/EthernetAnalyzer.h"
#include "Net/EthernetWebHandler.h"
#include "Net/SocketMonitor.h"
#include "Net/WhoisHandler.h"
#include "Net/WebServer/WebListener.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"
#include "UI/GUIPanel.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"
#include "UI/ListBoxLogger.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRPingMonitorForm : public UI::GUIForm
		{
		private:
			typedef struct
			{
				UInt32 ip;
				Int64 count;
				NN<Text::String> name;
				NN<Text::String> country;
			} IPInfo;

		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			NN<Net::SocketFactory> sockf;
			IO::LogTool log;
			NN<UI::ListBoxLogger> logger;
			Net::WebServer::WebListener *listener;
			Net::EthernetWebHandler *webHdlr;
			Net::SocketMonitor *socMon;
			Net::WhoisHandler whois;
			Net::EthernetAnalyzer analyzer;
			Sync::Mutex ipMut;
			Data::FastMap<UInt32, IPInfo*> ipMap;
			Bool ipListUpdated;
			Bool ipContUpdated;
			IPInfo *currIP;

			NN<UI::GUIPanel> pnlControl;
			NN<UI::GUILabel> lblInfo;
			NN<UI::GUITextBox> txtInfo;
			NN<UI::GUIButton> btnInfo;
			NN<UI::GUILabel> lblIP;
			NN<UI::GUIComboBox> cboIP;
			NN<UI::GUIButton> btnStart;
			NN<UI::GUITabControl> tcMain;

			NN<UI::GUITabPage> tpIP;
			NN<UI::GUIListBox> lbIP;
			NN<UI::GUIHSplitter> hspIP;
			NN<UI::GUITabControl> tcIP;
			NN<UI::GUITabPage> tpIPInfo;
			NN<UI::GUILabel> lblIPCount;
			NN<UI::GUITextBox> txtIPCount;
			NN<UI::GUILabel> lblIPName;
			NN<UI::GUITextBox> txtIPName;
			NN<UI::GUILabel> lblIPCountry;
			NN<UI::GUITextBox> txtIPCountry;
			NN<UI::GUITabPage> tpIPWhois;
			NN<UI::GUITextBox> txtIPWhois;

			NN<UI::GUITabPage> tpLog;
			NN<UI::GUITextBox> txtLog;
			NN<UI::GUIListBox> lbLog;

			static void __stdcall OnPingPacket(AnyType userData, UInt32 srcIP, UInt32 destIP, UInt8 ttl, UOSInt packetSize);
			static void __stdcall OnRAWData(AnyType userData, const UInt8 *rawData, UOSInt packetSize);
			static void __stdcall OnInfoClicked(AnyType userObj);
			static void __stdcall OnStartClicked(AnyType userObj);
			static void __stdcall OnLogSelChg(AnyType userObj);
			static void __stdcall OnIPSelChg(AnyType userObj);
			static void __stdcall OnTimerTick(AnyType userObj);

		public:
			AVIRPingMonitorForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRPingMonitorForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
