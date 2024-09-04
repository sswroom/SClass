import * as math from "./math.js";

/**
 * @param {number} lon
 * @param {number} level
 * @param {number} tileSize
 */
export function lon2PixelX(lon, level, tileSize)
{
	return ((lon + 180.0) / 360.0 * (1 << level)) * tileSize;
}

/**
 * @param {number} lat
 * @param {number} level
 * @param {number} tileSize
 */
export function lat2PixelY(lat, level, tileSize)
{
	return ((1.0 - Math.log(Math.tan(lat * Math.PI / 180.0) + 1.0 / Math.cos(lat * Math.PI / 180.0)) / Math.PI) / 2.0 * (1 << level)) * tileSize;
}

/**
 * @param {number} x
 * @param {number} level
 * @param {number} tileSize
 */
export function pixelX2Lon(x, level, tileSize)
{
	return x / tileSize * 360.0 / (1 << level) - 180;
}

/**
 * @param {number} y
 * @param {number} level
 * @param {number} tileSize
 */
export function pixelY2Lat(y, level, tileSize)
{
	let n = Math.PI - 2.0 * Math.PI * y / tileSize / (1 << level);
	return 180.0 / Math.PI * Math.atan(0.5 * (Math.exp(n) - Math.exp(-n)));
}

/**
 * @param {string} osmUrl
 * @param {math.Coord2D} minCoord
 * @param {math.Coord2D} maxCoord
 * @param {number} minLev
 * @param {number} maxLev
 */
export function tileUrls(osmUrl, minCoord, maxCoord, minLev, maxLev)
{
	let tileSize = 256;
	let ret = new Array();
	let minX;
	let minY;
	let maxX;
	let maxY;
	let i;
	let j;
	let url;
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
				url = osmUrl.replace("{x}", ""+i).replace("{y}", ""+j).replace("{z}", ""+minLev);
				ret.push(url);
				i++;
			}
			j++;
		}
		minLev++;
	}
	return ret;
}

/**
 * @param {string[]} urls
 * @param {string} osmUrl
 * @param {number} minLev
 * @param {number} maxLev
 * @param {math.RectArea[]| null} areaList
 */
export function removeTileUrls(urls, osmUrl, minLev, maxLev, areaList)
{
	if (areaList == null || areaList.length == 0)
		return;
	let urlMap = {};
	let i;
	for (i in urls)
	{
		urlMap[urls[i]] = true;
	}

	let tileSize = 256;
	let minX;
	let minY;
	let maxX;
	let maxY;
	let j;
	let k;
	let rect;
	let url;
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
					url = osmUrl.replace("{x}", ""+i).replace("{y}", ""+j).replace("{z}", ""+minLev);
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

/**
 * @param {number} scale
 */
export function scale2Level(scale)
{
	return Math.round(Math.log10(204094080000.0 / scale / 256) / Math.log10(2));
}
