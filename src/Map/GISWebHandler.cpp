#include "Stdafx.h"
#include "Map/GISWebHandler.h"
#include "Math/CoordinateSystemManager.h"
#include "Net/WebServer/HTTPServerUtil.h"
#include "Text/XML.h"

// http://192.168.1.25:8080/geoserver/ows?service=WMS&version=1.1.1&request=GetCapabilities
// http://192.168.1.25:8080/geoserver/ows?service=WMS&version=1.3.0&request=GetCapabilities
// http://192.168.1.25:8080/geoserver/gwc/service/wmts?service=WMTS&version=1.1.1&request=GetCapabilities
// http://192.168.1.25:8080/geoserver/gwc/service/wms?service=WMS&version=1.1.1&request=GetCapabilities&tiled=true
// http://192.168.1.25:8080/geoserver/gwc/service/tms/1.0.0
// http://192.168.1.25:8080/geoserver/ows?service=WFS&version=1.0.0&request=GetCapabilities
// http://192.168.1.25:8080/geoserver/ows?service=WFS&version=1.1.0&request=GetCapabilities
// http://192.168.1.25:8080/geoserver/ows?service=WFS&acceptversions=2.0.0&request=GetCapabilities

Bool __stdcall Map::GISWebHandler::OWSFunc(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svcHdlr)
{
	NN<Map::GISWebHandler> me = NN<Map::GISWebHandler>::ConvertFrom(svcHdlr);
	NN<Text::String> service;
	if (!req->GetQueryValue(CSTR("service")).SetTo(service))
	{
		return me->ResponseException(req, resp, CSTR("InvalidParameterValue"), CSTR("service"), CSTR("No service: ( ows )"));
	}
	if (service->EqualsICase(CSTR("WFS")))
	{
		return me->DoWFS(req, resp);
	}
	else if (service->EqualsICase(CSTR("WMS")))
	{

	}
	else
	{
		return me->ResponseException(req, resp, CSTR("InvalidParameterValue"), CSTR("service"), (Text::StringBuilderUTF8() + CSTR("No service: ( ") + service + CSTR(" )")).ToCString());
	}
	return false;
}

Bool __stdcall Map::GISWebHandler::WFSFunc(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svcHdlr)
{
	NN<Map::GISWebHandler> me = NN<Map::GISWebHandler>::ConvertFrom(svcHdlr);
	return me->DoWFS(req, resp);
}

Bool __stdcall Map::GISWebHandler::WMTSFunc(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svcHdlr)
{
	return false;
}

Bool __stdcall Map::GISWebHandler::WMSCFunc(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svcHdlr)
{
	return false;
}

Bool __stdcall Map::GISWebHandler::TMSFunc(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svcHdlr)
{
	return false;
}

Bool Map::GISWebHandler::DoWFS(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp)
{
	Text::CStringNN version;
	NN<Text::String> request;
	NN<Text::String> s;
	NN<GISFeature> feature;
	Math::RectAreaDbl bbox;
	NN<Math::CoordinateSystem> csys;
	version = Text::String::ToCStringOr(req->GetQueryValue(CSTR("version")), CSTR("null"));
	if (!req->GetQueryValue(CSTR("reqeust")).SetTo(request))
	{
		return this->ResponseException(req, resp, CSTR("MissingParameterValue"), CSTR("request"), CSTR("Could not determine wfs request from http request"));
	}
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	sptr = req->BuildURLHost(sbuff);
	sptr = req->GetRequestPath(sptr, 511 - (UOSInt)(sptr - sbuff));
	UOSInt i = Text::StrLastIndexOfCharC(sbuff, (UOSInt)(sptr - sbuff), '/');
	UOSInt j;
	sptr = &sbuff[i];
	sptr[0] = 0;
	if (request->Equals(CSTR("GetCapabilities")))
	{
		if (version.Equals(CSTR("1.0.0")))
		{
			Text::StringBuilderUTF8 sb;
			sb.Append(CSTR("<?xml version=\"1.0\" encoding=\"UTF-8\"?>"));
			sb.Append(CSTR("<WFS_Capabilities version=\"1.0.0\" xmlns=\"http://www.opengis.net/wfs\" xmlns:ogc=\"http://www.opengis.net/ogc\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">"));
				sb.Append(CSTR("<Service>"));
					sb.Append(CSTR("<Name>WFS</Name>"));
					sb.Append(CSTR("<Title>SSWR Web Feature Service</Title>"));
					sb.Append(CSTR("<Abstract>This is an implementation of WFS 1.0.0 and WFS 1.1.0, supports basic WFS operations.</Abstract>"));
					sb.Append(CSTR("<Keywords>WFS, SSWROOM</Keywords>"));
					sb.Append(CSTR("<OnlineResource>https://sswroom.no-ip.org</OnlineResource>"));
					sb.Append(CSTR("<Fees>NONE</Fees>"));
					sb.Append(CSTR("<AccessConstraints>NONE</AccessConstraints>"));
				sb.Append(CSTR("</Service>"));
				sb.Append(CSTR("<Capability>"));
					sb.Append(CSTR("<Request>"));
						sb.Append(CSTR("<GetCapabilities>"));
							sb.Append(CSTR("<DCPType>"));
								sb.Append(CSTR("<HTTP>"));
									sb.Append(CSTR("<Get onlineResource=\""))->AppendP(sbuff, sptr)->Append(CSTR("/wfs?request=GetCapabilities\"/>"));
								sb.Append(CSTR("</HTTP>"));
							sb.Append(CSTR("</DCPType>"));
							sb.Append(CSTR("<DCPType>"));
								sb.Append(CSTR("<HTTP>"));
									sb.Append(CSTR("<Post onlineResource=\""))->AppendP(sbuff, sptr)->Append(CSTR("/wfs\"/>"));
								sb.Append(CSTR("</HTTP>"));
							sb.Append(CSTR("</DCPType>"));
						sb.Append(CSTR("</GetCapabilities>"));
						sb.Append(CSTR("<DescribeFeatureType>"));
							sb.Append(CSTR("<SchemaDescriptionLanguage>"));
								sb.Append(CSTR("<XMLSCHEMA/>"));
							sb.Append(CSTR("</SchemaDescriptionLanguage>"));
							sb.Append(CSTR("<DCPType>"));
								sb.Append(CSTR("<HTTP>"));
									sb.Append(CSTR("<Get onlineResource=\""))->AppendP(sbuff, sptr)->Append(CSTR("/wfs?request=DescribeFeatureType\"/>"));
								sb.Append(CSTR("</HTTP>"));
							sb.Append(CSTR("</DCPType>"));
							sb.Append(CSTR("<DCPType>"));
								sb.Append(CSTR("<HTTP>"));
									sb.Append(CSTR("<Post onlineResource=\""))->AppendP(sbuff, sptr)->Append(CSTR("/wfs\"/>"));
								sb.Append(CSTR("</HTTP>"));
							sb.Append(CSTR("</DCPType>"));
						sb.Append(CSTR("</DescribeFeatureType>"));
						sb.Append(CSTR("<GetFeature>"));
							sb.Append(CSTR("<ResultFormat>"));
								sb.Append(CSTR("<KML/>"));
								sb.Append(CSTR("<GML2/>"));
								sb.Append(CSTR("<GML3/>"));
								sb.Append(CSTR("<SHAPE-ZIP/>"));
								sb.Append(CSTR("<CSV/>"));
								sb.Append(CSTR("<JSON/>"));
							sb.Append(CSTR("</ResultFormat>"));
							sb.Append(CSTR("<DCPType>"));
								sb.Append(CSTR("<HTTP>"));
									sb.Append(CSTR("<Get onlineResource=\""))->AppendP(sbuff, sptr)->Append(CSTR("/wfs?request=GetFeature\"/>"));
								sb.Append(CSTR("</HTTP>"));
							sb.Append(CSTR("</DCPType>"));
							sb.Append(CSTR("<DCPType>"));
								sb.Append(CSTR("<HTTP>"));
									sb.Append(CSTR("<Post onlineResource=\""))->AppendP(sbuff, sptr)->Append(CSTR("/wfs\"/>"));
								sb.Append(CSTR("</HTTP>"));
							sb.Append(CSTR("</DCPType>"));
						sb.Append(CSTR("</GetFeature>"));
/*						sb.Append(CSTR("<Transaction>"));
							sb.Append(CSTR("<DCPType>"));
								sb.Append(CSTR("<HTTP>"));
									sb.Append(CSTR("<Get onlineResource=\""))->AppendP(sbuff, sptr)->Append(CSTR("/wfs?request=Transaction\"/>"));
								sb.Append(CSTR("</HTTP>"));
							sb.Append(CSTR("</DCPType>"));
							sb.Append(CSTR("<DCPType>"));
								sb.Append(CSTR("<HTTP>"));
									sb.Append(CSTR("<Post onlineResource=\""))->AppendP(sbuff, sptr)->Append(CSTR("/wfs\"/>"));
								sb.Append(CSTR("</HTTP>"));
							sb.Append(CSTR("</DCPType>"));
						sb.Append(CSTR("</Transaction>"));
						sb.Append(CSTR("<LockFeature>"));
							sb.Append(CSTR("<DCPType>"));
								sb.Append(CSTR("<HTTP>"));
									sb.Append(CSTR("<Get onlineResource=\""))->AppendP(sbuff, sptr)->Append(CSTR("/wfs?request=LockFeature\"/>"));
								sb.Append(CSTR("</HTTP>"));
							sb.Append(CSTR("</DCPType>"));
							sb.Append(CSTR("<DCPType>"));
								sb.Append(CSTR("<HTTP>"));
									sb.Append(CSTR("<Post onlineResource=\""))->AppendP(sbuff, sptr)->Append(CSTR("/wfs\"/>"));
								sb.Append(CSTR("</HTTP>"));
							sb.Append(CSTR("</DCPType>"));
						sb.Append(CSTR("</LockFeature>"));
						sb.Append(CSTR("<GetFeatureWithLock>"));
							sb.Append(CSTR("<ResultFormat>"));
								sb.Append(CSTR("<GML2/>"));
							sb.Append(CSTR("</ResultFormat>"));
							sb.Append(CSTR("<DCPType>"));
								sb.Append(CSTR("<HTTP>"));
									sb.Append(CSTR("<Get onlineResource=\""))->AppendP(sbuff, sptr)->Append(CSTR("/wfs?request=GetFeatureWithLock\"/>"));
								sb.Append(CSTR("</HTTP>"));
							sb.Append(CSTR("</DCPType>"));
							sb.Append(CSTR("<DCPType>"));
								sb.Append(CSTR("<HTTP>"));
									sb.Append(CSTR("<Post onlineResource=\""))->AppendP(sbuff, sptr)->Append(CSTR("/wfs\"/>"));
								sb.Append(CSTR("</HTTP>"));
							sb.Append(CSTR("</DCPType>"));
						sb.Append(CSTR("</GetFeatureWithLock>"));*/
					sb.Append(CSTR("</Request>"));
				sb.Append(CSTR("</Capability>"));
				sb.Append(CSTR("<FeatureTypeList>"));
					sb.Append(CSTR("<Operations>"));
						sb.Append(CSTR("<Query/>"));
						sb.Append(CSTR("<Insert/>"));
						sb.Append(CSTR("<Update/>"));
						sb.Append(CSTR("<Delete/>"));
						sb.Append(CSTR("<Lock/>"));
					sb.Append(CSTR("</Operations>"));
			i = 0;
			j = this->features.GetCount();
			while (i < j)
			{
				feature = this->features.GetItemNoCheck(i);
					sb.Append(CSTR("<FeatureType>"));
						sb.Append(CSTR("<Name>"))->Append(s = Text::XML::ToNewXMLText(feature->name->v))->Append(CSTR("</Name>"));
						sb.Append(CSTR("<Title>"))->Append(s)->Append(CSTR("</Title>"));
						sb.Append(CSTR("<Abstract/>"));
						sb.Append(CSTR("<Keywords>features, "))->Append(s)->Append(CSTR("</Keywords>"));
						s->Release();
						csys = feature->layer->GetCoordinateSystem();
						sb.Append(CSTR("<SRS>EPSG:"))->AppendU32(csys->GetSRID())->Append(CSTR("</SRS>"));
						if (feature->layer->GetBounds(bbox))
						{
							if (csys->GetSRID() != 4326)
							{
								bbox.min = Math::CoordinateSystem::Convert(csys, this->wgs84, bbox.min);
								bbox.max = Math::CoordinateSystem::Convert(csys, this->wgs84, bbox.max);
								bbox = bbox.Reorder();
							}
							sb.Append(CSTR("<LatLongBoundingBox minx=\""))->AppendDouble(bbox.min.x)->Append(CSTR("\" miny=\""))->AppendDouble(bbox.min.y)->Append(CSTR("\" maxx=\""))->AppendDouble(bbox.max.x)->Append(CSTR("\" maxy=\""))->AppendDouble(bbox.max.y)->Append(CSTR("\"/>"));
						}
					sb.Append(CSTR("</FeatureType>"));
				i++;
			}
				sb.Append(CSTR("</FeatureTypeList>"));
				sb.Append(CSTR("<ogc:Filter_Capabilities xmlns:ogc=\"http://www.opengis.net/ogc\">"));
					sb.Append(CSTR("<ogc:Spatial_Capabilities>"));
						sb.Append(CSTR("<ogc:Spatial_Operators>"));
							sb.Append(CSTR("<ogc:Disjoint/>"));
							sb.Append(CSTR("<ogc:Equals/>"));
							sb.Append(CSTR("<ogc:DWithin/>"));
							sb.Append(CSTR("<ogc:Beyond/>"));
							sb.Append(CSTR("<ogc:Intersect/>"));
							sb.Append(CSTR("<ogc:Touches/>"));
							sb.Append(CSTR("<ogc:Crosses/>"));
							sb.Append(CSTR("<ogc:Within/>"));
							sb.Append(CSTR("<ogc:Contains/>"));
							sb.Append(CSTR("<ogc:Overlaps/>"));
							sb.Append(CSTR("<ogc:BBOX/>"));
						sb.Append(CSTR("</ogc:Spatial_Operators>"));
					sb.Append(CSTR("</ogc:Spatial_Capabilities>"));
					sb.Append(CSTR("<ogc:Scalar_Capabilities>"));
						sb.Append(CSTR("<ogc:Logical_Operators/>"));
						sb.Append(CSTR("<ogc:Comparison_Operators>"));
							sb.Append(CSTR("<ogc:Simple_Comparisons/>"));
							sb.Append(CSTR("<ogc:Between/>"));
							sb.Append(CSTR("<ogc:Like/>"));
							sb.Append(CSTR("<ogc:NullCheck/>"));
						sb.Append(CSTR("</ogc:Comparison_Operators>"));
						sb.Append(CSTR("<ogc:Arithmetic_Operators>"));
							sb.Append(CSTR("<ogc:Simple_Arithmetic/>"));
							sb.Append(CSTR("<ogc:Functions>"));
								sb.Append(CSTR("<ogc:Function_Names>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"1\">abs</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"1\">abs_2</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"1\">abs_3</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"1\">abs_4</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"1\">acos</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"2\">AddCoverages</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"-1\">Affine</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"-2\">Aggregate</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"2\">And</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"1\">Area</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"1\">area2</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"3\">AreaGrid</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"0\">array</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"1\">asin</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"1\">atan</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"2\">atan2</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"1\">attributeCount</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"-1\">BandMerge</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"0\">bands</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"-2\">BandSelect</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"-6\">BarnesSurface</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"3\">between</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"1\">boundary</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"1\">boundaryDimension</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"0\">boundedBy</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"1\">Bounds</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"2\">buffer</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"-2\">BufferFeatureCollection</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"3\">bufferWithSegments</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"7\">Categorize</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"1\">ceil</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"1\">centroid</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"2\">classify</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"-2\">ClassifyByRange</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"-2\">Clip</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"1\">CollectGeometries</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"1\">Collection_Average</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"1\">Collection_Bounds</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"1\">Collection_Count</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"1\">Collection_Max</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"1\">Collection_Median</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"1\">Collection_Min</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"1\">Collection_Nearest</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"1\">Collection_Sum</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"1\">Collection_Unique</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"-2\">Concatenate</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"2\">contains</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"-1\">Contour</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"-1\">contrast</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"2\">convert</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"1\">convexHull</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"-1\">ConvolveCoverage</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"1\">cos</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"1\">Count</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"-1\">CoverageClassStats</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"2\">CropCoverage</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"2\">crosses</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"-2\">darken</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"-2\">dateDifference</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"2\">dateFormat</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"2\">dateParse</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"-2\">desaturate</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"2\">difference</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"1\">dimension</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"2\">disjoint</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"2\">disjoint3D</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"2\">distance</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"2\">distance3D</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"1\">double2bool</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"1\">endAngle</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"1\">endPoint</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"1\">env</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"1\">envelope</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"-2\">EqualArea</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"-2\">EqualInterval</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"2\">equalsExact</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"3\">equalsExactTolerance</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"-2\">equalTo</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"1\">exp</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"1\">exteriorRing</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"3\">Feature</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"-2\">FeatureClassStats</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"1\">floor</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"0\">footprints</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"0\">geometry</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"1\">geometryType</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"1\">geomFromWKT</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"1\">geomLength</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"2\">getGeometryN</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"1\">getX</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"1\">getY</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"1\">getz</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"1\">grayscale</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"2\">greaterEqualThan</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"2\">greaterThan</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"-3\">Grid</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"-4\">GroupCandidateSelection</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"-5\">Heatmap</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"3\">hsl</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"0\">id</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"2\">IEEEremainder</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"3\">if_then_else</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"-2\">in</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"11\">in10</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"3\">in2</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"4\">in3</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"5\">in4</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"6\">in5</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"7\">in6</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"8\">in7</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"9\">in8</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"10\">in9</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"2\">inArray</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"2\">InclusionFeatureCollection</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"1\">int2bbool</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"1\">int2ddouble</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"1\">interiorPoint</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"2\">interiorRingN</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"-5\">Interpolate</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"2\">intersection</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"-2\">IntersectionFeatureCollection</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"2\">intersects</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"2\">intersects3D</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"1\">isCached</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"1\">isClosed</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"0\">isCoverage</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"1\">isEmpty</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"1\">isInstanceOf</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"2\">isLike</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"1\">isNull</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"2\">isometric</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"1\">isRing</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"1\">isSimple</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"1\">isValid</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"3\">isWithinDistance</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"3\">isWithinDistance3D</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"-2\">Jenks</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"-1\">Jiffle</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"3\">jsonArrayContains</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"2\">jsonPointer</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"0\">language</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"2\">lapply</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"1\">length</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"2\">lessEqualThan</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"2\">lessThan</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"-2\">lighten</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"-1\">list</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"2\">listMultiply</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"2\">litem</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"3\">literate</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"1\">log</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"4\">LRSGeocode</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"-4\">LRSMeasure</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"5\">LRSSegment</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"2\">mapGet</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"2\">max</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"2\">max_2</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"2\">max_3</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"2\">max_4</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"2\">min</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"2\">min_2</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"2\">min_3</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"2\">min_4</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"1\">mincircle</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"1\">minimumdiameter</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"1\">minrectangle</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"3\">mix</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"2\">modulo</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"2\">MultiplyCoverages</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"-2\">Nearest</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"1\">NormalizeCoverage</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"1\">not</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"2\">notEqualTo</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"0\">now</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"-2\">numberFormat</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"5\">numberFormat2</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"1\">numGeometries</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"1\">numInteriorRing</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"1\">numPoints</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"1\">octagonalenvelope</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"3\">offset</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"2\">Or</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"2\">overlaps</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"-1\">parameter</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"1\">parseBoolean</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"1\">parseDouble</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"1\">parseInt</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"1\">parseLong</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"2\">pgNearest</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"0\">pi</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"-1\">PointBuffers</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"2\">pointN</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"-7\">PointStacker</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"-1\">PolygonExtraction</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"2\">pow</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"1\">property</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"1\">PropertyExists</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"-2\">Quantile</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"-1\">Query</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"0\">random</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"-1\">RangeLookup</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"-1\">RasterAsPointCollection</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"-2\">RasterZonalStatistics</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"-6\">RasterZonalStatistics2</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"5\">Recode</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"-2\">RectangularClip</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"2\">relate</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"3\">relatePattern</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"-1\">Reproject</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"-3\">rescaleToPixels</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"1\">rint</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"1\">round</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"1\">round_2</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"1\">roundDouble</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"-2\">saturate</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"-5\">ScaleCoverage</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"2\">setCRS</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"2\">shade</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"3\">Simplify</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"1\">sin</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"1\">size</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"-2\">Snap</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"2\">spin</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"1\">sqrt</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"-2\">StandardDeviation</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"1\">startAngle</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"1\">startPoint</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"4\">strAbbreviate</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"1\">strCapitalize</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"2\">strConcat</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"2\">strDefaultIfBlank</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"2\">strEndsWith</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"2\">strEqualsIgnoreCase</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"2\">strIndexOf</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"4\">stringTemplate</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"2\">strLastIndexOf</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"1\">strLength</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"2\">strMatches</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"3\">strPosition</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"4\">strReplace</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"2\">strStartsWith</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"1\">strStripAccents</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"3\">strSubstring</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"2\">strSubstringStart</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"1\">strToLowerCase</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"1\">strToUpperCase</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"1\">strTrim</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"3\">strTrim2</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"-1\">strURLEncode</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"2\">StyleCoverage</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"2\">symDifference</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"1\">tan</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"2\">tint</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"1\">toDegrees</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"1\">toRadians</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"2\">touches</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"1\">toWKT</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"2\">Transform</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"-1\">TransparencyFill</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"2\">union</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"2\">UnionFeatureCollection</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"2\">Unique</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"-2\">UniqueInterval</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"-4\">VectorToRaster</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"3\">VectorZonalStatistics</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"1\">vertices</ogc:Function_Name>"));
									sb.Append(CSTR("<ogc:Function_Name nArgs=\"2\">within</ogc:Function_Name>"));
								sb.Append(CSTR("</ogc:Function_Names>"));
							sb.Append(CSTR("</ogc:Functions>"));
						sb.Append(CSTR("</ogc:Arithmetic_Operators>"));
					sb.Append(CSTR("</ogc:Scalar_Capabilities>"));
				sb.Append(CSTR("</ogc:Filter_Capabilities>"));
			sb.Append(CSTR("</WFS_Capabilities>"));
			resp->AddDefHeaders(req);
			resp->AddContentType(CSTR("application/xml"));
			this->AddHeaders(req, resp);
			return Net::WebServer::HTTPServerUtil::SendContent(req, resp, CSTR("application/xml"), sb.ToCString());
		}
		else if (version.StartsWith(CSTR("1.")))
		{
			return resp->ResponseError(req, Net::WebStatus::SC_NOT_IMPLEMENTED);
		}
		else
		{
			return resp->ResponseError(req, Net::WebStatus::SC_NOT_IMPLEMENTED);
		}
	}
	else
	{
		return this->ResponseException(req, resp, CSTR("OperationNotSupported"), request->ToCString(), (Text::StringBuilderUTF8() + CSTR("No such operation WFS ")+version+' '+request).ToCString());
	}
}

Bool Map::GISWebHandler::ResponseException(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN exceptionCode, Text::CStringNN locator, Text::CStringNN exceptionText)
{
	NN<Text::String> s;
	Text::StringBuilderUTF8 sb;
	sb.Append(CSTR("<?xml version=\"1.0\" encoding=\"UTF-8\"?>"));
	sb.Append(CSTR("<ows:ExceptionReport xmlns:xs=\"http://www.w3.org/2001/XMLSchema\" xmlns:ows=\"http://www.opengis.net/ows\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" version=\"1.0.0\">\r\n"));
	sb.Append(CSTR("<ows:Exception exceptionCode="));
	s = Text::XML::ToNewAttrText(exceptionCode.v);
	sb.Append(s);
	s->Release();
	sb.Append(CSTR(" locator="));
	s = Text::XML::ToNewAttrText(locator.v);
	sb.Append(s);
	s->Release();
	sb.Append(CSTR(">\r\n"));
	sb.Append(CSTR("<ows:ExceptionText>"));
	s = Text::XML::ToNewXMLText(exceptionText.v);
	sb.Append(s);
	s->Release();
	sb.Append(CSTR("</ows:ExceptionText>\r\n"));
	sb.Append(CSTR("</ows:Exception>\r\n"));
	sb.Append(CSTR("</ows:ExceptionReport>\r\n"));
	resp->AddDefHeaders(req);
	resp->AddContentType(CSTR("application/xml"));
	this->AddHeaders(req, resp);
	return Net::WebServer::HTTPServerUtil::SendContent(req, resp, CSTR("application/xml"), sb.ToCString());

}

void Map::GISWebHandler::AddHeaders(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp)
{
}

void __stdcall Map::GISWebHandler::FreeFeature(NN<GISFeature> feature)
{
	feature->name->Release();
	MemFreeNN(feature);
}

Map::GISWebHandler::GISWebHandler()
{
	this->wgs84 = Math::CoordinateSystemManager::CreateWGS84Csys();
	this->AddService(CSTR("/ows"), Net::WebUtil::RequestMethod::HTTP_GET, OWSFunc);
	this->AddService(CSTR("/wfs"), Net::WebUtil::RequestMethod::HTTP_GET, WFSFunc);
	this->AddService(CSTR("/gwc/servcie/wmts"), Net::WebUtil::RequestMethod::HTTP_GET, WMTSFunc);
	this->AddService(CSTR("/gwc/servcie/wms"), Net::WebUtil::RequestMethod::HTTP_GET, WMSCFunc);
	this->AddService(CSTR("/gwc/service/tms/1.0.0"), Net::WebUtil::RequestMethod::HTTP_GET, TMSFunc);
}

Map::GISWebHandler::~GISWebHandler()
{
	this->assets.DeleteAll();
	this->features.FreeAll(FreeFeature);
	this->wgs84.Delete();
}

UOSInt Map::GISWebHandler::AddAsset(NN<Map::MapDrawLayer> layer)
{
	return this->assets.Add(layer);
}

Bool Map::GISWebHandler::AddFeature(Text::CStringNN featureName, UOSInt assetIndex)
{
	NN<Map::MapDrawLayer> layer;
	if (!this->assets.GetItem(assetIndex).SetTo(layer))
	{
		return false;
	}
	if (this->features.GetC(featureName).NotNull())
	{
		return false;
	}
	NN<GISFeature> feature = MemAllocNN(GISFeature);
	feature->name = Text::String::New(featureName);
	feature->layer = layer;
	this->features.PutC(featureName, feature);
	return true;
}
