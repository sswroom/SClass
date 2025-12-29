#include "Stdafx.h"
#include "Data/Compress/Inflater.h"
#include "IO/FileStream.h"
#include "IO/MemoryStream.h"
#include "IO/StmData/MemoryDataCopy.h"
#include "Media/PDFObject.h"

Media::PDFObject::PDFObject(UInt32 id)
{
	this->id = id;
	this->streamData = false;
	this->fd = 0;
	this->decFd = 0;
	this->parameter = 0;
}

Media::PDFObject::~PDFObject()
{
	this->fd.Delete();
	this->decFd.Delete();
	this->parameter.Delete();
}

void Media::PDFObject::SetStream(NN<IO::StreamData> fd, UInt64 ofst, UInt64 len)
{
	this->fd.Delete();
	this->streamData = true;
	this->fd = fd->GetPartialData(ofst, len);
}

void Media::PDFObject::SetData(NN<IO::StreamData> fd, UInt64 ofst, UInt64 len)
{
	this->fd.Delete();
	this->streamData = false;
	this->fd = fd->GetPartialData(ofst, len);
}

void Media::PDFObject::SetParameter(Optional<PDFParameter> parameter)
{
	this->parameter.Delete();
	this->parameter = parameter;
}

Optional<Media::PDFParameter> Media::PDFObject::GetParameter() const
{
	return this->parameter;
}

UInt32 Media::PDFObject::GetId() const
{
	return this->id;
}

Bool Media::PDFObject::IsImage() const
{
	NN<Media::PDFParameter> parameter;
	NN<Text::String> s;
	if (this->parameter.SetTo(parameter) && parameter->GetEntryValue(CSTR("Subtype")).SetTo(s))
	{
		return s->Equals(UTF8STRC("/Image"));
	}
	return  false;
}

Bool Media::PDFObject::IsFlateDecode() const
{
	NN<Media::PDFParameter> parameter;
	NN<Text::String> s;
	if (this->parameter.SetTo(parameter) && parameter->GetEntryValue(CSTR("Filter")).SetTo(s))
	{
		return s->IndexOf(UTF8STRC("/FlateDecode")) != INVALID_INDEX;
	}
	return false;
}

Optional<Text::String> Media::PDFObject::GetType() const
{
	NN<Media::PDFParameter> parameter;
	if (!this->parameter.SetTo(parameter))
		return 0;
	UOSInt i = parameter->GetEntryIndex(CSTR("Type"));
	if (i != INVALID_INDEX)
		return parameter->GetEntryValue(i);
	return 0;
}

Optional<Text::String> Media::PDFObject::GetSubtype() const
{
	NN<Media::PDFParameter> parameter;
	if (!this->parameter.SetTo(parameter))
		return 0;
	UOSInt i = parameter->GetEntryIndex(CSTR("Subtype"));
	if (i != INVALID_INDEX)
		return parameter->GetEntryValue(i);
	return 0;
}

Optional<Text::String> Media::PDFObject::GetFilter() const
{
	NN<Media::PDFParameter> parameter;
	if (!this->parameter.SetTo(parameter))
		return 0;
	NN<Text::String> s;
	UOSInt i = parameter->GetEntryIndex(CSTR("Filter"));
	if (i != INVALID_INDEX)
	{
		NN<Media::PDFParameter::ParamEntry> entry = parameter->GetItemNoCheck(i);
		if (entry->value.SetTo(s))
			return s;
		return parameter->GetEntryValue(i);
	}
	return 0;
}

Optional<Text::String> Media::PDFObject::GetColorSpace() const
{
	NN<Media::PDFParameter> parameter;
	if (!this->parameter.SetTo(parameter))
		return 0;
	UOSInt i = parameter->GetEntryIndex(CSTR("ColorSpace"));
	if (i != INVALID_INDEX)
	{
		return parameter->GetEntryValue(i);
	}
	return 0;
}

UOSInt Media::PDFObject::GetBitPerComponent() const
{
	NN<Media::PDFParameter> parameter;
	if (!this->parameter.SetTo(parameter))
		return 0;
	NN<Text::String> s;
	if (parameter->GetEntryValue(CSTR("BitPerComponent")).SetTo(s))
		return s->ToUOSInt();
	return 0;
}

UOSInt Media::PDFObject::GetWidth() const
{
	NN<Media::PDFParameter> parameter;
	if (!this->parameter.SetTo(parameter))
		return 0;
	NN<Text::String> s;
	if (parameter->GetEntryValue(CSTR("Width")).SetTo(s))
		return s->ToUOSInt();
	return 0;
}

UOSInt Media::PDFObject::GetHeight() const
{
	NN<Media::PDFParameter> parameter;
	if (!this->parameter.SetTo(parameter))
		return 0;
	NN<Text::String> s;
	if (parameter->GetEntryValue(CSTR("Height")).SetTo(s))
		return s->ToUOSInt();
	return 0;
}

Bool Media::PDFObject::ToString(NN<Text::StringBuilderUTF8> sb) const
{
	sb->AppendU32(this->id);
	sb->Append(CSTR(" 0 obj\r"));
	NN<PDFParameter> param;
	NN<Text::String> s;
	if (this->parameter.SetTo(param))
	{
		sb->Append(CSTR("<<"));
		NN<PDFParameter::ParamEntry> ent;
		UOSInt i = 0;
		UOSInt j = param->GetCount();
		while (i < j)
		{
			ent = param->GetItemNoCheck(i);
			sb->AppendUTF8Char('/');
			sb->Append(ent->type);
			if (ent->value.SetTo(s) && s->leng > 0)
			{
				if (!s->StartsWith('[') && !s->StartsWith('<') && !s->StartsWith('/'))
				{
					sb->AppendUTF8Char(' ');
				}
				sb->Append(s);
			}
			i++;
		}
		sb->Append(CSTR(">>"));
	}
	NN<IO::StreamData> fd;
	if (!this->fd.SetTo(fd))
	{
		sb->AppendUTF8Char('\r');
		sb->Append(CSTR("endobj\r"));
		return true;
	}
	if (this->streamData)
	{
		sb->Append(CSTR("stream\r\n"));
		sb->Append(CSTR("..."));
		sb->Append(CSTR("\r\nendstream\r"));
	}
	else
	{
		sb->AppendUTF8Char('\r');
		UInt64 len = fd->GetDataSize();
		if (len > 1048576)
		{
			sb->Append(CSTR("...\r\n"));
		}
		else
		{
			sb->AllocLeng((UOSInt)len);
			if (fd->GetRealData(0, len, Data::ByteArray(sb->GetEndPtr(), (UOSInt)len)) == len)
			{
				sb->SetEndPtr(sb->GetEndPtr() + (UOSInt)len);
				return true;
			}
			sb->AppendUTF8Char('\r');
		}
	}
	sb->Append(CSTR("endobj\r"));
	return true;
}

Optional<IO::StreamData> Media::PDFObject::GetRAWData() const
{
	return this->fd;
}

Optional<IO::StreamData> Media::PDFObject::GetData()
{
	NN<IO::StreamData> fd;
	if (this->fd.SetTo(fd) && this->IsFlateDecode())
	{
		if (this->decFd.IsNull())
		{
			IO::MemoryStream memStm;
			Data::Compress::Inflater infStm(memStm, true);
			if (infStm.WriteFromData(fd, 1048576))
			{
				NEW_CLASSOPT(this->decFd, IO::StmData::MemoryDataCopy(memStm.GetArray()));
			}
			else
			{
				printf("Error in decompressing PDF object data\r\n");
				return this->fd;
			}
		}
		return this->decFd;
	}
	return this->fd;
}

Bool Media::PDFObject::SaveFile(Text::CStringNN fileName)
{
	NN<IO::StreamData> fd;
	if (this->fd.SetTo(fd))
	{
		IO::FileStream fs(fileName, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		return this->SaveStream(fs);
	}
	return false;
}

Bool Media::PDFObject::SaveStream(NN<IO::Stream> stm)
{
	NN<IO::StreamData> fd;
	if (this->fd.SetTo(fd))
	{
		NN<Text::String> filter;
		if (this->GetFilter().SetTo(filter) && filter->Equals(UTF8STRC("FlateDecode")))
		{
			Data::Compress::Inflater infStm(stm, true);
			return infStm.WriteFromData(fd, 1048576);
		}
		else
		{
			return stm->WriteFromData(fd, 1048576);
		}
	}
	return false;
}

