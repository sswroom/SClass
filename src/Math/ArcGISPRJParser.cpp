#include "Stdafx.h"
#include "IO/FileStream.h"
#include "Math/ArcGISPRJParser.h"
#include "Math/CoordinateSystemManager.h"
#include "Math/Mercator1SPProjectedCoordinateSystem.h"
#include "Math/MercatorProjectedCoordinateSystem.h"

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
	UInt8 buff[512];
	UOSInt buffSize;
	{
		IO::FileStream fs(fileName, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Sequential);
		if (fs.IsError())
		{
			return 0;
		}
		buffSize = fs.Read(Data::ByteArray(buff, 511));
		buff[buffSize] = 0;
	}
	if (buffSize == 511)
		return 0;

	return ParsePRJBuff(fileName, UARR(buff), buffSize, buffSize);
}

Optional<Math::CoordinateSystem> Math::ArcGISPRJParser::ParsePRJBuff(Text::CStringNN sourceName, UnsafeArray<UTF8Char> prjBuff, UOSInt buffSize, OptOut<UOSInt> parsedSize)
{
	UOSInt i;
	UOSInt j;
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
		i = 7;
		if (!ParsePRJString(&prjBuff[i], nameLen))
			return 0;
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
						return 0;
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
				else if (Text::StrStartsWithC(&prjBuff[i], buffSize - i, UTF8STRC("PRIMEM[")))
				{
					i += 7;
					if (!ParsePRJString(&prjBuff[i], j))
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
				else if (Text::StrStartsWithC(&prjBuff[i], buffSize - i, UTF8STRC("UNIT[")))
				{
					i += 5;
					if (!ParsePRJString(&prjBuff[i], j))
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
			const Math::CoordinateSystemManager::DatumInfo *datum = Math::CoordinateSystemManager::GetDatumInfoByName((const UTF8Char*)&prjBuff[datumOfst]);

			Math::GeographicCoordinateSystem::DatumData1 data;
			Math::CoordinateSystemManager::FillDatumData(data, datum, {&prjBuff[datumOfst], datumLen - 2}, ellipsoid, 0);
			NEW_CLASS(csys, Math::GeographicCoordinateSystem(sourceName, srid, {&prjBuff[nameOfst], nameLen - 2}, &data, primem, unit));
			return csys;
		}
		else
		{
			Math::EarthEllipsoid ellipsoid(a, f_1, eet);
			const Math::CoordinateSystemManager::DatumInfo *datum = Math::CoordinateSystemManager::GetDatumInfoByName((const UTF8Char*)&prjBuff[datumOfst]);
			Math::GeographicCoordinateSystem::DatumData1 data;
			Math::CoordinateSystemManager::FillDatumData(data, datum, {&prjBuff[datumOfst], datumLen - 2}, ellipsoid, 0);
			NEW_CLASS(csys, Math::GeographicCoordinateSystem(sourceName, srid, {&prjBuff[nameOfst], nameLen - 2}, &data, primem, unit));
			return csys;
		}
	}
	else if (Text::StrStartsWithC(prjBuff, buffSize, UTF8STRC("PROJCS[")))
	{
		Math::CoordinateSystem::CoordinateSystemType cst = Math::CoordinateSystem::CoordinateSystemType::Geographic;
		Double falseEasting = -1;
		Double falseNorthing = -1;
		Double centralMeridian = -1;
		Double scaleFactor = -1;
		Double latitudeOfOrigin = -1;
		UOSInt nOfst;
		UOSInt nLen;
		UOSInt vOfst;
		Bool commaFound;

		i = 7;
		if (!ParsePRJString(&prjBuff[i], nameLen))
			return 0;
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
						gcs.Delete();
						return 0;
					}
				}
				else if (Text::StrStartsWithC(&prjBuff[i], buffSize - i, UTF8STRC("PARAMETER[")))
				{
					i += 10;
					if (!ParsePRJString(&prjBuff[i], j))
					{
						gcs.Delete();
						return 0;
					}
					nOfst = i + 1;
					nLen = j - 2;
					prjBuff[i + j - 1] = 0;
					i += j;
					if (prjBuff[i] != ',')
					{
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
							gcs.Delete();
							return 0;
						}
						else if (c == ']')
						{
							prjBuff[i] = 0;
							i++;
							if (Text::StrEqualsICaseC(&prjBuff[nOfst], nLen, UTF8STRC("False_Easting")))
							{
								falseEasting = Text::StrToDouble(&prjBuff[vOfst]);
							}
							else if (Text::StrEqualsICaseC(&prjBuff[nOfst], nLen, UTF8STRC("False_Northing")))
							{
								falseNorthing = Text::StrToDouble(&prjBuff[vOfst]);
							}
							else if (Text::StrEqualsICaseC(&prjBuff[nOfst], nLen, UTF8STRC("Central_Meridian")))
							{
								centralMeridian = Text::StrToDouble(&prjBuff[vOfst]);
							}
							else if (Text::StrEqualsICaseC(&prjBuff[nOfst], nLen, UTF8STRC("Scale_Factor")))
							{
								scaleFactor = Text::StrToDouble(&prjBuff[vOfst]);
							}
							else if (Text::StrEqualsICaseC(&prjBuff[nOfst], nLen, UTF8STRC("Latitude_Of_Origin")))
							{
								latitudeOfOrigin = Text::StrToDouble(&prjBuff[vOfst]);
							}
							else
							{
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
								gcs.Delete();
								return 0;
							}
							break;
						}
						else if (c == 0)
						{
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
					gcs.Delete();
					return 0;
				}
			}
			else
			{
				gcs.Delete();
				return 0;
			}
		}
		NN<Math::GeographicCoordinateSystem> nngcs;
		if (cst == Math::CoordinateSystem::CoordinateSystemType::Geographic || falseEasting == -1 || falseNorthing == -1 || centralMeridian == -1 || scaleFactor == -1 || latitudeOfOrigin == -1 || !gcs.SetTo(nngcs))
		{
			gcs.Delete();
			return 0;
		}
		parsedSize.Set(i);
		srid = this->csys.GuessSRIDProj(Text::CStringNN(&prjBuff[nameOfst], nameLen - 2));
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
			gcs.Delete();
			return 0;
		}
	}

	return 0;
}
