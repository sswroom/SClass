#include "Stdafx.h"
#include "Crypto/Cert/X509Cert.h"
#include "Core/ByteTool_C.h"
#include "Data/RandomBytesGenerator.h"
#include "Net/SocketUtil.h"
#include "Net/SSLUtil.h"

#include <stdio.h>

Net::SSLCipherSuite Net::SSLUtil::csuitesObj[] = {
    {0x002f, UTF8STRCPTR("TLS_RSA_WITH_AES_128_CBC_SHA")},
    {0x0032, UTF8STRCPTR("TLS_DHE_DSS_WITH_AES_128_CBC_SHA")},
    {0x0033, UTF8STRCPTR("TLS_DHE_RSA_WITH_AES_128_CBC_SHA")},
    {0x0034, UTF8STRCPTR("TLS_DH_anon_WITH_AES_128_CBC_SHA")},
    {0x0035, UTF8STRCPTR("TLS_RSA_WITH_AES_256_CBC_SHA")},
    {0x0038, UTF8STRCPTR("TLS_DHE_DSS_WITH_AES_256_CBC_SHA")},
    {0x0039, UTF8STRCPTR("TLS_DHE_RSA_WITH_AES_256_CBC_SHA")},
    {0x003a, UTF8STRCPTR("TLS_DH_anon_WITH_AES_256_CBC_SHA")},
    {0x003c, UTF8STRCPTR("TLS_RSA_WITH_AES_128_CBC_SHA256")},
    {0x003d, UTF8STRCPTR("TLS_RSA_WITH_AES_256_CBC_SHA256")},
    {0x0040, UTF8STRCPTR("TLS_DHE_DSS_WITH_AES_128_CBC_SHA256")},
    {0x0041, UTF8STRCPTR("TLS_RSA_WITH_CAMELLIA_128_CBC_SHA")},
    {0x0044, UTF8STRCPTR("TLS_DHE_DSS_WITH_CAMELLIA_128_CBC_SHA")},
    {0x0045, UTF8STRCPTR("TLS_DHE_RSA_WITH_CAMELLIA_128_CBC_SHA")},
    {0x0046, UTF8STRCPTR("TLS_DH_anon_WITH_CAMELLIA_128_CBC_SHA")},
    {0x0067, UTF8STRCPTR("TLS_DHE_RSA_WITH_AES_128_CBC_SHA256")},
    {0x006a, UTF8STRCPTR("TLS_DHE_DSS_WITH_AES_256_CBC_SHA256")},
    {0x006b, UTF8STRCPTR("TLS_DHE_RSA_WITH_AES_256_CBC_SHA256")},
    {0x006c, UTF8STRCPTR("TLS_DH_anon_WITH_AES_128_CBC_SHA256")},
    {0x006d, UTF8STRCPTR("TLS_DH_anon_WITH_AES_256_CBC_SHA256")},
    {0x0084, UTF8STRCPTR("TLS_RSA_WITH_CAMELLIA_256_CBC_SHA")},
    {0x0087, UTF8STRCPTR("TLS_DHE_DSS_WITH_CAMELLIA_256_CBC_SHA")},
    {0x0088, UTF8STRCPTR("TLS_DHE_RSA_WITH_CAMELLIA_256_CBC_SHA")},
    {0x0089, UTF8STRCPTR("TLS_DH_anon_WITH_CAMELLIA_256_CBC_SHA")},
    {0x0096, UTF8STRCPTR("TLS_RSA_WITH_SEED_CBC_SHA")},
    {0x0099, UTF8STRCPTR("TLS_DHE_DSS_WITH_SEED_CBC_SHA")},
    {0x009a, UTF8STRCPTR("TLS_DHE_RSA_WITH_SEED_CBC_SHA")},
    {0x009b, UTF8STRCPTR("TLS_DH_anon_WITH_SEED_CBC_SHA")},
    {0x009c, UTF8STRCPTR("TLS_RSA_WITH_AES_128_GCM_SHA256")},
    {0x009d, UTF8STRCPTR("TLS_RSA_WITH_AES_256_GCM_SHA384")},
    {0x009e, UTF8STRCPTR("TLS_DHE_RSA_WITH_AES_128_GCM_SHA256")},
    {0x009f, UTF8STRCPTR("TLS_DHE_RSA_WITH_AES_256_GCM_SHA384")},
    {0x00a2, UTF8STRCPTR("TLS_DHE_DSS_WITH_AES_128_GCM_SHA256")},
    {0x00a3, UTF8STRCPTR("TLS_DHE_DSS_WITH_AES_256_GCM_SHA384")},
    {0x00a6, UTF8STRCPTR("TLS_DH_anon_WITH_AES_128_GCM_SHA256")},
    {0x00a7, UTF8STRCPTR("TLS_DH_anon_WITH_AES_256_GCM_SHA384")},
    {0x00ba, UTF8STRCPTR("TLS_RSA_WITH_CAMELLIA_128_CBC_SHA256")},
    {0x00bd, UTF8STRCPTR("TLS_DHE_DSS_WITH_CAMELLIA_128_CBC_SHA256")},
    {0x00be, UTF8STRCPTR("TLS_DHE_RSA_WITH_CAMELLIA_128_CBC_SHA256")},
    {0x00bf, UTF8STRCPTR("TLS_DH_anon_WITH_CAMELLIA_128_CBC_SHA256")},
    {0x00c0, UTF8STRCPTR("TLS_RSA_WITH_CAMELLIA_256_CBC_SHA256")},
    {0x00c3, UTF8STRCPTR("TLS_DHE_DSS_WITH_CAMELLIA_256_CBC_SHA256")},
    {0x00c4, UTF8STRCPTR("TLS_DHE_RSA_WITH_CAMELLIA_256_CBC_SHA256")},
    {0x00c5, UTF8STRCPTR("TLS_DH_anon_WITH_CAMELLIA_256_CBC_SHA256")},
    {0x00ff, UTF8STRCPTR("TLS_EMPTY_RENEGOTIATION_INFO_SCSV")},
	{0x1301, UTF8STRCPTR("TLS_AES_128_GCM_SHA256")},
	{0x1302, UTF8STRCPTR("TLS_AES_256_GCM_SHA384")},
	{0x1303, UTF8STRCPTR("TLS_CHACHA20_POLY1305_SHA256")},
    {0xc009, UTF8STRCPTR("TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA")},
    {0xc00a, UTF8STRCPTR("TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA")},
    {0xc013, UTF8STRCPTR("TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA")},
    {0xc014, UTF8STRCPTR("TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA")},
    {0xc018, UTF8STRCPTR("TLS_ECDH_anon_WITH_AES_128_CBC_SHA")},
    {0xc019, UTF8STRCPTR("TLS_ECDH_anon_WITH_AES_256_CBC_SHA")},
    {0xc023, UTF8STRCPTR("TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256")},
    {0xc024, UTF8STRCPTR("TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA384")},
    {0xc027, UTF8STRCPTR("TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA256")},
    {0xc028, UTF8STRCPTR("TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA384")},
    {0xc02b, UTF8STRCPTR("TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256")},
	{0xc02c, UTF8STRCPTR("TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384")},
    {0xc02f, UTF8STRCPTR("TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256")},
    {0xc030, UTF8STRCPTR("TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384")},
    {0xc050, UTF8STRCPTR("TLS_RSA_WITH_ARIA_128_GCM_SHA256")},
    {0xc051, UTF8STRCPTR("TLS_RSA_WITH_ARIA_256_GCM_SHA384")},
    {0xc052, UTF8STRCPTR("TLS_DHE_RSA_WITH_ARIA_128_GCM_SHA256")},
    {0xc053, UTF8STRCPTR("TLS_DHE_RSA_WITH_ARIA_256_GCM_SHA384")},
    {0xc056, UTF8STRCPTR("TLS_DHE_DSS_WITH_ARIA_128_GCM_SHA256")},
    {0xc057, UTF8STRCPTR("TLS_DHE_DSS_WITH_ARIA_256_GCM_SHA384")},
    {0xc05c, UTF8STRCPTR("TLS_ECDHE_ECDSA_WITH_ARIA_128_GCM_SHA256")},
    {0xc05d, UTF8STRCPTR("TLS_ECDHE_ECDSA_WITH_ARIA_256_GCM_SHA384")},
    {0xc060, UTF8STRCPTR("TLS_ECDHE_RSA_WITH_ARIA_128_GCM_SHA256")},
    {0xc061, UTF8STRCPTR("TLS_ECDHE_RSA_WITH_ARIA_256_GCM_SHA384")},
    {0xc072, UTF8STRCPTR("TLS_ECDHE_ECDSA_WITH_CAMELLIA_128_CBC_SHA256")},
    {0xc073, UTF8STRCPTR("TLS_ECDHE_ECDSA_WITH_CAMELLIA_256_CBC_SHA384")},
    {0xc076, UTF8STRCPTR("TLS_ECDHE_RSA_WITH_CAMELLIA_128_CBC_SHA256")},
    {0xc077, UTF8STRCPTR("TLS_ECDHE_RSA_WITH_CAMELLIA_256_CBC_SHA384")},
    {0xc09c, UTF8STRCPTR("TLS_RSA_WITH_AES_128_CCM")},
    {0xc09d, UTF8STRCPTR("TLS_RSA_WITH_AES_256_CCM")},
    {0xc09e, UTF8STRCPTR("TLS_DHE_RSA_WITH_AES_128_CCM")},
    {0xc09f, UTF8STRCPTR("TLS_DHE_RSA_WITH_AES_256_CCM")},
    {0xc0a0, UTF8STRCPTR("TLS_RSA_WITH_AES_128_CCM_8")},
    {0xc0a1, UTF8STRCPTR("TLS_RSA_WITH_AES_256_CCM_8")},
    {0xc0a2, UTF8STRCPTR("TLS_DHE_RSA_WITH_AES_128_CCM_8")},
    {0xc0a3, UTF8STRCPTR("TLS_DHE_RSA_WITH_AES_256_CCM_8")},
    {0xc0ac, UTF8STRCPTR("TLS_ECDHE_ECDSA_WITH_AES_128_CCM")},
    {0xc0ad, UTF8STRCPTR("TLS_ECDHE_ECDSA_WITH_AES_256_CCM")},
    {0xc0ae, UTF8STRCPTR("TLS_ECDHE_ECDSA_WITH_AES_128_CCM_8")},
    {0xc0af, UTF8STRCPTR("TLS_ECDHE_ECDSA_WITH_AES_256_CCM_8")},
    {0xcca8, UTF8STRCPTR("TLS_ECDHE_RSA_WITH_CHACHA20_POLY1305_SHA256")},
    {0xcca9, UTF8STRCPTR("TLS_ECDHE_ECDSA_WITH_CHACHA20_POLY1305_SHA256")},
    {0xccaa, UTF8STRCPTR("TLS_DHE_RSA_WITH_CHACHA20_POLY1305_SHA256")},
};

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

UOSInt Net::SSLUtil::GenClientHello(UInt8 *buff, Text::CStringNN serverHost, SSLVer ver)
{
	Data::RandomBytesGenerator randGen;
	UOSInt len;
	buff[0] = 1; //Client Hello
	WriteMUInt16(&buff[4], (UInt16)ver);
	randGen.NextBytes(&buff[6], 32); //Random
	buff[38] = 32; //Session ID Length
	randGen.NextBytes(&buff[39], 32); //Session ID
	WriteMUInt16(&buff[71], sizeof(csuites));
	UOSInt i = 0;
	UOSInt j = sizeof(csuites) >> 1;
	while (i < j)
	{
		WriteMUInt16(&buff[73 + i * 2], csuites[i]);
		i++;
	}
	len = 73 + j * 2;
	buff[len] = 1; //Compression Method Length
	buff[len + 1] = 0; //Compression Method: Null
	i = len + 2; //Extension
	len = i + 2;
	WriteMUInt16(&buff[len], 0); //Extension type: server_name
	WriteMUInt16(&buff[len + 2], serverHost.leng + 5); //Extension length
	WriteMUInt16(&buff[len + 4], serverHost.leng + 3); //Server name list length
	buff[len + 6] = 0; //Server name Type: host_name
	WriteMUInt16(&buff[len + 7], serverHost.leng); //Server name length
	MemCopyNOShort(&buff[len + 9], serverHost.v.Ptr(), serverHost.leng); //Server name
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

/*	WriteMUInt16(&buff[len], 43); //Extension type: supported_versions
	WriteMUInt16(&buff[len + 2], 9); //Extension length
	buff[len + 4] = 8; //Supported versions length
	WriteMUInt16(&buff[len + 5], 0x304); //TLS 1.3
	WriteMUInt16(&buff[len + 7], 0x303); //TLS 1.2
	WriteMUInt16(&buff[len + 9], 0x302); //TLS 1.1
	WriteMUInt16(&buff[len + 11], 0x301); //TLS 1.0
	len += 13;*/

	WriteMUInt16(&buff[i], len - i - 2); //Extensions Length
	WriteMUInt32(&buff[0], len - 4);
	buff[0] = 1; //Client Hello
	return len;
}

UOSInt Net::SSLUtil::GenSSLClientHello(UInt8 *buff, Text::CStringNN serverHost, SSLVer ver)
{
	UOSInt len;
	buff[0] = 22; //Handshake
	WriteMUInt16(&buff[1], (UInt16)ver);
	len = GenClientHello(&buff[5], serverHost, ver);
	WriteMUInt16(&buff[3], len);
	return len + 5;
}

void Net::SSLUtil::ParseResponse(const UInt8 *buff, UOSInt packetSize, NN<Text::StringBuilderUTF8> sb, OutParam<Optional<Crypto::Cert::X509File>> cert)
{
	cert.Set(nullptr);
	if (buff[0] == 21 && packetSize == 7)
	{
		sb->AppendC(UTF8STRC("Server Alert: Level = "));
		sb->Append(AlertLevelGetName(buff[5]));
		sb->AppendUTF8Char('(');
		sb->AppendU16(buff[5]);
		sb->AppendUTF8Char(')');
		sb->AppendC(UTF8STRC(", Desc: "));
		sb->Append(AlertDescGetName(buff[6]));
		sb->AppendC(UTF8STRC(" ("));
		sb->AppendU16(buff[6]);
		sb->AppendUTF8Char(')');
	}
	else if (buff[0] == 22 && packetSize >= 9)
	{
		Data::ArrayListNN<Crypto::Cert::X509Cert> certs;
		NN<Crypto::Cert::X509Cert> c;
		Bool hasServerHello = false;
		UOSInt i = 0;
		UOSInt j;
		UInt32 hsLeng;
		while (i < packetSize - 8)
		{
			if (buff[i] != 22)
			{
				sb->AppendC(UTF8STRC("Found non handshake packet ("));
				sb->AppendU16(buff[i]);
				sb->AppendUTF8Char(')');
				return;
			}
			hsLeng = ReadMUInt16(&buff[i + 3]);
			j = 0;
			while (j < hsLeng - 3)
			{
				if (buff[i + j + 5] == 2)
				{
					if (hasServerHello)
					{
						sb->AppendC(UTF8STRC("Found duplicate server hello"));
						if (certs.GetCount() > 0)
						{
							cert.Set(Crypto::Cert::X509File::CreateFromCertsAndClear(certs));
						}
						return;
					}
					hasServerHello = true;
				}
				else if (!hasServerHello)
				{
					sb->AppendC(UTF8STRC("Server Hello not found"));
					return;
				}
				if (buff[i + j + 5] == 11) //
				{
					UInt32 certsLeng = ReadMUInt24(&buff[i + j + 9]);
					UInt32 certLeng;
					UOSInt k = 0;
					while (k < certsLeng - 3)
					{
						certLeng = ReadMUInt24(&buff[i + j + k + 12]);
						NEW_CLASSNN(c, Crypto::Cert::X509Cert(CSTR("Cert.crt"), Data::ByteArrayR(&buff[i + j + k + 15], certLeng)));
						c->SetDefaultSourceName();
						certs.Add(c);
						k += certLeng + 3;
					}
				}
				else if (buff[i + j + 5] == 14)
				{
					sb->AppendC(UTF8STRC("Success"));
					if (certs.GetCount() > 0)
					{
						cert.Set(Crypto::Cert::X509File::CreateFromCertsAndClear(certs));
					}
					return;
				}
				j += ReadMUInt24(&buff[i + j + 6]) + 4;
			}
			i += hsLeng + 5;
		}
		sb->AppendC(UTF8STRC("Server hello end not found"));
		if (certs.GetCount() > 0)
		{
			cert.Set(Crypto::Cert::X509File::CreateFromCertsAndClear(certs));
		}
		return;		
	}
	else
	{
		sb->AppendC(UTF8STRC("Unknown server response, type = "));
		sb->AppendU16(buff[0]);
	}
}

Bool Net::SSLUtil::IncompleteHandshake(const UInt8 *buff, UOSInt packetSize)
{
	Bool hasServerHello = false;
	UOSInt i = 0;
	UOSInt len;
	UOSInt j = 0;
	while (i < packetSize - 8)
	{
		if (buff[i] != 22)
			return false;
		len = ReadMUInt16(&buff[i + 3]);
		j = 0;
		while (j < len - 3)
		{
			if (buff[i + j + 5] == 2)
			{
				if (hasServerHello)
					return false;
				hasServerHello = true;
			}
			else if (!hasServerHello)
			{
				return false;
			}
			if (buff[i + j + 5] == 14)
			{
				return false;
			}
			j += ReadMUInt24(&buff[i + j + 6]) + 4;
		}
		i += len + 5;
	}
	return true;
}

Text::CStringNN Net::SSLUtil::AlertLevelGetName(UInt8 level)
{
	switch (level)
	{
	case 1:
		return CSTR("Warning");
	case 2:
		return CSTR("Fatel");
	default:
		printf("Unknown AlertLevel %x\r\n", level);
		return CSTR("Unknown");
	}
}

Text::CStringNN Net::SSLUtil::AlertDescGetName(UInt8 desc)
{
	switch (desc)
	{
	case 0:
		return CSTR("SSL3_AD_CLOSE_NOTIFY");
	case 10:
		return CSTR("SSL3_AD_UNEXPECTED_MESSAGE");
	case 20:
		return CSTR("SSL3_AD_BAD_RECORD_MAC");
	case 21:
		return CSTR("TLS1_AD_DECRYPTION_FAILED");
	case 22:
		return CSTR("TLS1_AD_RECORD_OVERFLOW");
	case 30:
		return CSTR("SSL3_AD_DECOMPRESSION_FAILURE");
	case 40:
		return CSTR("SSL3_AD_HANDSHAKE_FAILURE");
	case 41:
		return CSTR("SSL3_AD_NO_CERTIFICATE");
	case 42:
		return CSTR("SSL3_AD_BAD_CERTIFICATE");
	case 43:
		return CSTR("SSL3_AD_UNSUPPORTED_CERTIFICATE");
	case 44:
		return CSTR("SSL3_AD_CERTIFICATE_REVOKED");
	case 45:
		return CSTR("SSL3_AD_CERTIFICATE_EXPIRED");
	case 46:
		return CSTR("SSL3_AD_CERTIFICATE_UNKNOWN");
	case 47:
		return CSTR("SSL3_AD_ILLEGAL_PARAMETER");
	case 48:
		return CSTR("TLS1_AD_UNKNOWN_CA");
	case 49:
		return CSTR("TLS1_AD_ACCESS_DENIED");
	case 50:
		return CSTR("TLS1_AD_DECODE_ERROR");
	case 51:
		return CSTR("TLS1_AD_DECRYPT_ERROR");
	case 60:
		return CSTR("TLS1_AD_EXPORT_RESTRICTION");
	case 70:
		return CSTR("TLS1_AD_PROTOCOL_VERSION");
	case 71:
		return CSTR("TLS1_AD_INSUFFICIENT_SECURITY");
	case 80:
		return CSTR("TLS1_AD_INTERNAL_ERROR");
	case 86:
		return CSTR("Inappropriate fallback");
	case 90:
		return CSTR("TLS1_AD_USER_CANCELLED");
	case 100:
		return CSTR("TLS1_AD_NO_RENEGOTIATION");
	case 109:
		return CSTR("Missing extension");
	case 110:
		return CSTR("TLS1_AD_UNSUPPORTED_EXTENSION");
	case 111:
		return CSTR("TLS1_AD_CERTIFICATE_UNOBTAINABLE");
	case 112:
		return CSTR("TLS1_AD_UNRECOGNIZED_NAME");
	case 113:
		return CSTR("TLS1_AD_BAD_CERTIFICATE_STATUS_RESPONSE");
	case 114:
		return CSTR("TLS1_AD_BAD_CERTIFICATE_HASH_VALUE");
	case 115:
		return CSTR("TLS1_AD_UNKNOWN_PSK_IDENTITY");
	case 116:
		return CSTR("Certificate required");
	case 120:
		return CSTR("No application protocol");
	default:
		printf("Unknown AlertDesc %d\r\n", desc);
		return CSTR("Unknown");
	}

}

Text::CStringNN Net::SSLUtil::RecordTypeGetName(UInt8 recordType)
{
	switch (recordType)
	{
	case 20:
		return CSTR("SSL3_RT_CHANGE_CIPHER_SPEC");
	case 21:
		return CSTR("SSL3_RT_ALERT");
	case 22:
		return CSTR("SSL3_RT_HANDSHAKE");
	case 23:
		return CSTR("SSL3_RT_APPLICATION_DATA");
	case 24:
		return CSTR("TLS1_RT_HEARTBEAT");
	default:
		printf("Unknown RecordType %x\r\n", recordType);
		return CSTR("Unknown");
	}
}

Text::CStringNN Net::SSLUtil::HandshakeTypeGetName(UInt8 hsType)
{
	switch (hsType)
	{
	case 0:
		return CSTR("SSL3_MT_HELLO_REQUEST");
	case 1:
		return CSTR("SSL3_MT_CLIENT_HELLO");
	case 2:
		return CSTR("SSL3_MT_SERVER_HELLO");
	case 4:
		return CSTR("SSL3_MT_NEWSESSION_TICKET");
	case 11:
		return CSTR("SSL3_MT_CERTIFICATE");
	case 12:
		return CSTR("SSL3_MT_SERVER_KEY_EXCHANGE");
	case 13:
		return CSTR("SSL3_MT_CERTIFICATE_REQUEST");
	case 14:
		return CSTR("SSL3_MT_SERVER_DONE");
	case 15:
		return CSTR("SSL3_MT_CERTIFICATE_VERIFY");
	case 16:
		return CSTR("SSL3_MT_CLIENT_KEY_EXCHANGE");
	case 20:
		return CSTR("SSL3_MT_FINISHED");
	default:
		printf("Unknown HandshakeType %x\r\n", hsType);
		return CSTR("Unknown");
	}
}

Text::CStringNN Net::SSLUtil::CompressionMethodGetName(UInt8 method)
{
	switch (method)
	{
	case 0:
		return CSTR("null");
	case 1:
		return CSTR("DEFLATE");
	default:
		printf("Unknown CompressionMethod %x\r\n", method);
		return CSTR("Unknown");
	}
}

Text::CStringNN Net::SSLUtil::ExtensionTypeGetName(UInt16 extType)
{
	switch (extType)
	{
	case 0x0:
		return CSTR("server_name");
	case 0xa:
		return CSTR("supported_groups");
	case 0xb:
		return CSTR("ec_point_formats");
	case 0xd:
		return CSTR("signature_algorithms");
	case 0x23:
		return CSTR("session_ticket");
	case 0x2b:
		return CSTR("supported_versions");
	case 0x33:
		return CSTR("key_share");
	case 0xff01:
		return CSTR("renegotiation_info");
	default:
		printf("Unknown ExtensionType %x\r\n", extType);
		return CSTR("Unknown");
	}
}

Text::CStringNN Net::SSLUtil::ECPointFormatGetName(UInt8 fmt)
{
	switch (fmt)
	{
	case 0:
		return CSTR("uncompressed");
	case 1:
		return CSTR("ansiX962_compressed_prime");
	case 2:
		return CSTR("ansiX962_compressed_char2");
	default:
		printf("Unknown ECPointFormat %x\r\n", fmt);
		return CSTR("Unknown");
	}
}

Net::SSLCipherSuite *Net::SSLUtil::CipherSuiteGet(UInt16 cipherSuite)
{
	OSInt i = 0;
	OSInt j = (sizeof(csuitesObj) / sizeof(csuitesObj[0])) - 1;
	OSInt k;
	Net::SSLCipherSuite *suite;
	while (i <= j)
	{
		k = (i + j) >> 1;
		suite = &csuitesObj[k];
		if (suite->id > cipherSuite)
			j = k - 1;
		else if (suite->id < cipherSuite)
			i = k + 1;
		else
			return suite;
	}
	printf("Unknown Cipher Suite %x\r\n", cipherSuite);
	return 0;
}

Text::CStringNN Net::SSLUtil::CipherSuiteGetName(UInt16 cipherSuite)
{
	SSLCipherSuite *cipher = CipherSuiteGet(cipherSuite);
	if (cipher)
		return Text::CStringNN(cipher->name, cipher->nameLen);
	return CSTR("Unknown");
}

Text::CStringNN Net::SSLVerGetName(UInt16 ver)
{
	switch ((Net::SSLVer)ver)
	{
	case Net::SSLVer::SSL3_0:
		return CSTR("SSL 3.0");
	case Net::SSLVer::TLS1_0:
		return CSTR("TLS 1.0");
	case Net::SSLVer::TLS1_1:
		return CSTR("TLS 1.1");
	case Net::SSLVer::TLS1_2:
		return CSTR("TLS 1.2");
	case Net::SSLVer::TLS1_3:
		return CSTR("TLS 1.3");
	default:
		printf("Unknown SSL Version %x\r\n", ver);
		return CSTR("Unknown");
	}
}
