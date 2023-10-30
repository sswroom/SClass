#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Data/RandomBytesGenerator.h"
#include "Net/SocketUtil.h"
#include "Net/SSLUtil.h"

UInt16 Net::SSLUtil::csuites[] = {
	0x1302, //TLS_AES_256_GCM_SHA384
	0x1303, //TLS_CHACHA20_POLY1305_SHA256
	0x1301, //TLS_AES_128_GCM_SHA256
	0xc02c, //TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384
    0xc030, //TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384 (0xc030)
    0x00a3, //TLS_DHE_DSS_WITH_AES_256_GCM_SHA384 (0x00a3)
    0x009f, //TLS_DHE_RSA_WITH_AES_256_GCM_SHA384 (0x009f)
    0xcca9, //TLS_ECDHE_ECDSA_WITH_CHACHA20_POLY1305_SHA256 (0xcca9)
    0xcca8, //TLS_ECDHE_RSA_WITH_CHACHA20_POLY1305_SHA256 (0xcca8)
    0xccaa, //TLS_DHE_RSA_WITH_CHACHA20_POLY1305_SHA256 (0xccaa)
    0xc0af, //TLS_ECDHE_ECDSA_WITH_AES_256_CCM_8 (0xc0af)
    0xc0ad, //TLS_ECDHE_ECDSA_WITH_AES_256_CCM (0xc0ad)
    0xc0a3, //TLS_DHE_RSA_WITH_AES_256_CCM_8 (0xc0a3)
    0xc09f, //TLS_DHE_RSA_WITH_AES_256_CCM (0xc09f)
    0xc05d, //TLS_ECDHE_ECDSA_WITH_ARIA_256_GCM_SHA384 (0xc05d)
    0xc061, //TLS_ECDHE_RSA_WITH_ARIA_256_GCM_SHA384 (0xc061)
    0xc057, //TLS_DHE_DSS_WITH_ARIA_256_GCM_SHA384 (0xc057)
    0xc053, //TLS_DHE_RSA_WITH_ARIA_256_GCM_SHA384 (0xc053)
    0x00a7, //TLS_DH_anon_WITH_AES_256_GCM_SHA384 (0x00a7)
    0xc02b, //TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256 (0xc02b)
    0xc02f, //TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256 (0xc02f)
    0x00a2, //TLS_DHE_DSS_WITH_AES_128_GCM_SHA256 (0x00a2)
    0x009e, //TLS_DHE_RSA_WITH_AES_128_GCM_SHA256 (0x009e)
    0xc0ae, //TLS_ECDHE_ECDSA_WITH_AES_128_CCM_8 (0xc0ae)
    0xc0ac, //TLS_ECDHE_ECDSA_WITH_AES_128_CCM (0xc0ac)
    0xc0a2, //TLS_DHE_RSA_WITH_AES_128_CCM_8 (0xc0a2)
    0xc09e, //TLS_DHE_RSA_WITH_AES_128_CCM (0xc09e)
    0xc05c, //TLS_ECDHE_ECDSA_WITH_ARIA_128_GCM_SHA256 (0xc05c)
    0xc060, //TLS_ECDHE_RSA_WITH_ARIA_128_GCM_SHA256 (0xc060)
    0xc056, //TLS_DHE_DSS_WITH_ARIA_128_GCM_SHA256 (0xc056)
    0xc052, //TLS_DHE_RSA_WITH_ARIA_128_GCM_SHA256 (0xc052)
    0x00a6, //TLS_DH_anon_WITH_AES_128_GCM_SHA256 (0x00a6)
    0xc024, //TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA384 (0xc024)
    0xc028, //TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA384 (0xc028)
    0x006b, //TLS_DHE_RSA_WITH_AES_256_CBC_SHA256 (0x006b)
    0x006a, //TLS_DHE_DSS_WITH_AES_256_CBC_SHA256 (0x006a)
    0xc073, //TLS_ECDHE_ECDSA_WITH_CAMELLIA_256_CBC_SHA384 (0xc073)
    0xc077, //TLS_ECDHE_RSA_WITH_CAMELLIA_256_CBC_SHA384 (0xc077)
    0x00c4, //TLS_DHE_RSA_WITH_CAMELLIA_256_CBC_SHA256 (0x00c4)
    0x00c3, //TLS_DHE_DSS_WITH_CAMELLIA_256_CBC_SHA256 (0x00c3)
    0x006d, //TLS_DH_anon_WITH_AES_256_CBC_SHA256 (0x006d)
    0x00c5, //TLS_DH_anon_WITH_CAMELLIA_256_CBC_SHA256 (0x00c5)
    0xc023, //TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256 (0xc023)
    0xc027, //TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA256 (0xc027)
    0x0067, //TLS_DHE_RSA_WITH_AES_128_CBC_SHA256 (0x0067)
    0x0040, //TLS_DHE_DSS_WITH_AES_128_CBC_SHA256 (0x0040)
    0xc072, //TLS_ECDHE_ECDSA_WITH_CAMELLIA_128_CBC_SHA256 (0xc072)
    0xc076, //TLS_ECDHE_RSA_WITH_CAMELLIA_128_CBC_SHA256 (0xc076)
    0x00be, //TLS_DHE_RSA_WITH_CAMELLIA_128_CBC_SHA256 (0x00be)
    0x00bd, //TLS_DHE_DSS_WITH_CAMELLIA_128_CBC_SHA256 (0x00bd)
    0x006c, //TLS_DH_anon_WITH_AES_128_CBC_SHA256 (0x006c)
    0x00bf, //TLS_DH_anon_WITH_CAMELLIA_128_CBC_SHA256 (0x00bf)
    0xc00a, //TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA (0xc00a)
    0xc014, //TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA (0xc014)
    0x0039, //TLS_DHE_RSA_WITH_AES_256_CBC_SHA (0x0039)
    0x0038, //TLS_DHE_DSS_WITH_AES_256_CBC_SHA (0x0038)
    0x0088, //TLS_DHE_RSA_WITH_CAMELLIA_256_CBC_SHA (0x0088)
    0x0087, //TLS_DHE_DSS_WITH_CAMELLIA_256_CBC_SHA (0x0087)
    0xc019, //TLS_ECDH_anon_WITH_AES_256_CBC_SHA (0xc019)
    0x003a, //TLS_DH_anon_WITH_AES_256_CBC_SHA (0x003a)
    0x0089, //TLS_DH_anon_WITH_CAMELLIA_256_CBC_SHA (0x0089)
    0xc009, //TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA (0xc009)
    0xc013, //TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA (0xc013)
    0x0033, //TLS_DHE_RSA_WITH_AES_128_CBC_SHA (0x0033)
    0x0032, //TLS_DHE_DSS_WITH_AES_128_CBC_SHA (0x0032)
    0x009a, //TLS_DHE_RSA_WITH_SEED_CBC_SHA (0x009a)
    0x0099, //TLS_DHE_DSS_WITH_SEED_CBC_SHA (0x0099)
    0x0045, //TLS_DHE_RSA_WITH_CAMELLIA_128_CBC_SHA (0x0045)
    0x0044, //TLS_DHE_DSS_WITH_CAMELLIA_128_CBC_SHA (0x0044)
    0xc018, //TLS_ECDH_anon_WITH_AES_128_CBC_SHA (0xc018)
    0x0034, //TLS_DH_anon_WITH_AES_128_CBC_SHA (0x0034)
    0x009b, //TLS_DH_anon_WITH_SEED_CBC_SHA (0x009b)
    0x0046, //TLS_DH_anon_WITH_CAMELLIA_128_CBC_SHA (0x0046)
    0x009d, //TLS_RSA_WITH_AES_256_GCM_SHA384 (0x009d)
    0xc0a1, //TLS_RSA_WITH_AES_256_CCM_8 (0xc0a1)
    0xc09d, //TLS_RSA_WITH_AES_256_CCM (0xc09d)
    0xc051, //TLS_RSA_WITH_ARIA_256_GCM_SHA384 (0xc051)
    0x009c, //TLS_RSA_WITH_AES_128_GCM_SHA256 (0x009c)
    0xc0a0, //TLS_RSA_WITH_AES_128_CCM_8 (0xc0a0)
    0xc09c, //TLS_RSA_WITH_AES_128_CCM (0xc09c)
    0xc050, //TLS_RSA_WITH_ARIA_128_GCM_SHA256 (0xc050)
    0x003d, //TLS_RSA_WITH_AES_256_CBC_SHA256 (0x003d)
    0x00c0, //TLS_RSA_WITH_CAMELLIA_256_CBC_SHA256 (0x00c0)
    0x003c, //TLS_RSA_WITH_AES_128_CBC_SHA256 (0x003c)
    0x00ba, //TLS_RSA_WITH_CAMELLIA_128_CBC_SHA256 (0x00ba)
    0x0035, //TLS_RSA_WITH_AES_256_CBC_SHA (0x0035)
    0x0084, //TLS_RSA_WITH_CAMELLIA_256_CBC_SHA (0x0084)
    0x002f, //TLS_RSA_WITH_AES_128_CBC_SHA (0x002f)
    0x0096, //TLS_RSA_WITH_SEED_CBC_SHA (0x0096)
    0x0041, //TLS_RSA_WITH_CAMELLIA_128_CBC_SHA (0x0041)
    0x00ff, //TLS_EMPTY_RENEGOTIATION_INFO_SCSV (0x00ff)
};

UOSInt Net::SSLUtil::GenClientHello(UInt8 *buff, Text::CStringNN serverHost)
{
	Data::RandomBytesGenerator randGen;
	UOSInt len;
	buff[0] = 1; //Client Hello
	buff[2] = 3; //TLS
	buff[3] = 3; //1.2
	randGen.NextBytes(&buff[4], 32); //Random
	buff[36] = 32; //Session ID Length
	randGen.NextBytes(&buff[37], 32); //Session ID
	WriteMUInt16(&buff[69], sizeof(csuites));
	UOSInt i = 0;
	UOSInt j = sizeof(csuites) >> 1;
	while (i < j)
	{
		WriteMUInt16(&buff[71 + i * 2], csuites[i]);
		i++;
	}
	len = 71 + j * 2;
	buff[len] = 1; //Compression Method Length
	buff[len + 1] = 0; //Compression Method: Null
	i = len + 2; //Extension
	len = i + 2;
	WriteMUInt16(&buff[len], 0); //Extension type: server_name
	WriteMUInt16(&buff[len + 2], serverHost.leng + 5); //Extension length
	WriteMUInt16(&buff[len + 4], serverHost.leng + 3); //Server name list length
	buff[len + 6] = 0; //Server name Type: host_name
	WriteMUInt16(&buff[len + 7], serverHost.leng); //Server name length
	MemCopyNOShort(&buff[len + 9], serverHost.v, serverHost.leng); //Server name
	len += serverHost.leng + 9;

	WriteMUInt16(&buff[len], 11); //Extension type: supported_versions
	WriteMUInt16(&buff[len + 2], 4); //Extension length
	buff[len + 4] = 3; //EC point formats length
	buff[len + 5] = 0; //EC point format: uncompressed
	buff[len + 6] = 1; //EC point format: ansiX962_compressed_prime
	buff[len + 7] = 2; //EC point format: ansiX962_compressed_char2
	len += 8;

	WriteMUInt16(&buff[len], 35); //Extension type: session_ticket
	WriteMUInt16(&buff[len + 2], 0); //Extension length
	len += 4;

	WriteMUInt16(&buff[len], 13); //Extension type: signature_algorithms
	WriteMUInt16(&buff[len + 2], 48); //Extension length
	WriteMUInt16(&buff[len + 4], 46); //Supported Group List length
	WriteMUInt16(&buff[len + 6], 0x0804); //rsa_pss_rsae_sha256
	WriteMUInt16(&buff[len + 8], 0x0805); //rsa_pss_rsae_sha384
	WriteMUInt16(&buff[len + 10], 0x0806); //rsa_pss_rsae_sha512
	WriteMUInt16(&buff[len + 12], 0x0807); //ed25519
	WriteMUInt16(&buff[len + 14], 0x0808); //ed448
	WriteMUInt16(&buff[len + 16], 0x0809); //rsa_pss_pss_sha256
	WriteMUInt16(&buff[len + 18], 0x080a); //rsa_pss_pss_sha384
	WriteMUInt16(&buff[len + 20], 0x080b); //rsa_pss_pss_sha512
	WriteMUInt16(&buff[len + 22], 0x0601); //rsa_pkcs1_sha512
	WriteMUInt16(&buff[len + 24], 0x0602); //SHA512 DSA
	WriteMUInt16(&buff[len + 26], 0x0603); //ecdsa_secp521r1_sha512
	WriteMUInt16(&buff[len + 28], 0x0501); //rsa_pkcs1_sha384
	WriteMUInt16(&buff[len + 30], 0x0502); //SHA384 DSA
	WriteMUInt16(&buff[len + 32], 0x0503); //ecdsa_secp384r1_sha384
	WriteMUInt16(&buff[len + 34], 0x0401); //rsa_pkcs1_sha256
	WriteMUInt16(&buff[len + 36], 0x0402); //SHA256 DSA
	WriteMUInt16(&buff[len + 38], 0x0403); //ecdsa_secp256r1_sha256
	WriteMUInt16(&buff[len + 40], 0x0301); //SHA224 RSA
	WriteMUInt16(&buff[len + 42], 0x0302); //SHA224 DSA
	WriteMUInt16(&buff[len + 44], 0x0303); //SHA224 ECDSA
	WriteMUInt16(&buff[len + 46], 0x0201); //rsa_pkcs1_sha1
	WriteMUInt16(&buff[len + 48], 0x0202); //SHA1 DSA
	WriteMUInt16(&buff[len + 50], 0x0203); //ecdsa_sha1
	len += 52;

	WriteMUInt16(&buff[len], 10); //Extension type: supported_groups
	WriteMUInt16(&buff[len + 2], 28); //Extension length
	WriteMUInt16(&buff[len + 4], 26); //Supported Group List length
	WriteMUInt16(&buff[len + 6], 0x0017); //secp256r1
	WriteMUInt16(&buff[len + 8], 0x0019); //secp521r1
	WriteMUInt16(&buff[len + 10], 0x001c); //brainpoolP512r1
	WriteMUInt16(&buff[len + 12], 0x001b); //brainpoolP384r1
	WriteMUInt16(&buff[len + 14], 0x0018); //secp384r1
	WriteMUInt16(&buff[len + 16], 0x001a); //brainpoolP256r1
	WriteMUInt16(&buff[len + 18], 0x0016); //secp256k1
	WriteMUInt16(&buff[len + 20], 0x000e); //sect571r1
	WriteMUInt16(&buff[len + 22], 0x000d); //sect571k1
	WriteMUInt16(&buff[len + 24], 0x000b); //sect409k1
	WriteMUInt16(&buff[len + 26], 0x000c); //sect409r1
	WriteMUInt16(&buff[len + 28], 0x0009); //sect283k1
	WriteMUInt16(&buff[len + 30], 0x000a); //sect283r1
	len += 32;

	WriteMUInt16(&buff[len], 43); //Extension type: supported_versions
	WriteMUInt16(&buff[len + 2], 9); //Extension length
	buff[len + 4] = 8; //Supported versions length
	WriteMUInt16(&buff[len + 5], 0x304); //TLS 1.3
	WriteMUInt16(&buff[len + 7], 0x303); //TLS 1.2
	WriteMUInt16(&buff[len + 9], 0x302); //TLS 1.1
	WriteMUInt16(&buff[len + 11], 0x301); //TLS 1.0
	len += 13;

	WriteMUInt16(&buff[i], len - i - 2); //Extensions Length
	WriteMUInt16(&buff[1], len - 4);
	return len;
}

UOSInt Net::SSLUtil::GenSSLClientHello(UInt8 *buff, Text::CStringNN serverHost)
{
	UOSInt len;
	buff[0] = 22; //Handshake
	buff[1] = 3; //TLS
	buff[2] = 1; //1.0
	len = GenClientHello(&buff[5], serverHost);
	WriteMUInt16(&buff[3], len);
	return len + 5;
}
