#ifndef _SM_EXPORTER_EXPORTERLIST
#define _SM_EXPORTER_EXPORTERLIST
#include "IO/FileExporter.h"
#include "Data/ArrayList.h"

namespace Exporter
{
	class ExporterList
	{
	private:
		Data::ArrayList<IO::FileExporter*> *exporters;

	public:
		ExporterList();
		~ExporterList();

		void SetCodePage(Int32 codePage);
		void SetEncFactory(Text::EncodingFactory *encFact);
		UOSInt GetSupportedExporters(Data::ArrayList<IO::FileExporter*> *exporters, IO::ParsedObject *obj);
	};
}
#endif
