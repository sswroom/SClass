#include "Stdafx.h"
#include "Crypto/Encrypt/RC4Cipher.h"
#include "Crypto/Hash/MD5.h"
#include "Media/CCITTFaxDecoder.h"
#include "Media/PDFDocument.h"
#include "Media/StaticImage.h"

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
	NEW_CLASSNN(obj, Media::PDFObject(id));
	this->objMap.Put(id, obj);
	return obj;
}

Optional<Media::PDFObject> Media::PDFDocument::GetObjectById(UInt32 id) const
{
	return this->objMap.Get(id);
}

void Media::PDFDocument::SetEncryptionRequired()
{
	this->encryptionRequired = true;
}

Bool Media::PDFDocument::SetEncryptionKey(UnsafeArray<const UInt8> key, UIntOS keyLen)
{
	if (keyLen == 0 || keyLen > sizeof(this->encryptionKey))
		return false;
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
				UnsafeArray<const UTF8Char> paramPtr = paramStr->v;
				UIntOS paramLen = paramStr->leng;
				while (paramLen > 0 && (*paramPtr == ' ' || *paramPtr == '\r' || *paramPtr == '\n'))
				{
					paramPtr++;
					paramLen--;
				}
				if (paramLen >= 4 && paramPtr[0] == '<' && paramPtr[1] == '<' && paramPtr[paramLen - 2] == '>' && paramPtr[paramLen - 1] == '>')
				{
					Optional<Media::PDFParameter> decodeParams = Media::PDFParameter::Parse(Text::CStringNN(paramPtr + 2, paramLen - 4));
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
			}
			UInt64 dataSize = fd->GetDataSize();
			if (dataSize > (UInt64)INVALID_INDEX)
			{
				printf("PDFDocument: Creating CCITT image failed, image data is too large\r\n");
				return nullptr;
			}
			Data::ByteBuffer dataBuff((UIntOS)dataSize);
			fd->GetRealData(0, dataSize, dataBuff);
			if (!this->DecryptStreamData(id, dataBuff.Arr(), dataBuff.GetSize()))
			{
				printf("PDFDocument: Creating CCITT image failed, encrypted stream key unavailable: id=%d\r\n", id);
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
			printf("PDFDocument: Creating image failed, unsupported filter: %s\r\n", filter->v.Ptr());
			return nullptr;
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
						return imgList;
					}
					else
					{
						printf("PDFDocument: Creating Gray image failed, unexpected data size\r\n");
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
						return imgList;
					}
					else
					{
						printf("PDFDocument: Creating CMYK image failed, unexpected data size\r\n");
					}
				}
				else
				{
					printf("PDFDocument: Creating image failed, unsupported color space\r\n");
				}
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
