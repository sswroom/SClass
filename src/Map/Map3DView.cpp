#include "Stdafx.h"
#include "Map/Map3DView.h"
#include "Data/ArrayListInt64.h"
#include "Data/ArrayListNN.hpp"
#include "Map/MapDrawLayer.h"
#include "Math/CoordinateSystemConverter.h"
#include "Math/Math_C.h"

static Media::Engine3D::SceneObjectType Map3DView_MapVecTypeToSceneObjType(Math::Geometry::Vector2D::VectorType vecType)
{
	switch (vecType)
	{
	case Math::Geometry::Vector2D::VectorType::Point:
	case Math::Geometry::Vector2D::VectorType::MultiPoint:
		return Media::Engine3D::SceneObjectType::Point;
	case Math::Geometry::Vector2D::VectorType::Polygon:
	case Math::Geometry::Vector2D::VectorType::MultiPolygon:
	case Math::Geometry::Vector2D::VectorType::CurvePolygon:
	case Math::Geometry::Vector2D::VectorType::Surface:
	case Math::Geometry::Vector2D::VectorType::MultiSurface:
		return Media::Engine3D::SceneObjectType::Polygon;
	case Math::Geometry::Vector2D::VectorType::Unknown:
	case Math::Geometry::Vector2D::VectorType::LineString:
	case Math::Geometry::Vector2D::VectorType::Polyline:
	case Math::Geometry::Vector2D::VectorType::GeometryCollection:
	case Math::Geometry::Vector2D::VectorType::CircularString:
	case Math::Geometry::Vector2D::VectorType::CompoundCurve:
	case Math::Geometry::Vector2D::VectorType::MultiCurve:
	case Math::Geometry::Vector2D::VectorType::Curve:
	case Math::Geometry::Vector2D::VectorType::PolyhedralSurface:
	case Math::Geometry::Vector2D::VectorType::Tin:
	case Math::Geometry::Vector2D::VectorType::Triangle:
	case Math::Geometry::Vector2D::VectorType::LinearRing:
	case Math::Geometry::Vector2D::VectorType::Image:
	case Math::Geometry::Vector2D::VectorType::String:
	case Math::Geometry::Vector2D::VectorType::Ellipse:
	case Math::Geometry::Vector2D::VectorType::PieArea:
	default:
		return Media::Engine3D::SceneObjectType::LineString;
	}
}

static void Map3DView_MapSceneObjectsFree(NN<Data::ArrayListNN<Media::Engine3D::SceneObject>> sceneObjs)
{
	UIntOS i = sceneObjs->GetCount();
	while (i-- > 0)
	{
		sceneObjs->GetItemNoCheck(i).Delete();
	}
	sceneObjs->Clear();
}

static Double Map3DView_MapUnitPerPixelToScale(NN<Math::CoordinateSystem> envCSys, Double mapUnitPerPixel)
{
	if (envCSys->IsProjected())
	{
		return mapUnitPerPixel * 72.0 / 0.0254;
	}
	else
	{
		return mapUnitPerPixel * 72.0 / (0.000005 * 0.0254);
	}
}

static Double Map3DView_CalcLayerScale(
	NN<Math::CoordinateSystem> envCSys,
	Math::Size2DDbl scnSize,
	Double fovDeg,
	Math::Vector3 camPos,
	Math::RectAreaDbl layerRect)
{
	Math::Coord2DDbl nearPos = camPos.GetXY();
	if (nearPos.x < layerRect.min.x)
		nearPos.x = layerRect.min.x;
	else if (nearPos.x > layerRect.max.x)
		nearPos.x = layerRect.max.x;
	if (nearPos.y < layerRect.min.y)
		nearPos.y = layerRect.min.y;
	else if (nearPos.y > layerRect.max.y)
		nearPos.y = layerRect.max.y;

	Double dx = camPos.GetX() - nearPos.x;
	Double dy = camPos.GetY() - nearPos.y;
	Double dz = camPos.GetZ();
	Double layerDist = Math_Sqrt(dx * dx + dy * dy + dz * dz);
	if (layerDist < 1.0)
		layerDist = 1.0;
	Double mapUnitPerPixel = 2.0 * layerDist * Math_Tan(fovDeg * Math::PI / 360.0) / scnSize.y;
	return Map3DView_MapUnitPerPixelToScale(envCSys, mapUnitPerPixel);
}

static Math::RectAreaDbl Map3DView_MapSceneConvertRect(
	NN<Math::CoordinateSystem> srcCSys,
	NN<Math::CoordinateSystem> destCSys,
	Math::RectAreaDbl srcRect)
{
	Math::Coord2DDbl corners[4];
	corners[0] = Math::CoordinateSystem::Convert(srcCSys, destCSys, srcRect.min);
	corners[1] = Math::CoordinateSystem::Convert(srcCSys, destCSys, Math::Coord2DDbl(srcRect.max.x, srcRect.min.y));
	corners[2] = Math::CoordinateSystem::Convert(srcCSys, destCSys, srcRect.max);
	corners[3] = Math::CoordinateSystem::Convert(srcCSys, destCSys, Math::Coord2DDbl(srcRect.min.x, srcRect.max.y));
	return Math::RectAreaDbl::GetRectArea(corners, 4);
}

static void Map3DView_MapSceneObjectsAddLayer(
	NN<Map::MapDrawLayer> layer,
	NN<Math::CoordinateSystem> envCSys,
	Math::Size2DDbl scnSize,
	Math::RectAreaDbl searchRect,
	NN<Data::ArrayListNN<Media::Engine3D::SceneObject>> sceneObjs)
{
	NN<Math::CoordinateSystem> layerCSys = layer->GetCoordinateSystem();
	Math::RectAreaDbl layerRect = searchRect;
	if (!layerCSys->Equals(envCSys))
	{
		layerRect = Map3DView_MapSceneConvertRect(envCSys, layerCSys, searchRect);
	}

	Data::ArrayListInt64 objIds;
	layer->GetObjectIdsMapXY(objIds, 0, layerRect, true);
	if (objIds.GetCount() == 0)
		return;

	NN<Map::GetObjectSess> sess = layer->BeginGetObject();
	Bool needConv = !layerCSys->Equals(envCSys);
	Math::CoordinateSystemConverter converter(layerCSys, envCSys);
	Int64 lastId = -1;
	UIntOS i = objIds.GetCount();
	while (i-- > 0)
	{
		Int64 thisId = objIds.GetItem(i);
		if (thisId == lastId)
			continue;
		lastId = thisId;

		Optional<Math::Geometry::Vector2D> vecOpt = layer->GetNewVectorById(sess, thisId);
		NN<Math::Geometry::Vector2D> vec;
		if (!vecOpt.SetTo(vec))
			continue;

		if (needConv)
		{
			vec->Convert(converter);
		}

		Data::ArrayListA<Math::Coord2DDbl> coordList;
		vec->GetCoordinates(coordList);
		if (coordList.GetCount() > 0)
		{
			Media::Engine3D::SceneObjectType objType = Map3DView_MapVecTypeToSceneObjType(vec->GetVectorType());
			UInt32 color = 0xff808080;
			if (objType == Media::Engine3D::SceneObjectType::Polygon && layer->HasPGStyle())
			{
				color = layer->GetPGStyleColor();
			}
			else if (layer->HasLineStyle())
			{
				color = layer->GetLineStyleColor();
			}

			NN<Media::Engine3D::SceneObject> sceneObj;
			NEW_CLASSNN(sceneObj, Media::Engine3D::SceneObject(objType, color));
			sceneObj->z = 0;
			UIntOS j = 0;
			UIntOS jCnt = coordList.GetCount();
			while (j < jCnt)
			{
				sceneObj->points.Add(coordList.GetItem(j));
				j++;
			}
			sceneObjs->Add(sceneObj);
		}

		vec.Delete();
	}
	layer->EndGetObject(sess);
}

static void Map3DView_MapSceneObjectsAddGroup(
	NN<Map::MapEnv> env,
	Optional<Map::MapEnv::GroupItem> group,
	NN<Math::CoordinateSystem> envCSys,
	Math::Size2DDbl scnSize,
	const Media::Engine3D::CameraState &camera,
	Math::RectAreaDbl searchRect,
	NN<Data::ArrayListNN<Media::Engine3D::SceneObject>> sceneObjs)
{
	UIntOS i = 0;
	UIntOS j = env->GetItemCount(group);
	while (i < j)
	{
		Optional<Map::MapEnv::MapItem> item = env->GetItem(group, i);
		NN<Map::MapEnv::MapItem> nnitem;
		if (item.SetTo(nnitem))
		{
			if (nnitem->itemType == Map::MapEnv::IT_GROUP)
			{
				Map3DView_MapSceneObjectsAddGroup(env, NN<Map::MapEnv::GroupItem>::ConvertFrom(nnitem), envCSys, scnSize, camera, searchRect, sceneObjs);
			}
			else if (nnitem->itemType == Map::MapEnv::IT_LAYER)
			{
				Map::MapEnv::LayerItem layerProp;
				if (env->GetLayerProp(NN<Map::MapEnv::LayerItem>::FromPtr(&layerProp), group, i))
				{
					Math::RectAreaDbl layerBounds;
					if ((layerProp.flags & Map::MapEnv::SFLG_HIDELAYER) == 0 && layerProp.layer->GetBounds(layerBounds))
					{
						NN<Math::CoordinateSystem> layerCSys = layerProp.layer->GetCoordinateSystem();
						Math::RectAreaDbl layerEnvBounds = layerBounds;
						if (!layerCSys->Equals(envCSys))
						{
							layerEnvBounds = Map3DView_MapSceneConvertRect(layerCSys, envCSys, layerBounds);
						}
						Double layerScale = Map3DView_CalcLayerScale(envCSys, scnSize, camera.fovDeg, camera.camPos, layerEnvBounds);
						if (layerProp.minScale <= layerScale && layerProp.maxScale >= layerScale)
						{
							layerProp.layer->SetDispSize(scnSize, 96.0);
							layerProp.layer->SetCurrScale(layerScale);
							Map3DView_MapSceneObjectsAddLayer(layerProp.layer, envCSys, scnSize, searchRect, sceneObjs);
						}
					}
				}
			}
		}
		i++;
	}
}

Map::Map3DView::Map3DView(NN<Map::MapEnv> env, NN<Media::Engine3D> eng3d, Math::Size2DDbl scnSize)
{
	this->env = env;
	this->eng3d = eng3d;
	this->scnSize = scnSize;
	this->mapCenter = Math::Coord2DDbl(0, 0);
	this->camDistance = 1000;
	this->vAngle = Math::PI * 0.25;
	this->hAngle = 0;
	this->fovDeg = 60.0;

	Math::RectAreaDbl bounds;
	if (env->GetBounds(nullptr, bounds))
	{
		this->mapCenter = bounds.GetCenter();
		Double initDist = bounds.GetWidth();
		if (initDist < bounds.GetHeight())
		{
			initDist = bounds.GetHeight();
		}
		if (initDist > 0)
		{
			this->camDistance = initDist * 1.8;
		}
	}
}

Map::Map3DView::~Map3DView()
{
}

void Map::Map3DView::SetScreenSize(Math::Size2DDbl scnSize)
{
	this->scnSize = scnSize;
}

void Map::Map3DView::SetCenter(Math::Coord2DDbl mapCenter)
{
	this->mapCenter = mapCenter;
}

Math::Coord2DDbl Map::Map3DView::GetCenter() const
{
	return this->mapCenter;
}

void Map::Map3DView::Orbit(Double hDiff, Double vDiff)
{
	this->hAngle += hDiff;
	this->vAngle += vDiff;
	if (this->vAngle < 0.05)
	{
		this->vAngle = 0.05;
	}
	else if (this->vAngle > Math::PI * 0.48)
	{
		this->vAngle = Math::PI * 0.48;
	}
}

void Map::Map3DView::PanScreen(Math::Coord2D<IntOS> diff)
{
	Double mapPerPixel = this->camDistance / this->scnSize.GetWidth();
	if (mapPerPixel < this->camDistance / this->scnSize.GetHeight())
	{
		mapPerPixel = this->camDistance / this->scnSize.GetHeight();
	}
	this->mapCenter.x -= IntOS2Double(diff.x) * mapPerPixel;
	this->mapCenter.y += IntOS2Double(diff.y) * mapPerPixel;
}

void Map::Map3DView::Zoom(Double ratio)
{
	this->camDistance *= ratio;
	if (this->camDistance < 10)
	{
		this->camDistance = 10;
	}
	else if (this->camDistance > 10000000.0)
	{
		this->camDistance = 10000000.0;
	}
}

void Map::Map3DView::SetVAngle(Double vAngle)
{
	this->vAngle = vAngle;
	if (this->vAngle < 0.05)
	{
		this->vAngle = 0.05;
	}
	else if (this->vAngle > Math::PI * 0.48)
	{
		this->vAngle = Math::PI * 0.48;
	}
}

void Map::Map3DView::SetHAngle(Double hAngle)
{
	this->hAngle = hAngle;
}

Double Map::Map3DView::GetVAngle() const
{
	return this->vAngle;
}

Double Map::Map3DView::GetHAngle() const
{
	return this->hAngle;
}

Bool Map::Map3DView::Render(NN<Media::Engine3DScene> scene3d, Math::Size2D<UInt32> frameSize)
{
	Double camZ = this->camDistance * Math_Sin(this->vAngle);
	Double camH = this->camDistance * Math_Cos(this->vAngle);
	Math::Vector3 targetPos(this->mapCenter, 0);
	Math::Vector3 camPos(this->mapCenter.x + camH * Math_Cos(this->hAngle), this->mapCenter.y + camH * Math_Sin(this->hAngle), camZ);

	Media::Engine3D::CameraState camera;
	camera.camPos = camPos;
	camera.targetPos = targetPos;
	camera.fovDeg = this->fovDeg;
	camera.vAngle = this->vAngle;
	camera.hAngle = this->hAngle;

	Data::ArrayListNN<Map::MapDrawLayer> layers;
	Data::ArrayListNN<Media::Engine3D::SceneObject> sceneObjs;
	Math::RectAreaDbl searchRect(
		this->mapCenter.x - this->camDistance,
		this->mapCenter.y - this->camDistance,
		this->camDistance * 2,
		this->camDistance * 2);
	NN<Math::CoordinateSystem> envCSys = this->env->GetCoordinateSystem();
	Map3DView_MapSceneObjectsAddGroup(this->env, nullptr, envCSys, this->scnSize, camera, searchRect, sceneObjs);

	if (!this->eng3d->BeginFrame(scene3d, frameSize))
	{
		Map3DView_MapSceneObjectsFree(sceneObjs);
		return false;
	}
	Bool ret = this->eng3d->RenderScene(camera, sceneObjs);
	this->eng3d->EndFrame();
	Map3DView_MapSceneObjectsFree(sceneObjs);
	return ret;
}
