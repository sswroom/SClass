import CoordinateSystem from "./CoordinateSystem";
import DatumData from "./DatumData";
var GeographicCoordinateSystem = function(srid, csysName, datumData)
{
	CoordinateSystem.call(this, srid, csysName);
	this.datum = datumData;
};

GeographicCoordinateSystem.prototype = Object.create(CoordinateSystem.prototype);

GeographicCoordinateSystem.prototype.calcSurfaceDistance = function(x1, y1, x2, y2, unit)
{
	return this.datum.spheroid.ellipsoid.calSurfaceDistance(y1, x1, y2, x2, unit);
}
export default GeographicCoordinateSystem;