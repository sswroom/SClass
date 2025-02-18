#ifndef _SM_MATH_COORDINATESYSTEMMANAGER
#define _SM_MATH_COORDINATESYSTEMMANAGER
#include "Data/ArrayList.h"
#include "Data/ArrayListArr.h"
#include "Math/ProjectedCoordinateSystem.h"
#include "Text/CString.h"

namespace Math
{
	class CoordinateSystemManager
	{
	public:
		typedef enum
		{
			GCST_WGS84,
			GCST_CGCS2000,
			GCST_MACAU2009,
			GCST_HK1980,

			GCST_FIRST = GCST_WGS84,
			GCST_LAST = GCST_HK1980
		} GeoCoordSysType;

		typedef enum
		{
			PCST_HK80,
			PCST_HK63,
			PCST_UK_NATIONAL_GRID,
			PCST_IRISH_NATIONAL_GRID,
			PCST_MACAU_GRID,
			PCST_TWD67,
			PCST_TWD97,

			PCST_FIRST = PCST_HK80,
			PCST_LAST = PCST_TWD97
		} ProjCoordSysType;

		typedef struct
		{
			Int32 srid;
			const Char *name;
			Double value;
		} PrememInfo;

		typedef struct
		{
			Int32 srid;
			const Char *name;
			Double meterScale;
		} UnitInfo;
		
		typedef struct
		{
			UInt32 srid;
			Math::EarthEllipsoid::EarthEllipsoidType eet;
			UnsafeArray<const UTF8Char> name;
			UOSInt nameLen;
		} SpheroidInfo;

		typedef struct
		{
			UInt32 srid;
			UInt32 spheroid;
			UnsafeArray<const UTF8Char> datumName;
			UOSInt datumNameLen;
			Double x0;
			Double y0;
			Double z0;
			Double cX;
			Double cY;
			Double cZ;
			Double xAngle;
			Double yAngle;
			Double zAngle;
			Double scale;
			Math::Unit::Angle::AngleUnit aunit;
		} DatumInfo;

		typedef struct
		{
			UInt32 srid;
			UInt32 datum;
			UnsafeArray<const UTF8Char> name;
			UOSInt nameLen;
			Math::CoordinateSystem::PrimemType primem;
			Math::CoordinateSystem::UnitType unit;
			Double unitScale;
			Bool reverseAxis;
		} GeogcsSRInfo;

		typedef struct
		{
			UInt32 srid;
			UInt32 geogcsSRID;
			Math::CoordinateSystem::CoordinateSystemType csysType;
			UnsafeArray<const UTF8Char> projName;
			UOSInt projNameLen;
			Double falseEasting;
			Double falseNorthing;
			Double centralMeridian;
			Double latitudeOfOrigin;
			Double scaleFactor;
			Math::CoordinateSystem::UnitType unit;
		} ProjcsSRInfo;

		typedef enum
		{
			SRT_GEOGCS,
			SRT_PROJCS,
			SRT_PRIMEM,
			SRT_UNIT,
			SRT_DATUM,
			SRT_SPHERO
		} SpatialReferenceType;

		typedef enum
		{
			OT_EPSG
		} OrgType;

		typedef struct
		{
			UInt32 srid; //srid
			SpatialReferenceType srType;
			OrgType org;
			const Char *name;
			Double minXGeo;
			Double minYGeo;
			Double maxXGeo;
			Double maxYGeo;
		} SpatialRefInfo;

		typedef struct
		{
			UInt32 srid;
			Math::CoordinateSystem::CoordinateSystemType csysType;
			UnsafeArray<const UTF8Char> projName;
			UOSInt projNameLen;
			Double falseEasting;
			Double falseNorthing;
			Double centralMeridian;
			Double latitudeOfOrigin;
			Double scaleFactor;
			CoordinateSystem::UnitType unitType;
			const Char *geoName;
		} ProjectedCSysInfo;

		typedef struct
		{
			UInt32 srid;
			UnsafeArray<const UTF8Char> geoName;
			UOSInt geoNameLen;
			const Char *datumName;
			Math::EarthEllipsoid::EarthEllipsoidType eet;
		} GeographicCSysInfo;

	private:
		static SpheroidInfo spheroidSRID[];
		static DatumInfo datumSRID[];
		static GeogcsSRInfo geogcsSRID[];
		static ProjcsSRInfo projcsSRID[];
		static SpatialRefInfo srInfoList[];
		static DatumInfo datumList[];
		static GeographicCSysInfo csysList[];
		static ProjectedCSysInfo pcsysList[];

	public:
		static const SpatialRefInfo *SRGetSpatialRef(UInt32 epsgId);
		static const SpatialRefInfo *SRGetSpatialRefPrev(UInt32 epsgId);
		static const SpatialRefInfo *SRGetSpatialRefNext(UInt32 epsgId);
		static const SpheroidInfo *SRGetSpheroid(UInt32 epsgId);
		static const DatumInfo *SRGetDatum(UInt32 epsgId);
		static const GeogcsSRInfo *SRGetGeogcsInfo(UInt32 epsgId);
		static const ProjcsSRInfo *SRGetProjcsInfo(UInt32 epsgId);
		static Bool SRAxisReversed(UInt32 epsgId);
		static Optional<Math::CoordinateSystem> SRCreateCSys(UInt32 epsgId);
		static NN<Math::CoordinateSystem> SRCreateCSysOrDef(UInt32 epsgId);
		static Optional<Math::ProjectedCoordinateSystem> SRCreateProjCSys(UInt32 epsgId);
		static Optional<Math::GeographicCoordinateSystem> SRCreateGeogCSys(UInt32 epsgId);
		
		static Optional<Math::CoordinateSystem> CreateFromName(Text::CStringNN name);
		static NN<Math::CoordinateSystem> CreateFromNameOrDef(Text::CStringNN name);

		static Optional<const Math::CoordinateSystemManager::DatumInfo> GetDatumInfoByName(UnsafeArray<const UTF8Char> name);
		static void FillDatumData(NN<Math::GeographicCoordinateSystem::DatumData1> data, Optional<const DatumInfo> datum, Text::CStringNN name, NN<Math::EarthEllipsoid> ee, Optional<const SpheroidInfo> spheroid);

		static Optional<Math::ProjectedCoordinateSystem> CreateProjCoordinateSystemDefName(ProjCoordSysType pcst);
		static NN<Math::CoordinateSystem> CreateProjCoordinateSystemDefNameOrDef(ProjCoordSysType pcst);
		static Optional<Math::ProjectedCoordinateSystem> CreateProjCoordinateSystem(Text::CStringNN sourceNmae, UnsafeArray<const UTF8Char> projName);
		static UOSInt GetProjCoordinateSystems(Data::ArrayList<ProjCoordSysType> *csysList);
		static UOSInt GetProjCoordinateSystemNames(NN<Data::ArrayListArr<const UTF8Char>> nameList);
		static const ProjectedCSysInfo *GetProjCoordinateSystemInfo(UnsafeArray<const UTF8Char> projName);

		static Optional<Math::GeographicCoordinateSystem> CreateGeogCoordinateSystemDefName(GeoCoordSysType gcst);
		static Optional<Math::GeographicCoordinateSystem> CreateGeogCoordinateSystem(Text::CStringNN sourceName, UnsafeArray<const UTF8Char> geoName);
		static UOSInt GetGeogCoordinateSystems(Data::ArrayList<GeoCoordSysType> *csysList);
		static const GeographicCSysInfo *GetGeogCoordinateSystemInfo(UnsafeArray<const UTF8Char> geoName);
		static NN<Math::GeographicCoordinateSystem> CreateWGS84Csys();
		static NN<Math::CoordinateSystem> CreateCsysByCoord(Math::Coord2DDbl coord);

		static Text::CStringNN GeoCoordSysTypeGetName(GeoCoordSysType gcst);
		static Text::CString ProjCoordSysTypeGetName(ProjCoordSysType pcst);
	};
}
#endif
