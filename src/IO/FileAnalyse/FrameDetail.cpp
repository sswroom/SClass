#include "Stdafx.h"
#include "IO/FileAnalyse/FrameDetail.h"

void __stdcall IO::FileAnalyse::FrameDetail::FreeFieldInfo(NN<FieldInfo> field)
{
	field->name->Release();
	field->value->Release();
	MemFreeNN(field);
}

void IO::FileAnalyse::FrameDetail::AddFieldInfo(UInt64 ofst, UInt64 size, Text::CStringNN name, Text::CString value, FieldType fieldType)
{
	NN<FieldInfo> field = MemAllocNN(FieldInfo);
	field->ofst = ofst;
	field->size = size;
	field->name = Text::String::New(name);
	field->value = Text::String::New(value.OrEmpty());
	field->fieldType = fieldType;
	this->fields.Add(field);
}

IO::FileAnalyse::FrameDetail::FrameDetail(UInt64 ofst, UInt64 size)
{
	this->ofst = ofst;
	this->size = size;
}

IO::FileAnalyse::FrameDetail::~FrameDetail()
{
	this->headers.FreeAll();
	this->fields.FreeAll(FreeFieldInfo);
}

UInt64 IO::FileAnalyse::FrameDetail::GetOffset() const
{
	return this->ofst;
}

UInt64 IO::FileAnalyse::FrameDetail::GetSize() const
{
	return this->size;
}

UOSInt IO::FileAnalyse::FrameDetail::GetFieldInfos(UInt64 ofst, NN<Data::ArrayListNN<const FieldInfo>> fieldList) const
{
	if (ofst < this->ofst || ofst >= this->ofst + this->size)
	{
		return 0;
	}
	NN<FieldInfo> field;
	UInt32 frameOfst = (UInt32)(ofst - this->ofst);
	UOSInt ret = 0;
	UOSInt i = 0;
	UOSInt j = this->fields.GetCount();
	while (i < j)
	{
		field = this->fields.GetItemNoCheck(i);
		if ((field->fieldType == FT_FIELD || field->fieldType == FT_SUBFRAME) && frameOfst >= field->ofst && frameOfst < field->ofst + field->size)
		{
			fieldList->Add(field);
			ret++;
		}
		i++;
	}
	return ret;
}

UOSInt IO::FileAnalyse::FrameDetail::GetAreaInfos(UInt64 ofst, NN<Data::ArrayListNN<const FieldInfo>> areaList) const
{
	if (ofst < this->ofst || ofst >= this->ofst + this->size)
	{
		return 0;
	}
	NN<FieldInfo> field;
	UInt32 frameOfst = (UInt32)(ofst - this->ofst);
	UOSInt ret = 0;
	UOSInt i = 0;
	UOSInt j = this->fields.GetCount();
	while (i < j)
	{
		field = this->fields.GetItemNoCheck(i);
		if (field->fieldType == FT_AREA && frameOfst >= field->ofst && frameOfst < field->ofst + field->size)
		{
			areaList->Add(field);
			ret++;
		}
		i++;
	}
	return ret;
}

void IO::FileAnalyse::FrameDetail::AddHeader(Text::CStringNN header)
{
	this->headers.Add(Text::String::New(header));
}

void IO::FileAnalyse::FrameDetail::AddField(UInt64 ofst, UInt64 size, Text::CStringNN name, Text::CString value)
{
	this->AddFieldInfo(ofst, size, name, value, FT_FIELD);
}

void IO::FileAnalyse::FrameDetail::AddSubfield(UInt64 ofst, UInt64 size, Text::CStringNN name, Text::CString value)
{
	this->AddFieldInfo(ofst, size, name, value, FT_SUBFIELD);
}

void IO::FileAnalyse::FrameDetail::AddFieldSeperstor(UInt64 ofst, Text::CStringNN name)
{
	this->AddFieldInfo(ofst, 0, name, CSTR_NULL, FT_SEPERATOR);
}

void IO::FileAnalyse::FrameDetail::AddText(UInt64 ofst, Text::CStringNN name)
{
	this->AddFieldInfo(ofst, 0, name, CSTR_NULL, FT_TEXT);
}

void IO::FileAnalyse::FrameDetail::AddSubframe(UInt64 ofst, UInt64 size)
{
	this->AddFieldInfo(ofst, size, CSTR("Subframe"), CSTR_NULL, FT_SUBFRAME);
}

void IO::FileAnalyse::FrameDetail::AddArea(UInt64 ofst, UOSInt size, Text::CStringNN name)
{
	this->AddFieldInfo(ofst, size, name, CSTR_NULL, FT_AREA);
}

void IO::FileAnalyse::FrameDetail::ToString(NN<Text::StringBuilderUTF8> sb) const
{
	sb->AppendC(UTF8STRC("Offset="));
	sb->AppendU64(this->ofst);
	Data::ArrayIterator<NN<Text::String>> it = this->headers.Iterator();
	while (it.HasNext())
	{
		sb->AppendC(UTF8STRC("\r\n"));
		sb->Append(it.Next());
	}

	NN<FieldInfo> field;
	UOSInt j = this->fields.GetCount();
	if (j > 0)
	{
		sb->AppendC(UTF8STRC("\r\n"));
		UOSInt i = 0;
		while (i < j)
		{
			field = this->fields.GetItemNoCheck(i);
			if (field->fieldType != FT_AREA)
			{
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
				case FT_AREA:
					sb->AppendC(UTF8STRC("\r\n"));
				}
				sb->Append(field->name);
				sb->AppendUTF8Char('=');
				sb->Append(field->value);
			}
			i++;
		}
	}
}
