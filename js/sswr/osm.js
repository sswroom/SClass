import sswr from "./sswr.js";
import "./math.js";
sswr.osm = new Object();

sswr.osm.lon2PixelX = function(lon, level, tileSize)
{
	return ((lon + 180.0) / 360.0 * (1 << level)) * tileSize;
}

sswr.osm.lat2PixelY = function(lat, level, tileSize)
{
	return ((1.0 - Math.log(Math.tan(lat * Math.PI / 180.0) + 1.0 / Math.cos(lat * Math.PI / 180.0)) / Math.PI) / 2.0 * (1 << level)) * tileSize;
}

sswr.osm.pixelX2Lon = function(x, level, tileSize)
{
	return x / tileSize * 360.0 / (1 << level) - 180;
}

sswr.osm.pixelY2Lat = function(y, level, tileSize)
{
	var n = Math.PI - 2.0 * Math.PI * y / tileSize / (1 << level);
	return 180.0 / Math.PI * Math.atan(0.5 * (Math.exp(n) - Math.exp(-n)));
}

sswr.osm.tileUrls = function(osmUrl, minCoord, maxCoord, minLev, maxLev)
{
    var tileSize = 256;
    var ret = new Array();
    var minX;
    var minY;
    var maxX;
    var maxY;
    var i;
    var j;
    var url;
    while (minLev <= maxLev)
    {
        minX = Math.floor(sswr.osm.lon2PixelX(minCoord.x, minLev, tileSize) / tileSize);
        minY = Math.floor(sswr.osm.lat2PixelY(maxCoord.y, minLev, tileSize) / tileSize);
        maxX = Math.floor(sswr.osm.lon2PixelX(maxCoord.x, minLev, tileSize) / tileSize);
        maxY = Math.floor(sswr.osm.lat2PixelY(minCoord.y, minLev, tileSize) / tileSize);
        j = minY;
        while (j <= maxY)
        {
            i = minX;
            while (i <= maxX)
            {
                url = osmUrl.replace("{x}", i).replace("{y}", j).replace("{z}", minLev);
                ret.push(url);
                i++;
            }
            j++;
        }
        minLev++;
    }
    return ret;
}

export default sswr;