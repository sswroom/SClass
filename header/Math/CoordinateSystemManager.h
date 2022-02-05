#ifndef _SM_MATH_COORDINATESYSTEMMANAGER
#define _SM_MATH_COORDINATESYSTEMMANAGER
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
			const Char *name;
		} SpheroidInfo;

		typedef struct
		{
			UInt32 srid;
			UInt32 spheroid;
			const Char *datumName;
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
			const UTF8Char *name;
			UOSInt nameLen;
			Math::CoordinateSystem::PrimemType primem;
			Math::CoordinateSystem::UnitType unit;
		} GeogcsSRInfo;

		typedef struct
		{
			UInt32 srid;
			UInt32 geogcsSRID;
			Math::CoordinateSystem::CoordinateSystemType csysType;
			const UTF8Char *projName;
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
		} SpatialRefInfo;

		typedef struct
		{
			UInt32 srid;
			Math::CoordinateSystem::CoordinateSystemType csysType;
			const UTF8Char *projName;
			UOSInt projNameLen;
			Double falseEasting;
			Double falseNorthing;
			Double centralMeridian;
			Double latitudeOfOrigin;
			Double scaleFactor;
			const Char *geoName;
		} ProjectedCSysInfo;

		typedef struct
		{
			UInt32 srid;
			const UTF8Char *geoName;
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
		static Math::CoordinateSystem *ParsePRJFile(Text::CString fileName);

		static const SpatialRefInfo *SRGetSpatialRef(UInt32 epsgId);
		static const SpatialRefInfo *SRGetSpatialRefPrev(UInt32 epsgId);
		static const SpatialRefInfo *SRGetSpatialRefNext(UInt32 epsgId);
		static const SpheroidInfo *SRGetSpheroid(UInt32 epsgId);
		static const DatumInfo *SRGetDatum(UInt32 epsgId);
		static const GeogcsSRInfo *SRGetGeogcsInfo(UInt32 epsgId);
		static const ProjcsSRInfo *SRGetProjcsInfo(UInt32 epsgId);
		static Math::CoordinateSystem *SRCreateCSys(UInt32 epsgId);
		static Math::ProjectedCoordinateSystem *SRCreateProjCSys(UInt32 epsgId);
		static Math::GeographicCoordinateSystem *SRCreateGeogCSys(UInt32 epsgId);

		static Math::CoordinateSystem *CreateFromName(Text::CString name);
		static Math::CoordinateSystem *ParsePRJBuff(Text::CString sourceName, UTF8Char *prjBuff, UOSInt buffSize, UOSInt *parsedSize);

		static const Math::CoordinateSystemManager::DatumInfo *GetDatumInfo(UInt32 srid);
		static const Math::CoordinateSystemManager::DatumInfo *GetDatumInfoByName(const UTF8Char *name);
		static void FillDatumData(Math::GeographicCoordinateSystem::DatumData1 *data, const DatumInfo *datum, const UTF8Char *name, Math::EarthEllipsoid *ee, const SpheroidInfo *spheroid);

		static Math::ProjectedCoordinateSystem *CreateProjCoordinateSystemDefName(ProjCoordSysType pcst);
		static Math::ProjectedCoordinateSystem *CreateProjCoordinateSystem(Text::CString sourceNmae, const UTF8Char *projName);
		static UOSInt GetProjCoordinateSystems(Data::ArrayList<ProjCoordSysType> *csysList);
		static UOSInt GetProjCoordinateSystemNames(Data::ArrayList<const UTF8Char *> *nameList);
		static const ProjectedCSysInfo *GetProjCoordinateSystemInfo(const UTF8Char *projName);

		static Math::GeographicCoordinateSystem *CreateGeogCoordinateSystemDefName(GeoCoordSysType gcst);
		static Math::GeographicCoordinateSystem *CreateGeogCoordinateSystem(Text::CString sourceName, const UTF8Char *geoName);
		static UOSInt GetGeogCoordinateSystems(Data::ArrayList<GeoCoordSysType> *csysList);
		static const GeographicCSysInfo *GetGeogCoordinateSystemInfo(const UTF8Char *geoName);
	private:
		static Bool ParsePRJString(UTF8Char *prjBuff, UOSInt *strSize);
	public:
		static Text::CString GeoCoordSysTypeGetName(GeoCoordSysType gcst);
		static Text::CString ProjCoordSysTypeGetName(ProjCoordSysType pcst);
	};
}
#endif
