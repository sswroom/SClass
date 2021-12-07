#include "Stdafx.h"
#include "IO/ConfigFile.h"

IO::ConfigFile::ConfigFile()
{
	NEW_CLASS(this->cfgVals, Data::StringMap<Data::StringMap<Text::String *>*>());
}

IO::ConfigFile::~ConfigFile()
{
	Data::StringMap<Text::String *> *cate;
	Data::ArrayList<Data::StringMap<Text::String *>*> *cates;
	Data::ArrayList<Text::String *> *vals;
	Text::String *s;
	UOSInt i;
	UOSInt j;
	cates = this->cfgVals->GetValues();
	i = cates->GetCount();
	while (i-- > 0)
	{
		cate = cates->GetItem(i);
		vals = cate->GetValues();
		j = vals->GetCount();
		while (j-- > 0)
		{
			s = vals->GetItem(j);
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

Text::String *IO::ConfigFile::GetValue(const UTF8Char *name)
{
	return GetValue((const UTF8Char*)"", name);
}

Text::String *IO::ConfigFile::GetValue(Text::String *category, Text::String *name)
{
	Data::StringMap<Text::String *> *cate = this->cfgVals->Get(Text::String::OrEmpty(category));
	if (cate == 0)
	{
		return 0;
	}
	return cate->Get(name);
}

Text::String *IO::ConfigFile::GetValue(const UTF8Char *category, const UTF8Char *name)
{
	if (category == 0)
	{
		category = (const UTF8Char*)"";
	}
	Data::StringMap<Text::String *> *cate = this->cfgVals->Get(category);
	if (cate == 0)
	{
		return 0;
	}
	return cate->Get(name);
}

Bool IO::ConfigFile::SetValue(Text::String *category, Text::String *name, Text::String *value)
{
	Data::StringMap<Text::String *> *cate;
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
		NEW_CLASS(cate, Data::StringMap<Text::String *>());
		this->cfgVals->Put(category, cate);
	}
	s = cate->Get(name);
	SDEL_STRING(s);
	cate->Put(name, SCOPY_STRING(value));
	return true;
}

Bool IO::ConfigFile::SetValue(const UTF8Char *category, const UTF8Char *name, const UTF8Char *value)
{
	Data::StringMap<Text::String *> *cate;
	Text::String *s;

	if (name == 0)
		return false;
	if (category == 0)
	{
		category = (const UTF8Char*)"";
	}
	cate = this->cfgVals->Get(category);
	if (cate == 0)
	{
		NEW_CLASS(cate, Data::StringMap<Text::String *>());
		this->cfgVals->Put(category, cate);
	}
	s = cate->Get(name);
	SDEL_STRING(s);
	cate->Put(name, Text::String::NewOrNull(value));
	return true;
}


Bool IO::ConfigFile::RemoveValue(const UTF8Char *category, const UTF8Char *name)
{
	Data::StringMap<Text::String *> *cate;
	Text::String *s;

	if (name == 0)
		return false;
	if (category == 0)
	{
		category = (const UTF8Char*)"";
	}
	cate = this->cfgVals->Get(category);
	if (cate == 0)
	{
		NEW_CLASS(cate, Data::StringMap<Text::String *>());
		this->cfgVals->Put(category, cate);
	}
	s = cate->Remove(name);
	SDEL_STRING(s);
	return true;
}

UOSInt IO::ConfigFile::GetCateCount()
{
	return this->cfgVals->GetCount();
}

UOSInt IO::ConfigFile::GetCateList(Data::ArrayList<Text::String *> *cateList, Bool withEmpty)
{
	UOSInt retCnt;
	UOSInt i = cateList->GetCount();
	UOSInt j;
	cateList->AddAll(this->cfgVals->GetKeys());
	j = cateList->GetCount();
	retCnt = j - i;
	if (!withEmpty)
	{
		while (j-- > i)
		{
			if (cateList->GetItem(j)->leng == 0)
			{
				cateList->RemoveAt(j);
				retCnt--;
			}
		}
	}
	return retCnt;
}

UOSInt IO::ConfigFile::GetKeys(Text::String *category, Data::ArrayList<Text::String *> *keyList)
{
	Data::StringMap<Text::String *> *cate;
	cate = this->cfgVals->Get(Text::String::OrEmpty(category));
	if (cate == 0)
		return 0;
	UOSInt cnt = keyList->GetCount();
	keyList->AddAll(cate->GetKeys());
	return keyList->GetCount() - cnt;
}

UOSInt IO::ConfigFile::GetKeys(const UTF8Char *category, Data::ArrayList<Text::String *> *keyList)
{
	Data::StringMap<Text::String *> *cate;
	if (category == 0)
	{
		category = (const UTF8Char*)"";
	}
	cate = this->cfgVals->Get(category);
	if (cate == 0)
		return 0;
	UOSInt cnt = keyList->GetCount();
	keyList->AddAll(cate->GetKeys());
	return keyList->GetCount() - cnt;
}

Bool IO::ConfigFile::HasCategory(const UTF8Char *category)
{
	if (category == 0)
	{
		category = (const UTF8Char*)"";
	}
	return this->cfgVals->Get(category) != 0;
}

IO::ConfigFile *IO::ConfigFile::CloneCate(const UTF8Char *category)
{
	Data::StringMap<Text::String*> *cate = this->cfgVals->Get(category);
	if (cate == 0)
	{
		return 0;
	}
	IO::ConfigFile *cfg;
	NEW_CLASS(cfg, IO::ConfigFile());
	Data::SortableArrayList<Text::String*> *keys = cate->GetKeys();
	UOSInt i = 0;
	UOSInt j = keys->GetCount();
	Text::String *key;
	while (i < j)
	{
		key = keys->GetItem(i);
		cfg->SetValue(0, key, cate->Get(key));
		i++;
	}
	return cfg;
}
