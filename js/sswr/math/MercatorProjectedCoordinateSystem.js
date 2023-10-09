import ProjectedCoordinateSystem from "./ProjectedCoordinateSystem.js";
var MercatorProjectedCoordinateSystem = function(srid, csysName, falseEasting, falseNorthing, dcentralMeridian, dlatitudeOfOrigin, scaleFactor, gcs)
{
	ProjectedCoordinateSystem.call(this, srid, csysName, falseEasting, falseNorthing, dcentralMeridian, dlatitudeOfOrigin, scaleFactor, gcs);
}

MercatorProjectedCoordinateSystem.prototype = Object.create(ProjectedCoordinateSystem.prototype);
export default MercatorProjectedCoordinateSystem;