#ifndef _SM_CRYPTO_CERT_X509FILELIST
#define _SM_CRYPTO_CERT_X509FILELIST
#include "Crypto/Cert/Certificate.h"
#include "Crypto/Cert/X509Cert.h"
#include "Crypto/Cert/X509File.h"

namespace Crypto
{
	namespace Cert
	{
		class X509FileList : public Crypto::Cert::X509File, public Crypto::Cert::Certificate
		{
		private:
			Data::ArrayListNN<Crypto::Cert::X509File> fileList;

		public:
			X509FileList(NN<Text::String> sourceName, NN<Crypto::Cert::X509Cert> cert);
			X509FileList(Text::CStringNN sourceName, NN<Crypto::Cert::X509Cert> cert);
			virtual ~X509FileList();

			virtual FileType GetFileType() const;
			virtual void ToShortName(NN<Text::StringBuilderUTF8> sb) const;

			virtual UOSInt GetCertCount();
			virtual Bool GetCertName(UOSInt index, NN<Text::StringBuilderUTF8> sb);
			virtual Optional<X509Cert> GetNewCert(UOSInt index);
			virtual ValidStatus IsValid(NN<Net::SSLEngine> ssl, Optional<Crypto::Cert::CertStore> trustStore) const;

			virtual NN<ASN1Data> Clone() const;
			virtual Optional<X509Cert> CreateX509Cert() const;
			virtual void ToString(NN<Text::StringBuilderUTF8> sb) const;
			virtual NN<Net::ASN1Names> CreateNames() const;

			void AddFile(NN<Crypto::Cert::X509File> file);
			UOSInt GetFileCount() const;
			Optional<Crypto::Cert::X509File> GetFile(UOSInt index) const;
			void SetDefaultSourceName();
		};
	}
}
#endif
