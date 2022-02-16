#include "Stdafx.h"
#include "IO/FileAnalyse/FrameDetail.h"

void IO::FileAnalyse::FrameDetail::FreeFieldInfo(FieldInfo *field)
{
	field->name->Release();
	SDEL_STRING(field->value);
	MemFree(field);
}

void IO::FileAnalyse::FrameDetail::AddFieldInfo(UOSInt ofst, UOSInt size, Text::CString name, Text::CString value, FieldType fieldType)
{
	FieldInfo *field = MemAlloc(FieldInfo, 1);
	field->ofst = (UInt32)ofst;
	field->size = (UInt32)size;
	field->name = Text::String::New(name.v, name.leng);
	field->value = Text::String::New(value.v, value.leng);
	field->fieldType = fieldType;
	this->fields->Add(field);
}

IO::FileAnalyse::FrameDetail::FrameDetail(UInt64 ofst, UInt64 size)
{
	this->ofst = ofst;
	this->size = size;
	NEW_CLASS(this->headers, Data::ArrayList<Text::String*>());
	NEW_CLASS(this->fields, Data::ArrayList<FieldInfo*>());
}

IO::FileAnalyse::FrameDetail::~FrameDetail()
{
	LIST_FREE_STRING(this->headers);
	DEL_CLASS(this->headers);
	LIST_FREE_FUNC(this->fields, FreeFieldInfo);
	DEL_CLASS(this->fields);
}

UInt64 IO::FileAnalyse::FrameDetail::GetOffset()
{
	return this->ofst;
}

UInt64 IO::FileAnalyse::FrameDetail::GetSize()
{
	return this->size;
}

UOSInt IO::FileAnalyse::FrameDetail::GetFieldInfos(UInt64 ofst, Data::ArrayList<const FieldInfo*> *fieldList)
{
	if (ofst < this->ofst || ofst >= this->ofst + this->size)
	{
		return 0;
	}
	FieldInfo *field;
	UInt32 frameOfst = (UInt32)(ofst - this->ofst);
	UOSInt ret = 0;
	UOSInt i = 0;
	UOSInt j = this->fields->GetCount();
	while (i < j)
	{
		field = this->fields->GetItem(i);
		if ((field->fieldType == FT_FIELD || field->fieldType == FT_SUBFRAME) && frameOfst >= field->ofst && frameOfst < field->ofst + field->size)
		{
			fieldList->Add(field);
			ret++;
		}
		i++;
	}
	return ret;
}

void IO::FileAnalyse::FrameDetail::AddHeader(Text::CString header)
{
	this->headers->Add(Text::String::New(header.v, header.leng));
}

void IO::FileAnalyse::FrameDetail::AddField(UOSInt ofst, UOSInt size, Text::CString name, Text::CString value)
{
	this->AddFieldInfo(ofst, size, name, value, FT_FIELD);
}

void IO::FileAnalyse::FrameDetail::AddSubfield(UOSInt ofst, UOSInt size, Text::CString name, Text::CString value)
{
	this->AddFieldInfo(ofst, size, name, value, FT_SUBFIELD);
}

void IO::FileAnalyse::FrameDetail::AddFieldSeperstor(UOSInt ofst, Text::CString name)
{
	this->AddFieldInfo(ofst, 0, name, CSTR_NULL, FT_SEPERATOR);
}

void IO::FileAnalyse::FrameDetail::AddText(UOSInt ofst, Text::CString name)
{
	this->AddFieldInfo(ofst, 0, name, CSTR_NULL, FT_TEXT);
}

void IO::FileAnalyse::FrameDetail::AddSubframe(UOSInt ofst, UOSInt size)
{
	this->AddFieldInfo(ofst, size, CSTR("Subframe"), CSTR_NULL, FT_SUBFRAME);
}

void IO::FileAnalyse::FrameDetail::ToString(Text::StringBuilderUTF8 *sb)
{
	sb->AppendC(UTF8STRC("Offset="));
	sb->AppendU64(this->ofst);
	UOSInt i = 0;
	UOSInt j = this->headers->GetCount();
	while (i < j)
	{
		sb->AppendC(UTF8STRC("\r\n"));
		sb->Append(this->headers->GetItem(i));
		i++;
	}

	FieldInfo *field;
	i = 0;
	j = this->fields->GetCount();
	if (j > 0)
	{
		sb->AppendC(UTF8STRC("\r\n"));
		while (i < j)
		{
			field = this->fields->GetItem(i);
			switch (field->fieldType)
			{
			case FT_TEXT:
			case FT_FIELD:
				sb->AppendC(UTF8STRC("\r\n"));
				break;
			case FT_SUBFIELD:
				sb->AppendC(UTF8STRC("\r\n-"));
				break;
			case FT_SEPERATOR:
				sb->AppendC(UTF8STRC("\r\n\r\n"));
				break;
			case FT_SUBFRAME:
				sb->AppendC(UTF8STRC("\r\n"));
			}
			sb->Append(field->name);
			if (field->value)
			{
				sb->AppendUTF8Char('=');
				sb->Append(field->value);
			}
			i++;
		}
	}
}
