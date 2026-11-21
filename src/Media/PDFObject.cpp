#include "Stdafx.h"
#include "Data/Compress/Inflater.h"
#include "Data/Compress/LZWStreamDataDec.h"
#include "IO/BitReaderMSB.h"
#include "IO/FileStream.h"
#include "IO/MemoryStream.h"
#include "IO/StreamDataStream.h"
#include "IO/StmData/MemoryDataCopy.h"
#include "Media/PDFDocument.h"
#include "Media/PDFObject.h"
#include "Media/PDFUtil.h"
#include "Text/TextBinEnc/ASCII85Enc.h"

Bool Media::PDFObject::IsSingleFilter(Text::CStringNN filter, Text::CStringNN target)
{
	Text::CStringNN value = filter.TrimNZ();
	if (value.leng >= 2 && value.v[0] == '[' && value.v[value.leng - 1] == ']')
		value = Text::CStringNN(value.v + 1, value.leng - 2).TrimNZ();
	return value.Equals(target);
}

Bool Media::PDFObject::ApplyPredictor(NN<IO::MemoryStream> decoded, NN<IO::MemoryStream> output, UIntOS predictor, UIntOS colors, UIntOS columns, UIntOS bits)
{
	if (predictor == 1)
		return output->Write(decoded->GetArray()) == decoded->GetLength();
	if (colors == 0 || columns == 0 || (bits != 1 && bits != 2 && bits != 4 && bits != 8 && bits != 16) ||
		colors > INVALID_INDEX / columns || colors * columns > (INVALID_INDEX - 7) / bits)
		return false;
	UIntOS samples = colors * columns;
	UIntOS rowSize = (samples * bits + 7) / 8;
	if (rowSize == 0 || colors > (INVALID_INDEX - 7) / bits)
		return false;
	UIntOS bytesPerPixel = (colors * bits + 7) / 8;
	Data::ByteArrayR input = decoded->GetArray();
	Bool png = predictor >= 10 && predictor <= 15;
	if ((predictor != 2 && !png) || (png && rowSize == INVALID_INDEX))
		return false;
	UIntOS encodedRowSize = rowSize + (png ? 1 : 0);
	if (input.GetSize() % encodedRowSize != 0)
		return false;
	Data::ByteBuffer previous(rowSize);
	Data::ByteBuffer row(rowSize);
	MemClear(previous.Arr().Ptr(), rowSize);
	UIntOS offset = 0;
	while (offset < input.GetSize())
	{
		UInt8 filter = png ? input[offset++] : 0;
		if (filter > 4)
			return false;
		row.CopyFrom(input.SubArray(offset, rowSize));
		offset += rowSize;
		if (png)
		{
			UIntOS i = 0;
			while (i < rowSize)
			{
				UInt8 left = i >= bytesPerPixel ? row[i - bytesPerPixel] : 0;
				UInt8 up = previous[i];
				UInt8 upperLeft = i >= bytesPerPixel ? previous[i - bytesPerPixel] : 0;
				if (filter == 1)
					row[i] = (UInt8)(row[i] + left);
				else if (filter == 2)
					row[i] = (UInt8)(row[i] + up);
				else if (filter == 3)
					row[i] = (UInt8)(row[i] + ((UInt16)left + up) / 2);
				else if (filter == 4)
				{
					Int32 p = (Int32)left + up - upperLeft;
					Int32 pa = p - left; if (pa < 0) pa = -pa;
					Int32 pb = p - up; if (pb < 0) pb = -pb;
					Int32 pc = p - upperLeft; if (pc < 0) pc = -pc;
					row[i] = (UInt8)(row[i] + (pa <= pb && pa <= pc ? left : pb <= pc ? up : upperLeft));
				}
				i++;
			}
		}
		else
		{
			UIntOS i = colors;
			UInt32 mask = ((UInt32)1 << bits) - 1;
			while (i < samples)
			{
				UIntOS bitPos = i * bits;
				UIntOS prevPos = (i - colors) * bits;
				UInt32 value;
				UInt32 prevValue;
				if (bits == 16)
				{
					value = ((UInt32)row[bitPos / 8] << 8) | row[bitPos / 8 + 1];
					prevValue = ((UInt32)row[prevPos / 8] << 8) | row[prevPos / 8 + 1];
					value = (value + prevValue) & mask;
					row[bitPos / 8] = (UInt8)(value >> 8);
					row[bitPos / 8 + 1] = (UInt8)value;
				}
				else
				{
					UIntOS shift = 8 - bits - (bitPos & 7);
					UIntOS prevShift = 8 - bits - (prevPos & 7);
					value = (row[bitPos / 8] >> shift) & mask;
					prevValue = (row[prevPos / 8] >> prevShift) & mask;
					row[bitPos / 8] = (UInt8)((row[bitPos / 8] & ~(mask << shift)) | (((value + prevValue) & mask) << shift));
				}
				i++;
			}
		}
		if (output->Write(row) != rowSize)
			return false;
		previous.CopyFrom(row);
	}
	return true;
}

Bool Media::PDFObject::ReadPredictorParams(NN<PDFParameter> parameter, UIntOS &earlyChange, UIntOS &predictor, UIntOS &colors, UIntOS &columns, UIntOS &bits)
{
	NN<Text::String> value;
	if (!parameter->GetEntryValue(CSTR("DecodeParms")).SetTo(value))
		return true;
	Text::CStringNN ref = value->ToCString().TrimNZ();
	if (ref.leng >= 2 && ref.v[0] == '[' && ref.v[ref.leng - 1] == ']')
		ref = Text::CStringNN(ref.v + 1, ref.leng - 2).TrimNZ();
	Optional<PDFParameter> ownedParams = nullptr;
	NN<PDFParameter> params;
	if (ref.EndsWith(CSTR(" 0 R")))
	{
		UInt32 refId;
		NN<PDFObject> refObj;
		Text::StringBuilderUTF8 refIdStr;
		Text::CStringNN refIdText = Text::CStringNN(ref.v, ref.leng - 4).TrimNZ();
		refIdStr.AppendC(refIdText.v, refIdText.leng);
		if (!refIdStr.ToUInt32(refId))
		{
#if defined(VERBOSE)
			printf("PDFObject: Invalid DecodeParms reference %s\r\n", value->v.Ptr());
#endif
			return false;
		}
		if (!this->document->GetObjectById(refId).SetTo(refObj) || !refObj->GetParameter().SetTo(params))
		{
#if defined(VERBOSE)
			printf("PDFObject: DecodeParms object %u unavailable\r\n", refId);
#endif
			return false;
		}
	}
	else
	{
		if (!PDFParameter::ParseDecodeParams(value, ownedParams))
			return false;
		if (!ownedParams.SetTo(params))
			return true;
	}
	Bool valid = true;
	if (params->GetEntryValue(CSTR("EarlyChange")).SetTo(value))
		valid = value->ToUIntOS(earlyChange) && earlyChange <= 1;
	if (valid && params->GetEntryValue(CSTR("Predictor")).SetTo(value))
		valid = value->ToUIntOS(predictor);
	if (valid && params->GetEntryValue(CSTR("Colors")).SetTo(value))
		valid = value->ToUIntOS(colors);
	if (valid && params->GetEntryValue(CSTR("Columns")).SetTo(value))
		valid = value->ToUIntOS(columns);
	if (valid && params->GetEntryValue(CSTR("BitsPerComponent")).SetTo(value))
		valid = value->ToUIntOS(bits);
	if (!valid)
	{
#if defined(VERBOSE)
		printf("PDFObject: Invalid predictor parameter '%s' in object %d\r\n", value->v.Ptr(), this->id);
#endif
	}
	ownedParams.Delete();
	return valid;
}

Media::PDFObject::PDFObject(UInt32 id, NN<PDFDocument> document)
{
	this->id = id;
	this->document = document;
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
	this->decFd.Delete();
	this->streamData = true;
	this->fd = fd->GetPartialData(ofst, len);
}

void Media::PDFObject::SetData(NN<IO::StreamData> fd, UInt64 ofst, UInt64 len)
{
	this->fd.Delete();
	this->decFd.Delete();
	this->streamData = false;
	this->streamData = false;
	this->fd = fd->GetPartialData(ofst, len);
}

void Media::PDFObject::SetParameter(Optional<PDFParameter> parameter)
{
	this->parameter.Delete();
	this->decFd.Delete();
	this->parameter = parameter;
}

void Media::PDFObject::ClearContents()
{
	this->fd.Delete();
	this->decFd.Delete();
	this->parameter.Delete();
	this->streamData = false;
}

void Media::PDFObject::ClearDecodedData()
{
	this->decFd.Delete();
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

Media::PDFObject::ImageType Media::PDFObject::GetImageType(NN<PDFDocument> doc) const
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
		Optional<Text::String> relStr = nullptr;
		if (s->EndsWith(CSTR(" 0 R")))
		{
			Text::StringBuilderUTF8 sb;
			sb.AppendC(s->v, s->leng - 4);
			UInt32 objId;
			NN<PDFObject> refObj;
			NN<IO::StreamData> fd;
			if (sb.ToUInt32(objId) && doc->GetObjectById(objId).SetTo(refObj) && refObj->GetData().SetTo(fd))
			{
				Data::ByteBuffer data((UIntOS)fd->GetDataSize());
				fd->GetRealData(0, fd->GetDataSize(), data);
				UnsafeArray<UInt8> dataArr = data.Arr();
				UIntOS dataSize = data.GetSize();
				while (dataSize > 0)
				{
					if (dataArr[dataSize - 1] == '\r' || dataArr[dataSize - 1] == '\n')
					{
						dataSize--;
					}
					else
					{
						break;
					}
				}
				relStr = s = Text::String::New(dataArr, dataSize);
			}
		}
		if (s->IndexOf(UTF8STRC("/DCTDecode")) != INVALID_INDEX)
		{
			OPTSTR_DEL(relStr);
			return ImageType::JPEG;
		}
		if (s->IndexOf(UTF8STRC("/CCITTFaxDecode")) != INVALID_INDEX)
		{
			OPTSTR_DEL(relStr);
			return ImageType::CCITTFax;
		}
		if (s->IndexOf(UTF8STRC("/JBIG2Decode")) != INVALID_INDEX)
		{
			OPTSTR_DEL(relStr);
			return ImageType::JBIG2;
		}
		if (IsSingleFilter(s->ToCString(), CSTR("/FlateDecode")))
		{
			OPTSTR_DEL(relStr);
			return ImageType::PlainImage;
		}
		if (IsSingleFilter(s->ToCString(), CSTR("/LZWDecode")))
		{
			OPTSTR_DEL(relStr);
			return ImageType::PlainImage;
		}
		OPTSTR_DEL(relStr);
	}
	else
	{
		return ImageType::PlainImage;
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
	if (!this->fd.SetTo(fd))
		return nullptr;
	if (this->decFd.NotNull())
		return this->decFd;

	Optional<IO::StreamData> decryptedFd = nullptr;
	NN<Text::String> objectType;
	Bool isXRef = this->parameter.SetTo(parameter) && parameter->GetEntryValue(CSTR("Type")).SetTo(objectType) && objectType->Equals(UTF8STRC("/XRef"));
	if (this->streamData && !isXRef && this->document->IsEncryptionRequired())
	{
		UInt64 size = fd->GetDataSize();
		if (size > (UInt64)INVALID_INDEX)
		{
#if defined(VERBOSE)
			printf("PDFObject: Encrypted stream %d is too large\r\n", this->id);
#endif
			return nullptr;
		}
		Data::ByteBuffer data((UIntOS)size);
		if (fd->GetRealData(0, (UIntOS)size, data) != size ||
			!this->document->DecryptStreamData(this->id, data.Arr(), data.GetSize()))
		{
#if defined(VERBOSE)
			printf("PDFObject: Unable to read or decrypt stream %d\r\n", this->id);
#endif
			return nullptr;
		}
		NEW_CLASSOPT(decryptedFd, IO::StmData::MemoryDataCopy(data));
		if (!decryptedFd.SetTo(fd))
			return nullptr;
	}
	if (this->parameter.SetTo(parameter) && parameter->GetEntryValue(CSTR("Filter")).SetTo(filter))
	{
		Text::StringBuilderUTF8 sbFilter;
		sbFilter.Append(filter);
		PDFUtil::ResolveReference(this->document, sbFilter);
		if (sbFilter.IndexOf(UTF8STRC("/LZWDecode")) != INVALID_INDEX)
		{
			UIntOS earlyChange = 1;
			UIntOS predictor = 1;
			UIntOS colors = 1;
			UIntOS columns = 1;
			UIntOS bits = 8;
			Bool valid = IsSingleFilter(sbFilter.ToCString(), CSTR("/LZWDecode")) &&
				this->ReadPredictorParams(parameter, earlyChange, predictor, colors, columns, bits);
			IO::MemoryStream decoded;
			IO::MemoryStream result;
			if (!valid || !Data::Compress::LZWStreamDataDec::Decode(fd, decoded, earlyChange) ||
				!ApplyPredictor(decoded, result, predictor, colors, columns, bits))
			{
#if defined(VERBOSE)
				printf("PDFObject: Error decoding LZW object %d (invalid parameters or data)\r\n", this->id);
#endif
				decryptedFd.Delete();
				return nullptr;
			}
			NEW_CLASSOPT(this->decFd, IO::StmData::MemoryDataCopy(result.GetArray()));
		}
		else if (sbFilter.IndexOf(UTF8STRC("/FlateDecode")) != INVALID_INDEX)
		{
			UIntOS earlyChange = 1;
			UIntOS predictor = 1;
			UIntOS colors = 1;
			UIntOS columns = 1;
			UIntOS bits = 8;
			if (!IsSingleFilter(sbFilter.ToCString(), CSTR("/FlateDecode")) ||
				!this->ReadPredictorParams(parameter, earlyChange, predictor, colors, columns, bits))
			{
#if defined(VERBOSE)
				printf("PDFObject: Invalid Flate filter or DecodeParms for object %d\r\n", this->id);
#endif
				decryptedFd.Delete();
				return nullptr;
			}
			UInt64 streamSize = fd->GetDataSize();
			UIntOS trailingEOLSize = 0;
			UInt8 tail[2];
			if (streamSize >= 2 && fd->GetRealData(streamSize - 2, 2, Data::ByteArray(tail, 2)) == 2)
			{
				if (tail[1] == '\n')
				{
					if (tail[0] == '\r')
						trailingEOLSize = 2;
					else
						trailingEOLSize = 1;
				}
				else if (tail[1] == '\r')
				{
					trailingEOLSize = 1;
				}
			}

			Bool decompressed = false;
			// Retry without a trailing stream EOL for PDFs that include it in /Length.
			UIntOS attempt = 0;
			while (attempt <= trailingEOLSize)
			{
				IO::MemoryStream memStm;
				NN<IO::StreamData> input = fd->GetPartialData(0, streamSize - attempt);
				Data::Compress::Inflater infStm(memStm, true);
				decompressed = infStm.WriteFromData(input, 1048576) && infStm.IsEnd();
				input.Delete();
				if (decompressed)
				{
					IO::MemoryStream result;
					if (!ApplyPredictor(memStm, result, predictor, colors, columns, bits))
					{
#if defined(VERBOSE)
						printf("PDFObject: Invalid Flate predictor data for object %d\r\n", this->id);
#endif
						decryptedFd.Delete();
						return nullptr;
					}
					NEW_CLASSOPT(this->decFd, IO::StmData::MemoryDataCopy(result.GetArray()));
					break;
				}
				attempt++;
			}
			if (!decompressed)
			{
#if defined(VERBOSE)
				printf("PDFObject: Error in decompressing PDF object %d data with %s\r\n", this->id, sbFilter.v.Ptr());
#endif
				decryptedFd.Delete();
				return nullptr;
			}
		}
		else if (sbFilter.IndexOf(UTF8STRC("/ASCII85Decode")) != INVALID_INDEX)
		{
			Text::TextBinEnc::ASCII85Enc ascii85Enc;
			UIntOS len = fd->GetDataSize();
			UnsafeArray<UInt8> srcArr = MemAllocArr(UInt8, len);
			UnsafeArray<UInt8> decArr = MemAllocArr(UInt8, len * 4);
			fd->GetRealData(0, len, Data::ByteArray(srcArr, len));
			UIntOS decLen = ascii85Enc.DecodeBin(Text::CStringNN(srcArr, len), decArr);
			NEW_CLASSOPT(this->decFd, IO::StmData::MemoryDataCopy(decArr, decLen));
			MemFreeArr(srcArr);
			MemFreeArr(decArr);
		}
		else if (sbFilter.IndexOf(UTF8STRC("/DCTDecode")) != INVALID_INDEX)
		{
			this->decFd = decryptedFd;
			return this->decFd.NotNull() ? this->decFd : this->fd;
		}
		else if (sbFilter.IndexOf(UTF8STRC("/CCITTFaxDecode")) != INVALID_INDEX)
		{
			this->decFd = decryptedFd;
			return this->decFd.NotNull() ? this->decFd : this->fd;
		}
		else if (sbFilter.IndexOf(UTF8STRC("/JBIG2Decode")) != INVALID_INDEX)
		{
			this->decFd = decryptedFd;
			return this->decFd.NotNull() ? this->decFd : this->fd;
		}
		else
		{
			printf("PDFObject: Unsupported filter: %s\r\n", sbFilter.v.Ptr());
			decryptedFd.Delete();
			return nullptr;
		}
		decryptedFd.Delete();
		return this->decFd;
	}
	if (decryptedFd.NotNull())
	{
		this->decFd = decryptedFd;
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
	if (this->GetData().SetTo(fd))
	{
		return stm->WriteFromData(fd, 1048576);
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
	case ImageType::JBIG2:
		return CSTR("JBIG2");
	case ImageType::PlainImage:
		return CSTR("PlainImage");
	default:
		return CSTR("Unknown");
	}
}
