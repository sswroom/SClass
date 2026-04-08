#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "IO/StmData/FileData.h"
#include "Parser/FullParserList.h"
#include "Exporter/KMLExporter.h"
#include "Map/MapDrawLayer.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	Text::CStringNN plFile = CSTR("20111022.csv");
	NN<IO::StmData::FileData> fd;
	Parser::ParserList *parsers;
	Exporter::KMLExporter *exporter;
	IO::ParserType pt;
	IO::ConsoleWriter *console;
	Double dist;

	NEW_CLASS(console, IO::ConsoleWriter());
	NEW_CLASSNN(fd, IO::StmData::FileData(plFile, false));
	NEW_CLASS(parsers, Parser::FullParserList());
	NEW_CLASS(exporter, Exporter::KMLExporter());
	NN<IO::ParsedObject> pobj;
	if (parsers->ParseFile(fd).SetTo(pobj))
	{
		pt = pobj->GetParserType();
		if (pt == IO::ParserType::MapLayer)
		{
			NN<Data::ArrayListInt64> objIds;
			NN<Map::MapDrawLayer> layer = NN<Map::MapDrawLayer>::ConvertFrom(pobj);
			NN<Map::GetObjectSess> sess = layer->BeginGetObject();
			Optional<Map::NameArray> nameArr;
			NN<Math::Geometry::Vector2D> vec;
			NN<Math::CoordinateSystem> coord;
			UIntOS i;

			NEW_CLASSNN(objIds, Data::ArrayListInt64());
			layer->GetAllObjectIds(objIds, nameArr);
			coord = layer->GetCoordinateSystem();

			i = objIds->GetCount();
			while (i-- > 0)
			{
				if (layer->GetNewVectorById(sess, objIds->GetItem(i)).SetTo(vec))
				{
					dist = 0;
					if (vec->GetVectorType() == Math::Geometry::Vector2D::VectorType::Polyline)
					{
						NN<Math::Geometry::Polyline> pl = NN<Math::Geometry::Polyline>::ConvertFrom(vec);
						dist = coord->CalDistance(pl, pl->HasZ(), Math::Unit::Distance::DU_METER);
						printf("ID = %lld, Dist = %lf(m)\r\n", objIds->GetItem(i), dist);
					}
					vec.Delete();
				}
			}
			
			layer->ReleaseNameArr(nameArr);
			objIds.Delete();
		}
		pobj.Delete();
	}
	DEL_CLASS(exporter);
	DEL_CLASS(parsers);
	fd.Delete();
	DEL_CLASS(console);
	return 0;
}
