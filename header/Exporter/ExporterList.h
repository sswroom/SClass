#ifndef _SM_EXPORTER_EXPORTERLIST
#define _SM_EXPORTER_EXPORTERLIST
#include "IO/FileExporter.h"
#include "Data/ArrayList.h"

namespace Exporter
{
	class ExporterList
	{
	private:
		Data::ArrayListNN<IO::FileExporter> exporters;

	public:
		ExporterList();
		~ExporterList();

		void SetCodePage(UInt32 codePage);
		void SetEncFactory(Optional<Text::EncodingFactory> encFact);
		UOSInt GetSupportedExporters(NN<Data::ArrayListNN<IO::FileExporter>> exporters, NN<IO::ParsedObject> obj);
	};
}
#endif
