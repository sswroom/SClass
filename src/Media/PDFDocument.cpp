#include "Stdafx.h"
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
}

Media::PDFDocument::~PDFDocument()
{
	this->version->Release();
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
		else if (filter->Equals(UTF8STRC("/FlateDecode")))
		{
			UIntOS bpc = obj->GetBitsPerComponent();
			UIntOS width = obj->GetWidth();
			UIntOS height = obj->GetHeight();
			NN<Text::String> colorSpace;
			NN<Media::StaticImage> simg;
			NN<Media::ImageList> imgList;
			if (bpc == 0 || width == 0 || height == 0 || !obj->GetColorSpace().SetTo(colorSpace))
			{
				printf("PDFDocument: Creating image failed, invalid image parameters: bpc=%d, width=%d, height=%d\r\n", (Int32)bpc, (Int32)width, (Int32)height);
				return nullptr;
			}
			if (bpc == 8)
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
			else
			{
				printf("PDFDocument: Creating image failed, unsupported bit per component\r\n");
			}
		}
		else
		{
			printf("PDFDocument: Creating image failed, unsupported filter\r\n");
			UIntOS bpc = obj->GetBitsPerComponent();
			UIntOS width = obj->GetWidth();
			UIntOS height = obj->GetHeight();
			NN<Text::String> colorSpace;
			if (bpc == 0 || width == 0 || height == 0)
				return nullptr;
			if (bpc == 8)
			{
				if (!obj->GetColorSpace().SetTo(colorSpace) || colorSpace->Equals(UTF8STRC("/DeviceGray")))
				{

				}
			}
		}
	}
	return nullptr;
}
