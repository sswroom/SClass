#ifndef _SM_MAP_BINGMAPSTILE
#define _SM_MAP_BINGMAPSTILE
#include "Map/MercatorTileMap.h"
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
		Data::ArrayList<Text::String*> subdomains;
		UOSInt urlNext;
		Sync::Mutex urlMut;
		Text::String *brandLogoUri;

		Text::String *GetNextSubdomain();
	public:
		BingMapsTile(ImagerySet is, Text::CString key, Text::CString cacheDir, Net::SocketFactory *sockf, Net::SSLEngine *ssl);
		virtual ~BingMapsTile();

		virtual Bool IsError();
		virtual Text::CString GetName();
		virtual TileType GetTileType();
		virtual UOSInt GetConcurrentCount();
		virtual UTF8Char *GetImageURL(UTF8Char *sbuff, UOSInt level, Int64 imgId);

		static void GetDefaultCacheDir(ImagerySet is, Text::StringBuilderUTF8 *sb);
		static Text::CString ImagerySetGetName(ImagerySet is);
		static UTF8Char *GenQuadkey(UTF8Char *sbuff, UOSInt level, Int32 imgX, Int32 imgY);
	};
}
#endif
