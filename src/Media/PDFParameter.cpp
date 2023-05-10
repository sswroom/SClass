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

Text::String *Media::PDFParameter::GetEntry(Text::CString type)
{
	ParamEntry *entry;
	UOSInt i = 0;
	UOSInt j = this->entries.GetCount();
	while (i < j)
	{
		entry = this->entries.GetItem(i);
		if (entry->type->Equals(type.v, type.leng))
			return entry->value;
		i++;
	}
	return 0;
}

Media::PDFParameter *Media::PDFParameter::Parse(Text::CString parameter)
{
	UOSInt i = 1;
	UOSInt j = parameter.leng;
	if (j == 0)
		return 0;
	if (parameter.v[0] != '/')
		return 0;
	PDFParameter *param;
	NEW_CLASS(param, PDFParameter());
	UOSInt startType = i;
	UOSInt startValue = 0;
	while (true)
	{
		if (i >= j || parameter.v[i] == '/')
		{
			if (startValue == 0)
			{
				param->AddEntry(Text::CString(&parameter.v[startType], i - startType), CSTR_NULL);
			}
			else
			{
				param->AddEntry(Text::CString(&parameter.v[startType], startValue - startType - 1), Text::CString(&parameter.v[startValue], i - startValue));
			}
			startType = i + 1;
			startValue = 0;
			if (i >= j)
				break;
		}
		else if (parameter.v[i] == ' ' && startValue == 0)
		{
			startValue = i + 1;
		}
		i++;
	}
	return param;
}
