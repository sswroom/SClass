#include "Stdafx.h"
#include "Crypto/Cert/X509Cert.h"
#include "Net/AzureManager.h"
#include "Net/HTTPClient.h"
#include "Text/JSON.h"
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
		Text::StringBuilderUTF8 sb;
		if (Net::HTTPClient::LoadContent(this->sockf, this->ssl, CSTR("https://login.microsoftonline.com/common/discovery/v2.0/keys"), sb, 1048576))
		{
			Text::JSONBase *json = Text::JSONBase::ParseJSONStr(sb.ToCString());
			if (json)
			{
				Text::JSONArray *keys = json->GetValueArray(CSTR("keys"));
				if (keys)
				{
					UOSInt i = 0;
					UOSInt j = keys->GetArrayLength();
					while (i < j)
					{
						Text::JSONBase *key = keys->GetArrayValue(i);
						NotNullPtr<Text::String> kid;
						Text::String *cert = key->GetValueString(CSTR("x5c[0]"));
						if (kid.Set(key->GetValueString(CSTR("kid"))) && cert != 0)
						{
							this->keyMap->PutNN(kid, cert->Clone().Ptr());
						}
						i++;
					}
				}
				json->EndUse();
			}
		}
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