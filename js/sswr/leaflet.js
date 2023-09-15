import sswr from "./sswr.js";
import "./math.js";
sswr.leaflet = new Object();
sswr.leaflet.fromLatLon = function(latLon)
{
    return new sswr.math.Coord2D(latLon.lng, latLon.lat);
}

export default sswr;