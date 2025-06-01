#ifndef _SM_MAP_HKPARKINGVACANCY
#define _SM_MAP_HKPARKINGVACANCY
#include "Data/FastStringMapNN.h"
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
			NN<Text::String> parkId;
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
		Data::FastStringMapNN<ParkingInfo> parkingMap;
		Sync::Mutex parkingMut;
		NN<Net::TCPClientFactory> clif;
		Optional<Net::SSLEngine> ssl;
		Math::RectAreaDbl bounds;

		void LoadParkingInfo();
		void LoadVacancy();
		static void __stdcall ParkingInfoFree(NN<ParkingInfo> parking);
	public:
		HKParkingVacancy(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl);
		virtual ~HKParkingVacancy();

		virtual DrawLayerType GetLayerType() const;
		virtual UOSInt GetAllObjectIds(NN<Data::ArrayListInt64> outArr, OptOut<Optional<NameArray>> nameArr);
		virtual UOSInt GetObjectIds(NN<Data::ArrayListInt64> outArr, OptOut<Optional<NameArray>> nameArr, Double mapRate, Math::RectArea<Int32> rect, Bool keepEmpty);
		virtual UOSInt GetObjectIdsMapXY(NN<Data::ArrayListInt64> outArr, OptOut<Optional<NameArray>> nameArr, Math::RectAreaDbl rect, Bool keepEmpty);
		virtual Int64 GetObjectIdMax() const;
		virtual void ReleaseNameArr(Optional<NameArray> nameArr);
		virtual Bool GetString(NN<Text::StringBuilderUTF8> sb, Optional<NameArray> nameArr, Int64 id, UOSInt strIndex);
		virtual UOSInt GetColumnCnt() const;
		virtual UnsafeArrayOpt<UTF8Char> GetColumnName(UnsafeArray<UTF8Char> buff, UOSInt colIndex);
		virtual DB::DBUtil::ColType GetColumnType(UOSInt colIndex, OptOut<UOSInt> colSize);
		virtual Bool GetColumnDef(UOSInt colIndex, NN<DB::ColDef> colDef);
		virtual UInt32 GetCodePage() const;
		virtual Bool GetBounds(OutParam<Math::RectAreaDbl> rect) const;

		virtual NN<GetObjectSess> BeginGetObject();
		virtual void EndGetObject(NN<GetObjectSess> session);
		virtual Optional<Math::Geometry::Vector2D> GetNewVectorById(NN<GetObjectSess> session, Int64 id);
		virtual UOSInt GetGeomCol() const;

		virtual ObjectClass GetObjectClass() const;
	};
}
#endif
