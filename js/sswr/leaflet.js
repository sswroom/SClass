import math from "./math.js";
var leaflet = {};
leaflet.fromLatLon = function(latLon)
{
    return new math.Coord2D(latLon.lng, latLon.lat);
}

export default leaflet;