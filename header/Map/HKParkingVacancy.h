#ifndef _SM_MAP_HKPARKINGVACANCY
#define _SM_MAP_HKPARKINGVACANCY
#include "Data/FastStringMap.h"
#include "Map/MapDrawLayer.h"
#include "Net/SSLEngine.h"
#include "Sync/Mutex.h"
#include "Text/String.h"

namespace Map
{
	class HKParkingVacancy : public Map::MapDrawLayer
	{
	private:
		struct ParkingInfo
		{
			NotNullPtr<Text::String> parkId;
			Optional<Text::String> parkingNameEn;
			Optional<Text::String> parkingAddressEn;
			Optional<Text::String> parkingDistictEn;
			Optional<Text::String> parkingNameSc;
			Optional<Text::String> parkingAddressSc;
			Optional<Text::String> parkingDistictSc;
			Optional<Text::String> parkingNameTc;
			Optional<Text::String> parkingAddressTc;
			Optional<Text::String> parkingDistictTc;
			Optional<Text::String> parkingStarttime;
			Optional<Text::String> parkingEndtime;
			Optional<Text::String> parkingContactNo;
			Double parkingLatitude;
			Double parkingLongitude;
			OSInt vacancy;
			Data::Timestamp lastupdate;
		};
		
	private:
		Data::FastStringMap<ParkingInfo*> parkingMap;
		Sync::Mutex parkingMut;
		NotNullPtr<Net::SocketFactory> sockf;
		Optional<Net::SSLEngine> ssl;
		Math::RectAreaDbl bounds;

		void LoadParkingInfo();
		void LoadVacancy();
		void ParkingInfoFree(ParkingInfo *parking);
	public:
		HKParkingVacancy(NotNullPtr<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl);
		virtual ~HKParkingVacancy();

		virtual DrawLayerType GetLayerType() const;
		virtual UOSInt GetAllObjectIds(NotNullPtr<Data::ArrayListInt64> outArr, NameArray **nameArr);
		virtual UOSInt GetObjectIds(NotNullPtr<Data::ArrayListInt64> outArr, NameArray **nameArr, Double mapRate, Math::RectArea<Int32> rect, Bool keepEmpty);
		virtual UOSInt GetObjectIdsMapXY(NotNullPtr<Data::ArrayListInt64> outArr, NameArray **nameArr, Math::RectAreaDbl rect, Bool keepEmpty);
		virtual Int64 GetObjectIdMax() const;
		virtual void ReleaseNameArr(NameArray *nameArr);
		virtual Bool GetString(NotNullPtr<Text::StringBuilderUTF8> sb, NameArray *nameArr, Int64 id, UOSInt strIndex);
		virtual UOSInt GetColumnCnt() const;
		virtual UTF8Char *GetColumnName(UTF8Char *buff, UOSInt colIndex);
		virtual DB::DBUtil::ColType GetColumnType(UOSInt colIndex, OptOut<UOSInt> colSize);
		virtual Bool GetColumnDef(UOSInt colIndex, NotNullPtr<DB::ColDef> colDef);
		virtual UInt32 GetCodePage() const;
		virtual Bool GetBounds(OutParam<Math::RectAreaDbl> rect) const;

		virtual GetObjectSess *BeginGetObject();
		virtual void EndGetObject(GetObjectSess *session);
		virtual Math::Geometry::Vector2D *GetNewVectorById(GetObjectSess *session, Int64 id);

		virtual ObjectClass GetObjectClass() const;
	};
}
#endif
