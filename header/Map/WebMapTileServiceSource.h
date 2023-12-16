#ifndef _SM_MAP_WEBMAPTILESERVICESOURCE
#define _SM_MAP_WEBMAPTILESERVICESOURCE
#include "Data/ArrayList.h"
#include "Data/ArrayListNN.h"
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
			NotNullPtr<Text::String> id;
			Int32 minRow;
			Int32 maxRow;
			Int32 minCol;
			Int32 maxCol;
		};

		struct ResourceURL
		{
			NotNullPtr<Text::String> templateURL;
			ResourceType resourceType;
			NotNullPtr<Text::String> format;
			Map::TileMap::ImageType imgType;
		};

		struct TileMatrixSet
		{
			Math::RectAreaDbl bounds;
			NotNullPtr<Text::String> id;
			NotNullPtr<Math::CoordinateSystem> csys;
			Data::ArrayList<TileMatrix*> tiles;
		};

		struct TileMatrixDef
		{
			Math::Coord2DDbl origin;
			NotNullPtr<Text::String> id;
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
			NotNullPtr<Math::CoordinateSystem> csys;
			Data::ArrayList<TileMatrixDef*> tiles;
		};

		struct TileLayer
		{
			Math::RectAreaDbl wgs84Bounds;
			Text::String *title;
			Text::String *id;
			Data::ArrayListStringNN format;
			Data::ArrayListStringNN infoFormat;
			Data::ArrayList<TileMatrixSet*> tileMatrixes;
			Data::ArrayList<ResourceURL*> resourceURLs;
		};
	private:
		Text::EncodingFactory *encFact;
		NotNullPtr<Text::String> wmtsURL;
		NotNullPtr<Text::String> cacheDir;
		NotNullPtr<Net::SocketFactory> sockf;
		Optional<Net::SSLEngine> ssl;
		Data::FastStringMap<TileLayer*> layers;
		TileLayer *currLayer;
		TileMatrixSet *currSet;
		TileMatrixDefSet *currDef;
		ResourceURL *currResource;
		ResourceURL *currResourceInfo;
		NotNullPtr<Math::CoordinateSystem> wgs84;
		Data::FastStringMap<TileMatrixDefSet*> matrixDef;

		void LoadXML();
		void ReadLayer(NotNullPtr<Text::XMLReader> reader);
		TileMatrixSet *ReadTileMatrixSetLink(NotNullPtr<Text::XMLReader> reader);
		TileMatrixDefSet *ReadTileMatrixSet(NotNullPtr<Text::XMLReader> reader);
		TileMatrix *GetTileMatrix(UOSInt level) const;
		void ReleaseLayer(TileLayer *layer);
		void ReleaseTileMatrix(TileMatrix *tileMatrix);
		void ReleaseTileMatrixSet(TileMatrixSet *set);
		void ReleaseTileMatrixDef(TileMatrixDef *tileMatrix);
		void ReleaseTileMatrixDefSet(TileMatrixDefSet *set);
		void ReleaseResourceURL(ResourceURL *resourceURL);
	public:
		WebMapTileServiceSource(NotNullPtr<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, Text::EncodingFactory *encFact, Text::CString wmtsURL);
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
		virtual NotNullPtr<Math::CoordinateSystem> GetCoordinateSystem() const;
		virtual Bool IsMercatorProj() const;
		virtual UOSInt GetTileSize() const;
		virtual ImageType GetImageType() const;
		virtual Bool CanQuery() const;
		virtual Bool QueryInfos(Math::Coord2DDbl coord, UOSInt level, Data::ArrayList<Math::Geometry::Vector2D*> *vecList, Data::ArrayList<UOSInt> *valueOfstList, Data::ArrayListStringNN *nameList, Data::ArrayList<Text::String*> *valueList) const;

		virtual UOSInt GetTileImageIDs(UOSInt level, Math::RectAreaDbl rect, Data::ArrayList<Math::Coord2D<Int32>> *ids);
		virtual Media::ImageList *LoadTileImage(UOSInt level, Math::Coord2D<Int32> tileId, NotNullPtr<Parser::ParserList> parsers, OutParam<Math::RectAreaDbl> bounds, Bool localOnly);
		virtual UTF8Char *GetTileImageURL(UTF8Char *sbuff, UOSInt level, Math::Coord2D<Int32> tileId);
		virtual Bool GetTileImageURL(NotNullPtr<Text::StringBuilderUTF8> sb, UOSInt level, Math::Coord2D<Int32> tileId);
		virtual Optional<IO::StreamData> LoadTileImageData(UOSInt level, Math::Coord2D<Int32> tileId, OutParam<Math::RectAreaDbl> bounds, Bool localOnly, OptOut<ImageType> it);

		Bool SetLayer(UOSInt index);
		Bool SetMatrixSet(UOSInt index);
		Bool SetResourceTileType(UOSInt index);
		Bool SetResourceInfoType(UOSInt index);
		Bool SetResourceInfoType(Text::CString name);
		UOSInt GetResourceInfoType();
		UOSInt GetLayerNames(Data::ArrayList<Text::String*> *layerNames);
		UOSInt GetMatrixSetNames(Data::ArrayListStringNN *matrixSetNames);
		UOSInt GetResourceTileTypeNames(Data::ArrayListStringNN *resourceTypeNames);
		UOSInt GetResourceInfoTypeNames(Data::ArrayListStringNN *resourceTypeNames);
		static Text::CString GetExt(Map::TileMap::ImageType imgType);
	};
}
#endif
