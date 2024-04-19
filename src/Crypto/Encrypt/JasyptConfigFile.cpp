#include "Stdafx.h"
#include "Crypto/Encrypt/JasyptConfigFile.h"
#include "Text/StringTool.h"

Crypto::Encrypt::JasyptConfigFile::JasyptConfigFile(NotNullPtr<IO::ConfigFile> cfg, Crypto::Encrypt::JasyptEncryptor::KeyAlgorithm keyAlg, Crypto::Encrypt::JasyptEncryptor::CipherAlgorithm cipherAlg, Data::ByteArrayR key) : enc(keyAlg, cipherAlg, key)
{
	this->cfg = cfg;
}

Crypto::Encrypt::JasyptConfigFile::~JasyptConfigFile()
{
	UOSInt i;
	UOSInt j;
	NotNullPtr<Data::FastStringMapNN<Text::String>> cate;
	i = this->decVals.GetCount();
	while (i-- > 0)
	{
		cate = this->decVals.GetItemNoCheck(i);
		j = cate->GetCount();
		while (j-- > 0)
		{
			cate->GetItemNoCheck(j)->Release();
		}
		cate.Delete();
	}
	this->cfg.Delete();
}

Optional<Text::String> Crypto::Encrypt::JasyptConfigFile::GetCateValue(NotNullPtr<Text::String> category, NotNullPtr<Text::String> name)
{
	UTF8Char sbuff[512];
	NotNullPtr<Text::String> s;
	NotNullPtr<Data::FastStringMapNN<Text::String>> cate;
	if (this->decVals.GetNN(category).SetTo(cate))
	{
		if (cate->GetNN(name).SetTo(s))
			return s;
	}
	if (!this->cfg->GetCateValue(category, name).SetTo(s))
		return 0;
	if (s->StartsWith(UTF8STRC("ENC(")) && s->EndsWith(')') && s->leng < 512)
	{
		UOSInt leng = this->enc.DecryptB64(Text::CString(&s->v[4], s->leng - 5), sbuff);
		if (Text::StringTool::IsASCIIText(Data::ByteArrayR(sbuff, leng)))
		{
			if (!this->decVals.GetNN(category).SetTo(cate))
			{
				NEW_CLASSNN(cate, Data::FastStringMapNN<Text::String>());
				this->decVals.PutNN(category, cate);
			}
			s = Text::String::New(sbuff, leng);
			cate->PutNN(name, s);
			return s;
		}
		else
		{
			return s;
		}
	}
	else
	{
		return s;
	}
}

Optional<Text::String> Crypto::Encrypt::JasyptConfigFile::GetCateValue(Text::CStringNN category, Text::CStringNN name)
{
	UTF8Char sbuff[512];
	NotNullPtr<Text::String> s;
	NotNullPtr<Data::FastStringMapNN<Text::String>> cate;
	if (this->decVals.GetC(category).SetTo(cate))
	{
		if (cate->GetC(name).SetTo(s))
			return s;
	}
	if (!this->cfg->GetCateValue(category, name).SetTo(s))
		return 0;
	if (s->StartsWith(UTF8STRC("ENC(")) && s->EndsWith(')'))
	{
		UOSInt leng = this->enc.DecryptB64(Text::CString(&s->v[4], s->leng - 5), sbuff);
		if (Text::StringTool::IsASCIIText(Data::ByteArrayR(sbuff, leng)))
		{
			if (!this->decVals.GetC(category).SetTo(cate))
			{
				NEW_CLASSNN(cate, Data::FastStringMapNN<Text::String>());
				this->decVals.PutC(category, cate);
			}
			s = Text::String::New(sbuff, leng);
			cate->PutC(name, s);
			return s;
		}
		else
		{
			return s;
		}

	}
	else
	{
		return s;
	}
}
