#include "Stdafx.h"
#include "MyMemory.h"
#include "Net/MIME.h"
#include "Text/MyString.h"

Net::MIME::MIMEEntry Net::MIME::mimeList[] = {
	{UTF8STRC("3gp"),  UTF8STRC("video/3gpp")},
	{UTF8STRC("3g2"),  UTF8STRC("video/3gpp2")},
	{UTF8STRC("7z"),   UTF8STRC("application/x-7z-compressed")},
	{UTF8STRC("aac"),  UTF8STRC("audio/aac")},
	{UTF8STRC("abw"),  UTF8STRC("application/x-abiword")},
	{UTF8STRC("aif"),  UTF8STRC("audio/aiff")},
	{UTF8STRC("arc"),  UTF8STRC("application/x-freearc")},
	{UTF8STRC("asf"),  UTF8STRC("video/x-ms-asf")},
	{UTF8STRC("avi"),  UTF8STRC("video/x-msvideo")},
	{UTF8STRC("avif"), UTF8STRC("image/avif")},
	{UTF8STRC("azw"),  UTF8STRC("application/vnd.amazon.ebook")},
	{UTF8STRC("bin"),  UTF8STRC("application/octet-stream")},
	{UTF8STRC("bmp"),  UTF8STRC("image/bmp")},
	{UTF8STRC("bz"),   UTF8STRC("application/x-bzip")},
	{UTF8STRC("bz2"),  UTF8STRC("application/x-bzip2")},
	{UTF8STRC("cda"),  UTF8STRC("application/x-cdf")},
	{UTF8STRC("cjs"),  UTF8STRC("application/node")},
	{UTF8STRC("csh"),  UTF8STRC("application/x-csh")},
	{UTF8STRC("css"),  UTF8STRC("text/css")},
	{UTF8STRC("csv"),  UTF8STRC("text/csv")},
	{UTF8STRC("dbf"),  UTF8STRC("application/dbf")},
	{UTF8STRC("doc"),  UTF8STRC("application/msword")},
	{UTF8STRC("docm"), UTF8STRC("application/vnd.ms-word.document.macroEnabled.12")},
	{UTF8STRC("docx"), UTF8STRC("application/vnd.openxmlformats-officedocument.wordprocessingml.document")},
	{UTF8STRC("dot"),  UTF8STRC("application/msword")},
	{UTF8STRC("dotm"), UTF8STRC("application/vnd.ms-word.template.macroEnabled.12")},
	{UTF8STRC("dotx"), UTF8STRC("application/vnd.openxmlformats-officedocument.wordprocessingml.template")},
	{UTF8STRC("eml"),  UTF8STRC("message/rfc822")},
	{UTF8STRC("eot"),  UTF8STRC("application/vnd.ms-fontobject")},
	{UTF8STRC("epub"), UTF8STRC("application/epub+zip")},
	{UTF8STRC("exe"),  UTF8STRC("application/x-exe")},
	{UTF8STRC("flv"),  UTF8STRC("video/x-flv")},
	{UTF8STRC("gif"),  UTF8STRC("image/gif")},
	{UTF8STRC("glb"),  UTF8STRC("model/gltf-binary")},
	{UTF8STRC("gpx"),  UTF8STRC("application/gpx+xml")},
	{UTF8STRC("gz"),   UTF8STRC("application/gzip")},
	{UTF8STRC("heic"), UTF8STRC("image/heic")},
	{UTF8STRC("heif"), UTF8STRC("image/heif")},
	{UTF8STRC("htm"),  UTF8STRC("text/html")},
	{UTF8STRC("html"), UTF8STRC("text/html")},
	{UTF8STRC("ico"),  UTF8STRC("image/vnd.microsoft.icon")},
	{UTF8STRC("ics"),  UTF8STRC("text/calendar")},
	{UTF8STRC("igs"),  UTF8STRC("model/iges")},
	{UTF8STRC("ipa"),  UTF8STRC("application/x-ios-app")},
	{UTF8STRC("iso"),  UTF8STRC("application/x-iso9660-image")},
	{UTF8STRC("jar"),  UTF8STRC("application/java-archive")},
	{UTF8STRC("jp2"),  UTF8STRC("image/jpeg2000")},
	{UTF8STRC("jpeg"), UTF8STRC("image/jpeg")},
	{UTF8STRC("jpg"),  UTF8STRC("image/jpeg")},
	{UTF8STRC("js"),   UTF8STRC("text/javascript")}, //RFC 9239
	{UTF8STRC("json"), UTF8STRC("application/json")},
	{UTF8STRC("jsonld"),UTF8STRC("application/ld+json")},
	{UTF8STRC("jxl"),  UTF8STRC("image/jxl")},
	{UTF8STRC("kml"),  UTF8STRC("application/vnd.google-earth.kml+xml")},
	{UTF8STRC("kmz"),  UTF8STRC("application/vnd.google-earth.kmz")},
	{UTF8STRC("lnk"),  UTF8STRC("application/x-ms-shortcut")},
	{UTF8STRC("m1v"),  UTF8STRC("video/MPV")},
	{UTF8STRC("m2v"),  UTF8STRC("video/MPV")},
	{UTF8STRC("m2p"),  UTF8STRC("video/MP2P")},
	{UTF8STRC("m2ts"), UTF8STRC("video/MP2T")},
	{UTF8STRC("m2t"),  UTF8STRC("video/MP2T")},
	{UTF8STRC("m3u8"), UTF8STRC("application/vnd.apple.mpegurl")},
	{UTF8STRC("m4a"),  UTF8STRC("audio/x-m4a")},
	{UTF8STRC("md"),   UTF8STRC("text/markdown")},
	{UTF8STRC("mdb"),  UTF8STRC("application/vnd.ms-access")},
	{UTF8STRC("mid"),  UTF8STRC("audio/midi")},
	{UTF8STRC("midi"), UTF8STRC("audio/midi")},
	{UTF8STRC("mjs"),  UTF8STRC("text/javascript")}, //RFC 9239
	{UTF8STRC("mkv"),  UTF8STRC("video/x-matroska")},
	{UTF8STRC("mov"),  UTF8STRC("video/quicktime")},
	{UTF8STRC("mp2"),  UTF8STRC("audio/mpeg")},
	{UTF8STRC("mp3"),  UTF8STRC("audio/mpeg")},
	{UTF8STRC("mp4"),  UTF8STRC("video/mp4")},
	{UTF8STRC("mpeg"), UTF8STRC("video/mpeg")},
	{UTF8STRC("mpg"),  UTF8STRC("video/mpeg")},
	{UTF8STRC("mpkg"), UTF8STRC("application/vnd.apple.installer+xml")},
	{UTF8STRC("odp"),  UTF8STRC("application/vnd.oasis.opendocument.presentation")},
	{UTF8STRC("ods"),  UTF8STRC("application/vnd.oasis.opendocument.spreadsheet")},
	{UTF8STRC("odt"),  UTF8STRC("application/vnd.oasis.opendocument.text")},
	{UTF8STRC("oga"),  UTF8STRC("audio/ogg")},
	{UTF8STRC("ogg"),  UTF8STRC("application/ogg")},
	{UTF8STRC("ogv"),  UTF8STRC("video/ogg")},
	{UTF8STRC("ogx"),  UTF8STRC("application/ogg")},
	{UTF8STRC("opus"), UTF8STRC("audio/opus")},
	{UTF8STRC("otf"),  UTF8STRC("font/otf")},
	{UTF8STRC("p10"),  UTF8STRC("application/pkcs10")},
	{UTF8STRC("p12"),  UTF8STRC("application/x-pkcs12")},
	{UTF8STRC("p7c"),  UTF8STRC("application/pkcs7-mime")},
	{UTF8STRC("p7m"),  UTF8STRC("application/pkcs7-mime")},
	{UTF8STRC("p7s"),  UTF8STRC("application/pkcs7-signature")},
	{UTF8STRC("pac"),  UTF8STRC("application/x-ns-proxy-autoconfig")},
	{UTF8STRC("pdf"),  UTF8STRC("application/pdf")},
	{UTF8STRC("php"),  UTF8STRC("application/x-httpd-php")},
	{UTF8STRC("pic"),  UTF8STRC("image/x-pict")},
	{UTF8STRC("png"),  UTF8STRC("image/png")},
	{UTF8STRC("pnt"),  UTF8STRC("image/x-maxpaint")},
	{UTF8STRC("ppt"),  UTF8STRC("application/vnd.ms-powerpoint")},
	{UTF8STRC("pptx"), UTF8STRC("application/vnd.openxmlformats-officedocument.presentationml.presentation")},
	{UTF8STRC("rar"),  UTF8STRC("application/vnd.rar")}, //application/x-rar-compressed
	{UTF8STRC("rtf"),  UTF8STRC("application/rtf")},
	{UTF8STRC("sh"),   UTF8STRC("application/x-sh")},
	{UTF8STRC("svg"),  UTF8STRC("image/svg+xml")},
	{UTF8STRC("swf"),  UTF8STRC("application/x-shockwave-flash")},
	{UTF8STRC("tar"),  UTF8STRC("application/x-tar")},
	{UTF8STRC("tga"),  UTF8STRC("image/x-targa")},
	{UTF8STRC("tif"),  UTF8STRC("image/tiff")},
	{UTF8STRC("tiff"), UTF8STRC("image/tiff")},
	{UTF8STRC("ts"),   UTF8STRC("video/mp2t")},
	{UTF8STRC("ttf"),  UTF8STRC("font/ttf")},
	{UTF8STRC("txt"),  UTF8STRC("text/plain")},
	{UTF8STRC("vsd"),  UTF8STRC("application/vnd.visio")},
	{UTF8STRC("wasm"), UTF8STRC("application/wasm")},
	{UTF8STRC("wav"),  UTF8STRC("audio/wav")},
	{UTF8STRC("weba"), UTF8STRC("audio/webm")},
	{UTF8STRC("webm"), UTF8STRC("video/webm")},
	{UTF8STRC("webp"), UTF8STRC("image/webp")},
	{UTF8STRC("wma"),  UTF8STRC("audio/x-ms-wma")},
	{UTF8STRC("wmv"),  UTF8STRC("video/x-ms-wmv")},
	{UTF8STRC("woff"), UTF8STRC("font/woff")},
	{UTF8STRC("woff2"),UTF8STRC("font/woff2")},
	{UTF8STRC("wrl"),  UTF8STRC("model/vrml")},
	{UTF8STRC("x3d"),  UTF8STRC("model/x3d+xml")},
	{UTF8STRC("x3dv"), UTF8STRC("model/x3d+vrml")},
	{UTF8STRC("x3db"), UTF8STRC("model/x3d+binary")},
	{UTF8STRC("xhtml"),UTF8STRC("application/xhtml+xml")},
	{UTF8STRC("xla"),  UTF8STRC("application/vnd.ms-excel")},
	{UTF8STRC("xlam"), UTF8STRC("application/vnd.ms-excel.addin.macroEnabled.12")},
	{UTF8STRC("xls"),  UTF8STRC("application/vnd.ms-excel")},
	{UTF8STRC("xlsb"), UTF8STRC("application/vnd.ms-excel.sheet.binary.macroEnabled.12")},
	{UTF8STRC("xlsm"), UTF8STRC("application/vnd.ms-excel.sheet.macroEnabled.12")},
	{UTF8STRC("xlsx"), UTF8STRC("application/vnd.openxmlformats-officedocument.spreadsheetml.sheet")},
	{UTF8STRC("xlt"),  UTF8STRC("application/vnd.ms-excel")},
	{UTF8STRC("xltm"), UTF8STRC("application/vnd.ms-excel.template.macroEnabled.12")},
	{UTF8STRC("xltx"), UTF8STRC("application/vnd.openxmlformats-officedocument.spreadsheetml.template")},
	{UTF8STRC("xml"),  UTF8STRC("application/xml")}, //RFC 7303
	{UTF8STRC("xul"),  UTF8STRC("application/vnd.mozilla.xul+xml")},
	{UTF8STRC("zip"),  UTF8STRC("application/zip")}
};

Text::CStringNN Net::MIME::GetMIMEFromExt(Text::CStringNN ext)
{
	UTF8Char sbuff[7];
	if (ext.leng > 6 || ext.leng < 2)
	{
		return CSTR("application/octet-stream");
	}
	Text::StrToLowerC(sbuff, ext.v, ext.leng);
	OSInt i = 0;
	OSInt j = (OSInt)(sizeof(mimeList) / sizeof(mimeList[0])) - 1;
	OSInt k;
	OSInt l;
	while (i <= j)
	{
		k = (i + j) >> 1;
		l = Text::StrCompareFastC(sbuff, ext.leng, mimeList[k].ext, mimeList[k].extLen);
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
	return CSTR("application/octet-stream");
}

Text::CStringNN Net::MIME::GetMIMEFromFileName(UnsafeArray<const UTF8Char> fileName, UOSInt nameLen)
{
	UOSInt i = Text::StrLastIndexOfCharC(fileName, nameLen, '.');
	if (i == INVALID_INDEX)
	{
		return CSTR("application/octet-stream");
	}
	return GetMIMEFromExt(Text::CStringNN(fileName + i + 1, nameLen - i - 1));
}
