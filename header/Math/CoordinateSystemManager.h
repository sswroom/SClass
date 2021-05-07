#ifndef _SM_MATH_COORDINATESYSTEMMANAGER
#define _SM_MATH_COORDINATESYSTEMMANAGER
#include "Math/ProjectedCoordinateSystem.h"

namespace Math
{
	class CoordinateSystemManager
	{
	public:
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
			const Char *name;
			Math::CoordinateSystem::PrimemType primem;
			Math::CoordinateSystem::UnitType unit;
		} GeogcsSRInfo;

		typedef struct
		{
			UInt32 srid;
			UInt32 geogcsSRID;
			Math::CoordinateSystem::CoordinateSystemType csysType;
			const Char *projName;
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
			const Char *projName;
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
			const Char *geoName;
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
		static Math::CoordinateSystem *ParsePRJFile(const UTF8Char *fileName);

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

		static Math::CoordinateSystem *CreateFromName(const UTF8Char *name);
		static Math::CoordinateSystem *ParsePRJBuff(const UTF8Char *sourceName, Char *prjBuff, UOSInt *parsedSize);

		static const Math::CoordinateSystemManager::DatumInfo *GetDatumInfo(UInt32 srid);
		static const Math::CoordinateSystemManager::DatumInfo *GetDatumInfoByName(const UTF8Char *name);
		static void FillDatumData(Math::GeographicCoordinateSystem::DatumData1 *data, const DatumInfo *datum, const Char *name, Math::EarthEllipsoid *ee, const SpheroidInfo *spheroid);

		static Math::ProjectedCoordinateSystem *CreateProjCoordinateSystemDefName(Math::ProjectedCoordinateSystem::ProjCoordSysType pcst);
		static Math::ProjectedCoordinateSystem *CreateProjCoordinateSystem(const UTF8Char *sourceNmae, const UTF8Char *projName);
		static UOSInt GetProjCoordinateSystems(Data::ArrayList<Math::ProjectedCoordinateSystem::ProjCoordSysType> *csysList);
		static UOSInt GetProjCoordinateSystemNames(Data::ArrayList<const UTF8Char *> *nameList);
		static const ProjectedCSysInfo *GetProjCoordinateSystemInfo(const UTF8Char *projName);

		static Math::GeographicCoordinateSystem *CreateGeogCoordinateSystemDefName(Math::GeographicCoordinateSystem::GeoCoordSysType gcst);
		static Math::GeographicCoordinateSystem *CreateGeogCoordinateSystem(const UTF8Char *sourceName, const UTF8Char *geoName);
		static UOSInt GetGeogCoordinateSystems(Data::ArrayList<Math::GeographicCoordinateSystem::GeoCoordSysType> *csysList);
		static const GeographicCSysInfo *GetGeogCoordinateSystemInfo(const UTF8Char *geoName);
	private:
		static Bool ParsePRJString(Char *prjBuff, UOSInt *strSize);
	};
}
#endif
