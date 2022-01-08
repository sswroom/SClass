#include "Stdafx.h"
#include "MyMemory.h"
#include "Net/MIME.h"
#include "Text/MyString.h"

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
	{"docm", "application/vnd.ms-word.document.macroEnabled.12"},
	{"docx", "application/vnd.openxmlformats-officedocument.wordprocessingml.document"},
	{"dot",  "application/msword"},
	{"dotm", "application/vnd.ms-word.template.macroEnabled.12"},
	{"dotx", "application/vnd.openxmlformats-officedocument.wordprocessingml.template"},
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
	{"js",   "application/javascript"},
	{"json", "application/json"},
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
	{"mdb",  "application/vnd.ms-access"},
	{"mkv",  "video/x-matroska"},
	{"mov",  "video/quicktime"},
	{"mp2",  "audio/mpeg"},
	{"mp3",  "audio/mpeg"},
	{"mp4",  "video/mp4"},
	{"mpg",  "video/mpeg"},
	{"ogg",  "application/ogg"},
	{"pac",  "application/x-ns-proxy-autoconfig"},
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
	{"xla",  "application/vnd.ms-excel"},
	{"xlam", "application/vnd.ms-excel.addin.macroEnabled.12"},
	{"xls",  "application/vnd.ms-excel"},
	{"xlsb", "application/vnd.ms-excel.sheet.binary.macroEnabled.12"},
	{"xlsm", "application/vnd.ms-excel.sheet.macroEnabled.12"},
	{"xlsx", "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"},
	{"xlt",  "application/vnd.ms-excel"},
	{"xltm", "application/vnd.ms-excel.template.macroEnabled.12"},
	{"xltx", "application/vnd.openxmlformats-officedocument.spreadsheetml.template"},
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
		l = Text::StrCompareICase(ext, (const UTF8Char*)mimeList[k].ext);
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
