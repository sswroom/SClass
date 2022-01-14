#include "Stdafx.h"
#include "IO/ConfigFile.h"

IO::ConfigFile::ConfigFile()
{
	NEW_CLASS(this->cfgVals, Data::FastStringMap<Data::FastStringMap<Text::String *>*>());
}

IO::ConfigFile::~ConfigFile()
{
	Data::FastStringMap<Text::String *> *cate;
	Text::String *s;
	UOSInt i;
	UOSInt j;
	i = this->cfgVals->GetCount();
	while (i-- > 0)
	{
		cate = this->cfgVals->GetItem(i);
		j = cate->GetCount();
		while (j-- > 0)
		{
			s = cate->GetItem(j);
			SDEL_STRING(s);
		}
		DEL_CLASS(cate);
	}
	DEL_CLASS(this->cfgVals);
}

Text::String *IO::ConfigFile::GetValue(Text::String *name)
{
	return GetValue(Text::String::NewEmpty(), name);
}

Text::String *IO::ConfigFile::GetValue(const UTF8Char *name, UOSInt nameLen)
{
	return GetValue((const UTF8Char*)"", 0, name, nameLen);
}

Text::String *IO::ConfigFile::GetValue(Text::String *category, Text::String *name)
{
	Data::FastStringMap<Text::String *> *cate = this->cfgVals->Get(Text::String::OrEmpty(category));
	if (cate == 0)
	{
		return 0;
	}
	return cate->Get(name);
}

Text::String *IO::ConfigFile::GetValue(const UTF8Char *category, UOSInt categoryLen, const UTF8Char *name, UOSInt nameLen)
{
	if (category == 0)
	{
		category = (const UTF8Char*)"";
	}
	Data::FastStringMap<Text::String *> *cate = this->cfgVals->GetC(category, categoryLen);
	if (cate == 0)
	{
		return 0;
	}
	return cate->GetC(name, nameLen);
}

Bool IO::ConfigFile::SetValue(Text::String *category, Text::String *name, Text::String *value)
{
	Data::FastStringMap<Text::String *> *cate;
	Text::String *s;

	if (name == 0)
		return false;
	if (category == 0)
	{
		category = Text::String::NewEmpty();
	}
	cate = this->cfgVals->Get(category);
	if (cate == 0)
	{
		NEW_CLASS(cate, Data::FastStringMap<Text::String *>());
		this->cfgVals->Put(category, cate);
	}
	s = cate->Get(name);
	SDEL_STRING(s);
	cate->Put(name, SCOPY_STRING(value));
	return true;
}

Bool IO::ConfigFile::SetValue(const UTF8Char *category, UOSInt categoryLen, const UTF8Char *name, UOSInt nameLen, const UTF8Char *value, UOSInt valueLen)
{
	Data::FastStringMap<Text::String *> *cate;
	Text::String *s;

	if (name == 0)
		return false;
	if (category == 0)
	{
		category = (const UTF8Char*)"";
	}
	cate = this->cfgVals->GetC(category, categoryLen);
	if (cate == 0)
	{
		NEW_CLASS(cate, Data::FastStringMap<Text::String *>());
		this->cfgVals->PutC(category, categoryLen, cate);
	}
	s = cate->GetC(name, nameLen);
	SDEL_STRING(s);
	cate->PutC(name, nameLen, Text::String::New(value, valueLen));
	return true;
}


Bool IO::ConfigFile::RemoveValue(const UTF8Char *category, UOSInt categoryLen, const UTF8Char *name, UOSInt nameLen)
{
	Data::FastStringMap<Text::String *> *cate;
	Text::String *s;

	if (name == 0)
		return false;
	if (category == 0)
	{
		category = (const UTF8Char*)"";
	}
	cate = this->cfgVals->GetC(category, categoryLen);
	if (cate == 0)
	{
		NEW_CLASS(cate, Data::FastStringMap<Text::String *>());
		this->cfgVals->PutC(category, categoryLen, cate);
	}
	s = cate->RemoveC(name, nameLen);
	SDEL_STRING(s);
	return true;
}

UOSInt IO::ConfigFile::GetCateCount()
{
	return this->cfgVals->GetCount();
}

UOSInt IO::ConfigFile::GetCateList(Data::ArrayList<Text::String *> *cateList, Bool withEmpty)
{
	UOSInt retCnt = 0;
	UOSInt i = 0;
	UOSInt j = this->cfgVals->GetCount();
	cateList->EnsureCapacity(j);
	while (i < j)
	{
		Text::String *key = this->cfgVals->GetKey(i);
		if (key->leng > 0 || withEmpty)
		{
			cateList->Add(key);
			retCnt++;
		}
		i++;
	}
	return retCnt;
}

UOSInt IO::ConfigFile::GetKeys(Text::String *category, Data::ArrayList<Text::String *> *keyList)
{
	Data::FastStringMap<Text::String *> *cate;
	cate = this->cfgVals->Get(Text::String::OrEmpty(category));
	if (cate == 0)
		return 0;
	UOSInt i = 0;
	UOSInt j = cate->GetCount();
	keyList->EnsureCapacity(j);
	while (i < j)
	{
		keyList->Add(cate->GetKey(i));
		i++;
	}
	return j;
}

UOSInt IO::ConfigFile::GetKeys(const UTF8Char *category, UOSInt categoryLen, Data::ArrayList<Text::String *> *keyList)
{
	Data::FastStringMap<Text::String *> *cate;
	if (category == 0)
	{
		category = (const UTF8Char*)"";
	}
	cate = this->cfgVals->GetC(category, categoryLen);
	if (cate == 0)
		return 0;
	UOSInt i = 0;
	UOSInt j = cate->GetCount();
	keyList->EnsureCapacity(j);
	while (i < j)
	{
		keyList->Add(cate->GetKey(i));
		i++;
	}
	return j;
}

Bool IO::ConfigFile::HasCategory(const UTF8Char *category, UOSInt categoryLen)
{
	if (category == 0)
	{
		category = (const UTF8Char*)"";
	}
	return this->cfgVals->GetC(category, categoryLen) != 0;
}

IO::ConfigFile *IO::ConfigFile::CloneCate(const UTF8Char *category, UOSInt categoryLen)
{
	Data::FastStringMap<Text::String*> *cate = this->cfgVals->GetC(category, categoryLen);
	if (cate == 0)
	{
		return 0;
	}
	IO::ConfigFile *cfg;
	NEW_CLASS(cfg, IO::ConfigFile());
	UOSInt i = 0;
	UOSInt j = cate->GetCount();
	Text::String *key;
	while (i < j)
	{
		key = cate->GetItem(i);
		cfg->SetValue(0, key, cate->Get(key));
		i++;
	}
	return cfg;
}
