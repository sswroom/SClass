#ifndef _SM_SSWR_AVIREAD_AVIRTRUSTSTOREFORM
#define _SM_SSWR_AVIREAD_AVIRTRUSTSTOREFORM
#include "Crypto/Cert/CertStore.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIListView.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRTrustStoreForm : public UI::GUIForm
		{
		public:
			struct CertEntry
			{
				Crypto::Cert::X509Cert *cert;
				NotNullPtr<Text::String> subjectCN;
			};
		private:
			SSWR::AVIRead::AVIRCore *core;
			Net::SSLEngine *ssl;
			NotNullPtr<Net::SocketFactory> sockf;
			Crypto::Cert::CertStore *store;

			UI::GUIListView *lvTrustCert;
			static void __stdcall OnTrustCertDblClicked(void *userObj, UOSInt index);
		public:
			AVIRTrustStoreForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, SSWR::AVIRead::AVIRCore *core, Crypto::Cert::CertStore *store);
			virtual ~AVIRTrustStoreForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
