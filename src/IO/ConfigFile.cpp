#include "Stdafx.h"
#include "IO/ConfigFile.h"

void IO::ConfigFile::MergeCate(NN<Data::FastStringMapNN<Text::String>> myCate, NN<Data::FastStringMapNN<Text::String>> cateToMerge)
{
	Text::String *name;
	NN<Text::String> value;
	UOSInt i = 0;
	UOSInt j = cateToMerge->GetCount();
	while (i < j)
	{
		name = cateToMerge->GetKey(i);
		if (myCate->Put(name, cateToMerge->GetItemNoCheck(i)->Clone()).SetTo(value))
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
	NN<Data::FastStringMapNN<Text::String>> cate;
	NN<Text::String> s;
	UOSInt i;
	UOSInt j;
	i = this->cfgVals.GetCount();
	while (i-- > 0)
	{
		cate = this->cfgVals.GetItemNoCheck(i);
		j = cate->GetCount();
		while (j-- > 0)
		{
			s = cate->GetItemNoCheck(j);
			s->Release();
		}
		cate.Delete();
	}
	this->defCate->Release();
}

Optional<Text::String> IO::ConfigFile::GetValue(NN<Text::String> name)
{
	return GetCateValue(this->defCate, name);
}

Optional<Text::String> IO::ConfigFile::GetValue(Text::CStringNN name)
{
	return GetCateValue(this->defCate->ToCString(), name);
}

Optional<Text::String> IO::ConfigFile::GetCateValue(NN<Text::String> category, NN<Text::String> name)
{
	NN<Data::FastStringMapNN<Text::String>> cate;
	if (!this->cfgVals.GetNN(category).SetTo(cate))
	{
		return 0;
	}
	return cate->GetNN(name);
}

Optional<Text::String> IO::ConfigFile::GetCateValue(Text::CStringNN category, Text::CStringNN name)
{
	NN<Data::FastStringMapNN<Text::String>> cate;
	if (!this->cfgVals.GetC(category).SetTo(cate))
	{
		return 0;
	}
	return cate->GetC(name);
}

Bool IO::ConfigFile::SetValue(NN<Text::String> category, NN<Text::String> name, Optional<Text::String> value)
{
	NN<Data::FastStringMapNN<Text::String>> cate;
	NN<Text::String> s;
	if (!this->cfgVals.GetNN(category).SetTo(cate))
	{
		NEW_CLASSNN(cate, Data::FastStringMapNN<Text::String>());
		this->cfgVals.PutNN(category, cate);
	}
	if (value.SetTo(s))
	{
		if (cate->PutNN(name, s->Clone()).SetTo(s))
			s->Release();
	}
	else
	{
		if (cate->RemoveNN(name).SetTo(s))
			s->Release();
	}
	return true;
}

Bool IO::ConfigFile::SetValue(Text::CStringNN category, Text::CStringNN name, Text::CString value)
{
	NN<Data::FastStringMapNN<Text::String>> cate;
	NN<Text::String> s;
	if (!this->cfgVals.GetC(category).SetTo(cate))
	{
		NEW_CLASSNN(cate, Data::FastStringMapNN<Text::String>());
		this->cfgVals.PutC(category, cate);
	}
	Text::CStringNN nnvalue;
	if (value.SetTo(nnvalue))
	{
		if (cate->PutC(name, Text::String::New(nnvalue)).SetTo(s))
			s->Release();
	}
	else
	{
		if (cate->RemoveC(name).SetTo(s))
			s->Release();
	}
	return true;
}


Bool IO::ConfigFile::RemoveValue(Text::CString category, Text::CStringNN name)
{
	NN<Data::FastStringMapNN<Text::String>> cate;
	NN<Text::String> s;
	Text::CStringNN categoryNN = category.OrEmpty();
	if (!this->cfgVals.GetC(categoryNN).SetTo(cate))
	{
		NEW_CLASSNN(cate, Data::FastStringMapNN<Text::String>());
		this->cfgVals.PutC(categoryNN, cate);
	}
	if (cate->RemoveC(name).SetTo(s))
		s->Release();
	return true;
}

UOSInt IO::ConfigFile::GetCateCount() const
{
	return this->cfgVals.GetCount();
}

UOSInt IO::ConfigFile::GetCateList(NN<Data::ArrayListStringNN> cateList, Bool withEmpty)
{
	UOSInt retCnt = 0;
	Data::FastStringNNKeyIterator<Data::FastStringMapNN<Text::String>> it = this->cfgVals.KeyIterator();
	cateList->EnsureCapacity(this->cfgVals.GetCount());
	while (it.HasNext())
	{
		NN<Text::String> key = it.Next();
		if (withEmpty || key->leng > 0)
		{
			cateList->Add(key);
			retCnt++;
		}
	}
	return retCnt;
}

UOSInt IO::ConfigFile::GetKeys(NN<Text::String> category, NN<Data::ArrayListStringNN> keyList)
{
	NN<Data::FastStringMapNN<Text::String>> cate;
	if (!this->cfgVals.GetNN(category).SetTo(cate))
		return 0;
	UOSInt cnt = cate->GetCount();
	Data::FastStringNNKeyIterator<Text::String> it = cate->KeyIterator();
	keyList->EnsureCapacity(cnt);
	while (it.HasNext())
	{
		keyList->Add(it.Next());
	}
	return cnt;
}

UOSInt IO::ConfigFile::GetKeys(Text::CStringNN category, NN<Data::ArrayListStringNN> keyList)
{
	NN<Data::FastStringMapNN<Text::String>> cate;
	if (!this->cfgVals.GetC(category).SetTo(cate))
		return 0;
	UOSInt cnt = cate->GetCount();
	keyList->EnsureCapacity(cnt);
	Data::FastStringNNKeyIterator<Text::String> it = cate->KeyIterator();
	while (it.HasNext())
	{
		keyList->Add(it.Next());
	}
	return cnt;
}

UOSInt IO::ConfigFile::GetCount(Text::CString category) const
{
	NN<Data::FastStringMapNN<Text::String>> cate;
	if (!this->cfgVals.GetC(category.OrEmpty()).SetTo(cate))
		return 0;
	else
		return cate->GetCount();
}

Optional<Text::String> IO::ConfigFile::GetKey(Text::CString category, UOSInt index) const
{
	NN<Data::FastStringMapNN<Text::String>> cate;
	if (!this->cfgVals.GetC(category.OrEmpty()).SetTo(cate))
		return 0;
	else
		return cate->GetKey(index);
}

Bool IO::ConfigFile::HasCategory(Text::CString category) const
{
	return this->cfgVals.GetC(category.OrEmpty()) != 0;
}

Optional<IO::ConfigFile> IO::ConfigFile::CloneCate(Text::CString category)
{
	NN<Data::FastStringMapNN<Text::String>> cate;
	if (!this->cfgVals.GetC(category.OrEmpty()).SetTo(cate))
	{
		return 0;
	}
	NN<IO::ConfigFile> cfg;
	NEW_CLASSNN(cfg, IO::ConfigFile());
	Data::FastStringNNKeyIterator<Text::String> it = cate->KeyIterator();
	NN<Text::String> key;
	while (it.HasNext())
	{
		key = it.Next();
		cfg->SetValue(Text::String::NewEmpty(), key, cate->GetNN(key));
	}
	return cfg;
}

void IO::ConfigFile::MergeConfig(NN<IO::ConfigFile> cfg)
{
	UOSInt i = cfg->cfgVals.GetCount();
	NN<Data::FastStringMapNN<Text::String>> cate;
	while (i-- > 0)
	{
		if (!this->cfgVals.Get(cfg->cfgVals.GetKey(i)).SetTo(cate))
		{
			NEW_CLASSNN(cate, Data::FastStringMapNN<Text::String>());
			this->cfgVals.Put(cfg->cfgVals.GetKey(i), cate);
		}
		this->MergeCate(cate, cfg->cfgVals.GetItemNoCheck(i));
	}
}
