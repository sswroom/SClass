#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Net/OSSocketFactory.h"
#include "Net/SSLEngineFactory.h"
#include "Parser/FileParser/X509Parser.h"

#include <stdio.h>

const Char key[] = 
	"-----BEGIN RSA PRIVATE KEY-----\r\n"
	"MIIEogIBAAKCAQEAxhafb2U3LBi+PriYlaUaau2QgXs9JyVc7xozqeYqYpvc32kM\r\n"
	"lQMpG+HuKbyAaW1CuqPAw1sB4hHTSAOH5Lgj2AA/oP+BYzEX5ViuCvSG6dIh84OX\r\n"
	"o7qIzS+EjS6teO8B6X0FumW8DzUXD42mJDGiy9cHki6P6PfaRVJpew5O1N2LAXCu\r\n"
	"TIcFLop/Ip3nNIPTD/ptseUbwjfo+3M0+1Pn3C7aJNl0UPLbCirM+U97+wIQgFo5\r\n"
	"KA2i++MEnk7fpxlUJoM1hUeCgz0CAQ8u8pewrwLBaGs2a4AjZ146lyxPn5MP/9tT\r\n"
	"ZPIQz4JVVnGIYWo7qY5iOyY9o1pZaMtQUT40cwIDAQABAoIBAG+WEt43kCrhDVpb\r\n"
	"tmimBwmxJbQq7jn9DCsNAYl1xO6jinq3vIS8sQ2p6fXM4DGDgRGOaWoi/B6TmAd7\r\n"
	"JbQsHtTw3uccx5BO+X0SxYgaU12oJ8P7nxJ2Y9XiLLMzVjlHujYDXZSw9o1ChOkY\r\n"
	"DD1bUK+vQ+6PGDZOWMCwOgc9+h5iaI1ZTMFjVObLUeZCDNbY03D7ZHrWiEdFRbVo\r\n"
	"NY9O/c1MHxV6lSR5eOcFzd80/ykIIz1cuRJOva9f5pI25kaLLRqkOFhFVC+dnw7B\r\n"
	"FC8w9ATdC7cgOEfn0bFbOwW/0VDf8upfNUSNfggvPt9HCfrgSA/iEACRgZOzXsOQ\r\n"
	"GXmlZOECgYEA6+/zHRDQfIyhryer5FwqHiJ1CcM9+i/Bdj4OcrQ32cbUjmtJC8zh\r\n"
	"ENVLBPc+s1Wvjhcd8cDqrV0USQ+dx7kRYfGJ4ohFqtWCni2yfBKJj5L+KN46aJZT\r\n"
	"8kE7aH9h5wYUs3ZNOD7OGNafcEeKK7cky0KTWRVB0bSNpJ1QF/mkOMkCgYEA1u7A\r\n"
	"MPPtvhTA6GDEzkp2nufmBDWxzV0O1EzvE8zzjRNwe52mItGo7xRjmIr0y2CxDLK2\r\n"
	"IAzzVbcC1lY4/PyuGBVREFUhYF/mI//2Zcq0HSh+BmtY7g/5hN2Kn9OQuMhqhFoN\r\n"
	"69va6zciK3OTi6N5TRBiZiVzrtcekbb6Q0zVXVsCgYBTDpp4DfZHEj7M6VtSvbs5\r\n"
	"9olbEXdc/rjNRlVRve23ZY5oQI5rcAl/IocRTXSuLyjpDWxGMdKriwvcGHpD1/ut\r\n"
	"jhkrPSvBUv5zbsTZdrEkcOu2OV04eks9Cge+5rDxYiuhbbELbtig19Uy8oxKjWOP\r\n"
	"hnVWs+wCxdvjKloKxqAwOQKBgBm+m1LBxRZhkgHDpY84KCE8kbarX9LDdMVFOdXb\r\n"
	"VMjvLSgBaCb/igapIIzjCBNcT5i/x0TriDIaK8LUrRrQgWHtG7qjF85dbkmdnTvw\r\n"
	"HcboVMAcF5jcXPznNHWUd99KUkfsKYK44O4Puo4N1/x/5kWu0rp9fe/RYJ3/ltZ8\r\n"
	"24nFAoGAW/SgXpvI7fiWSNgXA54Z0Lqt7UFumyd6rwYjrdE7SlLHHDnW/CEQHENJ\r\n"
	"sbAV2XEWfyrNEo9Zm7JsGLmYwVXPJHAtnIpyVo+6CYICOwZnYKC1YtMOVrTvym0T\r\n"
	"G3d2Pvh+Go3G2tJocuO5QnQ73BRsmF7DEUlirgFav5tP3uWZ48M=\r\n"
	"-----END RSA PRIVATE KEY-----\r\n";

Int32 MyMain(Core::IProgControl *progCtrl)
{
	Int32 ret = 1;
	UInt8 signData[2048];
	UOSInt signLen;
	Net::OSSocketFactory sockf(true);
	Net::SSLEngine *ssl = Net::SSLEngineFactory::Create(&sockf, true);
	Crypto::Cert::X509File *x509 = 0;
	if (ssl)
	{
		Text::String *fileName = Text::String::New(UTF8STRC("Temp.key"));
		x509 = Parser::FileParser::X509Parser::ParseBuff((const UInt8*)key, sizeof(key) - 1, fileName);
		fileName->Release();
	}
	if (x509 && x509->GetFileType() == Crypto::Cert::X509File::FileType::Key)
	{
		Crypto::Cert::X509Key *key = (Crypto::Cert::X509Key*)x509;
		if (!ssl->Signature(key, Crypto::Hash::HashType::SHA256, UTF8STRC("123456"), signData, &signLen))
		{
			printf("Error in generating signature\r\n");
		}
		else
		{
			if (ssl->SignatureVerify(key, Crypto::Hash::HashType::SHA256, UTF8STRC("123456"), signData, signLen))
			{
				printf("Signature verify success\r\n");
				ret = 0;
			}
			else
			{
				printf("Error in verifying signature, len = %d\r\n", (UInt32)signLen);
			}
		}
	}
	else
	{
		printf("Error in parsing private key\r\n");
	}
	SDEL_CLASS(x509);
	SDEL_CLASS(ssl);
	return ret;
}
