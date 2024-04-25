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
				NN<Crypto::Cert::X509Cert> cert;
				NN<Text::String> subjectCN;
			};
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			Optional<Net::SSLEngine> ssl;
			Optional<Crypto::Cert::CertStore> store;

			NN<UI::GUIListView> lvTrustCert;
			static void __stdcall OnTrustCertDblClicked(AnyType userObj, UOSInt index);
		public:
			AVIRTrustStoreForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, Optional<Crypto::Cert::CertStore> store);
			virtual ~AVIRTrustStoreForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
