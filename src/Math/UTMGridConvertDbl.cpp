#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Math/UTMGridConvertDbl.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"

Math::UTMGridConvertDbl::UTMGridConvertDbl()
{
	this->eccentricity = 0.081819190835646865372868616717538;
	this->radius = 6378137;
}

Math::UTMGridConvertDbl::~UTMGridConvertDbl()
{
}

UTF8Char *Math::UTMGridConvertDbl::WGS84_Grid(UTF8Char *gridStr, Int32 digitCnt, Int32 *lonZone, Char *latZone, Double *northing, Double *easting, Double lat, Double lon)
{
	Double k0 = 0.9996;
	Double deg2rad = Math::PI / 180.0;

	Double longOrigin;
	Double eccPrimeSquared;
	Double n;
	Double t;
	Double c;
	Double a;
	Double m;

	if (lat < -80 || lat > 84 || !Math::IsRealNum(lat))
	{
		return Text::StrConcat(gridStr, (const UTF8Char*)"Out of area");
	}
	Double longTemp = (lon + 180) - (Int32)((lon + 180) / 360) * 360 - 180; // -180.00 .. 179.9;

	Double latRad = lat * deg2rad;
	Double longRad = longTemp * deg2rad;
	Double longOriginRad;
	Int32 zoneNumber;
	
	Double sinLat = Math::Sin(latRad);
	Double cosLat = Math::Cos(latRad);
	Double tanLat = sinLat / cosLat;
	Double e2 = this->eccentricity * this->eccentricity;
	Double e4 = e2 * e2;
	Double e6 = e4 * e2;

	zoneNumber = (Int32)((longTemp + 180) / 6) + 1;

	if (lat >= 56.0 && lat < 64.0 && longTemp >= 3.0 && longTemp < 12.0)
	{
		zoneNumber = 32;
	}

	if (lat >= 72.0 && lat < 84.0)
	{
		if (longTemp >= 0.0  && longTemp <  9.0 )
			zoneNumber = 31;
		else if (longTemp >= 9.0 && longTemp < 21.0)
			zoneNumber = 33;
		else if (longTemp >= 21.0 && longTemp < 33.0)
			zoneNumber = 35;
		else if (longTemp >= 33.0 && longTemp < 42.0)
			zoneNumber = 37;
	}
	longOrigin = (zoneNumber - 1) * 6 - 180 + 3;
	longOriginRad = longOrigin * deg2rad;

	eccPrimeSquared = e2 / (1 - e2);

	n = this->radius / Math::Sqrt(1 - e2 * sinLat * sinLat);
	t = tanLat * tanLat;
	c = eccPrimeSquared * cosLat * cosLat;
	a = cosLat * (longRad - longOriginRad);

	m = this->radius * ((1 - e2 / 4 - 3 * e4 / 64 - 5 * e6 / 256) * latRad
		- (3 * e2 / 8 + 3 * e4 / 32 + 45 * e6 / 1024) * Math::Sin(2 * latRad)
		+ (15 * e4 / 256 + 45 * e6 / 1024) * Math::Sin(4 * latRad)
		- (35 * e6 / 3072) * Math::Sin(6 * latRad));

	Double utmEasting = (k0 * n * (a + (1 - t + c) * a * a * a/6
		+ (5 - 18 * t + t * t + 72 * c - 58 * eccPrimeSquared) * a * a * a * a * a / 120)
		+ 500000.0);

	Double utmNorthing = (k0 * (m + n * tanLat * (a * a / 2 + (5 - t + 9 * c + 4 * c * c) * a * a * a * a / 24
		+ (61 - 58 * t + t * t + 600 * c - 330 * eccPrimeSquared) * a * a * a * a * a * a / 720)));
	if(lat < 0)
		utmNorthing += 10000000.0; //10000000 meter offset for southern hemisphere
	
	const Char *letters = "ABCDEFGHJKLMNPQRSTUVWXYZ";
	
	Int32 ieast = (Int32)utmEasting;
	Int32 inorth = (Int32)utmNorthing;
	if (ieast < 0)
		ieast = -ieast;
	if (inorth < 0)
		inorth = -inorth;
	
	if (lonZone)
		*lonZone = zoneNumber;
	if (latZone)
		*latZone = letters[(((Int32)lat) >> 3) + 12];
	Int32 pos = (ieast / 100000) - 1 + (((zoneNumber + 2) % 3) << 3);
	Char eastZone = letters[pos];
	pos = (inorth / 100000) % 20;
	if ((zoneNumber & 1) == 0)
	{
		pos = (pos + 5) % 20;
	}
	Char northZone = letters[pos];
	if (easting)
		*easting = utmEasting;
	if (northing)
		*northing = utmNorthing;

	if (gridStr)
	{
		UTF8Char *sptr = Text::StrInt32(gridStr, zoneNumber);
		*sptr++ = (UTF8Char)letters[(((Int32)lat) >> 3) + 12];
		*sptr++ = (UTF8Char)eastZone;
		*sptr++ = (UTF8Char)northZone;

		if (digitCnt <= 3)
		{
			digitCnt = 3;
			inorth /= 100;
			ieast /= 100;
		}
		else if (digitCnt == 4)
		{
			digitCnt = 4;
			inorth /= 10;
			ieast /= 10;
		}
		else
		{
			digitCnt = 5;
		}
		
		Int32 i;
		sptr += digitCnt << 1;
		i = digitCnt;
		while (i-- > 0)
		{
			*--sptr = (UTF8Char)((inorth % 10) + 0x30);
			inorth /= 10;
		}
		i = digitCnt;
		while (i-- > 0)
		{
			*--sptr = (UTF8Char)((ieast % 10) + 0x30);
			ieast /= 10;
		}
		sptr += digitCnt << 1;
		*sptr = 0;
		return sptr;
	}
	else
	{
		return 0;
	}
}

Bool Math::UTMGridConvertDbl::Grid_WGS84(Double *latOut, Double *lonOut, const UTF8Char *grid)
{
	Double e2 = this->eccentricity * this->eccentricity;
	Double k0 = 0.9996;
	Double rad2deg = 180.0 / Math::PI;

	UInt32 zoneNumber;
	UOSInt latZone;
	Double easting;
	Double northing;

	Double c1;
	Double n1;
	Double r1;
	Double t1;
	Double d;
	Double lat;
	Double lon;

	UOSInt eastZone;
	UOSInt northZone;
	
	const UTF8Char *letters = (const UTF8Char*)"ABCDEFGHJKLMNPQRSTUVWXYZ";
	UTF8Char ch;
	
	UOSInt i;
	Int32 v;
	zoneNumber = 0;
	while (true)
	{
		ch = *grid++;
		if (ch < 48 || ch >= 58)
			break;
		zoneNumber = zoneNumber * 10 + (UInt32)(ch - 0x30);
	}
	latZone = Text::StrIndexOf(letters, ch);
	eastZone = Text::StrIndexOf(letters, *grid++);
	northZone = Text::StrIndexOf(letters, *grid++);;
	if (latZone == INVALID_INDEX || eastZone == INVALID_INDEX || northZone == INVALID_INDEX)
		return false;

	i = Text::StrCharCnt(grid);
	if (i == 6)
	{
		v = Text::StrToInt32(grid);
		easting = (v / 1000) * 100 + 50;
		northing = (v % 1000) * 100 + 50;
	}
	else if (i == 8)
	{
		v = Text::StrToInt32(grid);
		easting = (v / 10000) * 10 + 5;
		northing = (v % 10000) * 10 + 5;
	}
	else if (i == 10)
	{
		easting = 0.5 + (Int32)(Text::StrToDouble(grid) * 0.00001);
		northing = 0.5 + Text::StrToDouble(&grid[5]);
	}
	else
	{
		return false;
	}
	easting += Math::UOSInt2Double((eastZone - (((zoneNumber + 2) % 3) << 3) + 1) * 100000);
	if ((zoneNumber & 1) == 0)
	{
		northZone = (northZone + 15) % 20;
	}
	northing += Math::UOSInt2Double(northZone * 100000);
	if (latZone < 12)
	{
		northing -= 10000000.0;
	}
	
	Double longOrigin = (zoneNumber - 1) * 6 - 180 + 3;
	Double eccPrimeSquared = e2 / (1 - e2);
	Double e1 = (1 - Math::Sqrt(1 - e2)) / (1 + Math::Sqrt(1 - e2));
	i = 10;
	while (i-- > 0)
	{
		Double m = northing / k0;
		Double mu = m / (this->radius * (1 - e2 / 4 - 3 * e2 * e2 / 64 - 5 * e2 * e2 * e2 / 256));

		Double phi1Rad = mu + (3 * e1 / 2 - 27 * e1 * e1 * e1 / 32) * Math::Sin(2 * mu)
			+ (21 * e1 * e1 / 16 - 55 * e1 * e1 * e1 * e1 / 32) * Math::Sin(4 * mu)
			+ (151 * e1 * e1 * e1 / 96) * Math::Sin(6 * mu);
//		phi1 = phi1Rad * rad2deg;

		n1 = this->radius / Math::Sqrt(1 - e2 * Math::Sin(phi1Rad) * Math::Sin(phi1Rad));
		t1 = Math::Tan(phi1Rad) * Math::Tan(phi1Rad);
		c1 = eccPrimeSquared * Math::Cos(phi1Rad) * Math::Cos(phi1Rad);
		r1 = this->radius * (1 - e2) / Math::Pow(1 - e2 * Math::Sin(phi1Rad) * Math::Sin(phi1Rad), 1.5);
		d = (easting - 500000.0) / (n1 * k0);

		lat = phi1Rad - (n1 * Math::Tan(phi1Rad) / r1) * (d * d / 2 - (5 + 3 * t1 + 10 * c1 - 4 * c1 * c1 - 9 * eccPrimeSquared) * d * d * d * d / 24
			+ (61 + 90 * t1 + 298 * c1 + 45 * t1 * t1 - 252 * eccPrimeSquared - 3 * c1 * c1) * d * d * d * d * d * d / 720);
		lat = lat * rad2deg;

		lon = (d - (1 + 2 * t1 + c1) * d * d * d / 6 + (5 - 2 * c1 + 28 * t1 - 3 * c1 * c1 + 8 * eccPrimeSquared + 24 * t1 * t1) * d *d * d * d * d / 120) / Math::Cos(phi1Rad);
		lon = longOrigin + lon * rad2deg;
		
		if (latZone == (UInt32)(((Int32)lat) >> 3) + 12)
			break;
		northing += 2000000.0;
	}
	*latOut = lat;
	*lonOut = lon;
	return true;
}

Bool Math::UTMGridConvertDbl::Grid_WGS84(Double *latOut, Double *lonOut, Int32 lonZone, Char latZone, Double northing, Double easting)
{
	Double e2 = this->eccentricity * this->eccentricity;
	Double k0 = 0.9996;
	Double rad2deg = 180.0 / Math::PI;

	Double c1;
	Double n1;
	Double r1;
	Double t1;
	Double d;
	Double lat;
	Double lon;


	if (latZone < 12)
	{
		northing -= 10000000.0;
	}
	
	Double longOrigin = (lonZone - 1) * 6 - 180 + 3;
	Double eccPrimeSquared = e2 / (1 - e2);
	Double e1 = (1 - Math::Sqrt(1 - e2)) / (1 + Math::Sqrt(1 - e2));

	Double m = northing / k0;
	Double mu = m / (this->radius * (1 - e2 / 4 - 3 * e2 * e2 / 64 - 5 * e2 * e2 * e2 / 256));

	Double phi1Rad = mu + (3 * e1 / 2 - 27 * e1 * e1 * e1 / 32) * Math::Sin(2 * mu)
		+ (21 * e1 * e1 / 16 - 55 * e1 * e1 * e1 * e1 / 32) * Math::Sin(4 * mu)
		+ (151 * e1 * e1 * e1 / 96) * Math::Sin(6 * mu);
//		phi1 = phi1Rad * rad2deg;

	n1 = this->radius / Math::Sqrt(1 - e2 * Math::Sin(phi1Rad) * Math::Sin(phi1Rad));
	t1 = Math::Tan(phi1Rad) * Math::Tan(phi1Rad);
	c1 = eccPrimeSquared * Math::Cos(phi1Rad) * Math::Cos(phi1Rad);
	r1 = this->radius * (1 - e2) / Math::Pow(1 - e2 * Math::Sin(phi1Rad) * Math::Sin(phi1Rad), 1.5);
	d = (easting - 500000.0) / (n1 * k0);

	lat = phi1Rad - (n1 * Math::Tan(phi1Rad) / r1) * (d * d / 2 - (5 + 3 * t1 + 10 * c1 - 4 * c1 * c1 - 9 * eccPrimeSquared) * d * d * d * d / 24
		+ (61 + 90 * t1 + 298 * c1 + 45 * t1 * t1 - 252 * eccPrimeSquared - 3 * c1 * c1) * d * d * d * d * d * d / 720);
	lat = lat * rad2deg;

	lon = (d - (1 + 2 * t1 + c1) * d * d * d / 6 + (5 - 2 * c1 + 28 * t1 - 3 * c1 * c1 + 8 * eccPrimeSquared + 24 * t1 * t1) * d *d * d * d * d / 120) / Math::Cos(phi1Rad);
	lon = longOrigin + lon * rad2deg;
	
	*latOut = lat;
	*lonOut = lon;
	return true;
}

/*
	this->originNorthing = 0;
	this->originEasting = 500000;
	this->originLatitude = 0;
	this->originLongitude = 117.0;
	this->meridianScale = 0.9996;
	this->majorEllipsoid = 6378137.0;
	this->minorEllipsoid = 6356752.3142;
Double Math::ProjectionConvertDbl::CalMeridian(Double latRad)
{
	Double e4;
	Double meridianDist;
	Double a1 = this->majorEllipsoid * this->meridianScale;
	Double b1 = this->minorEllipsoid * this->meridianScale;
	Double e2 = ((a1 * a1) - (b1 * b1)) / (a1 * a1);

	e4 = e2 * e2;

	meridianDist = (latRad - e2 / 4 - 4 * e4 / 64) * latRad;
	meridianDist -= (e2 + e4 / 4) * 3 / 8 * Math::Sin(2 * latRad);
	meridianDist += e4 * 15 / 256 * Math::Sin(4 * latRad);
	meridianDist *= this->majorEllipsoid;
	return meridianDist;
}

WChar *Math::ProjectionConvertDbl::CalGrid(WChar *gridStr, Int32 digits, Double *northing, Double *easting, Double latDegree, Double lonDegree)
{
	Double a1 = this->majorEllipsoid * this->meridianScale;
	Double b1 = this->minorEllipsoid * this->meridianScale;
	Double n1 = (a1 - b1) / (a1 + b1);
	Double n2 = n1 * n1;
	Double n3 = n2 * n1;
	Double e2 = ((a1 * a1) - (b1 * b1)) / (a1 * a1);

	Double latRad = latDegree * Math::PI / 180.0;
	Double lonRad = lonDegree * Math::PI / 180.0;
	Double sinLat = Math::Sin(latRad);
	Double cosLat = Math::Cos(latRad);
	Double tanLat = sinLat / cosLat;
	Double tanLat2 = tanLat * tanLat;
	Double cosLat2 = cosLat * cosLat;
	Double cosLat3 = cosLat2 * cosLat;
	Double k3 = latRad - (this->originLatitude * Math::PI / 180.0);
	Double k4 = latRad + (this->originLatitude * Math::PI / 180.0);


	Int32 merid = ((Int32)lonDegree / 6) * 6 + 3;
	if (latDegree >= 72 && lonDegree >= 0)
	{
		if (lonDegree < 9)
			merid = 3;
		else if (lonDegree < 21)
			merid = 15;
		else if (lonDegree < 33)
			merid = 27;
		else if (lonDegree < 42)
			merid = 39;
	}
	if (latDegree >= 56 && latDegree < 64 && lonDegree >= 3 && lonDegree < 12)
		merid = 9;
	this->originLongitude = merid;

	Double mtmp1 = k3 * (1 + n1 + 1.25 * (n2 + n3));
	Double mtmp2 = Math::Sin(k3) * Math::Cos(k4) * (3 * (n1 + n2 + 0.875 * n3));
	Double mtmp3 = Math::Sin(2 * k3) * Math::Cos(2 * k4) * (1.875 * (n2 + n3));
	Double mtmp4 = Math::Sin(3 * k3) * Math::Cos(3 * k4) * 35 / 24 * n3;
	Double m = b1 * (mtmp1 - mtmp2 + mtmp3 - mtmp4);

	Double temp = 1 - e2 * sinLat * sinLat;
	Double v = a1 / Math::Sqrt(temp);
	Double r = v * (1 - e2) / temp;
	Double h2 = v / r - 1.0;

	Double p = lonRad - this->originLongitude * Math::PI / 180.0;
	Double p2 = p * p;
	Double p4 = p2 * p2;

	Double ntmp1 = m;
	Double ntmp2 = v * 0.5 * sinLat * cosLat;
	Double ntmp3 = v / 24 * sinLat * cosLat3 * (5 - tanLat2 + 9 * h2);
	Double ntmp4 = v / 720 * sinLat * cosLat3 * cosLat2 * (61 - 58 * tanLat2 + tanLat2 * tanLat2);
	*northing = this->originNorthing + ntmp1 + p2 * ntmp2 + p4 * ntmp3 + p4 * p2 * ntmp4;

	Double etmp1 = v * cosLat;
	Double etmp2 = v / 6 * cosLat3 * (v / r - tanLat2);
	Double etmp3 = v / 120 * cosLat3 * cosLat2 * (5 - 18 * tanLat2 + tanLat2 * tanLat2 + 14 * h2 - 58 * tanLat2 * h2);
	*easting = this->originEasting + p * etmp1 + p2 * p * etmp2 + p4 * p * etmp3;

	if (gridStr)
	{
		WChar *sptr;
		sptr = gridStr;
		Int32 zoneId = (merid - 3) / 6 + 31;
		if (latDegree < -80 || latDegree > 84 || *easting < 100000 || *easting > 899999)
		{
			*sptr = 0;
			return sptr;
		}

		Int32 ieast = (Int32)*easting;
		Int32 inorth = (Int32)*northing;
		if (ieast < 0)
			ieast = -ieast;
		if (inorth < 0)
			inorth = -inorth;
		WChar letters[] = L"ABCDEFGHJKLMNPQRSTUVWXYZ";
		sptr = Text::StrInt32(sptr, zoneId);
		*sptr++ = letters[(((Int32)latDegree) >> 3) + 12];
		if (sptr[-1] > 'X')
		{
			sptr[-1] = 'X';
		}
		
		Int32 pos = ((ieast / 100000) % 10) - 1 + (((zoneId + 2) % 3) << 3);
		*sptr++ = letters[pos];

		pos = (inorth / 100000) % 20;
		if ((zoneId & 1) == 0)
		{
			pos = (pos + 5) % 20;
		}
		*sptr++ = letters[pos];

		if (digits <= 3)
		{
			digits = 3;
			inorth /= 100;
			ieast /= 100;
		}
		else if (digits == 4)
		{
			inorth /= 10;
			ieast /= 10;
		}
		else
		{
			digits = 5;
		}

		Int32 i;
		sptr += digits << 1;
		i = digits;
		while (i-- > 0)
		{
			*--sptr = (inorth % 10) + 0x30;
			inorth = inorth / 10;
		}
		i = digits;
		while (i-- > 0)
		{
			*--sptr = (ieast % 10) + 0x30;
			ieast = ieast / 10;
		}
		sptr += digits << 1;
		*sptr = 0;
		return sptr;
	}
	else
	{
		return 0;
	}
}
*/
