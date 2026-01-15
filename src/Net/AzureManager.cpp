#include "Stdafx.h"
#include "Crypto/Cert/X509Cert.h"
#include "Net/AzureManager.h"
#include "Net/HTTPClient.h"
#include "Text/JSON.h"
#include "Text/TextBinEnc/Base64Enc.h"

Net::AzureManager::AzureManager(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl)
{
	this->clif = clif;
	this->ssl = ssl;
	this->keyMap = 0;
}

Net::AzureManager::~AzureManager()
{
	if (this->keyMap)
	{
		UIntOS i = this->keyMap->GetCount();
		while (i-- > 0)
		{
			this->keyMap->GetItemNoCheck(i)->Release();
		}
		DEL_CLASS(this->keyMap);
	}
}

Optional<Crypto::Cert::X509Key> Net::AzureManager::CreateKey(Text::CStringNN kid)
{
	if (this->keyMap == 0)
	{
		NEW_CLASS(this->keyMap, Data::FastStringMapNN<Text::String>());
		Text::StringBuilderUTF8 sb;
		if (Net::HTTPClient::LoadContent(this->clif, this->ssl, CSTR("https://login.microsoftonline.com/common/discovery/v2.0/keys"), sb, 1048576))
		{
			NN<Text::JSONBase> json;
			if (Text::JSONBase::ParseJSONStr(sb.ToCString()).SetTo(json))
			{
				NN<Text::JSONArray> keys;
				if (json->GetValueArray(CSTR("keys")).SetTo(keys))
				{
					UIntOS i = 0;
					UIntOS j = keys->GetArrayLength();
					while (i < j)
					{
						NN<Text::JSONBase> key;
						NN<Text::String> kid;
						NN<Text::String> cert;
						if (keys->GetArrayValue(i).SetTo(key) && key->GetValueString(CSTR("kid")).SetTo(kid) && key->GetValueString(CSTR("x5c[0]")).SetTo(cert))
						{
							this->keyMap->PutNN(kid, cert->Clone());
						}
						i++;
					}
				}
				json->EndUse();
			}
		}
	}
	NN<Text::String> s;
	if (!this->keyMap->GetC(kid).SetTo(s))
	{
		return nullptr;
	}
	Text::TextBinEnc::Base64Enc b64;
	UInt8 keyBuff[4096];
	UIntOS keySize = b64.DecodeBin(s->ToCString(), keyBuff);
	Crypto::Cert::X509Cert cert(kid, Data::ByteArrayR(keyBuff, keySize));
	return cert.GetNewPublicKey();
}
