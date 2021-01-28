#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"
#include "Net/MIME.h"

Net::MIME::MIMEEntry Net::MIME::mimeList[] = {
	{"3gp",  "video/3gpp"},
	{"3g2",  "video/3gpp2"},
	{"aac",  "audio/x-aac"},
	{"aif",  "audio/aiff"},
	{"asf",  "video/x-ms-asf"},
	{"bmp",  "image/x-bmp"},
	{"css",  "text/css"},
	{"csv",  "text/csv"},
	{"dbf",  "application/dbf"},
	{"doc",  "application/msword"},
	{"eml",  "message/rfc822"},
	{"exe",  "application/x-exe"},
	{"flv",  "video/x-flv"},
	{"gif",  "image/gif"},
	{"gpx",  "application/gpx+xml"},
	{"gz",   "application/x-gzip"},
	{"htm",  "text/html"},
	{"html", "text/html"},
	{"ico",  "image/vnd.microsoft.icon"},
	{"igs",  "model/iges"},
	{"iso",  "application/x-iso9660-image"},
	{"jp2",  "image/jpeg2000"},
	{"jpeg", "image/jpeg"},
	{"jpg",  "image/jpeg"},
	{"js",   "text/javascript"},
	{"kml",  "application/vnd.google-earth.kml+xml"},
	{"kmz",  "application/vnd.google-earth.kmz"},
	{"lnk",  "application/x-ms-shortcut"},
	{"m1v",  "video/MPV"},
	{"m2v",  "video/MPV"},
	{"m2p",  "video/MP2P"},
	{"m2ts", "video/MP2T"},
	{"m2t",  "video/MP2T"},
	{"m3u8", "application/vnd.apple.mpegurl"},
	{"m4a",  "audio/x-m4a"},
	{"mkv",  "video/x-matroska"},
	{"mov",  "video/quicktime"},
	{"mp2",  "audio/mpeg"},
	{"mp3",  "audio/mpeg"},
	{"mp4",  "video/mp4"},
	{"mpg",  "video/mpeg"},
	{"ogg",  "application/ogg"},
	{"pdf",  "application/pdf"},
	{"pic",  "image/x-pict"},
	{"png",  "image/png"},
	{"pnt",  "image/x-maxpaint"},
	{"rar",  "application/x-rar-compressed"},
	{"svg",  "image/svg+xml"},
	{"swf",  "application/x-shockwave-flash"},
	{"tar",  "application/x-tar"},
	{"tga",  "image/x-targa"},
	{"tif",  "image/tiff"},
	{"ts",   "video/MP2T"},
	{"txt",  "text/plain"},
	{"wav",  "audio/x-wav"},
	{"webm", "video/webm"},
	{"wma",  "audio/x-ms-wma"},
	{"wmv",  "video/x-ms-wmv"},
	{"wrl",  "model/vrml"},
	{"x3d",  "model/x3d+xml"},
	{"x3dv", "model/x3d+vrml"},
	{"x3db", "model/x3d+binary"},
	{"xls",  "application/vnd.ms-excel"},
	{"xml",  "text/xml"},
	{"zip",  "application/zip"}
};

const UTF8Char *Net::MIME::GetMIMEFromExt(const UTF8Char *ext)
{
	OSInt i = 0;
	OSInt j = (sizeof(mimeList) / sizeof(mimeList[0])) - 1;
	OSInt k;
	OSInt l;
	while (i <= j)
	{
		k = (i + j) >> 1;
		l = Text::StrCompareICase((const Char*)ext, mimeList[k].ext);
		if (l > 0)
		{
			i = k + 1;
		}
		else if (l < 0)
		{
			j = k - 1;
		}
		else
		{
			return (const UTF8Char*)mimeList[k].mime;
		}
	}
	return (const UTF8Char*)"application/octet-stream";
}
