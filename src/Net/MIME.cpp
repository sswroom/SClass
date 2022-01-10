#include "Stdafx.h"
#include "MyMemory.h"
#include "Net/MIME.h"
#include "Text/MyString.h"

Net::MIME::MIMEEntry Net::MIME::mimeList[] = {
	{"3gp",  UTF8STRC("video/3gpp")},
	{"3g2",  UTF8STRC("video/3gpp2")},
	{"aac",  UTF8STRC("audio/x-aac")},
	{"aif",  UTF8STRC("audio/aiff")},
	{"asf",  UTF8STRC("video/x-ms-asf")},
	{"bmp",  UTF8STRC("image/x-bmp")},
	{"css",  UTF8STRC("text/css")},
	{"csv",  UTF8STRC("text/csv")},
	{"dbf",  UTF8STRC("application/dbf")},
	{"doc",  UTF8STRC("application/msword")},
	{"docm", UTF8STRC("application/vnd.ms-word.document.macroEnabled.12")},
	{"docx", UTF8STRC("application/vnd.openxmlformats-officedocument.wordprocessingml.document")},
	{"dot",  UTF8STRC("application/msword")},
	{"dotm", UTF8STRC("application/vnd.ms-word.template.macroEnabled.12")},
	{"dotx", UTF8STRC("application/vnd.openxmlformats-officedocument.wordprocessingml.template")},
	{"eml",  UTF8STRC("message/rfc822")},
	{"exe",  UTF8STRC("application/x-exe")},
	{"flv",  UTF8STRC("video/x-flv")},
	{"gif",  UTF8STRC("image/gif")},
	{"gpx",  UTF8STRC("application/gpx+xml")},
	{"gz",   UTF8STRC("application/x-gzip")},
	{"htm",  UTF8STRC("text/html")},
	{"html", UTF8STRC("text/html")},
	{"ico",  UTF8STRC("image/vnd.microsoft.icon")},
	{"igs",  UTF8STRC("model/iges")},
	{"iso",  UTF8STRC("application/x-iso9660-image")},
	{"jp2",  UTF8STRC("image/jpeg2000")},
	{"jpeg", UTF8STRC("image/jpeg")},
	{"jpg",  UTF8STRC("image/jpeg")},
	{"js",   UTF8STRC("application/javascript")},
	{"json", UTF8STRC("application/json")},
	{"kml",  UTF8STRC("application/vnd.google-earth.kml+xml")},
	{"kmz",  UTF8STRC("application/vnd.google-earth.kmz")},
	{"lnk",  UTF8STRC("application/x-ms-shortcut")},
	{"m1v",  UTF8STRC("video/MPV")},
	{"m2v",  UTF8STRC("video/MPV")},
	{"m2p",  UTF8STRC("video/MP2P")},
	{"m2ts", UTF8STRC("video/MP2T")},
	{"m2t",  UTF8STRC("video/MP2T")},
	{"m3u8", UTF8STRC("application/vnd.apple.mpegurl")},
	{"m4a",  UTF8STRC("audio/x-m4a")},
	{"mdb",  UTF8STRC("application/vnd.ms-access")},
	{"mkv",  UTF8STRC("video/x-matroska")},
	{"mov",  UTF8STRC("video/quicktime")},
	{"mp2",  UTF8STRC("audio/mpeg")},
	{"mp3",  UTF8STRC("audio/mpeg")},
	{"mp4",  UTF8STRC("video/mp4")},
	{"mpg",  UTF8STRC("video/mpeg")},
	{"ogg",  UTF8STRC("application/ogg")},
	{"pac",  UTF8STRC("application/x-ns-proxy-autoconfig")},
	{"pdf",  UTF8STRC("application/pdf")},
	{"pic",  UTF8STRC("image/x-pict")},
	{"png",  UTF8STRC("image/png")},
	{"pnt",  UTF8STRC("image/x-maxpaint")},
	{"rar",  UTF8STRC("application/x-rar-compressed")},
	{"svg",  UTF8STRC("image/svg+xml")},
	{"swf",  UTF8STRC("application/x-shockwave-flash")},
	{"tar",  UTF8STRC("application/x-tar")},
	{"tga",  UTF8STRC("image/x-targa")},
	{"tif",  UTF8STRC("image/tiff")},
	{"ts",   UTF8STRC("video/MP2T")},
	{"txt",  UTF8STRC("text/plain")},
	{"wav",  UTF8STRC("audio/x-wav")},
	{"webm", UTF8STRC("video/webm")},
	{"wma",  UTF8STRC("audio/x-ms-wma")},
	{"wmv",  UTF8STRC("video/x-ms-wmv")},
	{"wrl",  UTF8STRC("model/vrml")},
	{"x3d",  UTF8STRC("model/x3d+xml")},
	{"x3dv", UTF8STRC("model/x3d+vrml")},
	{"x3db", UTF8STRC("model/x3d+binary")},
	{"xla",  UTF8STRC("application/vnd.ms-excel")},
	{"xlam", UTF8STRC("application/vnd.ms-excel.addin.macroEnabled.12")},
	{"xls",  UTF8STRC("application/vnd.ms-excel")},
	{"xlsb", UTF8STRC("application/vnd.ms-excel.sheet.binary.macroEnabled.12")},
	{"xlsm", UTF8STRC("application/vnd.ms-excel.sheet.macroEnabled.12")},
	{"xlsx", UTF8STRC("application/vnd.openxmlformats-officedocument.spreadsheetml.sheet")},
	{"xlt",  UTF8STRC("application/vnd.ms-excel")},
	{"xltm", UTF8STRC("application/vnd.ms-excel.template.macroEnabled.12")},
	{"xltx", UTF8STRC("application/vnd.openxmlformats-officedocument.spreadsheetml.template")},
	{"xml",  UTF8STRC("text/xml")},
	{"zip",  UTF8STRC("application/zip")}
};

Text::CString Net::MIME::GetMIMEFromExt(const UTF8Char *ext)
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
			return {mimeList[k].mime, mimeList[k].mimeLen};
		}
	}
	return {UTF8STRC("application/octet-stream")};
}
