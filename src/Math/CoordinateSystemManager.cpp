#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/FileStream.h"
#include "Math/CassiniSoldnerCoordinateSystem.h"
#include "Math/CoordinateSystemManager.h"
#include "Math/GeographicCoordinateSystem.h"
#include "Math/Math_C.h"
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
	{7001, Math::EarthEllipsoid::EET_AIRY1830,       UTF8STRC("Airy 1830")},
	{7002, Math::EarthEllipsoid::EET_AIRYM1849,      UTF8STRC("Airy Modified 1849")},
	{7003, Math::EarthEllipsoid::EET_AUSTRALIAN1966, UTF8STRC("Australian National Spheroid")},
	{7004, Math::EarthEllipsoid::EET_BESSEL1841,     UTF8STRC("Bessel 1841")},
	{7005, Math::EarthEllipsoid::EET_BESSELMOD,      UTF8STRC("Bessel Modified")},
	{7006, Math::EarthEllipsoid::EET_BESSELNAMIBIA,  UTF8STRC("Bessel Namibia")},
	{7007, Math::EarthEllipsoid::EET_CLARKE1858,     UTF8STRC("Clarke 1858")},
	{7008, Math::EarthEllipsoid::EET_CLARKE1866,     UTF8STRC("Clarke 1866")},
	{7009, Math::EarthEllipsoid::EET_CLARKE1866M,    UTF8STRC("Clarke 1866 Michigan")},
	{7010, Math::EarthEllipsoid::EET_CLARKE1880B,    UTF8STRC("Clarke 1880 (Benoit)")},
	{7011, Math::EarthEllipsoid::EET_CLARKE1880I,    UTF8STRC("Clarke 1880 (IGN)")},
	{7012, Math::EarthEllipsoid::EET_CLARKE1880R,    UTF8STRC("Clarke 1880 (RGS)")},
	{7013, Math::EarthEllipsoid::EET_CLARKE1880A,    UTF8STRC("Clarke 1880 (Arc)")},
	{7014, Math::EarthEllipsoid::EET_CLARKE1880S,    UTF8STRC("Clarke 1880 (SGA 1922)")},
	{7015, Math::EarthEllipsoid::EET_EVEREST1830A,   UTF8STRC("Everest 1830 (1937 Adjustment)")},
	{7016, Math::EarthEllipsoid::EET_EVEREST1830N,   UTF8STRC("Everest 1830 (1967 Definition)")},
	{7018, Math::EarthEllipsoid::EET_EVEREST1830M,   UTF8STRC("Everest 1830 Modified")},
	{7019, Math::EarthEllipsoid::EET_GRS80,          UTF8STRC("GRS 1980")},
	{7022, Math::EarthEllipsoid::EET_INTL1924,       UTF8STRC("International 1924")},
	{7030, Math::EarthEllipsoid::EET_WGS84,          UTF8STRC("WGS 84")},
	{7050, Math::EarthEllipsoid::EET_SAM1969,        UTF8STRC("GRS 1967 Modified")},
};

Math::CoordinateSystemManager::DatumInfo Math::CoordinateSystemManager::datumSRID[] = {
//	{srid, spheroid,"name", x0, y0, z0, cX, cY, cZ, xAngle, yAngle, zAngle, scale},
	{1025,  7050,  UTF8STRC("Taiwan_Datum_1967"), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, Math::Unit::Angle::AU_RADIAN},
	{1026,  7019,  UTF8STRC("Taiwan_Datum_1997"), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, Math::Unit::Angle::AU_RADIAN},
	{1207,  7022,  UTF8STRC("Macao_1920"), -2361554.788, 5417536.177, 2391608.926, -202.865, -303.990, -155.873, -34.079, 76.126, 32.660, 6.096, Math::Unit::Angle::AU_ARCSECOND},
	{6236,  7022,  UTF8STRC("Hu_Tzu_Shan_1950"), 0, 0, 0, -637, -549, -203, 0, 0, 0, 0, Math::Unit::Angle::AU_RADIAN},
	{6326,  7030,  UTF8STRC("WGS_1984"), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, Math::Unit::Angle::AU_RADIAN},
	{6600,  7012,  UTF8STRC("Anguilla_1957"), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, Math::Unit::Angle::AU_RADIAN},
	{6601,  7012,  UTF8STRC("Antigua_1943"), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, Math::Unit::Angle::AU_RADIAN},
	{6602,  7012,  UTF8STRC("Dominica_1945"), 0, 0, 0, 725, 685, 536, 0, 0, 0, 0, Math::Unit::Angle::AU_ARCSECOND},
	{6603,  7012,  UTF8STRC("Grenada_1953"), 0, 0, 0, 72, 213.7, 93, 0, 0, 0, 0, Math::Unit::Angle::AU_ARCSECOND},
	{6611,  7022,  UTF8STRC("Hong_Kong_1980"), 0, 0, 0, -162.619, -276.959, -161.764, 0.067753, -2.24365, -1.15883, -1.09425, Math::Unit::Angle::AU_ARCSECOND},
	{6738,  7007,  UTF8STRC("Hong_Kong_1963"), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, Math::Unit::Angle::AU_RADIAN},
};

Math::CoordinateSystemManager::GeogcsSRInfo Math::CoordinateSystemManager::geogcsSRID[] = {
//	{srid,  datum, name,    primem,   unit},
	{3821,  1025,  UTF8STRC("TWD67"), Math::CoordinateSystem::PT_GREENWICH, Math::CoordinateSystem::UT_DEGREE, 0.0174532925199433, true},
	{3824,  1026,  UTF8STRC("TWD97"), Math::CoordinateSystem::PT_GREENWICH, Math::CoordinateSystem::UT_DEGREE, 0.0174532925199433, true},
	{4236,  6236,  UTF8STRC("Hu Tzu Shan 1950"), Math::CoordinateSystem::PT_GREENWICH, Math::CoordinateSystem::UT_DEGREE, 0.0174532925199433, true},
	{4326,  6326,  UTF8STRC("WGS 84"), Math::CoordinateSystem::PT_GREENWICH, Math::CoordinateSystem::UT_DEGREE, 0.0174532925199433, true},
	{4600,  6600,  UTF8STRC("Anguilla 1957"), Math::CoordinateSystem::PT_GREENWICH, Math::CoordinateSystem::UT_DEGREE, 0.0174532925199433, true},
	{4601,  6601,  UTF8STRC("Antigua 1943"), Math::CoordinateSystem::PT_GREENWICH, Math::CoordinateSystem::UT_DEGREE, 0.0174532925199433, true},
	{4602,  6602,  UTF8STRC("Dominica 1945"), Math::CoordinateSystem::PT_GREENWICH, Math::CoordinateSystem::UT_DEGREE, 0.0174532925199433, true},
	{4603,  6603,  UTF8STRC("Grenada 1953"), Math::CoordinateSystem::PT_GREENWICH, Math::CoordinateSystem::UT_DEGREE, 0.0174532925199433, true},
	{4611,  6611,  UTF8STRC("Hong Kong 1980"), Math::CoordinateSystem::PT_GREENWICH, Math::CoordinateSystem::UT_DEGREE, 0.0174532925199433, true},
	{4738,  6738,  UTF8STRC("Hong Kong 1963"), Math::CoordinateSystem::PT_GREENWICH, Math::CoordinateSystem::UT_DEGREE, 0.0174532925199433, true},
	{8428,  1207,  UTF8STRC("Macau_2009"), Math::CoordinateSystem::PT_GREENWICH, Math::CoordinateSystem::UT_DEGREE, 0.0174532925199433, true},
};

Math::CoordinateSystemManager::ProjcsSRInfo Math::CoordinateSystemManager::projcsSRID[] = {
//	{srid,   geogcsSRID, csysType, projName, falseEasting, falseNorthing, centralMeridian, latitudeOfOrigin, scaleFactor, unit};
	{2000,   4600, Math::CoordinateSystem::CoordinateSystemType::MercatorProjected, UTF8STRC("Anguilla 1957 / British West Indies Grid"), 400000, 0, -62, 0, 0.9995, Math::CoordinateSystem::UT_METRE},
	{2001,   4601, Math::CoordinateSystem::CoordinateSystemType::MercatorProjected, UTF8STRC("Antigua 1943 / British West Indies Grid"), 400000, 0, -62, 0, 0.9995, Math::CoordinateSystem::UT_METRE},
	{2002,   4602, Math::CoordinateSystem::CoordinateSystemType::MercatorProjected, UTF8STRC("Dominica 1945 / British West Indies Grid"), 400000, 0, -62, 0, 0.9995, Math::CoordinateSystem::UT_METRE},
	{2003,   4603, Math::CoordinateSystem::CoordinateSystemType::MercatorProjected, UTF8STRC("Grenada 1953 / British West Indies Grid"), 400000, 0, -62, 0, 0.9995, Math::CoordinateSystem::UT_METRE},
	{2326,   4611, Math::CoordinateSystem::CoordinateSystemType::MercatorProjected, UTF8STRC("Hong Kong 1980 Grid System"), 836694.05, 819069.80, 114.17855555555555555555555555556, 22.312133333333333333333333333333, 1, Math::CoordinateSystem::UT_METRE},
	{3407,   4738, Math::CoordinateSystem::CoordinateSystemType::CassiniSoldner,    UTF8STRC("Hong Kong 1963 Grid System"), 132033.92, 62565.96, 114.178555555556, 22.3121333333333, 1, Math::CoordinateSystem::UT_CLARKE_FOOT},
	{3825,   3824, Math::CoordinateSystem::CoordinateSystemType::MercatorProjected, UTF8STRC("TWD97 / TM2 zone 119"), 250000.00, 0.00, 119.0, 0, 0.9999, Math::CoordinateSystem::UT_METRE},
	{3826,   3824, Math::CoordinateSystem::CoordinateSystemType::MercatorProjected, UTF8STRC("TWD97 / TM2 zone 121"), 250000.00, 0.00, 121.0, 0, 0.9999, Math::CoordinateSystem::UT_METRE},
	{3827,   3821, Math::CoordinateSystem::CoordinateSystemType::MercatorProjected, UTF8STRC("TWD67 / TM2 zone 119"), 250000.00, 0.00, 121.0, 0, 0.9999, Math::CoordinateSystem::UT_METRE},
	{3828,   3821, Math::CoordinateSystem::CoordinateSystemType::MercatorProjected, UTF8STRC("TWD67 / TM2 zone 121"), 250000.00, 0.00, 121.0, 0, 0.9999, Math::CoordinateSystem::UT_METRE},
	{3857,   4326, Math::CoordinateSystem::CoordinateSystemType::Mercator1SPProjected, UTF8STRC("WGS 84 / Pseudo-Mercator"), 0, 0, 0, 0, 1, Math::CoordinateSystem::UT_METRE},
	{8432,   8428, Math::CoordinateSystem::CoordinateSystemType::MercatorProjected, UTF8STRC("Macau Grid"), 20000.00, 20000.00, 113.53646944444444444444444444444, 22.21239722222222222222222222222222, 1, Math::CoordinateSystem::UT_METRE},
	{102100, 4326, Math::CoordinateSystem::CoordinateSystemType::Mercator1SPProjected, UTF8STRC("WGS 84 / Pseudo-Mercator"), 0, 0, 0, 0, 1, Math::CoordinateSystem::UT_METRE},
	{102140, 4611, Math::CoordinateSystem::CoordinateSystemType::MercatorProjected, UTF8STRC("Hong Kong 1980 Grid System"), 836694.05, 819069.80, 114.17855555555555555555555555556, 22.312133333333333333333333333333, 1, Math::CoordinateSystem::UT_METRE},
	{900913, 4326, Math::CoordinateSystem::CoordinateSystemType::Mercator1SPProjected, UTF8STRC("Google_Maps_Global_Mercator"), 0, 0, 0, 0, 1, Math::CoordinateSystem::UT_METRE},
};

Math::CoordinateSystemManager::SpatialRefInfo Math::CoordinateSystemManager::srInfoList[] = {
//	{id,     csysType, projName};
	{1025,   SRT_DATUM,  OT_EPSG,  "Taiwan_Datum_1967"},
	{1026,   SRT_DATUM,  OT_EPSG,  "Taiwan_Datum_1997"},
	{2000,   SRT_PROJCS, OT_EPSG,  "Anguilla 1957 / British West Indies Grid"},
	{2001,   SRT_PROJCS, OT_EPSG,  "Antigua 1943 / British West Indies Grid"},
	{2002,   SRT_PROJCS, OT_EPSG,  "Dominica 1945 / British West Indies Grid"},
	{2003,   SRT_PROJCS, OT_EPSG,  "Grenada 1953 / British West IndMath::CoordinateSystem::CoordinateSystemType::GEOGRAPHICruger CM 114E"},
	{2326,   SRT_PROJCS, OT_EPSG,  "Hong Kong 1980 Grid System", 113.76, 22.13, 114.51, 22.58},
	{3407,   SRT_PROJCS, OT_EPSG,  "Hong Kong 1963 Grid System", -9642.96, -3600.1, 244224.61, 160078.49},
	{3821,   SRT_GEOGCS, OT_EPSG,  "TWD67", 114.32, 17.36, 123.61, 26.96},
	{3824,   SRT_GEOGCS, OT_EPSG,  "TWD97", 114.32, 17.36, 123.61, 26.96},
	{3825,   SRT_PROJCS, OT_EPSG,  "TWD97 / TM2 zone 119", 114.32, 17.36, 123.61, 26.96},
	{3826,   SRT_PROJCS, OT_EPSG,  "TWD97 / TM2 zone 121", 114.32, 17.36, 123.61, 26.96},
	{3827,   SRT_PROJCS, OT_EPSG,  "TWD67 / TM2 zone 119", 119.25, 23.13, 119.78, 23.82},
	{3828,   SRT_PROJCS, OT_EPSG,  "TWD67 / TM2 zone 121", 119.99, 21.87, 122.06, 25.34},
	{3857,   SRT_PROJCS, OT_EPSG,  "WGS 84 / Pseudo-Mercator", -180.0, -85.05113, -180.0, -85.05113},
	{4236,   SRT_GEOGCS, OT_EPSG,  "Hu Tzu Shan 1950", 119.25, 21.87, 122.06, 25.34},
	{4326,   SRT_GEOGCS, OT_EPSG,  "WGS 84", -180.0, -90.0, 180.0, 90.0},
	{4600,   SRT_GEOGCS, OT_EPSG,  "Anguilla 1957", -63.22, 18.11, -62.92, 18.33},
	{4601,   SRT_GEOGCS, OT_EPSG,  "Antigua 1943", -61.95, 16.94, -61.61, 17.22},
	{4602,   SRT_GEOGCS, OT_EPSG,  "Dominica 1945", -61.55, 15.14, -61.2, 15.69},
	{4603,   SRT_GEOGCS, OT_EPSG,  "Grenada 1953", -61.84, 11.94, -61.54, 12.29},
	{4611,   SRT_GEOGCS, OT_EPSG,  "Hong Kong 1980", 113.76, 22.13, 114.51, 22.58},
	{4738,   SRT_GEOGCS, OT_EPSG,  "Hong Kong 1963", 113.76, 22.13, 114.51, 22.58},
	{6236,   SRT_DATUM,  OT_EPSG,  "Hu_Tzu_Shan_1950", 119.25, 21.87, 122.06, 25.34},
	{6326,   SRT_DATUM,  OT_EPSG,  "WGS_1984"},
	{6600,   SRT_DATUM,  OT_EPSG,  "Anguilla_1957"},
	{6601,   SRT_DATUM,  OT_EPSG,  "Antigua_1943"},
	{6602,   SRT_DATUM,  OT_EPSG,  "Dominica_1945"},
	{6603,   SRT_DATUM,  OT_EPSG,  "Grenada_1953"},
	{6611,   SRT_DATUM,  OT_EPSG,  "Hong_Kong_1980"},
	{6738,   SRT_DATUM,  OT_EPSG,  "Hong_Kong_1963"},
	{7007,   SRT_SPHERO, OT_EPSG,  "Clarke 1858"},
	{7012,   SRT_SPHERO, OT_EPSG,  "Clarke 1880 (RGS)"},
	{7019,   SRT_SPHERO, OT_EPSG,  "GRS 1980"},
	{7022,   SRT_SPHERO, OT_EPSG,  "International 1924"},
	{7030,   SRT_SPHERO, OT_EPSG,  "WGS 84"},
	{7050,   SRT_SPHERO, OT_EPSG,  "GRS 1967 Modified"},
	{8428,   SRT_GEOGCS, OT_EPSG,  "Macau_2009"},
	{8432,   SRT_PROJCS, OT_EPSG,  "Macau Grid", 113.52, 22.06, 113.68, 22.23},
	{8901,   SRT_PRIMEM, OT_EPSG,  "Greenwich"},
	{9001,   SRT_UNIT,   OT_EPSG,  "metre"},
	{9005,   SRT_UNIT,   OT_EPSG,  "Clarke's foot"},
	{9122,   SRT_UNIT,   OT_EPSG,  "degree"},
	{102100, SRT_PROJCS, OT_EPSG,  "WGS 84 / Pseudo-Mercator", -180.0, -85.05113, -180.0, -85.05113},
	{102140, SRT_PROJCS, OT_EPSG,  "Hong Kong 1980 Grid System", 113.76, 22.13, 114.51, 22.58},
	{900913, SRT_PROJCS, OT_EPSG,  "Google Maps Global Mercator", -180.0, -85.05113, -180.0, -85.05113},
};

Math::CoordinateSystemManager::DatumInfo Math::CoordinateSystemManager::datumList[] = {
//	{srid, spheroid,"name", x0, y0, z0, cX, cY, cZ, xAngle, yAngle, zAngle, scale},
	{0,     0,     UTF8STRC("AIRY_1830"), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, Math::Unit::Angle::AU_RADIAN},
	{6600,  7012,  UTF8STRC("Anguilla_1957"), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, Math::Unit::Angle::AU_RADIAN},
	{6601,  7012,  UTF8STRC("Antigua_1943"), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, Math::Unit::Angle::AU_RADIAN},
	{0,     0,     UTF8STRC("Beijing_1954"), 0, 0, 0, 15.8, -154.4, -82.3, 0, 0, 0, 0, Math::Unit::Angle::AU_ARCSECOND},
	{6602,  7012,  UTF8STRC("Dominica_1945"), 0, 0, 0, 725, 685, 536, 0, 0, 0, 0, Math::Unit::Angle::AU_ARCSECOND},
	{6603,  7012,  UTF8STRC("Grenada_1953"), 0, 0, 0, 72, 213.7, 93, 0, 0, 0, 0, Math::Unit::Angle::AU_ARCSECOND},
	{0,     0,     UTF8STRC("CGCS2000"), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, Math::Unit::Angle::AU_RADIAN},
	{0,     0,     UTF8STRC("HONGKONG"), 0, 0, 0, -162.619, -276.959, -161.764, 0.067753 / 0.999998905754, -2.243648 / 0.999998905754, -1.158828 / 0.999998905754, -1.094246, Math::Unit::Angle::AU_ARCSECOND},
	{6611,  7022,  UTF8STRC("Hong_Kong_1980"), 0, 0, 0, -162.619, -276.959, -161.764, 0.067753 / 0.999998905754, -2.243648 / 0.999998905754, -1.158828 / 0.999998905754, -1.094246, Math::Unit::Angle::AU_ARCSECOND},
	{6236,  7022,  UTF8STRC("Hu_Tzu_Shan_1950"), 637, -549, -203, 0, 0, 0, 0, Math::Unit::Angle::AU_ARCSECOND},
	{1207,  7022,  UTF8STRC("Macau_2009"), -2361554.788, 5417536.177, 2391608.926, -202.865, -303.990, -155.873, -34.079, 76.126, 32.660, 6.096, Math::Unit::Angle::AU_ARCSECOND},
	{0,     0,     UTF8STRC("Montserrat_1958"), 0, 0, 0, 174, 159, 365, 0, 0, 0, 0, Math::Unit::Angle::AU_ARCSECOND},
	{0,     0,     UTF8STRC("St_Kitts_1955"), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, Math::Unit::Angle::AU_RADIAN},
	{0,     0,     UTF8STRC("St_Lucia_1955"), 0, 0, 0, -149, 128, 296, 0, 0, 0, 0, Math::Unit::Angle::AU_ARCSECOND},
	{0,     0,     UTF8STRC("St_Lucia_1955"), 0, 0, 0, 195.671, 332.517, 274.607, 0, 0, 0, 0, Math::Unit::Angle::AU_ARCSECOND},
	{1025,  7050,  UTF8STRC("Taiwan_1967"), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, Math::Unit::Angle::AU_ARCSECOND},
	{1026,  7019,  UTF8STRC("Taiwan_1997"), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, Math::Unit::Angle::AU_RADIAN},
	{6326,  7030,  UTF8STRC("WGS_1984"), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, Math::Unit::Angle::AU_RADIAN},
};

Math::CoordinateSystemManager::GeographicCSysInfo Math::CoordinateSystemManager::csysList[] = {
//	{"srid","geoName", datumName, eet},
	{4001, UTF8STRC("AIRY_1830"), "AIRY_1830", Math::EarthEllipsoid::EET_AIRY1830},
	{4002, UTF8STRC("AIRY_1830M"), "AIRY_1830", Math::EarthEllipsoid::EET_AIRY1830M},
	{4600, UTF8STRC("Anguilla 1957"), "Anguilla_1957", Math::EarthEllipsoid::EET_CLARKE1880R},
	{4601, UTF8STRC("Antigua 1943"), "Antigua_1943", Math::EarthEllipsoid::EET_CLARKE1880R},
	{4214, UTF8STRC("Beijing 1954"), "Beijing_1954", Math::EarthEllipsoid::EET_KRASSOVSKY1940},
	{4490, UTF8STRC("CGCS2000"), "CGCS2000", Math::EarthEllipsoid::EET_INTL1924},
	{4602, UTF8STRC("Dominica 1945"), "Dominica_1945", Math::EarthEllipsoid::EET_CLARKE1880R},
	{4603, UTF8STRC("Grenada 1953"), "Grenada_1953", Math::EarthEllipsoid::EET_CLARKE1880R},
	{4739, UTF8STRC("HONGKONG"), "D_HONGKONG", Math::EarthEllipsoid::EET_INTL1924},
	{4611, UTF8STRC("Hong Kong 1980"), "Hong_Kong_1980", Math::EarthEllipsoid::EET_INTL1924},
	{4236, UTF8STRC("Hu Tzu Shan 1950"), "Hu_Tzu_Shan_1950", Math::EarthEllipsoid::EET_INTL1924},
	{8428, UTF8STRC("Macau_2009"), "Macau_2009", Math::EarthEllipsoid::EET_INTL1924},
	{4604, UTF8STRC("Montserrat 1958"), "Montserrat_1958", Math::EarthEllipsoid::EET_CLARKE1880R},
	{4605, UTF8STRC("St. Kitts 1955"), "St_Kitts_1955", Math::EarthEllipsoid::EET_CLARKE1880R},
	{4606, UTF8STRC("St. Lucia 1955"), "St_Lucia_1955", Math::EarthEllipsoid::EET_CLARKE1880R},
	{4607, UTF8STRC("St. Vincent 1945"), "St_Vincent_1945", Math::EarthEllipsoid::EET_CLARKE1880R},
	{3821, UTF8STRC("Taiwan_1967"), "Taiwan_1967", Math::EarthEllipsoid::EET_SAM1969},
	{3824, UTF8STRC("Taiwan_1997"), "Taiwan_1997", Math::EarthEllipsoid::EET_GRS80},
	{4326, UTF8STRC("WGS_1984"), "WGS_1984", Math::EarthEllipsoid::EET_WGS84},
	{4326, UTF8STRC("WGS 84"), "WGS_1984", Math::EarthEllipsoid::EET_WGS84}
};

Math::CoordinateSystemManager::ProjectedCSysInfo Math::CoordinateSystemManager::pcsysList[] = {
//	{srid, projName, falseEasting, falseNorthing, centralMeridian, latitudeOfOrigin, scaleFactor, geoName};
	{2000, Math::CoordinateSystem::CoordinateSystemType::MercatorProjected, UTF8STRC("Anguilla 1957 / British West Indies Grid"), 400000, 0, -62, 0, 0.9995, Math::CoordinateSystem::UT_METRE, "Anguilla 1957"},
	{2001, Math::CoordinateSystem::CoordinateSystemType::MercatorProjected, UTF8STRC("Antigua 1943 / British West Indies Grid"), 400000, 0, -62, 0, 0.9995, Math::CoordinateSystem::UT_METRE, "Antigua 1943"},
	{2435, Math::CoordinateSystem::CoordinateSystemType::MercatorProjected, UTF8STRC("Beijing 1954 / 3-degree Gauss-Kruger CM 114E"), 500000, 0, 114, 0, 1, Math::CoordinateSystem::UT_METRE, "Beijing 1954"},
	{2002, Math::CoordinateSystem::CoordinateSystemType::MercatorProjected, UTF8STRC("Dominica 1945 / British West Indies Grid"), 400000, 0, -62, 0, 0.9995, Math::CoordinateSystem::UT_METRE, "Dominica 1945"},
	{2003, Math::CoordinateSystem::CoordinateSystemType::MercatorProjected, UTF8STRC("Grenada 1953 / British West Indies Grid"), 400000, 0, -62, 0, 0.9995, Math::CoordinateSystem::UT_METRE, "Grenada 1953"},
	{3407, Math::CoordinateSystem::CoordinateSystemType::CassiniSoldner,    UTF8STRC("Hong Kong 1963 Grid System"), 132033.92, 62565.96, 114.178555555556, 22.3121333333333, 1, Math::CoordinateSystem::UT_CLARKE_FOOT, "Hong Kong 1963"},
	{2326, Math::CoordinateSystem::CoordinateSystemType::MercatorProjected, UTF8STRC("Hong Kong 1980 Grid System"), 836694.05, 819069.80, 114.17855555555555555555555555556, 22.312133333333333333333333333333, 1, Math::CoordinateSystem::UT_METRE, "Hong Kong 1980"},
	{29900,Math::CoordinateSystem::CoordinateSystemType::MercatorProjected, UTF8STRC("Irish National Grid"), 200000, 250000, -8, 53.5, 1.000035, Math::CoordinateSystem::UT_METRE, "AIRY_1830M"},
	{8432, Math::CoordinateSystem::CoordinateSystemType::MercatorProjected, UTF8STRC("Macau Grid"), 20000.00, 20000.00, 113.53646944444444444444444444444, 22.21239722222222222222222222222222, 1, Math::CoordinateSystem::UT_METRE, "Macau_2009"},
	{2004, Math::CoordinateSystem::CoordinateSystemType::MercatorProjected, UTF8STRC("Montserrat 1958 / British West Indies Grid"), 400000, 0, -62, 0, 0.9995, Math::CoordinateSystem::UT_METRE, "Montserrat 1958"},
	{2005, Math::CoordinateSystem::CoordinateSystemType::MercatorProjected, UTF8STRC("St. Kitts 1955 / British West Indies Grid"), 400000, 0, -62, 0, 0.9995, Math::CoordinateSystem::UT_METRE, "St. Kitts 1955"},
	{2006, Math::CoordinateSystem::CoordinateSystemType::MercatorProjected, UTF8STRC("St. Lucia 1955 / British West Indies Grid"), 400000, 0, -62, 0, 0.9995, Math::CoordinateSystem::UT_METRE, "St. Lucia 1955"},
	{2006, Math::CoordinateSystem::CoordinateSystemType::MercatorProjected, UTF8STRC("St. Vincent 45 / British West Indies Grid"), 400000, 0, -62, 0, 0.9995, Math::CoordinateSystem::UT_METRE, "St. Vincent 1945"},
	{3827, Math::CoordinateSystem::CoordinateSystemType::MercatorProjected, UTF8STRC("Taiwan 1967 Grid Zone 119"), 250000.00, 0.00, 119.0, 0, 0.9999, Math::CoordinateSystem::UT_METRE, "Taiwan_1967"},
	{3828, Math::CoordinateSystem::CoordinateSystemType::MercatorProjected, UTF8STRC("Taiwan 1967 Grid Zone 121"), 250000.00, 0.00, 121.0, 0, 0.9999, Math::CoordinateSystem::UT_METRE, "Taiwan_1967"},
	{3825, Math::CoordinateSystem::CoordinateSystemType::MercatorProjected, UTF8STRC("Taiwan 1997 Grid Zone 119"), 250000.00, 0.00, 119.0, 0, 0.9999, Math::CoordinateSystem::UT_METRE, "Taiwan_1997"},
	{3826, Math::CoordinateSystem::CoordinateSystemType::MercatorProjected, UTF8STRC("Taiwan 1997 Grid Zone 121"), 250000.00, 0.00, 121.0, 0, 0.9999, Math::CoordinateSystem::UT_METRE, "Taiwan_1997"},
	{0,    Math::CoordinateSystem::CoordinateSystemType::MercatorProjected, UTF8STRC("UK National Grid"), 400000, -100000, -2, 49, 0.9996012717, Math::CoordinateSystem::UT_METRE, "AIRY_1830"},
	{3857, Math::CoordinateSystem::CoordinateSystemType::Mercator1SPProjected, UTF8STRC("WGS 84 / Pseudo-Mercator"), 0, 0, 0, 0, 1, Math::CoordinateSystem::UT_METRE, "WGS 84"}
};

Optional<const Math::CoordinateSystemManager::SpatialRefInfo> Math::CoordinateSystemManager::SRGetSpatialRef(UInt32 epsgId)
{
	OSInt i = 0;
	OSInt j = (OSInt)(sizeof(srInfoList) / sizeof(srInfoList[0])) - 1;
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
	return nullptr;
}

Optional<const Math::CoordinateSystemManager::SpatialRefInfo> Math::CoordinateSystemManager::SRGetSpatialRefPrev(UInt32 epsgId)
{
	OSInt i = 0;
	OSInt j = (OSInt)(sizeof(srInfoList) / sizeof(srInfoList[0])) - 1;
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
			return nullptr;
		}
	}
#if defined(VERBOSE)
	printf("SRGetSpatialRefPrev(%d) not found\r\n", epsgId);
#endif
	return nullptr;
}

Optional<const Math::CoordinateSystemManager::SpatialRefInfo> Math::CoordinateSystemManager::SRGetSpatialRefNext(UInt32 epsgId)
{
	OSInt lastInd;
	OSInt i = 0;
	OSInt j = lastInd = (OSInt)(sizeof(srInfoList) / sizeof(srInfoList[0])) - 1;
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
			return nullptr;
		}
	}
#if defined(VERBOSE)
	printf("SRGetSpatialRefNext(%d) not found\r\n", epsgId);
#endif
	return nullptr;
}

Optional<const Math::CoordinateSystemManager::SpheroidInfo> Math::CoordinateSystemManager::SRGetSpheroid(UInt32 epsgId)
{
	OSInt i = 0;
	OSInt j = (OSInt)(sizeof(spheroidSRID) / sizeof(spheroidSRID[0])) - 1;
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
	return nullptr;
}

Optional<const Math::CoordinateSystemManager::DatumInfo> Math::CoordinateSystemManager::SRGetDatum(UInt32 epsgId)
{
	OSInt i = 0;
	OSInt j = (OSInt)(sizeof(datumSRID) / sizeof(datumSRID[0])) - 1;
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
	return nullptr;
}

Optional<const Math::CoordinateSystemManager::GeogcsSRInfo> Math::CoordinateSystemManager::SRGetGeogcsInfo(UInt32 epsgId)
{
	OSInt i = 0;
	OSInt j = (OSInt)(sizeof(geogcsSRID) / sizeof(geogcsSRID[0])) - 1;
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
	return nullptr;
}

Optional<const Math::CoordinateSystemManager::ProjcsSRInfo> Math::CoordinateSystemManager::SRGetProjcsInfo(UInt32 epsgId)
{
	OSInt i = 0;
	OSInt j = (OSInt)(sizeof(projcsSRID) / sizeof(projcsSRID[0])) - 1;
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
	return nullptr;
}

Bool Math::CoordinateSystemManager::SRAxisReversed(UInt32 epsgId)
{
	NN<const Math::CoordinateSystemManager::SpatialRefInfo> info;
	if (SRGetSpatialRef(epsgId).SetTo(info))
	{
		if (info->srType == SRT_GEOGCS)
		{
			NN<const GeogcsSRInfo> geogcs;
			if (SRGetGeogcsInfo(epsgId).SetTo(geogcs))
			{
				return geogcs->reverseAxis;
			}
		}
	}
	return false;
}

Optional<Math::CoordinateSystem> Math::CoordinateSystemManager::SRCreateCSys(UInt32 epsgId)
{
	NN<const Math::CoordinateSystemManager::SpatialRefInfo> info;
	if (SRGetSpatialRef(epsgId).SetTo(info))
	{
		Optional<Math::CoordinateSystem> csys = nullptr;
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
#if defined(VERBOSE)
	printf("Unsupported SRID:%d\r\n", epsgId);
#endif
	return nullptr;
}

NN<Math::CoordinateSystem> Math::CoordinateSystemManager::SRCreateCSysOrDef(UInt32 epsgId)
{
	NN<Math::CoordinateSystem> csys;
	if (SRCreateCSys(epsgId).SetTo(csys))
		return csys;
	else
		return CreateWGS84Csys();
}

Optional<Math::ProjectedCoordinateSystem> Math::CoordinateSystemManager::SRCreateProjCSys(UInt32 epsgId)
{
	NN<const Math::CoordinateSystemManager::ProjcsSRInfo> projcs;
	if (!SRGetProjcsInfo(epsgId).SetTo(projcs))
	{
		return nullptr;
	}
	NN<Math::GeographicCoordinateSystem> gcsys;
	if (!SRCreateGeogCSys(projcs->geogcsSRID).SetTo(gcsys))
	{
		return nullptr;
	}
	Math::ProjectedCoordinateSystem *csys;
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	sptr = Text::StrUInt32(Text::StrConcatC(sbuff, UTF8STRC("EPSG:")), epsgId);
	if (projcs->csysType == Math::CoordinateSystem::CoordinateSystemType::MercatorProjected || projcs->csysType == Math::CoordinateSystem::CoordinateSystemType::GausskrugerProjected)
	{
		NEW_CLASS(csys, Math::MercatorProjectedCoordinateSystem(CSTRP(sbuff, sptr), projcs->srid, Text::CStringNN(projcs->projName, projcs->projNameLen), projcs->falseEasting, projcs->falseNorthing, projcs->centralMeridian, projcs->latitudeOfOrigin, projcs->scaleFactor, gcsys, projcs->unit));
		return csys;
	}
	else if (projcs->csysType == Math::CoordinateSystem::CoordinateSystemType::Mercator1SPProjected)
	{
		NEW_CLASS(csys, Math::Mercator1SPProjectedCoordinateSystem(CSTRP(sbuff, sptr), projcs->srid, Text::CStringNN(projcs->projName, projcs->projNameLen), projcs->falseEasting, projcs->falseNorthing, projcs->centralMeridian, projcs->latitudeOfOrigin, projcs->scaleFactor, gcsys, projcs->unit));
		return csys;
	}
	else if (projcs->csysType == Math::CoordinateSystem::CoordinateSystemType::CassiniSoldner)
	{
		NEW_CLASS(csys, Math::CassiniSoldnerCoordinateSystem(CSTRP(sbuff, sptr), projcs->srid, Text::CStringNN(projcs->projName, projcs->projNameLen), projcs->falseEasting, projcs->falseNorthing, projcs->centralMeridian, projcs->latitudeOfOrigin, projcs->scaleFactor, gcsys, projcs->unit));
		return csys;
	}
	gcsys.Delete();
	return nullptr;
}

Optional<Math::GeographicCoordinateSystem> Math::CoordinateSystemManager::SRCreateGeogCSys(UInt32 epsgId)
{
	NN<const Math::CoordinateSystemManager::GeogcsSRInfo> geogcs;
	if (!SRGetGeogcsInfo(epsgId).SetTo(geogcs))
	{
		return nullptr;
	}
	NN<const Math::CoordinateSystemManager::DatumInfo> datum;
	if (!SRGetDatum(geogcs->datum).SetTo(datum))
	{
		return nullptr;
	}
	NN<const Math::CoordinateSystemManager::SpheroidInfo> spheroid;
	if (!SRGetSpheroid(datum->spheroid).SetTo(spheroid))
	{
		return nullptr;
	}
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	Math::GeographicCoordinateSystem *csys;
	sptr = Text::StrUInt32(Text::StrConcatC(sbuff, UTF8STRC("EPSG:")), epsgId);
	Math::EarthEllipsoid ellipsoid(spheroid->eet);
	Math::GeographicCoordinateSystem::DatumData1 data;
	FillDatumData(data, datum, Text::CStringNN(datum->datumName, datum->datumNameLen), ellipsoid, spheroid);
	NEW_CLASS(csys, Math::GeographicCoordinateSystem(CSTRP(sbuff, sptr), epsgId, Text::CStringNN(geogcs->name, geogcs->nameLen), &data, geogcs->primem, geogcs->unit));
	return csys;
}

Optional<Math::CoordinateSystem> Math::CoordinateSystemManager::CreateFromName(Text::CStringNN name, OptOut<Bool> axisInvertable)
{
	axisInvertable.Set(false);
	if (name.StartsWith(UTF8STRC("EPSG:")))
	{
		UInt32 epsgId = Text::StrToUInt32(&name.v[5]);
		return SRCreateCSys(epsgId);
	}
	else if (name.StartsWith(UTF8STRC("urn:ogc:def:crs:EPSG::")))
	{
		UInt32 epsgId = Text::StrToUInt32(&name.v[22]);
		if (axisInvertable.IsNotNull()) axisInvertable.SetNoCheck(SRAxisReversed(epsgId));
		return SRCreateCSys(epsgId);
	}
	else if (name.StartsWith(UTF8STRC("urn:x-ogc:def:crs:EPSG:")))
	{
		UInt32 epsgId = Text::StrToUInt32(&name.v[23]);
		if (axisInvertable.IsNotNull()) axisInvertable.SetNoCheck(SRAxisReversed(epsgId));
		return SRCreateCSys(epsgId);
	}
	else if (name.StartsWith(UTF8STRC("http://www.opengis.net/gml/srs/epsg.xml#")))
	{
		UInt32 epsgId = Text::StrToUInt32(&name.v[40]);
		return SRCreateCSys(epsgId);
	}
	else if (name.Equals(UTF8STRC("urn:ogc:def:crs:OGC:1.3:CRS84")))
	{
		return CreateGeogCoordinateSystem(name, Math::CoordinateSystemManager::GeoCoordSysTypeGetName(Math::CoordinateSystemManager::GCST_WGS84).v);
	}
	else if (name.Equals(UTF8STRC("CRS:84")))
	{
		return CreateGeogCoordinateSystem(name, Math::CoordinateSystemManager::GeoCoordSysTypeGetName(Math::CoordinateSystemManager::GCST_WGS84).v);
	}
	return nullptr;
}

NN<Math::CoordinateSystem> Math::CoordinateSystemManager::CreateFromNameOrDef(Text::CStringNN name)
{
	Bool axisInvertable;
	NN<Math::CoordinateSystem> csys;
	if (CreateFromName(name, axisInvertable).SetTo(csys))
		return csys;
	else
		return CreateWGS84Csys();
}

Optional<const Math::CoordinateSystemManager::DatumInfo> Math::CoordinateSystemManager::GetDatumInfoByName(UnsafeArray<const UTF8Char> name)
{
	if (name[0] == 'D' && name[1] == '_')
	{
		name = &name[2];
	}
	OSInt i = 0;
	OSInt j = (OSInt)(sizeof(datumList) / sizeof(datumList[0])) - 1;
	OSInt k;
	OSInt l;
	while (i <= j)
	{
		k = (i + j) >> 1;
		l = Text::StrCompareICase(datumList[k].datumName, name);
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
			return datumList[k];
		}
	}
	return nullptr;
}

void Math::CoordinateSystemManager::FillDatumData(NN<Math::GeographicCoordinateSystem::DatumData1> data, Optional<const Math::CoordinateSystemManager::DatumInfo> datum, Text::CStringNN name, NN<Math::EarthEllipsoid> ee, Optional<const SpheroidInfo> spheroid)
{
	NN<const Math::CoordinateSystemManager::DatumInfo> nndatum;
	NN<const SpheroidInfo> nnspheroid;
	if (datum.SetTo(nndatum))
	{
		data->srid = nndatum->srid;
		data->spheroid.ellipsoid = ee;
		data->spheroid.srid = nndatum->spheroid;
		if (spheroid.SetTo(nnspheroid))
		{
			data->spheroid.name = nnspheroid->name;
			data->spheroid.nameLen = nnspheroid->nameLen;
		}
		else
		{
			data->spheroid.name = nndatum->datumName;
			data->spheroid.nameLen = nndatum->datumNameLen;
		}
		data->name = nndatum->datumName;
		data->nameLen = nndatum->datumNameLen;
		data->x0 = nndatum->x0;
		data->y0 = nndatum->y0;
		data->z0 = nndatum->z0;
		data->cX = nndatum->cX;
		data->cY = nndatum->cY;
		data->cZ = nndatum->cZ;
		data->xAngle = nndatum->xAngle;
		data->yAngle = nndatum->yAngle;
		data->zAngle = nndatum->zAngle;
		data->scale = nndatum->scale;
		data->aunit = nndatum->aunit;
	}
	else
	{
		data->srid = 0;
		data->spheroid.ellipsoid = ee;
		data->spheroid.srid = 0;
		data->spheroid.name = name.v;
		data->spheroid.nameLen = name.leng;
		data->name = name.v;
		data->nameLen = name.leng;
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

Optional<Math::ProjectedCoordinateSystem> Math::CoordinateSystemManager::CreateProjCoordinateSystemDefName(Math::CoordinateSystemManager::ProjCoordSysType pcst)
{
	Text::CStringNN name;
	if (!Math::CoordinateSystemManager::ProjCoordSysTypeGetName(pcst).SetTo(name))
		return nullptr;
	return CreateProjCoordinateSystem(name, name.v);
}

NN<Math::CoordinateSystem> Math::CoordinateSystemManager::CreateProjCoordinateSystemDefNameOrDef(ProjCoordSysType pcst)
{
	NN<Math::CoordinateSystem> csys;
	if (!Optional<CoordinateSystem>(CreateProjCoordinateSystemDefName(pcst)).SetTo(csys))
		csys = CreateWGS84Csys();
	return csys;
}

Optional<Math::ProjectedCoordinateSystem> Math::CoordinateSystemManager::CreateProjCoordinateSystem(Text::CStringNN sourceName, UnsafeArray<const UTF8Char> projName)
{
	NN<const Math::CoordinateSystemManager::ProjectedCSysInfo> coord;
	NN<Math::GeographicCoordinateSystem> gcs;
	Math::ProjectedCoordinateSystem *csys = 0;
	if (!GetProjCoordinateSystemInfo(projName).SetTo(coord))
	{
		return nullptr;
	}
	if (!Math::CoordinateSystemManager::CreateGeogCoordinateSystem(sourceName, (const UTF8Char*)coord->geoName).SetTo(gcs))
	{
		return nullptr;
	}
	if (coord->csysType == Math::CoordinateSystem::CoordinateSystemType::MercatorProjected)
	{
		NEW_CLASS(csys, Math::MercatorProjectedCoordinateSystem(sourceName, coord->srid, Text::CStringNN(coord->projName, coord->projNameLen), coord->falseEasting, coord->falseNorthing, coord->centralMeridian, coord->latitudeOfOrigin, coord->scaleFactor, gcs, coord->unitType));
	}
	else if (coord->csysType == Math::CoordinateSystem::CoordinateSystemType::Mercator1SPProjected)
	{
		NEW_CLASS(csys, Math::Mercator1SPProjectedCoordinateSystem(sourceName, coord->srid, Text::CStringNN(coord->projName, coord->projNameLen), coord->falseEasting, coord->falseNorthing, coord->centralMeridian, coord->latitudeOfOrigin, coord->scaleFactor, gcs, coord->unitType));
	}
	else if (coord->csysType == Math::CoordinateSystem::CoordinateSystemType::CassiniSoldner)
	{
		NEW_CLASS(csys, Math::CassiniSoldnerCoordinateSystem(sourceName, coord->srid, Text::CStringNN(coord->projName, coord->projNameLen), coord->falseEasting, coord->falseNorthing, coord->centralMeridian, coord->latitudeOfOrigin, coord->scaleFactor, gcs, coord->unitType));
	}
	return csys;
}

UOSInt Math::CoordinateSystemManager::GetProjCoordinateSystems(NN<Data::ArrayListNative<ProjCoordSysType>> csysList)
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

UOSInt Math::CoordinateSystemManager::GetProjCoordinateSystemNames(NN<Data::ArrayListArr<const UTF8Char>> nameList)
{
	UOSInt i = 0;
	UOSInt j = (sizeof(pcsysList) / sizeof(pcsysList[0]));
	while (i < j)
	{
		nameList->Add(pcsysList[i].projName);
		i++;
	}
	return j;
}

Optional<const Math::CoordinateSystemManager::ProjectedCSysInfo> Math::CoordinateSystemManager::GetProjCoordinateSystemInfo(UnsafeArray<const UTF8Char> name)
{
	OSInt i = 0;
	OSInt j = (OSInt)(sizeof(pcsysList) / sizeof(pcsysList[0])) - 1;
	OSInt k;
	OSInt l;
	while (i <= j)
	{
		k = (i + j) >> 1;
		l = Text::StrCompareICase(pcsysList[k].projName, name);
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
	return nullptr;
}


Optional<Math::GeographicCoordinateSystem> Math::CoordinateSystemManager::CreateGeogCoordinateSystemDefName(GeoCoordSysType gcst)
{
	Text::CStringNN name = Math::CoordinateSystemManager::GeoCoordSysTypeGetName(gcst);
	return CreateGeogCoordinateSystem(name, name.v);
}

Optional<Math::GeographicCoordinateSystem> Math::CoordinateSystemManager::CreateGeogCoordinateSystem(Text::CStringNN sourceName, UnsafeArray<const UTF8Char> geoName)
{
	Math::GeographicCoordinateSystem *csys;
	NN<const Math::CoordinateSystemManager::GeographicCSysInfo> coord;
	if (!GetGeogCoordinateSystemInfo(geoName).SetTo(coord))
	{
		return nullptr;
	}
	NN<const Math::CoordinateSystemManager::DatumInfo> datum;
	if (!GetDatumInfoByName((const UTF8Char*)coord->datumName).SetTo(datum))
	{
		return nullptr;
	}
	Math::EarthEllipsoid ellipsoid(coord->eet);
	Math::GeographicCoordinateSystem::DatumData1 data;
	FillDatumData(data, datum, Text::CStringNN::FromPtr((const UTF8Char*)coord->datumName), ellipsoid, SRGetSpheroid(datum->spheroid));
	NEW_CLASS(csys, Math::GeographicCoordinateSystem(sourceName, coord->srid, Text::CStringNN(coord->geoName, coord->geoNameLen), &data, Math::GeographicCoordinateSystem::PT_GREENWICH, Math::GeographicCoordinateSystem::UT_DEGREE));
	return csys;
}

UOSInt Math::CoordinateSystemManager::GetGeogCoordinateSystems(NN<Data::ArrayListNative<GeoCoordSysType>> csysList)
{
	UOSInt initCnt = csysList->GetCount();
	csysList->Add(GCST_WGS84);
	return csysList->GetCount() - initCnt;
}

Optional<const Math::CoordinateSystemManager::GeographicCSysInfo> Math::CoordinateSystemManager::GetGeogCoordinateSystemInfo(UnsafeArray<const UTF8Char> name)
{
	OSInt i = 0;
	OSInt j = (OSInt)(sizeof(csysList) / sizeof(csysList[0])) - 1;
	OSInt k;
	OSInt l;
	while (i <= j)
	{
		k = (i + j) >> 1;
		l = Text::StrCompareICase(csysList[k].geoName, name);
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
	return nullptr;
}

NN<Math::GeographicCoordinateSystem> Math::CoordinateSystemManager::CreateWGS84Csys()
{
	NN<Math::GeographicCoordinateSystem> csys;
	NN<const Math::CoordinateSystemManager::DatumInfo> datum = NN<const Math::CoordinateSystemManager::DatumInfo>::FromPtr(GetDatumInfoByName((const UTF8Char*)"WGS_1984").OrNull());
	Math::EarthEllipsoid ellipsoid(Math::EarthEllipsoid::EET_WGS84);
	Math::GeographicCoordinateSystem::DatumData1 data;
	FillDatumData(data, datum, CSTR("WGS_1984"), ellipsoid, SRGetSpheroid(datum->spheroid));
	NEW_CLASSNN(csys, Math::GeographicCoordinateSystem(CSTR("WGS_1984"), 4326, CSTR("WGS_1984"), &data, Math::GeographicCoordinateSystem::PT_GREENWICH, Math::GeographicCoordinateSystem::UT_DEGREE));
	return csys;

}

NN<Math::CoordinateSystem> Math::CoordinateSystemManager::CreateCsysByCoord(Math::Coord2DDbl coord)
{
	if (coord.x >= 793241 && coord.x <= 870617.61 && coord.y >= 799018.15 && coord.y < 848909.8)
	{
		return SRCreateCSysOrDef(2326);
	}
	else if (coord.x >= -180 && coord.x <= 180 && coord.y >= -90 && coord.y <= 90)
	{
		return CreateWGS84Csys();
	}
	else
	{
		printf("CoordinateSystemManager: CreateCsysByCoord unknown position: %lf, %lf\r\n", coord.x, coord.y);
		return CreateWGS84Csys();
	}
}

Text::CStringNN Math::CoordinateSystemManager::GeoCoordSysTypeGetName(GeoCoordSysType gcst)
{
	switch (gcst)
	{
	case Math::CoordinateSystemManager::GCST_CGCS2000:
		return CSTR("CGCS2000");
	case Math::CoordinateSystemManager::GCST_MACAU2009:
		return CSTR("Macau_2009");
	case Math::CoordinateSystemManager::GCST_HK1980:
		return CSTR("HONGKONG");
	case Math::CoordinateSystemManager::GCST_WGS84:
	default:
		return CSTR("WGS_1984");
	}
}

Text::CString Math::CoordinateSystemManager::ProjCoordSysTypeGetName(ProjCoordSysType pcst)
{
	switch (pcst)
	{
	case PCST_HK80:
		return CSTR("Hong Kong 1980 Grid System");
	case PCST_HK63:
		return CSTR("Hong Kong 1963 Grid System");
	case PCST_UK_NATIONAL_GRID:
		return CSTR("UK National Grid");
	case PCST_IRISH_NATIONAL_GRID:
		return CSTR("Irish National Grid");
	case PCST_MACAU_GRID:
		return CSTR("Macau Grid");
	case PCST_TWD67:
		return CSTR("Taiwan 1967 Grid");
	case PCST_TWD97:
		return CSTR("Taiwan 1997 Grid");
	}
	return nullptr;
}
