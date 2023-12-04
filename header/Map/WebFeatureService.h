#ifndef _SM_MAP_WEBFEATURESERVICE
#define _SM_MAP_WEBFEATURESERVICE
#include "Data/ArrayListNN.h"
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
			NotNullPtr<Text::String> name;
			NotNullPtr<Text::String> title;
			NotNullPtr<Text::String> crs;
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
		NotNullPtr<Text::String> wfsURL;
		NotNullPtr<Net::SocketFactory> sockf;
		Optional<Net::SSLEngine> ssl;

		Text::String *version;
		Data::ArrayList<FeatureType*> features;
		FeatureType *currFeature;

		void LoadXML(Version version);
		void LoadXMLFeatureType(NotNullPtr<Text::XMLReader> reader);
	public:
		WebFeatureService(NotNullPtr<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, Text::EncodingFactory *encFact, Text::CString wfsURL, Version version);
		virtual ~WebFeatureService();

		Bool IsError() const;
		void SetFeature(UOSInt index);
		UOSInt GetFeatureNames(Data::ArrayListNN<Text::String> *nameList) const;

		Map::MapDrawLayer *LoadAsLayer();
	};
}
#endif
