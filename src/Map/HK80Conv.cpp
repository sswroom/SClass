#include "Stdafx.h"
#include "Math/Math_C.h"
#include <stdlib.h>
#include <math.h>
#include "Map/HK80Conv.h"

#define Lat0Deg 22.0
#define Lat0Min 18.0
#define Lat0Sec 43.68

#define Lng0Deg 114.0
#define Lng0Min 10.0
#define Lng0Sec 42.8

#define LAT0 22.31213333333333
#define LNG0 114.1785555555556

#define adjHK80toWGS84LAT -0.00152777777777778
#define adjHK80toWGS84LNG 0.00244444444444444

#define N0 819069.80
#define E0 836694.05

#define PI 3.14159265358979323846
#define ToRadians(deg) (deg * PI / 180.0)
#define ToDegrees(radian) ((radian) * 180.0 / PI)
Double Map::HK80Conv::calcM(Double lat)
{
	return a *((A0 * lat)- (A2 * sin(2 * lat)) + (A4 * sin(4 * lat)) - (A6 * sin(6 * lat)));
}

Double Map::HK80Conv::getLatp(Double M)
{
	Double maxLat = ToRadians(23.0);
	Double minLat = ToRadians(22.0);
	Double latp = 0;
	Int32 i = 0;
	Bool isSuccess = false;
	while(i < 200)
	{
		Double tryLat = (maxLat + minLat) / 2.0;
		Double tryM = calcM(tryLat);

		if (Math_Abs(M - tryM) < 0.000005)
		{
			latp = tryLat;
			isSuccess = true;
			break;
		}
		else if (M > tryM)
		{
			minLat = tryLat;
		}
		else
		{
			maxLat = tryLat;
		}

		i++;
	}			
	return latp;
}

Double Map::HK80Conv::gett(Double lat)
{
	return tan(lat);
}

Double Map::HK80Conv::getv(Double lat)
{
	return a / pow(1.0 - e2 * pow( sin(lat), 2.0), 1.0 / 2.0);
}
Double Map::HK80Conv::getp(Double lat)
{
	return a * ( 1.0 - e2) / pow(1.0 - e2 * pow(sin(lat), 2.0), 3.0 / 2.0); 
}

Double Map::HK80Conv::getpsi(Double v, Double p)
{
	return (v / p); 
}	

Double Map::HK80Conv::getM(Double HK80N)
{
	return (HK80N -N0 + M0) / m0;	
}

Map::HK80Conv::HK80Conv()
{
	a  = 6378388.0;
	f  = 297.0;
	e2  = 2.0 * 1.0 / f - (1.0 / (f * f));
	e4 = e2 * e2;
	e6 = e4 * e2;
							
	A0 = 1.0 - e2 / 4.0 - 3.0 * e4 / 64.0 - 5.0 * e6 / 256.0;
	A2 = 3.0 / 8.0 * (e2 + e4 / 4.0 + 15.0 * e6 / 128.0); 
	A4 = 15.0 / 256.0 * (e4 + 3 * e6 / 4.0);
	A6 = 35.0 * e6 / 3072.0;
	
	m0 = 1.0;
	M0 = calcM(ToRadians(LAT0));
}

Map::HK80Conv::~HK80Conv()
{
}

void Map::HK80Conv::ToWGS84(Double *pt_hk80, Double *pt_wgs84, Int32 ptCnt)
{
	Double psi;
	Double M;
	Double latp;

	Double t;
	Double p;
	Double v;

	while (ptCnt-- > 0)
	{
		M = getM(pt_hk80[0]);
		latp = getLatp(M);

		t = gett(latp);
		v = getv(latp);			
		p = getp(latp);

		psi = getpsi(v,p);
		Double dE = pt_hk80[1] - E0;
		
		Double Fs  = psi;
		Double t2 = t * t;
		Double t4 = t2 * t2;
		Double t6 = t4 * t2;
		

		//Eq5
		Double p31 = 8 * pow(Fs, 4) * (11 - 24 * t2);
		Double p32 = 12 * pow(Fs, 3) * (21 - 71 * t2);
		Double p33 = 15 * (Fs * Fs) * (15 - 98 * t2 + 15 * t4);
		Double p34 = 180 * Fs * (5 * t2 - 3 * t4);
		Double p35 = 360 * t4;
		
		Double mf = m0;
		Double Ps = p;
		Double Vs = v;
		Double p0 = t / mf / Ps;
		Double p1 = p0 * ((dE * dE) / 2.0 / mf / Vs);
		Double p2 = p0 * (pow(dE, 4.0) / 24.0 / pow(mf , 3.0) / pow( Vs , 3.0)) * (-4.0 * (Fs * Fs) + 9.0 * Fs * (1.0 - t2) + 12 * t2);
		Double p3 = p0 * (pow(dE , 6.0) / 720.0 / pow(mf , 5.0) / pow( Vs , 5.0)) * (p31 - p32 + p33 + p34 + p35);
		Double p4 = p0 * (pow(dE , 8.0 )/ 40320.0 / pow(mf , 7.0) / pow( Vs , 7.0)) * (13856.0 + 3633.0 * t2 + 4095.0 * t4 + 1575.0 * t6);
		pt_wgs84[0] = (ToDegrees(latp - p1 + p2 - p3 + p4)) + adjHK80toWGS84LAT;
		
		//Eq4
		Double secLatp = 1.0 / cos(latp);

		p31 = 4.0 * pow(Fs, 3.0) * (1 - 6.0 * t2);
		p32 = pow(Fs ,2.0) * (9 - 68.0 * t2);
		p33 = 72.0 * Fs * t2;
		p34 = 24.0 * t4;

		p1 = secLatp * (dE / mf / Vs);
		p2 = secLatp * (pow(dE , 3) / 6.0 / pow(mf , 3) / pow(Vs , 3)) * (Fs + 2 * t2);
		p3 = secLatp * (pow(dE , 5) / 120.0 / pow(mf , 5) / pow(Vs , 5)) * (-p31 + p32 + p33 + p34);
		p4 = secLatp * (pow(dE , 7) / 5040.0 / pow(mf , 7) / pow(Vs , 7)) * (61 + 662 * t2 + 1320 * t4 + 720 * t6);

		pt_wgs84[1] = ToDegrees( ToRadians(LNG0) + p1 - p2 + p3 - p4) + adjHK80toWGS84LNG;

		pt_hk80 += 2;
		pt_wgs84 += 2;
	}

	return;
}
