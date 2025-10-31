#ifndef _SM_SSWR_AVIREAD_AVIRGISSERVERFORM
#define _SM_SSWR_AVIREAD_AVIRGISSERVERFORM
#include "Map/GISWebHandler.h"
#include "Net/WebServer/WebListener.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUICheckBox.h"
#include "UI/GUIForm.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"
#include "UI/GUIPanel.h"
#include "UI/GUITabControl.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRGISServerForm : public UI::GUIForm
		{
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			Optional<Net::SSLEngine> ssl;
			Optional<Crypto::Cert::X509Cert> sslCert;
			Optional<Crypto::Cert::X509File> sslKey;
			Data::ArrayListNN<Crypto::Cert::X509Cert> caCerts;
			Map::GISWebHandler hdlr;
			Optional<Net::WebServer::WebListener> listener;

			NN<UI::GUIPanel> pnlConn;
			NN<UI::GUILabel> lblPort;
			NN<UI::GUITextBox> txtPort;
			NN<UI::GUILabel> lblSSL;
			NN<UI::GUICheckBox> chkSSL;
			NN<UI::GUIButton> btnSSLCert;
			NN<UI::GUILabel> lblSSLCert;
			NN<UI::GUILabel> lblWorkerCnt;
			NN<UI::GUITextBox> txtWorkerCnt;
			NN<UI::GUIButton> btnStart;
			NN<UI::GUITabControl> tcMain;

			NN<UI::GUITabPage> tpAsset;
			NN<UI::GUIListBox> lbAsset;
			NN<UI::GUIHSplitter> hspAsset;
			NN<UI::GUIPanel> pnlAsset;
			NN<UI::GUILabel> lblAssetPath;
			NN<UI::GUITextBox> txtAssetPath;
			NN<UI::GUILabel> lblAssetSRID;
			NN<UI::GUITextBox> txtAssetSRID;

			static void __stdcall OnSSLCertClicked(AnyType userObj);
			static void __stdcall OnStartClicked(AnyType userObj);
			static void __stdcall OnAssetSelChg(AnyType userObj);
			static void __stdcall OnFiles(AnyType userObj, Data::DataArray<NN<Text::String>> files);
			void ClearCACerts();
		public:
			AVIRGISServerForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRGISServerForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
