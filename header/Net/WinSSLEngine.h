#ifndef _SM_NET_WINSSLENGINE
#define _SM_NET_WINSSLENGINE
#include "Net/SSLEngine.h"

namespace Net
{
	class WinSSLEngine : public Net::SSLEngine
	{
	private:
		struct ClassData;
		struct ClassData *clsData;
		Bool skipCertCheck;
		ClientCertType cliCert;

		void DeinitClient();
		Bool InitClient(Method method, void *cred);
		Bool InitServer(Method method, void *cred, void *hRootStore);
		Optional<Net::SSLClient> CreateClientConn(void *sslObj, NN<Socket> s, Text::CStringNN hostName, OptOut<ErrorType> err);
		virtual Optional<Net::SSLClient> CreateServerConn(NN<Socket> s);
	public:
		WinSSLEngine(NN<Net::TCPClientFactory> clif, Method method);
		virtual ~WinSSLEngine();
		
		virtual Bool IsError();

		virtual Bool ServerSetCertsASN1(NN<Crypto::Cert::X509Cert> certASN1, NN<Crypto::Cert::X509File> keyASN1, NN<Data::ArrayListNN<Crypto::Cert::X509Cert>> cacerts);
		virtual Bool ServerSetRequireClientCert(ClientCertType cliCert);
		virtual Bool ServerSetClientCA(Text::CStringNN clientCA);
		virtual Bool ServerAddALPNSupport(Text::CStringNN proto);

		virtual void ClientSetSkipCertCheck(Bool skipCertCheck);
		virtual Bool ClientSetCertASN1(NN<Crypto::Cert::X509Cert> certASN1, NN<Crypto::Cert::X509File> keyASN1);
		virtual Optional<Net::SSLClient> ClientConnect(Text::CStringNN hostName, UInt16 port, OptOut<ErrorType> err, Data::Duration timeout);
		virtual Optional<Net::SSLClient> ClientInit(NN<Socket> s, Text::CStringNN hostName, OptOut<ErrorType> err);

		virtual UnsafeArray<UTF8Char> GetErrorDetail(UnsafeArray<UTF8Char> sbuff);
		virtual Bool GenerateCert(Text::CString country, Text::CString company, Text::CStringNN commonName, OutParam<NN<Crypto::Cert::X509Cert>> certASN1, OutParam<NN<Crypto::Cert::X509File>> keyASN1);
		virtual Optional<Crypto::Cert::X509Key> GenerateRSAKey();
		virtual Bool Signature(NN<Crypto::Cert::X509Key> key, Crypto::Hash::HashType hashType, Data::ByteArrayR payload, UnsafeArray<UInt8> signData, OutParam<UOSInt> signLen);
		virtual Bool SignatureVerify(NN<Crypto::Cert::X509Key> key, Crypto::Hash::HashType hashType, Data::ByteArrayR payload, Data::ByteArrayR signData);
		virtual UOSInt Encrypt(NN<Crypto::Cert::X509Key> key, UnsafeArray<UInt8> encData, Data::ByteArrayR payload, Crypto::Encrypt::RSACipher::Padding rsaPadding);
		virtual UOSInt Decrypt(NN<Crypto::Cert::X509Key> key, UnsafeArray<UInt8> decData, Data::ByteArrayR payload, Crypto::Encrypt::RSACipher::Padding rsaPadding);
	};
}
#endif
