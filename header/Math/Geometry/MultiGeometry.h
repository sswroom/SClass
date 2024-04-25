#ifndef _SM_MATH_GEOMETRY_MULTIGEOMETRY
#define _SM_MATH_GEOMETRY_MULTIGEOMETRY
#include "Data/ArrayListNN.h"
#include "Math/Coord2DDbl.h"
#include "Math/CoordinateSystem.h"
#include "Math/Geometry/Vector2D.h"

#ifndef VERBOSE
//#define VERBOSE
#endif
#if defined(VERBOSE)
#include <stdio.h>
#endif

namespace Math
{
	namespace Geometry
	{
		template <class T> class MultiGeometry : public Math::Geometry::Vector2D
		{
		protected:
			Data::ArrayListNN<T> geometries;
		public:
			MultiGeometry(UInt32 srid) : Math::Geometry::Vector2D(srid)
			{
			}

			virtual ~MultiGeometry()
			{
				UOSInt i = this->geometries.GetCount();
				while (i-- > 0)
				{
					this->geometries.GetItem(i).Delete();
				}
			}

			virtual void AddGeometry(NN<T> geometry)
			{
				this->geometries.Add(geometry);
			}

			UOSInt GetCount() const
			{
				return this->geometries.GetCount();
			}

			Optional<T> GetItem(UOSInt index) const
			{
				return this->geometries.GetItem(index);
			}

			Data::ArrayIterator<NN<T>> Iterator() const
			{
				return this->geometries.Iterator();
			}

			virtual Math::Coord2DDbl GetCenter() const
			{
				return this->GetBounds().GetCenter();
			}

			virtual Math::RectAreaDbl GetBounds() const
			{
				Math::RectAreaDbl bounds;
				Data::ArrayIterator<NN<T>> it = this->Iterator();
				if (!it.HasNext())
				{
					bounds.min = Math::Coord2DDbl(0, 0);
					bounds.max = Math::Coord2DDbl(0, 0);
				}
				else
				{
					bounds = it.Next()->GetBounds();
					while (it.HasNext())
					{
						bounds = bounds.MergeArea(it.Next()->GetBounds());
					}
				}
				return bounds;
			}

			virtual Double CalBoundarySqrDistance(Math::Coord2DDbl pt, OutParam<Math::Coord2DDbl> nearPt) const
			{
				Data::ArrayIterator<NN<T>> it = this->Iterator();
				if (!it.HasNext())
				{
					nearPt.Set(Math::Coord2DDbl(0, 0));
					return 1000000000;
				}
				Math::Coord2DDbl minPt;
				Double minDist = it.Next()->CalBoundarySqrDistance(pt, minPt);
				Math::Coord2DDbl thisPt;
				Double thisDist;
				while (it.HasNext())
				{
					thisDist = it.Next()->CalBoundarySqrDistance(pt, thisPt);
					if (minDist > thisDist)
					{
						minDist = thisDist;
						minPt = thisPt;
					}
				}
				nearPt.Set(minPt);
				return minDist;
			}

			virtual Double CalSqrDistance(Math::Coord2DDbl pt, OutParam<Math::Coord2DDbl> nearPt) const
			{
				Data::ArrayIterator<NN<T>> it = this->geometries.Iterator();
				if (!it.HasNext())
				{
					nearPt.Set(Math::Coord2DDbl(0, 0));
					return 1000000000;
				}
				Math::Coord2DDbl minPt;
				Double minDist = it.Next()->CalSqrDistance(pt, minPt);
				Math::Coord2DDbl thisPt;
				Double thisDist;
				while (it.HasNext())
				{
					thisDist = it.Next()->CalSqrDistance(pt, thisPt);
					if (minDist > thisDist)
					{
						minDist = thisDist;
						minPt = thisPt;
					}
				}
				nearPt.Set(minPt);
				return minDist;
			}

			virtual Double CalArea() const
			{
				Double totalArea = 0;
				Data::ArrayIterator<NN<T>> it = this->geometries.Iterator();
				while (it.HasNext())
				{
					totalArea += it.Next()->CalArea();
				}
				return totalArea;
			}

			virtual Bool JoinVector(NN<const Math::Geometry::Vector2D> vec)
			{
				if (this->GetVectorType() != vec->GetVectorType())
				{
					return false;
				}
				NN<const Math::Geometry::MultiGeometry<T>> obj = NN<const Math::Geometry::MultiGeometry<T>>::ConvertFrom(vec);
				Data::ArrayIterator<NN<T>> it = obj->Iterator();
				while (it.HasNext())
				{
					this->AddGeometry(NN<T>::ConvertFrom(it.Next()->Clone()));
				}
				return true;
			}

			virtual Bool HasZ() const
			{
				NN<T> geom;
				if (this->geometries.GetItem(0).SetTo(geom))
					return geom->HasZ();
				return false;
			}

			virtual Bool HasM() const
			{
				NN<T> geom;
				if (this->geometries.GetItem(0).SetTo(geom))
					return geom->HasM();
				return false;
			}

			virtual Bool GetZBounds(OutParam<Double> min, OutParam<Double> max) const
			{
				NN<T> geom;
				Double thisMin;
				Double thisMax;
				Double allMin;
				Double allMax;
				if (!this->geometries.GetItem(0).SetTo(geom))
					return false;
				if (!geom->GetZBounds(allMin, allMax))
					return false;

				UOSInt i = 1;
				UOSInt j = this->geometries.GetCount();
				while (i < j)
				{
					if (this->geometries.GetItem(i).SetTo(geom) && geom->GetZBounds(thisMin, thisMax))
					{
						if (thisMin < allMin)
							allMin = thisMin;
						if (thisMax > allMax)
							allMax = thisMax;
					}
					i++;
				}
				min.Set(allMin);
				max.Set(allMax);
				return true;
			}

			virtual Bool GetMBounds(OutParam<Double> min, OutParam<Double> max) const
			{
				NN<T> geom;
				Double thisMin;
				Double thisMax;
				Double allMin;
				Double allMax;
				if (!this->geometries.GetItem(0).SetTo(geom))
					return false;
				if (!geom->GetMBounds(allMin, allMax))
					return false;

				UOSInt i = 1;
				UOSInt j = this->geometries.GetCount();
				while (i < j)
				{
					if (this->geometries.GetItem(i).SetTo(geom) && geom->GetMBounds(thisMin, thisMax))
					{
						if (thisMin < allMin)
							allMin = thisMin;
						if (thisMax > allMax)
							allMax = thisMax;
					}
					i++;
				}
				min.Set(allMin);
				max.Set(allMax);
				return true;
			}

			virtual void Convert(NN<Math::CoordinateConverter> converter)
			{
				Data::ArrayIterator<NN<T>> it = this->Iterator();
				while (it.HasNext())
				{
					it.Next()->Convert(converter);
				}
				this->srid = converter->GetOutputSRID();
			}

			virtual Bool Equals(NN<const Vector2D> vec, Bool sameTypeOnly, Bool nearlyVal) const
			{
				if (this->GetVectorType() != vec->GetVectorType())
				{
					NN<T> geom;
					if (!sameTypeOnly && this->geometries.GetCount() == 1 && this->geometries.GetItem(0).SetTo(geom))
					{
						return geom->Equals(vec, sameTypeOnly, nearlyVal);
					}
#if defined(VERBOSE)
					printf("MultiGeometry: Vector type different\r\n");
#endif
					return false;
				}
				Math::Geometry::MultiGeometry<T> *obj = (Math::Geometry::MultiGeometry<T> *)vec.Ptr();
				if (obj->GetCount() != this->GetCount())
				{
#if defined(VERBOSE)
					printf("MultiGeometry: Vector count different: %d != %d\r\n", (UInt32)obj->GetCount(), (UInt32)this->GetCount());
#endif
					return false;
				}
				NN<T> v;
				Data::ArrayIterator<NN<T>> it = this->geometries.Iterator();
				UOSInt i = 0;
				while (it.HasNext())
				{
					if (!obj->GetItem(i).SetTo(v) || !it.Next()->Equals(v, sameTypeOnly, nearlyVal))
						return false;
					i++;
				}
				return true;
			}

			virtual UOSInt GetCoordinates(NN<Data::ArrayListA<Math::Coord2DDbl>> coordList) const
			{
				UOSInt ret = 0;
				Data::ArrayIterator<NN<T>> it = this->geometries.Iterator();
				while (it.HasNext())
				{
					ret += it.Next()->GetCoordinates(coordList);
				}
				return ret;
			}

			virtual Bool InsideOrTouch(Math::Coord2DDbl coord) const
			{
				Data::ArrayIterator<NN<T>> it = this->geometries.Iterator();
				while (it.HasNext())
				{
					if (it.Next()->InsideOrTouch(coord))
						return true;
				}
				return false;
			}

			virtual void SwapXY()
			{
				Data::ArrayIterator<NN<T>> it = this->geometries.Iterator();
				while (it.HasNext())
				{
					it.Next()->SwapXY();
				}
			}

			virtual void MultiplyCoordinatesXY(Double v)
			{
				Data::ArrayIterator<NN<T>> it = this->geometries.Iterator();
				while (it.HasNext())
				{
					it.Next()->MultiplyCoordinatesXY(v);
				}
			}

			virtual void SetSRID(UInt32 srid)
			{
				this->srid = srid;
				Data::ArrayIterator<NN<T>> it = this->geometries.Iterator();
				while (it.HasNext())
				{
					it.Next()->SetSRID(srid);
				}
			}

			virtual UOSInt GetPointCount() const
			{
				UOSInt ret = 0;
				Data::ArrayIterator<NN<T>> it = this->geometries.Iterator();
				while (it.HasNext())
				{
					ret += it.Next()->GetPointCount();
				}
				return ret;
			}
		};
	}
}
#endif
