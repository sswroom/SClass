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
			Text::String *name;
			Bool swapXY;
		};

		struct LayerInfo
		{
			Data::ArrayList<LayerCRS*> crsList;
			NotNullPtr<Text::String> name;
			Text::String *title;
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
		Text::EncodingFactory *encFact;
		NotNullPtr<Text::String> wmsURL;
		NotNullPtr<Net::SocketFactory> sockf;
		Optional<Net::SSLEngine> ssl;
		NotNullPtr<Math::CoordinateSystem> envCsys;

		Text::String *version;
		Data::ArrayList<LayerInfo*> layers;
		Data::ArrayListStringNN mapImageTypeNames;
		Data::ArrayListStringNN infoTypeNames;
		UOSInt layer;
		UOSInt mapImageType;
		UOSInt infoType;

		LayerCRS *currCRS;
		NotNullPtr<Math::CoordinateSystem> csys;

		void LoadXML(Version version);
		void LoadXMLRequest(NotNullPtr<Text::XMLReader> reader);
		void LoadXMLLayers(NotNullPtr<Text::XMLReader> reader);

	public:
		WebMapService(NotNullPtr<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, Text::EncodingFactory *encFact, Text::CString wmsURL, Version version, NotNullPtr<Math::CoordinateSystem> envCsys);
		virtual ~WebMapService();

		virtual NotNullPtr<Text::String> GetName() const;
		virtual NotNullPtr<Math::CoordinateSystem> GetCoordinateSystem() const;
		virtual Math::RectAreaDbl GetInitBounds() const;
		virtual Bool GetBounds(OutParam<Math::RectAreaDbl> bounds) const;
		virtual Bool CanQuery() const;
		virtual Bool QueryInfos(Math::Coord2DDbl coord, Math::RectAreaDbl bounds, UInt32 width, UInt32 height, Double dpi, NotNullPtr<Data::ArrayListNN<Math::Geometry::Vector2D>> vecList, Data::ArrayList<UOSInt> *valueOfstList, Data::ArrayListStringNN *nameList, Data::ArrayList<Text::String*> *valueList);
		virtual Media::ImageList *DrawMap(Math::RectAreaDbl bounds, UInt32 width, UInt32 height, Double dpi, Text::StringBuilderUTF8 *sbUrl);

		Bool IsError() const;
		UOSInt GetInfoType() const;
		UOSInt GetLayerCRS() const;
		void SetInfoType(UOSInt index);
		void SetLayer(UOSInt index);
		void SetMapImageType(UOSInt index);
		void SetLayerCRS(UOSInt index);
		UOSInt GetLayerNames(Data::ArrayListStringNN *nameList) const;
		UOSInt GetMapImageTypeNames(Data::ArrayListStringNN *nameList) const;
		UOSInt GetInfoTypeNames(Data::ArrayListStringNN *nameList) const;
		UOSInt GetLayerCRSNames(Data::ArrayList<Text::String*> *nameList) const;
	};
}
#endif
