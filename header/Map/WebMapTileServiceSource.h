#ifndef _SM_MAP_WEBMAPTILESERVICESOURCE
#define _SM_MAP_WEBMAPTILESERVICESOURCE
#include "Data/ArrayList.h"
#include "Data/ArrayListNN.h"
#include "Data/FastStringMapNN.h"
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
			NN<Text::String> id;
			Int32 minRow;
			Int32 maxRow;
			Int32 minCol;
			Int32 maxCol;
		};

		struct ResourceURL
		{
			NN<Text::String> templateURL;
			ResourceType resourceType;
			NN<Text::String> format;
			Map::TileMap::ImageType imgType;
		};

		struct TileMatrixSet
		{
			Math::RectAreaDbl bounds;
			NN<Text::String> id;
			NN<Math::CoordinateSystem> csys;
			Data::ArrayListNN<TileMatrix> tiles;
		};

		struct TileMatrixDef
		{
			Math::Coord2DDbl origin;
			NN<Text::String> id;
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
			NN<Math::CoordinateSystem> csys;
			Data::ArrayListNN<TileMatrixDef> tiles;
		};

		struct TileLayer
		{
			Math::RectAreaDbl wgs84Bounds;
			Optional<Text::String> title;
			Optional<Text::String> id;
			Data::ArrayListStringNN format;
			Data::ArrayListStringNN infoFormat;
			Data::ArrayListNN<TileMatrixSet> tileMatrixes;
			Data::ArrayListNN<ResourceURL> resourceURLs;
		};
	private:
		Optional<Text::EncodingFactory> encFact;
		NN<Text::String> wmtsURL;
		NN<Text::String> cacheDir;
		NN<Net::SocketFactory> sockf;
		Optional<Net::SSLEngine> ssl;
		Data::FastStringMapNN<TileLayer> layers;
		Optional<TileLayer> currLayer;
		Optional<TileMatrixSet> currSet;
		Optional<TileMatrixDefSet> currDef;
		Optional<ResourceURL> currResource;
		Optional<ResourceURL> currResourceInfo;
		NN<Math::CoordinateSystem> wgs84;
		Data::FastStringMapNN<TileMatrixDefSet> matrixDef;

		void LoadXML();
		void ReadLayer(NN<Text::XMLReader> reader);
		Optional<TileMatrixSet> ReadTileMatrixSetLink(NN<Text::XMLReader> reader);
		Optional<TileMatrixDefSet> ReadTileMatrixSet(NN<Text::XMLReader> reader);
		Optional<TileMatrix> GetTileMatrix(UOSInt level) const;
		static void ReleaseLayer(NN<TileLayer> layer);
		static void ReleaseTileMatrix(NN<TileMatrix> tileMatrix);
		static void ReleaseTileMatrixSet(NN<TileMatrixSet> set);
		static void ReleaseTileMatrixDef(NN<TileMatrixDef> tileMatrix);
		static void ReleaseTileMatrixDefSet(NN<TileMatrixDefSet> set);
		static void ReleaseResourceURL(NN<ResourceURL> resourceURL);
	public:
		WebMapTileServiceSource(NN<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, Optional<Text::EncodingFactory> encFact, Text::CString wmtsURL);
		virtual ~WebMapTileServiceSource();

		virtual Text::CStringNN GetName() const;
		virtual Bool IsError() const;
		virtual TileType GetTileType() const;
		virtual UOSInt GetMinLevel() const;
		virtual UOSInt GetMaxLevel() const;
		virtual Double GetLevelScale(UOSInt level) const;
		virtual UOSInt GetNearestLevel(Double scale) const;
		virtual UOSInt GetConcurrentCount() const;
		virtual Bool GetBounds(OutParam<Math::RectAreaDbl> bounds) const;
		virtual NN<Math::CoordinateSystem> GetCoordinateSystem() const;
		virtual Bool IsMercatorProj() const;
		virtual UOSInt GetTileSize() const;
		virtual ImageType GetImageType() const;
		virtual Bool CanQuery() const;
		virtual Bool QueryInfos(Math::Coord2DDbl coord, UOSInt level, NN<Data::ArrayListNN<Math::Geometry::Vector2D>> vecList, NN<Data::ArrayList<UOSInt>> valueOfstList, NN<Data::ArrayListStringNN> nameList, NN<Data::ArrayListNN<Text::String>> valueList) const;

		virtual UOSInt GetTileImageIDs(UOSInt level, Math::RectAreaDbl rect, Data::ArrayList<Math::Coord2D<Int32>> *ids);
		virtual Media::ImageList *LoadTileImage(UOSInt level, Math::Coord2D<Int32> tileId, NN<Parser::ParserList> parsers, OutParam<Math::RectAreaDbl> bounds, Bool localOnly);
		virtual UTF8Char *GetTileImageURL(UTF8Char *sbuff, UOSInt level, Math::Coord2D<Int32> tileId);
		virtual Bool GetTileImageURL(NN<Text::StringBuilderUTF8> sb, UOSInt level, Math::Coord2D<Int32> tileId);
		virtual Optional<IO::StreamData> LoadTileImageData(UOSInt level, Math::Coord2D<Int32> tileId, OutParam<Math::RectAreaDbl> bounds, Bool localOnly, OptOut<ImageType> it);

		Bool SetLayer(UOSInt index);
		Bool SetMatrixSet(UOSInt index);
		Bool SetResourceTileType(UOSInt index);
		Bool SetResourceInfoType(UOSInt index);
		Bool SetResourceInfoType(Text::CString name);
		UOSInt GetResourceInfoType();
		UOSInt GetLayerNames(NN<Data::ArrayListNN<Text::String>> layerNames);
		UOSInt GetMatrixSetNames(NN<Data::ArrayListStringNN> matrixSetNames);
		UOSInt GetResourceTileTypeNames(NN<Data::ArrayListStringNN> resourceTypeNames);
		UOSInt GetResourceInfoTypeNames(NN<Data::ArrayListStringNN> resourceTypeNames);
		static Text::CString GetExt(Map::TileMap::ImageType imgType);
	};
}
#endif
