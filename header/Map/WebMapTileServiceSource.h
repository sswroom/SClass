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
		enum ResourceType
		{
			Unknown,
			Tile,
			FeatureInfo	
		};

		struct TileMatrix
		{
			Text::String *id;
			Int32 minRow;
			Int32 maxRow;
			Int32 minCol;
			Int32 maxCol;
		};

		struct ResourceURL
		{
			Text::String *templateURL;
			ResourceType resourceType;
			Text::String *format;
			Map::TileMap::ImageType imgType;
		};

		struct TileMatrixSet
		{
			Math::RectAreaDbl bounds;
			Text::String *id;
			Math::CoordinateSystem *csys;
			Data::ArrayList<TileMatrix*> tiles;
		};

		struct TileMatrixDef
		{
			Math::Coord2DDbl origin;
			Text::String *id;
			Double scaleDenom;
			Double unitPerPixel;
			UInt32 tileWidth;
			UInt32 tileHeight;
			UInt32 matrixWidth;
			UInt32 matrixHeight;
		};

		struct TileMatrixDefSet
		{
			Text::String *id;
			Math::CoordinateSystem *csys;
			Data::ArrayList<TileMatrixDef*> tiles;
		};

		struct TileLayer
		{
			Math::RectAreaDbl wgs84Bounds;
			Text::String *title;
			Text::String *id;
			Data::ArrayList<Text::String*> format;
			Data::ArrayList<Text::String*> infoFormat;
			Data::ArrayList<TileMatrixSet*> tileMatrixes;
			Data::ArrayList<ResourceURL*> resourceURLs;
		};
	private:
		Text::EncodingFactory *encFact;
		Text::String *wmtsURL;
		Text::String *cacheDir;
		Net::SocketFactory *sockf;
		Net::SSLEngine *ssl;
		Data::FastStringMap<TileLayer*> layers;
		TileLayer *currLayer;
		TileMatrixSet *currSet;
		TileMatrixDefSet *currDef;
		ResourceURL *currResource;
		ResourceURL *currResourceInfo;
		Math::CoordinateSystem *wgs84;
		Data::FastStringMap<TileMatrixDefSet*> matrixDef;

		void LoadXML();
		void ReadLayer(Text::XMLReader *reader);
		TileMatrixSet *ReadTileMatrixSetLink(Text::XMLReader *reader);
		TileMatrixDefSet *ReadTileMatrixSet(Text::XMLReader *reader);
		TileMatrix *GetTileMatrix(UOSInt level) const;
		void ReleaseLayer(TileLayer *layer);
		void ReleaseTileMatrix(TileMatrix *tileMatrix);
		void ReleaseTileMatrixSet(TileMatrixSet *set);
		void ReleaseTileMatrixDef(TileMatrixDef *tileMatrix);
		void ReleaseTileMatrixDefSet(TileMatrixDefSet *set);
		void ReleaseResourceURL(ResourceURL *resourceURL);
	public:
		WebMapTileServiceSource(Net::SocketFactory *sockf, Net::SSLEngine *ssl, Text::EncodingFactory *encFact, Text::CString wmtsURL);
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
		virtual Bool CanQuery() const;
		virtual Math::Geometry::Vector2D *QueryInfo(Math::Coord2DDbl coord, UOSInt level, Data::ArrayList<Text::String*> *nameList, Data::ArrayList<Text::String*> *valueList) const;

		virtual UOSInt GetImageIDs(UOSInt level, Math::RectAreaDbl rect, Data::ArrayList<Int64> *ids);
		virtual Media::ImageList *LoadTileImage(UOSInt level, Int64 imgId, Parser::ParserList *parsers, Math::RectAreaDbl *bounds, Bool localOnly);
		virtual UTF8Char *GetImageURL(UTF8Char *sbuff, UOSInt level, Int64 imgId);
		virtual IO::IStreamData *LoadTileImageData(UOSInt level, Int64 imgId, Math::RectAreaDbl *bounds, Bool localOnly, Int32 *blockX, Int32 *blockY, ImageType *it);

		Bool SetLayer(UOSInt index);
		Bool SetMatrixSet(UOSInt index);
		Bool SetResourceTileType(UOSInt index);
		Bool SetResourceInfoType(UOSInt index);
		UOSInt GetLayerNames(Data::ArrayList<Text::String*> *layerNames);
		UOSInt GetMatrixSetNames(Data::ArrayList<Text::String*> *matrixSetNames);
		UOSInt GetResourceTileTypeNames(Data::ArrayList<Text::String*> *resourceTypeNames);
		UOSInt GetResourceInfoTypeNames(Data::ArrayList<Text::String*> *resourceTypeNames);
		static Text::CString GetExt(Map::TileMap::ImageType imgType);
	};
}
#endif
