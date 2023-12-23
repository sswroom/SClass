#ifndef _SM_MAP_ESRI_ESRIFEATURESERVER
#define _SM_MAP_ESRI_ESRIFEATURESERVER
#include "Data/ArrayListDbl.h"
#include "IO/Stream.h"
#include "Map/DrawMapService.h"
#include "Math/CoordinateSystem.h"
#include "Net/SocketFactory.h"
#include "Net/SSLEngine.h"
#include "Text/JSON.h"
#include "Text/String.h"

namespace Map
{
	namespace ESRI
	{
		class ESRIFeatureServer
		{
		private:
			NotNullPtr<Text::String> url;
			NotNullPtr<Text::String> name;
			NotNullPtr<Net::SocketFactory> sockf;
			Optional<Net::SSLEngine> ssl;

		public:
			ESRIFeatureServer(Text::CStringNN url, NotNullPtr<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl);
			virtual ~ESRIFeatureServer();

			NotNullPtr<Text::String> GetURL() const;
			NotNullPtr<Text::String> GetName() const;
		};
	}
}
#endif
