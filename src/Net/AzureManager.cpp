#include "Stdafx.h"
#include "Crypto/Cert/X509Cert.h"
#include "Net/AzureManager.h"
#include "Text/TextBinEnc/Base64Enc.h"

Net::AzureManager::AzureManager(NotNullPtr<Net::SocketFactory> sockf, Net::SSLEngine *ssl)
{
	this->sockf = sockf;
	this->ssl = ssl;
	this->keyMap = 0;
}

Net::AzureManager::~AzureManager()
{
	if (this->keyMap)
	{
		UOSInt i = this->keyMap->GetCount();
		while (i-- > 0)
		{
			this->keyMap->GetItem(i)->Release();
		}
		DEL_CLASS(this->keyMap);
	}
}

Crypto::Cert::X509Key *Net::AzureManager::CreateKey(Text::CStringNN kid)
{
	if (this->keyMap == 0)
	{
		NEW_CLASS(this->keyMap, Data::FastStringMap<Text::String*>());
		//////////////////////////////////
	}
	Text::String *s = this->keyMap->GetC(kid);
	if (s == 0)
	{
		return 0;
	}
	Text::TextBinEnc::Base64Enc b64;
	UInt8 keyBuff[4096];
	UOSInt keySize = b64.DecodeBin(s->v, s->leng, keyBuff);
	Crypto::Cert::X509Cert cert(kid, Data::ByteArrayR(keyBuff, keySize));
	return cert.GetNewPublicKey();
}
