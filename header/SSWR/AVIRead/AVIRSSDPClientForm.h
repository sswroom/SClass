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

			NotNullPtr<UI::GUIListBox> lbDevice;
			NotNullPtr<UI::GUIHSplitter> hspDevice;
			NotNullPtr<UI::GUIListBox> lbService;
			NotNullPtr<UI::GUIHSplitter> hspService;
			NotNullPtr<UI::GUIPanel> pnlService;
			NotNullPtr<UI::GUILabel> lblLocation;
			NotNullPtr<UI::GUITextBox> txtLocation;
			NotNullPtr<UI::GUILabel> lblDate;
			NotNullPtr<UI::GUITextBox> txtDate;
			NotNullPtr<UI::GUILabel> lblUSN;
			NotNullPtr<UI::GUITextBox> txtUSN;
			NotNullPtr<UI::GUILabel> lblST;
			NotNullPtr<UI::GUITextBox> txtST;
			NotNullPtr<UI::GUILabel> lblServer;
			NotNullPtr<UI::GUITextBox> txtServer;
			NotNullPtr<UI::GUILabel> lblOpt;
			NotNullPtr<UI::GUITextBox> txtOpt;
			NotNullPtr<UI::GUILabel> lblUserAgent;
			NotNullPtr<UI::GUITextBox> txtUserAgent;
			NotNullPtr<UI::GUILabel> lblUDN;
			NotNullPtr<UI::GUITextBox> txtUDN;
			NotNullPtr<UI::GUILabel> lblFriendlyName;
			NotNullPtr<UI::GUITextBox> txtFriendlyName;
			NotNullPtr<UI::GUILabel> lblManufacturer;
			NotNullPtr<UI::GUITextBox> txtManufacturer;
			NotNullPtr<UI::GUILabel> lblManufacturerURL;
			NotNullPtr<UI::GUITextBox> txtManufacturerURL;
			NotNullPtr<UI::GUILabel> lblModelName;
			NotNullPtr<UI::GUITextBox> txtModelName;
			NotNullPtr<UI::GUILabel> lblModelNumber;
			NotNullPtr<UI::GUITextBox> txtModelNumber;
			NotNullPtr<UI::GUILabel> lblModelURL;
			NotNullPtr<UI::GUITextBox> txtModelURL;
			NotNullPtr<UI::GUILabel> lblSerialNumber;
			NotNullPtr<UI::GUITextBox> txtSerialNumber;
			NotNullPtr<UI::GUILabel> lblPresentationURL;
			NotNullPtr<UI::GUITextBox> txtPresentationURL;
			NotNullPtr<UI::GUILabel> lblDeviceType;
			NotNullPtr<UI::GUITextBox> txtDeviceType;
			NotNullPtr<UI::GUILabel> lblDeviceURL;
			NotNullPtr<UI::GUITextBox> txtDeviceURL;

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
