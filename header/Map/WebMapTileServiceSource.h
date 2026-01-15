#ifndef _SM_MAP_WEBMAPTILESERVICESOURCE
#define _SM_MAP_WEBMAPTILESERVICESOURCE
#include "Data/ArrayListNative.hpp"
#include "Data/ArrayListNN.hpp"
#include "Data/ArrayListT.hpp"
#include "Data/FastStringMapNN.hpp"
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
		NN<Net::TCPClientFactory> clif;
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
		Optional<TileMatrix> GetTileMatrix(UIntOS level) const;
		static void __stdcall ReleaseLayer(NN<TileLayer> layer);
		static void __stdcall ReleaseTileMatrix(NN<TileMatrix> tileMatrix);
		static void __stdcall ReleaseTileMatrixSet(NN<TileMatrixSet> set);
		static void __stdcall ReleaseTileMatrixDef(NN<TileMatrixDef> tileMatrix);
		static void __stdcall ReleaseTileMatrixDefSet(NN<TileMatrixDefSet> set);
		static void __stdcall ReleaseResourceURL(NN<ResourceURL> resourceURL);
	public:
		WebMapTileServiceSource(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Optional<Text::EncodingFactory> encFact, Text::CStringNN wmtsURL);
		virtual ~WebMapTileServiceSource();

		virtual Text::CStringNN GetName() const;
		virtual Bool IsError() const;
		virtual TileType GetTileType() const;
		virtual UIntOS GetMinLevel() const;
		virtual UIntOS GetMaxLevel() const;
		virtual Double GetLevelScale(UIntOS level) const;
		virtual UIntOS GetNearestLevel(Double scale) const;
		virtual UIntOS GetConcurrentCount() const;
		virtual Bool GetBounds(OutParam<Math::RectAreaDbl> bounds) const;
		virtual NN<Math::CoordinateSystem> GetCoordinateSystem() const;
		virtual Bool IsMercatorProj() const;
		virtual UIntOS GetTileSize() const;
		virtual ImageType GetImageType() const;
		virtual Bool CanQuery() const;
		virtual Bool QueryInfos(Math::Coord2DDbl coord, UIntOS level, NN<Data::ArrayListNN<Math::Geometry::Vector2D>> vecList, NN<Data::ArrayListNative<UIntOS>> valueOfstList, NN<Data::ArrayListStringNN> nameList, NN<Data::ArrayListNN<Text::String>> valueList) const;

		virtual UIntOS GetTileImageIDs(UIntOS level, Math::RectAreaDbl rect, NN<Data::ArrayListT<Math::Coord2D<Int32>>> ids);
		virtual Optional<Media::ImageList> LoadTileImage(UIntOS level, Math::Coord2D<Int32> tileId, NN<Parser::ParserList> parsers, OutParam<Math::RectAreaDbl> bounds, Bool localOnly);
		virtual UnsafeArrayOpt<UTF8Char> GetTileImageURL(UnsafeArray<UTF8Char> sbuff, UIntOS level, Math::Coord2D<Int32> tileId);
		virtual Bool GetTileImageURL(NN<Text::StringBuilderUTF8> sb, UIntOS level, Math::Coord2D<Int32> tileId);
		virtual Optional<IO::StreamData> LoadTileImageData(UIntOS level, Math::Coord2D<Int32> tileId, OutParam<Math::RectAreaDbl> bounds, Bool localOnly, OptOut<ImageType> it);

		Bool SetLayer(UIntOS index);
		Bool SetMatrixSet(UIntOS index);
		Bool SetResourceTileType(UIntOS index);
		Bool SetResourceInfoType(UIntOS index);
		Bool SetResourceInfoType(Text::CStringNN name);
		UIntOS GetResourceInfoType();
		UIntOS GetLayerNames(NN<Data::ArrayListNN<Text::String>> layerNames);
		UIntOS GetMatrixSetNames(NN<Data::ArrayListStringNN> matrixSetNames);
		UIntOS GetResourceTileTypeNames(NN<Data::ArrayListStringNN> resourceTypeNames);
		UIntOS GetResourceInfoTypeNames(NN<Data::ArrayListStringNN> resourceTypeNames);
		static Text::CStringNN GetExt(Map::TileMap::ImageType imgType);
	};
}
#endif
