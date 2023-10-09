import CoordinateSystem from "./CoordinateSystem.js";
import GeographicCoordinateSystem from "./GeographicCoordinateSystem.js";
import unit from "./unit.js";

var ProjectedCoordinateSystem = function(srid, csysName, falseEasting, falseNorthing, dcentralMeridian, dlatitudeOfOrigin, scaleFactor, gcs)
{
	CoordinateSystem.call(this, srid, csysName);
	this.falseEasting = falseEasting;
	this.falseNorthing = falseNorthing;
	this.rcentralMeridian = dcentralMeridian * Math.PI / 180;
	this.rlatitudeOfOrigin = dlatitudeOfOrigin * Math.PI / 180;
	this.scaleFactor = scaleFactor;
	this.gcs = gcs;
};

ProjectedCoordinateSystem.prototype = Object.create(CoordinateSystem.prototype);

ProjectedCoordinateSystem.prototype.calcSurfaceDistance = function(x1, y1, x2, y2, distUnit)
{
	var diffX = x2 - x1;
	var diffY = y2 - y1;
	diffX = diffX * diffX;
	diffY = diffY * diffY;
	var d = Math.sqrt(diffX + diffY);
	if (distUnit != unit.Distance.DistanceUnit.METER)
	{
		d = unit.Distance.convert(unit.Distance.DistanceUnit.METER, distUnit, d);
	}
	return d;
}
export default ProjectedCoordinateSystem;