namespace Map
{
	class HK80Conv
	{
	private:
		Double a;
		Double f;
		Double e2;
		Double e4;
		Double e6;
        
		Double m0;
		Double M0;

		Double A0;
		Double A2;
		Double A4;
		Double A6;

	private:
		Double calcM(Double lat);
		Double getLatp(Double M);
		Double gett(Double lat);
		Double getv(Double lat);
		Double getp(Double lat);
		Double getpsi(Double v, Double p);
		Double getM(Double HK80N);

	public:
		HK80Conv();
		~HK80Conv();

		void ToWGS84(Double *pt_hk80, Double *pt_wgs84, Int32 ptCnt); //Y, X, Y, X, ...
	};
};
