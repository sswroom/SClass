export function lon2PixelX(lon, level, tileSize)
{
	return ((lon + 180.0) / 360.0 * (1 << level)) * tileSize;
}

export function lat2PixelY(lat, level, tileSize)
{
	return ((1.0 - Math.log(Math.tan(lat * Math.PI / 180.0) + 1.0 / Math.cos(lat * Math.PI / 180.0)) / Math.PI) / 2.0 * (1 << level)) * tileSize;
}

export function pixelX2Lon(x, level, tileSize)
{
	return x / tileSize * 360.0 / (1 << level) - 180;
}

export function pixelY2Lat(y, level, tileSize)
{
	var n = Math.PI - 2.0 * Math.PI * y / tileSize / (1 << level);
	return 180.0 / Math.PI * Math.atan(0.5 * (Math.exp(n) - Math.exp(-n)));
}

export function tileUrls(osmUrl, minCoord, maxCoord, minLev, maxLev)
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
		minX = Math.floor(lon2PixelX(minCoord.x, minLev, tileSize) / tileSize);
		minY = Math.floor(lat2PixelY(maxCoord.y, minLev, tileSize) / tileSize);
		maxX = Math.floor(lon2PixelX(maxCoord.x, minLev, tileSize) / tileSize);
		maxY = Math.floor(lat2PixelY(minCoord.y, minLev, tileSize) / tileSize);
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

export function removeTileUrls(urls, osmUrl, minLev, maxLev, areaList)
{
	if (areaList == null || areaList.length == 0)
		return;
	var urlMap = {};
	var i;
	for (i in urls)
	{
		urlMap[urls[i]] = true;
	}

	var tileSize = 256;
	var minX;
	var minY;
	var maxX;
	var maxY;
	var i;
	var j;
	var k;
	var rect;
	var url;
	while (minLev <= maxLev)
	{
		for (k in areaList)
		{
			rect = areaList[k];
			minX = Math.floor(lon2PixelX(rect.min.x, minLev, tileSize) / tileSize);
			minY = Math.floor(lat2PixelY(rect.max.y, minLev, tileSize) / tileSize);
			maxX = Math.floor(lon2PixelX(rect.max.x, minLev, tileSize) / tileSize);
			maxY = Math.floor(lat2PixelY(rect.min.y, minLev, tileSize) / tileSize);
			j = minY;
			while (j <= maxY)
			{
				i = minX;
				while (i <= maxX)
				{
					url = osmUrl.replace("{x}", i).replace("{y}", j).replace("{z}", minLev);
					delete urlMap[url];
					i++;
				}
				j++;
			}
		}
		minLev++;
	}
	urls.length = 0;
	for (i in urlMap)
	{
		urls.push(i);
	}
}
