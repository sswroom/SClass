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
			Data::ArrayList<Crypto::Cert::X509File *> fileList;

		public:
			X509FileList(Text::String *sourceName, Crypto::Cert::X509Cert *cert);
			X509FileList(Text::CString sourceName, Crypto::Cert::X509Cert *cert);
			virtual ~X509FileList();

			virtual FileType GetFileType() const;
			virtual void ToShortName(Text::StringBuilderUTF8 *sb) const;

			virtual UOSInt GetCertCount();
			virtual Bool GetCertName(UOSInt index, Text::StringBuilderUTF8 *sb);
			virtual X509Cert *GetNewCert(UOSInt index);
			virtual ValidStatus IsValid(Net::SSLEngine *ssl, Crypto::Cert::CertStore *trustStore) const;

			virtual ASN1Data *Clone() const;
			virtual X509Cert *CreateX509Cert() const;
			virtual void ToString(Text::StringBuilderUTF8 *sb) const;

			void AddFile(Crypto::Cert::X509File *file);
			UOSInt GetFileCount() const;
			Crypto::Cert::X509File *GetFile(UOSInt index) const;
		};
	}
}
#endif