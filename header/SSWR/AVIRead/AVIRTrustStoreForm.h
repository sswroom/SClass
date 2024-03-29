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
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Optional<Net::SSLEngine> ssl;
			Crypto::Cert::CertStore *store;

			NotNullPtr<UI::GUIListView> lvTrustCert;
			static void __stdcall OnTrustCertDblClicked(AnyType userObj, UOSInt index);
		public:
			AVIRTrustStoreForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, Crypto::Cert::CertStore *store);
			virtual ~AVIRTrustStoreForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
