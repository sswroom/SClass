#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/Path.h"
#include "IO/StmData/MemoryDataRef.h"
#include "Media/CCITTFaxDecoder.h"
#include "Media/ICCProfile.h"
#include "Media/JBIG2Decoder.h"
#include "Media/PDFDocument.h"
#include "Media/PDFImage.h"
#include "Media/StaticImage.h"
#include "Parser/FullParserList.h"
#include "Parser/FileParser/PDFParser.h"

Int32 CCITTFaxTest()
{
	UInt8 faxData[] = {0x83};
	Media::CCITTFaxDecoder faxDecoder;
	NN<Media::StaticImage> faxImage;
	if (!faxDecoder.DecodeImage(Data::ByteArrayR(faxData, sizeof(faxData)), 8, 1).SetTo(faxImage))
	{
		printf("CCITT fax decode test failed\r\n");
		return 1;
	}
	Bool faxTestPassed = faxImage->data[0] == 255 && faxImage->data[1] == 255 && faxImage->data[2] == 255 &&
		faxImage->data[3] == 0 && faxImage->data[4] == 0 && faxImage->data[5] == 0 && faxImage->data[6] == 0 && faxImage->data[7] == 0;
	faxImage.Delete();
	if (!faxTestPassed)
	{
		printf("CCITT fax decode test failed\r\n");
		return 1;
	}
	UInt8 fax4Data[] = {0x30, 0x78};
	if (!faxDecoder.DecodeImage(Data::ByteArrayR(fax4Data, sizeof(fax4Data)), 8, 2, -1).SetTo(faxImage))
	{
		printf("CCITT Group 4 decode test failed\r\n");
		return 1;
	}
	UIntOS faxBpl = faxImage->GetDataBpl();
	faxTestPassed = true;
	UIntOS x = 0;
	while (x < 8)
	{
		UInt8 expected = x < 3 ? 255 : 0;
		if (faxImage->data[x] != expected || faxImage->data[faxBpl + x] != expected)
		{
			faxTestPassed = false;
			break;
		}
		x++;
	}
	faxImage.Delete();
	if (!faxTestPassed)
	{
		printf("CCITT Group 4 decode test failed\r\n");
		return 1;
	}
	UInt8 faxBoundaryData[] = {0xA8};
	if (!faxDecoder.DecodeImage(Data::ByteArrayR(faxBoundaryData, sizeof(faxBoundaryData)), 8, 2, -1).SetTo(faxImage))
	{
		printf("CCITT Group 4 reference boundary test failed\r\n");
		return 1;
	}
	faxBpl = faxImage->GetDataBpl();
	faxTestPassed = true;
	x = 0;
	while (x < 8)
	{
		if (faxImage->data[x] != 255 || faxImage->data[faxBpl + x] != (x == 7 ? 0 : 255))
			faxTestPassed = false;
		x++;
	}
	faxImage.Delete();
	if (!faxTestPassed)
	{
		printf("CCITT Group 4 reference boundary test failed\r\n");
		return 1;
	}
	UInt8 faxGroup4WhiteRow[] = {0x60};
	if (!faxDecoder.DecodeImage(Data::ByteArrayR(faxGroup4WhiteRow, sizeof(faxGroup4WhiteRow)), 8, 1, -1).SetTo(faxImage))
	{
		printf("CCITT Group 4 white row decode test failed\r\n");
		return 1;
	}
	faxTestPassed = true;
	x = 0;
	while (x < 8)
	{
		if (faxImage->data[x++] != 255)
		{
			faxTestPassed = false;
			break;
		}
	}
	faxImage.Delete();
	if (!faxTestPassed)
	{
		printf("CCITT Group 4 white row decode test failed\r\n");
		return 1;
	}
	UInt8 fax3Data[] = {0xC1, 0xB0};
	if (!faxDecoder.DecodeImage(Data::ByteArrayR(fax3Data, sizeof(fax3Data)), 8, 2, 2).SetTo(faxImage))
	{
		printf("CCITT Group 3 mixed decode test failed\r\n");
		return 1;
	}
	faxTestPassed = true;
	x = 0;
	while (x < 8)
	{
		UInt8 expected = x < 3 ? 255 : 0;
		if (faxImage->data[x] != expected || faxImage->data[faxBpl + x] != expected)
		{
			faxTestPassed = false;
			break;
		}
		x++;
	}
	faxImage.Delete();
	if (!faxTestPassed)
	{
		printf("CCITT Group 3 mixed decode test failed\r\n");
		return 1;
	}
	UInt8 faxImageData[] =
	{
		0xF9,0x48,0x38,0x25,0x23,0x02,0x56,0x49,0xDE,0x74,0xBF,0xEB,0x3C,0x7F,0x23,0x87,
		0xE8,0x75,0x36,0x26,0x44,0x7D,0x13,0xEE,0x17,0x14,0x61,0xF4,0xBA,0x5D,0x2E,0x97,
		0x4B,0xA5,0xC2,0x5D,0x2E,0x10,0x5D,0x05,0xC2,0x0B,0x8F,0x25,0xD7,0x7B,0xDA,0xBC,
		0x7F,0xFC,0x00,0x40,0x04,0x0A
	};
	if (!faxDecoder.DecodeImage(Data::ByteArrayR(faxImageData, sizeof(faxImageData)), 59, 50, -1).SetTo(faxImage))
	{
		printf("CCITT Group 4 image decode test failed\r\n");
		return 1;
	}
	UInt32 faxHash = 2166136261U;
	faxBpl = faxImage->GetDataBpl();
	UIntOS y = 0;
	while (y < 50)
	{
		x = 0;
		while (x < 59)
			faxHash = (faxHash ^ faxImage->data[y * faxBpl + x++]) * 16777619U;
		y++;
	}
	faxImage.Delete();
	if (faxHash != 0xB05F65C2U)
	{
		printf("CCITT Group 4 image decode test failed\r\n");
		return 1;
	}
	UInt8 faxImage2Data[] =
	{
		0xF2,0x1D,0xB2,0x45,0xD8,0x78,0x30,0xF9,0x87,0x0D,0xE9,0x03,0x90,0xC7,0xA3,0x51,
		0x98,0x3A,0x9A,0x64,0x72,0xBD,0x17,0x51,0x05,0x0A,0xB0,0x73,0x90,0x0B,0x41,0xD6,
		0xA9,0x0D,0x0D,0x7D,0x1F,0x65,0x0E,0x83,0x23,0xB2,0xF1,0x8C,0x13,0xA0,0x40,0x9F,
		0x27,0x1D,0x55,0x30,0xAA,0x96,0x5E,0xFE,0x94,0x42,0x5C,0x2D,0x28,0x2A,0x88,0x2D,
		0x38,0x40,0xB9,0xC6,0x86,0x23,0xF3,0x87,0x38,0x7A,0xC1,0x3E,0x6F,0x92,0x8D,0x58,
		0x73,0xFA,0xF7,0xFC,0x23,0xBF,0x4B,0x06,0x38,0x27,0x4A,0x2A,0x92,0x2C,0x0E,0xAB,
		0x5C,0x7B,0x51,0xFF,0xE0,0x02,0x00,0x20,0x0A
	};
	if (!faxDecoder.DecodeImage(Data::ByteArrayR(faxImage2Data, sizeof(faxImage2Data)), 59, 50, -1).SetTo(faxImage))
	{
		printf("CCITT Group 4 second image decode test failed\r\n");
		return 1;
	}
	faxHash = 2166136261U;
	faxBpl = faxImage->GetDataBpl();
	y = 0;
	while (y < 50)
	{
		x = 0;
		while (x < 59)
			faxHash = (faxHash ^ faxImage->data[y * faxBpl + x++]) * 16777619U;
		y++;
	}
	faxImage.Delete();
	if (faxHash != 0x1DF657B8U)
	{
		printf("CCITT Group 4 second image decode test failed\r\n");
		return 1;
	}
	UInt8 faxLongBlackRun[] = {0x35,0x01,0x01,0x92,0x0B,0x80};
	if (!faxDecoder.DecodeImage(Data::ByteArrayR(faxLongBlackRun, sizeof(faxLongBlackRun)), 2000, 1).SetTo(faxImage))
	{
		printf("CCITT long black run decode test failed\r\n");
		return 1;
	}
	faxTestPassed = true;
	x = 0;
	while (x < 2000)
	{
		if (faxImage->data[x++] != 0)
		{
			faxTestPassed = false;
			break;
		}
	}
	faxImage.Delete();
	if (!faxTestPassed)
	{
		printf("CCITT long black run decode test failed\r\n");
		return 1;
	}
	NN<Text::String> pdfName = Text::String::New(UTF8STRC("fax-test.pdf"));
	Media::PDFDocument pdf(pdfName, CSTR("1.4"));
	pdfName->Release();
	Parser::FullParserList parsers;
	IO::StmData::MemoryDataRef faxStream(Data::ByteArrayR(faxBoundaryData, sizeof(faxBoundaryData)));
	Text::CStringNN dictionaries[] = {
		CSTR("/Subtype /Image /Filter /CCITTFaxDecode /Width 8 /Height 2 /DecodeParms << /K -1 /Columns 8 /Rows 2 >>"),
		CSTR("/Subtype /Image /Filter [ /CCITTFaxDecode ] /Width 8 /Height 2 /DecodeParms [ << /K -1 /Columns 8 /Rows 2 >> ]")
	};
	UIntOS testIndex = 0;
	while (testIndex < 2)
	{
		NN<Media::PDFObject> obj = pdf.AddObject((UInt32)testIndex + 1);
		obj->SetParameter(Media::PDFParameter::Parse(dictionaries[testIndex]));
		obj->SetStream(faxStream, 0, sizeof(faxBoundaryData));
		NN<Media::ImageList> imgList;
		NN<Media::Image> image;
		if (!Media::PDFImage::Parse(pdf, (UInt32)testIndex + 1, parsers).SetTo(imgList))
		{
			printf("PDF CCITT DecodeParms test failed\r\n");
			return 1;
		}
		if (!imgList->GetImage2(0, 0).SetTo(image) || image->GetImageType() != Media::ImageType::Raster ||
			NN<Media::RasterImage>::ConvertFrom(image)->GetImageClass() != Media::RasterImage::ImageClass::StaticImage)
		{
			imgList.Delete();
			printf("PDF CCITT DecodeParms test failed\r\n");
			return 1;
		}
		NN<Media::StaticImage> staticImage = NN<Media::StaticImage>::ConvertFrom(NN<Media::RasterImage>::ConvertFrom(image));
		faxTestPassed = staticImage->data[staticImage->GetDataBpl() + 6] == 255 &&
			staticImage->data[staticImage->GetDataBpl() + 7] == 0;
		imgList.Delete();
		if (!faxTestPassed)
		{
			printf("PDF CCITT DecodeParms test failed\r\n");
			return 1;
		}
		testIndex++;
	}
	return 0;
}

Int32 JBIG2Test()
{
	static const UInt8 jbig2Data[] =
	{
		0x97,0x4a,0x42,0x32,0x0d,0x0a,0x1a,0x0a,0x01,0x00,0x00,0x00,0x01,
		0x00,0x00,0x00,0x01,0x30,0x00,0x01,0x00,0x00,0x00,0x13,
		0x00,0x00,0x00,0x08,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x02,0x31,0x00,0x01,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x03,0x33,0x00,0x00,0x00,0x00,0x00,0x00
	};
	Media::JBIG2Decoder jbig2Decoder;
	NN<Media::StaticImage> jbig2Image;
	if (!jbig2Decoder.DecodeImage(Data::ByteArrayR(jbig2Data, sizeof(jbig2Data)), 8, 1).SetTo(jbig2Image))
	{
		printf("JBIG2 decode test failed\r\n");
		return 1;
	}
	Bool jbig2TestPassed = jbig2Image->data[0] == 255 && jbig2Image->data[7] == 255;
	jbig2Image.Delete();
	if (!jbig2TestPassed)
	{
		printf("JBIG2 decode test failed\r\n");
		return 1;
	}
	return 0;
}

void PDFTestWriteLZWCode(UnsafeArray<UInt8> data, UIntOS &bitPos, UInt32 code, UIntOS width)
{
	while (width > 0)
	{
		width--;
		if ((code >> width) & 1)
			data[bitPos >> 3] |= (UInt8)(0x80 >> (bitPos & 7));
		bitPos++;
	}
}

Int32 PDFLZWTest()
{
	static const UInt8 kwkwk[] = {0x80,0x10,0x48,0x50,0x28,0x24,0x04};
	static const UInt8 pngData[] = {0x80,0x00,0x01,0x40,0xa0,0x50,0x08,0x02,0x01,0x00,0xc0,0x40};
	static const UInt8 pngFilters[] = {0x80,0x00,0x41,0x40,0xa0,0x50,0x0c,0x1e,0x0f,0x07,0x81,0x01,0x40,0xa0,0x54,0x04};
	static const UInt8 tiffData[] = {0x80,0x04,0x84,0x50,0x10};
	static const UInt8 tiff16Data[] = {0x80,0x00,0x1f,0xe0,0x00,0x08,0x00,0x03,0x01};
	static const UInt8 tiff1Data[] = {0x80,0x20,0x20,0x20};
	static const UInt8 invalidData[] = {0x80,0x10,0x60,0x70,0x10};
	static const UInt8 pngExpected[] = {10,10,10,11,11,11};
	static const UInt8 pngFiltersExpected[] = {10,20,30,20,35,47,30,45,57};
	static const UInt8 tiffExpected[] = {0x13,0x57};
	static const UInt8 tiff16Expected[] = {0,255,1,0,1,1};
	NN<Text::String> name = Text::String::New(UTF8STRC("lzw-test.pdf"));
	Media::PDFDocument pdf(name, CSTR("1.4"));
	name->Release();
	NN<Media::PDFObject> obj = pdf.AddObject(1);
	NN<IO::StreamData> data;
	UInt8 output[7];
	IO::StmData::MemoryDataRef kwStream(Data::ByteArrayR(kwkwk, sizeof(kwkwk)));
	obj->SetParameter(Media::PDFParameter::Parse(CSTR("/Filter /LZWDecode")));
	obj->SetStream(kwStream, 0, sizeof(kwkwk));
	if (!obj->GetData().SetTo(data) || data->GetDataSize() != 7 ||
		data->GetRealData(0, 7, BYTEARR(output)) != 7 || memcmp(output, "ABABABA", 7) != 0)
	{
		printf("PDF LZW dictionary test failed\r\n");
		return 1;
	}
	obj->SetStream(kwStream, 0, sizeof(kwkwk) - 1);
	if (obj->GetData().NotNull())
	{
		printf("PDF truncated LZW test failed\r\n");
		return 1;
	}
	IO::StmData::MemoryDataRef invalidStream(Data::ByteArrayR(invalidData, sizeof(invalidData)));
	obj->SetStream(invalidStream, 0, sizeof(invalidData));
	if (obj->GetData().NotNull())
	{
		printf("PDF invalid LZW code test failed\r\n");
		return 1;
	}
	obj->SetParameter(Media::PDFParameter::Parse(CSTR("/Filter /LZWDecode /DecodeParms << /EarlyChange 2 >>")));
	obj->SetStream(kwStream, 0, sizeof(kwkwk));
	if (obj->GetData().NotNull())
	{
		printf("PDF invalid LZW parameters test failed\r\n");
		return 1;
	}
	obj->SetParameter(Media::PDFParameter::Parse(CSTR("/Filter [/ASCII85Decode /LZWDecode]")));
	obj->SetStream(kwStream, 0, sizeof(kwkwk));
	if (obj->GetData().NotNull())
	{
		printf("PDF unsupported LZW filter chain test failed\r\n");
		return 1;
	}
	IO::StmData::MemoryDataRef pngStream(Data::ByteArrayR(pngData, sizeof(pngData)));
	obj->SetParameter(Media::PDFParameter::Parse(CSTR("/Filter [/LZWDecode] /DecodeParms [<< /Predictor 15 /Columns 3 >>]")));
	obj->SetStream(pngStream, 0, sizeof(pngData));
	if (!obj->GetData().SetTo(data) || data->GetDataSize() != sizeof(pngExpected) ||
		data->GetRealData(0, sizeof(pngExpected), BYTEARR(output)) != sizeof(pngExpected) ||
		memcmp(output, pngExpected, sizeof(pngExpected)) != 0)
	{
		printf("PDF LZW PNG predictor test failed\r\n");
		return 1;
	}
	IO::StmData::MemoryDataRef filtersStream(Data::ByteArrayR(pngFilters, sizeof(pngFilters)));
	obj->SetStream(filtersStream, 0, sizeof(pngFilters));
	UInt8 filterOutput[sizeof(pngFiltersExpected)];
	if (!obj->GetData().SetTo(data) || data->GetDataSize() != sizeof(pngFiltersExpected) ||
		data->GetRealData(0, sizeof(filterOutput), BYTEARR(filterOutput)) != sizeof(filterOutput) ||
		memcmp(filterOutput, pngFiltersExpected, sizeof(filterOutput)) != 0)
	{
		printf("PDF LZW PNG filter test failed\r\n");
		return 1;
	}
	IO::StmData::MemoryDataRef tiffStream(Data::ByteArrayR(tiffData, sizeof(tiffData)));
	obj->SetParameter(Media::PDFParameter::Parse(CSTR("/Filter /LZWDecode /DecodeParms << /Predictor 2 /Columns 4 /BitsPerComponent 4 >>")));
	obj->SetStream(tiffStream, 0, sizeof(tiffData));
	if (!obj->GetData().SetTo(data) || data->GetDataSize() != sizeof(tiffExpected) ||
		data->GetRealData(0, sizeof(tiffExpected), BYTEARR(output)) != sizeof(tiffExpected) ||
		memcmp(output, tiffExpected, sizeof(tiffExpected)) != 0)
	{
		printf("PDF LZW TIFF predictor test failed\r\n");
		return 1;
	}
	IO::StmData::MemoryDataRef tiff16Stream(Data::ByteArrayR(tiff16Data, sizeof(tiff16Data)));
	obj->SetParameter(Media::PDFParameter::Parse(CSTR("/Filter /LZWDecode /DecodeParms << /Predictor 2 /Columns 3 /BitsPerComponent 16 >>")));
	obj->SetStream(tiff16Stream, 0, sizeof(tiff16Data));
	if (!obj->GetData().SetTo(data) || data->GetDataSize() != sizeof(tiff16Expected) ||
		data->GetRealData(0, sizeof(tiff16Expected), BYTEARR(output)) != sizeof(tiff16Expected) ||
		memcmp(output, tiff16Expected, sizeof(tiff16Expected)) != 0)
	{
		printf("PDF LZW 16-bit TIFF predictor test failed\r\n");
		return 1;
	}
	IO::StmData::MemoryDataRef tiff1Stream(Data::ByteArrayR(tiff1Data, sizeof(tiff1Data)));
	obj->SetParameter(Media::PDFParameter::Parse(CSTR("/Filter /LZWDecode /DecodeParms << /Predictor 2 /Columns 8 /BitsPerComponent 1 >>")));
	obj->SetStream(tiff1Stream, 0, sizeof(tiff1Data));
	if (!obj->GetData().SetTo(data) || data->GetDataSize() != 1 ||
		data->GetRealData(0, 1, BYTEARR(output)) != 1 || output[0] != 255)
	{
		printf("PDF LZW 1-bit TIFF predictor test failed\r\n");
		return 1;
	}
	UIntOS earlyChange = 0;
	while (earlyChange <= 1)
	{
		UInt8 encoded[7000];
		MemClear(encoded, sizeof(encoded));
		UIntOS bitPos = 0;
		UIntOS codeSize = 9;
		UIntOS nextCode = 258;
		Bool hasPrevious = false;
		PDFTestWriteLZWCode(encoded, bitPos, 256, codeSize);
		UIntOS i = 0;
		while (i < 4400)
		{
			if (nextCode == 4096)
			{
				PDFTestWriteLZWCode(encoded, bitPos, 256, codeSize);
				codeSize = 9;
				nextCode = 258;
				hasPrevious = false;
			}
			PDFTestWriteLZWCode(encoded, bitPos, (UInt32)(i & 255), codeSize);
			if (hasPrevious)
			{
				nextCode++;
				if (codeSize < 12 && nextCode + earlyChange == ((UIntOS)1 << codeSize))
					codeSize++;
			}
			hasPrevious = true;
			i++;
		}
		PDFTestWriteLZWCode(encoded, bitPos, 257, codeSize);
		IO::StmData::MemoryDataRef encodedStream(encoded, (bitPos + 7) / 8);
		obj->SetParameter(Media::PDFParameter::Parse(earlyChange == 0 ?
			CSTR("/Filter /LZWDecode /DecodeParms << /EarlyChange 0 >>") :
			CSTR("/Filter /LZWDecode /DecodeParms << /EarlyChange 1 >>")));
		obj->SetStream(encodedStream, 0, (bitPos + 7) / 8);
		if (!obj->GetData().SetTo(data) || data->GetDataSize() != 4400)
		{
			printf("PDF LZW code width test failed\r\n");
			return 1;
		}
		Data::ByteBuffer actual(4400);
		if (data->GetRealData(0, 4400, actual) != 4400)
			return 1;
		i = 0;
		while (i < 4400)
		{
			if (actual[i] != (UInt8)(i & 255))
			{
				printf("PDF LZW code width data test failed\r\n");
				return 1;
			}
			i++;
		}
		earlyChange++;
	}
	UInt8 encrypted[sizeof(kwkwk)];
	MemCopyNO(encrypted, kwkwk, sizeof(kwkwk));
	UInt8 key[] = {1,2,3,4,5};
	pdf.SetEncryptionRequired();
	if (!pdf.SetEncryptionKey(key, sizeof(key)) || !pdf.DecryptStreamData(1, encrypted, sizeof(encrypted)))
		return 1;
	IO::StmData::MemoryDataRef encryptedStream(Data::ByteArrayR(encrypted, sizeof(encrypted)));
	obj->SetParameter(Media::PDFParameter::Parse(CSTR("/Subtype /Image /Filter [ /LZWDecode ]")));
	obj->SetStream(encryptedStream, 0, sizeof(encrypted));
	if (obj->GetImageType(pdf) != Media::PDFObject::ImageType::PlainImage ||
		!obj->GetData().SetTo(data) || data->GetDataSize() != 7 ||
		data->GetRealData(0, 7, BYTEARR(output)) != 7 || memcmp(output, "ABABABA", 7) != 0)
	{
		printf("PDF encrypted LZW test failed\r\n");
		return 1;
	}
	return 0;
}

Int32 PDFFlatePredictorTest()
{
	static const UInt8 compressed[] = {0x78,0x9c,0x63,0xe0,0x12,0x91,0x63,0x62,0x64,0x62,0x06,0x00,0x01,0x6e,0x00,0x45};
	static const UInt8 expected[] = {10,20,30,11,22,33};
	static const UInt8 tiffCompressed[] = {0x78,0x9c,0x13,0x52,0x02,0x00,0x00,0x48,0x00,0x35};
	static const UInt8 invalidCompressed[] = {0x78,0x9c,0x63,0x65,0x64,0x62,0x06,0x00,0x00,0x22,0x00,0x0c};
	static const UInt8 lzwData[] = {0x80,0x00,0x01,0x40,0xa0,0x50,0x08,0x02,0x01,0x00,0xc0,0x40};
	static const UInt8 lzwExpected[] = {10,10,10,11,11,11};
	NN<Text::String> name = Text::String::New(UTF8STRC("predictor-test.pdf"));
	Media::PDFDocument pdf(name, CSTR("1.4"));
	name->Release();
	NN<Media::PDFObject> imageObj = pdf.AddObject(1);
	NN<Media::PDFObject> parmsObj = pdf.AddObject(309);
	parmsObj->SetParameter(Media::PDFParameter::Parse(CSTR("/Predictor 15 /Colors 3 /Columns 1 /BitsPerComponent 8")));
	IO::StmData::MemoryDataRef source(Data::ByteArrayR(compressed, sizeof(compressed)));
	imageObj->SetStream(source, 0, sizeof(compressed));
	Text::CStringNN dictionaries[] = {
		CSTR("/Subtype /Image /Width 1 /Height 2 /BitsPerComponent 8 /ColorSpace /DeviceRGB /Filter /FlateDecode /DecodeParms 309 0 R"),
		CSTR("/Subtype /Image /Width 1 /Height 2 /BitsPerComponent 8 /ColorSpace /DeviceRGB /Filter [/FlateDecode] /DecodeParms [<< /Predictor 15 /Colors 3 /Columns 1 >>]"),
		CSTR("/Subtype /Image /Width 1 /Height 2 /BitsPerComponent 8 /ColorSpace /DeviceRGB /Filter /FlateDecode /DecodeParms [309 0 R]")
	};
	Parser::FullParserList parsers;
	UIntOS i = 0;
	while (i < 3)
	{
		imageObj->SetParameter(Media::PDFParameter::Parse(dictionaries[i]));
		NN<IO::StreamData> data;
		UInt8 output[sizeof(expected)];
		if (!imageObj->GetData().SetTo(data) || data->GetDataSize() != sizeof(expected) ||
			data->GetRealData(0, sizeof(output), BYTEARR(output)) != sizeof(output) ||
			memcmp(output, expected, sizeof(expected)) != 0)
		{
			printf("PDF Flate predictor stream test failed\r\n");
			return 1;
		}
		NN<Media::ImageList> images;
		NN<Media::Image> image;
		if (!Media::PDFImage::Parse(pdf, 1, parsers).SetTo(images))
		{
			printf("PDF Flate predictor image test failed\r\n");
			return 1;
		}
		if (!images->GetImage2(0, 0).SetTo(image) || image->GetImageType() != Media::ImageType::Raster ||
			NN<Media::RasterImage>::ConvertFrom(image)->GetImageClass() != Media::RasterImage::ImageClass::StaticImage)
		{
			images.Delete();
			printf("PDF Flate predictor image test failed\r\n");
			return 1;
		}
		NN<Media::StaticImage> img = NN<Media::StaticImage>::ConvertFrom(NN<Media::RasterImage>::ConvertFrom(image));
		UIntOS bpl = img->GetDataBpl();
		Bool match = img->data[0] == 30 && img->data[1] == 20 && img->data[2] == 10 &&
			img->data[bpl] == 33 && img->data[bpl + 1] == 22 && img->data[bpl + 2] == 11;
		images.Delete();
		if (!match)
		{
			printf("PDF Flate predictor RGB image test failed\r\n");
			return 1;
		}
		i++;
	}
	IO::StmData::MemoryDataRef tiffSource(Data::ByteArrayR(tiffCompressed, sizeof(tiffCompressed)));
	imageObj->SetStream(tiffSource, 0, sizeof(tiffCompressed));
	imageObj->SetParameter(Media::PDFParameter::Parse(CSTR("/Filter /FlateDecode /DecodeParms << /Predictor 2 /Columns 4 /BitsPerComponent 4 >>")));
	NN<IO::StreamData> data;
	UInt8 tiffOutput[2];
	if (!imageObj->GetData().SetTo(data) || data->GetDataSize() != 2 ||
		data->GetRealData(0, 2, BYTEARR(tiffOutput)) != 2 || tiffOutput[0] != 0x13 || tiffOutput[1] != 0x57)
	{
		printf("PDF Flate TIFF predictor test failed\r\n");
		return 1;
	}
	imageObj->SetParameter(Media::PDFParameter::Parse(CSTR("/Filter /FlateDecode /DecodeParms 999 0 R")));
	if (imageObj->GetData().NotNull())
	{
		printf("PDF missing DecodeParms test failed\r\n");
		return 1;
	}
	IO::StmData::MemoryDataRef invalidSource(Data::ByteArrayR(invalidCompressed, sizeof(invalidCompressed)));
	imageObj->SetStream(invalidSource, 0, sizeof(invalidCompressed));
	imageObj->SetParameter(Media::PDFParameter::Parse(CSTR("/Filter /FlateDecode /DecodeParms << /Predictor 15 /Columns 3 >>")));
	if (imageObj->GetData().NotNull())
	{
		printf("PDF invalid Flate predictor test failed\r\n");
		return 1;
	}
	parmsObj->SetParameter(Media::PDFParameter::Parse(CSTR("/Predictor 15 /Columns 3")));
	IO::StmData::MemoryDataRef lzwSource(Data::ByteArrayR(lzwData, sizeof(lzwData)));
	imageObj->SetStream(lzwSource, 0, sizeof(lzwData));
	imageObj->SetParameter(Media::PDFParameter::Parse(CSTR("/Filter /LZWDecode /DecodeParms 309 0 R")));
	UInt8 lzwOutput[sizeof(lzwExpected)];
	if (!imageObj->GetData().SetTo(data) || data->GetDataSize() != sizeof(lzwExpected) ||
		data->GetRealData(0, sizeof(lzwOutput), BYTEARR(lzwOutput)) != sizeof(lzwOutput) ||
		memcmp(lzwOutput, lzwExpected, sizeof(lzwExpected)) != 0)
	{
		printf("PDF indirect LZW DecodeParms test failed\r\n");
		return 1;
	}
	return 0;
}

Int32 PDFSeparationImageTest()
{
	NN<Text::String> name = Text::String::New(UTF8STRC("separation-test.pdf"));
	Media::PDFDocument pdf(name, CSTR("1.4"));
	name->Release();
	NN<Media::PDFObject> imageObj = pdf.AddObject(1);
	const UInt8 sampleData[] = {0, 128, 255};
	IO::StmData::MemoryDataRef imageStream(Data::ByteArrayR(sampleData, sizeof(sampleData)));
	imageObj->SetStream(imageStream, 0, imageStream.GetDataSize());
	IO::StmData::MemoryDataRef colorSpaceData(Data::ByteArrayR(UTF8STRC("[ /Separation /Spot /DeviceRGB 3 0 R ]")));
	pdf.AddObject(2)->SetData(colorSpaceData, 0, colorSpaceData.GetDataSize());
	NN<Media::PDFObject> functionObj = pdf.AddObject(3);
	functionObj->SetParameter(Media::PDFParameter::Parse(CSTR("/FunctionType 2 /Domain [0 1] /C0 [1 0 0] /C1 [0 0 1] /N 1")));
	Parser::FullParserList parsers;
	NN<Media::ImageList> images;
	NN<Media::Image> image;
	imageObj->SetParameter(Media::PDFParameter::Parse(CSTR("/Subtype /Image /Width 3 /Height 1 /BitsPerComponent 8 /ColorSpace [/Separation /Spot /DeviceRGB << /FunctionType 2 /Domain [0 1] /C0 [1 0 0] /C1 [0 0 1] /N 1 >>]")));
	if (!Media::PDFImage::Parse(pdf, 1, parsers).SetTo(images) ||
		!images->GetImage2(0, 0).SetTo(image) ||
		image->GetImageType() != Media::ImageType::Raster ||
		NN<Media::RasterImage>::ConvertFrom(image)->GetImageClass() != Media::RasterImage::ImageClass::StaticImage)
	{
		printf("PDF Separation inline image creation test failed\r\n");
		return 1;
	}
	NN<Media::StaticImage> staticImage = NN<Media::StaticImage>::ConvertFrom(NN<Media::RasterImage>::ConvertFrom(image));
	Bool ok = staticImage->info.dispSize.x == 3 && staticImage->info.dispSize.y == 1 &&
		staticImage->info.pf == Media::PixelFormat::PF_B8G8R8 &&
		staticImage->data[0] == 0 && staticImage->data[1] == 0 && staticImage->data[2] == 255 &&
		staticImage->data[3] == 128 && staticImage->data[4] == 0 && staticImage->data[5] == 127 &&
		staticImage->data[6] == 255 && staticImage->data[7] == 0 && staticImage->data[8] == 0;
	images.Delete();
	if (!ok)
	{
		printf("PDF Separation inline tint transform test failed\r\n");
		return 1;
	}

	imageObj->SetParameter(Media::PDFParameter::Parse(CSTR("/Subtype /Image /Width 3 /Height 1 /BitsPerComponent 8 /ColorSpace 2 0 R")));
	if (!Media::PDFImage::Parse(pdf, 1, parsers).SetTo(images) ||
		!images->GetImage2(0, 0).SetTo(image) ||
		image->GetImageType() != Media::ImageType::Raster ||
		NN<Media::RasterImage>::ConvertFrom(image)->GetImageClass() != Media::RasterImage::ImageClass::StaticImage)
	{
		printf("PDF Separation indirect image creation test failed\r\n");
		return 1;
	}
	staticImage = NN<Media::StaticImage>::ConvertFrom(NN<Media::RasterImage>::ConvertFrom(image));
	ok = staticImage->data[0] == 0 && staticImage->data[2] == 255 &&
		staticImage->data[3] == 128 && staticImage->data[5] == 127 &&
		staticImage->data[6] == 255 && staticImage->data[8] == 0;
	images.Delete();
	if (!ok)
	{
		printf("PDF Separation indirect tint transform test failed\r\n");
		return 1;
	}

	const UInt8 sampledData[] = {255, 0, 0, 0, 0, 255};
	IO::StmData::MemoryDataRef sampledStream(Data::ByteArrayR(sampledData, sizeof(sampledData)));
	NN<Media::PDFObject> sampledFunction = pdf.AddObject(4);
	sampledFunction->SetParameter(Media::PDFParameter::Parse(CSTR("/FunctionType 0 /Domain [0 1] /Range [0 1 0 1 0 1] /Size [2] /BitsPerSample 8 /Decode [0 1 0 1 0 1]")));
	sampledFunction->SetStream(sampledStream, 0, sizeof(sampledData));
	IO::StmData::MemoryDataRef sampledColorSpace(Data::ByteArrayR(UTF8STRC("[ /Separation /Spot /DeviceRGB 4 0 R ]")));
	pdf.AddObject(5)->SetData(sampledColorSpace, 0, sampledColorSpace.GetDataSize());
	imageObj->SetParameter(Media::PDFParameter::Parse(CSTR("/Subtype /Image /Width 3 /Height 1 /BitsPerComponent 8 /ColorSpace 5 0 R")));
	if (!Media::PDFImage::Parse(pdf, 1, parsers).SetTo(images) ||
		!images->GetImage2(0, 0).SetTo(image) ||
		image->GetImageType() != Media::ImageType::Raster ||
		NN<Media::RasterImage>::ConvertFrom(image)->GetImageClass() != Media::RasterImage::ImageClass::StaticImage)
	{
		printf("PDF Separation sampled image creation test failed\r\n");
		return 1;
	}
	staticImage = NN<Media::StaticImage>::ConvertFrom(NN<Media::RasterImage>::ConvertFrom(image));
	ok = staticImage->data[0] == 0 && staticImage->data[1] == 0 && staticImage->data[2] == 255 &&
		staticImage->data[3] == 128 && staticImage->data[4] == 0 && staticImage->data[5] == 127 &&
		staticImage->data[6] == 255 && staticImage->data[7] == 0 && staticImage->data[8] == 0;
	images.Delete();
	if (!ok)
	{
		printf("PDF Separation sampled tint transform test failed\r\n");
		return 1;
	}

	UInt8 cmykICC[132];
	MemClear(cmykICC, sizeof(cmykICC));
	cmykICC[3] = 132;
	cmykICC[16] = 'C';
	cmykICC[17] = 'M';
	cmykICC[18] = 'Y';
	cmykICC[19] = 'K';
	cmykICC[36] = 'a';
	cmykICC[37] = 'c';
	cmykICC[38] = 's';
	cmykICC[39] = 'p';
	IO::StmData::MemoryDataRef cmykICCStream(Data::ByteArrayR(cmykICC, sizeof(cmykICC)));
	NN<Media::PDFObject> cmykICCObj = pdf.AddObject(8);
	cmykICCObj->SetParameter(Media::PDFParameter::Parse(CSTR("/N 4 /Alternate /DeviceCMYK")));
	cmykICCObj->SetStream(cmykICCStream, 0, sizeof(cmykICC));
	IO::StmData::MemoryDataRef cmykBase(Data::ByteArrayR(UTF8STRC("[/ICCBased 8 0 R]")));
	pdf.AddObject(9)->SetData(cmykBase, 0, cmykBase.GetDataSize());
	const UInt8 cmykSamples[] = {0, 0, 0, 0, 0, 0, 0, 255};
	IO::StmData::MemoryDataRef cmykSamplesStream(Data::ByteArrayR(cmykSamples, sizeof(cmykSamples)));
	NN<Media::PDFObject> cmykFunction = pdf.AddObject(10);
	cmykFunction->SetParameter(Media::PDFParameter::Parse(CSTR("/FunctionType 0 /Domain [0 1] /Range [0 1 0 1 0 1 0 .5] /Size [2] /BitsPerSample 8")));
	cmykFunction->SetStream(cmykSamplesStream, 0, sizeof(cmykSamples));
	IO::StmData::MemoryDataRef cmykColorSpace(Data::ByteArrayR(UTF8STRC("[/Separation /Black 9 0 R 10 0 R]")));
	pdf.AddObject(11)->SetData(cmykColorSpace, 0, cmykColorSpace.GetDataSize());
	imageObj->SetParameter(Media::PDFParameter::Parse(CSTR("/Subtype /Image /Width 3 /Height 1 /BitsPerComponent 8 /ColorSpace 11 0 R")));
	if (!Media::PDFImage::Parse(pdf, 1, parsers).SetTo(images))
	{
		printf("PDF Separation ICC CMYK sampled image creation test failed\r\n");
		return 1;
	}
	ok = images->GetImage2(0, 0).SetTo(image) && image->GetImageType() == Media::ImageType::Raster &&
		NN<Media::RasterImage>::ConvertFrom(image)->GetImageClass() == Media::RasterImage::ImageClass::StaticImage;
	if (ok)
	{
		staticImage = NN<Media::StaticImage>::ConvertFrom(NN<Media::RasterImage>::ConvertFrom(image));
		ok = staticImage->data[0] == 255 && staticImage->data[1] == 255 && staticImage->data[2] == 255 &&
			staticImage->data[3] == 191 && staticImage->data[4] == 191 && staticImage->data[5] == 191 &&
			staticImage->data[6] == 128 && staticImage->data[7] == 128 && staticImage->data[8] == 128 &&
			staticImage->info.color.GetRAWICC().IsNull();
	}
	images.Delete();
	if (!ok)
	{
		printf("PDF Separation sampled default Decode test failed\r\n");
		return 1;
	}
	cmykICCObj->SetParameter(Media::PDFParameter::Parse(CSTR("/N 4 /Alternate /DeviceRGB")));
	if (Media::PDFImage::Parse(pdf, 1, parsers).SetTo(images))
	{
		images.Delete();
		printf("PDF Separation invalid ICC alternate test failed\r\n");
		return 1;
	}
	cmykICCObj->SetParameter(Media::PDFParameter::Parse(CSTR("/N 4 /Alternate /DeviceCMYK")));
	cmykICCObj->SetStream(cmykICCStream, 0, sizeof(cmykICC) - 1);
	if (Media::PDFImage::Parse(pdf, 1, parsers).SetTo(images))
	{
		images.Delete();
		printf("PDF Separation truncated CMYK ICC test failed\r\n");
		return 1;
	}
	cmykICCObj->SetStream(cmykICCStream, 0, sizeof(cmykICC));
	cmykFunction->SetParameter(Media::PDFParameter::Parse(CSTR("/FunctionType 0 /Domain [0 1] /Range [0 1 0 1 0 1 0 1] /Size [2] /BitsPerSample 8 /Order 3")));
	if (Media::PDFImage::Parse(pdf, 1, parsers).SetTo(images))
	{
		images.Delete();
		printf("PDF Separation unsupported sampled interpolation test failed\r\n");
		return 1;
	}

	NN<Media::PDFObject> calculatorFunction = pdf.AddObject(6);
	calculatorFunction->SetParameter(Media::PDFParameter::Parse(CSTR("/FunctionType 4 /Domain [0 1] /Range [0 1 0 1 0 1 0 1]")));
	IO::StmData::MemoryDataRef calculatorStream(Data::ByteArrayR(UTF8STRC("{dup 0 mul exch dup 0 mul exch dup 0 mul exch 1 mul}")));
	calculatorFunction->SetStream(calculatorStream, 0, calculatorStream.GetDataSize());
	IO::StmData::MemoryDataRef calculatorColorSpace(Data::ByteArrayR(UTF8STRC("[ /Separation /Spot /DeviceCMYK 6 0 R ]")));
	pdf.AddObject(7)->SetData(calculatorColorSpace, 0, calculatorColorSpace.GetDataSize());
	imageObj->SetParameter(Media::PDFParameter::Parse(CSTR("/Subtype /Image /Width 3 /Height 1 /BitsPerComponent 8 /ColorSpace 7 0 R")));
	if (!Media::PDFImage::Parse(pdf, 1, parsers).SetTo(images) ||
		!images->GetImage2(0, 0).SetTo(image) ||
		image->GetImageType() != Media::ImageType::Raster ||
		NN<Media::RasterImage>::ConvertFrom(image)->GetImageClass() != Media::RasterImage::ImageClass::StaticImage)
	{
		printf("PDF Separation calculator image creation test failed\r\n");
		return 1;
	}
	staticImage = NN<Media::StaticImage>::ConvertFrom(NN<Media::RasterImage>::ConvertFrom(image));
	ok = staticImage->data[0] == 255 && staticImage->data[1] == 255 && staticImage->data[2] == 255 &&
		staticImage->data[3] == 127 && staticImage->data[4] == 127 && staticImage->data[5] == 127 &&
		staticImage->data[6] == 0 && staticImage->data[7] == 0 && staticImage->data[8] == 0;
	images.Delete();
	if (!ok)
	{
		printf("PDF Separation calculator tint transform test failed\r\n");
		return 1;
	}

	imageObj->SetParameter(Media::PDFParameter::Parse(CSTR("/Subtype /Image /Width 3 /Height 1 /BitsPerComponent 8 /ColorSpace [/Separation /Spot /DeviceRGB << /FunctionType 0 /N 1 >>]")));
	if (Media::PDFImage::Parse(pdf, 1, parsers).SetTo(images))
	{
		images.Delete();
		printf("PDF Separation unsupported tint function test failed\r\n");
		return 1;
	}
	return 0;
}

Int32 PDFICCImageTest()
{
	NN<Text::String> name = Text::String::New(UTF8STRC("icc-test.pdf"));
	Media::PDFDocument pdf(name, CSTR("1.4"));
	name->Release();
	NN<Media::PDFObject> imageObj = pdf.AddObject(1);
	NN<Media::PDFObject> iccObj = pdf.AddObject(2);
	UnsafeArray<const UInt8> icc = Media::ICCProfile::GetSRGBICCData();
	IO::StmData::MemoryDataRef iccStream(Data::ByteArrayR(icc, ReadMUInt32(icc.Ptr())));
	iccObj->SetParameter(Media::PDFParameter::Parse(CSTR("/N 3")));
	iccObj->SetStream(iccStream, 0, iccStream.GetDataSize());
	IO::StmData::MemoryDataRef colorSpace(Data::ByteArrayR(UTF8STRC("[ /ICCBased 2 0 R ]")));
	NN<Media::PDFObject> colorObj = pdf.AddObject(3);
	colorObj->SetData(colorSpace, 0, colorSpace.GetDataSize());
	Parser::FullParserList parsers;
	const UInt8 packed1[] = {0x90, 0x24};
	const UInt8 packed2[] = {0xd3, 0x40, 0x1c, 0x70};
	const UInt8 packed4[] = {0xf8, 0x0f, 0x80, 0x08, 0xf0, 0x8f};
	const UInt8 packed8[] = {255,128,0,255,128,0, 0,128,255,0,128,255, 10};
	const UInt8 packed16[] = {255,255,128,128,0,0,255,255,128,128,0,0, 0,0,128,128,255,255,0,0,128,128,255,255};
	const UInt8 *samples[] = {packed1, packed2, packed4, packed8, packed16};
	UIntOS sizes[] = {sizeof(packed1), sizeof(packed2), sizeof(packed4), sizeof(packed8), sizeof(packed16)};
	Text::CStringNN dictionaries[] = {
		CSTR("/Subtype /Image /Width 2 /Height 2 /BitsPerComponent 1 /ColorSpace [/ICCBased 2 0 R]"),
		CSTR("/Subtype /Image /Width 2 /Height 2 /BitsPerComponent 2 /ColorSpace 3 0 R"),
		CSTR("/Subtype /Image /Width 2 /Height 2 /BitsPerComponent 4 /ColorSpace [/ICCBased 2 0 R]"),
		CSTR("/Subtype /Image /Width 2 /Height 2 /BitsPerComponent 8 /ColorSpace 3 0 R /Decode [0 1 0 1 0 1]"),
		CSTR("/Subtype /Image /Width 2 /Height 2 /BitsPerComponent 16 /ColorSpace [/ICCBased 2 0 R]")
	};
	UInt8 middle[] = {0, 85, 136, 128, 128};
	for (UIntOS i = 0; i < 5; i++)
	{
		IO::StmData::MemoryDataRef stream(Data::ByteArrayR(samples[i], sizes[i]));
		imageObj->SetStream(stream, 0, sizes[i]);
		imageObj->SetParameter(Media::PDFParameter::Parse(dictionaries[i]));
		NN<Media::ImageList> images;
		NN<Media::Image> image;
		if (!Media::PDFImage::Parse(pdf, 1, parsers).SetTo(images))
		{
			printf("PDF ICCBased image creation test failed for case %d\r\n", (Int32)i);
			return 1;
		}
		Bool ok = images->GetImage2(0, 0).SetTo(image) && image->GetImageType() == Media::ImageType::Raster &&
			NN<Media::RasterImage>::ConvertFrom(image)->GetImageClass() == Media::RasterImage::ImageClass::StaticImage;
		if (ok)
		{
			NN<Media::StaticImage> img = NN<Media::StaticImage>::ConvertFrom(NN<Media::RasterImage>::ConvertFrom(image));
			UIntOS bpl = img->GetDataBpl();
			ok = img->info.color.GetRAWICC().NotNull() && img->info.pf == Media::PixelFormat::PF_B8G8R8 &&
				img->data[0] == 0 && img->data[1] == middle[i] && img->data[2] == 255 &&
				img->data[3] == 0 && img->data[4] == middle[i] && img->data[5] == 255 &&
				img->data[bpl] == 255 && img->data[bpl + 1] == middle[i] && img->data[bpl + 2] == 0 &&
				img->data[bpl + 3] == 255 && img->data[bpl + 4] == middle[i] && img->data[bpl + 5] == 0;
		}
		images.Delete();
		if (!ok)
		{
			printf("PDF ICCBased image sample test failed for case %d\r\n", (Int32)i);
			return 1;
		}
	}
	static const UInt8 compressed[] = {0x78,0x9c,0x63,0xe0,0x12,0x91,0x63,0x62,0x64,0x62,0x06,0x00,0x01,0x6e,0x00,0x45};
	IO::StmData::MemoryDataRef compressedStream(Data::ByteArrayR(compressed, sizeof(compressed)));
	imageObj->SetStream(compressedStream, 0, sizeof(compressed));
	imageObj->SetParameter(Media::PDFParameter::Parse(CSTR("/Subtype /Image /Width 1 /Height 2 /BitsPerComponent 8 /ColorSpace 3 0 R /Filter /FlateDecode /DecodeParms << /Predictor 15 /Colors 3 /Columns 1 >>")));
	NN<Media::ImageList> images;
	NN<Media::Image> image;
	if (!Media::PDFImage::Parse(pdf, 1, parsers).SetTo(images))
	{
		printf("PDF ICCBased Flate predictor image test failed\r\n");
		return 1;
	}
	Bool ok = images->GetImage2(0, 0).SetTo(image) && image->GetImageType() == Media::ImageType::Raster &&
		NN<Media::RasterImage>::ConvertFrom(image)->GetImageClass() == Media::RasterImage::ImageClass::StaticImage;
	if (ok)
	{
		NN<Media::StaticImage> img = NN<Media::StaticImage>::ConvertFrom(NN<Media::RasterImage>::ConvertFrom(image));
		UIntOS bpl = img->GetDataBpl();
		ok = img->info.color.GetRAWICC().NotNull() && img->data[0] == 30 && img->data[1] == 20 && img->data[2] == 10 &&
			img->data[bpl] == 33 && img->data[bpl + 1] == 22 && img->data[bpl + 2] == 11;
	}
	images.Delete();
	if (!ok)
	{
		printf("PDF ICCBased Flate predictor pixel test failed\r\n");
		return 1;
	}
	iccObj->SetParameter(Media::PDFParameter::Parse(CSTR("/N 3 /Range [0 1 0 1 0 1]")));
	if (!Media::PDFImage::Parse(pdf, 1, parsers).SetTo(images))
	{
		printf("PDF ICCBased default Range test failed\r\n");
		return 1;
	}
	images.Delete();
	IO::StmData::MemoryDataRef shortStream(Data::ByteArrayR(packed8, 2));
	imageObj->SetStream(shortStream, 0, 2);
	imageObj->SetParameter(Media::PDFParameter::Parse(dictionaries[3]));
	if (Media::PDFImage::Parse(pdf, 1, parsers).SetTo(images))
	{
		images.Delete();
		printf("PDF ICCBased truncated image test failed\r\n");
		return 1;
	}
	IO::StmData::MemoryDataRef stream(Data::ByteArrayR(packed8, sizeof(packed8)));
	imageObj->SetStream(stream, 0, sizeof(packed8));
	imageObj->SetParameter(Media::PDFParameter::Parse(CSTR("/Subtype /Image /Width 2 /Height 2 /BitsPerComponent 8 /ColorSpace [/ICCBased 2 0 R] /Decode [1 0 0 1 0 1]")));
	if (Media::PDFImage::Parse(pdf, 1, parsers).SetTo(images))
	{
		images.Delete();
		printf("PDF ICCBased unsupported Decode test failed\r\n");
		return 1;
	}
	iccObj->SetParameter(Media::PDFParameter::Parse(CSTR("/N 4")));
	imageObj->SetParameter(Media::PDFParameter::Parse(dictionaries[3]));
	if (Media::PDFImage::Parse(pdf, 1, parsers).SetTo(images))
	{
		images.Delete();
		printf("PDF ICCBased component mismatch test failed\r\n");
		return 1;
	}
	iccObj->SetParameter(Media::PDFParameter::Parse(CSTR("/N 3 /Range [0 1 0 1 0 2]")));
	if (Media::PDFImage::Parse(pdf, 1, parsers).SetTo(images))
	{
		images.Delete();
		printf("PDF ICCBased unsupported Range test failed\r\n");
		return 1;
	}
	iccObj->SetParameter(Media::PDFParameter::Parse(CSTR("/N 3")));
	IO::StmData::MemoryDataRef badICC(Data::ByteArrayR(icc, 100));
	iccObj->SetStream(badICC, 0, 100);
	if (Media::PDFImage::Parse(pdf, 1, parsers).SetTo(images))
	{
		images.Delete();
		printf("PDF ICCBased truncated profile test failed\r\n");
		return 1;
	}
	IO::StmData::MemoryDataRef cycle(Data::ByteArrayR(UTF8STRC("3 0 R")));
	colorObj->SetData(cycle, 0, cycle.GetDataSize());
	imageObj = pdf.AddObject(4);
	imageObj->SetStream(stream, 0, sizeof(packed8));
	imageObj->SetParameter(Media::PDFParameter::Parse(dictionaries[3]));
	if (Media::PDFImage::Parse(pdf, 4, parsers).SetTo(images))
	{
		images.Delete();
		printf("PDF ICCBased cyclic reference test failed\r\n");
		return 1;
	}
	return 0;
}

Int32 PDFIndexedImageTest()
{
	static const UInt8 imageData[] = {0x10, 0xB0};
	static const UInt8 paletteBytes[] = {255,0,0, 0,255,0, 0,0,255, 255,255,255};
	NN<Text::String> name = Text::String::New(UTF8STRC("indexed-test.pdf"));
	Media::PDFDocument pdf(name, CSTR("1.4"));
	name->Release();
	NN<Media::PDFObject> imageObj = pdf.AddObject(1);
	IO::StmData::MemoryDataRef imageStream(Data::ByteArrayR(imageData, sizeof(imageData)));
	imageObj->SetStream(imageStream, 0, sizeof(imageData));
	NN<Media::PDFObject> paletteObj = pdf.AddObject(2);
	IO::StmData::MemoryDataRef paletteStream(Data::ByteArrayR(paletteBytes, sizeof(paletteBytes)));
	paletteObj->SetStream(paletteStream, 0, sizeof(paletteBytes));
	UnsafeArray<const UInt8> iccData = Media::ICCProfile::GetSRGBICCData();
	IO::StmData::MemoryDataRef iccStream(Data::ByteArrayR(iccData, ReadMUInt32(iccData.Ptr())));
	NN<Media::PDFObject> iccObj = pdf.AddObject(2473);
	iccObj->SetParameter(Media::PDFParameter::Parse(CSTR("/N 3")));
	iccObj->SetStream(iccStream, 0, iccStream.GetDataSize());
	IO::StmData::MemoryDataRef baseStream(Data::ByteArrayR(UTF8STRC("[/ICCBased 2473 0 R]")));
	NN<Media::PDFObject> baseObj = pdf.AddObject(2474);
	baseObj->SetData(baseStream, 0, baseStream.GetDataSize());
	IO::StmData::MemoryDataRef deviceBase(Data::ByteArrayR(UTF8STRC("/DeviceRGB")));
	pdf.AddObject(2475)->SetData(deviceBase, 0, deviceBase.GetDataSize());
	IO::StmData::MemoryDataRef chainedBase(Data::ByteArrayR(UTF8STRC("2475 0 R")));
	pdf.AddObject(2476)->SetData(chainedBase, 0, chainedBase.GetDataSize());
	Parser::FullParserList parsers;
	Text::CStringNN dictionaries[] = {
		CSTR("/Subtype /Image /Width 2 /Height 2 /BitsPerComponent 2 /ColorSpace [/Indexed/DeviceRGB 3<FF0000 00FF00 0000FF FFFFFF>]"),
		CSTR("/Subtype /Image /Width 2 /Height 2 /BitsPerComponent 2 /ColorSpace [/I /RGB 3 (\\377\\000\\000\\000\\377\\000\\000\\000\\377\\377\\377\\377)]"),
		CSTR("/Subtype /Image /Width 2 /Height 2 /BitsPerComponent 2 /ColorSpace [/I /RGB 3 2 0 R]"),
		CSTR("/Subtype /Image /Width 2 /Height 2 /BitsPerComponent 2 /ColorSpace [/Indexed 2474 0 R 3 2 0 R]"),
		CSTR("/Subtype /Image /Width 2 /Height 2 /BitsPerComponent 2 /ColorSpace [/Indexed [/ICCBased 2473 0 R] 3 2 0 R]"),
		CSTR("/Subtype /Image /Width 2 /Height 2 /BitsPerComponent 2 /ColorSpace [/Indexed 2476 0 R 3 2 0 R]")
	};
	UIntOS i = 0;
	while (i < 6)
	{
		imageObj->SetParameter(Media::PDFParameter::Parse(dictionaries[i]));
		NN<Media::ImageList> images;
		NN<Media::Image> image;
		if (!Media::PDFImage::Parse(pdf, 1, parsers).SetTo(images) ||
			!images->GetImage2(0, 0).SetTo(image) ||
			image->GetImageType() != Media::ImageType::Raster ||
			NN<Media::RasterImage>::ConvertFrom(image)->GetImageClass() != Media::RasterImage::ImageClass::StaticImage)
		{
			printf("PDF Indexed image creation test failed\r\n");
			return 1;
		}
		NN<Media::StaticImage> staticImage = NN<Media::StaticImage>::ConvertFrom(NN<Media::RasterImage>::ConvertFrom(image));
		UnsafeArray<UInt8> palette;
		Bool ok = staticImage->info.dispSize.x == 2 && staticImage->info.dispSize.y == 2 &&
			staticImage->info.pf == Media::PixelFormat::PF_PAL_W8 &&
			((i != 3 && i != 4) || staticImage->info.color.GetRAWICC().NotNull()) &&
			staticImage->data[0] == 0 && staticImage->data[1] == 1 &&
			staticImage->data[2] == 2 && staticImage->data[3] == 3 &&
			staticImage->pal.SetTo(palette) &&
			palette[0] == 0 && palette[1] == 0 && palette[2] == 255 && palette[3] == 255 &&
			palette[4] == 0 && palette[5] == 255 && palette[6] == 0 && palette[7] == 255 &&
			palette[8] == 255 && palette[9] == 0 && palette[10] == 0 && palette[11] == 255 &&
			palette[12] == 255 && palette[13] == 255 && palette[14] == 255 && palette[15] == 255;
		images.Delete();
		if (!ok)
		{
			printf("PDF Indexed image palette test failed\r\n");
			return 1;
		}
		i++;
	}
	imageObj->SetParameter(Media::PDFParameter::Parse(CSTR("/Subtype /Image /Width 2 /Height 2 /BitsPerComponent 2 /ColorSpace [/Indexed/DeviceCMYK 3<00000000 000000FF 0000FF00 0000FFFF>]")));
	NN<Media::ImageList> images;
	NN<Media::Image> image;
	if (!Media::PDFImage::Parse(pdf, 1, parsers).SetTo(images) ||
		!images->GetImage2(0, 0).SetTo(image) ||
		image->GetImageType() != Media::ImageType::Raster ||
		NN<Media::RasterImage>::ConvertFrom(image)->GetImageClass() != Media::RasterImage::ImageClass::StaticImage)
	{
		printf("PDF Indexed CMYK image creation test failed\r\n");
		return 1;
	}
	NN<Media::StaticImage> staticImage = NN<Media::StaticImage>::ConvertFrom(NN<Media::RasterImage>::ConvertFrom(image));
	UnsafeArray<UInt8> palette;
	Bool ok = staticImage->pal.SetTo(palette) &&
		palette[0] == 255 && palette[1] == 255 && palette[2] == 255 &&
		palette[4] == 0 && palette[5] == 0 && palette[6] == 0;
	images.Delete();
	if (!ok)
	{
		printf("PDF Indexed CMYK palette test failed\r\n");
		return 1;
	}
	UInt8 fullPalette[768];
	for (UIntOS p = 0; p < 256; p++)
	{
		fullPalette[p * 3] = (UInt8)p;
		fullPalette[p * 3 + 1] = (UInt8)(255 - p);
		fullPalette[p * 3 + 2] = 17;
	}
	const UInt8 fullImage[] = {0, 127, 255};
	IO::StmData::MemoryDataRef fullPaletteStream(Data::ByteArrayR(fullPalette, sizeof(fullPalette)));
	pdf.AddObject(4)->SetStream(fullPaletteStream, 0, sizeof(fullPalette));
	IO::StmData::MemoryDataRef fullImageStream(Data::ByteArrayR(fullImage, sizeof(fullImage)));
	imageObj->SetStream(fullImageStream, 0, sizeof(fullImage));
	imageObj->SetParameter(Media::PDFParameter::Parse(CSTR("/Subtype /Image /Width 3 /Height 1 /BitsPerComponent 8 /ColorSpace [/Indexed 2474 0 R 255 4 0 R]")));
	if (!Media::PDFImage::Parse(pdf, 1, parsers).SetTo(images))
	{
		printf("PDF indirect Indexed ICC image creation test failed\r\n");
		return 1;
	}
	ok = images->GetImage2(0, 0).SetTo(image) && image->GetImageType() == Media::ImageType::Raster &&
		NN<Media::RasterImage>::ConvertFrom(image)->GetImageClass() == Media::RasterImage::ImageClass::StaticImage;
	if (ok)
	{
		staticImage = NN<Media::StaticImage>::ConvertFrom(NN<Media::RasterImage>::ConvertFrom(image));
		ok = staticImage->info.color.GetRAWICC().NotNull() && staticImage->pal.SetTo(palette) &&
			staticImage->data[0] == 0 && staticImage->data[1] == 127 && staticImage->data[2] == 255 &&
			palette[0] == 17 && palette[1] == 255 && palette[2] == 0 &&
			palette[127 * 4] == 17 && palette[127 * 4 + 1] == 128 && palette[127 * 4 + 2] == 127 &&
			palette[255 * 4] == 17 && palette[255 * 4 + 1] == 0 && palette[255 * 4 + 2] == 255;
	}
	images.Delete();
	if (!ok)
	{
		printf("PDF indirect Indexed ICC palette test failed\r\n");
		return 1;
	}
	return 0;
}

Int32 PDFObjectStreamTest()
{
	static const UInt8 compressed[] = {
		0x78,0x9c,0x33,0x53,0x30,0x50,0x30,0x57,0x30,0xb4,0x54,0x30,0x51,0x30,0x36,0x51,0xb0,
		0xb1,0xd1,0x0f,0xa9,0x2c,0x48,0x55,0xd0,0x77,0xad,0x48,0xcc,0x2d,0xc8,0x49,0xb5,0xb3,
		0x53,0x88,0x56,0xd0,0x77,0x49,0x2d,0xcb,0x4c,0x4e,0x0d,0x72,0x77,0x52,0x88,0x55,0xb0,
		0xb4,0xb4,0x04,0x00,0x45,0x41,0x0d,0x80
	};
	Text::StringBuilderUTF8 sb;
	sb.Append(CSTR("%PDF-1.6\n"));
	UIntOS existingOffset = sb.GetLength();
	sb.Append(CSTR("4 0 obj\n<< /Type /Existing >>\nendobj\n"));
	sb.Append(CSTR("6 0 obj\n(Old data)\nendobj\n7 0 obj\n<< /Type /Old >>\nendobj\n"));
	UIntOS streamOffset = sb.GetLength();
	sb.Append(CSTR("5 0 obj\n<< /Type /ObjStm /N 3 /First 14 /Filter /FlateDecode /Length "));
	sb.AppendU32((UInt32)sizeof(compressed));
	sb.Append(CSTR(" >>\nstream\n"));
	sb.AppendC(compressed, sizeof(compressed));
	sb.Append(CSTR("\nendstream\nendobj\n"));
	sb.Append(CSTR("9 0 obj\n<< /Type /ObjStm /N 1 /First 0 /Length 1 >>\nstream\nx\nendstream\nendobj\n"));
	UIntOS xrefOffset = sb.GetLength();
	UInt8 xref[9 * 4];
	MemClear(xref, sizeof(xref));
	xref[4 * 4] = 1;
	xref[4 * 4 + 1] = (UInt8)(existingOffset >> 8);
	xref[4 * 4 + 2] = (UInt8)existingOffset;
	xref[5 * 4] = 1;
	xref[5 * 4 + 1] = (UInt8)(streamOffset >> 8);
	xref[5 * 4 + 2] = (UInt8)streamOffset;
	xref[6 * 4] = 2;
	xref[6 * 4 + 2] = 5;
	xref[7 * 4] = 2;
	xref[7 * 4 + 2] = 5;
	xref[7 * 4 + 3] = 1;
	xref[8 * 4] = 1;
	xref[8 * 4 + 1] = (UInt8)(xrefOffset >> 8);
	xref[8 * 4 + 2] = (UInt8)xrefOffset;
	sb.Append(CSTR("8 0 obj\n<< /Type /XRef /Size 9 /W [1 2 1] /Length 36 >>\nstream\n"));
	UIntOS xrefDataOffset = sb.GetLength();
	sb.AppendC(xref, sizeof(xref));
	sb.Append(CSTR("\nendstream\nendobj\nstartxref\n"));
	sb.AppendU32((UInt32)xrefOffset);
	sb.Append(CSTR("\n%%EOF\n"));
	IO::StmData::MemoryDataRef input(Data::ByteArrayR(sb.v, sb.GetLength()));
	Parser::FileParser::PDFParser parser;
	NN<IO::ParsedObject> parsed;
	if (!parser.ParseFile(input, nullptr, IO::ParserType::Unknown).SetTo(parsed))
	{
		printf("PDF compressed object stream parse test failed\r\n");
		return 1;
	}
	NN<Media::PDFDocument> doc = NN<Media::PDFDocument>::ConvertFrom(parsed);
	NN<Media::PDFObject> object;
	NN<Text::String> type;
	NN<IO::StreamData> data;
	UInt8 output[14];
	Bool ok = doc->GetObjectById(4).SetTo(object) && object->GetType().SetTo(type) &&
		type->Equals(UTF8STRC("/Existing")) &&
		doc->GetObjectById(6).SetTo(object) && object->GetType().SetTo(type) &&
		type->Equals(UTF8STRC("/Example")) && object->GetRAWData().IsNull() &&
		doc->GetObjectById(7).SetTo(object) && object->GetType().IsNull() && object->GetData().SetTo(data) &&
		data->GetDataSize() == sizeof(output) &&
		data->GetRealData(0, sizeof(output), BYTEARR(output)) == sizeof(output) &&
		memcmp(output, "[ /DeviceRGB ]", sizeof(output)) == 0;
	doc.Delete();
	if (!ok)
	{
		printf("PDF compressed object stream contents test failed\r\n");
		return 1;
	}
	sb.v[xrefDataOffset + 7 * 4 + 3] = 2;
	if (parser.ParseFile(input, nullptr, IO::ParserType::Unknown).SetTo(parsed))
	{
		parsed.Delete();
		printf("PDF invalid compressed object reference test failed\r\n");
		return 1;
	}
	UIntOS newXrefOffset = sb.GetLength();
	sb.Append(CSTR("10 0 obj\n<< /Type /XRef /Size 11 /Index [7 1] /W [1 2 1] /Length 4 /Prev "));
	sb.AppendU32((UInt32)xrefOffset);
	sb.Append(CSTR(" >>\nstream\n"));
	const UInt8 freeEntry[] = {0, 0, 0, 0};
	sb.AppendC(freeEntry, sizeof(freeEntry));
	sb.Append(CSTR("\nendstream\nendobj\nstartxref\n"));
	sb.AppendU32((UInt32)newXrefOffset);
	sb.Append(CSTR("\n%%EOF\n"));
	IO::StmData::MemoryDataRef incrementalInput(Data::ByteArrayR(sb.v, sb.GetLength()));
	if (!parser.ParseFile(incrementalInput, nullptr, IO::ParserType::Unknown).SetTo(parsed))
	{
		printf("PDF incremental compressed object reference test failed\r\n");
		return 1;
	}
	doc = NN<Media::PDFDocument>::ConvertFrom(parsed);
	ok = doc->GetObjectById(6).SetTo(object) && object->GetType().SetTo(type) &&
		type->Equals(UTF8STRC("/Example")) &&
		doc->GetObjectById(7).SetTo(object) && object->GetType().SetTo(type) &&
		type->Equals(UTF8STRC("/Old"));
	doc.Delete();
	if (!ok)
	{
		printf("PDF incremental compressed object precedence test failed\r\n");
		return 1;
	}
	return 0;
}

Int32 PDFStreamTest()
{
	Int32 separationResult = PDFSeparationImageTest();
	if (separationResult != 0)
		return separationResult;
	Int32 iccResult = PDFICCImageTest();
	if (iccResult != 0)
		return iccResult;
	Int32 indexedResult = PDFIndexedImageTest();
	if (indexedResult != 0)
		return indexedResult;
	Int32 objectResult = PDFObjectStreamTest();
	if (objectResult != 0)
		return objectResult;
	Int32 predictorResult = PDFFlatePredictorTest();
	if (predictorResult != 0)
		return predictorResult;
	Int32 lzwResult = PDFLZWTest();
	if (lzwResult != 0)
		return lzwResult;
	NN<Media::PDFParameter> parameter;
	NN<Text::String> parameterValue;
	if (!Media::PDFParameter::Parse(CSTR("/O (abc[<</not-an-entry\\)x\\(foo\\\\bar) /P -1324 /R 3")).SetTo(parameter))
	{
		printf("PDF literal string parse test failed\r\n");
		return 1;
	}
	Bool parameterPassed = parameter->GetEntryValue(CSTR("O")).SetTo(parameterValue) &&
		parameterValue->Equals(UTF8STRC("(abc[<</not-an-entry\\)x\\(foo\\\\bar)")) &&
		parameter->GetEntryValue(CSTR("P")).SetTo(parameterValue) && parameterValue->Equals(UTF8STRC("-1324")) &&
		parameter->GetEntryValue(CSTR("R")).SetTo(parameterValue) && parameterValue->Equals(UTF8STRC("3"));
	parameter.Delete();
	if (!parameterPassed)
	{
		printf("PDF literal string parse test failed\r\n");
		return 1;
	}
	static const UInt8 compressed[] = {
		0x78,0xda,0x0b,0x70,0x71,0x53,0x48,0xce,0xcf,0x2d,0x28,0x4a,0x2d,0x2e,0x4e,0x4d,0x51,
		0x28,0x2e,0x29,0x4a,0x4d,0xcc,0x55,0x28,0x49,0x2d,0x2e,0x01,0x00,0x7e,0x50,0x09,0xbc
	};
	static const UInt8 expected[] = "PDF compressed stream test";
	UInt8 key[] = {1,2,3,4,5};
	NN<Text::String> name = Text::String::New(UTF8STRC("stream-test.pdf"));
	Media::PDFDocument pdf(name, CSTR("1.4"));
	name->Release();
	pdf.SetEncryptionRequired();
	NN<Media::PDFObject> obj = pdf.AddObject(1);
	obj->SetParameter(Media::PDFParameter::Parse(CSTR("/Filter /FlateDecode")));
	IO::StmData::MemoryDataRef noKeyStream(Data::ByteArrayR(compressed, sizeof(compressed)));
	obj->SetStream(noKeyStream, 0, sizeof(compressed));
	if (obj->GetData().NotNull() || !pdf.SetEncryptionKey(key, sizeof(key)))
	{
		printf("PDF encrypted stream key test failed\r\n");
		return 1;
	}
	UInt8 encrypted[sizeof(compressed) + 1];
	MemCopyNO(encrypted, compressed, sizeof(compressed));
	encrypted[sizeof(compressed)] = '\n';
	if (!pdf.DecryptStreamData(1, encrypted, sizeof(encrypted)))
		return 1;
	IO::StmData::MemoryDataRef encryptedStream(Data::ByteArrayR(encrypted, sizeof(encrypted)));
	obj->SetStream(encryptedStream, 0, sizeof(encrypted));
	NN<IO::StreamData> data;
	UInt8 output[sizeof(expected) - 1];
	if (!obj->GetData().SetTo(data) || data->GetDataSize() != sizeof(output) ||
		data->GetRealData(0, sizeof(output), BYTEARR(output)) != sizeof(output) ||
		memcmp(output, expected, sizeof(output)) != 0 ||
		!obj->GetData().SetTo(data) || data->GetDataSize() != sizeof(output))
	{
		printf("PDF encrypted Flate stream test failed\r\n");
		return 1;
	}
	if (!obj->GetRAWData().SetTo(data) || data->GetDataSize() != sizeof(encrypted))
	{
		printf("PDF raw stream preservation test failed\r\n");
		return 1;
	}
	IO::MemoryStream saved;
	if (!obj->SaveStream(saved) || saved.GetLength() != sizeof(output) ||
		memcmp(saved.GetArray().Arr().Ptr(), expected, sizeof(output)) != 0)
	{
		printf("PDF decoded stream save test failed\r\n");
		return 1;
	}
	obj->SetData(noKeyStream, 0, sizeof(compressed));
	if (!obj->GetData().SetTo(data) || data->GetDataSize() != sizeof(output) ||
		data->GetRealData(0, sizeof(output), BYTEARR(output)) != sizeof(output) ||
		memcmp(output, expected, sizeof(output)) != 0)
	{
		printf("PDF unencrypted data cache test failed\r\n");
		return 1;
	}
	obj->SetData(noKeyStream, 0, sizeof(compressed) - 1);
	if (obj->GetData().NotNull())
	{
		printf("PDF truncated Flate stream test failed\r\n");
		return 1;
	}
	NN<Media::PDFObject> xrefObj = pdf.AddObject(3);
	xrefObj->SetParameter(Media::PDFParameter::Parse(CSTR("/Type /XRef /Filter /FlateDecode")));
	xrefObj->SetStream(noKeyStream, 0, sizeof(compressed));
	if (!xrefObj->GetData().SetTo(data) || data->GetDataSize() != sizeof(output) ||
		data->GetRealData(0, sizeof(output), BYTEARR(output)) != sizeof(output) ||
		memcmp(output, expected, sizeof(output)) != 0)
	{
		printf("PDF unencrypted cross-reference stream test failed\r\n");
		return 1;
	}
	UInt8 plainData[] = {1,2,3,4};
	if (!pdf.DecryptStreamData(4, plainData, sizeof(plainData)))
		return 1;
	IO::StmData::MemoryDataRef plainStream(Data::ByteArrayR(plainData, sizeof(plainData)));
	NN<Media::PDFObject> plainObj = pdf.AddObject(4);
	plainObj->SetStream(plainStream, 0, sizeof(plainData));
	UInt8 plainOutput[sizeof(plainData)];
	if (!plainObj->GetData().SetTo(data) || data->GetDataSize() != sizeof(plainOutput) ||
		data->GetRealData(0, sizeof(plainOutput), BYTEARR(plainOutput)) != sizeof(plainOutput) ||
		plainOutput[0] != 1 || plainOutput[1] != 2 || plainOutput[2] != 3 || plainOutput[3] != 4)
	{
		printf("PDF encrypted unfiltered stream test failed\r\n");
		return 1;
	}
	UInt8 faxData[] = {0xA8};
	if (!pdf.DecryptStreamData(2, faxData, sizeof(faxData)))
		return 1;
	IO::StmData::MemoryDataRef faxStream(Data::ByteArrayR(faxData, sizeof(faxData)));
	NN<Media::PDFObject> faxObj = pdf.AddObject(2);
	faxObj->SetParameter(Media::PDFParameter::Parse(CSTR("/Subtype /Image /Filter /CCITTFaxDecode /Width 8 /Height 2 /DecodeParms << /K -1 /Columns 8 >>")));
	faxObj->SetStream(faxStream, 0, sizeof(faxData));
	Parser::FullParserList parsers;
	NN<Media::ImageList> images;
	NN<Media::Image> image;
	if (!Media::PDFImage::Parse(pdf, 2, parsers).SetTo(images))
	{
		printf("PDF encrypted CCITT stream test failed\r\n");
		return 1;
	}
	if (!images->GetImage2(0, 0).SetTo(image) || image->GetImageType() != Media::ImageType::Raster ||
		NN<Media::RasterImage>::ConvertFrom(image)->GetImageClass() != Media::RasterImage::ImageClass::StaticImage)
	{
		images.Delete();
		printf("PDF encrypted CCITT image test failed\r\n");
		return 1;
	}
	NN<Media::StaticImage> faxImage = NN<Media::StaticImage>::ConvertFrom(NN<Media::RasterImage>::ConvertFrom(image));
	Bool faxPassed = faxImage->data[faxImage->GetDataBpl() + 6] == 255 && faxImage->data[faxImage->GetDataBpl() + 7] == 0;
	images.Delete();
	if (!faxPassed)
	{
		printf("PDF encrypted CCITT image test failed\r\n");
		return 1;
	}
	return 0;
}

Int32 PDFSingleTest()
{
	Text::CStringNN fileName = CSTR("/mnt/QDisk1/Data/BookFile/371.pdf");
	Parser::FullParserList parsers;
	Parser::FileParser::PDFParser parser;
	NN<IO::ParsedObject> pobj;
	if (parser.ParseFilePath(fileName).SetTo(pobj))
	{
		printf("PDF parse succeeded for file %s\r\n", fileName.v.Ptr());
		NN<Media::PDFDocument> pdf = NN<Media::PDFDocument>::ConvertFrom(pobj);
		NN<Media::ImageList> imgList;
		Bool allImages = true;
		if (allImages)
		{
			NN<Media::PDFObject> obj;
			UIntOS imgCount = 0;
			UIntOS i = 0;
			UIntOS j = pdf->GetCount();
			while (i < j)
			{
				obj = pdf->GetItemNoCheck(i);
				if (obj->GetImageType(pdf) != Media::PDFObject::ImageType::NonImage)
				{
					if (Media::PDFImage::Parse(pdf, obj->GetId(), parsers).SetTo(imgList))
					{
						imgList.Delete();
						imgCount++;
					}
					else
					{
						printf("PDF id %d image creation failed\r\n", obj->GetId());
						pdf.Delete();
						return 1;
					}
				}
				i++;
			}
			printf("PDF all images creation succeeded, total images: %d\r\n", (Int32)imgCount);
		}
		else
		{
			UInt32 id = 19;
			if (Media::PDFImage::Parse(pdf, id, parsers).SetTo(imgList))
			{
				printf("PDF id %d image creation succeeded\r\n", id);
				NN<Media::Image> img;
				if (!imgList->GetImage2(0, 0).SetTo(img) || img->GetImageType() != Media::ImageType::Raster)
				{
					printf("PDF id %d image raster validation failed\r\n", id);
					imgList.Delete();
					pdf.Delete();
					return 1;
				}
				NN<Media::RasterImage> raster = NN<Media::RasterImage>::ConvertFrom(img);
				if (raster->GetImageClass() != Media::RasterImage::ImageClass::StaticImage)
				{
					printf("PDF id %d image raster validation failed\r\n", id);
					imgList.Delete();
					pdf.Delete();
					return 1;
				}
				NN<Media::StaticImage> staticImage = NN<Media::StaticImage>::ConvertFrom(raster);
				if (staticImage->info.dispSize.x != 2560 || staticImage->info.dispSize.y != 3307)
				{
					printf("PDF id %d image dimensions validation failed\r\n", id);
					imgList.Delete();
					pdf.Delete();
					return 1;
				}
				UInt64 hash = 14695981039346656037ULL;
				UIntOS bpl = staticImage->GetDataBpl();
				UIntOS row = 0;
				while (row < 3307)
				{
					UIntOS x = 0;
					while (x < 2560)
					{
						hash = (hash ^ staticImage->data[row * bpl + x++]) * 1099511628211ULL;
					}
					row++;
				}
				if (hash != 0xc659e537ae79b813ULL)
				{
					printf("PDF id %d image content validation failed: %016llx\r\n", id, (unsigned long long)hash);
					imgList.Delete();
					pdf.Delete();
					return 1;
				}
				imgList.Delete();
			}
			else
			{
				printf("PDF id %d image creation failed\r\n", id);
				pdf.Delete();
				return 1;
			}
		}
		pdf.Delete();
	}
	else
	{
		printf("PDF parse failed for file %s\r\n", fileName.v.Ptr());
	}
	return 0;
}

Int32 PDFAllTest()
{
	Text::CStringNN path = CSTR("/mnt/QDisk1/Data/BookFile/");
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UTF8Char> sptr2;
	sptr = path.ConcatTo(sbuff);
	sptr2 = Text::StrConcatC(sptr, UTF8STRC("*.pdf"));
	Parser::FileParser::PDFParser parser;
	NN<IO::ParsedObject> pobj;
	NN<IO::Path::FindFileSession> sess;
	NN<Media::ImageList> imgList;
	Parser::FullParserList parsers;
	if (IO::Path::FindFile(CSTRP(sbuff, sptr2)).SetTo(sess))
	{
		IO::Path::PathType pt;
		while (IO::Path::FindNextFile(sptr, sess, nullptr, pt, nullptr).SetTo(sptr2))
		{
			if (pt == IO::Path::PathType::File)
			{
				//UIntOS imgCnt = 0;
				if (parser.ParseFilePath(CSTRP(sbuff, sptr2)).SetTo(pobj))
				{
					NN<Media::PDFDocument> pdf = NN<Media::PDFDocument>::ConvertFrom(pobj);
					UIntOS i = 0;
					UIntOS j = pdf->GetCount();
					while (i < j)
					{
						NN<Media::PDFObject> pdfObj = pdf->GetItemNoCheck(i);
						Media::PDFObject::ImageType imgType = pdfObj->GetImageType(pdf);
						if (imgType == Media::PDFObject::ImageType::Unknown)
						{
							printf("PDF %s object %d has unknown image type\r\n", sptr.Ptr(), pdfObj->GetId());
							break;
						}
						else if (imgType != Media::PDFObject::ImageType::NonImage)
						{
							if (Media::PDFImage::Parse(pdf, pdfObj->GetId(), parsers).SetTo(imgList))
							{
								//imgCnt++;
								imgList.Delete();
							}
							else
							{
								printf("PDF %s object %d image creation failed\r\n", sptr.Ptr(), pdfObj->GetId());
								break;
							}
						}
						i++;
					}
					pobj.Delete();
					//printf("PDF %s has %d image objects\r\n", sptr.Ptr(), (Int32)imgCnt);
				}
				else
				{
					printf("PDF parse failed for file %s\r\n", sptr.Ptr());
				}
			}
		}
		IO::Path::FindFileClose(sess);
	}
	return 0;
}

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	UIntOS testType = 0;
	UIntOS argc;
	UnsafeArray<UnsafeArray<UTF8Char>> argv = progCtrl->GetCommandLines(progCtrl, argc);
	if (argc == 2)
	{
		if (Text::StrEqualsC(argv[1], Text::StrCharCnt(argv[1]), UTF8STRC("--self-test")))
		{
			Int32 result = CCITTFaxTest();
			if (result != 0) return result;
			result = JBIG2Test();
			if (result != 0) return result;
			return PDFStreamTest();
		}
		else
		{
			Text::StrToUIntOS(argv[1], testType);
		}
	}
	switch (testType)
	{
	case 0:
		return PDFSingleTest();
	case 1:
		return PDFAllTest();
	case 2:
		return CCITTFaxTest();
	case 3:
		return PDFStreamTest();
	case 4:
		return JBIG2Test();
	default:
		return 0;
	}
}
