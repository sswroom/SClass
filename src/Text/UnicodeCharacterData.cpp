#include "Stdafx.h"
#include "IO/FileStream.h"
#include "IO/StreamDataStream.h"
#include "Parser/FileParser/ZIPParser.h"
#include "Text/UTF8Reader.h"
#include "Text/UnicodeCharacterData.h"
#include <stdio.h>

NN<Text::UnicodeCharacterData::UnicodeData> Text::UnicodeCharacterData::GetOrNewUnicodeData(UInt32 code)
{
	OSInt i = 0;
	OSInt j = (OSInt)this->dataList.GetCount() - 1;
	OSInt k;
	NN<UnicodeData> data;
	while (i <= j)
	{
		k = (i + j) >> 1;
		data = this->dataList.GetItemNoCheck((UOSInt)k);
		if (data->code > code)
		{
			i = k + 1;
		}
		else if (data->code < code)
		{
			j = k - 1;
		}
		else
		{
			return data;
		}
	}
	data = MemAllocNN(UnicodeData);
	data->code = code;
	data->characterName = Text::String::NewEmpty();
	data->unicode10Name = Text::String::NewEmpty();
	data->uppercaseMapping = 0;
	data->lowercaseMapping = 0;
	data->titlecaseMapping = 0;
	this->dataList.Insert((UOSInt)i, data);
	return data;
}

Text::UnicodeCharacterData::UnicodeCharacterData()
{
}

Text::UnicodeCharacterData::~UnicodeCharacterData()
{
	UOSInt i = this->dataList.GetCount();
	NN<UnicodeData> data;
	while (i-- > 0)
	{
		data = this->dataList.GetItemNoCheck(i);
		data->characterName->Release();
		data->unicode10Name->Release();
		MemFreeNN(data);
	}
	this->dataList.Clear();
}

void Text::UnicodeCharacterData::LoadUnicodeDataFile(Text::CStringNN filePath)
{
	IO::FileStream fs(filePath, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	if (!fs.IsError())
	{
		LoadUnicodeDataFile(fs);
	}
}

void Text::UnicodeCharacterData::LoadUnicodeDataFile(NN<IO::Stream> stm)
{
	Text::PString sarr[16];
	NN<UnicodeData> data;
	UInt32 code;
	Text::UTF8Reader reader(stm);
	Text::StringBuilderUTF8 sb;
	while (reader.ReadLine(sb, 1024))
	{
		if (Text::StrSplitP(sarr, 16, sb, ';') == 15 && sarr[0].Hex2UInt32(code))
		{
			data = GetOrNewUnicodeData(code);
			data->characterName->Release();
			data->characterName = Text::String::New(sarr[1].ToCString());
			data->unicode10Name->Release();
			data->unicode10Name = Text::String::New(sarr[10].ToCString());
			if (sarr[12].Hex2UInt32(code))
			{
				data->uppercaseMapping = code;
			}
			if (sarr[13].Hex2UInt32(code))
			{
				data->lowercaseMapping = code;
			}
			if (sarr[14].Hex2UInt32(code))
			{
				data->titlecaseMapping = code;
			}
		}
		sb.ClearStr();
	}
}

void Text::UnicodeCharacterData::LoadUCDPackage(NN<IO::PackageFile> pkg)
{
	NN<IO::StreamData> stmData;
	if (pkg->GetItemStmDataNew(CSTR("UnicodeData.txt")).SetTo(stmData))
	{
		IO::StreamDataStream stm(stmData);
		LoadUnicodeDataFile(stm);
		stmData.Delete();
	}
}

void Text::UnicodeCharacterData::LoadUCDZip(Text::CStringNN ucdZipFile)
{
	Parser::FileParser::ZIPParser parser;
	NN<IO::ParsedObject> pobj;
	if (parser.ParseFilePath(ucdZipFile).SetTo(pobj))
	{
		LoadUCDPackage(NN<IO::PackageFile>::ConvertFrom(pobj));
		pobj.Delete();
	}
}

Optional<Text::UnicodeCharacterData::UnicodeData> Text::UnicodeCharacterData::GetUnicodeData(UInt32 code) const
{
	OSInt i = 0;
	OSInt j = (OSInt)this->dataList.GetCount() - 1;
	OSInt k;
	NN<UnicodeData> data;
	while (i <= j)
	{
		k = (i + j) >> 1;
		data = this->dataList.GetItemNoCheck((UOSInt)k);
		if (data->code > code)
		{
			i = k + 1;
		}
		else if (data->code < code)
		{
			j = k - 1;
		}
		else
		{
			return data;
		}
	}
	return nullptr;
}
