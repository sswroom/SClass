#ifndef _SM_SSWR_AVIREAD_AVIRSSDPCLIENTFORM
#define _SM_SSWR_AVIREAD_AVIRSSDPCLIENTFORM
#include "Net/SSDPClient.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"
#include "UI/GUIPanel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRSSDPClientForm : public UI::GUIForm
		{
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			NotNullPtr<Net::SocketFactory> sockf;
			Optional<Net::SSLEngine> ssl;
			Net::SSDPClient *ssdp;
			Data::FastStringMap<Net::SSDPClient::SSDPRoot*> *rootMap;

			UI::GUIListBox *lbDevice;
			NotNullPtr<UI::GUIHSplitter> hspDevice;
			UI::GUIListBox *lbService;
			NotNullPtr<UI::GUIHSplitter> hspService;
			NotNullPtr<UI::GUIPanel> pnlService;
			NotNullPtr<UI::GUILabel> lblLocation;
			UI::GUITextBox *txtLocation;
			NotNullPtr<UI::GUILabel> lblDate;
			UI::GUITextBox *txtDate;
			NotNullPtr<UI::GUILabel> lblUSN;
			UI::GUITextBox *txtUSN;
			NotNullPtr<UI::GUILabel> lblST;
			UI::GUITextBox *txtST;
			NotNullPtr<UI::GUILabel> lblServer;
			UI::GUITextBox *txtServer;
			NotNullPtr<UI::GUILabel> lblOpt;
			UI::GUITextBox *txtOpt;
			NotNullPtr<UI::GUILabel> lblUserAgent;
			UI::GUITextBox *txtUserAgent;
			NotNullPtr<UI::GUILabel> lblUDN;
			UI::GUITextBox *txtUDN;
			NotNullPtr<UI::GUILabel> lblFriendlyName;
			UI::GUITextBox *txtFriendlyName;
			NotNullPtr<UI::GUILabel> lblManufacturer;
			UI::GUITextBox *txtManufacturer;
			NotNullPtr<UI::GUILabel> lblManufacturerURL;
			UI::GUITextBox *txtManufacturerURL;
			NotNullPtr<UI::GUILabel> lblModelName;
			UI::GUITextBox *txtModelName;
			NotNullPtr<UI::GUILabel> lblModelNumber;
			UI::GUITextBox *txtModelNumber;
			NotNullPtr<UI::GUILabel> lblModelURL;
			UI::GUITextBox *txtModelURL;
			NotNullPtr<UI::GUILabel> lblSerialNumber;
			UI::GUITextBox *txtSerialNumber;
			NotNullPtr<UI::GUILabel> lblPresentationURL;
			UI::GUITextBox *txtPresentationURL;
			NotNullPtr<UI::GUILabel> lblDeviceType;
			UI::GUITextBox *txtDeviceType;
			NotNullPtr<UI::GUILabel> lblDeviceURL;
			UI::GUITextBox *txtDeviceURL;

			static void __stdcall OnTimerTick(void *userObj);
			static void __stdcall OnDeviceSelChg(void *userObj);
			static void __stdcall OnServiceSelChg(void *userObj);
		public:
			AVIRSSDPClientForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRSSDPClientForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
