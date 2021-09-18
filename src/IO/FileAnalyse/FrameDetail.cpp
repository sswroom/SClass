#include "Stdafx.h"
#include "IO/FileAnalyse/FrameDetail.h"

void IO::FileAnalyse::FrameDetail::FreeFieldInfo(FieldInfo *field)
{
	Text::StrDelNew(field->name);
	SDEL_TEXT(field->value);
	MemFree(field);
}

void IO::FileAnalyse::FrameDetail::AddFieldInfo(UInt32 ofst, UInt32 size, const UTF8Char *name, const UTF8Char *value, FieldType fieldType)
{
	FieldInfo *field = MemAlloc(FieldInfo, 1);
	field->ofst = ofst;
	field->size = size;
	field->name = Text::StrCopyNew(name);
	field->value = SCOPY_TEXT(value);
	field->fieldType = fieldType;
	this->fields->Add(field);
}

IO::FileAnalyse::FrameDetail::FrameDetail(UInt64 ofst, UInt32 size)
{
	this->ofst = ofst;
	this->size = size;
	NEW_CLASS(this->headers, Data::ArrayList<const UTF8Char*>());
	NEW_CLASS(this->fields, Data::ArrayList<FieldInfo*>());
}

IO::FileAnalyse::FrameDetail::~FrameDetail()
{
	LIST_FREE_FUNC(this->headers, Text::StrDelNew);
	DEL_CLASS(this->headers);
	LIST_FREE_FUNC(this->fields, FreeFieldInfo);
	DEL_CLASS(this->fields);
}

UInt64 IO::FileAnalyse::FrameDetail::GetOffset()
{
	return this->ofst;
}

UInt32 IO::FileAnalyse::FrameDetail::GetSize()
{
	return this->size;
}

const IO::FileAnalyse::FrameDetail::FieldInfo *IO::FileAnalyse::FrameDetail::GetFieldInfo(UInt64 ofst)
{
	if (ofst < this->ofst || ofst >= this->ofst + this->size)
	{
		return 0;
	}
	FieldInfo *field;
	UInt32 frameOfst = (UInt32)(ofst - this->ofst);
	UOSInt i = 0;
	UOSInt j = this->fields->GetCount();
	while (i < j)
	{
		field = this->fields->GetItem(i);
		if (field->fieldType == FT_FIELD && frameOfst >= field->ofst && frameOfst < field->ofst + field->size)
		{
			return field;
		}
		i++;
	}
	return 0;
}

void IO::FileAnalyse::FrameDetail::AddHeader(const UTF8Char *header)
{
	this->headers->Add(Text::StrCopyNew(header));
}

void IO::FileAnalyse::FrameDetail::AddField(UInt32 ofst, UInt32 size, const UTF8Char *name, const UTF8Char *value)
{
	this->AddFieldInfo(ofst, size, name, value, FT_FIELD);
}

void IO::FileAnalyse::FrameDetail::AddSubfield(UInt32 ofst, UInt32 size, const UTF8Char *name, const UTF8Char *value)
{
	this->AddFieldInfo(ofst, size, name, value, FT_SUBFIELD);
}

void IO::FileAnalyse::FrameDetail::AddFieldSeperstor(UInt32 ofst, const UTF8Char *name)
{
	this->AddFieldInfo(ofst, 0, name, 0, FT_SEPERATOR);
}

void IO::FileAnalyse::FrameDetail::AddText(UInt32 ofst, const UTF8Char *name)
{
	this->AddFieldInfo(ofst, 0, name, 0, FT_TEXT);
}

void IO::FileAnalyse::FrameDetail::ToString(Text::StringBuilderUTF *sb)
{
	sb->Append((const UTF8Char*)"Offset=");
	sb->AppendU64(this->ofst);
	UOSInt i = 0;
	UOSInt j = this->headers->GetCount();
	while (i < j)
	{
		sb->Append((const UTF8Char*)"\r\n");
		sb->Append(this->headers->GetItem(i));
		i++;
	}

	FieldInfo *field;
	i = 0;
	j = this->fields->GetCount();
	if (j > 0)
	{
		sb->Append((const UTF8Char*)"\r\n");
		while (i < j)
		{
			field = this->fields->GetItem(i);
			switch (field->fieldType)
			{
			case FT_TEXT:
			case FT_FIELD:
				sb->Append((const UTF8Char *)"\r\n");
				break;
			case FT_SUBFIELD:
				sb->Append((const UTF8Char *)"\r\n-");
				break;
			case FT_SEPERATOR:
				sb->Append((const UTF8Char *)"\r\n\r\n");
				break;
			}
			sb->Append(field->name);
			if (field->value)
			{
				sb->AppendChar('=', 1);
				sb->Append(field->value);
			}
			i++;
		}
	}
}
