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
		value = myCate->Put(name, cateToMerge->GetItem(i)->Clone().Ptr());
		if (value)
		{
			value->Release();
		}
		i++;
	}
}

IO::ConfigFile::ConfigFile()
{
	this->defCate = Text::String::NewEmpty();
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
	this->defCate->Release();
}

Text::String *IO::ConfigFile::GetValue(NotNullPtr<Text::String> name)
{
	return GetCateValue(this->defCate, name);
}

Text::String *IO::ConfigFile::GetValue(Text::CStringNN name)
{
	return GetCateValue(this->defCate->ToCString(), name);
}

Text::String *IO::ConfigFile::GetCateValue(Text::String *category, NotNullPtr<Text::String> name)
{
	Data::FastStringMap<Text::String *> *cate = this->cfgVals.GetNN(Text::String::OrEmpty(category));
	if (cate == 0)
	{
		return 0;
	}
	return cate->GetNN(name);
}

Text::String *IO::ConfigFile::GetCateValue(NotNullPtr<Text::String> category, NotNullPtr<Text::String> name)
{
	Data::FastStringMap<Text::String *> *cate = this->cfgVals.GetNN(category);
	if (cate == 0)
	{
		return 0;
	}
	return cate->GetNN(name);
}

Text::String *IO::ConfigFile::GetCateValue(Text::CStringNN category, Text::CStringNN name)
{
	Data::FastStringMap<Text::String *> *cate = this->cfgVals.GetC(category.OrEmpty());
	if (cate == 0)
	{
		return 0;
	}
	return cate->GetC(name);
}

Bool IO::ConfigFile::SetValue(Text::String *category, NotNullPtr<Text::String> name, Text::String *value)
{
	Data::FastStringMap<Text::String *> *cate;
	Text::String *s;

	NotNullPtr<Text::String> cateNN;
	if (!cateNN.Set(category))
	{
		cateNN = Text::String::NewEmpty();
	}
	cate = this->cfgVals.GetNN(cateNN);
	if (cate == 0)
	{
		NEW_CLASS(cate, Data::FastStringMap<Text::String *>());
		this->cfgVals.PutNN(cateNN, cate);
	}
	s = cate->PutNN(name, SCOPY_STRING(value));
	SDEL_STRING(s);
	return true;
}

Bool IO::ConfigFile::SetValue(Text::CString category, Text::CStringNN name, Text::CString value)
{
	Data::FastStringMap<Text::String *> *cate;
	Text::String *s;
	Text::CStringNN categoryNN = category.OrEmpty();
	cate = this->cfgVals.GetC(categoryNN);
	if (cate == 0)
	{
		NEW_CLASS(cate, Data::FastStringMap<Text::String *>());
		this->cfgVals.PutC(categoryNN, cate);
	}
	s = cate->PutC(name, Text::String::NewOrNull(value));
	SDEL_STRING(s);
	return true;
}


Bool IO::ConfigFile::RemoveValue(Text::CString category, Text::CStringNN name)
{
	Data::FastStringMap<Text::String *> *cate;
	Text::String *s;
	Text::CStringNN categoryNN = category.OrEmpty();
	cate = this->cfgVals.GetC(categoryNN);
	if (cate == 0)
	{
		NEW_CLASS(cate, Data::FastStringMap<Text::String *>());
		this->cfgVals.PutC(categoryNN, cate);
	}
	s = cate->RemoveC(name);
	SDEL_STRING(s);
	return true;
}

UOSInt IO::ConfigFile::GetCateCount() const
{
	return this->cfgVals.GetCount();
}

UOSInt IO::ConfigFile::GetCateList(Data::ArrayListNN<Text::String> *cateList, Bool withEmpty)
{
	UOSInt retCnt = 0;
	Data::FastStringKeyIterator<Data::FastStringMap<Text::String*>*> it = this->cfgVals.KeyIterator();
	cateList->EnsureCapacity(this->cfgVals.GetCount());
	while (it.HasNext())
	{
		NotNullPtr<Text::String> key = it.Next();
		if (withEmpty || key->leng > 0)
		{
			cateList->Add(key);
			retCnt++;
		}
	}
	return retCnt;
}

UOSInt IO::ConfigFile::GetKeys(Text::String *category, NotNullPtr<Data::ArrayListNN<Text::String>> keyList)
{
	Data::FastStringMap<Text::String *> *cate;
	cate = this->cfgVals.GetNN(Text::String::OrEmpty(category));
	if (cate == 0)
		return 0;
	UOSInt cnt = cate->GetCount();
	Data::FastStringKeyIterator<Text::String*> it = cate->KeyIterator();
	keyList->EnsureCapacity(cnt);
	while (it.HasNext())
	{
		keyList->Add(it.Next());
	}
	return cnt;
}

UOSInt IO::ConfigFile::GetKeys(Text::CString category, NotNullPtr<Data::ArrayListNN<Text::String>> keyList)
{
	Data::FastStringMap<Text::String *> *cate;
	cate = this->cfgVals.GetC(category.OrEmpty());
	if (cate == 0)
		return 0;
	UOSInt cnt = cate->GetCount();
	keyList->EnsureCapacity(cnt);
	Data::FastStringKeyIterator<Text::String*> it = cate->KeyIterator();
	while (it.HasNext())
	{
		keyList->Add(it.Next());
	}
	return cnt;
}

UOSInt IO::ConfigFile::GetCount(Text::CString category) const
{
	Data::FastStringMap<Text::String*> *cate = this->cfgVals.GetC(category.OrEmpty());
	if (cate == 0)
		return 0;
	else
		return cate->GetCount();
}

Text::String *IO::ConfigFile::GetKey(Text::CString category, UOSInt index) const
{
	Data::FastStringMap<Text::String*> *cate = this->cfgVals.GetC(category.OrEmpty());
	if (cate == 0)
		return 0;
	else
		return cate->GetKey(index);
}

Bool IO::ConfigFile::HasCategory(Text::CString category) const
{
	return this->cfgVals.GetC(category.OrEmpty()) != 0;
}

IO::ConfigFile *IO::ConfigFile::CloneCate(Text::CString category)
{
	Data::FastStringMap<Text::String*> *cate = this->cfgVals.GetC(category.OrEmpty());
	if (cate == 0)
	{
		return 0;
	}
	IO::ConfigFile *cfg;
	NEW_CLASS(cfg, IO::ConfigFile());
	Data::FastStringKeyIterator<Text::String*> it = cate->KeyIterator();
	NotNullPtr<Text::String> key;
	while (it.HasNext())
	{
		key = it.Next();
		cfg->SetValue(0, key, cate->GetNN(key));
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
