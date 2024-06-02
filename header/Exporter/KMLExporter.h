#ifndef _SM_EXPORTER_KMLEXPORTER
#define _SM_EXPORTER_KMLEXPORTER
#include "IO/FileExporter.h"

namespace Exporter
{
	class KMLExporter : public IO::FileExporter
	{
	private:
		UInt32 codePage;
		Optional<Text::EncodingFactory> encFact;
	public:
		KMLExporter();
		virtual ~KMLExporter();

		virtual Int32 GetName();
		virtual SupportType IsObjectSupported(NN<IO::ParsedObject> pobj);
		virtual Bool GetOutputName(UOSInt index, UnsafeArray<UTF8Char> nameBuff, UnsafeArray<UTF8Char> fileNameBuff);
		virtual void SetCodePage(UInt32 codePage);
		virtual void SetEncFactory(Optional<Text::EncodingFactory> encFact);
		virtual Bool ExportFile(NN<IO::SeekableStream> stm, Text::CStringNN fileName, NN<IO::ParsedObject> pobj, Optional<ParamData> param);

		virtual UOSInt GetParamCnt();
		virtual Optional<ParamData> CreateParam(NN<IO::ParsedObject> pobj);
		virtual void DeleteParam(Optional<ParamData> param);
		virtual Bool GetParamInfo(UOSInt index, NN<ParamInfo> info);
		virtual Bool SetParamInt32(Optional<ParamData> param, UOSInt index, Int32 val);
		virtual Int32 GetParamInt32(Optional<ParamData> param, UOSInt index);

	};
}
#endif
