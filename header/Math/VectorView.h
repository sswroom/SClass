#ifndef _SM_MATH_VECTORVIEW
#define _SM_MATH_VECTORVIEW

namespace Math
{
	class VectorView
	{
	private:
		Int32 scnWidth;
		Int32 scnHeight;
		Double viewW;
		Double viewH;
		Double left;
		Double top;
	public:
		VectorView(Int32 scnWidth, Int32 scnHeight, Double left, Double top, Double right, Double bottom);
		~VectorView();

		void ChangeView(Int32 scnWidth, Int32 scnHeight, Double left, Double top, Double right, Double bottom);
		Double GetLeft();
		Double GetTop();
		Double GetRight();
		Double GetBottom();
		Double GetCentX();
		Double GetCentY();
		Int32 GetScnWidth();
		Int32 GetScnHeight();

		Bool InView(Double xPos, Double yPos);
		Bool ViewToScnXY(const Double *srcArr, Int32 *destArr, Int32 nPoints, Int32 ofstX, Int32 ofstY); // return inScreen
		void ViewToScnXY(Double xPos, Double yPos, Int32 *scnX, Int32 *scnY);
		void ScnXYToView(Int32 scnX, Int32 scnY, Double *xPos, Double *yPos);
		void UpdateSize(Int32 width, Int32 height);
	};
};
#endif
