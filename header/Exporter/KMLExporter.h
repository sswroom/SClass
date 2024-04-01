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
		virtual SupportType IsObjectSupported(NotNullPtr<IO::ParsedObject> pobj);
		virtual Bool GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff);
		virtual void SetCodePage(UInt32 codePage);
		virtual void SetEncFactory(Optional<Text::EncodingFactory> encFact);
		virtual Bool ExportFile(NotNullPtr<IO::SeekableStream> stm, Text::CStringNN fileName, NotNullPtr<IO::ParsedObject> pobj, Optional<ParamData> param);

		virtual UOSInt GetParamCnt();
		virtual Optional<ParamData> CreateParam(NotNullPtr<IO::ParsedObject> pobj);
		virtual void DeleteParam(Optional<ParamData> param);
		virtual Bool GetParamInfo(UOSInt index, NotNullPtr<ParamInfo> info);
		virtual Bool SetParamInt32(Optional<ParamData> param, UOSInt index, Int32 val);
		virtual Int32 GetParamInt32(Optional<ParamData> param, UOSInt index);

	};
}
#endif
