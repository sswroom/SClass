#include "Stdafx.h"
#include "Crypto/Encrypt/JasyptConfigFile.h"
#include "Text/StringTool.h"

Crypto::Encrypt::JasyptConfigFile::JasyptConfigFile(IO::ConfigFile *cfg, Crypto::Encrypt::JasyptEncryptor::KeyAlgorithm keyAlg, Crypto::Encrypt::JasyptEncryptor::CipherAlgorithm cipherAlg, const UInt8 *key, UOSInt keyLen) : enc(keyAlg, cipherAlg, key, keyLen)
{
	this->cfg = cfg;
}

Crypto::Encrypt::JasyptConfigFile::~JasyptConfigFile()
{
	UOSInt i;
	UOSInt j;
	Data::FastStringMap<Text::String *> *cate;
	i = this->decVals.GetCount();
	while (i-- > 0)
	{
		cate = this->decVals.GetItem(i);
		j = cate->GetCount();
		while (j-- > 0)
		{
			cate->GetItem(j)->Release();
		}
		DEL_CLASS(cate);
	}
	DEL_CLASS(this->cfg);
}

Text::String *Crypto::Encrypt::JasyptConfigFile::GetCateValue(Text::String *category, NotNullPtr<Text::String> name)
{
	return GetCateValue(Text::String::OrEmpty(category), name);
}

Text::String *Crypto::Encrypt::JasyptConfigFile::GetCateValue(NotNullPtr<Text::String> category, NotNullPtr<Text::String> name)
{
	UTF8Char sbuff[512];
	Text::String *s;
	Data::FastStringMap<Text::String*> *cate = this->decVals.GetNN(category);
	if (cate)
	{
		s = cate->GetNN(name);
		if (s)
			return s;
	}
	s = this->cfg->GetCateValue(category, name);
	if (s && s->StartsWith(UTF8STRC("ENC(")) && s->EndsWith(')') && s->leng < 512)
	{
		UOSInt leng = this->enc.DecryptB64(Text::CString(&s->v[4], s->leng - 5), sbuff);
		if (Text::StringTool::IsASCIIText(Data::ByteArrayR(sbuff, leng)))
		{
			cate = this->decVals.GetNN(category);
			if (cate == 0)
			{
				NEW_CLASS(cate, Data::FastStringMap<Text::String*>());
				this->decVals.PutNN(category, cate);
			}
			s = Text::String::New(sbuff, leng).Ptr();
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

Text::String *Crypto::Encrypt::JasyptConfigFile::GetCateValue(Text::CStringNN category, Text::CString name)
{
	UTF8Char sbuff[512];
	Text::String *s;
	Data::FastStringMap<Text::String*> *cate = this->decVals.GetC(category);
	if (cate)
	{
		s = cate->GetC(name.OrEmpty());
		if (s)
			return s;
	}
	s = this->cfg->GetCateValue(category, name);
	if (s && s->StartsWith(UTF8STRC("ENC(")) && s->EndsWith(')'))
	{
		UOSInt leng = this->enc.DecryptB64(Text::CString(&s->v[4], s->leng - 5), sbuff);
		if (Text::StringTool::IsASCIIText(Data::ByteArrayR(sbuff, leng)))
		{
			cate = this->decVals.GetC(category);
			if (cate == 0)
			{
				NEW_CLASS(cate, Data::FastStringMap<Text::String*>());
				this->decVals.PutC(category, cate);
			}
			s = Text::String::New(sbuff, leng).Ptr();
			cate->PutC(name.OrEmpty(), s);
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
