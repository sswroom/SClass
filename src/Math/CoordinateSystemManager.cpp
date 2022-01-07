#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/FileStream.h"
#include "Math/Math.h"
#include "Math/CoordinateSystemManager.h"
#include "Math/GeographicCoordinateSystem.h"
#include "Math/MercatorProjectedCoordinateSystem.h"
#include "Math/Mercator1SPProjectedCoordinateSystem.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"

#define VERBOSE
#if defined(VERBOSE)
#include <stdio.h>
#endif

Math::CoordinateSystemManager::SpheroidInfo Math::CoordinateSystemManager::spheroidSRID[] = {
//	{srid, Math::EarthEllipsoid::EarthEllipsoidType, name}
	{7012, Math::EarthEllipsoid::EET_CLARKE1880, "Clarke 1880 (RGS)"},	
	{7022, Math::EarthEllipsoid::EET_INTL1924,   "International 1924"},	
	{7030, Math::EarthEllipsoid::EET_WGS84,      "WGS 84"},	
};

Math::CoordinateSystemManager::DatumInfo Math::CoordinateSystemManager::datumSRID[] = {
//	{srid, spheroid,"name", x0, y0, z0, cX, cY, cZ, xAngle, yAngle, zAngle, scale},
	{6326,  7030,  "WGS_1984", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, Math::Unit::Angle::AU_RADIAN},
	{6600,  7012,  "Anguilla_1957", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, Math::Unit::Angle::AU_RADIAN},
	{6601,  7012,  "Antigua_1943", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, Math::Unit::Angle::AU_RADIAN},
	{6602,  7012,  "Dominica_1945", 0, 0, 0, 725, 685, 536, 0, 0, 0, 0, Math::Unit::Angle::AU_ARCSECOND},
	{6603,  7012,  "Grenada_1953", 0, 0, 0, 72, 213.7, 93, 0, 0, 0, 0, Math::Unit::Angle::AU_ARCSECOND},
	{6611,  7022,  "Hong_Kong_1980", 0, 0, 0, -162.619, -276.959, -161.764, 0.067753, -2.24365, -1.15883, -1.09425, Math::Unit::Angle::AU_ARCSECOND},
};

Math::CoordinateSystemManager::GeogcsSRInfo Math::CoordinateSystemManager::geogcsSRID[] = {
//	{srid,  datum, name,    primem,   unit},
	{4326,  6326,  "WGS 84", Math::CoordinateSystem::PT_GREENWICH, Math::CoordinateSystem::UT_DEGREE},
	{4600,  6600,  "Anguilla 1957", Math::CoordinateSystem::PT_GREENWICH, Math::CoordinateSystem::UT_DEGREE},
	{4601,  6601,  "Antigua 1943", Math::CoordinateSystem::PT_GREENWICH, Math::CoordinateSystem::UT_DEGREE},
	{4602,  6602,  "Dominica 1945", Math::CoordinateSystem::PT_GREENWICH, Math::CoordinateSystem::UT_DEGREE},
	{4603,  6603,  "Grenada 1953", Math::CoordinateSystem::PT_GREENWICH, Math::CoordinateSystem::UT_DEGREE},
	{4611,  6611,  "Hong Kong 1980", Math::CoordinateSystem::PT_GREENWICH, Math::CoordinateSystem::UT_DEGREE},
};

Math::CoordinateSystemManager::ProjcsSRInfo Math::CoordinateSystemManager::projcsSRID[] = {
//	{srid, geogcsSRID, csysType, projName, falseEasting, falseNorthing, centralMeridian, latitudeOfOrigin, scaleFactor, unit};
	{2000, 4600, Math::CoordinateSystem::CoordinateSystemType::MercatorProjected, "Anguilla 1957 / British West Indies Grid", 400000, 0, -62, 0, 0.9995, Math::CoordinateSystem::UT_METRE},
	{2001, 4601, Math::CoordinateSystem::CoordinateSystemType::MercatorProjected, "Antigua 1943 / British West Indies Grid", 400000, 0, -62, 0, 0.9995, Math::CoordinateSystem::UT_METRE},
	{2002, 4602, Math::CoordinateSystem::CoordinateSystemType::MercatorProjected, "Dominica 1945 / British West Indies Grid", 400000, 0, -62, 0, 0.9995, Math::CoordinateSystem::UT_METRE},
	{2003, 4603, Math::CoordinateSystem::CoordinateSystemType::MercatorProjected, "Grenada 1953 / British West Indies Grid", 400000, 0, -62, 0, 0.9995, Math::CoordinateSystem::UT_METRE},
	{2326, 4611, Math::CoordinateSystem::CoordinateSystemType::MercatorProjected, "Hong Kong 1980 Grid System", 836694.05, 819069.80, 114.17855555555555555555555555556, 22.312133333333333333333333333333, 1, Math::CoordinateSystem::UT_METRE},
	{3857, 4326, Math::CoordinateSystem::CoordinateSystemType::Mercator1SPProjected, "WGS 84 / Pseudo-Mercator", 0, 0, 0, 0, 1, Math::CoordinateSystem::UT_METRE},

};

Math::CoordinateSystemManager::SpatialRefInfo Math::CoordinateSystemManager::srInfoList[] = {
//	{id, csysType, projName};
	{2000,   SRT_PROJCS, OT_EPSG,  "Anguilla 1957 / British West Indies Grid"},
	{2001,   SRT_PROJCS, OT_EPSG,  "Antigua 1943 / British West Indies Grid"},
	{2002,   SRT_PROJCS, OT_EPSG,  "Dominica 1945 / British West Indies Grid"},
	{2003,   SRT_PROJCS, OT_EPSG,  "Grenada 1953 / British West IndMath::CoordinateSystem::CoordinateSystemType::GEOGRAPHICruger CM 114E"},
	{2326,   SRT_PROJCS, OT_EPSG,  "Hong Kong 1980 Grid System"},
	{3857,   SRT_PROJCS, OT_EPSG,  "WGS 84 / Pseudo-Mercator"},
	{4326,   SRT_GEOGCS, OT_EPSG,  "WGS 84"},
	{4600,   SRT_GEOGCS, OT_EPSG,  "Anguilla 1957"},
	{4601,   SRT_GEOGCS, OT_EPSG,  "Antigua 1943"},
	{4602,   SRT_GEOGCS, OT_EPSG,  "Dominica 1945"},
	{4603,   SRT_GEOGCS, OT_EPSG,  "Grenada 1953"},
	{4611,   SRT_GEOGCS, OT_EPSG,  "Hong Kong 1980"},
	{6326,   SRT_DATUM,  OT_EPSG,  "WGS_1984"},
	{6600,   SRT_DATUM,  OT_EPSG,  "Anguilla_1957"},
	{6601,   SRT_DATUM,  OT_EPSG,  "Antigua_1943"},
	{6602,   SRT_DATUM,  OT_EPSG,  "Dominica_1945"},
	{6603,   SRT_DATUM,  OT_EPSG,  "Grenada_1953"},
	{6611,   SRT_DATUM,  OT_EPSG,  "Hong_Kong_1980"},
	{7012,   SRT_SPHERO, OT_EPSG,  "Clarke 1880 (RGS)"},
	{7030,   SRT_SPHERO, OT_EPSG,  "WGS 84"},
	{8901,   SRT_PRIMEM, OT_EPSG,  "Greenwich"},
	{9001,   SRT_UNIT,   OT_EPSG,  "metre"},
	{9122,   SRT_UNIT,   OT_EPSG,  "degree"},
};

Math::CoordinateSystemManager::DatumInfo Math::CoordinateSystemManager::datumList[] = {
//	{srid, spheroid,"name", x0, y0, z0, cX, cY, cZ, xAngle, yAngle, zAngle, scale},
	{0,     0,     "AIRY_1830", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, Math::Unit::Angle::AU_RADIAN},
	{0,     0,     "Anguilla_1957", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, Math::Unit::Angle::AU_RADIAN},
	{0,     0,     "Antigua_1943", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, Math::Unit::Angle::AU_RADIAN},
	{0,     0,     "Beijing_1954", 0, 0, 0, 15.8, -154.4, -82.3, 0, 0, 0, 0, Math::Unit::Angle::AU_ARCSECOND},
	{0,     0,     "Dominica_1945", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, Math::Unit::Angle::AU_RADIAN},
	{0,     0,     "Grenada_1953", 0, 0, 0, 72, 213.7, 93, 0, 0, 0, 0, Math::Unit::Angle::AU_RADIAN},
	{0,     0,     "CGCS2000", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, Math::Unit::Angle::AU_RADIAN},
	{0,     0,     "HONGKONG", 0, 0, 0, -162.619, -276.959, -161.764, 0.067753 / 0.999998905754, -2.243648 / 0.999998905754, -1.158828 / 0.999998905754, -1.094246, Math::Unit::Angle::AU_ARCSECOND},
	{0,     0,     "Hong_Kong_1980", 0, 0, 0, -162.619, -276.959, -161.764, 0.067753 / 0.999998905754, -2.243648 / 0.999998905754, -1.158828 / 0.999998905754, -1.094246, Math::Unit::Angle::AU_ARCSECOND},
	{0,     0,     "Macau_2009", -2361554.788, 5417536.177, 2391608.926, -202.865, -303.990, -155.873, -34.079, 76.126, 32.660, 6.096, Math::Unit::Angle::AU_ARCSECOND},
	{0,     0,     "Montserrat_1958", 0, 0, 0, 174, 159, 365, 0, 0, 0, 0, Math::Unit::Angle::AU_ARCSECOND},
	{0,     0,     "St_Kitts_1955", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, Math::Unit::Angle::AU_RADIAN},
	{0,     0,     "St_Lucia_1955", 0, 0, 0, -149, 128, 296, 0, 0, 0, 0, Math::Unit::Angle::AU_ARCSECOND},
	{0,     0,     "St_Lucia_1955", 0, 0, 0, 195.671, 332.517, 274.607, 0, 0, 0, 0, Math::Unit::Angle::AU_ARCSECOND},
	{0,     0,     "Taiwan_1967", 0, 0, 0, -730.160, -346.212, -472.186, -7.968, -3.5498, -0.4063, -18.2, Math::Unit::Angle::AU_ARCSECOND},
	{0,     0,     "Taiwan_1997", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, Math::Unit::Angle::AU_RADIAN},
	{6326,  7030,  "WGS_1984", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, Math::Unit::Angle::AU_RADIAN},
};

Math::CoordinateSystemManager::GeographicCSysInfo Math::CoordinateSystemManager::csysList[] = {
//	{"srid","geoName", datumName, eet},
	{0,    "AIRY_1830", "AIRY_1830", Math::EarthEllipsoid::EET_AIRY1830},
	{0,    "AIRY_1830M", "AIRY_1830", Math::EarthEllipsoid::EET_AIRY1830M},
	{0,    "Anguilla 1957", "Anguilla_1957", Math::EarthEllipsoid::EET_CLARKE1880},
	{0,    "Antigua 1943", "Antigua_1943", Math::EarthEllipsoid::EET_CLARKE1880},
	{0,    "Beijing 1954", "Beijing_1954", Math::EarthEllipsoid::EET_KRASSOVSKY1940},
	{0,    "CGCS2000", "CGCS2000", Math::EarthEllipsoid::EET_INTL1924},
	{0,    "Dominica 1945", "Dominica_1945", Math::EarthEllipsoid::EET_CLARKE1880},
	{0,    "Grenada 1953", "Grenada_1953", Math::EarthEllipsoid::EET_CLARKE1880},
	{0,    "Hong Kong 1980", "Hong_Kong_1980", Math::EarthEllipsoid::EET_INTL1924},
	{0,    "HONGKONG", "D_HONGKONG", Math::EarthEllipsoid::EET_INTL1924},
	{0,    "Macau_2009", "D_Macau_2009", Math::EarthEllipsoid::EET_INTL1924},
	{0,    "Montserrat 1958", "Montserrat_1958", Math::EarthEllipsoid::EET_CLARKE1880},
	{0,    "St. Kitts 1955", "St_Kitts_1955", Math::EarthEllipsoid::EET_CLARKE1880},
	{0,    "St. Lucia 1955", "St_Lucia_1955", Math::EarthEllipsoid::EET_CLARKE1880},
	{0,    "St. Vincent 1945", "St_Vincent_1945", Math::EarthEllipsoid::EET_CLARKE1880},
	{0,    "Taiwan_1967", "Taiwan_1967", Math::EarthEllipsoid::EET_SAM1969},
	{0,    "Taiwan_1997", "Taiwan_1997", Math::EarthEllipsoid::EET_GRS80},
	{0,    "WGS_1984", "WGS_1984", Math::EarthEllipsoid::EET_WGS84},
	{4326, "WGS 84", "WGS_1984", Math::EarthEllipsoid::EET_WGS84}
};

Math::CoordinateSystemManager::ProjectedCSysInfo Math::CoordinateSystemManager::pcsysList[] = {
//	{srid, projName, falseEasting, falseNorthing, centralMeridian, latitudeOfOrigin, scaleFactor, geoName};
	{2000, Math::CoordinateSystem::CoordinateSystemType::MercatorProjected, "Anguilla 1957 / British West Indies Grid", 400000, 0, -62, 0, 0.9995, "Anguilla 1957"},
	{2001, Math::CoordinateSystem::CoordinateSystemType::MercatorProjected, "Antigua 1943 / British West Indies Grid", 400000, 0, -62, 0, 0.9995, "Antigua 1943"},
	{2435, Math::CoordinateSystem::CoordinateSystemType::MercatorProjected, "Beijing 1954 / 3-degree Gauss-Kruger CM 114E", 500000, 0, 114, 0, 1, "Beijing 1954"},
	{2002, Math::CoordinateSystem::CoordinateSystemType::MercatorProjected, "Dominica 1945 / British West Indies Grid", 400000, 0, -62, 0, 0.9995, "Dominica 1945"},
	{2003, Math::CoordinateSystem::CoordinateSystemType::MercatorProjected, "Grenada 1953 / British West Indies Grid", 400000, 0, -62, 0, 0.9995, "Grenada 1953"},
	{2326, Math::CoordinateSystem::CoordinateSystemType::MercatorProjected, "Hong Kong 1980 Grid System", 836694.05, 819069.80, 114.17855555555555555555555555556, 22.312133333333333333333333333333, 1, "Hong Kong 1980"},
	{0,    Math::CoordinateSystem::CoordinateSystemType::MercatorProjected, "Irish National Grid", 200000, 250000, -8, 53.5, 1.000035, "AIRY_1830M"},
	{0,    Math::CoordinateSystem::CoordinateSystemType::MercatorProjected, "Macau Grid", 20000.00, 20000.00, 113.53646944444444444444444444444, 22.21239722222222222222222222222222, 1, "Macau_2009"},
	{2004, Math::CoordinateSystem::CoordinateSystemType::MercatorProjected, "Montserrat 1958 / British West Indies Grid", 400000, 0, -62, 0, 0.9995, "Montserrat 1958"},
	{2005, Math::CoordinateSystem::CoordinateSystemType::MercatorProjected, "St. Kitts 1955 / British West Indies Grid", 400000, 0, -62, 0, 0.9995, "St. Kitts 1955"},
	{2006, Math::CoordinateSystem::CoordinateSystemType::MercatorProjected, "St. Lucia 1955 / British West Indies Grid", 400000, 0, -62, 0, 0.9995, "St. Lucia 1955"},
	{2006, Math::CoordinateSystem::CoordinateSystemType::MercatorProjected, "St. Vincent 45 / British West Indies Grid", 400000, 0, -62, 0, 0.9995, "St. Vincent 1945"},
	{0,    Math::CoordinateSystem::CoordinateSystemType::MercatorProjected, "Taiwan 1967 Grid", 250000.00, 0.00, 121.0, 0, 0.9999, "Taiwan_1967"},
	{0,    Math::CoordinateSystem::CoordinateSystemType::MercatorProjected, "Taiwan 1997 Grid", 250000.00, 0.00, 121.0, 0, 0.9999, "Taiwan_1997"},
	{0,    Math::CoordinateSystem::CoordinateSystemType::MercatorProjected, "UK National Grid", 400000, -100000, -2, 49, 0.9996012717, "AIRY_1830"},
	{3857, Math::CoordinateSystem::CoordinateSystemType::Mercator1SPProjected, "WGS 84 / Pseudo-Mercator", 0, 0, 0, 0, 1, "WGS 84"}
};

Math::CoordinateSystem *Math::CoordinateSystemManager::ParsePRJFile(const UTF8Char *fileName)
{
	UInt8 buff[512];
	UOSInt buffSize;
	IO::FileStream *fs;
	NEW_CLASS(fs, IO::FileStream(fileName, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Sequential));
	if (fs->IsError())
	{
		DEL_CLASS(fs);
		return 0;
	}
	buffSize = fs->Read(buff, 511);
	buff[buffSize] = 0;
	DEL_CLASS(fs);
	if (buffSize == 511)
		return 0;

	return ParsePRJBuff(fileName, (Char*)buff, &buffSize);
}

const Math::CoordinateSystemManager::SpatialRefInfo *Math::CoordinateSystemManager::SRGetSpatialRef(UInt32 epsgId)
{
	OSInt i = 0;
	OSInt j = (sizeof(srInfoList) / sizeof(srInfoList[0])) - 1;
	OSInt k;
	while (i <= j)
	{
		k = (i + j) >> 1;
		if (srInfoList[k].srid > epsgId)
		{
			j = k - 1;
		}
		else if (srInfoList[k].srid < epsgId)
		{
			i = k + 1;
		}
		else
		{
			return &srInfoList[k];
		}
	}
#if defined(VERBOSE)
	printf("SRGetSpatialRef(%d) not found\r\n", epsgId);
#endif
	return 0;
}

const Math::CoordinateSystemManager::SpatialRefInfo *Math::CoordinateSystemManager::SRGetSpatialRefPrev(UInt32 epsgId)
{
	OSInt i = 0;
	OSInt j = (sizeof(srInfoList) / sizeof(srInfoList[0])) - 1;
	OSInt k;
	while (i <= j)
	{
		k = (i + j) >> 1;
		if (srInfoList[k].srid > epsgId)
		{
			j = k - 1;
		}
		else if (srInfoList[k].srid < epsgId)
		{
			i = k + 1;
		}
		else
		{
			if (k > 0)
			{
				return &srInfoList[k - 1];
			}
			return 0;
		}
	}
#if defined(VERBOSE)
	printf("SRGetSpatialRefPrev(%d) not found\r\n", epsgId);
#endif
	return 0;
}

const Math::CoordinateSystemManager::SpatialRefInfo *Math::CoordinateSystemManager::SRGetSpatialRefNext(UInt32 epsgId)
{
	OSInt lastInd;
	OSInt i = 0;
	OSInt j = lastInd = (sizeof(srInfoList) / sizeof(srInfoList[0])) - 1;
	OSInt k;
	while (i <= j)
	{
		k = (i + j) >> 1;
		if (srInfoList[k].srid > epsgId)
		{
			j = k - 1;
		}
		else if (srInfoList[k].srid < epsgId)
		{
			i = k + 1;
		}
		else
		{
			if (k < lastInd)
			{
				return &srInfoList[k + 1];
			}
			return 0;
		}
	}
#if defined(VERBOSE)
	printf("SRGetSpatialRefNext(%d) not found\r\n", epsgId);
#endif
	return 0;
}

const Math::CoordinateSystemManager::SpheroidInfo *Math::CoordinateSystemManager::SRGetSpheroid(UInt32 epsgId)
{
	OSInt i = 0;
	OSInt j = (sizeof(spheroidSRID) / sizeof(spheroidSRID[0])) - 1;
	OSInt k;
	while (i <= j)
	{
		k = (i + j) >> 1;
		if (spheroidSRID[k].srid > epsgId)
		{
			j = k - 1;
		}
		else if (spheroidSRID[k].srid < epsgId)
		{
			i = k + 1;
		}
		else
		{
			return &spheroidSRID[k];
		}
	}
#if defined(VERBOSE)
	printf("SRGetSpheroid(%d) not found\r\n", epsgId);
#endif
	return 0;
}

const Math::CoordinateSystemManager::DatumInfo *Math::CoordinateSystemManager::SRGetDatum(UInt32 epsgId)
{
	OSInt i = 0;
	OSInt j = (sizeof(datumSRID) / sizeof(datumSRID[0])) - 1;
	OSInt k;
	while (i <= j)
	{
		k = (i + j) >> 1;
		if (datumSRID[k].srid > epsgId)
		{
			j = k - 1;
		}
		else if (datumSRID[k].srid < epsgId)
		{
			i = k + 1;
		}
		else
		{
			return &datumSRID[k];
		}
	}
#if defined(VERBOSE)
	printf("SRGetDatum(%d) not found\r\n", epsgId);
#endif
	return 0;
}

const Math::CoordinateSystemManager::GeogcsSRInfo *Math::CoordinateSystemManager::SRGetGeogcsInfo(UInt32 epsgId)
{
	OSInt i = 0;
	OSInt j = (sizeof(geogcsSRID) / sizeof(geogcsSRID[0])) - 1;
	OSInt k;
	while (i <= j)
	{
		k = (i + j) >> 1;
		if (geogcsSRID[k].srid > epsgId)
		{
			j = k - 1;
		}
		else if (geogcsSRID[k].srid < epsgId)
		{
			i = k + 1;
		}
		else
		{
			return &geogcsSRID[k];
		}
	}
#if defined(VERBOSE)
	printf("SRGetGeogcsInfo(%d) not found\r\n", epsgId);
#endif
	return 0;	
}

const Math::CoordinateSystemManager::ProjcsSRInfo *Math::CoordinateSystemManager::SRGetProjcsInfo(UInt32 epsgId)
{
	OSInt i = 0;
	OSInt j = (sizeof(projcsSRID) / sizeof(projcsSRID[0])) - 1;
	OSInt k;
	while (i <= j)
	{
		k = (i + j) >> 1;
		if (projcsSRID[k].srid > epsgId)
		{
			j = k - 1;
		}
		else if (projcsSRID[k].srid < epsgId)
		{
			i = k + 1;
		}
		else
		{
			return &projcsSRID[k];
		}
	}
#if defined(VERBOSE)
	printf("SRGetProjcsInfo(%d) not found\r\n", epsgId);
#endif
	return 0;	
}

Math::CoordinateSystem *Math::CoordinateSystemManager::SRCreateCSys(UInt32 epsgId)
{
	const Math::CoordinateSystemManager::SpatialRefInfo *info = SRGetSpatialRef(epsgId);
	if (info)
	{
		Math::CoordinateSystem *csys = 0;
		if (info->srType == SRT_PROJCS)
		{
			csys = SRCreateProjCSys(epsgId);
		}
		else if (info->srType == SRT_GEOGCS)
		{
			csys = SRCreateGeogCSys(epsgId);
		}
		return csys;
	}
	return 0;
}

Math::ProjectedCoordinateSystem *Math::CoordinateSystemManager::SRCreateProjCSys(UInt32 epsgId)
{
	const Math::CoordinateSystemManager::ProjcsSRInfo *projcs = SRGetProjcsInfo(epsgId);
	if (projcs == 0)
	{
		return 0;
	}
	Math::GeographicCoordinateSystem *gcsys = SRCreateGeogCSys(projcs->geogcsSRID);
	if (gcsys == 0)
	{
		return 0;
	}
	Math::ProjectedCoordinateSystem *csys;
	UTF8Char sbuff[32];
	Text::StrUInt32(Text::StrConcatC(sbuff, UTF8STRC("EPSG:")), epsgId);
	if (projcs->csysType == Math::CoordinateSystem::CoordinateSystemType::MercatorProjected || projcs->csysType == Math::CoordinateSystem::CoordinateSystemType::GausskrugerProjected)
	{
		NEW_CLASS(csys, Math::MercatorProjectedCoordinateSystem(sbuff, projcs->srid, (const UTF8Char*)projcs->projName, projcs->falseEasting, projcs->falseNorthing, projcs->centralMeridian, projcs->latitudeOfOrigin, projcs->scaleFactor, gcsys, projcs->unit));
		return csys;
	}
	else if (projcs->csysType == Math::CoordinateSystem::CoordinateSystemType::Mercator1SPProjected)
	{
		NEW_CLASS(csys, Math::Mercator1SPProjectedCoordinateSystem(sbuff, projcs->srid, (const UTF8Char*)projcs->projName, projcs->falseEasting, projcs->falseNorthing, projcs->centralMeridian, projcs->latitudeOfOrigin, projcs->scaleFactor, gcsys, projcs->unit));
		return csys;
	}
	DEL_CLASS(gcsys);
	return 0;
}

Math::GeographicCoordinateSystem *Math::CoordinateSystemManager::SRCreateGeogCSys(UInt32 epsgId)
{
	const Math::CoordinateSystemManager::GeogcsSRInfo *geogcs = SRGetGeogcsInfo(epsgId);
	if (geogcs == 0)
	{
		return 0;
	}
	const Math::CoordinateSystemManager::DatumInfo *datum = SRGetDatum(geogcs->datum);
	if (datum == 0)
	{
		return 0;
	}
	const Math::CoordinateSystemManager::SpheroidInfo *spheroid = SRGetSpheroid(datum->spheroid);
	if (spheroid == 0)
	{
		return 0;
	}
	UTF8Char sbuff[32];
	Math::GeographicCoordinateSystem *csys;
	Text::StrUInt32(Text::StrConcatC(sbuff, UTF8STRC("EPSG:")), epsgId);
	Math::EarthEllipsoid ellipsoid(spheroid->eet);
	Math::GeographicCoordinateSystem::DatumData1 data;
	FillDatumData(&data, datum, datum->datumName, &ellipsoid, spheroid);
	NEW_CLASS(csys, Math::GeographicCoordinateSystem(sbuff, epsgId, (const UTF8Char*)geogcs->name, &data, geogcs->primem, geogcs->unit));
	return csys;
}

Math::CoordinateSystem *Math::CoordinateSystemManager::CreateFromName(const UTF8Char *name)
{
	if (Text::StrStartsWith(name, (const UTF8Char*)"EPSG:"))
	{
		UInt32 epsgId = Text::StrToUInt32(&name[5]);
		return SRCreateCSys(epsgId);
	}
	else if (Text::StrStartsWith(name, (const UTF8Char*)"urn:ogc:def:crs:EPSG::"))
	{
		UInt32 epsgId = Text::StrToUInt32(&name[22]);
		return SRCreateCSys(epsgId);
	}
	else if (Text::StrEquals(name, (const UTF8Char*)"urn:ogc:def:crs:OGC:1.3:CRS84"))
	{
		return CreateGeogCoordinateSystem(name, Math::CoordinateSystemManager::GeoCoordSysTypeGetName(Math::CoordinateSystemManager::GCST_WGS84));
	}
	return 0;
}

Math::CoordinateSystem *Math::CoordinateSystemManager::ParsePRJBuff(const UTF8Char *sourceName, Char *prjBuff, UOSInt *parsedSize)
{
	UOSInt i;
	UOSInt j;
	UOSInt nameOfst;
	UOSInt datumOfst = 0;
	UOSInt spIndex = 0;
	Double a = 0;
	Double f_1 = 0;
	Math::CoordinateSystem *csys = 0;
	Math::EarthEllipsoid::EarthEllipsoidType eet;
	Math::GeographicCoordinateSystem *gcs = 0;
	Math::GeographicCoordinateSystem::PrimemType primem = Math::GeographicCoordinateSystem::PT_GREENWICH;
	Math::GeographicCoordinateSystem::UnitType unit = Math::GeographicCoordinateSystem::UT_DEGREE;
	Char c;
	UInt32 srid = 0;
	if (Text::StrStartsWith(prjBuff, "GEOGCS["))
	{
		i = 7;
		if (!ParsePRJString(&prjBuff[i], &j))
			return 0;
		nameOfst = i + 1;
		prjBuff[i + j - 1] = 0;
		i += j;
		while (true)
		{
			c = prjBuff[i];
			if (c == ']')
			{
				i++;
				break;
			}
			else if (c == ',')
			{
				i++;
				if (Text::StrStartsWith(&prjBuff[i], "DATUM["))
				{
					i += 6;
					if (!ParsePRJString(&prjBuff[i], &j))
						return 0;
					datumOfst = i + 1;
					prjBuff[i + j - 1] = 0;
					i += j;
					while (true)
					{
						c = prjBuff[i];
						if (c == ']')
						{
							break;
						}
						else if (c == 0)
						{
							return 0;
						}
						else if (c == ',')
						{
							i++;
							if (Text::StrStartsWith(&prjBuff[i], "SPHEROID["))
							{
								i += 9;
								if (!ParsePRJString(&prjBuff[i], &j))
									return 0;
								prjBuff[i + j - 1] = 0;
								i += j;
								j = (UOSInt)-1;
								spIndex = 1;
								while (true)
								{
									c = prjBuff[i];
									if (c == ']' || c == ',')
									{
										if ((OSInt)j >= 0)
										{
											prjBuff[i] = 0;
											if (spIndex == 1)
											{
												a = Text::StrToDouble(&prjBuff[j]);
											}
											else if (spIndex == 2)
											{
												f_1 = Text::StrToDouble(&prjBuff[j]);
											}
											spIndex++;
										}
										i++;
										if (c == ']')
											break;
										j = i;
									}
									else if (c == 0)
									{
										return 0;
									}
									else
									{
										i++;
									}
								}
							}
							else
							{
								return 0;
							}
						}
						else
						{
							return 0;
						}
					}
					i++;
				}
				else if (Text::StrStartsWith(&prjBuff[i], "PRIMEM["))
				{
					i += 7;
					if (!ParsePRJString(&prjBuff[i], &j))
						return 0;
					i += j;
					while (true)
					{
						c = prjBuff[i];
						if (c == ']')
							break;
						if (c == 0)
							return 0;
						if (c == '[')
							return 0;
						i++;
					}
					i++;
				}
				else if (Text::StrStartsWith(&prjBuff[i], "UNIT["))
				{
					i += 5;
					if (!ParsePRJString(&prjBuff[i], &j))
						return 0;
					i += j;
					while (true)
					{
						c = prjBuff[i];
						if (c == ']')
							break;
						if (c == 0)
							return 0;
						if (c == '[')
							return 0;
						i++;
					}
					i++;
				}
				else
				{
					return 0;
				}
			}
			else
			{
				return 0;
			}
		}
		if (spIndex != 3)
		{
			return 0;
		}
		if (parsedSize)
		{
			*parsedSize = i;
		}
		eet = Math::EarthEllipsoid::EET_OTHER;
		if (a == 6378137.0 && f_1 == 298.257223563)
		{
			eet = Math::EarthEllipsoid::EET_WGS84;
		}
		else if (a == 6378137.0 && f_1 == 298.257222101)
		{
			eet = Math::EarthEllipsoid::EET_GRS80;
		}
		else if (a == 6378206.4 && f_1 == 294.9786982)
		{
			eet = Math::EarthEllipsoid::EET_CLARKE1866;
		}
		else if (a == 6378137.0 && f_1 == 298.257222932867)
		{
			eet = Math::EarthEllipsoid::EET_WGS84_OGC;
		}
		else if (a == 6378388.0 && f_1 == 297.0)
		{
			eet = Math::EarthEllipsoid::EET_INTL1924;
		}
		else if (a == 6378388.0 && f_1 == 297.0000000000601)
		{
			eet = Math::EarthEllipsoid::EET_INTL1924;
		}
		if (eet != Math::EarthEllipsoid::EET_OTHER)
		{
			Math::EarthEllipsoid ellipsoid(eet);
			const Math::CoordinateSystemManager::DatumInfo *datum = GetDatumInfoByName((const UTF8Char*)&prjBuff[datumOfst]);

			Math::GeographicCoordinateSystem::DatumData1 data;
			FillDatumData(&data, datum, &prjBuff[datumOfst], &ellipsoid, 0);
			NEW_CLASS(csys, Math::GeographicCoordinateSystem(sourceName, srid, (const UTF8Char*)&prjBuff[nameOfst], &data, primem, unit));
			return csys;
		}
		else
		{
			Math::EarthEllipsoid ellipsoid(a, f_1, eet);
			const Math::CoordinateSystemManager::DatumInfo *datum = GetDatumInfoByName((const UTF8Char*)&prjBuff[datumOfst]);
			Math::GeographicCoordinateSystem::DatumData1 data;
			FillDatumData(&data, datum, &prjBuff[datumOfst], &ellipsoid, 0);
			NEW_CLASS(csys, Math::GeographicCoordinateSystem(sourceName, srid, (const UTF8Char*)&prjBuff[nameOfst], &data, primem, unit));
			return csys;
		}
	}
	else if (Text::StrStartsWith(prjBuff, "PROJCS["))
	{
		Math::CoordinateSystem::CoordinateSystemType cst = Math::CoordinateSystem::CoordinateSystemType::Geographic;
		Double falseEasting = -1;
		Double falseNorthing = -1;
		Double centralMeridian = -1;
		Double scaleFactor = -1;
		Double latitudeOfOrigin = -1;
		UOSInt nOfst;
		UOSInt vOfst;
		Bool commaFound;

		i = 7;
		if (!ParsePRJString(&prjBuff[i], &j))
			return 0;
		nameOfst = i + 1;
		prjBuff[i + j - 1] = 0;
		i += j;
		while (true)
		{
			c = prjBuff[i];
			if (c == ']')
			{
				i++;
				break;
			}
			else if (c == ',')
			{
				i++;
				if (Text::StrStartsWith(&prjBuff[i], "GEOGCS["))
				{
					gcs = (Math::GeographicCoordinateSystem *)ParsePRJBuff(sourceName, &prjBuff[i], &j);
					if (gcs == 0)
						return 0;
					i += j;
				}
				else if (Text::StrStartsWith(&prjBuff[i], "PROJECTION["))
				{
					if (Text::StrStartsWith(&prjBuff[i + 11], "\"Transverse_Mercator\"]"))
					{
						i += 33;
						cst = Math::CoordinateSystem::CoordinateSystemType::MercatorProjected;
					}
					else if (Text::StrStartsWith(&prjBuff[i + 11], "\"Mercator_1SP\"]"))
					{
						i += 26;
						cst = Math::CoordinateSystem::CoordinateSystemType::Mercator1SPProjected;
					}
					else if (Text::StrStartsWith(&prjBuff[i + 11], "\"Gauss_Kruger\"]"))
					{
						i += 26;
						cst = Math::CoordinateSystem::CoordinateSystemType::GausskrugerProjected;
					}
					else
					{
						SDEL_CLASS(gcs);
						return 0;
					}
				}
				else if (Text::StrStartsWith(&prjBuff[i], "PARAMETER["))
				{
					i += 10;
					if (!ParsePRJString(&prjBuff[i], &j))
					{
						SDEL_CLASS(gcs);
						return 0;
					}
					nOfst = i + 1;
					prjBuff[i + j - 1] = 0;
					i += j;
					if (prjBuff[i] != ',')
					{
						SDEL_CLASS(gcs);
						return 0;
					}
					vOfst = i + 1;
					i++;
					while (true)
					{
						c = prjBuff[i];
						if (c == 0 || c == ',')
						{
							SDEL_CLASS(gcs);
							return 0;
						}
						else if (c == ']')
						{
							prjBuff[i] = 0;
							i++;
							if (Text::StrEqualsICase(&prjBuff[nOfst], "False_Easting"))
							{
								falseEasting = Text::StrToDouble(&prjBuff[vOfst]);
							}
							else if (Text::StrEqualsICase(&prjBuff[nOfst], "False_Northing"))
							{
								falseNorthing = Text::StrToDouble(&prjBuff[vOfst]);
							}
							else if (Text::StrEqualsICase(&prjBuff[nOfst], "Central_Meridian"))
							{
								centralMeridian = Text::StrToDouble(&prjBuff[vOfst]);
							}
							else if (Text::StrEqualsICase(&prjBuff[nOfst], "Scale_Factor"))
							{
								scaleFactor = Text::StrToDouble(&prjBuff[vOfst]);
							}
							else if (Text::StrEqualsICase(&prjBuff[nOfst], "Latitude_Of_Origin"))
							{
								latitudeOfOrigin = Text::StrToDouble(&prjBuff[vOfst]);
							}
							else
							{
								SDEL_CLASS(gcs);
								return 0;
							}
							break;
						}
						else
						{
							i++;
						}
					}
				}
				else if (Text::StrStartsWith(&prjBuff[i], "UNIT["))
				{
					i += 5;
					if (!ParsePRJString(&prjBuff[i], &j))
					{
						SDEL_CLASS(gcs);
						return 0;
					}
					i += j;
					commaFound = false;
					while (true)
					{
						c = prjBuff[i];
						if (c == ',')
						{
							i++;
							if (commaFound)
							{
								SDEL_CLASS(gcs);
								return 0;
							}
							commaFound = true;
						}
						else if (c == ']')
						{
							i++;
							if (!commaFound)
							{
								SDEL_CLASS(gcs);
								return 0;
							}
							break;
						}
						else if (c == 0)
						{
							SDEL_CLASS(gcs);
							return 0;
						}
						else
						{
							i++;
						}
					}
				}
				else
				{
					SDEL_CLASS(gcs);
					return 0;
				}
			}
			else
			{
				SDEL_CLASS(gcs);
				return 0;
			}
		}
		if (cst == Math::CoordinateSystem::CoordinateSystemType::Geographic || falseEasting == -1 || falseNorthing == -1 || centralMeridian == -1 || scaleFactor == -1 || latitudeOfOrigin == -1 || gcs == 0)
		{
			SDEL_CLASS(gcs);
			return 0;
		}
		if (parsedSize)
		{
			*parsedSize = i;
		}
		if (cst == Math::CoordinateSystem::CoordinateSystemType::MercatorProjected || cst == Math::CoordinateSystem::CoordinateSystemType::GausskrugerProjected)
		{
			NEW_CLASS(csys, Math::MercatorProjectedCoordinateSystem(sourceName, srid, (const UTF8Char*)&prjBuff[nameOfst], falseEasting, falseNorthing, centralMeridian, latitudeOfOrigin, scaleFactor, gcs, unit));
			return csys;
		}
		else if (cst == Math::CoordinateSystem::CoordinateSystemType::Mercator1SPProjected)
		{
			NEW_CLASS(csys, Math::Mercator1SPProjectedCoordinateSystem(sourceName, srid, (const UTF8Char*)&prjBuff[nameOfst], falseEasting, falseNorthing, centralMeridian, latitudeOfOrigin, scaleFactor, gcs, unit));
			return csys;
		}
		else
		{
			SDEL_CLASS(gcs);
			return 0;
		}
	}

	return 0;
}

Bool Math::CoordinateSystemManager::ParsePRJString(Char *prjBuff, UOSInt *strSize)
{
	UOSInt i;
	Char c;
	if (prjBuff[0] != '\"')
		return false;
	i = 1;
	while (true)
	{
		c = prjBuff[i];
		if (c == 0)
			return false;
		if (c == '\"')
		{
			i++;
			*strSize = i;
			return true;
		}
		i++;
	}
}

const Math::CoordinateSystemManager::DatumInfo *Math::CoordinateSystemManager::GetDatumInfoByName(const UTF8Char *name)
{
	if (name[0] == 'D' && name[1] == '_')
	{
		name = &name[2];
	}
	OSInt i = 0;
	OSInt j = (sizeof(datumList) / sizeof(datumList[0])) - 1;
	OSInt k;
	OSInt l;
	while (i <= j)
	{
		k = (i + j) >> 1;
		l = Text::StrCompareICase(datumList[k].datumName, (const Char*)name);
		if (l > 0)
		{
			j = k - 1;
		}
		else if (l < 0)
		{
			i = k + 1;
		}
		else
		{
			return &datumList[k];
		}
	}
	return 0;
}

void Math::CoordinateSystemManager::FillDatumData(Math::GeographicCoordinateSystem::DatumData1 *data, const Math::CoordinateSystemManager::DatumInfo *datum, const Char *name, Math::EarthEllipsoid *ee, const SpheroidInfo *spheroid)
{
	if (datum)
	{
		data->srid = datum->srid;
		data->spheroid.ellipsoid = ee;
		data->spheroid.srid = datum->spheroid;
		if (spheroid)
		{
			data->spheroid.name = spheroid->name;
		}
		else
		{
			data->spheroid.name = datum->datumName;
		}
		data->name = datum->datumName;
		data->x0 = datum->x0;
		data->y0 = datum->y0;
		data->z0 = datum->z0;
		data->cX = datum->cX;
		data->cY = datum->cY;
		data->cZ = datum->cZ;
		data->xAngle = datum->xAngle;
		data->yAngle = datum->yAngle;
		data->zAngle = datum->zAngle;
		data->scale = datum->scale;
		data->aunit = datum->aunit;
	}
	else
	{
		data->srid = 0;
		data->spheroid.ellipsoid = ee;
		data->spheroid.srid = 0;
		data->spheroid.name = name;
		data->name = name;
		data->x0 = 0;
		data->y0 = 0;
		data->z0 = 0;
		data->cX = 0;
		data->cY = 0;
		data->cZ = 0;
		data->xAngle = 0;
		data->yAngle = 0;
		data->zAngle = 0;
		data->scale = 0;
		data->aunit = Math::Unit::Angle::AU_RADIAN;
	}
}

Math::ProjectedCoordinateSystem *Math::CoordinateSystemManager::CreateProjCoordinateSystemDefName(Math::CoordinateSystemManager::ProjCoordSysType pcst)
{
	const UTF8Char *name = Math::CoordinateSystemManager::ProjCoordSysTypeGetName(pcst);
	if (name == 0)
		return 0;
	return CreateProjCoordinateSystem(name, name);
}

Math::ProjectedCoordinateSystem *Math::CoordinateSystemManager::CreateProjCoordinateSystem(const UTF8Char *sourceName, const UTF8Char *projName)
{
	const Math::CoordinateSystemManager::ProjectedCSysInfo *coord = GetProjCoordinateSystemInfo(projName);
	Math::GeographicCoordinateSystem *gcs;
	Math::ProjectedCoordinateSystem *csys = 0;
	if (coord == 0)
	{
		return 0;
	}
	gcs = Math::CoordinateSystemManager::CreateGeogCoordinateSystem(sourceName, (const UTF8Char*)coord->geoName);
	if (gcs == 0)
	{
		return 0;
	}
	if (coord->csysType == Math::CoordinateSystem::CoordinateSystemType::MercatorProjected)
	{
		NEW_CLASS(csys, Math::MercatorProjectedCoordinateSystem(sourceName, coord->srid, (const UTF8Char*)coord->projName, coord->falseEasting, coord->falseNorthing, coord->centralMeridian, coord->latitudeOfOrigin, coord->scaleFactor, gcs, Math::CoordinateSystem::UT_METRE));
	}
	else if (coord->csysType == Math::CoordinateSystem::CoordinateSystemType::Mercator1SPProjected)
	{
		NEW_CLASS(csys, Math::Mercator1SPProjectedCoordinateSystem(sourceName, coord->srid, (const UTF8Char*)coord->projName, coord->falseEasting, coord->falseNorthing, coord->centralMeridian, coord->latitudeOfOrigin, coord->scaleFactor, gcs, Math::CoordinateSystem::UT_METRE));
	}
	return csys;
}

UOSInt Math::CoordinateSystemManager::GetProjCoordinateSystems(Data::ArrayList<ProjCoordSysType> *csysList)
{
	UOSInt initCnt = csysList->GetCount();
	Math::CoordinateSystemManager::ProjCoordSysType pcst = Math::CoordinateSystemManager::PCST_FIRST;
	while (pcst <= Math::CoordinateSystemManager::PCST_LAST)
	{
		csysList->Add(pcst);
		pcst = (Math::CoordinateSystemManager::ProjCoordSysType)(pcst + 1);
	}
	return csysList->GetCount() - initCnt;
}

UOSInt Math::CoordinateSystemManager::GetProjCoordinateSystemNames(Data::ArrayList<const UTF8Char *> *nameList)
{
	UOSInt i = 0;
	UOSInt j = (sizeof(pcsysList) / sizeof(pcsysList[0]));
	while (i < j)
	{
		nameList->Add((const UTF8Char*)pcsysList[i].projName);
		i++;
	}
	return j;
}

const Math::CoordinateSystemManager::ProjectedCSysInfo *Math::CoordinateSystemManager::GetProjCoordinateSystemInfo(const UTF8Char *name)
{
	OSInt i = 0;
	OSInt j = (sizeof(pcsysList) / sizeof(pcsysList[0])) - 1;
	OSInt k;
	OSInt l;
	while (i <= j)
	{
		k = (i + j) >> 1;
		l = Text::StrCompareICase(pcsysList[k].projName, (const Char*)name);
		if (l > 0)
		{
			j = k - 1;
		}
		else if (l < 0)
		{
			i = k + 1;
		}
		else
		{
			return &pcsysList[k];
		}
	}
	return 0;
}


Math::GeographicCoordinateSystem *Math::CoordinateSystemManager::CreateGeogCoordinateSystemDefName(GeoCoordSysType gcst)
{
	const UTF8Char *name = Math::CoordinateSystemManager::GeoCoordSysTypeGetName(gcst);
	if (name == 0)
		return 0;
	return CreateGeogCoordinateSystem(name, name);
}

Math::GeographicCoordinateSystem *Math::CoordinateSystemManager::CreateGeogCoordinateSystem(const UTF8Char *sourceName, const UTF8Char *geoName)
{
	const Math::CoordinateSystemManager::GeographicCSysInfo *coord = GetGeogCoordinateSystemInfo(geoName);
	Math::GeographicCoordinateSystem *csys;
	if (coord == 0)
	{
		return 0;
	}
	const Math::CoordinateSystemManager::DatumInfo *datum = GetDatumInfoByName((const UTF8Char*)coord->datumName);
	if (datum == 0)
	{
		return 0;
	}
	Math::EarthEllipsoid ellipsoid(coord->eet);
	Math::GeographicCoordinateSystem::DatumData1 data;
	FillDatumData(&data, datum, 0, &ellipsoid, SRGetSpheroid(datum->spheroid));
	NEW_CLASS(csys, Math::GeographicCoordinateSystem(sourceName, coord->srid, (const UTF8Char*)coord->geoName, &data, Math::GeographicCoordinateSystem::PT_GREENWICH, Math::GeographicCoordinateSystem::UT_DEGREE));
	return csys;
}

UOSInt Math::CoordinateSystemManager::GetGeogCoordinateSystems(Data::ArrayList<GeoCoordSysType> *csysList)
{
	UOSInt initCnt = csysList->GetCount();
	csysList->Add(GCST_WGS84);
	return csysList->GetCount() - initCnt;
}

const Math::CoordinateSystemManager::GeographicCSysInfo *Math::CoordinateSystemManager::GetGeogCoordinateSystemInfo(const UTF8Char *name)
{
	OSInt i = 0;
	OSInt j = (sizeof(csysList) / sizeof(csysList[0])) - 1;
	OSInt k;
	OSInt l;
	while (i <= j)
	{
		k = (i + j) >> 1;
		l = Text::StrCompareICase(csysList[k].geoName, (const Char*)name);
		if (l > 0)
		{
			j = k - 1;
		}
		else if (l < 0)
		{
			i = k + 1;
		}
		else
		{
			return &csysList[k];
		}
	}
	return 0;
}

const UTF8Char *Math::CoordinateSystemManager::GeoCoordSysTypeGetName(GeoCoordSysType gcst)
{
	switch (gcst)
	{
	case Math::CoordinateSystemManager::GCST_CGCS2000:
		return (const UTF8Char*)"CGCS2000";
	case Math::CoordinateSystemManager::GCST_MACAU2009:
		return (const UTF8Char*)"Macau_2009";
	case Math::CoordinateSystemManager::GCST_HK1980:
		return (const UTF8Char*)"HONGKONG";
	case Math::CoordinateSystemManager::GCST_WGS84:
	default:
		return (const UTF8Char*)"WGS_1984";
	}
}

const UTF8Char *Math::CoordinateSystemManager::ProjCoordSysTypeGetName(ProjCoordSysType pcst)
{
	switch (pcst)
	{
	case PCST_HK80:
		return (const UTF8Char*)"Hong Kong 1980 Grid System";
	case PCST_UK_NATIONAL_GRID:
		return (const UTF8Char*)"UK National Grid";
	case PCST_IRISH_NATIONAL_GRID:
		return (const UTF8Char*)"Irish National Grid";
	case PCST_MACAU_GRID:
		return (const UTF8Char*)"Macau Grid";
	case PCST_TWD67:
		return (const UTF8Char*)"Taiwan 1967 Grid";
	case PCST_TWD97:
		return (const UTF8Char*)"Taiwan 1997 Grid";
	}
	return 0;
}
