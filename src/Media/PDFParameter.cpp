#include "Stdafx.h"
#include "Data/ArrayList.hpp"
#include "Media/PDFParameter.h"

Media::PDFParameter::PDFParameter()
{

}

Media::PDFParameter::~PDFParameter()
{
	NN<ParamEntry> entry;
	UOSInt i = this->entries.GetCount();
	while (i-- > 0)
	{
		entry = this->entries.GetItemNoCheck(i);
		entry->type->Release();
		OPTSTR_DEL(entry->value);
		MemFreeNN(entry);
	}
}

NN<Media::PDFParameter::ParamEntry> Media::PDFParameter::AddEntry(Text::CStringNN type, Text::CString value)
{
	NN<ParamEntry> entry = MemAllocNN(ParamEntry);
	entry->type = Text::String::New(type);
	entry->value = Text::String::NewOrNull(value);
	this->entries.Add(entry);
	return entry;
}

Optional<Text::String> Media::PDFParameter::GetEntryValue(Text::CStringNN type) const
{
	NN<ParamEntry> entry;
	if (this->GetEntry(type).SetTo(entry))
		return entry->value;
	return 0;
}

Optional<Text::String> Media::PDFParameter::GetEntryType(UOSInt index) const
{
	NN<ParamEntry> entry;
	if (this->GetItem(index).SetTo(entry))
		return entry->type;
	return 0;
}

Optional<Text::String> Media::PDFParameter::GetEntryValue(UOSInt index) const
{
	NN<ParamEntry> entry;
	if (this->GetItem(index).SetTo(entry))
		return entry->value;
	return 0;
}

Bool Media::PDFParameter::ContainsEntry(Text::CStringNN type) const
{
	return this->GetEntry(type).NotNull();
}

Optional<Media::PDFParameter::ParamEntry> Media::PDFParameter::GetEntry(Text::CStringNN type) const
{
	NN<ParamEntry> entry;
	UOSInt i = 0;
	UOSInt j = this->entries.GetCount();
	while (i < j)
	{
		entry = this->entries.GetItemNoCheck(i);
		if (entry->type->Equals(type))
			return entry;
		i++;
	}
	return 0;
}

UOSInt Media::PDFParameter::GetEntryIndex(Text::CStringNN type) const
{
	NN<ParamEntry> entry;
	UOSInt i = 0;
	UOSInt j = this->entries.GetCount();
	while (i < j)
	{
		entry = this->entries.GetItemNoCheck(i);
		if (entry->type->Equals(type))
			return i;
		i++;
	}
	return INVALID_INDEX;
}

UOSInt Media::PDFParameter::GetCount() const
{
	return this->entries.GetCount();
}

NN<Media::PDFParameter::ParamEntry> Media::PDFParameter::GetItemNoCheck(UOSInt index) const
{
	return this->entries.GetItemNoCheck(index);
}

Optional<Media::PDFParameter::ParamEntry> Media::PDFParameter::GetItem(UOSInt index) const
{
	return this->entries.GetItem(index);
}

Optional<Media::PDFParameter> Media::PDFParameter::Parse(Text::CStringNN parameter)
{
	parameter = parameter.LTrim();
	UOSInt i = 1;
	UOSInt j = parameter.leng;
	UOSInt k;
	if (j == 0)
		return 0;
	if (parameter.v[0] != '/')
		return 0;
	PDFParameter *param;
	NEW_CLASS(param, PDFParameter());
	Data::ArrayList<UTF8Char> endChars;
	Optional<ParamEntry> lastEntry = 0;
	NN<ParamEntry> nnlastEntry;
	UOSInt startType = i;
	UOSInt endType = 0;
	UOSInt startValue = 0;
	while (true)
	{
		if (i >= j || (endChars.GetCount() == 0 && parameter.v[i] == '/'))
		{
			k = i;
			while (parameter.v[k - 1] == ' ')
				k--;
			if (startValue == 0 || k == endType)
			{
				if (lastEntry.SetTo(nnlastEntry))
				{
					nnlastEntry->value = Text::String::New(&parameter.v[startType - 1], k - startType + 1);
					lastEntry = 0;
				}
				else
				{
					lastEntry = param->AddEntry(Text::CStringNN(&parameter.v[startType], k - startType), nullptr);
				}
			}
			else
			{
				param->AddEntry(Text::CStringNN(&parameter.v[startType], endType - startType), Text::CString(&parameter.v[startValue], k - startValue));
				lastEntry = 0;
			}
			startType = i + 1;
			endType = 0;
			startValue = 0;
			if (i >= j)
				break;
		}
		else if (parameter.v[i] == '<' && parameter.v[i + 1] == '<')
		{
			if (startValue == 0)
			{
				endType = i;
				startValue = i;
			}
			endChars.Add('>');
			endChars.Add('>');
		}
		else if (parameter.v[i] == '[')
		{
			if (startValue == 0)
			{
				endType = i;
				startValue = i;
			}
			endChars.Add(']');
		}
		else if (parameter.v[i] == '(')
		{
			if (startValue == 0)
			{
				endType = i;
				startValue = i;
			}
			endChars.Add(')');
		}
		else if (parameter.v[i] == ' ' && startValue == 0)
		{
			startValue = i + 1;
			endType = i;
		}
		else if (endChars.GetCount() > 0 && parameter.v[i] == endChars.GetItem(endChars.GetCount() - 1))
		{
			endChars.RemoveAt(endChars.GetCount() - 1);
		}
		i++;
	}
	return param;
}
