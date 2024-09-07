import * as data from "../data.js";
import * as map from "../map.js";
import * as math from "../math.js";

export class GPXExporter
{
	constructor()
	{
	}

	getName()
	{
		return "GPSExporter";
	}

	/**
	 * @param {data.ParsedObject} pobj
	 */
	isObjectSupported(pobj)
	{
		if (pobj instanceof map.GPSTrack)
		{
			return true;
		}
		return false;
	}

	getOutputExt()
	{
		return "gpx";
	}

	getOutputMIME()
	{
		return "application/gpx+xml";
	}

	/**
	 * @param {string} fileName
	 * @param {data.ParsedObject} pobj
	 * @param {object|undefined} param
	 */
	exportFile(fileName, pobj, param)
	{
		let track = pobj;
		if (!(track instanceof map.GPSTrack))
			return null;
		let sb = [];
		let i;
		let j;
		let k;
		let l;
		let recs;
	
		sb.push("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n");
		sb.push("<gpx version=\"v1.0\" creator=\"sswr - https://github.com/sswroom/SClass\">\r\n");
		sb.push("<trk>\r\n");
		sb.push("<name>Track</name>\r\n");
		i = 0;
		j = track.getTrackCnt();
		while (i < j)
		{
			if ((recs = track.getTrack(i)) != null)
			{
				sb.push("<trkseg>\r\n");
				k = 0;
				l = recs.length;
				while (k < l)
				{
					sb.push("<trkpt lat=\""+recs[k].lat+"\" lon=\""+recs[k].lon+"\">\r\n");
					sb.push("<ele>"+recs[k].altitude+"</ele>");
					sb.push("<time>"+data.Timestamp.fromTicks(recs[k].recTime, 0).toString("yyyy-MM-ddTHH:mm:ssZ")+"</time>\r\n");
					sb.push("<desc>lat.="+math.roundToStr(recs[k].lat, 6)+", lon.="+math.roundToStr(recs[k].lon, 6));
					sb.push(", Alt.="+math.roundToStr(recs[k].altitude, 6)+"m, Speed="+math.roundToStr(recs[k].speed * 1.852, 6)+"m/h.</desc>\r\n");
					sb.push("<speed>"+math.roundToStr(recs[k].speed * 1.852 / 3.6, 6)+"</speed>\r\n");
					sb.push("</trkpt>\r\n");
	
					k++;
				}
				sb.push("</trkseg>\r\n");
			}
			i++;
		}
	
		sb.push("</trk>\r\n");
		sb.push("</gpx>\r\n");
		return new TextEncoder().encode(sb.join(""));
	}
};
