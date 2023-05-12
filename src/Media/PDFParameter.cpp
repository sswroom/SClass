#include "Stdafx.h"
#include "Media/PDFParameter.h"

Media::PDFParameter::PDFParameter()
{

}

Media::PDFParameter::~PDFParameter()
{
	ParamEntry *entry;
	UOSInt i = this->entries.GetCount();
	while (i-- > 0)
	{
		entry = this->entries.GetItem(i);
		entry->type->Release();
		SDEL_STRING(entry->value);
		MemFree(entry);
	}
}

void Media::PDFParameter::AddEntry(Text::CString type, Text::CString value)
{
	ParamEntry *entry = MemAlloc(ParamEntry, 1);
	entry->type = Text::String::New(type);
	entry->value = Text::String::NewOrNull(value);
	this->entries.Add(entry);
}

Text::String *Media::PDFParameter::GetEntryValue(Text::CString type) const
{
	ParamEntry *entry = this->GetEntry(type);
	if (entry)
		return entry->value;
	return 0;
}

Text::String *Media::PDFParameter::GetEntryType(UOSInt index) const
{
	ParamEntry *entry = this->GetItem(index);
	if (entry)
		return entry->type;
	return 0;
}

Text::String *Media::PDFParameter::GetEntryValue(UOSInt index) const
{
	ParamEntry *entry = this->GetItem(index);
	if (entry)
		return entry->value;
	return 0;
}

Bool Media::PDFParameter::ContainsEntry(Text::CString type) const
{
	return this->GetEntry(type) != 0;
}

Media::PDFParameter::ParamEntry *Media::PDFParameter::GetEntry(Text::CString type) const
{
	ParamEntry *entry;
	UOSInt i = 0;
	UOSInt j = this->entries.GetCount();
	while (i < j)
	{
		entry = this->entries.GetItem(i);
		if (entry->type->Equals(type.v, type.leng))
			return entry;
		i++;
	}
	return 0;
}

UOSInt Media::PDFParameter::GetEntryIndex(Text::CString type) const
{
	ParamEntry *entry;
	UOSInt i = 0;
	UOSInt j = this->entries.GetCount();
	while (i < j)
	{
		entry = this->entries.GetItem(i);
		if (entry->type->Equals(type.v, type.leng))
			return i;
		i++;
	}
	return INVALID_INDEX;
}

UOSInt Media::PDFParameter::GetCount() const
{
	return this->entries.GetCount();
}

Media::PDFParameter::ParamEntry *Media::PDFParameter::GetItem(UOSInt index) const
{
	return this->entries.GetItem(index);
}

Media::PDFParameter *Media::PDFParameter::Parse(Text::CString parameter)
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
				param->AddEntry(Text::CString(&parameter.v[startType], k - startType), CSTR_NULL);
			}
			else
			{
				param->AddEntry(Text::CString(&parameter.v[startType], endType - startType), Text::CString(&parameter.v[startValue], k - startValue));
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
