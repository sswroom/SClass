#include "Stdafx.h"
#include "Crypto/Encrypt/RC4Cipher.h"
#include "Crypto/Hash/MD5.h"
#include "Data/ByteBuffer.h"
#include "Media/CCITTFaxDecoder.h"
#include "Media/JBIG2Decoder.h"
#include "Media/PDFDocument.h"
#include "Media/StaticImage.h"

void Media::PDFDocument::SkipPDFWhitespace(UnsafeArray<const UTF8Char> str, UIntOS length, UIntOS &pos)
{
	while (pos < length && (str[pos] == ' ' || str[pos] == '\t' || str[pos] == '\r' || str[pos] == '\n' || str[pos] == '\f'))
		pos++;
}

Bool Media::PDFDocument::ReadPDFNumberToken(UnsafeArray<const UTF8Char> str, UIntOS length, UIntOS &pos, UIntOS &start, UIntOS &end)
{
	SkipPDFWhitespace(str, length, pos);
	start = pos;
	while (pos < length && str[pos] != ' ' && str[pos] != '\t' && str[pos] != '\r' && str[pos] != '\n' && str[pos] != '\f' && str[pos] != ']')
		pos++;
	end = pos;
	return start != end;
}

Bool Media::PDFDocument::IsPDFDecodeEndpoint(UnsafeArray<const UTF8Char> str, UIntOS start, UIntOS end, UInt8 endpoint)
{
	Bool negative = false;
	if (start < end && (str[start] == '+' || str[start] == '-'))
	{
		negative = str[start] == '-';
		start++;
	}
	Bool hasDigit = false;
	UInt8 integer = 0;
	while (start < end && str[start] >= '0' && str[start] <= '9')
	{
		hasDigit = true;
		if (integer > 1)
			return false;
		integer = (UInt8)(integer * 10 + str[start] - '0');
		start++;
	}
	if (start < end && str[start] == '.')
	{
		start++;
		while (start < end && str[start] >= '0' && str[start] <= '9')
		{
			hasDigit = true;
			if (str[start] != '0')
				return false;
			start++;
		}
	}
	return start == end && hasDigit && integer == endpoint && (!negative || endpoint == 0);
}

Bool Media::PDFDocument::IsReverseDecodeArray(Text::CStringNN value)
{
	UnsafeArray<const UTF8Char> str = value.v;
	UIntOS pos = 0;
	SkipPDFWhitespace(str, value.leng, pos);
	if (pos >= value.leng || str[pos++] != '[')
		return false;
	UIntOS firstStart;
	UIntOS firstEnd;
	UIntOS secondStart;
	UIntOS secondEnd;
	if (!ReadPDFNumberToken(str, value.leng, pos, firstStart, firstEnd) ||
		!ReadPDFNumberToken(str, value.leng, pos, secondStart, secondEnd))
		return false;
	SkipPDFWhitespace(str, value.leng, pos);
	if (pos >= value.leng || str[pos++] != ']')
		return false;
	SkipPDFWhitespace(str, value.leng, pos);
	return pos == value.leng &&
		IsPDFDecodeEndpoint(str, firstStart, firstEnd, 1) &&
		IsPDFDecodeEndpoint(str, secondStart, secondEnd, 0);
}

Optional<IO::ParsedObject> Media::PDFDocument::SetPObjName(Optional<IO::ParsedObject> pobj, UInt32 objId, Text::CStringNN ext)
{
	NN<IO::ParsedObject> nnpobj;
	if (pobj.SetTo(nnpobj))
	{
		Text::StringBuilderUTF8 sb;
		sb.Append(this->sourceName);
		sb.AppendUTF8Char('.');
		sb.AppendU32(objId);
		if (ext.leng > 0)
		{
			sb.Append(ext);
		}
		nnpobj->SetSourceName(sb.ToCString());
	}
	return pobj;
}

Media::PDFDocument::PDFDocument(NN<Text::String> sourceName, Text::CStringNN version) : IO::ParsedObject(sourceName)
{
	this->version = Text::String::New(version);
	this->encryptionRequired = false;
	this->encryptionKeyLen = 0;
	MemClear(this->encryptionKey, sizeof(this->encryptionKey));
}

Media::PDFDocument::~PDFDocument()
{
	this->version->Release();
	MemClear(this->encryptionKey, sizeof(this->encryptionKey));
	UIntOS i = this->objMap.GetCount();
	NN<Media::PDFObject> obj;
	while (i-- > 0)
	{
		obj = this->objMap.GetItemNoCheck(i);
		obj.Delete();
	}
}

IO::ParserType Media::PDFDocument::GetParserType() const
{
	return IO::ParserType::PDFDocument;
}

UIntOS Media::PDFDocument::GetCount() const
{
	return this->objMap.GetCount();
}

NN<Media::PDFObject> Media::PDFDocument::GetItemNoCheck(UIntOS index) const
{
	return this->objMap.GetItemNoCheck(index);
}

Optional<Media::PDFObject> Media::PDFDocument::GetItem(UIntOS index) const
{
	return this->objMap.GetItem(index);
}

NN<Media::PDFObject> Media::PDFDocument::AddObject(UInt32 id)
{
	NN<Media::PDFObject> obj;
	if (this->objMap.Get(id).SetTo(obj))
		return obj;
	NEW_CLASSNN(obj, Media::PDFObject(id, *this));
	this->objMap.Put(id, obj);
	return obj;
}

Optional<Media::PDFObject> Media::PDFDocument::GetObjectById(UInt32 id) const
{
	return this->objMap.Get(id);
}

void Media::PDFDocument::SetEncryptionRequired()
{
	this->ClearCachedStreams();
	this->encryptionRequired = true;
}

void Media::PDFDocument::ClearCachedStreams()
{
	UIntOS i = this->objMap.GetCount();
	while (i-- > 0)
		this->objMap.GetItemNoCheck(i)->ClearDecodedData();
}

Bool Media::PDFDocument::IsEncryptionRequired() const
{
	return this->encryptionRequired;
}

Bool Media::PDFDocument::SetEncryptionKey(UnsafeArray<const UInt8> key, UIntOS keyLen)
{
	if (keyLen == 0 || keyLen > sizeof(this->encryptionKey))
		return false;
	this->ClearCachedStreams();
	MemCopyNO(this->encryptionKey, key.Ptr(), keyLen);
	this->encryptionKeyLen = keyLen;
	return true;
}

Bool Media::PDFDocument::DecryptStreamData(UInt32 objId, UnsafeArray<UInt8> data, UIntOS dataSize) const
{
	if (!this->encryptionRequired)
		return true;
	if (this->encryptionKeyLen == 0)
		return false;

	UInt8 keyData[21];
	MemCopyNO(keyData, this->encryptionKey, this->encryptionKeyLen);
	keyData[this->encryptionKeyLen] = (UInt8)objId;
	keyData[this->encryptionKeyLen + 1] = (UInt8)(objId >> 8);
	keyData[this->encryptionKeyLen + 2] = (UInt8)(objId >> 16);
	keyData[this->encryptionKeyLen + 3] = 0;
	keyData[this->encryptionKeyLen + 4] = 0;
	UInt8 digest[16];
	Crypto::Hash::MD5 md5;
	md5.Calc(UnsafeArray<const UInt8>(keyData), this->encryptionKeyLen + 5);
	md5.GetValue(digest);
	UIntOS objKeyLen = this->encryptionKeyLen + 5;
	if (objKeyLen > 16)
		objKeyLen = 16;
	Crypto::Encrypt::RC4Cipher cipher(UnsafeArray<const UInt8>(digest), objKeyLen);
	cipher.Decrypt(UnsafeArray<const UInt8>(data), dataSize, data);
	return true;
}

Optional<Media::ImageList> Media::PDFDocument::CreateImage(UInt32 id, NN<Parser::ParserList> parsers)
{
	NN<Media::PDFObject> obj;
	if (!this->objMap.Get(id).SetTo(obj))
		return nullptr;
	if (!obj->IsImage())
		return nullptr;
	NN<IO::StreamData> fd;
	NN<Text::String> filter;
	if (obj->GetData().SetTo(fd) && obj->GetFilter().SetTo(filter))
	{
		if (filter->IndexOf(UTF8STRC("/DCTDecode")) != INVALID_INDEX)
		{
			return Optional<Media::ImageList>::ConvertFrom(SetPObjName(parsers->ParseFileType(fd, IO::ParserType::ImageList), id, CSTR(".jpg")));
		}
		else if (filter->IndexOf(UTF8STRC("/CCITTFaxDecode")) != INVALID_INDEX)
		{
			UIntOS width = obj->GetWidth();
			UIntOS height = obj->GetHeight();
			NN<Media::PDFParameter> params;
			NN<Text::String> paramStr;
			Int32 k = 0;
			Bool endOfLine = false;
			Bool encodedByteAlign = false;
			if (width == 0 || height == 0)
			{
				printf("PDFDocument: Creating CCITT image failed, invalid dimensions\r\n");
				return nullptr;
			}
			if (obj->GetParameter().SetTo(params) && params->GetEntryValue(CSTR("DecodeParms")).SetTo(paramStr))
			{
				Optional<Media::PDFParameter> decodeParams = nullptr;
				if (!Media::PDFParameter::ParseDecodeParams(paramStr, decodeParams))
				{
					printf("PDFDocument: Creating CCITT image failed, invalid DecodeParms\r\n");
					return nullptr;
				}
				NN<Media::PDFParameter> nnDecodeParams;
				if (decodeParams.SetTo(nnDecodeParams))
				{
					if (nnDecodeParams->GetEntryValue(CSTR("K")).SetTo(paramStr))
						k = paramStr->ToInt32();
					if (nnDecodeParams->GetEntryValue(CSTR("EndOfLine")).SetTo(paramStr))
						endOfLine = paramStr->Equals(UTF8STRC("true"));
					if (nnDecodeParams->GetEntryValue(CSTR("EncodedByteAlign")).SetTo(paramStr))
						encodedByteAlign = paramStr->Equals(UTF8STRC("true"));
					if (nnDecodeParams->GetEntryValue(CSTR("Columns")).SetTo(paramStr) && paramStr->ToUIntOS() != width)
					{
						printf("PDFDocument: Creating CCITT image failed, Columns does not match image width\r\n");
						decodeParams.Delete();
						return nullptr;
					}
					if (nnDecodeParams->GetEntryValue(CSTR("Rows")).SetTo(paramStr) && paramStr->ToUIntOS() != 0 && paramStr->ToUIntOS() != height)
					{
						printf("PDFDocument: Creating CCITT image failed, Rows does not match image height\r\n");
						decodeParams.Delete();
						return nullptr;
					}
				}
				decodeParams.Delete();
			}
			UInt64 dataSize = fd->GetDataSize();
			if (dataSize > (UInt64)INVALID_INDEX)
			{
				printf("PDFDocument: Creating CCITT image failed, image data is too large\r\n");
				return nullptr;
			}
			Data::ByteBuffer dataBuff((UIntOS)dataSize);
			if (fd->GetRealData(0, dataSize, dataBuff) != dataSize)
			{
				printf("PDFDocument: Creating CCITT image failed, unable to read image data: id=%d\r\n", id);
				return nullptr;
			}
			Media::CCITTFaxDecoder decoder;
			NN<Media::StaticImage> simg;
			if (!decoder.DecodeImage(dataBuff, width, height, k, endOfLine, encodedByteAlign).SetTo(simg))
			{
				printf("PDFDocument: Creating CCITT image failed, invalid encoded data: id=%d\r\n", id);
				return nullptr;
			}
			NN<Media::ImageList> imgList;
			NEW_CLASSNN(imgList, Media::ImageList(CSTR("Image.ccitt")));
			imgList->AddImage(simg, 0);
			return imgList;
		}
		else if (filter->IndexOf(UTF8STRC("/JBIG2Decode")) != INVALID_INDEX)
		{
			UIntOS width = obj->GetWidth();
			UIntOS height = obj->GetHeight();
			if (width == 0 || height == 0 || width > INVALID_INDEX / height)
			{
				printf("PDFDocument: Creating JBIG2 image failed, invalid dimensions\r\n");
				return nullptr;
			}

			Data::ByteBuffer globalData;
			Bool hasGlobalData = false;
			NN<Media::PDFParameter> params;
			NN<Text::String> paramStr;
			if (obj->GetParameter().SetTo(params) && params->GetEntryValue(CSTR("DecodeParms")).SetTo(paramStr))
			{
				Optional<Media::PDFParameter> decodeParams = nullptr;
				if (!Media::PDFParameter::ParseDecodeParams(paramStr, decodeParams))
				{
					printf("PDFDocument: Creating JBIG2 image failed, invalid DecodeParms\r\n");
					return nullptr;
				}
				NN<Media::PDFParameter> nnDecodeParams;
				if (decodeParams.SetTo(nnDecodeParams))
				{
					NN<Text::String> globalRef;
					NN<Media::PDFObject> globalObj;
					NN<IO::StreamData> globalStream;
					UInt32 globalObjId;
					if (nnDecodeParams->GetEntryValue(CSTR("JBIG2Globals")).SetTo(globalRef))
					{
						if (!globalRef->EndsWith(CSTR(" 0 R")) ||
							!Text::CStringNN(globalRef->v, globalRef->leng - 4).ToUInt32(globalObjId) ||
							!this->GetObjectById(globalObjId).SetTo(globalObj) ||
							!globalObj->GetData().SetTo(globalStream))
						{
							printf("PDFDocument: Creating JBIG2 image failed, invalid JBIG2Globals reference\r\n");
							decodeParams.Delete();
							return nullptr;
						}
						UInt64 globalSize = globalStream->GetDataSize();
						if (globalSize == 0 || globalSize > (UInt64)INVALID_INDEX)
						{
							printf("PDFDocument: Creating JBIG2 image failed, invalid JBIG2Globals stream size\r\n");
							decodeParams.Delete();
							return nullptr;
						}
						globalData.ChangeSizeAndIgnoreData((UIntOS)globalSize);
						if (globalStream->GetRealData(0, globalSize, globalData) != globalSize)
						{
							printf("PDFDocument: Creating JBIG2 image failed, unable to read JBIG2Globals stream\r\n");
							decodeParams.Delete();
							return nullptr;
						}
						hasGlobalData = true;
					}
				}
				decodeParams.Delete();
			}

			UInt64 dataSize = fd->GetDataSize();
			if (dataSize == 0 || dataSize > (UInt64)INVALID_INDEX)
			{
				printf("PDFDocument: Creating JBIG2 image failed, invalid image data size\r\n");
				return nullptr;
			}
			Data::ByteBuffer dataBuff((UIntOS)dataSize);
			if (fd->GetRealData(0, dataSize, dataBuff) != dataSize)
			{
				printf("PDFDocument: Creating JBIG2 image failed, unable to read image data: id=%d\r\n", id);
				return nullptr;
			}

			Media::JBIG2Decoder decoder;
			NN<Media::StaticImage> simg;
			if (hasGlobalData)
			{
				if (!decoder.DecodeImage(dataBuff, width, height, globalData).SetTo(simg))
				{
					printf("PDFDocument: Creating JBIG2 image failed, invalid encoded data or decoder unavailable: id=%d\r\n", id);
					return nullptr;
				}
			}
			else
			{
				if (!decoder.DecodeImage(dataBuff, width, height).SetTo(simg))
				{
					printf("PDFDocument: Creating JBIG2 image failed, invalid encoded data or decoder unavailable: id=%d\r\n", id);
					return nullptr;
				}
			}
			NN<Media::PDFParameter> imageParams;
			NN<Text::String> decodeArray;
			Bool reverseDecode = false;
			if (obj->GetParameter().SetTo(imageParams) && imageParams->GetEntryValue(CSTR("Decode")).SetTo(decodeArray))
			{
				reverseDecode = IsReverseDecodeArray(Text::CStringNN(decodeArray->v, decodeArray->leng));
			}
			if (reverseDecode)
			{
				UIntOS bpl = simg->GetDataBpl();
				UIntOS y = 0;
				while (y < height)
				{
					UIntOS x = 0;
					while (x < width)
					{
						simg->data[y * bpl + x] = (UInt8)(255 - simg->data[y * bpl + x]);
						x++;
					}
					y++;
				}
			}
			NN<Media::ImageList> imgList;
			NEW_CLASSNN(imgList, Media::ImageList(CSTR("Image.jbig2")));
			imgList->AddImage(simg, 0);
			return imgList;
		}
		else
		{
			UIntOS bpc = obj->GetBitsPerComponent();
			UIntOS width = obj->GetWidth();
			UIntOS height = obj->GetHeight();
			NN<Text::String> colorSpace;
			NN<Media::StaticImage> simg;
			NN<Media::ImageList> imgList;
			if (bpc == 0 || width == 0 || height == 0)
			{
				printf("PDFDocument: Creating image failed, invalid image parameters: bpc=%d, width=%d, height=%d\r\n", (Int32)bpc, (Int32)width, (Int32)height);
				return nullptr;
			}
			if (bpc == 8 && obj->GetColorSpace().SetTo(colorSpace))
			{
				Optional<Text::String> relStr = nullptr;
				while (colorSpace->EndsWith(CSTR(" R")))
				{
					UTF8Char sbuff[32];
					UnsafeArray<UTF8Char> sptr;
					if (colorSpace->leng > 20)
					{
						printf("PDFDocument: Color space reference too long: %s\r\n", colorSpace->v.Ptr());
						break;
					}
					sptr = colorSpace->ConcatTo(sbuff);
					sptr -= 2;
					sptr[0] = 0;
					UIntOS i = Text::StrLastIndexOfC(sbuff, (UIntOS)(sptr - sbuff), ' ');
					if (i == INVALID_INDEX)
					{
						printf("PDFDocument: Invalid color space reference: %s\r\n", colorSpace->v.Ptr());
						break;
					}
					sbuff[i] = 0;
					UInt32 id;
					if (!Text::StrToUInt32(sbuff, id))
					{	
						printf("PDFDocument: Invalid color space reference ID: %s\r\n", sbuff);
						break;
					}
					NN<Media::PDFObject> refObj;
					if (!this->GetObjectById(id).SetTo(refObj))
					{
						printf("PDFDocument: Failed to get color space reference object: %s\r\n", sbuff);
						break;
					}
					NN<IO::StreamData> refData;
					if (!refObj->GetData().SetTo(refData))
					{
						printf("PDFDocument: Failed to get color space reference data: %s\r\n", sbuff);
						break;
					}
					UnsafeArray<UInt8> refArr = MemAllocArr(UInt8, (UIntOS)refData->GetDataSize() + 1);
					refData->GetRealData(0, (UIntOS)refData->GetDataSize(), Data::ByteArray(refArr, (UIntOS)refData->GetDataSize()));
					refArr[(UIntOS)refData->GetDataSize()] = 0;
					Text::StrReplace(refArr, '\r', ' ');
					Text::StrReplace(refArr, '\n', ' ');
					colorSpace = Text::String::New(refArr, (UIntOS)refData->GetDataSize());
					MemFreeArr(refArr);
					OPTSTR_DEL(relStr);
					relStr = colorSpace;
				}

				if (colorSpace->Equals(UTF8STRC("/DeviceGray")))
				{
					if (fd->GetDataSize() == (width * height))
					{
						Media::ColorProfile color(Media::ColorProfile::CPT_PUNKNOWN);
						NEW_CLASSNN(simg, Media::StaticImage(Math::Size2D<UIntOS>(width, height), 0, 8, Media::PixelFormat::PF_PAL_W8, width * height, color, Media::ColorProfile::YUVT_UNKNOWN, Media::AlphaType::AT_IGNORE_ALPHA, Media::YCOFST_C_CENTER_LEFT));
						fd->GetRealData(0, width * height, simg->GetDataArray());
						UnsafeArray<UInt8> pal;
						if (simg->pal.SetTo(pal))
						{
							UIntOS i = 0;
							UIntOS j = 256;
							while (i < j)
							{
								pal[i * 4] = (UInt8)i;
								pal[i * 4 + 1] = (UInt8)i;
								pal[i * 4 + 2] = (UInt8)i;
								pal[i * 4 + 3] = 255;
								i++;
							}
						}
						NEW_CLASSNN(imgList, Media::ImageList(CSTR("Image.raw")));
						imgList->AddImage(simg, 0);
						OPTSTR_DEL(relStr);
						return imgList;
					}
					else
					{
						printf("PDFDocument: Creating Gray image failed, unexpected data size, width=%d, height=%d, size=%llu\r\n", (UInt32)width, (UInt32)height, fd->GetDataSize());
					}
				}
				else if (colorSpace->Equals(UTF8STRC("/DeviceCMYK")))
				{
					if (fd->GetDataSize() == (width * height * 4))
					{
						Media::ColorProfile color(Media::ColorProfile::CPT_PUNKNOWN);
						NEW_CLASSNN(simg, Media::StaticImage(Math::Size2D<UIntOS>(width, height), 0, 24, Media::PixelFormat::PF_B8G8R8, width * height, color, Media::ColorProfile::YUVT_UNKNOWN, Media::AlphaType::AT_IGNORE_ALPHA, Media::YCOFST_C_CENTER_LEFT));
						UnsafeArray<UInt8> cmyk = MemAllocArr(UInt8, width * 4);
						UnsafeArray<UInt8> bytePtr = simg->data;
						UIntOS ofst = 0;
						UIntOS currH = 0;
						while (currH < height)
						{
							fd->GetRealData(ofst, width * 4, Data::ByteArray(cmyk, width * 4));
							UIntOS i = 0;
							while (i < width)
							{
								UInt8 c = cmyk[i * 4];
								UInt8 m = cmyk[i * 4 + 1];
								UInt8 y = cmyk[i * 4 + 2];
								UInt8 k = cmyk[i * 4 + 3];
								bytePtr[i * 3] = (UInt8)(((255 - y) * (255 - k)) / 255);
								bytePtr[i * 3 + 1] = (UInt8)(((255 - m) * (255 - k)) / 255);
								bytePtr[i * 3 + 2] = (UInt8)(((255 - c) * (255 - k)) / 255);
								i++;
							}
							bytePtr += width * 3;
							ofst += width * 4;
							currH++;
						}
						MemFreeArr(cmyk);
						NEW_CLASSNN(imgList, Media::ImageList(CSTR("Image.raw")));
						imgList->AddImage(simg, 0);
						OPTSTR_DEL(relStr);
						return imgList;
					}
					else
					{
						printf("PDFDocument: Creating CMYK image failed, unexpected data size, width=%d, height=%d, size=%llu\r\n", (UInt32)width, (UInt32)height, fd->GetDataSize());
					}
				}
				else if (colorSpace->Equals(UTF8STRC("/DeviceRGB")))
				{
					if (fd->GetDataSize() == (width * height * 3))
					{
						Media::ColorProfile color(Media::ColorProfile::CPT_PUNKNOWN);
						NEW_CLASSNN(simg, Media::StaticImage(Math::Size2D<UIntOS>(width, height), 0, 24, Media::PixelFormat::PF_B8G8R8, width * height, color, Media::ColorProfile::YUVT_UNKNOWN, Media::AlphaType::AT_IGNORE_ALPHA, Media::YCOFST_C_CENTER_LEFT));
						Data::ByteBuffer rgb(width * 3);
						UIntOS row = 0;
						while (row < height)
						{
							if (fd->GetRealData(row * width * 3, width * 3, rgb) != width * 3)
							{
								printf("PDFDocument: Unable to read RGB image data\r\n");
								simg.Delete();
								OPTSTR_DEL(relStr);
								return nullptr;
							}
							UnsafeArray<UInt8> bytePtr = simg->data + row * simg->GetDataBpl();
							UIntOS x = 0;
							while (x < width)
							{
								bytePtr[x * 3] = rgb[x * 3 + 2];
								bytePtr[x * 3 + 1] = rgb[x * 3 + 1];
								bytePtr[x * 3 + 2] = rgb[x * 3];
								x++;
							}
							row++;
						}
						NEW_CLASSNN(imgList, Media::ImageList(CSTR("Image.raw")));
						imgList->AddImage(simg, 0);
						OPTSTR_DEL(relStr);
						return imgList;
					}
					else
					{
						printf("PDFDocument: Creating RGB image failed, unexpected data size, width=%d, height=%d, size=%llu\r\n", (UInt32)width, (UInt32)height, fd->GetDataSize());
					}
				}
				else
				{
					printf("PDFDocument: Creating image failed, unsupported color space: %s\r\n", colorSpace->v.Ptr());
				}
				OPTSTR_DEL(relStr);
			}
			else if (bpc == 1)
			{
				UIntOS bpl = (width + 7) / 8;
				if (fd->GetDataSize() == (bpl * height))
				{
					Media::ColorProfile color(Media::ColorProfile::CPT_PUNKNOWN);
					NEW_CLASSNN(simg, Media::StaticImage(Math::Size2D<UIntOS>(width, height), 0, 1, Media::PixelFormat::PF_PAL_1, bpl * height, color, Media::ColorProfile::YUVT_UNKNOWN, Media::AlphaType::AT_IGNORE_ALPHA, Media::YCOFST_C_CENTER_LEFT));
					fd->GetRealData(0, bpl * height, simg->GetDataArray());
					UnsafeArray<UInt8> pal;
					if (simg->pal.SetTo(pal))
					{
						pal[0] = 0;
						pal[1] = 0;
						pal[2] = 0;
						pal[3] = 255;
						pal[4] = 255;
						pal[5] = 255;
						pal[6] = 255;
						pal[7] = 255;
					}
					NEW_CLASSNN(imgList, Media::ImageList(CSTR("Image.raw")));
					imgList->AddImage(simg, 0);
					return imgList;
				}
				else
				{
					printf("PDFDocument: Creating Gray image failed, unexpected data size\r\n");
				}
			}
			else
			{
				printf("PDFDocument: Creating image failed, unsupported bit per component: bpc=%d, width=%d, height=%d, size=%llu\r\n", (UInt32)bpc, (UInt32)width, (UInt32)height, fd->GetDataSize());
			}
		}
	}
	return nullptr;
}
