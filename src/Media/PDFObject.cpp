#include "Stdafx.h"
#include "Data/Compress/Inflater.h"
#include "Data/Compress/LZWDecStream.h"
#include "IO/FileStream.h"
#include "IO/MemoryStream.h"
#include "IO/StreamDataStream.h"
#include "IO/StmData/MemoryDataCopy.h"
#include "Media/PDFObject.h"
#include "Text/TextBinEnc/ASCII85Enc.h"

Media::PDFObject::PDFObject(UInt32 id)
{
	this->id = id;
	this->streamData = false;
	this->fd = nullptr;
	this->decFd = nullptr;
	this->parameter = nullptr;
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
	return false;
}

Media::PDFObject::ImageType Media::PDFObject::GetImageType() const
{
	NN<Media::PDFParameter> parameter;
	if (!this->parameter.SetTo(parameter))
		return ImageType::NonImage;
	NN<Text::String> s;
	if (parameter->GetEntryValue(CSTR("Subtype")).SetTo(s))
	{
		if (!s->Equals(UTF8STRC("/Image")))
			return ImageType::NonImage;
	}
	else
	{
		return ImageType::NonImage;
	}
	if (parameter->GetEntryValue(CSTR("Filter")).SetTo(s))
	{
		if (s->IndexOf(UTF8STRC("/DCTDecode")) != INVALID_INDEX)
			return ImageType::JPEG;
		if (s->IndexOf(UTF8STRC("/CCITTFaxDecode")) != INVALID_INDEX)
			return ImageType::CCITTFax;
		if (s->Equals(UTF8STRC("/FlateDecode")))
			return ImageType::FlateDecode;
	}
	return ImageType::Unknown;
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
		return nullptr;
	UIntOS i = parameter->GetEntryIndex(CSTR("Type"));
	if (i != INVALID_INDEX)
		return parameter->GetEntryValue(i);
	return nullptr;
}

Optional<Text::String> Media::PDFObject::GetSubtype() const
{
	NN<Media::PDFParameter> parameter;
	if (!this->parameter.SetTo(parameter))
		return nullptr;
	UIntOS i = parameter->GetEntryIndex(CSTR("Subtype"));
	if (i != INVALID_INDEX)
		return parameter->GetEntryValue(i);
	return nullptr;
}

Optional<Text::String> Media::PDFObject::GetFilter() const
{
	NN<Media::PDFParameter> parameter;
	if (!this->parameter.SetTo(parameter))
		return nullptr;
	NN<Text::String> s;
	UIntOS i = parameter->GetEntryIndex(CSTR("Filter"));
	if (i != INVALID_INDEX)
	{
		NN<Media::PDFParameter::ParamEntry> entry = parameter->GetItemNoCheck(i);
		if (entry->value.SetTo(s))
			return s;
		return parameter->GetEntryValue(i);
	}
	return nullptr;
}

Optional<Text::String> Media::PDFObject::GetColorSpace() const
{
	NN<Media::PDFParameter> parameter;
	if (!this->parameter.SetTo(parameter))
		return nullptr;
	UIntOS i = parameter->GetEntryIndex(CSTR("ColorSpace"));
	if (i != INVALID_INDEX)
	{
		return parameter->GetEntryValue(i);
	}
	return nullptr;
}

UIntOS Media::PDFObject::GetBitsPerComponent() const
{
	NN<Media::PDFParameter> parameter;
	if (!this->parameter.SetTo(parameter))
		return 0;
	NN<Text::String> s;
	if (parameter->GetEntryValue(CSTR("BitsPerComponent")).SetTo(s))
		return s->ToUIntOS();
	return 0;
}

UIntOS Media::PDFObject::GetWidth() const
{
	NN<Media::PDFParameter> parameter;
	if (!this->parameter.SetTo(parameter))
		return 0;
	NN<Text::String> s;
	if (parameter->GetEntryValue(CSTR("Width")).SetTo(s))
		return s->ToUIntOS();
	return 0;
}

UIntOS Media::PDFObject::GetHeight() const
{
	NN<Media::PDFParameter> parameter;
	if (!this->parameter.SetTo(parameter))
		return 0;
	NN<Text::String> s;
	if (parameter->GetEntryValue(CSTR("Height")).SetTo(s))
		return s->ToUIntOS();
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
		UIntOS i = 0;
		UIntOS j = param->GetCount();
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
			sb->AllocLeng((UIntOS)len);
			if (fd->GetRealData(0, len, Data::ByteArray(sb->GetEndPtr(), (UIntOS)len)) == len)
			{
				sb->SetEndPtr(sb->GetEndPtr() + (UIntOS)len);
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
	NN<Media::PDFParameter> parameter;
	NN<Text::String> filter;
	NN<IO::StreamData> fd;
	if (this->fd.SetTo(fd) && this->parameter.SetTo(parameter) && parameter->GetEntryValue(CSTR("Filter")).SetTo(filter))
	{
		if (this->decFd.IsNull())
		{
			if (filter->IndexOf(UTF8STRC("/FlateDecode")) != INVALID_INDEX)
			{
				IO::MemoryStream memStm;
				Data::Compress::Inflater infStm(memStm, true);
				if (infStm.WriteFromData(fd, 1048576))
				{
					NEW_CLASSOPT(this->decFd, IO::StmData::MemoryDataCopy(memStm.GetArray()));
				}
				else
				{
					printf("PDFObject: Error in decompressing PDF object data\r\n");
					return this->fd;
				}
			}
			else if (filter->IndexOf(UTF8STRC("/ASCII85Decode")) != INVALID_INDEX)
			{
				Text::TextBinEnc::ASCII85Enc ascii85Enc;
				UIntOS len = fd->GetDataSize();
				UnsafeArray<UInt8> srcArr = MemAllocArr(UInt8, len);
				UnsafeArray<UInt8> decArr = MemAllocArr(UInt8, len * 4);
				fd->GetRealData(0, len, Data::ByteArray(srcArr, len));
				UIntOS decLen = ascii85Enc.DecodeBin(Text::CStringNN(srcArr, len), decArr);
				NEW_CLASSOPT(this->decFd, IO::StmData::MemoryDataCopy(decArr, decLen));
			}
/*			else if (filter->IndexOf(UTF8STRC("/LZWDecode")) != INVALID_INDEX)
			{
				IO::MemoryStream memStm;
				UInt8 buff[2048];
				IO::StreamDataStream stm(fd);
				Data::Compress::LZWDecStream lzwStm(stm, false, 8, 12, 0);
				UIntOS size;
				while ((size = lzwStm.Read(Data::ByteArray(buff, 2048))) > 0)
				{
					memStm.Write(Data::ByteArrayR(buff, size));
				}
				printf("PDFObject: LZWDecode filter processed, size %llu\r\n", memStm.GetLength());
				NEW_CLASSOPT(this->decFd, IO::StmData::MemoryDataCopy(memStm.GetArray()));
			}*/
			else
			{
				printf("PDFObject: Unsupported filter: %s\r\n", filter->v.Ptr());
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
Text::CStringNN Media::PDFObject::ImageTypeGetName(ImageType imgType)
{
	switch (imgType)
	{
	case ImageType::NonImage:
		return CSTR("NonImage");
	case ImageType::Unknown:
		return CSTR("Unknown");
	case ImageType::JPEG:
		return CSTR("JPEG");
	case ImageType::CCITTFax:
		return CSTR("CCITTFax");
	case ImageType::FlateDecode:
		return CSTR("FlateDecode");
	default:
		return CSTR("Unknown");
	}
}

