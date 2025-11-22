#ifndef _SM_MAP_BINGMAPSTILE
#define _SM_MAP_BINGMAPSTILE
#include "Data/ArrayListNN.hpp"
#include "Map/MercatorTileMap.h"
#include "Media/SharedImage.h"
#include "Sync/Mutex.h"

namespace Map
{
	class BingMapsTile : public Map::MercatorTileMap
	{
	public:
		enum class ImagerySet
		{
			Aerial,
			AerialWithLabelsOnDemand,
			Birdseye,
			BirdseyeWithLabels,
			BirdseyeV2,
			BirdseyeV2WithLabels,
			CanvasDark,
			CanvasLight,
			CanvasGray,
			OrdnanceSurvey,
			RoadOnDemand,
			Streetside,

			Last = Streetside
		};
	private:
		Optional<Text::String> url;
		Optional<Text::String> key;
		Data::ArrayListStringNN subdomains;
		UOSInt urlNext;
		Sync::Mutex urlMut;
		Optional<Text::String> brandLogoUri;
		Optional<Media::SharedImage> brandLogoImg;
		Math::Size2DDbl dispSize;
		Double dispDPI;
		Bool hideLogo;

		Optional<Text::String> GetNextSubdomain();
	public:
		BingMapsTile(ImagerySet is, Text::CString key, Text::CString cacheDir, NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl);
		virtual ~BingMapsTile();

		virtual Bool IsError() const;
		virtual Text::CStringNN GetName() const;
		virtual TileType GetTileType() const;
		virtual ImageType GetImageType() const;
		virtual UOSInt GetConcurrentCount() const;
		virtual void SetDispSize(Math::Size2DDbl size, Double dpi);
		virtual UnsafeArrayOpt<UTF8Char> GetTileImageURL(UnsafeArray<UTF8Char> sbuff, UOSInt level, Math::Coord2D<Int32> imgId);
		virtual Bool GetTileImageURL(NN<Text::StringBuilderUTF8> sb, UOSInt level, Math::Coord2D<Int32> imgId);

		virtual UOSInt GetScreenObjCnt();
		virtual Optional<Math::Geometry::Vector2D> CreateScreenObjVector(UOSInt index);
		virtual UnsafeArrayOpt<UTF8Char> GetScreenObjURL(UnsafeArray<UTF8Char> sbuff, UOSInt index);
		virtual Bool GetScreenObjURL(NN<Text::StringBuilderUTF8> sb, UOSInt index);

		void SetHideLogo(Bool hideLogo);
		
		static void GetDefaultCacheDir(ImagerySet is, NN<Text::StringBuilderUTF8> sb);
		static Text::CStringNN ImagerySetGetName(ImagerySet is);
		static UnsafeArray<UTF8Char> GenQuadkey(UnsafeArray<UTF8Char> sbuff, UOSInt level, Int32 imgX, Int32 imgY);
	};
}
#endif
