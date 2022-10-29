#ifndef _SM_MAP_HKPARKINGVACANCY
#define _SM_MAP_HKPARKINGVACANCY
#include "Data/FastStringMap.h"
#include "Map/IMapDrawLayer.h"
#include "Net/SSLEngine.h"
#include "Sync/Mutex.h"
#include "Text/String.h"

namespace Map
{
	class HKParkingVacancy : public Map::IMapDrawLayer
	{
	private:
		struct ParkingInfo
		{
			Text::String *parkId;
			Text::String *parkingNameEn;
			Text::String *parkingAddressEn;
			Text::String *parkingDistictEn;
			Text::String *parkingNameSc;
			Text::String *parkingAddressSc;
			Text::String *parkingDistictSc;
			Text::String *parkingNameTc;
			Text::String *parkingAddressTc;
			Text::String *parkingDistictTc;
			Text::String *parkingStarttime;
			Text::String *parkingEndtime;
			Text::String *parkingContactNo;
			Double parkingLatitude;
			Double parkingLongitude;
			OSInt vacancy;
			Data::Timestamp lastupdate;
		};
		
	private:
		Data::FastStringMap<ParkingInfo*> parkingMap;
		Sync::Mutex parkingMut;
		Net::SocketFactory *sockf;
		Net::SSLEngine *ssl;
		Math::RectAreaDbl bounds;

		void LoadParkingInfo();
		void LoadVacancy();
		void ParkingInfoFree(ParkingInfo *parking);
	public:
		HKParkingVacancy(Net::SocketFactory *sockf, Net::SSLEngine *ssl);
		virtual ~HKParkingVacancy();

		virtual DrawLayerType GetLayerType();
		virtual UOSInt GetAllObjectIds(Data::ArrayListInt64 *outArr, void **nameArr);
		virtual UOSInt GetObjectIds(Data::ArrayListInt64 *outArr, void **nameArr, Double mapRate, Math::RectArea<Int32> rect, Bool keepEmpty);
		virtual UOSInt GetObjectIdsMapXY(Data::ArrayListInt64 *outArr, void **nameArr, Math::RectAreaDbl rect, Bool keepEmpty);
		virtual Int64 GetObjectIdMax();
		virtual void ReleaseNameArr(void *nameArr);
		virtual UTF8Char *GetString(UTF8Char *buff, UOSInt buffSize, void *nameArr, Int64 id, UOSInt strIndex);
		virtual UOSInt GetColumnCnt();
		virtual UTF8Char *GetColumnName(UTF8Char *buff, UOSInt colIndex);
		virtual DB::DBUtil::ColType GetColumnType(UOSInt colIndex, UOSInt *colSize);
		virtual Bool GetColumnDef(UOSInt colIndex, DB::ColDef *colDef);
		virtual UInt32 GetCodePage();
		virtual Bool GetBounds(Math::RectAreaDbl *rect);

		virtual void *BeginGetObject();
		virtual void EndGetObject(void *session);
		virtual Math::Geometry::Vector2D *GetNewVectorById(void *session, Int64 id);

		virtual ObjectClass GetObjectClass();
	};
}
#endif
