#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Data/ArrayListDbl.h"
#include "Data/ArrayListInt32.h"
#include "Data/ArrayListStringNN.h"
#include "IO/FileStream.h"
#include "IO/StreamReader.h"
#include "Map/KMLGenerator.h"
#include "Text/MyString.h"

Data::ArrayListInt32 *cids;
Data::ArrayListDbl *lats;
Data::ArrayListDbl *lons;

void LoadPeopleTxt(Text::CStringNN fileName)
{
	UTF8Char buff[512];
	UnsafeArray<UTF8Char> cols[3];
	UnsafeArray<UTF8Char> dm[3];
	NN<IO::FileStream> fs;
	IO::StreamReader *reader;

	NEW_CLASSNN(fs, IO::FileStream(fileName, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	if (fs->IsError())
	{
		fs.Delete();
		return;
	}
	NEW_CLASS(reader, IO::StreamReader(fs, 0));

	reader->ReadLine(buff, 512);
	while (reader->ReadLine(buff, 512).NotNull())
	{
		if (Text::StrSplit(cols, 3, buff, '\t') == 3)
		{
			Bool valid;
			Int32 cid;
			Double lat;
			Double lon;
			UIntOS i;
			valid = true;
			cid = Text::StrToInt32(cols[0]);
			
			if (Text::StrSplit(dm, 3, cols[1], '-') == 3)
			{
				lat = Text::StrToDoubleOr(&dm[0][1], 0) + Text::StrToDoubleOr(dm[1], 0) / 60.0 + Text::StrToDoubleOr(dm[2], 0) / 3600.0;
				if (dm[0][0] == 'S')
					lat = -lat;
			}
			else
			{
				valid = false;
			}

			if (Text::StrSplit(dm, 3, cols[2], '-') == 3)
			{
				lon = Text::StrToDoubleOr(&dm[0][1], 0) + Text::StrToDoubleOr(dm[1], 0) / 60.0 + Text::StrToDoubleOr(dm[2], 0) / 3600.0;
				if (dm[0][0] == 'W')
					lon = -lon;
			}
			else
			{
				valid = false;
			}
			if (valid)
			{
				i = cids->SortedInsert(cid);
				lats->Insert(i, lat);
				lons->Insert(i, lon);
			}
		}
	}

	DEL_CLASS(reader);
	fs.Delete();
}

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	IO::StreamReader *reader;
	NN<IO::FileStream> fs;
	Text::Encoding *enc;

	UTF8Char buff[2048];
	UTF8Char outBuff[1024];
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UTF8Char> cols[5];
	Int32 cid;
	IntOS si;

	NN<Data::ArrayListStringNN> outName;
	NN<Data::ArrayListDbl> outLats;
	NN<Data::ArrayListDbl> outLons;

	NEW_CLASS(enc, Text::Encoding(65001));
	NEW_CLASS(cids, Data::ArrayListInt32());
	NEW_CLASS(lats, Data::ArrayListDbl());
	NEW_CLASS(lons, Data::ArrayListDbl());

	NEW_CLASSNN(outName, Data::ArrayListStringNN());
	NEW_CLASSNN(outLats, Data::ArrayListDbl());
	NEW_CLASSNN(outLons, Data::ArrayListDbl());

	LoadPeopleTxt(CSTR("PeopleCSV2KML/All_ladlog_100921.txt"));

	NEW_CLASSNN(fs, IO::FileStream(CSTR("0_req/20100930/maplog.txt"), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	NEW_CLASS(reader, IO::StreamReader(fs, 0));

	while (reader->ReadLine(buff, 2048).NotNull())
	{
		if (Text::StrSplit(cols, 5, buff, ',') == 5)
		{
			if (Text::StrEquals(cols[1], U8STR("2")))
			{
				cid = Text::StrToInt32(cols[2]);
				si = cids->SortedIndexOf(cid);
				if (si >= 0)
				{
					sptr = Text::StrConcat(outBuff, cols[0]);
					sptr = Text::StrConcatC(sptr, UTF8STRC(","));
					sptr = Text::StrInt32(sptr, cid);
					outName->Add(Text::String::NewNotNullSlow(outBuff));
					outLats->Add(lats->GetItem((UIntOS)si));
					outLons->Add(lons->GetItem((UIntOS)si));
				}
			}
		}
	}

	DEL_CLASS(reader);
	fs.Delete();

	if (outName->GetCount() > 0)
	{
		NEW_CLASSNN(fs, IO::FileStream(CSTR("0_req/20100930/output.txt"), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
		Map::KMLGenerator::GenKMLPoints(fs, outName, outLats, outLons);
		fs.Delete();
		outName->FreeAll();
	}

	outLons.Delete();
	outLats.Delete();
	outName.Delete();

	DEL_CLASS(lons);
	DEL_CLASS(lats);
	DEL_CLASS(cids);
	DEL_CLASS(enc);
	return 0;
}
