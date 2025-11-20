#ifndef _SM_MAP_WEBFEATURESERVICE
#define _SM_MAP_WEBFEATURESERVICE
#include "Data/ArrayListNN.hpp"
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
			NN<Text::String> name;
			NN<Text::String> title;
			NN<Text::String> crs;
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
		Optional<Text::EncodingFactory> encFact;
		NN<Text::String> wfsURL;
		NN<Net::TCPClientFactory> clif;
		Optional<Net::SSLEngine> ssl;

		Optional<Text::String> version;
		Data::ArrayListNN<FeatureType> features;
		Optional<FeatureType> currFeature;

		void LoadXML(Version version);
		void LoadXMLFeatureType(NN<Text::XMLReader> reader);
	public:
		WebFeatureService(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Optional<Text::EncodingFactory> encFact, Text::CStringNN wfsURL, Version version);
		virtual ~WebFeatureService();

		Bool IsError() const;
		void SetFeature(UOSInt index);
		UOSInt GetFeatureNames(Data::ArrayListStringNN *nameList) const;

		Optional<Map::MapDrawLayer> LoadAsLayer();
	};
}
#endif
