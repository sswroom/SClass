#ifndef _SM_EXPORTER_CUREXPORTER
#define _SM_EXPORTER_CUREXPORTER
#include "IO/FileExporter.h"
#include "Media/ImageList.h"

namespace Exporter
{
	class CURExporter : public IO::FileExporter
	{
	public:
		static Bool ImageSupported(Media::RasterImage *img);
		static UOSInt CalcBuffSize(NN<Media::ImageList> imgList);
		static UOSInt BuildBuff(UInt8 *buff, NN<Media::ImageList> imgList, Bool hasHotSpot);
		
	public:
		CURExporter();
		virtual ~CURExporter();

		virtual Int32 GetName();
		virtual SupportType IsObjectSupported(NN<IO::ParsedObject> pobj);
		virtual Bool GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff);
		virtual void SetCodePage(UInt32 codePage);
		virtual Bool ExportFile(NN<IO::SeekableStream> stm, Text::CStringNN fileName, NN<IO::ParsedObject> pobj, Optional<ParamData> param);
	};
}
#endif
