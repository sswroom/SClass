#include "Stdafx.h"
#include "Crypto/Cert/X509Cert.h"
#include "Crypto/Cert/X509PubKey.h"
#include "Crypto/Cert/X509PrivKey.h"
#include "Crypto/Hash/HashCreator.h"
#include "Data/DateTime.h"
//#include "IO/DebugWriter.h"
#include "IO/WindowsError.h"
#include "Net/ASN1PDUBuilder.h"
#include "Net/WinSSLClient.h"
#include "Net/WinSSLEngine.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/StringBuilderW.h"
#define SCHANNEL_USE_BLACKLISTS
#include <windows.h>
#include <ntsecapi.h>
#define SECURITY_WIN32
#include <sspi.h>
#include <schnlsp.h>
#include <bcrypt.h>

#if defined(__CYGWIN__)
#define FLAGS_ULONG unsigned int
#else
#define FLAGS_ULONG unsigned long
#endif
//#define VERBOSE_SVR
//#define VERBOSE_CLI
#if defined(VERBOSE_SVR) || defined(VERBOSE_CLI)
#include <stdio.h>
#include "WinDebug.h"
#endif

struct Net::WinSSLEngine::ClassData
{
	CredHandle hCredCli;
	Bool cliInit;
	CredHandle hCredSvr;
	Bool svrInit;
	Method method;
};

void SecBuffer_Set(SecBuffer *buff, UInt32 type, UInt8 *inpBuff, UInt32 leng)
{
	buff->BufferType = type;
	buff->pvBuffer = inpBuff;
	buff->cbBuffer = leng;
}

void SecBufferDesc_Set(SecBufferDesc *desc, SecBuffer *buffs, UInt32 nBuffs)
{
	desc->ulVersion = SECBUFFER_VERSION;
	desc->pBuffers = buffs;
	desc->cBuffers = nBuffs;
}

void WinSSLEngine_PrintCERT_CONTEXT(PCCERT_CONTEXT cert)
{
#if defined(VERBOSE_SVR) || defined(VERBOSE_CLI)
	UInt8 tmpBuff[1024];
	DWORD retSize = sizeof(tmpBuff);
	CRYPT_KEY_PROV_INFO *keyProvInfo = (CRYPT_KEY_PROV_INFO*)tmpBuff;
	if (CertGetCertificateContextProperty(cert, CERT_KEY_PROV_INFO_PROP_ID, keyProvInfo, &retSize))
	{
		printf("Get CRYPT_KEY_PROV_INFO, size = %d\r\n", retSize);
		printf("Key Container Name = %ls\r\n", keyProvInfo->pwszContainerName);
	}
#endif
}

UInt32 WinSSLEngine_GetProtocols(Net::SSLEngine::Method method, Bool server)
{
	if (server)
	{
		switch (method)
		{
		case Net::SSLEngine::Method::Default:
		#if defined(SP_PROT_TLS1_2)
			return SP_PROT_TLS1_2_SERVER;
		#else
			return 0;
		#endif
		case Net::SSLEngine::Method::SSLV3:
			return SP_PROT_SSL3_SERVER;
		case Net::SSLEngine::Method::SSLV23:
			return SP_PROT_SSL2_SERVER;
		#if defined(SP_PROT_TLS1_2)
		case Net::SSLEngine::Method::TLS:
			return SP_PROT_TLS1_1_SERVER;
		case Net::SSLEngine::Method::TLSV1:
			return SP_PROT_TLS1_0_SERVER;
		case Net::SSLEngine::Method::TLSV1_1:
			return SP_PROT_TLS1_1_SERVER;
		case Net::SSLEngine::Method::TLSV1_2:
			return SP_PROT_TLS1_2_SERVER;
		case Net::SSLEngine::Method::DTLS:
			return SP_PROT_DTLS1_0_SERVER;
		case Net::SSLEngine::Method::DTLSV1:
			return SP_PROT_DTLS1_0_SERVER;
		case Net::SSLEngine::Method::DTLSV1_2:
#if defined(SP_PROT_DTLS1_2_SERVER)
			return SP_PROT_DTLS1_2_SERVER;
		default:
			return SP_PROT_TLS1_0_SERVER | SP_PROT_TLS1_1_SERVER | SP_PROT_TLS1_2_SERVER | SP_PROT_TLS1_3_SERVER;
#else
			return SP_PROT_TLS1_2_SERVER;
		default:
			return SP_PROT_TLS1_0_SERVER | SP_PROT_TLS1_1_SERVER | SP_PROT_TLS1_2_SERVER;
#endif
		#else
		case Net::SSLEngine::Method::TLS:
			return SP_PROT_TLS1;
		case Net::SSLEngine::Method::TLSV1:
			return SP_PROT_TLS1;
		case Net::SSLEngine::Method::TLSV1_1:
			return SP_PROT_TLS1;
		case Net::SSLEngine::Method::TLSV1_2:
			return SP_PROT_TLS1;
		case Net::SSLEngine::Method::DTLS:
			return SP_PROT_TLS1;
		case Net::SSLEngine::Method::DTLSV1:
			return SP_PROT_TLS1;
		case Net::SSLEngine::Method::DTLSV1_2:
			return SP_PROT_TLS1;
		default:
			return SP_PROT_TLS1;
		#endif
		}
	}
	else
	{
		switch (method)
		{
		case Net::SSLEngine::Method::Default:
			return 0;
		case Net::SSLEngine::Method::SSLV3:
			return SP_PROT_SSL3_CLIENT;
		case Net::SSLEngine::Method::SSLV23:
			return SP_PROT_SSL3_CLIENT;
		#if defined(SP_PROT_TLS1_2)
		case Net::SSLEngine::Method::TLS:
			return SP_PROT_TLS1_2_CLIENT;
		case Net::SSLEngine::Method::TLSV1:
			return SP_PROT_TLS1_0_CLIENT;
		case Net::SSLEngine::Method::TLSV1_1:
			return SP_PROT_TLS1_1_CLIENT;
		case Net::SSLEngine::Method::TLSV1_2:
			return SP_PROT_TLS1_2_CLIENT;
		case Net::SSLEngine::Method::DTLSV1:
			return SP_PROT_DTLS1_0_CLIENT;
#if defined(SP_PROT_DTLS1_2_CLIENT)
		case Net::SSLEngine::Method::DTLS:
			return SP_PROT_DTLS1_2_CLIENT;
		case Net::SSLEngine::Method::DTLSV1_2:
			return SP_PROT_DTLS1_2_CLIENT;
#else
		case Net::SSLEngine::Method::DTLS:
			return SP_PROT_TLS1_2_CLIENT;
		case Net::SSLEngine::Method::DTLSV1_2:
			return SP_PROT_TLS1_2_CLIENT;
#endif
		default:
			return SP_PROT_TLS1_2_CLIENT;
		#else
		case Net::SSLEngine::Method::TLS:
			return SP_PROT_TLS1_CLIENT;
		case Net::SSLEngine::Method::TLSV1:
			return SP_PROT_TLS1_CLIENT;
		case Net::SSLEngine::Method::TLSV1_1:
			return SP_PROT_TLS1_CLIENT;
		case Net::SSLEngine::Method::TLSV1_2:
			return SP_PROT_TLS1_CLIENT;
		case Net::SSLEngine::Method::DTLS:
			return SP_PROT_TLS1_CLIENT;
		case Net::SSLEngine::Method::DTLSV1:
			return SP_PROT_TLS1_CLIENT;
		case Net::SSLEngine::Method::DTLSV1_2:
			return SP_PROT_TLS1_CLIENT;
		default:
			return SP_PROT_TLS1_CLIENT;
		#endif
		}
	}
}

ALG_ID WinSSLEngine_CryptGetHashAlg(Crypto::Hash::HashType hashType)
{
	if (hashType == Crypto::Hash::HashType::SHA256)
	{
		return CALG_SHA_256;
	}
	else if (hashType == Crypto::Hash::HashType::SHA384)
	{
		return CALG_SHA_384;
	}
	else if (hashType == Crypto::Hash::HashType::SHA512)
	{
		return CALG_SHA_512;
	}
	else if (hashType == Crypto::Hash::HashType::SHA1)
	{
		return CALG_SHA1;
	}
	else if (hashType == Crypto::Hash::HashType::MD5)
	{
		return CALG_MD5;
	}
	else
	{
#if defined(VERBOSE_SVR) || defined(VERBOSE_CLI)
		printf("SSL: hashType not supported\r\n");
#endif
		return 0;
	}
}


void WinSSLEngine_HCRYPTKEY_ToString(HCRYPTKEY hKey, Text::StringBuilderUTF *sb)
{
	UInt8 buff[4096];
	DWORD dataLen;
	ALG_ID algId;
	dataLen = sizeof(algId);
	if (CryptGetKeyParam(hKey, KP_ALGID, (BYTE*)&algId, &dataLen, 0))
	{
		sb->AppendC(UTF8STRC("Key ALG ID = 0x"));
		sb->AppendHex32(algId);
		sb->AppendC(UTF8STRC(", Class="));
		switch (GET_ALG_CLASS(algId))
		{
		case ALG_CLASS_ANY:
			sb->AppendC(UTF8STRC("Any"));
			break;
		case ALG_CLASS_SIGNATURE:
			sb->AppendC(UTF8STRC("Signature"));
			break;
		case ALG_CLASS_MSG_ENCRYPT:
			sb->AppendC(UTF8STRC("Message Encrypt"));
			break;
		case ALG_CLASS_DATA_ENCRYPT:
			sb->AppendC(UTF8STRC("Data Encrypt"));
			break;
		case ALG_CLASS_HASH:
			sb->AppendC(UTF8STRC("Hash"));
			break;
		case ALG_CLASS_KEY_EXCHANGE:
			sb->AppendC(UTF8STRC("Key Exchange"));
			break;
		case ALG_CLASS_ALL:
			sb->AppendC(UTF8STRC("All"));
			break;
		default:
			sb->AppendC(UTF8STRC("Unknown"));
			break;
		}
		sb->AppendC(UTF8STRC(", Type="));
		switch (GET_ALG_TYPE(algId))
		{
		case ALG_TYPE_ANY:
			sb->AppendC(UTF8STRC("Any"));
			break;
		case ALG_TYPE_DSS:
			sb->AppendC(UTF8STRC("DSS"));
			break;
		case ALG_TYPE_RSA:
			sb->AppendC(UTF8STRC("RSA"));
			break;
		case ALG_TYPE_BLOCK:
			sb->AppendC(UTF8STRC("Block"));
			break;
		case ALG_TYPE_STREAM:
			sb->AppendC(UTF8STRC("Stream"));
			break;
		case ALG_TYPE_DH:
			sb->AppendC(UTF8STRC("DH"));
			break;
		case ALG_TYPE_SECURECHANNEL:
			sb->AppendC(UTF8STRC("Secure Channel"));
			break;
#if (NTDDI_VERSION >= NTDDI_VISTA) && defined(ALG_TYPE_ECDH)
		case ALG_TYPE_ECDH:
			sb->AppendC(UTF8STRC("ECDH"));
			break;
#endif
#if defined(_MSC_VER) && (NTDDI_VERSION >= NTDDI_WIN10_RS1) && defined(ALG_TYPE_THIRDPARTY)
		case ALG_TYPE_THIRDPARTY:
			sb->AppendC(UTF8STRC("ThirdParty"));
			break;
#endif
		default:
			sb->AppendC(UTF8STRC("Unknown"));
			break;
		}
		sb->AppendC(UTF8STRC(", SID="));
		sb->AppendU32(GET_ALG_SID(algId));
		sb->AppendC(UTF8STRC("\r\n"));
	}
	DWORD blockLen;
	dataLen = sizeof(blockLen);
	if (CryptGetKeyParam(hKey, KP_BLOCKLEN, (BYTE*)&blockLen, &dataLen, 0))
	{
		sb->AppendC(UTF8STRC("Key BlockLen="));
		sb->AppendU32(blockLen);
		sb->AppendC(UTF8STRC("\r\n"));
	}
	dataLen = sizeof(buff);
	if (CryptGetKeyParam(hKey, KP_CERTIFICATE, buff, &dataLen, 0))
	{
		sb->AppendC(UTF8STRC("Key Certificate=\r\n"));
		sb->AppendHexBuff(buff, dataLen, ' ', Text::LineBreakType::CRLF);
		sb->AppendC(UTF8STRC("\r\n"));
	}
	dataLen = sizeof(blockLen);
	if (CryptGetKeyParam(hKey, KP_KEYLEN, (BYTE*)&blockLen, &dataLen, 0))
	{
		sb->AppendC(UTF8STRC("Key Len="));
		sb->AppendU32(blockLen);
		sb->AppendC(UTF8STRC("\r\n"));
	}
	dataLen = sizeof(buff);
	if (CryptGetKeyParam(hKey, KP_SALT, buff, &dataLen, 0))
	{
		sb->AppendC(UTF8STRC("Key Salt="));
		sb->AppendHexBuff(buff, dataLen, ' ', Text::LineBreakType::None);
		sb->AppendC(UTF8STRC("\r\n"));
	}
	dataLen = sizeof(blockLen);
	if (CryptGetKeyParam(hKey, KP_PERMISSIONS, (BYTE*)&blockLen, &dataLen, 0))
	{
		sb->AppendC(UTF8STRC("Key Permissions=0x"));
		sb->AppendHex32(blockLen);
		sb->AppendC(UTF8STRC("\r\n"));
	}
}

void WinSSLEngine_HCRYPTPROV_ToString(HCRYPTPROV hProv, Text::StringBuilderUTF *sb)
{
	HCRYPTKEY hKey;
	if (CryptGetUserKey(hProv, AT_KEYEXCHANGE, &hKey))
	{
		sb->AppendC(UTF8STRC("KEYEXCHANGE: Success,\r\n"));
		WinSSLEngine_HCRYPTKEY_ToString(hKey, sb);
		CryptDestroyKey(hKey);
	}
	else
	{
		sb->AppendC(UTF8STRC("KEYEXCHANGE: Failed, code = 0x"));
		sb->AppendHex32(GetLastError());
		sb->AppendC(UTF8STRC("\r\n"));
	}

	if (CryptGetUserKey(hProv, AT_SIGNATURE, &hKey))
	{
		sb->AppendC(UTF8STRC("SIGNATURE: Success,\r\n"));
		WinSSLEngine_HCRYPTKEY_ToString(hKey, sb);
		CryptDestroyKey(hKey);
	}
	else
	{
		sb->AppendC(UTF8STRC("SIGNATURE: Failed, code = 0x"));
		sb->AppendHex32(GetLastError());
		sb->AppendC(UTF8STRC("\r\n"));
	}
}

Bool WinSSLEngine_CryptImportRSAPrivateKey(_Out_ HCRYPTKEY* phKey,
	_In_ HCRYPTPROV hProv,
	_In_ const UInt8 *pbKey,
	_In_ ULONG cbKey,
	Bool signature)
{
	ULONG cb;
	PCRYPT_PRIVATE_KEY_INFO PrivateKeyInfo;

	BOOL succ = CryptDecodeObjectEx(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, PKCS_PRIVATE_KEY_INFO,
		pbKey, cbKey, CRYPT_DECODE_ALLOC_FLAG | CRYPT_DECODE_NOCOPY_FLAG, 0, (void**)&PrivateKeyInfo, &cb);

	if (succ)
	{
		PUBLICKEYSTRUC* ppks;

		succ = CryptDecodeObjectEx(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
			PKCS_RSA_PRIVATE_KEY, PrivateKeyInfo->PrivateKey.pbData, PrivateKeyInfo->PrivateKey.cbData,
			CRYPT_DECODE_ALLOC_FLAG, 0, (void**)&ppks, &cb);

		LocalFree(PrivateKeyInfo);

		if (succ)
		{
			if (signature && ppks->aiKeyAlg == CALG_RSA_KEYX)
			{
				ppks->aiKeyAlg = CALG_RSA_SIGN;
			}
			succ = CryptImportKey(hProv, (PUCHAR)ppks, cb, 0, CRYPT_EXPORTABLE, phKey);
			if (!succ)
			{
#if defined(VERBOSE_SVR) || defined(VERBOSE_CLI)
				printf("WinSSLEngine_CryptImportRSAPrivateKey: Import Key failed: CryptImportKey\r\n");
#endif
			}
			LocalFree(ppks);
		}
		else
		{
#if defined(VERBOSE_SVR) || defined(VERBOSE_CLI)
			printf("WinSSLEngine_CryptImportRSAPrivateKey: Import Key failed: CryptDecodeObjectEx\r\n");
#endif
		}
	}
	else
	{
#if defined(VERBOSE_SVR) || defined(VERBOSE_CLI)
		printf("WinSSLEngine_CryptImportRSAPrivateKey: Import Key failed: GetKeyDecodeSize\r\n");
#endif
	}

	return (succ != FALSE);
}

Bool WinSSLEngine_CryptImportECDSAPrivateKey(_Out_ HCRYPTKEY* phKey,
	_In_ HCRYPTPROV hProv,
	_In_ const UInt8* pbKey,
	_In_ ULONG cbKey,
	Bool signature)
{
	PCRYPT_PRIVATE_KEY_INFO PrivateKeyInfo;
	ULONG pkeyLeng;

	BOOL succ = CryptDecodeObjectEx(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, PKCS_PRIVATE_KEY_INFO,
		pbKey, cbKey, CRYPT_DECODE_ALLOC_FLAG | CRYPT_DECODE_NOCOPY_FLAG, 0, (void**)&PrivateKeyInfo, &pkeyLeng);

	if (succ)
	{
		PUBLICKEYSTRUC* ppks;
		ULONG ppksLen;

		succ = CryptDecodeObjectEx(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
			X509_ECC_PRIVATE_KEY, PrivateKeyInfo->PrivateKey.pbData, PrivateKeyInfo->PrivateKey.cbData,
			CRYPT_DECODE_ALLOC_FLAG, 0, (void**)&ppks, &ppksLen);

		if (succ)
		{
			succ = CryptImportKey(hProv, (PUCHAR)PrivateKeyInfo, pkeyLeng, 0, CRYPT_EXPORTABLE, phKey);
			if (!succ)
			{
#if defined(VERBOSE_SVR) || defined(VERBOSE_CLI)
				printf("WinSSLEngine_CryptImportECDSAPrivateKey: Import Key failed: CryptImportKey, code = %x\r\n", GetLastError());
#endif
			}
			LocalFree(ppks);
		}
		else
		{
#if defined(VERBOSE_SVR) || defined(VERBOSE_CLI)
			printf("WinSSLEngine_CryptImportECDSAPrivateKey: Import Key failed: CryptDecodeObjectEx, size = %ld\r\n", pkeyLeng);
#endif
		}
		LocalFree(PrivateKeyInfo);
	}
	else
	{
#if defined(VERBOSE_SVR) || defined(VERBOSE_CLI)
		printf("WinSSLEngine_CryptImportECDSAPrivateKey: Import Key failed: GetKeyDecodeSize\r\n");
#endif
	}

	return (succ != FALSE);
}

Bool WinSSLEngine_CryptImportPublicKey(_Out_ HCRYPTKEY* phKey,
	_In_ HCRYPTPROV hProv,
	_In_ const UInt8 *pbKey,
	_In_ ULONG cbKey,
	Bool signature)
{
	ULONG cb;
	CERT_PUBLIC_KEY_INFO *publicKeyInfo;

	BOOL succ = CryptDecodeObjectEx(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, X509_PUBLIC_KEY_INFO,
		pbKey, cbKey, CRYPT_DECODE_ALLOC_FLAG, 0, (void**)&publicKeyInfo, &cb);

	if (succ)
	{

		succ = CryptImportPublicKeyInfo(hProv, X509_ASN_ENCODING, publicKeyInfo, phKey);
		LocalFree(publicKeyInfo);

#if defined(VERBOSE_SVR) || defined(VERBOSE_CLI)
		if (!succ)
		{
			printf("SSL: Import Pub Key failed: CryptImportPublicKeyInfo\r\n");
		}
#endif
	}
	else
	{
#if defined(VERBOSE_SVR) || defined(VERBOSE_CLI)
		printf("SSL: Import Pub Key failed: CryptDecodeObjectEx, error = 0x%x\r\n", (UInt32)GetLastError());
#endif
	}

	return (succ != FALSE);
}

BOOL WinSSLEngine_CryptAcquireContextW(HCRYPTPROV* phProv, LPCWSTR szContainer, LPCWSTR szProvider, DWORD dwProvType, DWORD dwFlags)
{
	if (CryptAcquireContextW(phProv, szContainer, szProvider, dwProvType, dwFlags))
	{
		return TRUE;
	}
	if (GetLastError() == (DWORD)NTE_BAD_KEYSET)
	{
		return CryptAcquireContextW(phProv, szContainer, szProvider, dwProvType, dwFlags | CRYPT_NEWKEYSET);
	}
	return FALSE;
}

HCRYPTPROV WinSSLEngine_CreateProv(Crypto::Cert::X509File::KeyType keyType, const WChar *containerName, CRYPT_KEY_PROV_INFO *keyProvInfo)
{
	HCRYPTPROV hProv;
	DWORD flags;
	if (containerName == 0)
	{
		flags = CRYPT_VERIFYCONTEXT;
	}
	else
	{
		flags = 0;
	}

	if (keyType == Crypto::Cert::X509File::KeyType::RSA || keyType == Crypto::Cert::X509File::KeyType::RSAPublic)
	{
/*		if (WinSSLEngine_CryptAcquireContextW(&hProv, containerName, MS_ENHANCED_PROV, PROV_RSA_FULL, flags))
		{
			if (keyProvInfo)
			{
				keyProvInfo->pwszContainerName = (LPWSTR)containerName;
				keyProvInfo->pwszProvName = (LPWSTR)MS_ENHANCED_PROV;
				keyProvInfo->dwProvType = PROV_RSA_FULL;
				keyProvInfo->dwFlags = flags;
			}
			return hProv;
		}
#if defined(VERBOSE_SVR) || defined(VERBOSE_CLI)
		printf("SSL: CryptAcquireContext RSA (MS_ENHANCED_PROV) failed: 0x%x\r\n", GetLastError());
#endif*/
		if (WinSSLEngine_CryptAcquireContextW(&hProv, containerName, MS_ENH_RSA_AES_PROV_W, PROV_RSA_AES, flags))
		{
			if (keyProvInfo)
			{
				keyProvInfo->pwszContainerName = (LPWSTR)containerName;
				keyProvInfo->pwszProvName = (LPWSTR)MS_ENH_RSA_AES_PROV_W;
				keyProvInfo->dwProvType = PROV_RSA_AES;
				keyProvInfo->dwFlags = flags;
			}
			return hProv;
		}
#if defined(VERBOSE_SVR) || defined(VERBOSE_CLI)
		printf("SSL: CryptAcquireContext RSA (AES_PROV) failed: 0x%x\r\n", GetLastError());
#endif
		if (WinSSLEngine_CryptAcquireContextW(&hProv, containerName, NULL, PROV_RSA_FULL, flags))
		{
			if (keyProvInfo)
			{
				keyProvInfo->pwszContainerName = (LPWSTR)containerName;
				keyProvInfo->pwszProvName = NULL;
				keyProvInfo->dwProvType = PROV_RSA_FULL;
				keyProvInfo->dwFlags = flags;
			}
			return hProv;
		}
#if defined(VERBOSE_SVR) || defined(VERBOSE_CLI)
		printf("SSL: CryptAcquireContext RSA failed: 0x%x\r\n", GetLastError());
#endif
		return 0;
	}
	else if (keyType == Crypto::Cert::X509File::KeyType::ECDSA || keyType == Crypto::Cert::X509File::KeyType::ECPublic)
	{
		if (WinSSLEngine_CryptAcquireContextW(&hProv, containerName, NULL, PROV_RSA_FULL, flags))
		{
			if (keyProvInfo)
			{
				keyProvInfo->pwszContainerName = (LPWSTR)containerName;
				keyProvInfo->pwszProvName = NULL;
				keyProvInfo->dwProvType = PROV_RSA_FULL;
				keyProvInfo->dwFlags = flags;
			}
			return hProv;
		}
#if defined(VERBOSE_SVR) || defined(VERBOSE_CLI)
		printf("SSL: CryptAcquireContext ECDSA failed: %x\r\n", (UInt32)GetLastError());
#endif
		return 0;
	}
	return 0;
}

HCRYPTKEY WinSSLEngine_ImportKey(HCRYPTPROV hProv, NN<Crypto::Cert::X509Key> key, Bool privateKeyOnly, Bool signature)
{
	HCRYPTKEY hKey;
	Crypto::Cert::X509File::KeyType keyType = key->GetKeyType();
	NN<Crypto::Cert::X509PrivKey> privKey;
	if (keyType == Crypto::Cert::X509File::KeyType::RSA && Crypto::Cert::X509PrivKey::CreateFromKey(key).SetTo(privKey))
	{
		if (WinSSLEngine_CryptImportRSAPrivateKey(&hKey, hProv, privKey->GetASN1Buff().Ptr(), (ULONG)privKey->GetASN1BuffSize(), signature))
		{
			privKey.Delete();
			return hKey;
		}
		privKey.Delete();
#if defined(VERBOSE_SVR) || defined(VERBOSE_CLI)
		printf("WinSSLEngine_ImportKey: Import RSA Private Key failed\r\n");
#endif
		return 0;
	}
	else if (keyType == Crypto::Cert::X509File::KeyType::ECDSA && Crypto::Cert::X509PrivKey::CreateFromKey(key).SetTo(privKey))
	{
		if (WinSSLEngine_CryptImportECDSAPrivateKey(&hKey, hProv, privKey->GetASN1Buff().Ptr(), (ULONG)privKey->GetASN1BuffSize(), signature))
		{
			privKey.Delete();
			return hKey;
		}
		privKey.Delete();
#if defined(VERBOSE_SVR) || defined(VERBOSE_CLI)
		printf("WinSSLEngine_ImportKey: Import ECDSA Private Key failed\r\n");
#endif
		return 0;
	}
	else if (privateKeyOnly)
	{
		return 0;
	}
	else if (keyType == Crypto::Cert::X509File::KeyType::RSAPublic)
	{
		NN<Crypto::Cert::X509PubKey> pubKey = Crypto::Cert::X509PubKey::CreateFromKey(key);
		if (WinSSLEngine_CryptImportPublicKey(&hKey, hProv, pubKey->GetASN1Buff().Ptr(), (ULONG)pubKey->GetASN1BuffSize(), true))
		{
			pubKey.Delete();
			return hKey;
		}
		pubKey.Delete();
#if defined(VERBOSE_SVR) || defined(VERBOSE_CLI)
		printf("SSL: Import Key failed\r\n");
#endif
		return 0;
	}
	return 0;
}


HCRYPTKEY WinSSLEngine_ImportPrivKey(HCRYPTPROV hProv, NN<Crypto::Cert::X509PrivKey> key, Bool signature)
{
	HCRYPTKEY hKey;
	Crypto::Cert::X509File::KeyType keyType = key->GetKeyType();
	if (keyType == Crypto::Cert::X509File::KeyType::RSA)
	{
		if (WinSSLEngine_CryptImportRSAPrivateKey(&hKey, hProv, key->GetASN1Buff().Ptr(), (ULONG)key->GetASN1BuffSize(), signature))
		{
			return hKey;
		}
		return 0;
	}
	return 0;
}

Bool WinSSLEngine_InitKey(HCRYPTPROV *hProvOut, HCRYPTKEY *hKeyOut, NN<Crypto::Cert::X509File> keyASN1, const WChar *containerName, Bool signature, CRYPT_KEY_PROV_INFO *keyProvInfo)
{
	HCRYPTPROV hProv;
	HCRYPTKEY hKey;
	if (keyASN1->GetFileType() == Crypto::Cert::X509File::FileType::Key)
	{
		NN<Crypto::Cert::X509Key> key = NN<Crypto::Cert::X509Key>::ConvertFrom(keyASN1);
		hProv = WinSSLEngine_CreateProv(key->GetKeyType(), containerName, keyProvInfo);
		if (hProv == 0)
		{
			return false;
		}
		hKey = WinSSLEngine_ImportKey(hProv, key, true, signature);
		if (hKey == 0)
		{
			CryptReleaseContext(hProv, 0);
			return false;
		}
	}
	else if (keyASN1->GetFileType() == Crypto::Cert::X509File::FileType::PrivateKey)
	{
		NN<Crypto::Cert::X509PrivKey> key = NN<Crypto::Cert::X509PrivKey>::ConvertFrom(keyASN1);
		hProv = WinSSLEngine_CreateProv(key->GetKeyType(), containerName, keyProvInfo);
		if (hProv == 0)
		{
			return false;
		}
		hKey = WinSSLEngine_ImportPrivKey(hProv, key, signature);
		if (hKey == 0)
		{
			CryptReleaseContext(hProv, 0);
			return false;
		}
	}
	else
	{
		return false;
	}
	*hProvOut = hProv;
	*hKeyOut = hKey;
	return true;
}

BCRYPT_ALG_HANDLE WinSSLEngine_BCryptOpenHash(Crypto::Hash::HashType hashType)
{
	const WChar *hashAlg = 0;
	if (hashType == Crypto::Hash::HashType::SHA256)
	{
		hashAlg = BCRYPT_SHA256_ALGORITHM;
	}
	else if (hashType == Crypto::Hash::HashType::SHA384)
	{
		hashAlg = BCRYPT_SHA384_ALGORITHM;
	}
	else if (hashType == Crypto::Hash::HashType::SHA512)
	{
		hashAlg = BCRYPT_SHA512_ALGORITHM;
	}
	else if (hashType == Crypto::Hash::HashType::SHA1)
	{
		hashAlg = BCRYPT_SHA1_ALGORITHM;
	}
	else if (hashType == Crypto::Hash::HashType::MD5)
	{
		hashAlg = BCRYPT_MD5_ALGORITHM;
	}
	else
	{
#if defined(VERBOSE_SVR) || defined(VERBOSE_CLI)
		printf("SSL: hashType not supported\r\n");
#endif
		return 0;
	}
	BCRYPT_ALG_HANDLE hAlg = 0;
	NTSTATUS status;
	if ((status = BCryptOpenAlgorithmProvider(&hAlg, hashAlg, NULL, 0)) == 0)
	{
		return hAlg;
	}
#if defined(VERBOSE_SVR) || defined(VERBOSE_CLI)
	printf("SSL: BCryptOpenAlgorithmProvider Hash failed: 0x%x\r\n", (UInt32)status);
#endif
	return 0;
}

const WChar *WinSSLEngine_BCryptGetECDSAAlg(Crypto::Cert::X509File::ECName ecName)
{
	switch (ecName)
	{
	case Crypto::Cert::X509File::ECName::secp256r1:
		return BCRYPT_ECDSA_P256_ALGORITHM;
	case Crypto::Cert::X509File::ECName::secp384r1:
		return BCRYPT_ECDSA_P384_ALGORITHM;
	case Crypto::Cert::X509File::ECName::secp521r1:
		return BCRYPT_ECDSA_P521_ALGORITHM;
	case Crypto::Cert::X509File::ECName::Unknown:
	default:
#if defined(VERBOSE_SVR) || defined(VERBOSE_CLI)
		printf("WinSSLEngine_BCryptGetECDSAAlg: ECName not supported\r\n");
#endif
		return 0;
	}

}
BCRYPT_ALG_HANDLE WinSSLEngine_BCryptOpenECDSA(Crypto::Cert::X509File::ECName ecName)
{
	const WChar *hashAlg = WinSSLEngine_BCryptGetECDSAAlg(ecName);
	if (hashAlg == 0)
	{
		return 0;
	}
	BCRYPT_ALG_HANDLE hAlg = 0;
	NTSTATUS status;
	if ((status = BCryptOpenAlgorithmProvider(&hAlg, hashAlg, NULL, 0)) == 0)
	{
		return hAlg;
	}
#if defined(VERBOSE_SVR) || defined(VERBOSE_CLI)
	printf("SSL: BCryptOpenAlgorithmProvider ECDSA failed: 0x%x\r\n", (UInt32)status);
#endif
	return 0;
}

Bool WinSSLEngine_BCryptImportKey(NN<Crypto::Cert::X509Key> key, OutParam<BCRYPT_ALG_HANDLE> hAlg, OutParam<BCRYPT_KEY_HANDLE> hKey)
{
	UOSInt privLen;
	UnsafeArray<const UInt8> privBuff;
	UOSInt pubLen;
	UnsafeArray<const UInt8> pubBuff;
	if (!key->GetECPrivate(privLen).SetTo(privBuff) || !key->GetECPublic(pubLen).SetTo(pubBuff))
	{
#if defined(VERBOSE_SVR) || defined(VERBOSE_CLI)
		printf("WinSSLEngine_BCryptImportKey: Key is not ECDSA\r\n");
#endif
		return false;
	}
	BCRYPT_ALG_HANDLE hSignAlg = WinSSLEngine_BCryptOpenECDSA(key->GetECName());
	if (hSignAlg == 0)
	{
#if defined(VERBOSE_SVR) || defined(VERBOSE_CLI)
		printf("WinSSLEngine_BCryptImportKey: BCryptOpenECDSA failed, ecName = %s\r\n", Crypto::Cert::X509File::ECNameGetName(key->GetECName()).v.Ptr());
#endif
		return false;
	}
	Crypto::Cert::X509File::ECName name = key->GetECName();
	UInt8 buff[512];
	if (name == Crypto::Cert::X509File::ECName::secp256r1)
	{
		WriteUInt32(&buff[0], BCRYPT_ECDSA_PRIVATE_P256_MAGIC);
	}
	else if (name == Crypto::Cert::X509File::ECName::secp384r1)
	{
		WriteUInt32(&buff[0], BCRYPT_ECDSA_PRIVATE_P384_MAGIC);
	}
	else
	{
		WriteUInt32(&buff[0], BCRYPT_ECDSA_PRIVATE_P521_MAGIC);
	}
	WriteUInt32(&buff[4], (UInt32)privLen);
	MemCopyNO(&buff[8], &pubBuff[1], pubLen - 1);
	MemCopyNO(&buff[8 + pubLen - 1], privBuff.Ptr(), privLen);

	BCRYPT_KEY_HANDLE hRetKey;
	NTSTATUS status = BCryptImportKeyPair(hSignAlg, 0, BCRYPT_ECCPRIVATE_BLOB, &hRetKey, buff, (ULONG)(8 + privLen + pubLen), 0);
	if (status != 0)
	{
#if defined(VERBOSE_SVR) || defined(VERBOSE_CLI)
		printf("WinSSLEngine_BCryptImportKey: BCryptImportKeyPair failed, status = 0x%x\r\n", (UInt32)status);
#endif
		BCryptCloseAlgorithmProvider(hSignAlg, 0);
		return false;
	}
	hAlg.Set(hSignAlg);
	hKey.Set(hRetKey);
	return true;
}

Bool WinSSLEngine_NCryptInitKey(NCRYPT_PROV_HANDLE *hProvOut, NCRYPT_KEY_HANDLE *hKeyOut, NN<Crypto::Cert::X509Key> key, Bool privateKeyOnly)
{
	const WChar *algName = WinSSLEngine_BCryptGetECDSAAlg(key->GetECName());
	if (algName == 0)
	{
		return false;
	}
	NTSTATUS status;
	NCRYPT_PROV_HANDLE hProv;
	NCRYPT_KEY_HANDLE hKey;
	NN<Crypto::Cert::X509PrivKey> privKey;
	if((status = NCryptOpenStorageProvider(&hProv, MS_KEY_STORAGE_PROVIDER, 0)) != 0)
	{
#if defined(VERBOSE_SVR) || defined(VERBOSE_CLI)
		printf("WinSSLEngine_NCryptInitKey: NCryptOpenStorageProvider failed: 0x%x\r\n", (UInt32)status);
#endif
		return false;
	}
	if (!Crypto::Cert::X509PrivKey::CreateFromKey(key).SetTo(privKey))
	{
		NCryptFreeObject(hProv);
		return false;
	}
	if ((status = NCryptImportKey(hProv, 0, algName, 0, &hKey, (PBYTE)privKey->GetASN1Buff().Ptr(), (DWORD)privKey->GetASN1BuffSize(), 0)) != 0)
    {
#if defined(VERBOSE_SVR) || defined(VERBOSE_CLI)
		printf("WinSSLEngine_NCryptInitKey: NCryptImportKey failed: algName = %ls, 0x%x\r\n", algName, (UInt32)status);
#endif
		privKey.Delete();
		NCryptFreeObject(hProv);
		return false;
    }
	privKey.Delete();
	*hProvOut = hProv;
	*hKeyOut = hKey;
	return true;
}

void Net::WinSSLEngine::DeinitClient()
{
	if (this->clsData->cliInit)
	{
		FreeCredentialsHandle(&this->clsData->hCredCli);
		this->clsData->cliInit = false;
	}
}
Bool Net::WinSSLEngine::InitClient(Method method, void *cred)
{
	SCHANNEL_CRED credData;
	SECURITY_STATUS status;
	TimeStamp lifetime;
	MemClear(&credData, sizeof(credData));
	credData.dwVersion = SCHANNEL_CRED_VERSION;
	credData.grbitEnabledProtocols = WinSSLEngine_GetProtocols(method, false);
	if (cred)
	{
		credData.paCred = (PCCERT_CONTEXT*)&cred;
		credData.cCreds = 1;
	}

	status = AcquireCredentialsHandleW(
		NULL,
		(LPWSTR)UNISP_NAME,
		SECPKG_CRED_OUTBOUND,
		NULL,
		&credData,
		NULL,
		NULL,
		&this->clsData->hCredCli,
		&lifetime
	);
	return status == 0;
}

Bool Net::WinSSLEngine::InitServer(Method method, void *cred, void *hRootStore)
{
#if 0//defined(SCH_CREDENTIALS_VERSION)
	SCH_CREDENTIALS credData;
	MemClear(&credData, sizeof(credData));
	credData.dwVersion = SCH_CREDENTIALS_VERSION;
	credData.hRootStore = (HCERTSTORE)hRootStore;
	credData.paCred = (PCCERT_CONTEXT*)creds;
	credData.cCreds = (DWORD)ncred;
#else
	SCHANNEL_CRED credData;
	MemClear(&credData, sizeof(credData));
	credData.dwVersion = SCHANNEL_CRED_VERSION;
	credData.grbitEnabledProtocols = 0;// WinSSLEngine_GetProtocols(method, true);
	credData.hRootStore = (HCERTSTORE)hRootStore;
	credData.paCred = (PCCERT_CONTEXT*)cred;
	credData.cCreds = (DWORD)1;
#endif
	SECURITY_STATUS status;
	TimeStamp lifetime;

	status = AcquireCredentialsHandleW(
		NULL,
		(LPWSTR)UNISP_NAME,
		SECPKG_CRED_INBOUND,
		NULL,
		&credData,
		NULL,
		NULL,
		&this->clsData->hCredSvr,
		&lifetime
	);
	if (status == SEC_E_INTERNAL_ERROR)
	{

	}
#if defined(VERBOSE_SVR)
	printf("WinSSLEngine: AcquireCredentialsHandleW error: 0x%x (%s)\r\n", status, IO::WindowsError::GetString(status).v.Ptr());
#endif
	return status == 0;
}

Optional<Net::SSLClient> Net::WinSSLEngine::CreateClientConn(void* sslObj, NN<Socket> s, Text::CStringNN hostName, OptOut<ErrorType> err)
{
	CtxtHandle ctxt;
	UnsafeArray<const WChar> wptr = Text::StrToWCharNew(hostName.v);
	UInt32 retFlags = ISC_REQ_SEQUENCE_DETECT | ISC_REQ_REPLAY_DETECT | ISC_REQ_CONFIDENTIALITY | ISC_REQ_ALLOCATE_MEMORY | ISC_REQ_STREAM;
	TimeStamp ts;
	SecBuffer outputBuff[3];
	SecBuffer_Set(&outputBuff[0], SECBUFFER_EMPTY, 0, 0);
	SecBufferDesc inputDesc;
	SecBufferDesc outputDesc;
	SecBufferDesc_Set(&outputDesc, outputBuff, 1);

	if (this->skipCertCheck)
	{
		retFlags |= ISC_REQ_MANUAL_CRED_VALIDATION;
	}

	SECURITY_STATUS status;
	status = InitializeSecurityContextW(
		&this->clsData->hCredCli,
		0,
		(WChar*)wptr.Ptr(),
		(unsigned long)retFlags,
		0,
		0,
		0,
		0,
		&ctxt,
		&outputDesc,
		(FLAGS_ULONG*)&retFlags,
		&ts
	);
	if (status != SEC_I_CONTINUE_NEEDED)
	{
#if defined(VERBOSE_CLI)
		printf("SSL: Cli %x, Error in InitializeSecurityContext, ret = %x\r\n", (Int32)(OSInt)s.Ptr(), (UInt32)status);
#endif
		Text::StrDelNew(wptr);
		err.Set(ErrorType::InitSession);
		return 0;
	}
	Net::SocketFactory::ErrorType et;
#if defined(VERBOSE_CLI)
	printf("SSL: Cli %x, SendData, size = %d\r\n", (Int32)(OSInt)s.Ptr(), (Int32)outputBuff[0].cbBuffer);
#endif
	if (this->clif->GetSocketFactory()->SendData(s, (UInt8*)outputBuff[0].pvBuffer, outputBuff[0].cbBuffer, et) != outputBuff[0].cbBuffer)
	{
#if defined(VERBOSE_CLI)
		printf("SSL: Cli %x, Error in sendData, ret = %x\r\n", (Int32)(OSInt)s.Ptr(), (UInt32)status);
#endif
		DeleteSecurityContext(&ctxt);
		FreeContextBuffer(outputBuff[0].pvBuffer);
		Text::StrDelNew(wptr);
		err.Set(ErrorType::InitSession);
		return 0;
	}
	FreeContextBuffer(outputBuff[0].pvBuffer);

	this->clif->GetSocketFactory()->SetRecvTimeout(s, 3000);
	SecBuffer inputBuff[2];
	UInt8 recvBuff[8192];
	UOSInt recvOfst = 0;
	UOSInt recvSize;
	UOSInt i;
	while (status == SEC_I_CONTINUE_NEEDED || status == SEC_E_INCOMPLETE_MESSAGE)
	{
		if (recvOfst == 0 || status == SEC_E_INCOMPLETE_MESSAGE)
		{
			recvSize = this->clif->GetSocketFactory()->ReceiveData(s, &recvBuff[recvOfst], 8192 - recvOfst, et);
#if defined(VERBOSE_CLI)
			printf("SSL: Cli %x, recvData, size = %d\r\n", (Int32)(OSInt)s.Ptr(), (UInt32)recvSize);
#endif
			if (recvSize <= 0)
			{
				Text::StrDelNew(wptr);
				err.Set(ErrorType::InitSession);
				return 0;
			}
			recvOfst += recvSize;
		}

		SecBuffer_Set(&inputBuff[0], SECBUFFER_TOKEN, recvBuff, (UInt32)recvOfst);
		SecBuffer_Set(&inputBuff[1], SECBUFFER_EMPTY, 0, 0);
		SecBufferDesc_Set(&inputDesc, inputBuff, 2);

		SecBuffer_Set(&outputBuff[0], SECBUFFER_TOKEN, 0, 0);
#if defined(SECBUFFER_ALERT)
		SecBuffer_Set(&outputBuff[1], SECBUFFER_ALERT, 0, 0);
#else
		SecBuffer_Set(&outputBuff[1], SECBUFFER_EMPTY, 0, 0);
#endif
		SecBuffer_Set(&outputBuff[2], SECBUFFER_EMPTY, 0, 0);
		SecBufferDesc_Set(&outputDesc, outputBuff, 3);

		status = InitializeSecurityContextW(
			&this->clsData->hCredCli,
			&ctxt,
			(WChar*)wptr.Ptr(),
			retFlags,
			0,
			0,
			&inputDesc,
			0,
			0,
			&outputDesc,
			(FLAGS_ULONG*)&retFlags,
			&ts);

		if (status == SEC_E_INCOMPLETE_MESSAGE)
		{

		}
		else if (status == SEC_I_CONTINUE_NEEDED || status == SEC_E_OK)
		{
			Bool succ = true;
			i = 0;
			while (i < 3)
			{
				if (outputBuff[i].BufferType == SECBUFFER_TOKEN && outputBuff[i].cbBuffer > 0)
				{
					if (this->clif->GetSocketFactory()->SendData(s, (const UInt8*)outputBuff[i].pvBuffer, outputBuff[i].cbBuffer, et) != outputBuff[i].cbBuffer)
					{
						succ = false;
					}
				}

				if (outputBuff[i].pvBuffer)
				{
					FreeContextBuffer(outputBuff[i].pvBuffer);
				}
				i++;
			}
			if (!succ)
			{
				DeleteSecurityContext(&ctxt);
				Text::StrDelNew(wptr);
				err.Set(ErrorType::InitSession);
				return 0;
			}
			if (inputBuff[1].BufferType == SECBUFFER_EXTRA)
			{
				MemCopyO(recvBuff, &recvBuff[recvOfst - inputBuff[1].cbBuffer], inputBuff[1].cbBuffer);
				recvOfst = inputBuff[1].cbBuffer;
			}
			else
			{
				recvOfst = 0;
			}
		}
		else
		{
#if defined(VERBOSE_CLI)
			printf("SSL: Cli %x, Error in InitializeSecurityContext 2, ret = %x\r\n", (Int32)(OSInt)s.Ptr(), (UInt32)status);
#endif
			if (status == SEC_I_INCOMPLETE_CREDENTIALS)
			{

			}
			DeleteSecurityContext(&ctxt);
			Text::StrDelNew(wptr);
			err.Set(ErrorType::InitSession);
			return 0;
		}
	}
	Text::StrDelNew(wptr);

	clif->GetSocketFactory()->SetRecvTimeout(s, 120000);
	Net::SSLClient *cli;
	NEW_CLASS(cli, Net::WinSSLClient(clif->GetSocketFactory(), s, &ctxt));
	return cli;
}

Optional<Net::SSLClient> Net::WinSSLEngine::CreateServerConn(NN<Socket> s)
{
	if (!this->clsData->svrInit)
	{
#if defined(VERBOSE_SVR)
		printf("SSL: Server not init\r\n");
#endif
		this->clif->GetSocketFactory()->DestroySocket(s);
		return 0;
	}

#if defined(VERBOSE_SVR)
	UTF8Char debugBuff[64];;
	Data::DateTime dtDebug;
	dtDebug.SetCurrTime();
	dtDebug.ToString(debugBuff, "HH:mm:ss.fff");
	printf("%s SSL: Svr %x, Init begin, Tid = %d\r\n", debugBuff, (Int32)(OSInt)s.Ptr(), (UInt32)GetCurrentThreadId());
#endif

	this->clif->GetSocketFactory()->SetRecvTimeout(s, 3000);
	CtxtHandle ctxt;
	SecBuffer inputBuff[2];
	SecBuffer outputBuff[3];
	SecBufferDesc inputDesc;
	SecBufferDesc outputDesc;
	UInt8 recvBuff[2048];
	unsigned long retFlags = ASC_REQ_ALLOCATE_MEMORY | ASC_REQ_STREAM;
	UOSInt recvOfst = 0;
	UOSInt recvSize;
	TimeStamp ts;
	Net::SocketFactory::ErrorType et;
	recvSize = this->clif->GetSocketFactory()->ReceiveData(s, recvBuff, 2048, et);
	if (recvSize == 0)
	{
#if defined(VERBOSE_SVR)
		dtDebug.SetCurrTime();
		dtDebug.ToString(debugBuff, "HH:mm:ss.fff");
		printf("%s SSL: Svr %x, Recv size 0\r\n", debugBuff, (Int32)(OSInt)s.Ptr());
#endif
		this->clif->GetSocketFactory()->DestroySocket(s);
		return 0;
	}

	SecBuffer_Set(&inputBuff[0], SECBUFFER_TOKEN, recvBuff, (UInt32)recvSize);
	SecBuffer_Set(&inputBuff[1], SECBUFFER_EMPTY, 0, 0);
	SecBufferDesc_Set(&inputDesc, inputBuff, 2);

	SecBuffer_Set(&outputBuff[0], SECBUFFER_TOKEN, 0, 0);
#if defined(SECBUFFER_ALERT)
	SecBuffer_Set(&outputBuff[1], SECBUFFER_ALERT, 0, 0);
#else
	SecBuffer_Set(&outputBuff[1], SECBUFFER_EMPTY, 0, 0);
#endif
	SecBuffer_Set(&outputBuff[2], SECBUFFER_EMPTY, 0, 0);
	SecBufferDesc_Set(&outputDesc, outputBuff, 3);

/*	IO::DebugWriter debug;
	Text::StringBuilderW sb;
	sb.ClearStr();
	sb.AppendC(UTF8STRC("Received "));
	sb.AppendUOSInt(recvSize);
	sb.AppendC(UTF8STRC(" bytes"));
	debug.WriteLineW(sb.ToString());*/

	Bool succ = true;
	UOSInt i;
	SECURITY_STATUS status;
	status = AcceptSecurityContext(
		&this->clsData->hCredSvr,
		0,
		&inputDesc,
		retFlags,
		0,
		&ctxt,
		&outputDesc,
		(FLAGS_ULONG*)&retFlags,
		&ts
	);
	if (status != SEC_I_CONTINUE_NEEDED)
	{
#if defined(VERBOSE_SVR)
		dtDebug.SetCurrTime();
		dtDebug.ToString(debugBuff, "HH:mm:ss.fff");
		printf("%s SSL: Svr %x, AcceptSecurityContext error, status %x\r\n", debugBuff, (Int32)(OSInt)s.Ptr(), (UInt32)status);
#endif
		this->clif->GetSocketFactory()->DestroySocket(s);
		return 0;
	}
	recvOfst = 0;
	i = 0;
	while (i < 3)
	{
		if (outputBuff[i].BufferType == SECBUFFER_TOKEN && outputBuff[i].cbBuffer > 0)
		{
			if (this->clif->GetSocketFactory()->SendData(s, (const UInt8*)outputBuff[i].pvBuffer, outputBuff[i].cbBuffer, et) != outputBuff[i].cbBuffer)
			{
#if defined(VERBOSE_SVR)
				dtDebug.SetCurrTime();
				dtDebug.ToString(debugBuff, "HH:mm:ss.fff");
				printf("%s SSL: Svr %x, Send data error\r\n", debugBuff, (Int32)(OSInt)s.Ptr());
#endif
				succ = false;
			}
		}

		if (inputBuff[1].BufferType == SECBUFFER_EXTRA)
		{
			MemCopyO(recvBuff, inputBuff[1].pvBuffer, inputBuff[1].cbBuffer);
			recvOfst = inputBuff[1].cbBuffer;
		}
		if (outputBuff[i].pvBuffer)
		{
			FreeContextBuffer(outputBuff[i].pvBuffer);
		}
		i++;
	}
	if (!succ)
	{
		DeleteSecurityContext(&ctxt);
		this->clif->GetSocketFactory()->DestroySocket(s);
		return 0;
	}

	while (status == SEC_I_CONTINUE_NEEDED || status == SEC_E_INCOMPLETE_MESSAGE)
	{
		if (recvOfst == 0 || status == SEC_E_INCOMPLETE_MESSAGE)
		{
			recvSize = this->clif->GetSocketFactory()->ReceiveData(s, &recvBuff[recvOfst], 2048 - recvOfst, et);
			if (recvSize <= 0)
			{
#if defined(VERBOSE_SVR)
				dtDebug.SetCurrTime();
				dtDebug.ToString(debugBuff, "HH:mm:ss.fff");
				printf("%s SSL: Svr %x, Recv size2 0\r\n", debugBuff, (Int32)(OSInt)s.Ptr());
#endif
				DeleteSecurityContext(&ctxt);
				this->clif->GetSocketFactory()->DestroySocket(s);
				return 0;
			}
			recvOfst += recvSize;

/*			sb.ClearStr();
			sb.AppendC(UTF8STRC("Received "));
			sb.AppendUOSInt(recvSize);
			sb.AppendC(UTF8STRC(" bytes"));
			debug.WriteLineW(sb.ToString());*/
		}
		SecBuffer_Set(&inputBuff[0], SECBUFFER_TOKEN, recvBuff, (UInt32)recvOfst);
		SecBuffer_Set(&inputBuff[1], SECBUFFER_EMPTY, 0, 0);
		SecBufferDesc_Set(&inputDesc, inputBuff, 2);

		SecBuffer_Set(&outputBuff[0], SECBUFFER_TOKEN, 0, 0);
#if defined(SECBUFFER_ALERT)
		SecBuffer_Set(&outputBuff[1], SECBUFFER_ALERT, 0, 0);
#else
		SecBuffer_Set(&outputBuff[1], SECBUFFER_EMPTY, 0, 0);
#endif
		SecBuffer_Set(&outputBuff[2], SECBUFFER_EMPTY, 0, 0);
		SecBufferDesc_Set(&outputDesc, outputBuff, 3);

		status = AcceptSecurityContext(
			&this->clsData->hCredSvr,
			&ctxt,
			&inputDesc,
			retFlags,
			0,
			&ctxt,
			&outputDesc,
			(FLAGS_ULONG*)&retFlags,
			&ts
		);

		if (status == SEC_E_INCOMPLETE_MESSAGE)
		{

		}
		else if (status == SEC_I_CONTINUE_NEEDED || status == SEC_E_OK)
		{
			succ = true;
			recvOfst = 0;
			i = 0;
			while (i < 3)
			{
				if (outputBuff[i].BufferType == SECBUFFER_TOKEN && outputBuff[i].cbBuffer > 0)
				{
					if (this->clif->GetSocketFactory()->SendData(s, (const UInt8*)outputBuff[i].pvBuffer, outputBuff[i].cbBuffer, et) != outputBuff[i].cbBuffer)
					{
#if defined(VERBOSE_SVR)
						dtDebug.SetCurrTime();
						dtDebug.ToString(debugBuff, "HH:mm:ss.fff");
						printf("%s SSL: Svr %x, Send data error2\r\n", debugBuff, (Int32)(OSInt)s.Ptr());
#endif
						succ = false;
					}
				}

				if (inputBuff[1].BufferType == SECBUFFER_EXTRA && inputBuff[1].pvBuffer)
				{
					MemCopyO(recvBuff, inputBuff[1].pvBuffer, inputBuff[1].cbBuffer);
					recvOfst = inputBuff[1].cbBuffer;
				}
				if (outputBuff[i].pvBuffer)
				{
					FreeContextBuffer(outputBuff[i].pvBuffer);
				}
				i++;
			}
			if (!succ)
			{
				DeleteSecurityContext(&ctxt);
				this->clif->GetSocketFactory()->DestroySocket(s);
				return 0;
			}
		}
		else
		{
/*			sb.ClearStr();
			sb.AppendC(UTF8STRC("Error in initializing SSL Server connection: 0x"));
			sb.AppendHex32(status);
			debug.WriteLineW(sb.ToString());*/
			if (status == SEC_I_INCOMPLETE_CREDENTIALS)
			{

			}
#if defined(VERBOSE_SVR)
			dtDebug.SetCurrTime();
			dtDebug.ToString(debugBuff, "HH:mm:ss.fff");
			printf("%s SSL: Svr %x, AcceptSecurityContext error2, status %s (%x)\r\n", debugBuff, (Int32)(OSInt)s.Ptr(), IO::WindowsError::GetString(status).v.Ptr(), (UInt32)status);
#endif
			DeleteSecurityContext(&ctxt);
			this->clif->GetSocketFactory()->DestroySocket(s);
			return 0;
		}
	}

#if defined(VERBOSE_SVR)
	dtDebug.SetCurrTime();
	dtDebug.ToString(debugBuff, "HH:mm:ss.fff");
	printf("%s SSL: Svr %x, Success, extra size = %d\r\n", debugBuff, (Int32)(OSInt)s.Ptr(), (UInt32)recvOfst);
#endif

	Net::SSLClient *cli;
	NEW_CLASS(cli, Net::WinSSLClient(clif->GetSocketFactory(), s, &ctxt));
	return cli;
}

Net::WinSSLEngine::WinSSLEngine(NN<Net::TCPClientFactory> clif, Method method) : Net::SSLEngine(clif)
{
	this->clsData = MemAlloc(ClassData, 1);
	MemClear(this->clsData, sizeof(ClassData));
	this->clsData->method = method;

	this->skipCertCheck = false;
	this->cliCert = ClientCertType::None;
}

Net::WinSSLEngine::~WinSSLEngine()
{
	if (this->clsData->cliInit)
	{
		FreeCredentialsHandle(&this->clsData->hCredCli);
	}
	if (this->clsData->svrInit)
	{
		FreeCredentialsHandle(&this->clsData->hCredSvr);
	}
	MemFree(this->clsData);
}

Bool Net::WinSSLEngine::IsError()
{
	return false;
}

Bool Net::WinSSLEngine::ServerSetCertsASN1(NN<Crypto::Cert::X509Cert> certASN1, NN<Crypto::Cert::X509File> keyASN1, NN<Data::ArrayListNN<Crypto::Cert::X509Cert>> cacerts)
{
	if (this->clsData->svrInit)
	{
		return false;
	}
//	const WChar *containerName = L"SelfSign";
	const WChar *containerName = L"ServerCert";
	HCRYPTKEY hKey;
	HCRYPTPROV hProv;
	CRYPT_KEY_PROV_INFO keyProvInfo;
	MemClear(&keyProvInfo, sizeof(keyProvInfo));
	if (!WinSSLEngine_InitKey(&hProv, &hKey, keyASN1, containerName, false, &keyProvInfo))
	{
		return false;
	}

	PCCERT_CONTEXT svrCert;
	svrCert = CertCreateCertificateContext(X509_ASN_ENCODING, certASN1->GetASN1Buff().Ptr(), (DWORD)certASN1->GetASN1BuffSize());
	keyProvInfo.cProvParam = 0;
	keyProvInfo.rgProvParam = NULL;
	keyProvInfo.dwKeySpec = AT_KEYEXCHANGE;
	if (!CertSetCertificateContextProperty(svrCert, CERT_KEY_PROV_INFO_PROP_ID, 0, &keyProvInfo))
	{
#if defined(VERBOSE_SVR)
		printf("WinSSLEngine: CertSetCertificateContextProperty error: 0x%x\r\n", GetLastError());
#endif
	}

	HCRYPTPROV_OR_NCRYPT_KEY_HANDLE hCryptProvOrNCryptKey = 0;
	BOOL fCallerFreeProvOrNCryptKey = FALSE;
	DWORD dwKeySpec;
	if (!CryptAcquireCertificatePrivateKey(svrCert, 0, NULL, &hCryptProvOrNCryptKey, &dwKeySpec, &fCallerFreeProvOrNCryptKey))
	{
#if defined(VERBOSE_SVR)
		printf("WinSSLEngine: CryptAcquireCertificatePrivateKey error: 0x%x\r\n", GetLastError());
#endif
	}
	WinSSLEngine_PrintCERT_CONTEXT(svrCert);

	HCERTSTORE certStore = 0;
	if (cacerts->GetCount() > 0)
	{
		certStore = CertOpenStore(CERT_STORE_PROV_MEMORY, 0, 0, 0, 0);
		Data::ArrayIterator<NN<Crypto::Cert::X509Cert>> it = cacerts->Iterator();
		while (it.HasNext())
		{
			NN<Crypto::Cert::X509Cert> caCert = it.Next();
			CertAddCertificateContextToStore(certStore, CertCreateCertificateContext(X509_ASN_ENCODING, caCert->GetASN1Buff().Ptr(), (DWORD)caCert->GetASN1BuffSize()), CERT_STORE_ADD_NEW, 0);
		}
	}
	if (!this->InitServer(this->clsData->method, &svrCert, certStore))
	{
		if (certStore) CertCloseStore(certStore, 0);
		CertFreeCertificateContext(svrCert);
		CryptDestroyKey(hKey);
		CryptReleaseContext(hProv, 0);
		return false;
	}
	this->clsData->svrInit = true;
	if (certStore) CertCloseStore(certStore, 0);
	CertFreeCertificateContext(svrCert);
	CryptDestroyKey(hKey);
	CryptReleaseContext(hProv, 0);
	return true;
}

Bool Net::WinSSLEngine::ServerSetRequireClientCert(ClientCertType cliCert)
{
	this->cliCert = cliCert;
	return true;
}

Bool Net::WinSSLEngine::ServerSetClientCA(Text::CStringNN clientCA)
{
	return false;
}

Bool Net::WinSSLEngine::ServerAddALPNSupport(Text::CStringNN proto)
{
	return false;
}

void Net::WinSSLEngine::ClientSetSkipCertCheck(Bool skipCertCheck)
{
	this->skipCertCheck = skipCertCheck;
}

Bool Net::WinSSLEngine::ClientSetCertASN1(NN<Crypto::Cert::X509Cert> certASN1, NN<Crypto::Cert::X509File> keyASN1)
{
	const WChar *containerName = L"ClientCert";
	HCRYPTKEY hKey;
	HCRYPTPROV hProv;
	CRYPT_KEY_PROV_INFO keyProvInfo;
	MemClear(&keyProvInfo, sizeof(keyProvInfo));
	if (!WinSSLEngine_InitKey(&hProv, &hKey, keyASN1, containerName, false, &keyProvInfo))
	{
		return false;
	}

/*	IO::DebugWriter debug;
	Text::StringBuilderUTF16 sbDebug;
	WinSSLEngine_HCRYPTPROV_ToString(hProv, &sbDebug);
	debug.WriteLineW(sbDebug.ToString());*/

	PCCERT_CONTEXT serverCert = CertCreateCertificateContext(X509_ASN_ENCODING, certASN1->GetASN1Buff().Ptr(), (DWORD)certASN1->GetASN1BuffSize());
	keyProvInfo.cProvParam = 0;
	keyProvInfo.rgProvParam = NULL;
	keyProvInfo.dwKeySpec = AT_KEYEXCHANGE;
	CertSetCertificateContextProperty(serverCert, CERT_KEY_PROV_INFO_PROP_ID, 0, &keyProvInfo);

	HCRYPTPROV_OR_NCRYPT_KEY_HANDLE hCryptProvOrNCryptKey = 0;
	BOOL fCallerFreeProvOrNCryptKey = FALSE;
	DWORD dwKeySpec;
	CryptAcquireCertificatePrivateKey(serverCert, 0, NULL, &hCryptProvOrNCryptKey, &dwKeySpec, &fCallerFreeProvOrNCryptKey);

	this->DeinitClient();
	if (!this->InitClient(this->clsData->method, (void*)serverCert) &&
			!this->InitClient(Method::TLSV1_2, (void*)serverCert) &&
			!this->InitClient(Method::TLSV1_1, (void*)serverCert) &&
			!this->InitClient(Method::TLSV1, (void*)serverCert))
	{
		CertFreeCertificateContext(serverCert);
		CryptDestroyKey(hKey);
		CryptReleaseContext(hProv, 0);
		return false;
	}
	this->clsData->cliInit = true;
	CertFreeCertificateContext(serverCert);
	CryptDestroyKey(hKey);
	CryptReleaseContext(hProv, 0);
	return true;
}

Optional<Net::SSLClient> Net::WinSSLEngine::ClientConnect(Text::CStringNN hostName, UInt16 port, OptOut<ErrorType> err, Data::Duration timeout)
{
	if (!this->clsData->cliInit)
	{
		if (!this->InitClient(this->clsData->method, 0) &&
			!this->InitClient(Method::TLSV1_2, 0) &&
			!this->InitClient(Method::TLSV1_1, 0) &&
			!this->InitClient(Method::TLSV1, 0))
		{
			err.Set(ErrorType::InitEnv);
			return 0;
		}
		this->clsData->cliInit = true;
	}

	Net::SocketUtil::AddressInfo addr[1];
	UOSInt addrCnt = this->clif->GetSocketFactory()->DNSResolveIPs(hostName, Data::DataArray<Net::SocketUtil::AddressInfo>(addr, 1));
	NN<Socket> s;
	if (addrCnt == 0)
	{
		err.Set(ErrorType::HostnameNotResolved);
		return 0;
	}
	NN<Net::TCPClient> cli = this->clif->Create(hostName, port, timeout);
	if (cli->RemoveSocket().SetTo(s))
	{
		cli.Delete();
		return CreateClientConn(0, s, hostName, err);
	}
	cli.Delete();
	err.Set(ErrorType::CannotConnect);
	return 0;
}

Optional<Net::SSLClient> Net::WinSSLEngine::ClientInit(NN<Socket> s, Text::CStringNN hostName, OptOut<ErrorType> err)
{
	if (!this->clsData->cliInit)
	{
		if (!this->InitClient(this->clsData->method, 0) &&
			!this->InitClient(Method::TLSV1_2, 0) &&
			!this->InitClient(Method::TLSV1_1, 0) &&
			!this->InitClient(Method::TLSV1, 0))
		{
			return 0;
		}
		this->clsData->cliInit = true;
	}
	return this->CreateClientConn(0, s, hostName, err);
}

UnsafeArray<UTF8Char> Net::WinSSLEngine::GetErrorDetail(UnsafeArray<UTF8Char> sbuff)
{
	*sbuff = 0;
	return sbuff;
}

Bool Net::WinSSLEngine::GenerateCert(Text::CString country, Text::CString company, Text::CStringNN commonName, OutParam<NN<Crypto::Cert::X509Cert>> certASN1, OutParam<NN<Crypto::Cert::X509File>> keyASN1, UOSInt keyLength)
{
	HCRYPTKEY hKey;
	HCRYPTPROV hProv;
	if (!WinSSLEngine_CryptAcquireContextW(&hProv, L"SelfSign", NULL, PROV_RSA_FULL, CRYPT_MACHINE_KEYSET))
	{
		return false;
	}
	UInt32 lenFlags;
	if (keyLength == 2048)
	{
		lenFlags = 0x08000000;
	}
	else if (keyLength == 4096)
	{
		lenFlags = 0x10000000;
	}
	else
	{
		lenFlags = 0x0C0000000;
	}
	if (!CryptGenKey(hProv, AT_SIGNATURE, lenFlags | CRYPT_EXPORTABLE, &hKey))
	{
		CryptReleaseContext(hProv, 0);
		return false;
	}

	Text::StringBuilderW sb;
	Bool found = false;
	Text::CStringNN nns;
	if (country.SetTo(nns))
	{
		if (found) sb.AppendC(UTF8STRC(", "));
		found = true;
		sb.AppendC(UTF8STRC("C="));
		sb.Append(nns.v);
	}
	if (company.SetTo(nns))
	{
		if (found) sb.AppendC(UTF8STRC(", "));
		found = true;
		sb.AppendC(UTF8STRC("O="));
		sb.Append(nns.v);
	}
	if (found) sb.AppendC(UTF8STRC(", "));
	found = true;
	sb.AppendC(UTF8STRC("CN="));
	sb.Append(commonName.v);

	PCCERT_CONTEXT pCertContext = NULL;
	BYTE *pbEncoded = NULL;
	DWORD cbEncoded = 0;

	if (!CertStrToNameW(X509_ASN_ENCODING, sb.ToPtr(), CERT_X500_NAME_STR, NULL, pbEncoded, &cbEncoded, NULL))
	{
		CryptDestroyKey(hKey);
		CryptReleaseContext(hProv, 0);
		return false;
	}
	pbEncoded = MemAlloc(BYTE, cbEncoded);
	if (!CertStrToNameW(X509_ASN_ENCODING, sb.ToPtr(), CERT_X500_NAME_STR, NULL, pbEncoded, &cbEncoded, NULL))
	{
		MemFree(pbEncoded);
		CryptDestroyKey(hKey);
		CryptReleaseContext(hProv, 0);
		return false;
	}

	// Prepare certificate Subject for self-signed certificate
	CERT_NAME_BLOB subjectIssuerBlob;
	MemClear(&subjectIssuerBlob, sizeof(subjectIssuerBlob));
	subjectIssuerBlob.cbData = cbEncoded;
	subjectIssuerBlob.pbData = pbEncoded;

	CRYPT_KEY_PROV_INFO keyProvInfo;
	MemClear(&keyProvInfo, sizeof(keyProvInfo));
	keyProvInfo.pwszContainerName = (WChar*)L"SelfSign";
	keyProvInfo.pwszProvName = NULL;
	keyProvInfo.dwProvType = PROV_RSA_FULL;
	keyProvInfo.dwFlags = CRYPT_MACHINE_KEYSET;
	keyProvInfo.cProvParam = 0;
	keyProvInfo.rgProvParam = NULL;
	keyProvInfo.dwKeySpec = AT_SIGNATURE;

	CRYPT_ALGORITHM_IDENTIFIER signatureAlgorithm;
	MemClear(&signatureAlgorithm, sizeof(signatureAlgorithm));
	signatureAlgorithm.pszObjId = (Char*)szOID_RSA_SHA256RSA;

	SYSTEMTIME endTime;
	GetSystemTime(&endTime);
	endTime.wYear += 1;

	pCertContext = CertCreateSelfSignCertificate(0, &subjectIssuerBlob, 0, &keyProvInfo, &signatureAlgorithm, 0, &endTime, 0);
	if (!pCertContext)
	{
		MemFree(pbEncoded);
		CryptDestroyKey(hKey);
		CryptReleaseContext(hProv, 0);
		return false;
	}
	UInt8 privKeyBuff[2048];
	UInt8 certBuff[4096];
	DWORD certBuffSize = 4096;
	DWORD privKeySize = 2048;
	if (!CryptExportKey(hKey, 0, PRIVATEKEYBLOB, 0, privKeyBuff, &privKeySize))
	{
		CertFreeCertificateContext(pCertContext);
		MemFree(pbEncoded);
		CryptDestroyKey(hKey);
		CryptReleaseContext(hProv, 0);
		return false;
	}
	if (!CryptEncodeObjectEx(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, PKCS_RSA_PRIVATE_KEY, privKeyBuff, 0, 0, certBuff, &certBuffSize))
	{
		CertFreeCertificateContext(pCertContext);
		MemFree(pbEncoded);
		CryptDestroyKey(hKey);
		CryptReleaseContext(hProv, 0);
		return false;
	}
	Text::StringBuilderUTF8 sb2;
	sb2.ClearStr();
	sb2.Append(commonName);
	sb2.AppendC(UTF8STRC(".crt"));
	NN<Crypto::Cert::X509Cert> nnCert;
	NEW_CLASSNN(nnCert, Crypto::Cert::X509Cert(sb2.ToCString(), Data::ByteArray(pCertContext->pbCertEncoded, pCertContext->cbCertEncoded)));
	certASN1.Set(nnCert);
	sb2.ClearStr();
	sb2.Append(commonName);
	sb2.AppendC(UTF8STRC(".key"));
	NN<Text::String> s = Text::String::New(sb2.ToString(), sb2.GetLength());
	keyASN1.Set(Crypto::Cert::X509PrivKey::CreateFromKeyBuff(Crypto::Cert::X509File::KeyType::RSA, certBuff, certBuffSize, s.Ptr()));
	s->Release();
	CertFreeCertificateContext(pCertContext);
	//CryptReleaseContext(hCryptProvOrNCryptKey, 0);
	MemFree(pbEncoded);
	CryptDestroyKey(hKey);
	CryptReleaseContext(hProv, 0);
	return true;
}

Optional<Crypto::Cert::X509Key> Net::WinSSLEngine::GenerateRSAKey(UOSInt keyLength)
{
	HCRYPTKEY hKey;
	HCRYPTPROV hProv;
	UInt8 privKeyBuff[4096];
	DWORD privKeySize = 4096;
	UInt8 certBuff[4096];
	DWORD certBuffSize = 4096;
	if (!WinSSLEngine_CryptAcquireContextW(&hProv, L"SelfSign", NULL, PROV_RSA_FULL, CRYPT_MACHINE_KEYSET))
	{
		return 0;
	}
	UInt32 lenFlags;
	if (keyLength == 2048)
	{
		lenFlags = 0x08000000;
	}
	else if (keyLength == 4096)
	{
		lenFlags = 0x10000000;
	}
	else
	{
		lenFlags = 0x0C0000000;
	}
	if (!CryptGenKey(hProv, AT_SIGNATURE, lenFlags | CRYPT_EXPORTABLE, &hKey))
	{
		CryptReleaseContext(hProv, 0);
		return 0;
	}

	if (!CryptExportKey(hKey, 0, PRIVATEKEYBLOB, 0, privKeyBuff, &privKeySize))
	{
		CryptDestroyKey(hKey);
		CryptReleaseContext(hProv, 0);
		return 0;
	}
	if (!CryptEncodeObjectEx(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, PKCS_RSA_PRIVATE_KEY, privKeyBuff, 0, 0, certBuff, &certBuffSize))
	{
		CryptDestroyKey(hKey);
		CryptReleaseContext(hProv, 0);
		return 0;
	}

	Crypto::Cert::X509Key *key;
	NEW_CLASS(key, Crypto::Cert::X509Key(CSTR("RSAKey.key"), Data::ByteArray(certBuff, certBuffSize), Crypto::Cert::X509File::KeyType::RSA));
	CryptDestroyKey(hKey);
	CryptReleaseContext(hProv, 0);
	return key;
}

Optional<Crypto::Cert::X509Key> Net::WinSSLEngine::GenerateECDSAKey(Crypto::Cert::X509File::ECName name)
{
	BCRYPT_ALG_HANDLE hAlgorithm;
	BCRYPT_KEY_HANDLE hKey;
	UInt8 buff[8192];
	ULONG buffSize;
	const WChar *algId = 0;
	UInt32 bitLeng;
	switch (name)
	{
	case Crypto::Cert::X509File::ECName::secp256r1:
		algId = BCRYPT_ECDSA_P256_ALGORITHM;
		bitLeng = 256;
		break;
	case Crypto::Cert::X509File::ECName::secp384r1:
		algId = BCRYPT_ECDSA_P384_ALGORITHM;
		bitLeng = 384;
		break;
	case Crypto::Cert::X509File::ECName::secp521r1:
		algId = BCRYPT_ECDSA_P521_ALGORITHM;
		bitLeng = 521;
		break;
	default:
#if defined(VERBOSE_SVR) || defined(VERBOSE_CLI)
		printf("WinSSLEngine.GenerateECDSAKey: Unsupported name\r\n");
#endif
		return 0;
	}
	NTSTATUS status;
	if ((status = BCryptOpenAlgorithmProvider(&hAlgorithm, algId, 0, 0)) != 0)
	{
#if defined(VERBOSE_SVR) || defined(VERBOSE_CLI)
		printf("WinSSLEngine.GenerateECDSAKey: BCryptOpenAlgorithmProvider failed: %d\r\n", status);
#endif
		return 0;
	}
	if ((status = BCryptGenerateKeyPair(hAlgorithm, &hKey, bitLeng, 0)) != 0)
	{
#if defined(VERBOSE_SVR) || defined(VERBOSE_CLI)
		printf("WinSSLEngine.GenerateECDSAKey: BCryptGenerateKeyPair failed: %d\r\n", status);
#endif
		BCryptCloseAlgorithmProvider(hAlgorithm, 0);
		return 0;
	}
	if ((status = BCryptFinalizeKeyPair(hKey, 0)) != 0)
	{
#if defined(VERBOSE_SVR) || defined(VERBOSE_CLI)
		printf("WinSSLEngine.GenerateECDSAKey: BCryptFinalizeKeyPair failed: %d\r\n", status);
#endif
		BCryptCloseAlgorithmProvider(hAlgorithm, 0);
		return 0;
	}
	Optional<Crypto::Cert::X509Key> key = 0;
	if ((status = BCryptExportKey(hKey, 0, BCRYPT_ECCPRIVATE_BLOB, buff, sizeof(buff), &buffSize, 0)) == 0)
	{
		UInt32 pkLen = ReadUInt32(&buff[4]);
#if defined(VERBOSE_SVR) || defined(VERBOSE_CLI)
		printf("WinSSLEngine.GenerateECDSAKey: BCryptExportKey success\r\n");

		Text::StringBuilderUTF8 sb;
		sb.AppendHexBuff(Data::ByteArrayR(buff, buffSize), ' ', Text::LineBreakType::CRLF);
		printf("WinSSLEngine.GenerateECDSAKey: %s\r\n", sb.v.Ptr());
#endif
		Net::ASN1PDUBuilder asn1;
		asn1.BeginSequence();
		asn1.AppendInt32(1);
		asn1.AppendOctetString(buff + 8 + pkLen + pkLen, pkLen);
		asn1.BeginContentSpecific(0);
			if (name == Crypto::Cert::X509File::ECName::secp256r1)
				asn1.AppendOIDString(CSTR("1.2.840.10045.3.1.7"));
			else if (name == Crypto::Cert::X509File::ECName::secp384r1)
				asn1.AppendOIDString(CSTR("1.3.132.0.34"));
			else if (name == Crypto::Cert::X509File::ECName::secp521r1)
				asn1.AppendOIDString(CSTR("1.3.132.0.35"));
		asn1.EndLevel();
		asn1.BeginContentSpecific(1);
			buff[7] = 4;
			asn1.AppendBitString(0, Data::ByteArrayR(buff + 7, pkLen * 2 + 1));
		asn1.EndLevel();
		asn1.EndLevel();
		NEW_CLASSOPT(key, Crypto::Cert::X509Key(CSTR("ECDSAKey.key"), asn1.GetArray(), Crypto::Cert::X509File::KeyType::ECDSA));
	}
	else
	{
#if defined(VERBOSE_SVR) || defined(VERBOSE_CLI)
		printf("WinSSLEngine.GenerateECDSAKey: BCryptExportKey failed: %d\r\n", status);
#endif
	}
	BCryptDestroyKey(hKey);
	BCryptCloseAlgorithmProvider(hAlgorithm, 0);
	return key;
}

Bool Net::WinSSLEngine::Signature(NN<Crypto::Cert::X509Key> key, Crypto::Hash::HashType hashType, Data::ByteArrayR payload, UnsafeArray<UInt8> signData, OutParam<UOSInt> signLen)
{
	Crypto::Cert::X509File::KeyType keyType = key->GetKeyType();
	if (keyType == Crypto::Cert::X509File::KeyType::RSA)
	{
		ALG_ID alg = WinSSLEngine_CryptGetHashAlg(hashType);
		if (alg == 0)
		{
			return false;
		}
		HCRYPTPROV hProv = WinSSLEngine_CreateProv(keyType, 0, 0);
		if (hProv == 0)
		{
			return false;
		}
		HCRYPTKEY hKey = WinSSLEngine_ImportKey(hProv, key, true, true);
		if (hKey == 0)
		{
			CryptReleaseContext(hProv, 0);
			return false;
		}
		HCRYPTHASH hHash;
		if (!CryptCreateHash(hProv, alg, 0, 0, &hHash))
		{
#if defined(VERBOSE_SVR) || defined(VERBOSE_CLI)
			UInt32 errCode = GetLastError();
			printf("WinSSLEngine.Signature: CryptCreateHash failed, errCode = 0x%x\r\n", errCode);
#endif
			CryptReleaseContext(hProv, 0);
			CryptDestroyKey(hKey);
			return false;
		}
		if (!CryptHashData(hHash, payload.Arr().Ptr(), (DWORD)payload.GetSize(), 0))
		{
			CryptReleaseContext(hProv, 0);
			CryptDestroyHash(hHash);
			CryptDestroyKey(hKey);
#if defined(VERBOSE_SVR) || defined(VERBOSE_CLI)
			printf("WinSSLEngine.Signature: CryptHashData failed\r\n");
#endif
			return false;
		}
		DWORD len = 512;
		if (!CryptSignHashW(hHash, AT_SIGNATURE, 0, 0, signData.Ptr(), &len))
		{
#if defined(VERBOSE_SVR) || defined(VERBOSE_CLI)
			UInt32 errCode = GetLastError();
			printf("WinSSLEngine.Signature: CryptSignHash failed, errCode = 0x%x\r\n", errCode);
#endif
			CryptReleaseContext(hProv, 0);
			CryptDestroyHash(hHash);
			CryptDestroyKey(hKey);
			return false;
		}
		signLen.Set(len);
		CryptReleaseContext(hProv, 0);
		CryptDestroyHash(hHash);
		CryptDestroyKey(hKey);
		UOSInt i = 0;
		UOSInt j = len - 1;
		UInt8 t;
		while (i < j)
		{
			t = signData[i];
			signData[i] = signData[j];
			signData[j] = t;
			i++;
			j--;
		}
#if defined(VERBOSE_SVR) || defined(VERBOSE_CLI)
		printf("WinSSLEngine.Signature: Signature success, len = %d\r\n", (UInt32)len);
#endif
		return true;
	}
	else if (keyType == Crypto::Cert::X509File::KeyType::ECDSA)
	{
		BCRYPT_ALG_HANDLE hSignAlg;
		BCRYPT_KEY_HANDLE hKey;
		if (!WinSSLEngine_BCryptImportKey(key, hSignAlg, hKey))
		{
#if defined(VERBOSE_SVR) || defined(VERBOSE_CLI)
			printf("WinSSLEngine.Signature: WinSSLEngine_BCryptImportKey failed\r\n");
#endif
		}

#if defined(VERBOSE_SVR) || defined(VERBOSE_CLI)
		printf("WinSSLEngine.Signature: BCryptImportKeyPair success\r\n");
#endif

		UInt8 hashVal[64];
		UInt8 signBuff[256];
		UOSInt hashLen;
		NN<Crypto::Hash::HashAlgorithm> hash;
		if (!Crypto::Hash::HashCreator::CreateHash(hashType).SetTo(hash))
		{
#if defined(VERBOSE_SVR) || defined(VERBOSE_CLI)
			printf("WinSSLEngine.Signature: HashType not supported: %s\r\n", Crypto::Hash::HashTypeGetName(hashType).v.Ptr());
#endif
			BCryptDestroyKey(hKey);
			BCryptCloseAlgorithmProvider(hSignAlg, 0);
			return false;
		}
		hash->Calc(payload.Arr(), payload.GetSize());
		hash->GetValue(hashVal);
		hashLen = hash->GetResultSize();
		hash.Delete();
		Bool succ = false;
		ULONG resLen = 0;
		NTSTATUS status = BCryptSignHash(hKey, 0, hashVal, (ULONG)hashLen, signBuff, 256, &resLen, 0);
		if (status == 0)
		{
			succ = true;
			UOSInt i;
			signData[0] = 0x30;
			if (signBuff[0] & 0x80)
			{
				signData[2] = 2;
				signData[3] = (UInt8)((resLen >> 1) + 1);
				signData[4] = 0;
				MemCopyNO(&signData[5], signBuff, resLen >> 1);
				i = 5 + (resLen >> 1);
			}
			else
			{
				signData[2] = 2;
				signData[3] = (UInt8)(resLen >> 1);
				MemCopyNO(&signData[4], signBuff, resLen >> 1);
				i = 4 + (resLen >> 1);
			}
			if (signBuff[resLen >> 1] & 0x80)
			{
				signData[i] = 2;
				signData[i + 1] = (UInt8)((resLen >> 1) + 1);
				signData[i + 2] = 0;
				MemCopyNO(&signData[i + 3], &signBuff[resLen >> 1], resLen >> 1);
				i += 3 + (resLen >> 1);
			}
			else
			{
				signData[i] = 2;
				signData[i + 1] = (UInt8)(resLen >> 1);
				MemCopyNO(&signData[i + 2], &signBuff[resLen >> 1], resLen >> 1);
				i += 2 + (resLen >> 1);
			}
			signData[1] = (UInt8)(i - 2);
			signLen.Set(i);
		}
		else
		{
#if defined(VERBOSE_SVR) || defined(VERBOSE_CLI)
			printf("WinSSLEngine.Signature: BCryptSignHash failed: 0x%x\r\n", status);
#endif
		}
		BCryptDestroyKey(hKey);
		BCryptCloseAlgorithmProvider(hSignAlg, 0);
		return succ;
	}
	return false;
}

Bool Net::WinSSLEngine::SignatureVerify(NN<Crypto::Cert::X509Key> key, Crypto::Hash::HashType hashType, Data::ByteArrayR payload, Data::ByteArrayR signData)
{
	Crypto::Cert::X509File::KeyType keyType = key->GetKeyType();
	if (keyType == Crypto::Cert::X509File::KeyType::RSA || keyType == Crypto::Cert::X509File::KeyType::RSAPublic)
	{
		ALG_ID alg = WinSSLEngine_CryptGetHashAlg(hashType);
		if (alg == 0)
		{
#if defined(VERBOSE_SVR) || defined(VERBOSE_CLI)
			printf("WinSSLEngine.SignatureVerify: WinSSLEngine_CryptGetHashAlg failed\r\n");
#endif
			return false;
		}
		HCRYPTPROV hProv = WinSSLEngine_CreateProv(key->GetKeyType(), 0, 0);
		if (hProv == 0)
		{
#if defined(VERBOSE_SVR) || defined(VERBOSE_CLI)
			printf("WinSSLEngine.SignatureVerify: WinSSLEngine_CreateProv failed\r\n");
#endif
			return false;
		}
		HCRYPTKEY hKey = WinSSLEngine_ImportKey(hProv, key, false, true);
		if (hKey == 0)
		{
#if defined(VERBOSE_SVR) || defined(VERBOSE_CLI)
			printf("WinSSLEngine.SignatureVerify: WinSSLEngine_ImportKey failed\r\n");
#endif
			CryptReleaseContext(hProv, 0);
			return false;
		}
		HCRYPTHASH hHash;
		if (!CryptCreateHash(hProv, alg, 0, 0, &hHash))
		{
#if defined(VERBOSE_SVR) || defined(VERBOSE_CLI)
			UInt32 errCode = GetLastError();
			printf("WinSSLEngine.SignatureVerify: CryptCreateHash failed, errCode = 0x%x\r\n", errCode);
#endif
			CryptReleaseContext(hProv, 0);
			CryptDestroyKey(hKey);
			return false;
		}
		if (!CryptHashData(hHash, payload.Arr().Ptr(), (DWORD)payload.GetSize(), 0))
		{
#if defined(VERBOSE_SVR) || defined(VERBOSE_CLI)
			UInt32 errCode = GetLastError();
			printf("WinSSLEngine.SignatureVerify: CryptHashData failed, errCode = 0x%x\r\n", errCode);
#endif
			CryptReleaseContext(hProv, 0);
			CryptDestroyHash(hHash);
			CryptDestroyKey(hKey);
			return false;
		}
		UInt8 mySignData[512];
		if (signData.GetSize() > 512)
		{
			CryptReleaseContext(hProv, 0);
			CryptDestroyHash(hHash);
			CryptDestroyKey(hKey);
#if defined(VERBOSE_SVR) || defined(VERBOSE_CLI)
			printf("WinSSLEngine.SignatureVerify: Signature length too long\r\n");
#endif
			return false;
		}
		UOSInt i = 0;
		UOSInt j = signData.GetSize() - 1;
		while (i < signData.GetSize())
		{
			mySignData[i] = signData[j];
			i++;
			j--;
		}
		if (!CryptVerifySignatureW(hHash, mySignData, (DWORD)signData.GetSize(), hKey, 0, 0))
		{
#if defined(VERBOSE_SVR) || defined(VERBOSE_CLI)
			UInt32 errCode = GetLastError();
			printf("WinSSLEngine.SignatureVerify: CryptVerifySignatureW failed, errCode = 0x%x\r\n", errCode);
#endif
			CryptReleaseContext(hProv, 0);
			CryptDestroyHash(hHash);
			CryptDestroyKey(hKey);
			return false;
		}
		CryptReleaseContext(hProv, 0);
		CryptDestroyHash(hHash);
		CryptDestroyKey(hKey);
#if defined(VERBOSE_SVR) || defined(VERBOSE_CLI)
		printf("WinSSLEngine.SignatureVerify: Verify Signature success\r\n");
#endif
		return true;
	}
	else if (keyType == Crypto::Cert::X509File::KeyType::ECDSA || keyType == Crypto::Cert::X509File::KeyType::ECPublic)
	{
		BCRYPT_ALG_HANDLE hSignAlg;
		BCRYPT_KEY_HANDLE hKey;
		if (!WinSSLEngine_BCryptImportKey(key, hSignAlg, hKey))
		{
#if defined(VERBOSE_SVR) || defined(VERBOSE_CLI)
			printf("WinSSLEngine.Signature: WinSSLEngine_BCryptImportKey failed\r\n");
#endif
		}

		UInt8 hashVal[64];
		UOSInt hashLen;
		UInt8 signBuff[256];
		NN<Crypto::Hash::HashAlgorithm> hash;
		if (!Crypto::Hash::HashCreator::CreateHash(hashType).SetTo(hash))
		{
#if defined(VERBOSE_SVR) || defined(VERBOSE_CLI)
			printf("WinSSLEngine.SignatureVerify: HashType not supported: %s\r\n", Crypto::Hash::HashTypeGetName(hashType).v.Ptr());
#endif
			BCryptDestroyKey(hKey);
			BCryptCloseAlgorithmProvider(hSignAlg, 0);
			return false;
		}
		hash->Calc(payload.Arr(), payload.GetSize());
		hash->GetValue(hashVal);
		hashLen = hash->GetResultSize();
		hash.Delete();
		Bool succ = false;
		ULONG signLen = 0;
		if (signData[0] == 0x30 && signData[1] == signData.GetSize() - 2 && signData[2] == 2)
		{
			UOSInt i;
			UOSInt len = signData[3];
			i = 4 + len;
			if (signData[4] == 0)
			{
				MemCopyNO(signBuff, &signData[5], len - 1);
				signLen = (ULONG)(len - 1);
			}
			else
			{
				MemCopyNO(signBuff, &signData[4], len);
				signLen = (ULONG)len;
			}
			len = signData[i + 1];
			if (signData[i + 2] == 0)
			{
				MemCopyNO(&signBuff[signLen], &signData[i + 3], len - 1);
				signLen += (ULONG)(len - 1);
			}
			else
			{
				MemCopyNO(&signBuff[signLen], &signData[i + 2], len);
				signLen += (ULONG)len;
			}
			NTSTATUS status = BCryptVerifySignature(hKey, 0, hashVal, (ULONG)hashLen, signBuff, signLen, 0);
			if (status == 0)
			{
				succ = true;
			}
			else
			{
#if defined(VERBOSE_SVR) || defined(VERBOSE_CLI)
				printf("WinSSLEngine.SignatureVerify: BCryptVerifySignature failed: 0x%x\r\n", status);
#endif
			}
		}
		else
		{
#if defined(VERBOSE_SVR) || defined(VERBOSE_CLI)
			printf("WinSSLEngine.SignatureVerify: SignData format invalid\r\n");
#endif
		}
		BCryptDestroyKey(hKey);
		BCryptCloseAlgorithmProvider(hSignAlg, 0);
		return succ;
	}
	return false;
}

UOSInt Net::WinSSLEngine::Encrypt(NN<Crypto::Cert::X509Key> key, UnsafeArray<UInt8> encData, Data::ByteArrayR payload, Crypto::Encrypt::RSACipher::Padding rsaPadding)
{
	HCRYPTPROV hProv = WinSSLEngine_CreateProv(key->GetKeyType(), 0, 0);
	if (hProv == 0)
	{
		return false;
	}
	HCRYPTKEY hKey = WinSSLEngine_ImportKey(hProv, key, false, false);
	if (hKey == 0)
	{
		CryptReleaseContext(hProv, 0);
		return false;
	}
	DWORD dataSize = (DWORD)payload.GetSize();
	DWORD flags = 0;
	if (rsaPadding == Crypto::Encrypt::RSACipher::Padding::PKCS1_OAEP)
	{
		flags = CRYPT_OAEP;
	}
	UInt8 myBuff[512];
	MemCopyNO(myBuff, payload.Arr().Ptr(), payload.GetSize());
	if (!CryptEncrypt(hKey, 0, TRUE, flags, myBuff, &dataSize, 512))
	{
		CryptReleaseContext(hProv, 0);
		CryptDestroyKey(hKey);
#if defined(VERBOSE_SVR) || defined(VERBOSE_CLI)
		UInt32 err = GetLastError();
		printf("SSL: CryptEncrypt failed, 0x%x (%s)\r\n", err, IO::WindowsError::GetString(err).v.Ptr());
#endif
		return 0;
	}
	CryptReleaseContext(hProv, 0);
	CryptDestroyKey(hKey);
	UOSInt i = 0;
	UOSInt j = dataSize - 1;
	while (i < dataSize)
	{
		encData[i] = myBuff[j];
		i++;
		j--;
	}
	return dataSize;
}

UOSInt Net::WinSSLEngine::Decrypt(NN<Crypto::Cert::X509Key> key, UnsafeArray<UInt8> decData, Data::ByteArrayR payload, Crypto::Encrypt::RSACipher::Padding rsaPadding)
{
	if (payload.GetSize() > 512)
	{
#if defined(VERBOSE_SVR) || defined(VERBOSE_CLI)
		printf("SSL: Data too large to decrypt\r\n");
#endif
		return 0;
	}
	ALG_ID alg = WinSSLEngine_CryptGetHashAlg(Crypto::Hash::HashType::SHA1);
	if (alg == 0)
	{
		return false;
	}
	HCRYPTPROV hProv = WinSSLEngine_CreateProv(key->GetKeyType(), 0, 0);
	if (hProv == 0)
	{
		return false;
	}
	HCRYPTKEY hKey = WinSSLEngine_ImportKey(hProv, key, false, false);
	if (hKey == 0)
	{
		CryptReleaseContext(hProv, 0);
		return false;
	}
	HCRYPTHASH hHash;
	if (!CryptCreateHash(hProv, alg, 0, 0, &hHash))
	{
#if defined(VERBOSE_SVR) || defined(VERBOSE_CLI)
		UInt32 errCode = GetLastError();
		printf("SSL: CryptCreateHash failed, errCode = 0x%x\r\n", errCode);
#endif
		CryptReleaseContext(hProv, 0);
		CryptDestroyKey(hKey);
		return false;
	}
	DWORD dataSize = (DWORD)payload.GetSize();
	DWORD flags;
	if (rsaPadding == Crypto::Encrypt::RSACipher::Padding::PKCS1)
	{
		flags = 0;
	}
	else
	{
		flags = CRYPT_DECRYPT_RSA_NO_PADDING_CHECK;
	}
	UInt8 myBuff[512];
	UOSInt i = 0;
	UOSInt j = payload.GetSize() - 1;
	while (i < payload.GetSize())
	{
		myBuff[i] = payload[j];
		i++;
		j--;
	}
	if (!CryptDecrypt(hKey, 0, TRUE, flags, myBuff, &dataSize))
	{
		CryptReleaseContext(hProv, 0);
		CryptDestroyHash(hHash);
		CryptDestroyKey(hKey);
#if defined(VERBOSE_SVR) || defined(VERBOSE_CLI)
		UInt32 err = GetLastError();
		printf("SSL: CryptDecrypt failed, 0x%x (%s)\r\n", err, IO::WindowsError::GetString(err).v.Ptr());
#endif
		return 0;
	}
	if (rsaPadding == Crypto::Encrypt::RSACipher::Padding::PKCS1)
	{
		MemCopyNO(decData.Ptr(), myBuff, dataSize);
	}
	else
	{
		dataSize = (DWORD)Crypto::Encrypt::RSACipher::PaddingRemove(decData, myBuff, dataSize, rsaPadding);
	}
	CryptReleaseContext(hProv, 0);
	CryptDestroyHash(hHash);
	CryptDestroyKey(hKey);
	return dataSize;
}
