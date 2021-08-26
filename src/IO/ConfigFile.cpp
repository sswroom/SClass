#include "Stdafx.h"
#include "IO/ConfigFile.h"

IO::ConfigFile::ConfigFile()
{
	NEW_CLASS(this->cfgVals, Data::StringUTF8Map<Data::StringUTF8Map<const UTF8Char *>*>());
}

IO::ConfigFile::~ConfigFile()
{
	Data::StringUTF8Map<const UTF8Char *> *cate;
	Data::ArrayList<Data::StringUTF8Map<const UTF8Char *>*> *cates;
	Data::ArrayList<const UTF8Char *> *vals;
	const UTF8Char *s;
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
			if (s)
			{
				Text::StrDelNew(s);
			}
		}
		DEL_CLASS(cate);
	}
	DEL_CLASS(this->cfgVals);
}

const UTF8Char *IO::ConfigFile::GetValue(const UTF8Char *name)
{
	return GetValue((const UTF8Char*)"", name);
}

const UTF8Char *IO::ConfigFile::GetValue(const UTF8Char *category, const UTF8Char *name)
{
	if (category == 0)
	{
		category = (const UTF8Char*)"";
	}
	Data::StringUTF8Map<const UTF8Char *> *cate = this->cfgVals->Get(category);
	if (cate == 0)
	{
		return 0;
	}
	return cate->Get(name);
}

Bool IO::ConfigFile::SetValue(const UTF8Char *category, const UTF8Char *name, const UTF8Char *value)
{
	Data::StringUTF8Map<const UTF8Char *> *cate;
	const UTF8Char *s;

	if (name == 0)
		return false;
	if (category == 0)
	{
		category = (const UTF8Char*)"";
	}
	cate = this->cfgVals->Get(category);
	if (cate == 0)
	{
		NEW_CLASS(cate, Data::StringUTF8Map<const UTF8Char *>());
		this->cfgVals->Put(category, cate);
	}
	s = cate->Get(name);
	if (s)
	{
		Text::StrDelNew(s);
	}
	if (value == 0)
	{
		cate->Put(name, 0);
	}
	else
	{
		cate->Put(name, Text::StrCopyNew(value));
	}
	return true;
}


Bool IO::ConfigFile::RemoveValue(const UTF8Char *category, const UTF8Char *name)
{
	Data::StringUTF8Map<const UTF8Char *> *cate;
	const UTF8Char *s;

	if (name == 0)
		return false;
	if (category == 0)
	{
		category = (const UTF8Char*)"";
	}
	cate = this->cfgVals->Get(category);
	if (cate == 0)
	{
		NEW_CLASS(cate, Data::StringUTF8Map<const UTF8Char *>());
		this->cfgVals->Put(category, cate);
	}
	s = cate->Remove(name);
	if (s)
	{
		Text::StrDelNew(s);
	}
	return true;
}

UOSInt IO::ConfigFile::GetCateCount()
{
	return this->cfgVals->GetCount();
}

UOSInt IO::ConfigFile::GetCateList(Data::ArrayList<const UTF8Char *> *cateList)
{
	UOSInt retCnt;
	UOSInt i = cateList->GetCount();
	UOSInt j;
	cateList->AddAll(this->cfgVals->GetKeys());
	j = cateList->GetCount();
	retCnt = j - i;
	while (j-- > i)
	{
		if (cateList->GetItem(j)[0] == 0)
		{
			cateList->RemoveAt(j);
			retCnt--;
		}
	}
	return retCnt;
}

UOSInt IO::ConfigFile::GetKeys(const UTF8Char *category, Data::ArrayList<const UTF8Char *> *keyList)
{
	Data::StringUTF8Map<const UTF8Char *> *cate;
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
	Data::StringUTF8Map<const UTF8Char*> *cate = this->cfgVals->Get(category);
	if (cate == 0)
	{
		return 0;
	}
	IO::ConfigFile *cfg;
	NEW_CLASS(cfg, IO::ConfigFile());
	Data::SortableArrayList<const UTF8Char*> *keys = cate->GetKeys();
	UOSInt i = 0;
	UOSInt j = keys->GetCount();
	const UTF8Char *key;
	while (i < j)
	{
		key = keys->GetItem(i);
		cfg->SetValue(0, key, cate->Get(key));
		i++;
	}
	return cfg;
}
