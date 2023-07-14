#ifndef _SM_EXPORTER_ANIEXPORTER
#define _SM_EXPORTER_ANIEXPORTER
#include "IO/FileExporter.h"
#include "Media/ImageList.h"

namespace Exporter
{
	class ANIExporter : public IO::FileExporter
	{
	public:
		static Bool ImageSupported(Media::Image *img);
		static OSInt CalcBuffSize(Media::ImageList *imgList);
		static OSInt BuildBuff(UInt8 *buff, Media::ImageList *imgList, Bool hasHotSpot);
		
	public:
		ANIExporter();
		virtual ~ANIExporter();

		virtual Int32 GetName();
		virtual SupportType IsObjectSupported(IO::ParsedObject *pobj);
		virtual Bool GetOutputName(OSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff);
		virtual void SetCodePage(UInt32 codePage);
		virtual Bool ExportFile(NotNullPtr<IO::SeekableStream> stm, const UTF8Char *fileName, IO::ParsedObject *pobj, void *param);
	};
};
#endif
