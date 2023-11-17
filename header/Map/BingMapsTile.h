#ifndef _SM_MAP_BINGMAPSTILE
#define _SM_MAP_BINGMAPSTILE
#include "Data/ArrayListNN.h"
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
		Text::String *url;
		Text::String *key;
		Data::ArrayListNN<Text::String> subdomains;
		UOSInt urlNext;
		Sync::Mutex urlMut;
		Text::String *brandLogoUri;
		Media::SharedImage *brandLogoImg;
		Math::Size2DDbl dispSize;
		Double dispDPI;
		Bool hideLogo;

		Text::String *GetNextSubdomain();
	public:
		BingMapsTile(ImagerySet is, Text::CString key, Text::CString cacheDir, NotNullPtr<Net::SocketFactory> sockf, Net::SSLEngine *ssl);
		virtual ~BingMapsTile();

		virtual Bool IsError() const;
		virtual Text::CStringNN GetName() const;
		virtual TileType GetTileType() const;
		virtual ImageType GetImageType() const;
		virtual UOSInt GetConcurrentCount() const;
		virtual void SetDispSize(Math::Size2DDbl size, Double dpi);
		virtual UTF8Char *GetTileImageURL(UTF8Char *sbuff, UOSInt level, Math::Coord2D<Int32> imgId);

		virtual UOSInt GetScreenObjCnt();
		virtual Math::Geometry::Vector2D *CreateScreenObjVector(UOSInt index);
		virtual UTF8Char *GetScreenObjURL(UTF8Char *sbuff, UOSInt index);

		void SetHideLogo(Bool hideLogo);
		
		static void GetDefaultCacheDir(ImagerySet is, NotNullPtr<Text::StringBuilderUTF8> sb);
		static Text::CStringNN ImagerySetGetName(ImagerySet is);
		static UTF8Char *GenQuadkey(UTF8Char *sbuff, UOSInt level, Int32 imgX, Int32 imgY);
	};
}
#endif
