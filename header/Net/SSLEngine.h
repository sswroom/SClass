#ifndef _SM_NET_SSLENGINE
#define _SM_NET_SSLENGINE
#include "Crypto/Cert/CertStore.h"
#include "Crypto/Cert/X509Cert.h"
#include "Crypto/Cert/X509File.h"
#include "Crypto/Cert/X509Key.h"
#include "Crypto/Encrypt/RSACipher.h"
#include "Crypto/Hash/IHash.h"
#include "Net/SSLClient.h"
#include "Text/CString.h"

namespace Net
{
	class SSLEngine
	{
	public:
		enum class Method
		{
			Default,
			SSLV3,
			SSLV23,
			TLS,
			TLSV1,
			TLSV1_1,
			TLSV1_2,
			DTLS,
			DTLSV1,
			DTLSV1_2
		};

		enum class ErrorType
		{
			None,
			InitEnv,
			HostnameNotResolved,
			OutOfMemory,
			CannotConnect,
			InitSession,
			CertNotFound,
			InvalidName,
			SelfSign,
			InvalidPeriod
		};

		enum class ThreadStatus
		{
			NotRunning,
			Starting,
			Running,
			Processing,
			NewClient
		};

		enum class ClientCertType
		{
			None,
			Optional,
			MustExist
		};

		typedef void (__stdcall *ClientReadyHandler)(NotNullPtr<Net::TCPClient> cli, void *userObj);

		struct ThreadState
		{
			ThreadStatus status;
			Socket *s;
			ClientReadyHandler clientReady;
			void *clientReadyObj;
			Sync::Event *evt;
			SSLEngine *me;
		};
	protected:
		NotNullPtr<Net::SocketFactory> sockf;
		UOSInt maxThreadCnt;
		UOSInt currThreadCnt;
		Bool threadToStop;
		Sync::Mutex threadMut;
		ThreadState *threadSt;

		static Crypto::Cert::CertStore *trustStore;
		static UInt32 trustStoreCnt;
		Bool trustStoreUsed;

		static UInt32 __stdcall ServerThread(void *userObj);
		virtual Net::SSLClient *CreateServerConn(Socket *s) = 0;
		SSLEngine(NotNullPtr<Net::SocketFactory> sockf);
	public:
		virtual ~SSLEngine();
		
		virtual Bool IsError() = 0;

		virtual Bool ServerSetCertsASN1(Crypto::Cert::X509Cert *certASN1, Crypto::Cert::X509File *keyASN1, Crypto::Cert::X509Cert *caCert) = 0;
		virtual Bool ServerSetRequireClientCert(ClientCertType cliCert) = 0;
		virtual Bool ServerSetClientCA(Text::CString clientCA) = 0;
		virtual Bool ServerAddALPNSupport(Text::CString proto) = 0;
		Bool ServerSetCerts(Text::CStringNN certFile, Text::CStringNN keyFile);
		void ServerInit(Socket *s, ClientReadyHandler readyHdlr, void *userObj);

		virtual Bool ClientSetCertASN1(Crypto::Cert::X509Cert *certASN1, Crypto::Cert::X509File *keyASN1) = 0;
		virtual Net::SSLClient *ClientConnect(Text::CString hostName, UInt16 port, ErrorType *err, Data::Duration timeout) = 0;
		virtual Net::SSLClient *ClientInit(Socket *s, Text::CString hostName, ErrorType *err) = 0;
		virtual void ClientSetSkipCertCheck(Bool skipCertCheck) = 0;

		virtual UTF8Char *GetErrorDetail(UTF8Char *sbuff) = 0;
		virtual Bool GenerateCert(Text::CString country, Text::CString company, Text::CString commonName, Crypto::Cert::X509Cert **certASN1, Crypto::Cert::X509File **keyASN1) = 0;
		virtual Crypto::Cert::X509Key *GenerateRSAKey() = 0;
		virtual Bool Signature(Crypto::Cert::X509Key *key, Crypto::Hash::HashType hashType, const UInt8 *payload, UOSInt payloadLen, UInt8 *signData, UOSInt *signLen) = 0;
		virtual Bool SignatureVerify(Crypto::Cert::X509Key *key, Crypto::Hash::HashType hashType, const UInt8 *payload, UOSInt payloadLen, const UInt8 *signData, UOSInt signLen) = 0;
		virtual UOSInt Encrypt(Crypto::Cert::X509Key *key, UInt8 *encData, const UInt8 *payload, UOSInt payloadLen, Crypto::Encrypt::RSACipher::Padding rsaPadding) = 0;
		virtual UOSInt Decrypt(Crypto::Cert::X509Key *key, UInt8 *decData, const UInt8 *payload, UOSInt payloadLen, Crypto::Encrypt::RSACipher::Padding rsaPadding) = 0;

		Crypto::Cert::CertStore *GetTrustStore();

		static Text::CString ErrorTypeGetName(ErrorType err);
	};
}
#endif
