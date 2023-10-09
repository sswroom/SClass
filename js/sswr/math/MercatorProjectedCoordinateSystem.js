import ProjectedCoordinateSystem from "./ProjectedCoordinateSystem";
import GeographicCoordinateSystem from "./GeographicCoordinateSystem";
var MercatorProjectedCoordinateSystem = function(srid, csysName, falseEasting, falseNorthing, dcentralMeridian, dlatitudeOfOrigin, scaleFactor, gcs)
{
	ProjectedCoordinateSystem.call(this, srid, csysName, falseEasting, falseNorthing, dcentralMeridian, dlatitudeOfOrigin, scaleFactor, gcs);
}

MercatorProjectedCoordinateSystem.prototype = Object.create(math.ProjectedCoordinateSystem.prototype);
export default MercatorProjectedCoordinateSystem;