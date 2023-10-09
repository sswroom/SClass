import geometry from "./math/geometry.js";
import unit from "./math/unit.js";
import Coord2D from "./math/Coord2D.js";
import CoordinateSystem from "./math/CoordinateSystem.js";
import CoordinateSystemManager from "./math/CoordinateSystemManager.js";
import DatumData from "./math/DatumData.js";
import EarthEllipsoid from "./math/EarthEllipsoid.js";
import GeographicCoordinateSystem from "./math/GeographicCoordinateSystem.js";
import MercatorProjectedCoordinateSystem from "./math/MercatorProjectedCoordinateSystem.js";
import ProjectedCoordinateSystem from "./math/ProjectedCoordinateSystem.js";
import Spheroid from "./math/Spheroid.js";

var math = {
	roundToFloat: function(n, decimalPoints)
	{
		if (decimalPoints === undefined) {
			decimalPoints = 0;
		}
	
		var multiplicator = Math.pow(10, decimalPoints);
		n = parseFloat((n * multiplicator).toFixed(11));
		return Math.round(n) / multiplicator;
	},
	
	roundToStr: function(n, decimalPoints)
	{
		if (decimalPoints === undefined) {
			decimalPoints = 0;
		}
	
		var multiplicator = Math.pow(10, decimalPoints);
		n = parseFloat((n * multiplicator).toFixed(11));
		var s = "" + Math.round(n);
		if (decimalPoints == 0)
		{
			return s;
		}
		else if (s.length > decimalPoints)
		{
			return s.substring(0, s.length - decimalPoints) + "." + s.substring(s.length - decimalPoints);
		}
		else
		{
			return "0."+("0".repeat(decimalPoints - s.length)) + s;
		}
	},
	GeoJSON: {
		parseGeometry: function(srid, geom)
		{
			if (geom.type == "Polygon")
			{
				return new geometry.Polygon(srid, geom.coordinates);
			}
			else if (geom.type == "MultiPolygon")
			{
				return new geometry.MultiPolygon(srid, geom.coordinates);
			}
			else
			{
				console.log("GeoJSON.parseGeometry: unknown type "+geom.type);
			}
			return null;
		}
	},
	geometry: geometry,
	unit: unit,
	Coord2D: Coord2D,
	CoordinateSystem: CoordinateSystem,
	CoordinateSystemManager: CoordinateSystemManager,
	DatumData: DatumData,
	EarthEllipsoid: EarthEllipsoid,
	GeographicCoordinateSystem: GeographicCoordinateSystem,
	MercatorProjectedCoordinateSystem: MercatorProjectedCoordinateSystem,
	ProjectedCoordinateSystem: ProjectedCoordinateSystem,
	Spheroid: Spheroid
};

export default math;