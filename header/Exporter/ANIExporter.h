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
		static IntOS CalcBuffSize(NN<Media::ImageList> imgList);
		static IntOS BuildBuff(UInt8 *buff, NN<Media::ImageList> imgList, Bool hasHotSpot);
		
	public:
		ANIExporter();
		virtual ~ANIExporter();

		virtual Int32 GetName();
		virtual SupportType IsObjectSupported(NN<IO::ParsedObject> pobj);
		virtual Bool GetOutputName(UIntOS index, UnsafeArray<UTF8Char> nameBuff, UnsafeArray<UTF8Char> fileNameBuff);
		virtual void SetCodePage(UInt32 codePage);
		virtual Bool ExportFile(NN<IO::SeekableStream> stm, Text::CStringNN fileName, NN<IO::ParsedObject> pobj, Optional<ParamData> param);
	};
};
#endif
