// MakeObject.h : header file
//	
//
/////////////////////////////////////////////////////////////////////////////
// CMakeObject window
// class CDPoint :public CPoint
// {
// public:
// 	float x, y;
// 	CDPoint()
// 	{
// // 		x = y = 6.5;  // Camera Resolution
// 	}
// };

#define DEF_ARRAY_SIZE	5000

class CMakeObject 
{
// Construction
public:
	CMakeObject();

// Attributes
public:
	double	Theta;
	double	DrX[2][2];
	double	DrY[2][2];
	double	DrZ[2][2];
	double	DDrXYZ;
	double	SrX[2], SrY[2], SrZ[2];
	double	D[2];

	double	mLeastY[DEF_ARRAY_SIZE];
	double	mLeastX[DEF_ARRAY_SIZE];
	double	mLeastA[DEF_ARRAY_SIZE];
	double	mLeastM[DEF_ARRAY_SIZE];
	short	mLeastO;


// Operations
public:
	BOOL Line(double Point[][3], int Gasu, double InPo[][3], double *Linearity);
	BOOL Ellipse(double Point[][3], int Gasu, double InP[3], double *Xr, double *Yr);

	BOOL Circle(double Point[][3], int Gasu, double Angle[], double InP[3], double *Radius, double *Circularity);
	BOOL CircleC(double Point[][3], int Gasu, double InP[3], double *Radius, double *Circularity);
	BOOL CalculateCircle3(double Point[][3], double InP[3], double *Radius);
	BOOL CalculateCircleMoreThan4(double Point[][3], int Gasu, double InPo[3], double *Radius);

	BOOL Plane(double Point[][3], int Gasu, double InPo[4], double PointC[3], double *Flatness);
	BOOL Polynomial(double Point[][3], int Gasu, int Order, double Coef[10], double Peak[3]);

	BOOL AngleTwoLines(double FirstLine[2][3], double SencondLine[2][3],double *Theta);
	BOOL CalAngle(double DirVec1[3], double DirVec2[3], short Mode, double *Theta);
	
	BOOL InterPointTwoLines(double FirstLine[2][3], double SencondLine[2][3],double InPoint[6]);
	BOOL InterPointLineToCircle(double Line[2][3], double cXYZ[3],double Dir[3], double Rad, double InPo[2][3], short *Gasu); 
	BOOL InterPointLineToPlane(double Line[2][3], double DIR[3], double Dd, double *InPo);
	BOOL InterPointCircleToCircle(double cXYZ1[3],double DIR1[3],double Rad1, double cXYZ2[3],double DIR2[3], double Rad2, double InPo[2][3], short *Gasu); 
	bool InterPointCircleToCircle2(double cXYZ1[3],double Rad1, double cXYZ2[3], double Rad2, double InPo[2][3], short *Gasu);

	BOOL InterLinePlanePlane(double XYZ1[3],double DIR1[3],double D1, double XYZ2[3], double DIR2[3], double D2, double *InPo); 
	BOOL InterLinePlanePlane(double XYZ1[3],double DIR1[3],double D1, double XYZ2[3], double DIR2[3], double D2, double *InPo, double MinXYZ[3], double MaxXYZ[3]);
	BOOL LinePlanePlane(double XYZ1[3],double DIR1[3],double D1, double XYZ2[3], double DIR2[3], double D2, double *InPo, double MinXYZ[3], double MaxXYZ[3]); 
	BOOL InterLinePlanePlane(void); 


	double DistancePointToPoint(double Point1[3], double Point2[3]);
	double DistancePointToPoint_float(double Point1[2], double Point2[2]);
	double DistancePointToLine(double Point[3], double LinePoint[2][3], double InPo[3], short mode);
	double DistancePointToLine(double Point1[3], double Point2[3]);
	double DistancePointToCircle2d(double Pnt[3], double cXY[3], double Rad);
	double DistancePointToPlane(double Point[3], double DIR[3], double Dd, double *InPo);
	double DistanceLineToLine(double LinePoint1[2][3], double LinePoint[2][3], double InPo[6]); 

	double Getangle(double gradient1, double gradient2);
	bool GetInEllipse(double cx, double cy, double xr, double yr, double edgeX, double edgeY);

	float GetDistanceLineToDot(float gradient, float Y_value, CDPoint dot, int direction, int* x, int* y, int direction2=0, int bdNo=0, int camNo=0);
	float GetDistanceLineToDot_float(float gradient, float Y_value, CDPoint dot, int direction, float* x, float* y, int direction2=0, int bdNo=0, int camNo=0);


	int Get_roots(double a,double b,double c, double *root1,double *root2);
	int Get_circle_y(double x,double y,double r,double xk,double *yk);
	int Get_circle_x(double x,double y,double r,double yk,double *xk);
	int Get_circle_y2(double xo,double yo,double ro,double a,double xk,double yk,double *xp,double *yp);
	double Distance(double x1,double y1,double x2,double y2);
	int Get_point_1(double xo1,double yo1,double ro1,double xo2,double yo2,double ro2,double *xp1,double *yp1,double *xp2,double *yp2);
	int Get_point_2(double xo1,double yo1,double ro1,double xo2,double yo2,double ro2,double *xp1,double *yp1,double *xp2,double *yp2);
	int Get_point_3(double xo1,double yo1,double ro1,double xo2,double yo2,double ro2,double *xp1,double *yp1,double *xp2,double *yp2);

	void FittingParabolic(double* Point1, double* Point2, double* Point3, double dConst[3]);
	
	/******************************************************************************************

					The following functions are for common purposes to process data....

	*******************************************************************************************/

	void Mean_SD(double XYZ[][3], int Gasu, double MeanXYZ[3], double SD_XYZ[3], double MixXYZ[3], double MaxXYZ[3], short Order);
	BOOL InverseMatrix(int Order, double *al);
	int	minver(double a_Inv[], int RowCol, double eps, double *det);
	BOOL Get_Gauss(int m_GaussMean, double m_GaussSDev, double Gauss[]);
	BOOL Correlation(double m_Gasu, double Data1[], double Data2[], int m_GaussMean, double *MaxG, double *MinG, double Correl[]);
	BOOL FittingPolynomial( int Gasu, double m_X[], double m_Y[], short Order );
	BOOL PseudoInverse(int Order, int Gasu, double Q[], double YY[], double YTT[]);
	BOOL SortWithAngle(double Point[][3], int Gasu, double Angle[], short Mode);

	void Node(double gradient1, double y_intercept1, double gradient2, double y_intercept2, double *nodeX, double *nodeY);

// Implementation
public:
	virtual ~CMakeObject();


};

/////////////////////////////////////////////////////////////////////////////

extern CMakeObject* pMakeObj;
