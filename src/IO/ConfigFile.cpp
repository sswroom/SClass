#include "Stdafx.h"
#include "IO/ConfigFile.h"

void IO::ConfigFile::MergeCate(Data::FastStringMap<Text::String *> *myCate, Data::FastStringMap<Text::String *> *cateToMerge)
{
	Text::String *name;
	Text::String *value;
	UOSInt i = 0;
	UOSInt j = cateToMerge->GetCount();
	while (i < j)
	{
		name = cateToMerge->GetKey(i);
		value = myCate->Put(name, cateToMerge->GetItem(i)->Clone());
		if (value)
		{
			value->Release();
		}
		i++;
	}
}

IO::ConfigFile::ConfigFile()
{
}

IO::ConfigFile::~ConfigFile()
{
	Data::FastStringMap<Text::String *> *cate;
	Text::String *s;
	UOSInt i;
	UOSInt j;
	i = this->cfgVals.GetCount();
	while (i-- > 0)
	{
		cate = this->cfgVals.GetItem(i);
		j = cate->GetCount();
		while (j-- > 0)
		{
			s = cate->GetItem(j);
			SDEL_STRING(s);
		}
		DEL_CLASS(cate);
	}
}

Text::String *IO::ConfigFile::GetValue(Text::String *name)
{
	return GetValue(Text::String::NewEmpty(), name);
}

Text::String *IO::ConfigFile::GetValue(Text::CString name)
{
	return GetValue(CSTR(""), name);
}

Text::String *IO::ConfigFile::GetValue(Text::String *category, Text::String *name)
{
	Data::FastStringMap<Text::String *> *cate = this->cfgVals.Get(Text::String::OrEmpty(category));
	if (cate == 0)
	{
		return 0;
	}
	return cate->Get(name);
}

Text::String *IO::ConfigFile::GetValue(Text::CString category, Text::CString name)
{
	if (category.leng == 0)
	{
		category = CSTR("");
	}
	Data::FastStringMap<Text::String *> *cate = this->cfgVals.GetC(category);
	if (cate == 0)
	{
		return 0;
	}
	return cate->GetC(name);
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
	cate = this->cfgVals.Get(category);
	if (cate == 0)
	{
		NEW_CLASS(cate, Data::FastStringMap<Text::String *>());
		this->cfgVals.Put(category, cate);
	}
	s = cate->Get(name);
	SDEL_STRING(s);
	cate->Put(name, SCOPY_STRING(value));
	return true;
}

Bool IO::ConfigFile::SetValue(Text::CString category, Text::CString name, Text::CString value)
{
	Data::FastStringMap<Text::String *> *cate;
	Text::String *s;

	if (name.v == 0)
		return false;
	if (category.v == 0)
	{
		category = CSTR("");
	}
	cate = this->cfgVals.GetC(category);
	if (cate == 0)
	{
		NEW_CLASS(cate, Data::FastStringMap<Text::String *>());
		this->cfgVals.PutC(category, cate);
	}
	s = cate->GetC(name);
	SDEL_STRING(s);
	cate->PutC(name, Text::String::New(value));
	return true;
}


Bool IO::ConfigFile::RemoveValue(Text::CString category, Text::CString name)
{
	Data::FastStringMap<Text::String *> *cate;
	Text::String *s;

	if (name.v == 0)
		return false;
	if (category.v == 0)
	{
		category = CSTR("");
	}
	cate = this->cfgVals.GetC(category);
	if (cate == 0)
	{
		NEW_CLASS(cate, Data::FastStringMap<Text::String *>());
		this->cfgVals.PutC(category, cate);
	}
	s = cate->RemoveC(name);
	SDEL_STRING(s);
	return true;
}

UOSInt IO::ConfigFile::GetCateCount()
{
	return this->cfgVals.GetCount();
}

UOSInt IO::ConfigFile::GetCateList(Data::ArrayList<Text::String *> *cateList, Bool withEmpty)
{
	UOSInt retCnt = 0;
	UOSInt i = 0;
	UOSInt j = this->cfgVals.GetCount();
	cateList->EnsureCapacity(j);
	while (i < j)
	{
		Text::String *key = this->cfgVals.GetKey(i);
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
	cate = this->cfgVals.Get(Text::String::OrEmpty(category));
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

UOSInt IO::ConfigFile::GetKeys(Text::CString category, Data::ArrayList<Text::String *> *keyList)
{
	Data::FastStringMap<Text::String *> *cate;
	if (category.v == 0)
	{
		category = CSTR("");
	}
	cate = this->cfgVals.GetC(category);
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

Bool IO::ConfigFile::HasCategory(Text::CString category)
{
	if (category.v == 0)
	{
		category = CSTR("");
	}
	return this->cfgVals.GetC(category) != 0;
}

IO::ConfigFile *IO::ConfigFile::CloneCate(Text::CString category)
{
	Data::FastStringMap<Text::String*> *cate = this->cfgVals.GetC(category);
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

void IO::ConfigFile::MergeConfig(IO::ConfigFile *cfg)
{
	UOSInt i = cfg->cfgVals.GetCount();
	Data::FastStringMap<Text::String*> *cate;
	while (i-- > 0)
	{
		cate = this->cfgVals.Get(cfg->cfgVals.GetKey(i));
		if (cate == 0)
		{
			NEW_CLASS(cate, Data::FastStringMap<Text::String*>());
			this->cfgVals.Put(cfg->cfgVals.GetKey(i), cate);
		}
		this->MergeCate(cate, cfg->cfgVals.GetItem(i));
	}
}
