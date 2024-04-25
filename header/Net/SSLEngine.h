#ifndef _SM_NET_SSLENGINE
#define _SM_NET_SSLENGINE
#include "AnyType.h"
#include "Crypto/Cert/CertStore.h"
#include "Crypto/Cert/X509Cert.h"
#include "Crypto/Cert/X509File.h"
#include "Crypto/Cert/X509Key.h"
#include "Crypto/Encrypt/RSACipher.h"
#include "Crypto/Hash/IHash.h"
#include "Data/ArrayListNN.h"
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

		typedef void (__stdcall *ClientReadyHandler)(NN<Net::TCPClient> cli, AnyType userObj);

		struct ThreadState
		{
			ThreadStatus status;
			Socket *s;
			UOSInt index;
			ClientReadyHandler clientReady;
			AnyType clientReadyObj;
			Sync::Event *evt;
			SSLEngine *me;
		};
	protected:
		NN<Net::SocketFactory> sockf;
		UOSInt maxThreadCnt;
		UOSInt currThreadCnt;
		Bool threadToStop;
		Sync::Mutex threadMut;
		ThreadState *threadSt;

		static Optional<Crypto::Cert::CertStore> trustStore;
		static UInt32 trustStoreCnt;
		Optional<Crypto::Cert::CertStore> usedTrustStore;

		static UInt32 __stdcall ServerThread(AnyType userObj);
		virtual Net::SSLClient *CreateServerConn(Socket *s) = 0;
		SSLEngine(NN<Net::SocketFactory> sockf);
	public:
		virtual ~SSLEngine();
		
		virtual Bool IsError() = 0;

		virtual Bool ServerSetCertsASN1(NN<Crypto::Cert::X509Cert> certASN1, NN<Crypto::Cert::X509File> keyASN1, NN<Data::ArrayListNN<Crypto::Cert::X509Cert>> cacerts) = 0;
		virtual Bool ServerSetRequireClientCert(ClientCertType cliCert) = 0;
		virtual Bool ServerSetClientCA(Text::CStringNN clientCA) = 0;
		virtual Bool ServerAddALPNSupport(Text::CStringNN proto) = 0;
		Bool ServerSetCerts(Text::CStringNN certFile, Text::CStringNN keyFile, Text::CString caFile);
		Bool ServerSetCerts(Text::CStringNN certFile, Text::CStringNN keyFile);
		void ServerInit(Socket *s, ClientReadyHandler readyHdlr, AnyType userObj);

		virtual Bool ClientSetCertASN1(NN<Crypto::Cert::X509Cert> certASN1, NN<Crypto::Cert::X509File> keyASN1) = 0;
		virtual Net::SSLClient *ClientConnect(Text::CStringNN hostName, UInt16 port, OptOut<ErrorType> err, Data::Duration timeout) = 0;
		virtual Net::SSLClient *ClientInit(Socket *s, Text::CStringNN hostName, OptOut<ErrorType> err) = 0;
		virtual void ClientSetSkipCertCheck(Bool skipCertCheck) = 0;

		virtual UTF8Char *GetErrorDetail(UTF8Char *sbuff) = 0;
		virtual Bool GenerateCert(Text::CString country, Text::CString company, Text::CStringNN commonName, OutParam<Crypto::Cert::X509Cert*> certASN1, OutParam<Crypto::Cert::X509File*> keyASN1) = 0;
		virtual Optional<Crypto::Cert::X509Key> GenerateRSAKey() = 0;
		virtual Bool Signature(NN<Crypto::Cert::X509Key> key, Crypto::Hash::HashType hashType, const UInt8 *payload, UOSInt payloadLen, UInt8 *signData, OutParam<UOSInt> signLen) = 0;
		virtual Bool SignatureVerify(NN<Crypto::Cert::X509Key> key, Crypto::Hash::HashType hashType, const UInt8 *payload, UOSInt payloadLen, const UInt8 *signData, UOSInt signLen) = 0;
		virtual UOSInt Encrypt(NN<Crypto::Cert::X509Key> key, UInt8 *encData, const UInt8 *payload, UOSInt payloadLen, Crypto::Encrypt::RSACipher::Padding rsaPadding) = 0;
		virtual UOSInt Decrypt(NN<Crypto::Cert::X509Key> key, UInt8 *decData, const UInt8 *payload, UOSInt payloadLen, Crypto::Encrypt::RSACipher::Padding rsaPadding) = 0;

		NN<Crypto::Cert::CertStore> GetTrustStore();

		static Text::CStringNN ErrorTypeGetName(ErrorType err);
	};
}
#endif
