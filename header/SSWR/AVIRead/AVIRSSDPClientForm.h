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
			NN<SSWR::AVIRead::AVIRCore> core;
			NN<Net::TCPClientFactory> clif;
			Optional<Net::SSLEngine> ssl;
			NN<Net::SSDPClient> ssdp;
			Data::FastStringMapNN<Net::SSDPClient::SSDPRoot> rootMap;

			NN<UI::GUIListBox> lbDevice;
			NN<UI::GUIHSplitter> hspDevice;
			NN<UI::GUIListBox> lbService;
			NN<UI::GUIHSplitter> hspService;
			NN<UI::GUIPanel> pnlService;
			NN<UI::GUILabel> lblLocation;
			NN<UI::GUITextBox> txtLocation;
			NN<UI::GUILabel> lblDate;
			NN<UI::GUITextBox> txtDate;
			NN<UI::GUILabel> lblUSN;
			NN<UI::GUITextBox> txtUSN;
			NN<UI::GUILabel> lblST;
			NN<UI::GUITextBox> txtST;
			NN<UI::GUILabel> lblServer;
			NN<UI::GUITextBox> txtServer;
			NN<UI::GUILabel> lblOpt;
			NN<UI::GUITextBox> txtOpt;
			NN<UI::GUILabel> lblUserAgent;
			NN<UI::GUITextBox> txtUserAgent;
			NN<UI::GUILabel> lblUDN;
			NN<UI::GUITextBox> txtUDN;
			NN<UI::GUILabel> lblFriendlyName;
			NN<UI::GUITextBox> txtFriendlyName;
			NN<UI::GUILabel> lblManufacturer;
			NN<UI::GUITextBox> txtManufacturer;
			NN<UI::GUILabel> lblManufacturerURL;
			NN<UI::GUITextBox> txtManufacturerURL;
			NN<UI::GUILabel> lblModelName;
			NN<UI::GUITextBox> txtModelName;
			NN<UI::GUILabel> lblModelNumber;
			NN<UI::GUITextBox> txtModelNumber;
			NN<UI::GUILabel> lblModelURL;
			NN<UI::GUITextBox> txtModelURL;
			NN<UI::GUILabel> lblSerialNumber;
			NN<UI::GUITextBox> txtSerialNumber;
			NN<UI::GUILabel> lblPresentationURL;
			NN<UI::GUITextBox> txtPresentationURL;
			NN<UI::GUILabel> lblDeviceType;
			NN<UI::GUITextBox> txtDeviceType;
			NN<UI::GUILabel> lblDeviceURL;
			NN<UI::GUITextBox> txtDeviceURL;

			static void __stdcall OnTimerTick(AnyType userObj);
			static void __stdcall OnDeviceSelChg(AnyType userObj);
			static void __stdcall OnServiceSelChg(AnyType userObj);
		public:
			AVIRSSDPClientForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRSSDPClientForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
