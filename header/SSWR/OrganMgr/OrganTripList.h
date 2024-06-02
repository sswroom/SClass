#ifndef _SM_SSWR_ORGANMGR_ORGANTRIPLIST
#define _SM_SSWR_ORGANMGR_ORGANTRIPLIST

#include "Data/DateTime.h"
#include "Data/ArrayList.h"
#include "DB/DBTool.h"
#include "Text/CString.h"
#include "Text/String.h"

namespace SSWR
{
	namespace OrganMgr
	{
		class Trip
		{
		public:
			Data::Timestamp fromDate;
			Data::Timestamp toDate;
			Int32 locId;

		public:
			Trip(const Data::Timestamp &fromDate, const Data::Timestamp &toDate, Int32 locId);
			~Trip();
			UnsafeArray<UTF8Char> ToString(UnsafeArray<UTF8Char> sbuff);
		};

		class Location
		{
		public:
			Int32 id;
			Int32 parId;
			NN<Text::String> ename;
			NN<Text::String> cname;
			Double lat;
			Double lon;
			Int32 locType;

			Location(Int32 id, Int32 parId, Text::CString ename, Text::CString cname, Int32 locType);
			~Location();
			UnsafeArray<UTF8Char> ToString(UnsafeArray<UTF8Char> sbuff);
		};

		class LocationType
		{
		public:
			Int32 id;
			NN<Text::String> engName;
			NN<Text::String> chiName;

			LocationType(Int32 id, Text::CString engName, Text::CString chiName);
			~LocationType();
		};

/*		class OrganTripList
		{
		private:
			DB::DBTool *db;
			Int32 cateId;
			Data::ArrayListNN<Trip> *trips;
			Data::ArrayListNN<Location> *locs;
			Data::ArrayListNN<LocationType> *locType;

			void FreeTrips();
		public:
			OrganTripList(DB::DBTool *db, Int32 cateId);
			~OrganTripList();
			void ReloadTrips(Int32 cateId);
			OSInt GetTripIndex(Data::DateTime *d);
			Trip *GetTrip(Int32 userId, Data::DateTime *d);
			Data::ArrayListNN<Trip> *GetTripList();
			Bool AddTrip(Data::DateTime *fromDate, Data::DateTime *toDate, Int32 locId);
			OSInt GetLocIndex(Int32 locId);
			Location *GetLocation(Int32 locId);
			Data::ArrayListNN<Location> *GetLocSub(Int32 locId);
			Bool UpdateLoc(Int32 locId, const UTF8Char *engName, const UTF8Char *chiName);
			Bool AddLoc(Int32 locId, const UTF8Char *engName, const UTF8Char *chiName);
			OSInt GetLocTypeIndex(Int32 lType);
		};*/
	}
}
#endif