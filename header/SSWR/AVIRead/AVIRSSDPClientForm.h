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
			SSWR::AVIRead::AVIRCore *core;
			Net::SocketFactory *sockf;
			Net::SSLEngine *ssl;
			Net::SSDPClient *ssdp;
			Data::FastStringMap<Net::SSDPClient::SSDPRoot*> *rootMap;

			UI::GUIListBox *lbDevice;
			UI::GUIHSplitter *hspDevice;
			UI::GUIListBox *lbService;
			UI::GUIHSplitter *hspService;
			UI::GUIPanel *pnlService;
			UI::GUILabel *lblLocation;
			UI::GUITextBox *txtLocation;
			UI::GUILabel *lblDate;
			UI::GUITextBox *txtDate;
			UI::GUILabel *lblUSN;
			UI::GUITextBox *txtUSN;
			UI::GUILabel *lblST;
			UI::GUITextBox *txtST;
			UI::GUILabel *lblServer;
			UI::GUITextBox *txtServer;
			UI::GUILabel *lblOpt;
			UI::GUITextBox *txtOpt;
			UI::GUILabel *lblUserAgent;
			UI::GUITextBox *txtUserAgent;
			UI::GUILabel *lblUDN;
			UI::GUITextBox *txtUDN;
			UI::GUILabel *lblFriendlyName;
			UI::GUITextBox *txtFriendlyName;
			UI::GUILabel *lblManufacturer;
			UI::GUITextBox *txtManufacturer;
			UI::GUILabel *lblManufacturerURL;
			UI::GUITextBox *txtManufacturerURL;
			UI::GUILabel *lblModelName;
			UI::GUITextBox *txtModelName;
			UI::GUILabel *lblModelNumber;
			UI::GUITextBox *txtModelNumber;
			UI::GUILabel *lblModelURL;
			UI::GUITextBox *txtModelURL;
			UI::GUILabel *lblSerialNumber;
			UI::GUITextBox *txtSerialNumber;
			UI::GUILabel *lblPresentationURL;
			UI::GUITextBox *txtPresentationURL;
			UI::GUILabel *lblDeviceType;
			UI::GUITextBox *txtDeviceType;
			UI::GUILabel *lblDeviceURL;
			UI::GUITextBox *txtDeviceURL;

			static void __stdcall OnTimerTick(void *userObj);
			static void __stdcall OnDeviceSelChg(void *userObj);
			static void __stdcall OnServiceSelChg(void *userObj);
		public:
			AVIRSSDPClientForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRSSDPClientForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
