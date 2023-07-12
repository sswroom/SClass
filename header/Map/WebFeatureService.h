#ifndef _SM_MAP_WEBFEATURESERVICE
#define _SM_MAP_WEBFEATURESERVICE
#include "Map/MapDrawLayer.h"
#include "Math/RectAreaDbl.h"
#include "Net/SSLEngine.h"
#include "Text/EncodingFactory.h"
#include "Text/String.h"
#include "Text/XMLReader.h"

namespace Map
{
	class WebFeatureService
	{
	private:
		struct FeatureType
		{
			Math::RectAreaDbl wgs84Bounds;
			Text::String *name;
			Text::String *title;
			Text::String *crs;
		};
	public:
		enum class Version
		{
			ANY,
			V1_0_0,
			V1_1_0,
			V2_0_0
		};
	private:
		Text::EncodingFactory *encFact;
		Text::String *wfsURL;
		Net::SocketFactory *sockf;
		Net::SSLEngine *ssl;

		Text::String *version;
		Data::ArrayList<FeatureType*> features;
		FeatureType *currFeature;

		void LoadXML(Version version);
		void LoadXMLFeatureType(Text::XMLReader *reader);
	public:
		WebFeatureService(Net::SocketFactory *sockf, Net::SSLEngine *ssl, Text::EncodingFactory *encFact, Text::CString wfsURL, Version version);
		virtual ~WebFeatureService();

		Bool IsError() const;
		void SetFeature(UOSInt index);
		UOSInt GetFeatureNames(Data::ArrayList<Text::String*> *nameList) const;

		Map::MapDrawLayer *LoadAsLayer();
	};
}
#endif
