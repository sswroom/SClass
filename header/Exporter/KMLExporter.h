#ifndef _SM_EXPORTER_KMLEXPORTER
#define _SM_EXPORTER_KMLEXPORTER
#include "IO/FileExporter.h"

namespace Exporter
{
	class KMLExporter : public IO::FileExporter
	{
	private:
		Int32 codePage;
		Text::EncodingFactory *encFact;
	public:
		KMLExporter();
		virtual ~KMLExporter();

		virtual Int32 GetName();
		virtual SupportType IsObjectSupported(IO::ParsedObject *pobj);
		virtual Bool GetOutputName(OSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff);
		virtual void SetCodePage(Int32 codePage);
		virtual void SetEncFactory(Text::EncodingFactory *encFact);
		virtual Bool ExportFile(IO::SeekableStream *stm, const UTF8Char *fileName, IO::ParsedObject *pobj, void *param);

		virtual OSInt GetParamCnt();
		virtual void *CreateParam(IO::ParsedObject *pobj);
		virtual void DeleteParam(void *param);
		virtual Bool GetParamInfo(OSInt index, ParamInfo *info);
		virtual Bool SetParamInt32(void *param, OSInt index, Int32 val);
		virtual Int32 GetParamInt32(void *param, OSInt index);

	};
};
#endif
