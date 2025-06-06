#ifndef _SM_MAP_WEBMAPSERVICE
#define _SM_MAP_WEBMAPSERVICE
#include "Data/ArrayListNN.h"
#include "Map/DrawMapService.h"
#include "Net/SSLEngine.h"
#include "Text/EncodingFactory.h"
#include "Text/String.h"
#include "Text/XMLReader.h"

namespace Map
{
	class WebMapService : public Map::DrawMapService
	{
	private:
		struct LayerCRS
		{
			Math::RectAreaDbl bounds;
			Optional<Text::String> name;
			Bool swapXY;
		};

		struct LayerInfo
		{
			Data::ArrayListNN<LayerCRS> crsList;
			NN<Text::String> name;
			NN<Text::String> title;
			Bool queryable;
		};
	public:
		enum class Version
		{
			ANY,
			V1_1_1,
			V1_3_0,
			V1_1_1_TILED
		};
	private:
		Optional<Text::EncodingFactory> encFact;
		NN<Text::String> wmsURL;
		NN<Net::TCPClientFactory> clif;
		Optional<Net::SSLEngine> ssl;
		NN<Math::CoordinateSystem> envCsys;

		Optional<Text::String> version;
		Data::ArrayListNN<LayerInfo> layers;
		Data::ArrayListStringNN mapImageTypeNames;
		Data::ArrayListStringNN infoTypeNames;
		UOSInt layer;
		UOSInt mapImageType;
		UOSInt infoType;

		Optional<LayerCRS> currCRS;
		NN<Math::CoordinateSystem> csys;

		void LoadXML(Version version);
		void LoadXMLRequest(NN<Text::XMLReader> reader);
		void LoadXMLLayers(NN<Text::XMLReader> reader);

	public:
		WebMapService(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Optional<Text::EncodingFactory> encFact, Text::CStringNN wmsURL, Version version, NN<Math::CoordinateSystem> envCsys);
		virtual ~WebMapService();

		virtual NN<Text::String> GetName() const;
		virtual NN<Math::CoordinateSystem> GetCoordinateSystem() const;
		virtual Math::RectAreaDbl GetInitBounds() const;
		virtual Bool GetBounds(OutParam<Math::RectAreaDbl> bounds) const;
		virtual Bool CanQuery() const;
		virtual Bool QueryInfos(Math::Coord2DDbl coord, Math::RectAreaDbl bounds, UInt32 width, UInt32 height, Double dpi, NN<Data::ArrayListNN<Math::Geometry::Vector2D>> vecList, NN<Data::ArrayList<UOSInt>> valueOfstList, NN<Data::ArrayListStringNN> nameList, NN<Data::ArrayListNN<Text::String>> valueList);
		virtual Optional<Media::ImageList> DrawMap(Math::RectAreaDbl bounds, UInt32 width, UInt32 height, Double dpi, Optional<Text::StringBuilderUTF8> sbUrl);

		Bool IsError() const;
		UOSInt GetInfoType() const;
		UOSInt GetLayerCRS() const;
		void SetInfoType(UOSInt index);
		void SetLayer(UOSInt index);
		void SetMapImageType(UOSInt index);
		void SetLayerCRS(UOSInt index);
		UOSInt GetLayerNames(NN<Data::ArrayListStringNN> nameList) const;
		UOSInt GetMapImageTypeNames(NN<Data::ArrayListStringNN> nameList) const;
		UOSInt GetInfoTypeNames(NN<Data::ArrayListStringNN> nameList) const;
		UOSInt GetLayerCRSNames(NN<Data::ArrayListNN<Text::String>> nameList) const;
	};
}
#endif
