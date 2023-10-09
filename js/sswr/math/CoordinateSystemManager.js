import CoordinateSystem from "./CoordinateSystem";
import EarthEllipsoid from "./EarthEllipsoid";
import unit from "./unit";
import Spheroid from "./Spheroid";

var CoordinateSystemManager = {
	srCreateGeogCSys: function(srid, datumSrid, name)
	{
		var data = this.srGetDatumData(datumSrid);
		if (data == null)
		{
			return null;
		}
		return new math.GeographicCoordinateSystem(srid, name, data);
	},
	
	srCreateProjCSys: function(srid, geogcsSrid, csysType, projName, falseEasting, falseNorthing, centralMeridian, latitudeOfOrigin, scaleFactor)
	{
		var gcsys = this.srCreateGeogCSys(geogcsSrid);
		if (gcsys == null)
			return null;
		if (csysType == CoordinateSystem.CoordinateSystemType.MercatorProjected || csysType == CoordinateSystem.CoordinateSystemType.GausskrugerProjected)
		{
			return new math.MercatorProjectedCoordinateSystem(srid, projName, falseEasting, falseNorthing, centralMeridian, latitudeOfOrigin, scaleFactor, gcsys);
		}
		else if (csysType == CoordinateSystem.CoordinateSystemType.Mercator1SPProjected)
		{
			return new math.Mercator1SPProjectedCoordinateSystem(srid, projName, falseEasting, falseNorthing, centralMeridian, latitudeOfOrigin, scaleFactor, gcsys);
		}
		return null;
	},
	
	srCreateCsys: function(srid)
	{
		switch (srid)
		{
		case 2326:
		case 102140:
			return this.srCreateProjCSys(srid, 4611, CoordinateSystem.CoordinateSystemType.MercatorProjected, "Hong Kong 1980 Grid System", 836694.05, 819069.80, 114.17855555555555555555555555556, 22.312133333333333333333333333333, 1);
		case 3857:
		case 900913:
			return this.srCreateProjCSys(srid, 4326, CoordinateSystem.CoordinateSystemType.Mercator1SPProjected, "WGS 84 / Pseudo-Mercator", 0, 0, 0, 0, 1);
		case 4326:
			return this.srCreateGeogCSys(srid, 6326, "WGS 84");
		case 4611:
			return this.srCreateGeogCSys(srid, 6611, "Hong Kong 1980");
		default:
			return null;
		}
	},
	
	srGetDatumData: function(srid)
	{
		switch (srid)
		{
		case 6326:
			return new math.DatumData(6326, this.srGetSpheroid(7030), "WGS_1984", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, unit.Angle.AngleUnit.RADIAN);
		case 6600:
			return new math.DatumData(6600, this.srGetSpheroid(7012), "Anguilla_1957", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, unit.Angle.AngleUnit.RADIAN);
		case 6601:
			return new math.DatumData(6601, this.srGetSpheroid(7012), "Antigua_1943", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, unit.Angle.AngleUnit.RADIAN);
		case 6602:
			return new math.DatumData(6602, this.srGetSpheroid(7012), "Dominica_1945", 0, 0, 0, 725, 685, 536, 0, 0, 0, 0, unit.Angle.AngleUnit.ARCSECOND);
		case 6603:
			return new math.DatumData(6603, this.srGetSpheroid(7012), "Grenada_1953", 0, 0, 0, 72, 213.7, 93, 0, 0, 0, 0, unit.Angle.AngleUnit.ARCSECOND);
		case 6611:
			return new math.DatumData(6611, this.srGetSpheroid(7022), "Hong_Kong_1980", 0, 0, 0, -162.619, -276.959, -161.764, 0.067753, -2.24365, -1.15883, -1.09425, unit.Angle.AngleUnit.ARCSECOND);
		}
		return null;
	},
	
	srGetSpheroid: function(srid)
	{
		switch (srid)
		{
		case 7012:
			return new Spheroid(7012, new EarthEllipsoid(null, null, EarthEllipsoid.EarthEllipsoidType.CLARKE1880), "Clarke 1880 (RGS)");
		case 7022:
			return new Spheroid(7022, new EarthEllipsoid(null, null, EarthEllipsoid.EarthEllipsoidType.INTL1924),   "International 1924");
		case 7030:
			return new Spheroid(7030, new EarthEllipsoid(null, null, EarthEllipsoid.EarthEllipsoidType.WGS84),      "WGS 84");
		}
	}	
};
export default CoordinateSystemManager;
