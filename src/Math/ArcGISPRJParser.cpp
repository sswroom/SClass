#include "Stdafx.h"
#include "IO/FileStream.h"
#include "Math/ArcGISPRJParser.h"
#include "Math/CoordinateSystemManager.h"
#include "Math/Mercator1SPProjectedCoordinateSystem.h"
#include "Math/MercatorProjectedCoordinateSystem.h"

//#define VERBOSE
#if defined(VERBOSE)
#include <stdio.h>
#endif

Bool Math::ArcGISPRJParser::ParsePRJString(UnsafeArray<UTF8Char> prjBuff, OutParam<UOSInt> strSize)
{
	UOSInt i;
	UTF8Char c;
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
			strSize.Set(i);
			return true;
		}
		i++;
	}
}

Math::ArcGISPRJParser::ArcGISPRJParser()
{

}

Math::ArcGISPRJParser::~ArcGISPRJParser()
{

}

Optional<Math::CoordinateSystem> Math::ArcGISPRJParser::ParsePRJFile(Text::CStringNN fileName)
{
	UInt8 buff[1024];
	UOSInt buffSize;
	{
		IO::FileStream fs(fileName, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Sequential);
		if (fs.IsError())
		{
#if defined(VERBOSE)
			printf("ArcGISPRJParser: Error in opening file\r\n");
#endif
			return 0;
		}
		buffSize = fs.Read(Data::ByteArray(buff, 1023));
		buff[buffSize] = 0;
	}
	if (buffSize == 1023)
	{
#if defined(VERBOSE)
		printf("ArcGISPRJParser: File too large\r\n");
#endif
		return 0;
	}

	return ParsePRJBuff(fileName, UARR(buff), buffSize, buffSize);
}

Optional<Math::CoordinateSystem> Math::ArcGISPRJParser::ParsePRJBuff(Text::CStringNN sourceName, UnsafeArray<UTF8Char> prjBuff, UOSInt buffSize, OptOut<UOSInt> parsedSize)
{
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt nameOfst;
	UOSInt nameLen;
	UOSInt datumOfst = 0;
	UOSInt datumLen = 0;
	UOSInt spIndex = 0;
	Double a = 0;
	Double f_1 = 0;
	Math::CoordinateSystem *csys = 0;
	Math::EarthEllipsoid::EarthEllipsoidType eet;
	Optional<Math::GeographicCoordinateSystem> gcs = 0;
	Math::GeographicCoordinateSystem::PrimemType primem = Math::GeographicCoordinateSystem::PT_GREENWICH;
	Math::GeographicCoordinateSystem::UnitType unit = Math::GeographicCoordinateSystem::UT_DEGREE;
	UTF8Char c;
	UInt32 srid = 0;
	if (Text::StrStartsWithC(prjBuff, buffSize, UTF8STRC("GEOGCS[")))
	{
		Bool hasToWGS84 = false;
		Double cX = 0;
		Double cY = 0;
		Double cZ = 0;
		Double xAngle = 0;
		Double yAngle = 0;
		Double zAngle = 0;
		Double scale = 0;
		i = 7;
		if (!ParsePRJString(&prjBuff[i], nameLen))
		{
#if defined(VERBOSE)
			printf("ArcGISPRJParser: Error in parsing String for GEOGCS: %s\r\n", &prjBuff[i]);
#endif
			return 0;
		}
		nameOfst = i + 1;
		prjBuff[i + nameLen - 1] = 0;
		i += nameLen;
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
				if (Text::StrStartsWithC(&prjBuff[i], buffSize - i, UTF8STRC("DATUM[")))
				{
					i += 6;
					if (!ParsePRJString(&prjBuff[i], datumLen))
					{
#if defined(VERBOSE)
						printf("ArcGISPRJParser: Error in parsing String for DATUM: %s\r\n", &prjBuff[i]);
#endif
						return 0;
					}
					datumOfst = i + 1;
					prjBuff[i + datumLen - 1] = 0;
					i += datumLen;
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
							if (Text::StrStartsWithC(&prjBuff[i], buffSize - i, UTF8STRC("SPHEROID[")))
							{
								i += 9;
								if (!ParsePRJString(&prjBuff[i], j))
								{
#if defined(VERBOSE)
									printf("ArcGISPRJParser: Error in parsing String for SPHEROID: %s\r\n", &prjBuff[i]);
#endif
									return 0;
								}
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
												a = Text::StrToDoubleOr(&prjBuff[j], 0);
											}
											else if (spIndex == 2)
											{
												f_1 = Text::StrToDoubleOr(&prjBuff[j], 0);
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
#if defined(VERBOSE)
										printf("ArcGISPRJParser: Unexpected end of string at %d\r\n", (UInt32)i);
#endif
										return 0;
									}
									else
									{
										i++;
									}
								}
							}
							else if (Text::StrStartsWithC(&prjBuff[i], buffSize - i, UTF8STRC("TOWGS84[")))
							{
								i += 8;
								j = i;
								k = 1;
								while (true)
								{
									c = prjBuff[i];
									if (c == ']' || c == ',')
									{
										if ((OSInt)j >= 0)
										{
											prjBuff[i] = 0;
											if (k == 1)
											{
												cX = Text::StrToDoubleOr(&prjBuff[j], 0);
											}
											else if (k == 2)
											{
												cY = Text::StrToDoubleOr(&prjBuff[j], 0);
											}
											else if (k == 3)
											{
												cZ = Text::StrToDoubleOr(&prjBuff[j], 0);
											}
											else if (k == 4)
											{
												xAngle = Text::StrToDoubleOr(&prjBuff[j], 0);
											}
											else if (k == 5)
											{
												yAngle = Text::StrToDoubleOr(&prjBuff[j], 0);
											}
											else if (k == 6)
											{
												zAngle = Text::StrToDoubleOr(&prjBuff[j], 0);
											}
											else if (k == 7)
											{
												scale = Text::StrToDoubleOr(&prjBuff[j], 0);
											}
											k++;
										}
										i++;
										if (c == ']')
										{
											if (k != 8)
											{
#if defined(VERBOSE)
												printf("ArcGISPRJParser: TOWGS84 expected 7 parameters, now is %d\r\n", (UInt32)(k - 1));
#endif
												return 0;
											}
											hasToWGS84 = true;
											break;
										}
										j = i;
									}
									else if (c == 0)
									{
#if defined(VERBOSE)
										printf("ArcGISPRJParser: Unexpected end of string at %d\r\n", (UInt32)i);
#endif
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
#if defined(VERBOSE)
								printf("ArcGISPRJParser: Expected to be SPHEROID, current is %s\r\n", &prjBuff[i]);
#endif
								return 0;
							}
						}
						else
						{
#if defined(VERBOSE)
							printf("ArcGISPRJParser: Unknown char in DATUM: %s\r\n", &prjBuff[i]);
#endif
							return 0;
						}
					}
					i++;
				}
				else if (Text::StrStartsWithC(&prjBuff[i], buffSize - i, UTF8STRC("PRIMEM[")))
				{
					i += 7;
					if (!ParsePRJString(&prjBuff[i], j))
					{
#if defined(VERBOSE)
						printf("ArcGISPRJParser: Error in parsing String for PRIMEM: %s\r\n", &prjBuff[i]);
#endif
						return 0;
					}
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
				else if (Text::StrStartsWithC(&prjBuff[i], buffSize - i, UTF8STRC("UNIT[")))
				{
					i += 5;
					if (!ParsePRJString(&prjBuff[i], j))
					{
#if defined(VERBOSE)
						printf("ArcGISPRJParser: Error in parsing String for UNIT: %s\r\n", &prjBuff[i]);
#endif
						return 0;
					}
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
#if defined(VERBOSE)
					printf("ArcGISPRJParser: Unknown value on GEOGCS: %s\r\n", &prjBuff[i]);
#endif
					return 0;
				}
			}
			else
			{
#if defined(VERBOSE)
				printf("ArcGISPRJParser: Unknown char in GEOGCS: %s\r\n", &prjBuff[i]);
#endif
				return 0;
			}
		}
		if (spIndex != 3)
		{
#if defined(VERBOSE)
			printf("ArcGISPRJParser: SPHEROID not valid, index = %d\r\n", (UInt32)spIndex);
#endif
			return 0;
		}
		parsedSize.Set(i);
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
		srid = this->csys.GuessSRIDGeog(Text::CStringNN(&prjBuff[nameOfst], nameLen - 2));
		if (eet != Math::EarthEllipsoid::EET_OTHER)
		{
			Math::EarthEllipsoid ellipsoid(eet);
			Optional<const Math::CoordinateSystemManager::DatumInfo> datum = Math::CoordinateSystemManager::GetDatumInfoByName((const UTF8Char*)&prjBuff[datumOfst]);

			Math::GeographicCoordinateSystem::DatumData1 data;
			Math::CoordinateSystemManager::FillDatumData(data, datum, {&prjBuff[datumOfst], datumLen - 2}, ellipsoid, 0);
			if (hasToWGS84)
			{
				data.cX = cX;
				data.cY = cY;
				data.cZ = cZ;
				data.xAngle = xAngle;
				data.yAngle = yAngle;
				data.zAngle = zAngle;
				data.scale = scale;
				data.aunit = Math::Unit::Angle::AU_ARCSECOND;
			}
			NEW_CLASS(csys, Math::GeographicCoordinateSystem(sourceName, srid, {&prjBuff[nameOfst], nameLen - 2}, &data, primem, unit));
			return csys;
		}
		else
		{
			Math::EarthEllipsoid ellipsoid(a, f_1, eet);
			Optional<const Math::CoordinateSystemManager::DatumInfo> datum = Math::CoordinateSystemManager::GetDatumInfoByName((const UTF8Char*)&prjBuff[datumOfst]);
			Math::GeographicCoordinateSystem::DatumData1 data;
			Math::CoordinateSystemManager::FillDatumData(data, datum, {&prjBuff[datumOfst], datumLen - 2}, ellipsoid, 0);
			if (hasToWGS84)
			{
				data.cX = cX;
				data.cY = cY;
				data.cZ = cZ;
				data.xAngle = xAngle;
				data.yAngle = yAngle;
				data.zAngle = zAngle;
				data.scale = scale;
				data.aunit = Math::Unit::Angle::AU_ARCSECOND;
			}
			NEW_CLASS(csys, Math::GeographicCoordinateSystem(sourceName, srid, {&prjBuff[nameOfst], nameLen - 2}, &data, primem, unit));
			return csys;
		}
	}
	else if (Text::StrStartsWithC(prjBuff, buffSize, UTF8STRC("PROJCS[")))
	{
		Math::CoordinateSystem::CoordinateSystemType cst = Math::CoordinateSystem::CoordinateSystemType::Geographic;
		Double falseEasting = NAN;
		Double falseNorthing = NAN;
		Double centralMeridian = NAN;
		Double scaleFactor = NAN;
		Double latitudeOfOrigin = NAN;
		UOSInt nOfst;
		UOSInt nLen;
		UOSInt vOfst;
		Bool commaFound;

		i = 7;
		if (!ParsePRJString(&prjBuff[i], nameLen))
		{
#if defined(VERBOSE)
			printf("ArcGISPRJParser: Error in parsing String for PROJCS: %s\r\n", &prjBuff[i]);
#endif
			return 0;
		}
		nameOfst = i + 1;
		prjBuff[i + nameLen - 1] = 0;
		i += nameLen;
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
				if (Text::StrStartsWithC(&prjBuff[i], buffSize - i, UTF8STRC("GEOGCS[")))
				{
					gcs = Optional<Math::GeographicCoordinateSystem>::ConvertFrom(ParsePRJBuff(sourceName, &prjBuff[i], buffSize - i, j));
					if (gcs.IsNull())
						return 0;
					i += j;
				}
				else if (Text::StrStartsWithC(&prjBuff[i], buffSize - i, UTF8STRC("PROJECTION[")))
				{
					if (Text::StrStartsWithC(&prjBuff[i + 11], buffSize - i - 11, UTF8STRC("\"Transverse_Mercator\"]")))
					{
						i += 33;
						cst = Math::CoordinateSystem::CoordinateSystemType::MercatorProjected;
					}
					else if (Text::StrStartsWithC(&prjBuff[i + 11], buffSize - i - 11, UTF8STRC("\"Mercator_1SP\"]")))
					{
						i += 26;
						cst = Math::CoordinateSystem::CoordinateSystemType::Mercator1SPProjected;
					}
					else if (Text::StrStartsWithC(&prjBuff[i + 11], buffSize - i - 11, UTF8STRC("\"Gauss_Kruger\"]")))
					{
						i += 26;
						cst = Math::CoordinateSystem::CoordinateSystemType::GausskrugerProjected;
					}
					else
					{
#if defined(VERBOSE)
						printf("ArcGISPRJParser: Unsupported PROJECTION: %s\r\n", &prjBuff[i + 11]);
#endif
						gcs.Delete();
						return 0;
					}
				}
				else if (Text::StrStartsWithC(&prjBuff[i], buffSize - i, UTF8STRC("PARAMETER[")))
				{
					i += 10;
					if (!ParsePRJString(&prjBuff[i], j))
					{
#if defined(VERBOSE)
						printf("ArcGISPRJParser: Error in parsing String for PARAMETER: %s\r\n", &prjBuff[i]);
#endif
						gcs.Delete();
						return 0;
					}
					nOfst = i + 1;
					nLen = j - 2;
					prjBuff[i + j - 1] = 0;
					i += j;
					if (prjBuff[i] != ',')
					{
#if defined(VERBOSE)
						printf("ArcGISPRJParser: PARAMETER comma not found: %s\r\n", &prjBuff[i]);
#endif
						gcs.Delete();
						return 0;
					}
					vOfst = i + 1;
					i++;
					while (true)
					{
						c = prjBuff[i];
						if (c == 0 || c == ',')
						{
#if defined(VERBOSE)
							printf("ArcGISPRJParser: PARAMETER Unexpected end at %d\r\n", (UInt32)i);
#endif
							gcs.Delete();
							return 0;
						}
						else if (c == ']')
						{
							prjBuff[i] = 0;
							i++;
							if (Text::StrEqualsICaseC(&prjBuff[nOfst], nLen, UTF8STRC("False_Easting")))
							{
								falseEasting = Text::StrToDoubleOrNAN(&prjBuff[vOfst]);
							}
							else if (Text::StrEqualsICaseC(&prjBuff[nOfst], nLen, UTF8STRC("False_Northing")))
							{
								falseNorthing = Text::StrToDoubleOrNAN(&prjBuff[vOfst]);
							}
							else if (Text::StrEqualsICaseC(&prjBuff[nOfst], nLen, UTF8STRC("Central_Meridian")))
							{
								centralMeridian = Text::StrToDoubleOrNAN(&prjBuff[vOfst]);
							}
							else if (Text::StrEqualsICaseC(&prjBuff[nOfst], nLen, UTF8STRC("Scale_Factor")))
							{
								scaleFactor = Text::StrToDoubleOrNAN(&prjBuff[vOfst]);
							}
							else if (Text::StrEqualsICaseC(&prjBuff[nOfst], nLen, UTF8STRC("Latitude_Of_Origin")))
							{
								latitudeOfOrigin = Text::StrToDoubleOrNAN(&prjBuff[vOfst]);
							}
							else
							{
#if defined(VERBOSE)
								printf("ArcGISPRJParser: Unknown PARAMETER type: %s\r\n", &prjBuff[nOfst]);
#endif
								gcs.Delete();
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
				else if (Text::StrStartsWithC(&prjBuff[i], buffSize - i, UTF8STRC("UNIT[")))
				{
					i += 5;
					if (!ParsePRJString(&prjBuff[i], j))
					{
#if defined(VERBOSE)
						printf("ArcGISPRJParser: Error in parsing String for UNIT: %s\r\n", &prjBuff[i]);
#endif
						gcs.Delete();
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
#if defined(VERBOSE)
								printf("ArcGISPRJParser: PARAMETER Unexpected comma at %d\r\n", (UInt32)i);
#endif
								gcs.Delete();
								return 0;
							}
							commaFound = true;
						}
						else if (c == ']')
						{
							i++;
							if (!commaFound)
							{
#if defined(VERBOSE)
								printf("ArcGISPRJParser: PARAMETER Unexpected close at %d\r\n", (UInt32)i);
#endif
								gcs.Delete();
								return 0;
							}
							break;
						}
						else if (c == 0)
						{
#if defined(VERBOSE)
							printf("ArcGISPRJParser: PARAMETER Unexpected end at %d\r\n", (UInt32)i);
#endif
							gcs.Delete();
							return 0;
						}
						else
						{
							i++;
						}
					}
				}
				else if (Text::StrStartsWithC(&prjBuff[i], buffSize - i, UTF8STRC("AUTHORITY[")))
				{
					i += 10;
					if (!ParsePRJString(&prjBuff[i], j))
					{
#if defined(VERBOSE)
						printf("ArcGISPRJParser: Error in parsing String for AUTHORITY: %s\r\n", &prjBuff[i]);
#endif
						gcs.Delete();
						return 0;
					}
					i += j;
					if (prjBuff[i] != ',')
					{
#if defined(VERBOSE)
						printf("ArcGISPRJParser: Comma not found after AUTHORITY name: %s\r\n", &prjBuff[i]);
#endif
						gcs.Delete();
						return 0;
					}
					i++;
					j = i;
					while (true)
					{
						c = prjBuff[i];
						if (c == ']')
						{
							prjBuff[i] = 0;
							if (!Text::StrToUInt32(&prjBuff[j], srid))
							{
#if defined(VERBOSE)
								printf("ArcGISPRJParser: AUTHORITY is not an integer %s\r\n", &prjBuff[j]);
#endif
							}
							i++;
							break;
						}
						else if (c == 0)
						{
#if defined(VERBOSE)
							printf("ArcGISPRJParser: AUTHORITY Unexpected end at %d\r\n", (UInt32)i);
#endif
							gcs.Delete();
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
#if defined(VERBOSE)
					printf("ArcGISPRJParser: Unsupport type in PROJCS: %s\r\n", &prjBuff[i]);
#endif
					gcs.Delete();
					return 0;
				}
			}
			else
			{
#if defined(VERBOSE)
				printf("ArcGISPRJParser: Error in parsing PROJCS: %s\r\n", &prjBuff[i]);
#endif
				gcs.Delete();
				return 0;
			}
		}
		NN<Math::GeographicCoordinateSystem> nngcs;
		if (cst == Math::CoordinateSystem::CoordinateSystemType::Geographic || Math::IsNAN(falseEasting) || Math::IsNAN(falseNorthing) || Math::IsNAN(centralMeridian) || Math::IsNAN(scaleFactor) || Math::IsNAN(latitudeOfOrigin) || !gcs.SetTo(nngcs))
		{
#if defined(VERBOSE)
			printf("ArcGISPRJParser: Parameter missing in PROJCS\r\n");
#endif
			gcs.Delete();
			return 0;
		}
		parsedSize.Set(i);
		if (srid == 0)
		{
			srid = this->csys.GuessSRIDProj(Text::CStringNN(&prjBuff[nameOfst], nameLen - 2));
		}
		if (cst == Math::CoordinateSystem::CoordinateSystemType::MercatorProjected || cst == Math::CoordinateSystem::CoordinateSystemType::GausskrugerProjected)
		{
			NEW_CLASS(csys, Math::MercatorProjectedCoordinateSystem(sourceName, srid, {&prjBuff[nameOfst], nameLen - 2}, falseEasting, falseNorthing, centralMeridian, latitudeOfOrigin, scaleFactor, nngcs, unit));
			return csys;
		}
		else if (cst == Math::CoordinateSystem::CoordinateSystemType::Mercator1SPProjected)
		{
			NEW_CLASS(csys, Math::Mercator1SPProjectedCoordinateSystem(sourceName, srid, {&prjBuff[nameOfst], nameLen - 2}, falseEasting, falseNorthing, centralMeridian, latitudeOfOrigin, scaleFactor, nngcs, unit));
			return csys;
		}
		else
		{
#if defined(VERBOSE)
			printf("ArcGISPRJParser: Unknown ProjectedCoordinateSystem\r\n");
#endif
			gcs.Delete();
			return 0;
		}
	}

	return 0;
}
