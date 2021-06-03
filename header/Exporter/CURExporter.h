#ifndef _SM_EXPORTER_CUREXPORTER
#define _SM_EXPORTER_CUREXPORTER
#include "IO/FileExporter.h"
#include "Media/ImageList.h"

namespace Exporter
{
	class CURExporter : public IO::FileExporter
	{
	public:
		static Bool ImageSupported(Media::Image *img);
		static UOSInt CalcBuffSize(Media::ImageList *imgList);
		static UOSInt BuildBuff(UInt8 *buff, Media::ImageList *imgList, Bool hasHotSpot);
		
	public:
		CURExporter();
		virtual ~CURExporter();

		virtual Int32 GetName();
		virtual SupportType IsObjectSupported(IO::ParsedObject *pobj);
		virtual Bool GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff);
		virtual void SetCodePage(UInt32 codePage);
		virtual Bool ExportFile(IO::SeekableStream *stm, const UTF8Char *fileName, IO::ParsedObject *pobj, void *param);
	};
}
#endif
