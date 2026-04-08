#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "Map/LayerTools.h"
#include "Parser/FullParserList.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UTF8Char> sptr2;
	NN<IO::Path::FindFileSession> sess;
	NN<Data::ArrayListNN<Map::MapDrawLayer>> layers;
	Parser::ParserList *parsers;
	NN<Map::MapDrawLayer> lyr;
	IO::ParserType pt;
	NN<IO::ParsedObject> pobj;
	NN<IO::StmData::FileData> fd;
	UIntOS i;

	NEW_CLASS(parsers, Parser::FullParserList());
	NEW_CLASSNN(layers, Data::ArrayListNN<Map::MapDrawLayer>());
	sptr = Text::StrConcatC(sbuff, UTF8STRC("Hiking/HK Island East/"));
	sptr2 = Text::StrConcatC(sptr, UTF8STRC("*.csv"));
	if (IO::Path::FindFile(CSTRP(sbuff, sptr2)).SetTo(sess))
	{
		while (IO::Path::FindNextFile(sptr, sess, nullptr, nullptr, nullptr).SetTo(sptr2))
		{
			NEW_CLASSNN(fd, IO::StmData::FileData(CSTRP(sbuff, sptr2), false));
			if (parsers->ParseFile(fd).SetTo(pobj))
			{
				pt = pobj->GetParserType();
				if (pt == IO::ParserType::MapLayer)
				{
					layers->Add(NN<Map::MapDrawLayer>::ConvertFrom(pobj));
				}
				else
				{
					pobj.Delete();
				}
			}
			fd.Delete();
		}
		IO::Path::FindFileClose(sess);
	}
	DEL_CLASS(parsers);

	if (Optional<Map::MapDrawLayer>(Map::LayerTools::CombineLayers(layers, nullptr)).SetTo(lyr))
	{
		lyr.Delete();
	}

	i = layers->GetCount();
	while (i-- > 0)
	{
		lyr = layers->GetItemNoCheck(i);
		lyr.Delete();
	}
	layers.Delete();
	return 0;
}