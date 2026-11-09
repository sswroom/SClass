#ifndef _SM_MATH_CSYS_COORDINATESYSTEMMANAGER
#define _SM_MATH_CSYS_COORDINATESYSTEMMANAGER
#include "Data/ArrayListArr.hpp"
#include "Data/ArrayListNative.hpp"
#include "Math/CSys/ProjectedCoordinateSystem.h"
#include "Text/CString.h"

namespace Math
{
	namespace CSys
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
				PCST_WEB_MERCATOR,

				PCST_FIRST = PCST_HK80,
				PCST_LAST = PCST_WEB_MERCATOR
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
				UIntOS nameLen;
			} SpheroidInfo;

			typedef struct
			{
				UInt32 srid;
				UInt32 spheroid;
				UnsafeArray<const UTF8Char> datumName;
				UIntOS datumNameLen;
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
				UIntOS nameLen;
				CoordinateSystem::PrimemType primem;
				CoordinateSystem::UnitType unit;
				Double unitScale;
				Bool reverseAxis;
			} GeogcsSRInfo;

			typedef struct
			{
				UInt32 srid;
				UInt32 geogcsSRID;
				CoordinateSystem::CoordinateSystemType csysType;
				UnsafeArray<const UTF8Char> projName;
				UIntOS projNameLen;
				Double falseEasting;
				Double falseNorthing;
				Double centralMeridian;
				Double latitudeOfOrigin;
				Double scaleFactor;
				CoordinateSystem::UnitType unit;
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
				CoordinateSystem::CoordinateSystemType csysType;
				UnsafeArray<const UTF8Char> projName;
				UIntOS projNameLen;
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
				UIntOS geoNameLen;
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
			static Optional<const SpatialRefInfo> SRGetSpatialRef(UInt32 epsgId);
			static Optional<const SpatialRefInfo> SRGetSpatialRefPrev(UInt32 epsgId);
			static Optional<const SpatialRefInfo> SRGetSpatialRefNext(UInt32 epsgId);
			static Optional<const SpheroidInfo> SRGetSpheroid(UInt32 epsgId);
			static Optional<const DatumInfo> SRGetDatum(UInt32 epsgId);
			static Optional<const GeogcsSRInfo> SRGetGeogcsInfo(UInt32 epsgId);
			static Optional<const ProjcsSRInfo> SRGetProjcsInfo(UInt32 epsgId);
			static Bool SRAxisReversed(UInt32 epsgId);
			static Optional<CoordinateSystem> SRCreateCSys(UInt32 epsgId);
			static NN<CoordinateSystem> SRCreateCSysOrDef(UInt32 epsgId);
			static Optional<ProjectedCoordinateSystem> SRCreateProjCSys(UInt32 epsgId);
			static Optional<GeographicCoordinateSystem> SRCreateGeogCSys(UInt32 epsgId);
			
			static Optional<CoordinateSystem> CreateFromName(Text::CStringNN name, OptOut<Bool> axisInvertable);
			static NN<CoordinateSystem> CreateFromNameOrDef(Text::CStringNN name);

			static Optional<const CoordinateSystemManager::DatumInfo> GetDatumInfoByName(UnsafeArray<const UTF8Char> name);
			static void FillDatumData(NN<GeographicCoordinateSystem::DatumData1> data, Optional<const DatumInfo> datum, Text::CStringNN name, NN<Math::EarthEllipsoid> ee, Optional<const SpheroidInfo> spheroid);

			static Optional<ProjectedCoordinateSystem> CreateProjCoordinateSystemDefName(ProjCoordSysType pcst);
			static NN<CoordinateSystem> CreateProjCoordinateSystemDefNameOrDef(ProjCoordSysType pcst);
			static Optional<ProjectedCoordinateSystem> CreateProjCoordinateSystem(Text::CStringNN sourceName, UnsafeArray<const UTF8Char> projName);
			static UIntOS GetProjCoordinateSystems(NN<Data::ArrayListNative<ProjCoordSysType>> csysList);
			static UIntOS GetProjCoordinateSystemNames(NN<Data::ArrayListArr<const UTF8Char>> nameList);
			static Optional<const ProjectedCSysInfo> GetProjCoordinateSystemInfo(UnsafeArray<const UTF8Char> projName);

			static Optional<GeographicCoordinateSystem> CreateGeogCoordinateSystemDefName(GeoCoordSysType gcst);
			static Optional<GeographicCoordinateSystem> CreateGeogCoordinateSystem(Text::CStringNN sourceName, UnsafeArray<const UTF8Char> geoName);
			static UIntOS GetGeogCoordinateSystems(NN<Data::ArrayListNative<GeoCoordSysType>> csysList);
			static Optional<const GeographicCSysInfo> GetGeogCoordinateSystemInfo(UnsafeArray<const UTF8Char> geoName);
			static NN<GeographicCoordinateSystem> CreateWGS84Csys();
			static NN<CoordinateSystem> CreateCsysByCoord(Math::Coord2DDbl coord);

			static Text::CStringNN GeoCoordSysTypeGetName(GeoCoordSysType gcst);
			static Text::CString ProjCoordSysTypeGetName(ProjCoordSysType pcst);
		};
	}
}
#endif
