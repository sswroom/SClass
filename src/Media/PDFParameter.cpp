#include "Stdafx.h"
#include "Data/ArrayListNative.hpp"
#include "Media/PDFParameter.h"

Media::PDFParameter::PDFParameter()
{

}

Media::PDFParameter::~PDFParameter()
{
	NN<ParamEntry> entry;
	UIntOS i = this->entries.GetCount();
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
	return nullptr;
}

Optional<Text::String> Media::PDFParameter::GetEntryType(UIntOS index) const
{
	NN<ParamEntry> entry;
	if (this->GetItem(index).SetTo(entry))
		return entry->type;
	return nullptr;
}

Optional<Text::String> Media::PDFParameter::GetEntryValue(UIntOS index) const
{
	NN<ParamEntry> entry;
	if (this->GetItem(index).SetTo(entry))
		return entry->value;
	return nullptr;
}

Bool Media::PDFParameter::ContainsEntry(Text::CStringNN type) const
{
	return this->GetEntry(type).NotNull();
}

Optional<Media::PDFParameter::ParamEntry> Media::PDFParameter::GetEntry(Text::CStringNN type) const
{
	NN<ParamEntry> entry;
	UIntOS i = 0;
	UIntOS j = this->entries.GetCount();
	while (i < j)
	{
		entry = this->entries.GetItemNoCheck(i);
		if (entry->type->Equals(type))
			return entry;
		i++;
	}
	return nullptr;
}

UIntOS Media::PDFParameter::GetEntryIndex(Text::CStringNN type) const
{
	NN<ParamEntry> entry;
	UIntOS i = 0;
	UIntOS j = this->entries.GetCount();
	while (i < j)
	{
		entry = this->entries.GetItemNoCheck(i);
		if (entry->type->Equals(type))
			return i;
		i++;
	}
	return INVALID_INDEX;
}

UIntOS Media::PDFParameter::GetCount() const
{
	return this->entries.GetCount();
}

NN<Media::PDFParameter::ParamEntry> Media::PDFParameter::GetItemNoCheck(UIntOS index) const
{
	return this->entries.GetItemNoCheck(index);
}

Optional<Media::PDFParameter::ParamEntry> Media::PDFParameter::GetItem(UIntOS index) const
{
	return this->entries.GetItem(index);
}

Optional<Media::PDFParameter> Media::PDFParameter::Parse(Text::CStringNN parameter)
{
	parameter = parameter.LTrim();
	UIntOS i = 1;
	UIntOS j = parameter.leng;
	UIntOS k;
	if (j == 0)
		return nullptr;
	if (parameter.v[0] != '/')
		return nullptr;
	PDFParameter *param;
	NEW_CLASS(param, PDFParameter());
	Data::ArrayListNative<UTF8Char> endChars;
	Optional<ParamEntry> lastEntry = nullptr;
	NN<ParamEntry> nnlastEntry;
	UIntOS startType = i;
	UIntOS endType = 0;
	UIntOS startValue = 0;
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
					lastEntry = nullptr;
				}
				else
				{
					lastEntry = param->AddEntry(Text::CStringNN(&parameter.v[startType], k - startType), nullptr);
				}
			}
			else
			{
				param->AddEntry(Text::CStringNN(&parameter.v[startType], endType - startType), Text::CString(&parameter.v[startValue], k - startValue));
				lastEntry = nullptr;
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
