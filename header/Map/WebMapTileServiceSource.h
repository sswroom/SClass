#ifndef _SM_MAP_WEBMAPTILESERVICESOURCE
#define _SM_MAP_WEBMAPTILESERVICESOURCE
#include "Data/ArrayList.h"
#include "Data/FastStringMap.h"
#include "Map/TileMap.h"
#include "Net/SocketFactory.h"
#include "Text/String.h"
#include "Text/XMLReader.h"

namespace Map
{
	class WebMapTileServiceSource : public Map::TileMap
	{
	private:
		struct TileSet
		{
			Double unitPerPixel;
			UOSInt order;
		};

		struct TileMatrixSet
		{
			Text::String *id;
			UOSInt tileWidth;
			UOSInt tileHeight;
			Math::RectAreaDbl bounds;
			Math::Coord2DDbl origin;
			Math::Coord2DDbl csysOrigin;
			ImageType imgType;
			Text::String *tileExt;
			Math::CoordinateSystem *csys;
			Text::String *url;
			Data::ArrayList<TileSet*> tileSets;
		};

		struct TileLayer
		{
			Text::String *id;
			Data::ArrayList<TileMatrixSet*> tileMatrixes;
		};
	private:
		Text::EncodingFactory *encFact;
		Text::String *wmtsURL;
		Text::String *cacheDir;
		Net::SocketFactory *sockf;
		Data::FastStringMap<TileLayer*> layers;
		TileLayer *currLayer;
		TileMatrixSet *currMatrix;

		void LoadXML();
		void ReadLayer(Text::XMLReader *reader);
		Double CalcScaleDiv();
		TileSet *GetTileSet(UOSInt level);
		void ReleaseLayer(TileLayer *layer);
		void ReleaseTileMatrix(TileMatrixSet *tileMatrix);
	public:
		WebMapTileServiceSource(Net::SocketFactory *sockf, Text::EncodingFactory *encFact, Text::CString wmtsURL);
		virtual ~WebMapTileServiceSource();

		virtual Text::CString GetName();
		virtual Bool IsError();
		virtual TileType GetTileType();
		virtual UOSInt GetLevelCount();
		virtual Double GetLevelScale(UOSInt level);
		virtual UOSInt GetNearestLevel(Double scale);
		virtual UOSInt GetConcurrentCount();
		virtual Bool GetBounds(Math::RectAreaDbl *bounds);
		virtual Math::CoordinateSystem *GetCoordinateSystem();
		virtual Bool IsMercatorProj();
		virtual UOSInt GetTileSize();

		virtual UOSInt GetImageIDs(UOSInt level, Math::RectAreaDbl rect, Data::ArrayList<Int64> *ids);
		virtual Media::ImageList *LoadTileImage(UOSInt level, Int64 imgId, Parser::ParserList *parsers, Math::RectAreaDbl *bounds, Bool localOnly);
		virtual UTF8Char *GetImageURL(UTF8Char *sbuff, UOSInt level, Int64 imgId);
		virtual IO::IStreamData *LoadTileImageData(UOSInt level, Int64 imgId, Math::RectAreaDbl *bounds, Bool localOnly, Int32 *blockX, Int32 *blockY, ImageType *it);
	};
}
#endif
