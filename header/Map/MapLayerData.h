#ifndef _SM_MAP_MAPLAYERDATA
#define _SM_MAP_MAPLAYERDATA
#include "IO/IFileBuffer.h"
#include "Map/IMapSearchLayer.h"

namespace Map
{
	class MapLayerData : public Map::IMapSearchLayer
	{
	public:
		UInt8 *cipFile;
		UInt8 *cixFile;
		UInt8 *ciuFile;
		UInt8 *blkFile;
		IO::IFileBuffer *cipFileObj;
		IO::IFileBuffer *cixFileObj;
		IO::IFileBuffer *ciuFileObj;
		IO::IFileBuffer *blkFileObj;
	public:
		MapLayerData(const UTF8Char *filePath);
		virtual ~MapLayerData();

		virtual Bool IsError();

		virtual UTF8Char *GetPGLabelD(UTF8Char *buff, Double xpos, Double ypos);
		virtual UTF8Char *GetPLLabelD(UTF8Char *buff, Double xpos, Double ypos, Double *xposOut, Double *yposOut);
	};
};
#endif
