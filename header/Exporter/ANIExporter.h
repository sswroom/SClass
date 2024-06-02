#ifndef _SM_EXPORTER_ANIEXPORTER
#define _SM_EXPORTER_ANIEXPORTER
#include "IO/FileExporter.h"
#include "Media/ImageList.h"

namespace Exporter
{
	class ANIExporter : public IO::FileExporter
	{
	public:
		static Bool ImageSupported(NN<Media::RasterImage> img);
		static OSInt CalcBuffSize(NN<Media::ImageList> imgList);
		static OSInt BuildBuff(UInt8 *buff, NN<Media::ImageList> imgList, Bool hasHotSpot);
		
	public:
		ANIExporter();
		virtual ~ANIExporter();

		virtual Int32 GetName();
		virtual SupportType IsObjectSupported(NN<IO::ParsedObject> pobj);
		virtual Bool GetOutputName(UOSInt index, UnsafeArray<UTF8Char> nameBuff, UnsafeArray<UTF8Char> fileNameBuff);
		virtual void SetCodePage(UInt32 codePage);
		virtual Bool ExportFile(NN<IO::SeekableStream> stm, Text::CStringNN fileName, NN<IO::ParsedObject> pobj, Optional<ParamData> param);
	};
};
#endif
