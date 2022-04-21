// MakeObject.cpp : implementation file
//

#include	"stdafx.h"
#include	"MakeObj.h"

#include	"math.h"
#define		Min(a,b)	(((a)<(b)) ? (a): (b))
#define		Max(a,b)	(((a)>(b)) ? (a): (b))
#define		Error		0.000001
// #define		PI			3.141592653589793
#define		DD(x)		((x)*(x))
#define		threshold	0.001

CMakeObject* pMakeObj;
/////////////////////////////////////////////////////////////////////////////
// CMakeObject

CMakeObject::CMakeObject()
{
}

CMakeObject::~CMakeObject()
{
}

/*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

		GENERATE NEW GEOMETRIC ELEMENTS USING LEAST SQUARE METHOD

			(1)	Line
			(2) Circle
			(3) Ellipse
			(4) Plane
			(5) Curve
			(6) Are
			(7) Curverture

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*/


/*--------------------------------------------------------------------------------------------

							Start- Line Calculation Fuction

---------------------------------------------------------------------------------------------*/
BOOL CMakeObject::Line(double Point[][3], int Gasu, double InPo[][3], double *Linearity)
{
	double	MeanXYZ[3];
	double	SD_XYZ[3];
	double	sum;
//	short	Mode;
	double	MinXYZ[3];
	double	MaxXYZ[3];
	int		i;

	// Calculate Mean value and standard deviation value 

	if(Gasu <= 2)
	{
		return FALSE;
	}

	Mean_SD(Point, Gasu, MeanXYZ, SD_XYZ, MinXYZ, MaxXYZ, 3);

//	for(i=0;i<Gasu;i++){

//		Point[i][0] -= MinXYZ[0];
//		Point[i][1] -= MinXYZ[1];
//		Point[i][2] -= MinXYZ[2];

//	}

//MinXYZ[0] = 0;
//	MinXYZ[1] = 0;
//	MinXYZ[2] = 0;


	if(	(SD_XYZ[0] - SD_XYZ[1]) > -Error &&
		(SD_XYZ[0] - SD_XYZ[2]) > -Error &&
		(SD_XYZ[1] - SD_XYZ[2]) > -Error )
	{
														// y = A_xy x + B_xy, z = A_xz x + B_xz
//		InPo[0][0] = MinXYZ[0]-MinXYZ[0];
//		InPo[1][0] = MaxXYZ[0]-MinXYZ[0];
		InPo[0][0] = MinXYZ[0];
		InPo[1][0] = MaxXYZ[0];

		for(i=0;i<Gasu;i++)
		{
			if(2 * i + 1 >= DEF_ARRAY_SIZE)
				break;

			mLeastY[i]		= Point[i][1];
			mLeastX[2*i]	= Point[i][0];
			mLeastX[2*i+1]	= (double)1.0;
			mLeastO			= 2;

		}

		PseudoInverse(mLeastO, Gasu, mLeastX, mLeastY, mLeastA);
		InPo[0][1] = mLeastA[0]*InPo[0][0] + mLeastA[1]; 
		InPo[1][1] = mLeastA[0]*InPo[1][0] + mLeastA[1]; 
	
		for(i=0;i<Gasu;i++)
		{
			if (2 * i + 1 >= DEF_ARRAY_SIZE)
				break;

			mLeastY[i]		= Point[i][2];
			mLeastX[2*i]	= Point[i][0];
			mLeastX[2*i+1]	= (double)1.0;
			mLeastO			= 2;

		}

		PseudoInverse(mLeastO, Gasu, mLeastX, mLeastY, mLeastA);
		InPo[0][2] = mLeastA[0]*InPo[0][0] + mLeastA[1]; 
		InPo[1][2] = mLeastA[0]*InPo[1][0] + mLeastA[1]; 
	}
	else if((SD_XYZ[1] - SD_XYZ[0]) > -Error &&
			 (SD_XYZ[1] - SD_XYZ[2]) > -Error &&
		     (SD_XYZ[0] - SD_XYZ[2]) > -Error )
	{
														// x = A_yx y + B_yx, z = A_yz y + B_yz
//		InPo[0][1] = MinXYZ[1]-MinXYZ[1];
//		InPo[1][1] = MaxXYZ[1]-MinXYZ[1];
		InPo[0][1] = MinXYZ[1];
		InPo[1][1] = MaxXYZ[1];

		for(i=0;i<Gasu;i++)
		{
			if (2 * i + 1 >= DEF_ARRAY_SIZE)
				break;

			mLeastY[i]		= Point[i][0];
			mLeastX[i*2]	= Point[i][1];
			mLeastX[i*2+1]	= (double)1.0;
			mLeastO			= 2;
		}
		
		PseudoInverse(mLeastO, Gasu, mLeastX, mLeastY, mLeastA);
		InPo[0][0] = mLeastA[0]*MinXYZ[1] + mLeastA[1]; 
		InPo[1][0] = mLeastA[0]*MaxXYZ[1] + mLeastA[1]; 
		
		for(i=0;i<Gasu;i++)
		{
			if (2 * i + 1 >= DEF_ARRAY_SIZE)
				break;

			mLeastY[i]		= Point[i][2];
			mLeastX[2*i]	= Point[i][1];
			mLeastX[i*2+1]	= (double)1.0;
			mLeastO			= 2;
		}

		PseudoInverse(mLeastO, Gasu, mLeastX, mLeastY, mLeastA);
		InPo[0][2] = mLeastA[0]*MinXYZ[1] + mLeastA[1]; 
		InPo[1][2] = mLeastA[0]*MaxXYZ[1] + mLeastA[1]; 

	}
	else if((SD_XYZ[2] - SD_XYZ[0]) > -Error &&
			 (SD_XYZ[2] - SD_XYZ[1]) > -Error &&
		     (SD_XYZ[0] - SD_XYZ[1]) > -Error )
	{
														// x = A_zx z + B_zx, y = A_zy z + B_zy
		InPo[0][2] = MinXYZ[2]-MinXYZ[2];
		InPo[1][2] = MaxXYZ[2]-MinXYZ[2];

		for(i=0;i<Gasu;i++)
		{
			if (2 * i + 1 >= DEF_ARRAY_SIZE)
				break;

			mLeastY[i]		= Point[i][0];
			mLeastX[i*2]	= Point[i][2];
			mLeastX[i*2+1]	= (double)1.0;
			mLeastO			= 2;
		}

		PseudoInverse(mLeastO, Gasu, mLeastX, mLeastY, mLeastA);
		InPo[0][0] = mLeastA[0]*MinXYZ[2] + mLeastA[1]; 
		InPo[1][0] = mLeastA[0]*MaxXYZ[2] + mLeastA[1]; 

		for(i=0;i<Gasu;i++)
		{
			if (2 * i + 1 >= DEF_ARRAY_SIZE)
				break;

			mLeastY[i]		= Point[i][1];
			mLeastX[i*2]	= Point[i][2];
			mLeastX[i*2+1]	= (double)1.0;
			mLeastO			= 2;
		}

		PseudoInverse(mLeastO, Gasu, mLeastX, mLeastY, mLeastA);
		InPo[0][1] = mLeastA[0]*MinXYZ[1] + mLeastA[1]; 
		InPo[1][1] = mLeastA[0]*MaxXYZ[1] + mLeastA[1]; 

	}
	else if((SD_XYZ[0] - SD_XYZ[2]) > -Error &&
			 (SD_XYZ[0] - SD_XYZ[1]) > -Error &&
		     (SD_XYZ[2] - SD_XYZ[1]) > -Error )
	{
														// z = A_xz x + B_xz, y = A_xy x + B_xy
		InPo[0][0] = MinXYZ[0]-MinXYZ[0];
		InPo[1][0] = MaxXYZ[0]-MinXYZ[0];

		for(i=0;i<Gasu;i++)
		{
			if (2 * i + 1 >= DEF_ARRAY_SIZE)
				break;

			mLeastY[i]		= Point[i][2];
			mLeastX[i*2]	= Point[i][0];
			mLeastX[i*2+1]	= (double)1.0;
			mLeastO			= 2;
		}

		PseudoInverse(mLeastO, Gasu, mLeastX, mLeastY, mLeastA);
		InPo[0][2] = mLeastA[0]*MinXYZ[0] + mLeastA[1]; 
		InPo[1][2] = mLeastA[0]*MaxXYZ[0] + mLeastA[1]; 

		for(i=0;i<Gasu;i++)
		{
			if (2 * i + 1 >= DEF_ARRAY_SIZE)
				break;

			mLeastY[i]		= Point[i][1];
			mLeastX[i*2]	= Point[i][0];
			mLeastX[i*2+1]	= (double)1.0;
			mLeastO			= 2;
		}

		PseudoInverse(mLeastO, Gasu, mLeastX, mLeastY, mLeastA);
		InPo[0][1] = mLeastA[0]*MinXYZ[0] + mLeastA[1]; 
		InPo[1][1] = mLeastA[0]*MaxXYZ[0] + mLeastA[1]; 

	}
	else if((SD_XYZ[1] - SD_XYZ[2]) > -Error &&
			 (SD_XYZ[1] - SD_XYZ[0]) > -Error &&
		     (SD_XYZ[2] - SD_XYZ[0]) > -Error )
	{
														// z = A_yz y + B_yz, x = A_yx y + B_yx
		InPo[0][1] = MinXYZ[1]-MinXYZ[1];
		InPo[1][1] = MaxXYZ[1]-MinXYZ[1];

		for(i=0;i<Gasu;i++)
		{
			if (2 * i + 1 >= DEF_ARRAY_SIZE)
				break;

			mLeastY[i]		= Point[i][2];
			mLeastX[i*2]	= Point[i][1];
			mLeastX[i*2+1]	= (double)1.0;
			mLeastO			= 2;
		}

		PseudoInverse(mLeastO, Gasu, mLeastX, mLeastY, mLeastA);
		InPo[0][2] = mLeastA[0]*MinXYZ[1] + mLeastA[1]; 
		InPo[1][2] = mLeastA[0]*MaxXYZ[1] + mLeastA[1]; 
	
		for(i=0;i<Gasu;i++)
		{
			if (2 * i + 1 >= DEF_ARRAY_SIZE)
				break;

			mLeastY[i]		= Point[i][0];
			mLeastX[i*2]	= Point[i][1];
			mLeastX[i*2+1]	= (double)1.0;
			mLeastO			= 2;
		}

		PseudoInverse(mLeastO, Gasu, mLeastX, mLeastY, mLeastA);
		InPo[0][0] = mLeastA[0]*MinXYZ[1] + mLeastA[1]; 
		InPo[1][0] = mLeastA[0]*MaxXYZ[1] + mLeastA[1]; 

	}
	else if((SD_XYZ[2] - SD_XYZ[1]) > -Error &&
			 (SD_XYZ[2] - SD_XYZ[0]) > -Error &&
		     (SD_XYZ[1] - SD_XYZ[0]) > -Error )
	{
														// y = A_zy z + B_zy, x = A_zx z + B_zx
		InPo[0][2] = MinXYZ[2]-MinXYZ[2];
		InPo[1][2] = MaxXYZ[2]-MinXYZ[2];

		for(i=0;i<Gasu;i++)
		{
			if (2 * i + 1 >= DEF_ARRAY_SIZE)
				break;

			mLeastY[i]		= Point[i][1];
			mLeastX[i*2]	= Point[i][2];
			mLeastX[i*2+1]	= (double)1.0;
			mLeastO			= 2;
		}

		PseudoInverse(mLeastO, Gasu, mLeastX, mLeastY, mLeastA);
		InPo[0][1] = mLeastA[0]*MinXYZ[2] + mLeastA[1]; 
		InPo[1][1] = mLeastA[0]*MaxXYZ[2] + mLeastA[1]; 
	
		for(i=0;i<Gasu;i++)
		{
			if (2 * i + 1 >= DEF_ARRAY_SIZE)
				break;

			mLeastY[i]		= Point[i][0];
			mLeastX[i*2]	= Point[i][2];
			mLeastX[i*2+1]	= (double)1.0;
			mLeastO			= 2;
		}

		PseudoInverse(mLeastO, Gasu, mLeastX, mLeastY, mLeastA);
		InPo[0][0] = mLeastA[0]*MinXYZ[2] + mLeastA[1]; 
		InPo[1][0] = mLeastA[0]*MaxXYZ[2] + mLeastA[1]; 

	}

///or(i=0;i<Gasu;i++){

//		Point[i][0] += MinXYZ[0];
//		Point[i][1] += MinXYZ[1];
//		Point[i][2] += MinXYZ[2];

//	}

//	InPo[0][0] += MinXYZ[0];
//	InPo[0][1] += MinXYZ[1];
//	InPo[0][2] += MinXYZ[2];
	
//	InPo[1][0] += MinXYZ[0];
//	InPo[1][1] += MinXYZ[1];
//	InPo[1][2] += MinXYZ[2];

	//====================== The following part is to calculate the LINEARITY ==============//

	double	tInPo[2][3], t2InPo[3],t3InPo[3], dis, Dir[3], Dir2[3];

	// for the xy plane

	if( SD_XYZ[0] < 0.00001 || SD_XYZ[1] < 0.00001)
	{
		MinXYZ[0] = (double) 0.0;
		MaxXYZ[0] = (double) 0.0;
	}
	else
	{
		for(i=0;i<3;i++)
		{
			for(int j=0;j<2;j++)
			{
				tInPo[j][i] = InPo[j][i];
			}
		}

		tInPo[0][2] = (double) 0.0;
		tInPo[1][2] = (double) 0.0;
		MinXYZ[0]	= (double) 1000.0;
		MaxXYZ[0]	= (double) -1000.0;

		for(i=0;i<Gasu;i++)
		{
			t2InPo[0] = Point[i][0];
			t2InPo[1] = Point[i][1];
			t2InPo[2] = Point[i][2];

			dis = DistancePointToLine(t2InPo, tInPo, t3InPo, 1);

			if( i == 0 )
			{
				sum = (double) 0.0;
				Dir[0] = Point[i][0] - t3InPo[0];
				Dir[1] = Point[i][1] - t3InPo[1];
				sum = sqrt( DD(Dir[0]) + DD(Dir[1]));
				Dir[0]/= sum;
				Dir[1]/= sum;
			}
			else
			{
				sum = (double) 0.0;
				Dir2[0] = Point[i][0] - t3InPo[0];
				Dir2[1] = Point[i][1] - t3InPo[1];
				sum = sqrt( DD(Dir2[0]) + DD(Dir2[1]));
				Dir2[0]/= sum;
				Dir2[1]/= sum;

				if( (Dir[0]*Dir2[0] + Dir[1]*Dir2[1]) < 0 )
				{
					dis *= (double)-1.0;
				}
			}

			MinXYZ[0] = Min( MinXYZ[0], dis);
			MaxXYZ[0] = Max( MaxXYZ[0], dis);
		}
	}

	// for the yz plane

	if( SD_XYZ[1] < 0.00001 || SD_XYZ[2] < 0.00001)
	{
		MinXYZ[1] = (double) 0.0;
		MaxXYZ[1] = (double) 0.0;
	}
	else
	{
		for(i=0;i<3;i++)
		{
			for(int j=0;j<2;j++)
			{
				tInPo[j][i] = InPo[j][i];
			}
		}

		tInPo[0][0] = (double) 0.0;
		tInPo[1][0] = (double) 0.0;
		MinXYZ[1]	= (double) 1000.0;
		MaxXYZ[1]	= (double) -1000.0;

		for(i=0;i<Gasu;i++)
		{
			t2InPo[0] = Point[i][0];
			t2InPo[1] = Point[i][1];
			t2InPo[2] = Point[i][2];

			dis = DistancePointToLine(t2InPo, tInPo, t3InPo, 2);

			if( i == 0 )
			{
				sum = (double) 0.0;
				Dir[1] = Point[i][1] - t3InPo[1];
				Dir[2] = Point[i][2] - t3InPo[2];
				sum = sqrt( DD(Dir[1]) + DD(Dir[2]));
				Dir[1]/= sum;
				Dir[2]/= sum;
			}
			else
			{
				sum = (double) 0.0;
				Dir2[1] = Point[i][1] - t3InPo[1];
				Dir2[2] = Point[i][2] - t3InPo[2];
				sum = sqrt( DD(Dir2[1]) + DD(Dir2[2]));
				Dir2[1]/= sum;
				Dir2[2]/= sum;

				if( (Dir[1]*Dir2[1] + Dir[2]*Dir2[2]) < 0 )
				{
					dis *= (double)-1.0;
				}
			}

			MinXYZ[1] = Min( MinXYZ[1], dis);
			MaxXYZ[1] = Max( MaxXYZ[1], dis);
		}
	}
		
	// for the zx plane

	if( SD_XYZ[0] < 0.00001 || SD_XYZ[2] < 0.00001)
	{
		MinXYZ[2] = (double) 0.0;
		MaxXYZ[2] = (double) 0.0;
	}
	else
	{
		for(i=0;i<3;i++)
		{
			for(int j=0;j<2;j++)
			{
				tInPo[j][i] = InPo[j][i];
			}
		}

		tInPo[0][1] = (double) 0.0;
		tInPo[1][1] = (double) 0.0;
		MinXYZ[2]	= (double) 1000.0;
		MaxXYZ[2]	= (double) -1000.0;

		for(i=0;i<Gasu;i++)
		{
			t2InPo[0] = Point[i][0];
			t2InPo[1] = Point[i][1];
			t2InPo[2] = Point[i][2];

			dis = DistancePointToLine(t2InPo, tInPo, t3InPo, 3);

			if( i == 0 )
			{
				sum = (double) 0.0;
				Dir[2] = Point[i][2] - t3InPo[2];
				Dir[0] = Point[i][0] - t3InPo[0];
				sum = sqrt( DD(Dir[2]) + DD(Dir[0]));
				Dir[2]/= sum;
				Dir[0]/= sum;
			}
			else
			{
				sum = (double) 0.0;
				Dir2[2] = Point[i][2] - t3InPo[2];
				Dir2[0] = Point[i][0] - t3InPo[0];
				sum = sqrt( DD(Dir2[0]) + DD(Dir2[2]));
				Dir2[0]/= sum;
				Dir2[2]/= sum;

				if( (Dir[0]*Dir2[0] + Dir[2]*Dir2[2]) < 0 )
				{
					dis *= (double)-1.0;
				}
			}

			MinXYZ[2] = Min( MinXYZ[2], dis);
			MaxXYZ[2] = Max( MaxXYZ[2], dis);
		}
	}

	*Linearity = fabs(MinXYZ[0])+fabs(MaxXYZ[0]);
	*Linearity = Max( *Linearity, fabs(MinXYZ[1])+fabs(MaxXYZ[1]));
	*Linearity = Max( *Linearity, fabs(MinXYZ[2])+fabs(MaxXYZ[2]));

	return TRUE;

}

/*--------------------------------------------------------------------------------------------

							Start - Circle Calculation Fuction

  앞으로 고쳐야 할 것은 측정점들을 이용하여 평면을 구성한 후 원의 존재하는 면의 식을 얻어
  좌표변환을 한다. 그런후에 아래 함수들을 이용하여 원에 맞춤을 한 후 중심점을 면의 식으로
  부터 구한다. 

---------------------------------------------------------------------------------------------*/

BOOL CMakeObject::Circle(double Point[][3], int Gasu, double Angle[], double InP[3], double *Radius, double *Circularity)
{

	if(CircleC(Point, Gasu, InP, Radius, Circularity))
	{
		SortWithAngle(Point, Gasu, Angle, 0);

		return TRUE;
	}
	else 
	{
		return FALSE;
	}
}

double CMakeObject::Getangle(double gradient1, double gradient2)
{
	double tanVal=0.0;
	float t=0.0, u=0.0;
	double angle;

	// 	tanVal = (tan(gradient1)-tan(gradient2)) / (1+tan(gradient1)+tan(gradient2));
	// 	angle = atan(tanVal);
	// 
	// 	angle *= (180.0/M_PI);
	// 
	// 	angle = fabs(angle);
	// 	if(angle>=180) angle = fabs(180.f-angle);
	// 	
	// 	return (float)fabs(angle);
	// 
	angle = atan((gradient2-gradient1) / (1+gradient1*gradient2));
	angle *= (180.0/PI); 

	if(angle>=180) angle = (180.f-angle);

	return (double)(angle);
}


BOOL CMakeObject::CircleC(double Point[][3], int Gasu, double InP[3], double *Radius, double *Circularity)
{
	CString str;
	int		i;

	if( Gasu < 3)
	{
		return FALSE;

	}
	else if(Gasu == 3)
	{
		if(!CalculateCircle3(Point, InP, Radius))
		{
			return FALSE;
		}	
	}
	else
	{
		CalculateCircleMoreThan4(Point, Gasu, InP, Radius);	
	}

	/*=====================================================================================

			The following part is to calculate the circularity.

	======================================================================================*/

	double min, max, dis;

	max = (double)-10000;
	min = (double)10000;

	for(i=0;i<Gasu;i++)
	{
		dis  = sqrt( DD(Point[i][0]-InP[0]) + DD(Point[i][1]-InP[1]) + DD(Point[i][2]-InP[2]));
		dis -= *Radius;

		max = Max(max, dis);
		min = Min(min, dis);
	}

	*Circularity = fabs(max - min);

	return TRUE;

}

BOOL CMakeObject::CalculateCircle3(double Point[][3], double InPo[3], double *Radius)
{
//	int		i, j, k;
	double	cDeter;

	mLeastX[0] = 2.*Point[0][0];	mLeastX[1] = 2.*Point[0][1];	mLeastX[2] = 1.;
	mLeastX[3] = 2.*Point[1][0];	mLeastX[4] = 2.*Point[1][1];	mLeastX[5] = 1.;
	mLeastX[6] = 2.*Point[2][0];	mLeastX[7] = 2.*Point[2][1];	mLeastX[8] = 1.;

	cDeter  = mLeastX[0]*(mLeastX[4]*mLeastX[8]-mLeastX[7]*mLeastX[5]);
	cDeter -= mLeastX[1]*(mLeastX[3]*mLeastX[8]-mLeastX[6]*mLeastX[5]);
	cDeter += mLeastX[2]*(mLeastX[3]*mLeastX[7]-mLeastX[4]*mLeastX[6]);

	if( fabs(cDeter) < Error )
	{
		return FALSE;
	}

	mLeastX[0] = DD(Point[0][0])+DD(Point[0][1]);	mLeastX[1] = 2*Point[0][1];	mLeastX[2] = 1;
	mLeastX[3] = DD(Point[1][0])+DD(Point[1][1]);	mLeastX[4] = 2*Point[1][1];	mLeastX[5] = 1;
	mLeastX[6] = DD(Point[2][0])+DD(Point[2][1]);	mLeastX[7] = 2*Point[2][1];	mLeastX[8] = 1;

	InPo[0]  = mLeastX[0]*(mLeastX[4]*mLeastX[8]-mLeastX[7]*mLeastX[5]);
	InPo[0] -= mLeastX[1]*(mLeastX[3]*mLeastX[8]-mLeastX[6]*mLeastX[5]);
	InPo[0] += mLeastX[2]*(mLeastX[3]*mLeastX[7]-mLeastX[4]*mLeastX[6]);

	InPo[0] /= cDeter;

	mLeastX[0] = 2*Point[0][0];	mLeastX[1] = DD(Point[0][0])+DD(Point[0][1]);	mLeastX[2] = 1;
	mLeastX[3] = 2*Point[1][0];	mLeastX[4] = DD(Point[1][0])+DD(Point[1][1]);	mLeastX[5] = 1;
	mLeastX[6] = 2*Point[2][0];	mLeastX[7] = DD(Point[2][0])+DD(Point[2][1]);	mLeastX[8] = 1;
	
	InPo[1]  = mLeastX[0]*(mLeastX[4]*mLeastX[8]-mLeastX[7]*mLeastX[5]);
	InPo[1] -= mLeastX[1]*(mLeastX[3]*mLeastX[8]-mLeastX[6]*mLeastX[5]);
	InPo[1] += mLeastX[2]*(mLeastX[3]*mLeastX[7]-mLeastX[4]*mLeastX[6]);
	InPo[1] /= cDeter;

	mLeastX[0] = 2*Point[0][0];	mLeastX[1] = 2*Point[0][1];	mLeastX[2] = DD(Point[0][0])+DD(Point[0][1]);
	mLeastX[3] = 2*Point[1][0];	mLeastX[4] = 2*Point[1][1];	mLeastX[5] = DD(Point[1][0])+DD(Point[1][1]);
	mLeastX[6] = 2*Point[2][0];	mLeastX[7] = 2*Point[2][1];	mLeastX[8] = DD(Point[2][0])+DD(Point[2][1]);
	
	*Radius  = mLeastX[0]*(mLeastX[4]*mLeastX[8]-mLeastX[7]*mLeastX[5]);
	*Radius -= mLeastX[1]*(mLeastX[3]*mLeastX[8]-mLeastX[6]*mLeastX[5]);
	*Radius += mLeastX[2]*(mLeastX[3]*mLeastX[7]-mLeastX[4]*mLeastX[6]);

	*Radius /= cDeter;
	
	*Radius = sqrt(*Radius+DD(InPo[0])+DD(InPo[1]));
	InPo[2] = Point[0][2];

	return TRUE;

}

BOOL CMakeObject::CalculateCircleMoreThan4(double Point[][3], int Gasu, double InPo[3], double *Radius)
{
	int i = 0;
	for(i=0;i<Gasu;i++)
	{
		if (i * 3 + 2 >= DEF_ARRAY_SIZE)
			break;

		mLeastY[i]		= DD(Point[i][0])+DD(Point[i][1]);
		mLeastX[3*i]	= 2.0*Point[i][0];
		mLeastX[3*i+1]  = 2.0*Point[i][1];
		mLeastX[3*i+2]	= (double)1.0;
		mLeastO			= 3;

	}

	PseudoInverse(mLeastO, Gasu, mLeastX, mLeastY, mLeastA);

	InPo[0] = mLeastA[0];
	InPo[1] = mLeastA[1];
	*Radius = sqrt(mLeastA[2]+DD(InPo[0])+DD(InPo[1]));
	InPo[2] = Point[0][2];

	return TRUE;
	
}

/*--------------------------------------------------------------------------------------------

							Start- Plane Calculation Fuction

---------------------------------------------------------------------------------------------*/
BOOL CMakeObject::Plane(double Point[][3], int Gasu, double InPo[4], double PointC[3], double *Flatness)
{
	int		i;
//	int		j;
	double	MeanXYZ[3];
	double	SD_XYZ[3];
	double	sum;
//	short	Mode;
	double	MinXYZ[3];
	double	MaxXYZ[3];

	// Calculate Mean value and standard deviation value 

	if(Gasu <= 2)
	{
		return FALSE;
	}

	Mean_SD(Point, Gasu, MeanXYZ, SD_XYZ, MinXYZ, MaxXYZ, 3);

	// SD_XYZ[0] = Sx, SD_XYZ[1] = Sy, SD_XYZ[2] = Sz

	if(	SD_XYZ[2] < SD_XYZ[0] && SD_XYZ[2] < SD_XYZ[1])
	{ 	// z = Ax + By + C
		for(i=0;i<Gasu;i++)
		{
			if (i * 3 + 2 >= DEF_ARRAY_SIZE)
				break;

			mLeastY[i]		= Point[i][2];
			mLeastX[i*3]	= Point[i][0];
			mLeastX[i*3+1]	= Point[i][1];
			mLeastX[i*3+2]	= (double)1.0;
			mLeastO			= 3;
		}

		PseudoInverse(mLeastO, Gasu, mLeastX, mLeastY, mLeastA);

		sum = DD(mLeastA[0])+DD(mLeastA[1])+(double)1.0;

		InPo[0] = -mLeastA[0]/sum;
		InPo[1] = -mLeastA[1]/sum;
		InPo[2] = (double)1/sum;
		InPo[3] = mLeastA[2]/sum;

		PointC[0] = MeanXYZ[0];
		PointC[1] = MeanXYZ[1];
		PointC[2] = (InPo[3] - MeanXYZ[0]*InPo[0] - MeanXYZ[1]*InPo[1] )/InPo[2];
	
	}
	else if(SD_XYZ[1] < SD_XYZ[0] && SD_XYZ[1] < SD_XYZ[2])
	{
		for(i=0;i<Gasu;i++)
		{
			if (i * 3 + 2 >= DEF_ARRAY_SIZE)
				break;

			mLeastY[i]		= Point[i][1];
			mLeastX[i*3]	= Point[i][0];
			mLeastX[i*3+1]	= Point[i][2];
			mLeastX[i*3+2]	= (double)1.0;
			mLeastO			= 3;
		}

		PseudoInverse(mLeastO, Gasu, mLeastX, mLeastY, mLeastA);

		sum = DD(mLeastA[0])+DD(mLeastA[1])+(double)1.0;

		InPo[0] = -mLeastA[0]/sum;
		InPo[1] =  (double)1/sum;
		InPo[2] = -mLeastA[1]/sum;
		InPo[3] =  mLeastA[2]/sum;
	
		PointC[0] = MeanXYZ[0];
		PointC[2] = MeanXYZ[2];
		PointC[1] = (InPo[3] - MeanXYZ[0]*InPo[0] - MeanXYZ[2]*InPo[2] )/InPo[1];

	}
	else
	{													
		for(i=0;i<Gasu;i++)
		{
			if (i * 3 + 2 >= DEF_ARRAY_SIZE)
				break;

			mLeastY[i]		= Point[i][0];
			mLeastX[i*3]	= Point[i][1];
			mLeastX[i*3+1]	= Point[i][2];
			mLeastX[i*3+2]	= (double)1.0;
			mLeastO			= 3;
		}

		PseudoInverse(mLeastO, Gasu, mLeastX, mLeastY, mLeastA);

		sum = DD(mLeastA[0])+DD(mLeastA[1])+(double)1.0;

		InPo[0] = (double)1/sum;
		InPo[1] = -mLeastA[0]/sum;
		InPo[2] = -mLeastA[1]/sum;
		InPo[3] = mLeastA[2]/sum;

		PointC[1] = MeanXYZ[1];
		PointC[2] = MeanXYZ[2];
		PointC[0] = (InPo[3] - MeanXYZ[1]*InPo[1] - MeanXYZ[2]*InPo[2] )/InPo[0];
	}

	// In the following part, the variable 'sum' is used to representing the distance variable
	// to save memory ....

	MinXYZ[0] = (double)100.0;
	MaxXYZ[0] = (double)-100.0;

	for(i=0; i<Gasu; i++)
	{
		sum = (double)0.0;

		for(int j=0; j<3; j++)
			sum += InPo[j]*Point[i][j];

		sum -= InPo[3];

		MinXYZ[0] = Min(sum,MinXYZ[0]);
		MaxXYZ[0] = Max(sum,MaxXYZ[0]);

	}

	*Flatness = fabs(MinXYZ[0])+fabs(MaxXYZ[0]);

	return TRUE;

}

/*--------------------------------------------------------------------------------------------

							Start- Ellipse Calculation Fuction

---------------------------------------------------------------------------------------------*/
BOOL CMakeObject::Ellipse(double Point[][3], int Gasu, double InP[3], double *Xr, double *Yr)
{
	double	MeanXYZ[3];
	double	SD_XYZ[3];
//	double	sum;
//	short	Mode;
	double	MinXYZ[3];
	double	MaxXYZ[3];

	double	XRR, YRR;

	int i;
	
	if( Gasu < 4) return FALSE;

	Mean_SD(Point, Gasu, MeanXYZ, SD_XYZ, MinXYZ, MaxXYZ, 3);

	for(i=0;i<Gasu;i++)
	{
		if(4 * i + 3 >= DEF_ARRAY_SIZE)
			break;

		mLeastY[i]		= DD(Point[i][0]-MeanXYZ[0]);
		mLeastX[4*i]	= Point[i][0]-MeanXYZ[0];
		mLeastX[4*i+1]  = Point[i][1]-MeanXYZ[1];
		mLeastX[4*i+2]	= DD(mLeastX[4*i+1]);
		mLeastX[4*i+3]	= (double)1.0;
		mLeastO			= 4;
	}

	PseudoInverse(mLeastO, Gasu, mLeastX, mLeastY, mLeastA);

	InP[0] = mLeastA[0]/(double)2.0;
	InP[1] = -mLeastA[1]/((double)2.0*mLeastA[2]);
	InP[2] = MeanXYZ[2];

	XRR = mLeastA[3]-mLeastA[2]*DD(InP[1]) + DD(InP[0]);
	if(XRR < 0) 
	{
		return FALSE;
	}

	XRR = sqrt(XRR);

	if( fabs(mLeastA[2]) < (double)0.0000001)
	{
		return FALSE;
	}
	
	YRR = -DD(XRR)/mLeastA[2]; 

	if( YRR < 0)
	{
		return FALSE;
	}

	YRR = sqrt(YRR);

	*Xr = XRR;
	*Yr = YRR;
		
	for(i=0;i<3;i++)
	{
		InP[i] += MeanXYZ[i];
	}

	return TRUE;

}
/*--------------------------------------------------------------------------------------------

							Start- Polynomial Calculation Fuction

---------------------------------------------------------------------------------------------*/

BOOL CMakeObject::Polynomial(double Point[][3], int Gasu, int Order, double Coef[10], double Peak[3])
{
	double	MeanXYZ[3];
	double	SD_XYZ[3];
//	double	sum;
//	short	Mode;
	double	MinXYZ[3];
	double	MaxXYZ[3];

	Mean_SD(Point, Gasu, MeanXYZ, SD_XYZ, MinXYZ, MaxXYZ, 3);

	if( Gasu < Order+1 )
	{
		return FALSE;
	}
		
	for(int i=0;i<Gasu;i++)
	{
		mLeastY[i]		= Point[i][0]-MeanXYZ[0];

		for(int j=0;j<=Order;j++)
		{
			mLeastX[i*(Order+1)+j]	= pow(Point[i][1]-MeanXYZ[1],j);
		}

		mLeastO			= Order+1;

	}

	PseudoInverse(mLeastO, Gasu, mLeastX, mLeastY, mLeastA);

	memcpy(Coef, mLeastA, sizeof(double)*(Order+1));

	if(Order == 2)
	{
		Peak[1] = - mLeastA[1]/(2*mLeastA[0]);
		Peak[0] = mLeastA[0] + mLeastA[1]*Peak[1]+mLeastA[2]*DD(Peak[1]);
		Peak[1] += MeanXYZ[1];
		Peak[0] += MeanXYZ[0];
	}

	return TRUE;

}


/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		CALCULATE THE RELATIONS BETWEEN SELECTED OBJECTS

			(1)	Angle
			(2) Intersection point between two lines
			(3) Intersection points between a line and a circle

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/


/*=============================================================================================

  The following function is to find the angle between two lines. In this function, two points
  on each line be required.

  ============================================================================================*/

BOOL CMakeObject::AngleTwoLines(double FirstLine[2][3], double SencondLine[2][3],double *Theta)
{
	double	Dir1[3]; 
	double	Dir2[3];
	double	Distance;

	int i;

	Distance = 0.0;

	for(i=0; i<3; i++)
	{
		Dir1[i]		= FirstLine[1][i]-FirstLine[0][i];
		Distance	+= DD(Dir1[i]);
	}

	Distance = sqrt(Distance);

	for( i=0; i<3; i++)
	{
		Dir1[i]		/= Distance;
	}

	Distance = 0.0;

	for( i=0; i<3; i++)
	{
		Dir2[i]		= SencondLine[1][i]-SencondLine[0][i];
		Distance	+= DD(Dir2[i]);
	}

	Distance = sqrt(Distance);

	for( i=0; i<3; i++)
	{
		Dir2[i]		/= Distance;
	}

	CalAngle(Dir1, Dir2, 0, Theta);

	return TRUE;

}

/*=========================================================================================

  This function is to find the angle between two vectors. If Mode is to 0, the unit of the
  return value is radian, otherwise degree.

 ============================================================================================*/

BOOL CMakeObject::CalAngle(double DirVec1[3], double DirVec2[3], short Mode, double *Theta)
{
							
	*Theta = acos( DirVec1[0]*DirVec2[0] + DirVec1[1]*DirVec2[1]+DirVec1[2]*DirVec2[2]);

	if(Mode == 0)
	{
		*Theta	= *Theta*180/PI;
	}

	return TRUE;
}

/*==============================================================================================

	The following function is to find the intersection point between two lines in 3D space.
	In order to determine the intersection point, two points on each selected line are required.

	*InPo : Intersection Point
	If there is no intersection point, FALSE will be returned.

================================================================================================*/
BOOL CMakeObject::InterPointTwoLines(double FirstLine[2][3], double SecondLine[2][3],double InPo[6])
{

	DrX[0][0]  = FirstLine[0][0]-SecondLine[0][0]; 
	DrY[0][0]  = FirstLine[0][1]-SecondLine[0][1]; 
	DrZ[0][0]  = FirstLine[0][2]-SecondLine[0][2]; 

	DrX[0][1]  = FirstLine[1][0]-SecondLine[0][0]; 
	DrY[0][1]  = FirstLine[1][1]-SecondLine[0][1]; 
	DrZ[0][1]  = FirstLine[1][2]-SecondLine[0][2]; 

	DrX[1][0]  = FirstLine[0][0]-SecondLine[1][0]; 
	DrY[1][0]  = FirstLine[0][1]-SecondLine[1][1]; 
	DrZ[1][0]  = FirstLine[0][2]-SecondLine[1][2]; 

	DrX[1][1]  = FirstLine[1][0]-SecondLine[1][0]; 
	DrY[1][1]  = FirstLine[1][1]-SecondLine[1][1]; 
	DrZ[1][1]  = FirstLine[1][2]-SecondLine[1][2]; 

	DDrXYZ = sqrt( DD(DrX[0][0])+DD(DrY[0][0])+DD(DrZ[0][0]) );

	if(DDrXYZ < Error)
	{
		InPo[0]	= FirstLine[0][0];
		InPo[1]	= FirstLine[0][1];
		InPo[2]	= FirstLine[0][2];
		return TRUE;
	}
	else
	{
		DrX[0][0] /= DDrXYZ; DrY[0][0] /= DDrXYZ;DrZ[0][0] /= DDrXYZ;
	}

	DDrXYZ = sqrt( DD(DrX[0][1])+DD(DrY[0][1])+DD(DrZ[0][1]) );

	if(DDrXYZ < Error)
	{
		InPo[0]	= FirstLine[1][0];
		InPo[1]	= FirstLine[1][1];
		InPo[2]	= FirstLine[1][2];
		return TRUE;
	}
	else
	{
		DrX[0][1] /= DDrXYZ; DrY[0][1] /= DDrXYZ;DrZ[0][1] /= DDrXYZ;
	}

	SrX[0] = DrY[0][0]*DrZ[0][1] - DrZ[0][0]*DrY[0][1];
	SrY[0] = DrZ[0][0]*DrX[0][1] - DrX[0][0]*DrZ[0][1];
	SrZ[0] = DrX[0][0]*DrY[0][1] - DrY[0][0]*DrX[0][1];
	DDrXYZ = sqrt( DD(SrX[0])+DD(SrY[0])+DD(SrZ[0]) );
	SrX[0]/=DDrXYZ;
	SrY[0]/=DDrXYZ;
	SrZ[0]/=DDrXYZ;

	DDrXYZ = sqrt( DD(DrX[1][0])+DD(DrY[1][0])+DD(DrZ[1][0]) );
	
	if(DDrXYZ < Error)
	{
		InPo[0]	= FirstLine[0][0];
		InPo[1]	= FirstLine[0][1];
		InPo[2]	= FirstLine[0][2];
		return TRUE;
	}
	else
	{
		DrX[1][0] /= DDrXYZ; DrY[1][0] /= DDrXYZ;DrZ[1][0] /= DDrXYZ;
	}

	DDrXYZ = sqrt( DD(DrX[1][1])+DD(DrY[1][1])+DD(DrZ[1][1]) );
	if(DDrXYZ < Error)
	{
		InPo[0]	= FirstLine[1][0];
		InPo[1]	= FirstLine[1][1];
		InPo[2]	= FirstLine[1][2];
		return TRUE;
	}
	else
	{	
		DrX[1][1] /= DDrXYZ; DrY[1][1] /= DDrXYZ;DrZ[1][1] /= DDrXYZ;
	}

	DDrXYZ = DistancePointToLine(&SecondLine[0][0], FirstLine, InPo, 0);

	if(DDrXYZ < Error)
	{
		InPo[0]	= SecondLine[0][0];
		InPo[1]	= SecondLine[0][1];
		InPo[2]	= SecondLine[0][2];
		return TRUE;
	}
	
	DDrXYZ = DistancePointToLine(&SecondLine[1][0], FirstLine, InPo, 0);
//str2.Format("2: %lf",DDrXYZ);
//AfxMessageBox(str2, MB_OK,0);
	if(DDrXYZ < Error)
	{
		InPo[0]	= SecondLine[1][0];
		InPo[1]	= SecondLine[1][1];
		InPo[2]	= SecondLine[1][2];
		return TRUE;
	}	
	
	DDrXYZ = DistancePointToLine(&FirstLine[0][0], SecondLine, InPo, 0);
//str2.Format("3: %lf",DDrXYZ);
//AfxMessageBox(str2, MB_OK,0);
	if(DDrXYZ < Error)
	{
		InPo[0]	= FirstLine[0][0];
		InPo[1]	= FirstLine[0][1];
		InPo[2]	= FirstLine[0][2];
		return TRUE;
	}
	
	DDrXYZ = DistancePointToLine(&FirstLine[1][0], SecondLine, InPo, 0);
//str2.Format("4: %lf",DDrXYZ);
//AfxMessageBox(str2, MB_OK,0);
	if(DDrXYZ < Error)
	{
		InPo[0]	= FirstLine[1][0];
		InPo[1]	= FirstLine[1][1];
		InPo[2]	= FirstLine[1][2];
		return TRUE;
	}		
	
	SrX[1] = DrY[1][0]*DrZ[1][1] - DrZ[1][0]*DrY[1][1];
	SrY[1] = DrZ[1][0]*DrX[1][1] - DrX[1][0]*DrZ[1][1];
	SrZ[1] = DrX[1][0]*DrY[1][1] - DrY[1][0]*DrX[1][1];
	DDrXYZ = sqrt( DD(SrX[1])+DD(SrY[1])+DD(SrZ[1]) );
	
	SrX[1]/=DDrXYZ;
	SrY[1]/=DDrXYZ;
	SrZ[1]/=DDrXYZ;

	if( (SrZ[1] > 0 && SrZ[0] < 0) || (SrX[1] > 0 && SrX[0] < 0) || (SrY[1] > 0 && SrY[0] < 0) )
	{
		SrX[0] *= -1.0;
		SrY[0] *= -1.0;
		SrZ[0] *= -1.0;
	}
	else if((SrZ[0] > 0 && SrZ[1] < 0) || (SrX[0] > 0 && SrX[1] < 0) || (SrY[0] > 0 && SrY[1] < 0))
	{
		SrX[1] *= -1.0;
		SrY[1] *= -1.0;
		SrZ[1] *= -1.0;
	}

	D[0]  = SrX[0]*FirstLine[0][0];
	D[0] += SrY[0]*FirstLine[0][1];
	D[0] += SrZ[0]*FirstLine[0][2];

	D[1]  = SrX[1]*SecondLine[0][0];
	D[1] += SrY[1]*SecondLine[0][1];
	D[1] += SrZ[1]*SecondLine[0][2];

	DrX[0][0]  = FirstLine[1][0]-FirstLine[0][0];
	DrY[0][0]  = FirstLine[1][1]-FirstLine[0][1]; 
	DrZ[0][0]  = FirstLine[1][2]-FirstLine[0][2]; 

	DDrXYZ = sqrt( DD(DrX[0][0])+DD(DrY[0][0])+DD(DrZ[0][0]) );

	DrX[0][0] /= DDrXYZ;
	DrY[0][0] /= DDrXYZ;
	DrZ[0][0] /= DDrXYZ;

	DrX[0][1]  = SecondLine[1][0]-SecondLine[0][0];
	DrY[0][1]  = SecondLine[1][1]-SecondLine[0][1]; 
	DrZ[0][1]  = SecondLine[1][2]-SecondLine[0][2]; 

	DDrXYZ = sqrt( DD(DrX[0][1])+DD(DrY[0][1])+DD(DrZ[0][1]) );

	DrX[0][1] /= DDrXYZ;
	DrY[0][1] /= DDrXYZ;
	DrZ[0][1] /= DDrXYZ;

	Theta = acos( DrX[0][0]*DrX[0][1]+ DrY[0][0]*DrY[0][1]+ DrZ[0][0]*DrZ[0][1]);
	
	if( fabs(Theta - 180) < 0.01 || fabs(Theta) < 0.01 )
	{
		InPo[0]	= -10000.0;
		InPo[1]	= -10000.0;
		InPo[2]	= -10000.0;
		return FALSE;
	}

	if( fabs(D[0]-D[1]) < Error )
	{
		*InPo = DrX[0][0]*(DrY[0][1]*SecondLine[0][0]-DrX[0][1]*SecondLine[0][1])-
				DrX[0][1]*(DrY[0][0]*FirstLine[0][0]-DrX[0][0]*FirstLine[0][1]);
		*InPo/= DrX[0][0]*DrY[0][1]-DrY[0][0]*DrX[0][1];
		*(InPo+1) = DrY[0][0]*(DrY[0][1]*SecondLine[0][0]-DrX[0][1]*SecondLine[0][1])-
					DrY[0][1]*(DrY[0][0]*FirstLine[0][0]-DrX[0][0]*FirstLine[0][1]);
		*(InPo+1)/= DrX[0][0]*DrY[0][1]-DrY[0][0]*DrX[0][1];
		if( fabs(DrX[0][0])>fabs(DrY[0][0]))
		{
			*(InPo+2) = FirstLine[0][2]+DrZ[0][0]*(*InPo-FirstLine[0][0])/DrX[0][0];
		}
		else
		{
			*(InPo+2) = FirstLine[0][2]+DrZ[0][0]*(*(InPo+1)-FirstLine[0][1])/DrY[0][0];
		}
		return TRUE;

	}
	else
	{
		*(InPo)		= 10000.0;
		*(InPo+1)	= 10000.0;
		*(InPo+2)	= 10000.0;

		return FALSE; 
	}

	return TRUE;

}

/*==========================================================================================//
																							
				Calculate the intersection point between a line and a circle						
				------------------------------------------------------------

  Line[2][3]	; Two points on the line	(Line[0][0], Line[0][1], Line[0][2])-
											(Line[1][0], Line[1][1], Line[1][2])

  cXYZ			; The center point of the circle
  DIR[3]		; The normal vector of the plane generated with peripheral points of the circle.
  Rad			; The radius of the circle

  *InPo			; Intersection points

				(x = *InPo,		y = *(InPo+1), z = *(InPo+2)),
				(x = *(InPo+3), y = *(InPo+4), z = *(InPo+5))

  *Gasu			; The number of the intersection points	

  If there is no intersection point, the return value is FALSE.
																							
==========================================================================================*/

BOOL CMakeObject::InterPointLineToCircle(double Line[2][3], double cXYZ[3],double DIR[3], double Rad, double InPo[2][3], short *Gasu) 
{
	double		N_1[3];
	double		N[3];
	double		DIS_1, DIS_1s;
	double		DIS;
	double		Ang;
	double		PlaneC;
	double		dis_LTC[2];
	double		s_Point[3];

	int i;

	DIS_1	= 0.0;
	DIS		= 0.0;

	for(i=0; i<3;i++)
	{
		N_1[i]	= cXYZ[i] - Line[0][i];
		DIS_1 +=DD(N_1[i]);
		N[i]	= Line[1][i] - Line[0][i];
		DIS += DD(N[i]);
 	}

	PlaneC  = DIR[0]*cXYZ[0]+DIR[1]*cXYZ[1]+DIR[2]*cXYZ[2];

	for(i=0;i<2;i++)
	{
		dis_LTC[i] = DIR[0]*Line[i][0]+DIR[1]*Line[i][1]+DIR[2]*Line[i][2]-PlaneC;
	}

	for(i=0;i<2;i++)
		dis_LTC[i] = fabs(dis_LTC[i]);

	DIS_1	= sqrt(DIS_1);
	DIS		= sqrt(DIS);

	if( dis_LTC[0] > Error || dis_LTC[1] > Error)
	{
		return FALSE;
	}

	for( i = 0; i<3 ; i++)	N[i] /= DIS;

	if(DIS_1 > Error)
	{ 
		for(i = 0; i<3 ; i++) 	N_1[i]	/= DIS_1;

		Ang = 0.0;
		for(i=0;i<3;i++) Ang += N_1[i]*N[i];
	
		Ang /= sqrt(DD(N_1[0])+DD(N_1[1])+DD(N_1[2]))*sqrt(DD(N[0])+DD(N[1])+DD(N[2]));

		if( fabs(Ang-1.0000)<Error || fabs(Ang+1.0000) < Error )
		{
			for(i=0;i<3;i++)
				s_Point[i] = cXYZ[i];
			
			for(i=0;i<3;i++)
			{
			//	*(InPo+i) = s_Point[i] + N[i]*Rad;
				InPo[0][i] = s_Point[i] + N[i]*Rad;
			}
	
			for(i=0;i<3;i++)
			{
			//	*(InPo+3+i) = s_Point[i] - N[i]*Rad;
				InPo[1][i] = s_Point[i] + N[i]*Rad;
			}

			*Gasu =	2;
		//	Gasu =	2;

		}
		else
		{
			Ang = acos(Ang)*180/3.141592654;

			if(Ang > 90.000)
			{
				N[0] *= -1; N[1] *= -1; N[2] *= -1;
				Ang = 180 - Ang;
			}

			DIS		= DIS_1*sin(Ang*3.141592654/180.0);	// The shortest distance
			DIS_1s	= DIS_1*cos(Ang*3.141592654/180.0); 

	// Check whether the line is on the plane consisted of the circle ...
	
			if( DIS > Rad)
			{
				return FALSE;
			}

			for(i=0;i<3;i++)
			{
			//	*(InPo+i)	= s_Point[i] = Line[0][i] + N[i]*DIS_1s;
				*Gasu		=	1;
				InPo[0][i]	= s_Point[i] = Line[0][i] + N[i]*DIS_1s;
			//	Gasu		=	1;
			}

			if( fabs(DIS-Rad)< Error)
			{
				return TRUE;

			}
			else
			{
				for(i=0;i<3;i++)
				{
				//	*(InPo+i) =s_Point[i] + N[i]*sqrt(DD(Rad)-DD(DIS));
					InPo[0][i] =s_Point[i] + N[i]*sqrt(DD(Rad)-DD(DIS));
				}
		
				for(i=0;i<3;i++)
				{
				//	*(InPo+i+3)=s_Point[i] - N[i]*sqrt(DD(Rad)-DD(DIS));
					InPo[1][i] = s_Point[i] - N[i]*sqrt(DD(Rad)-DD(DIS));
				}

				*Gasu =	2;
			//	Gasu =	2;

			}
		}

	}
	else
	{
		s_Point[i] = cXYZ[i];

		for(i=0;i<3;i++)
		{
		//	*(InPo+i)=s_Point[i] + N[i]*Rad;
			InPo[0][i]=s_Point[i] + N[i]*Rad;
		}
	
		for(i=0;i<3;i++)
		{
		//	*(InPo+i+3)=s_Point[i] - N[i]*Rad;
			InPo[1][i]=s_Point[i] + N[i]*Rad;
		}

		*Gasu =	2;
	//	Gasu =	2;
	}

	return TRUE;

}

/*==========================================================================================//
																							
				Calculate the intersection point between a circle and a circle						
				------------------------------------------------------------

  *InPo			; Intersection points

				(x = *InPo,		y = *(InPo+1), z = *(InPo+2)),
				(x = *(InPo+3), y = *(InPo+4), z = *(InPo+5))

  *Gasu			; The number of the intersection points	

  If there is no intersection point, the return value is FALSE.
																							
=========================================================================================*/

BOOL CMakeObject::InterPointCircleToCircle(double cXYZ1[3],double DIR1[3],double Rad1, double cXYZ2[3],double DIR2[3], double Rad2, double InPo[2][3], short *Gasu) 
{
	double		N_1[3];
	double		N[3];
	double		DIS_1, DIS_2;
	double		DIS;
	double		Ang;
//	double		PlaneC;
//	double		dis_LTC[2];
	double		s_Point[3];
	double		Sum1, Sum2;
	int			i;
	double		TempRad[2];

	Sum1 = sqrt(DD(DIR1[0])+DD(DIR1[1])+DD(DIR1[2]));
	Sum2 = sqrt(DD(DIR2[0])+DD(DIR2[1])+DD(DIR2[2]));

	for(i=0; i<3; i++)
	{ // Normalization of the normal vectors of the planes including the circles
		DIR1[i] /= Sum1;
		DIR2[i] /= Sum2;
	}

	Ang = (double)0.0;
	for( i = 0 ; i< 3; i++)
	{ // Calculation the angle between two planes to check	if two circles are parallel.
		Ang += DIR1[i]*DIR1[i];
	}
	
	Ang = acos(Ang);


	if( !(fabs( Ang-PI) < Error || fabs(Ang) < Error) )
	{ 
		return FALSE; // Because two circles are not parallel ... : 1-st condition : two circles have intersection points
	}

	// Make a plane with a circle 2 ...

	DIS_2 = DIR2[0]*cXYZ2[0]+DIR2[1]*cXYZ2[1]+DIR2[2]*cXYZ2[2];

	// Calculation the shortest distance between the center point of the first circle between
	// the plane which have been made above 
	
	DIS  = fabs(DIR2[0]*cXYZ1[0]+DIR2[1]*cXYZ1[1]+DIR2[2]*cXYZ1[2] - DIS_2);

	if( DIS > Error )
	{
		return FALSE;	// There is no intersection point because the center point of the first circle
						// is not located on the plane generated with the second circle ...
	}

	DIS = sqrt( DD(cXYZ1[0]-cXYZ2[0]) + DD(cXYZ1[1]-cXYZ2[1]) + DD(cXYZ1[2]-cXYZ2[2]) );

	TempRad[0] = Min( Rad1, Rad2);
	TempRad[1] = Max( Rad1, Rad2);

	// The condition which two circles have intersection points is 
	// TempRad[1]-TampRad[0] < DIS < TempRad[1]+TampRad[0].

	if( !((TempRad[1]-TempRad[0] < DIS ) && (DIS < TempRad[1]+TempRad[0])) )
	{
		return FALSE;
	}

	//  The following process is to find intersection points where two circle meet...

	Ang = acos( (DD(Rad1)+DD(DIS)-DD(Rad2))/(2*Rad1*DIS) );
	DIS = Rad1 * cos(Ang);

	//	The vector from the center of the first circle to the center of the 2nd ...

	Sum1 = 0;

	for( i=0; i< 3; i++)
	{
		N_1[i]  = cXYZ2[i] - cXYZ1[i];
		Sum1 += DD(N_1[i]);
	}

	Sum1 = sqrt(Sum1);

	for( i=0; i< 3; i++)
	{ // Normalizing the vector ...
		N_1[i] /= Sum1;
	}

	// Cross product between the above vector and the normal vector of the plane ...

	N[0] = N_1[1]*DIR2[2] - N_1[2]*DIR2[1];
	N[1] = N_1[2]*DIR2[0] - N_1[0]*DIR2[2];
	N[2] = N_1[0]*DIR2[1] - N_1[1]*DIR2[0];

	/* --------------------------------------------------------------------------------------
		Calculation of P point .... P point is the point which is the intersection point between 
		the center line and the intersection line. The intersection line consists of two 
		two intersection points of two circles when two circles meet at the two points.
  
	--------------------------------------------------------------------------------------*/ 

	for( i = 0; i < 3; i++)
	{
		s_Point[i] = DIS * N_1[i] + cXYZ1[i];	
	}

	/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

							Calculation of the intersection points ...

	++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

	DIS = Rad1 * sin(Ang);

	for( i = 0; i < 3; i++)
	{
//		InPo[i] = DIS * N[i] + s_Point[i];	
		InPo[0][i] = DIS * N[i] + s_Point[i];	
	}

	for( i = 3; i < 6; i++)
	{
//		InPo[i] = -DIS * N[i-3] + s_Point[i-3];	
		InPo[1][i-3] = -DIS * N[i-3] + s_Point[i-3];	
	}

	/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


			Check if two circles are meet at one point or two points ...

	++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

	DIS_1 = sqrt( (double)DD(InPo[0]-InPo[3])+DD(InPo[1]-InPo[4])+DD(InPo[2]-InPo[5]));

	if( DIS_1 < Error )
	{
		*Gasu = 1;
	}
	else
	{
		*Gasu = 2;
	}
	//=== The end of the process of checking if the two circles are on the same plane ========/

	return TRUE;

}


/*==========================================================================================//
																							
				Calculate the intersection line between two planes 						
				------------------------------------------------------------

  *InPo			; Two points on the intersection line

				(x = *InPo,		y = *(InPo+1), z = *(InPo+2)),
				(x = *(InPo+3), y = *(InPo+4), z = *(InPo+5))

  If there is no intersection line, the return value is FALSE.
																							
==========================================================================================*/

BOOL CMakeObject::InterLinePlanePlane(double XYZ1[3],double DIR1[3],double D1, double XYZ2[3], double DIR2[3], double D2, double *InPo)
{
	double	MaxXYZ[3];
	double	MinXYZ[3];
	int		i;

	for(i=0; i<3 ; i++)
	{
		MaxXYZ[i] = 0;
		MinXYZ[i] = 0;
	}

	if(LinePlanePlane(XYZ1,DIR1,D1,XYZ2,DIR2, D2, InPo, MinXYZ, MaxXYZ))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
	
}

BOOL CMakeObject::InterLinePlanePlane(double XYZ1[3],double DIR1[3],double D1, double XYZ2[3], double DIR2[3], double D2, double *InPo, double MinXYZ[3], double MaxXYZ[3])
{

	if(LinePlanePlane(XYZ1,DIR1,D1,XYZ2,DIR2, D2, InPo, MinXYZ, MaxXYZ))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}	

}


BOOL CMakeObject::LinePlanePlane(double XYZ1[3],double DIR1[3],double D1, double XYZ2[3], double DIR2[3], double D2, double *InPo, double MinXYZ[3], double MaxXYZ[3]) 
{
	double		AA[3][3];
	double		Sum1, Sum2;
	int			i;
//	double		TempRad[2];
	double		Ang;
	BOOL		MaxMinMode;

	if( fabs(MaxXYZ[0]-MinXYZ[0]) < Error && fabs(MaxXYZ[1]-MinXYZ[1]) < Error && fabs(MaxXYZ[2]-MinXYZ[2]) < Error)
	{
		MaxMinMode = FALSE;
	}
	else
	{
		MaxMinMode = TRUE;
	}

	Sum1 = sqrt(DD(DIR1[0])+DD(DIR1[1])+DD(DIR1[2]));
	Sum2 = sqrt(DD(DIR2[0])+DD(DIR2[1])+DD(DIR2[2]));

	for(i=0; i<3; i++)
	{ // Normalization of the normal vectors of the planes including the circles
		DIR1[i] /= Sum1;
		DIR2[i] /= Sum2;
	}

	D1 = 0.0;
	D2 = 0.0;

	for(i=0;i<3;i++)
	{
		D1 += DIR1[i]*XYZ1[i];
		D2 += DIR2[i]*XYZ2[i];
	}
	
	Ang = (double)0.0;
	for( i = 0 ; i< 3; i++)
	{
		Ang += DIR1[i]*DIR2[i];
	}
	
	Ang = acos(Ang);

	if( fabs( Ang-PI) < Error || fabs(Ang) < Error )
	{ 
		return FALSE; 
	}

	/*==========================================

		AA[0][0] y + A[0][1] z = A[0][2]
		AA[1][0] x + A[1][1] y = A[1][2]
		AA[2][0] x + A[2][1] z = A[2][2]

	===========================================*/

	AA[0][0] = DIR2[0]*DIR1[1] - DIR1[0]*DIR2[1];
	AA[0][1] = DIR2[0]*DIR1[2] - DIR1[0]*DIR2[2];
	AA[0][2] = DIR2[0]*D1      - DIR1[0]*D2;

	AA[1][0] = DIR2[2]*DIR1[0] - DIR1[2]*DIR2[0];
	AA[1][1] = DIR2[2]*DIR1[1] - DIR1[2]*DIR2[1];
	AA[1][2] = DIR2[2]*D1      - DIR1[2]*D2;
	
	AA[2][0] = DIR2[1]*DIR1[0] - DIR1[1]*DIR2[0];
	AA[2][1] = DIR2[1]*DIR1[2] - DIR1[1]*DIR2[2];
	AA[2][2] = DIR2[1]*D1      - DIR1[1]*D2;
	
	// ============= Find the Intersection Line ============= //

//	CString str;

	if( fabs(AA[0][0]) < Error && fabs(AA[1][1]) < Error )
	{
		InPo[2] = AA[0][2]/AA[0][1];
		InPo[0] = AA[1][2]/AA[1][0];
		InPo[3] = InPo[0];
		InPo[5] = InPo[2];

		if( !MaxMinMode )
		{
			InPo[1] = XYZ1[1];
			InPo[4] = XYZ2[1];
		}
		else
		{
			InPo[1] = MinXYZ[1];
			InPo[4] = MaxXYZ[1];
		}

//str.Format("(%lf, %lf, %lf)-(%lf, %lf, %lf)", InPo[0], InPo[1], InPo[2], InPo[3], InPo[4], InPo[5]);
//AfxMessageBox(str); 
		return TRUE;

	}
	else if( fabs(AA[1][0]) < Error && fabs(AA[2][0]) < Error )
	{
		InPo[1] = InPo[4] = AA[1][2]/AA[1][1];
		InPo[2] = InPo[5] = AA[2][2]/AA[2][1];

		if( !MaxMinMode )
		{
			InPo[0] = XYZ1[0];
			InPo[3] = XYZ2[0];
		}
		else
		{
			InPo[0] = MinXYZ[0];
			InPo[3] = MaxXYZ[0];
		}

		if( fabs(InPo[0]-InPo[3]) < Error ) InPo[3] += 10;

//str.Format("(%lf, %lf, %lf)-(%lf, %lf, %lf)", InPo[0], InPo[1], InPo[2], InPo[3], InPo[4], InPo[5]);
//AfxMessageBox(str); 
		return TRUE;

	}
	else if( fabs(AA[0][1]) < Error && fabs(AA[2][1]) < Error )
	{
		InPo[4] = AA[0][2]/AA[0][0];
		InPo[3] = AA[2][2]/AA[2][0];
		InPo[1] = InPo[4];
		InPo[0] = InPo[3];
		
		if( !MaxMinMode )
		{
			InPo[2] = XYZ1[2];
			InPo[5] = XYZ2[2];
		}
		else
		{
			InPo[2] = MinXYZ[2];
			InPo[5] = MaxXYZ[2];
		}

		if( fabs(InPo[2]-InPo[5]) < Error ) InPo[5] += 10;

//str.Format("(%lf, %lf, %lf)-(%lf, %lf, %lf)", InPo[0], InPo[1], InPo[2], InPo[3], InPo[4], InPo[5]);
//AfxMessageBox(str); 
		return TRUE;

	}

	/*==========================================

		AA[0][0] y + A[0][1] z = A[0][2]
		AA[1][0] x + A[1][1] y = A[1][2]
		AA[2][0] x + A[2][1] z = A[2][2]

	===========================================*/

	if( fabs(AA[0][0]) < Error && fabs(AA[1][1]) > Error && fabs(AA[0][1]) > Error )
	{
		InPo[2] = AA[0][2]/AA[0][1];

		if( fabs(AA[1][0]) > fabs(AA[1][1]) )
		{
			if( !MaxMinMode )
			{
				InPo[1] = XYZ1[1];
			}
			else
			{
				InPo[1] = MinXYZ[1];
			}
			InPo[0] = (AA[1][2]-AA[1][1]*InPo[1])/AA[1][0];

			if( !MaxMinMode )
			{
				InPo[4] = XYZ1[1]+10;
			}
			else
			{
				InPo[4] = MaxXYZ[1];
			}
			InPo[3] = (AA[1][2]-AA[1][1]*InPo[4])/AA[1][0];

		}
		else
		{
			if( !MaxMinMode )
			{
				InPo[0] = XYZ1[0];
			}
			else
			{
				InPo[0] = MinXYZ[0];
			}
			InPo[1] = (AA[1][2]-AA[1][0]*InPo[0])/AA[1][1];

			if( !MaxMinMode )
			{
				InPo[3] = XYZ1[0]+10;
			}
			else
			{
				InPo[3] = MaxXYZ[0];
			}
			InPo[4] = (AA[1][2]-AA[1][0]*InPo[3])/AA[1][1];
		}

		InPo[5] = InPo[2];
		 
//str.Format("(%lf, %lf, %lf)-(%lf, %lf, %lf)", InPo[0], InPo[1], InPo[2], InPo[3], InPo[4], InPo[5]);
//AfxMessageBox(str); 
		return TRUE;

	}
	else if( fabs(AA[1][0]) < Error && fabs(AA[1][1]) > Error && fabs(AA[2][0]) > Error )
	{
	/*==========================================

		AA[0][0] y + A[0][1] z = A[0][2]
		AA[1][0] x + A[1][1] y = A[1][2]
		AA[2][0] x + A[2][1] z = A[2][2]

	===========================================*/

		InPo[1] = AA[1][2]/AA[1][1];

		if( fabs(AA[2][0]) > fabs(AA[2][1]) )
		{
			if( !MaxMinMode )
			{
				InPo[2] = XYZ1[2];
			}
			else
			{
				InPo[2] = MinXYZ[2];
			}
			InPo[0] = (AA[2][2]-AA[2][1]*InPo[2])/AA[2][0];

			if( !MaxMinMode )
			{
				InPo[5] = XYZ1[2]+10;
			}
			else
			{
				InPo[5] = MaxXYZ[2];
			}
			InPo[3] = (AA[2][2]-AA[2][1]*InPo[5])/AA[2][0];

		}
		else
		{
			if( !MaxMinMode )
			{
				InPo[0] = XYZ1[0];
			}
			else
			{
				InPo[0] = MinXYZ[0];
			}
			InPo[2] = (AA[2][2]-AA[2][0]*InPo[0])/AA[2][1];

			if( !MaxMinMode )
			{
				InPo[3] = XYZ1[0]+10;
			}
			else
			{
				InPo[3] = MaxXYZ[0];
			}
			InPo[5] = (AA[2][2]-AA[2][0]*InPo[3])/AA[2][1];
		}

		InPo[4] = InPo[1];
		 
//str.Format("(%lf, %lf, %lf)-(%lf, %lf, %lf)", InPo[0], InPo[1], InPo[2], InPo[3], InPo[4], InPo[5]);
//AfxMessageBox(str); 
		return TRUE;

	}
	else if( fabs(AA[2][1]) < Error && fabs(AA[0][1]) > Error && fabs(AA[2][0]) > Error )
	{
	/*==========================================

		AA[0][0] y + A[0][1] z = A[0][2]
		AA[1][0] x + A[1][1] y = A[1][2]
		AA[2][0] x + A[2][1] z = A[2][2]

	===========================================*/

		InPo[0] = AA[2][2]/AA[2][0];

		if( fabs(AA[0][0]) > fabs(AA[0][1]) )
		{
			if( !MaxMinMode )
			{
				InPo[2] = XYZ1[2];
			}
			else
			{
				InPo[2] = MinXYZ[2];
			}
			InPo[1] = (AA[0][2]-AA[0][1]*InPo[2])/AA[0][0];

			if( !MaxMinMode )
			{
				InPo[5] = XYZ1[2]+10;
			}
			else
			{
				InPo[5] = MaxXYZ[2];
			}
			InPo[4] = (AA[0][2]-AA[0][1]*InPo[5])/AA[0][0];

		}
		else
		{
			if( !MaxMinMode ){
				InPo[1] = XYZ1[1];
			}else{
				InPo[1] = MinXYZ[1];
			}
			InPo[2] = (AA[0][2]-AA[0][0]*InPo[1])/AA[0][1];

			if( !MaxMinMode ){
				InPo[4] = XYZ1[1]+10;
			}else{
				InPo[4] = MaxXYZ[1];
			}
			InPo[5] = (AA[0][2]-AA[0][0]*InPo[4])/AA[0][1];
		}

		InPo[3] = InPo[0];
		 
//str.Format("(%lf, %lf, %lf)-(%lf, %lf, %lf)", InPo[0], InPo[1], InPo[2], InPo[3], InPo[4], InPo[5]);
//AfxMessageBox(str); 
		return TRUE;

	}


	/*==========================================

		AA[0][0] y + A[0][1] z = A[0][2]
		AA[1][0] x + A[1][1] y = A[1][2]
		AA[2][0] x + A[2][1] z = A[2][2]

	===========================================*/
	
	if( (fabs(DIR1[2]) > fabs(DIR1[0]) ) && (fabs(DIR1[2]) > fabs(DIR1[1]))){
		
		if( fabs(DIR1[1]) > fabs(DIR1[0]) ){

			if( !MaxMinMode ){
				InPo[0] = XYZ1[0];
			}else{
				InPo[0] = MinXYZ[0];
			}
			InPo[1] = (AA[1][2]-AA[1][0]*InPo[0])/AA[1][1];
			
		}else{

			if( !MaxMinMode ){
				InPo[1] = XYZ1[1];
			}else{
				InPo[1] = MinXYZ[1];
			}
			InPo[0] = (AA[1][2]-AA[1][1]*InPo[1])/AA[1][0];

		}

		if( fabs(AA[0][1]) > Error ){
			InPo[2] = (AA[0][2]-AA[0][0]*InPo[1])/AA[0][1];
		}else{
			InPo[2] = (AA[2][2]-AA[2][0]*InPo[0])/AA[2][1];
		}
	
		if( fabs(DIR1[1]) > fabs(DIR1[0]) ){

			if( !MaxMinMode ){
				InPo[3] = XYZ1[0]+10;
			}else{
				InPo[3] = MaxXYZ[0];
			}
			InPo[4] = (AA[1][2]-AA[1][0]*InPo[3])/AA[1][1];
			
		}else{

			if( !MaxMinMode ){
				InPo[4] = XYZ1[1]+10;
			}else{
				InPo[4] = MaxXYZ[1];
			}
			InPo[3] = (AA[1][2]-AA[1][1]*InPo[4])/AA[1][0];

		}

		if( fabs(AA[0][1]) > Error ){
			InPo[5] = (AA[0][2]-AA[0][0]*InPo[4])/AA[0][1];
		}else{
			InPo[5] = (AA[2][2]-AA[2][0]*InPo[3])/AA[2][1];
		}
		
	/*==========================================

		AA[0][0] y + A[0][1] z = A[0][2]
		AA[1][0] x + A[1][1] y = A[1][2]
		AA[2][0] x + A[2][1] z = A[2][2]

	===========================================*/
	}else if((fabs(DIR1[1]) > fabs(DIR1[2]) ) && (fabs(DIR1[1]) > fabs(DIR1[0]))){
		
		if( fabs(DIR1[2]) > fabs(DIR1[0]) ){

			if( !MaxMinMode ){
				InPo[0] = XYZ1[0];
			}else{
				InPo[0] = MinXYZ[0];
			}
			InPo[2] = (AA[2][2]-AA[2][0]*InPo[0])/AA[2][1];
			
		}else{

			if( !MaxMinMode ){
				InPo[2] = XYZ1[2];
			}else{
				InPo[2] = MinXYZ[2];
			}
			InPo[0] = (AA[2][2]-AA[2][1]*InPo[2])/AA[2][0];

		}

		if( fabs(AA[0][0]) > Error ){
			InPo[1] = (AA[0][2]-AA[0][1]*InPo[2])/AA[0][0];
		}else{
			InPo[1] = (AA[1][2]-AA[1][0]*InPo[0])/AA[1][1];
		}

		if( fabs(DIR1[2]) > fabs(DIR1[0]) ){

			if( !MaxMinMode ){
				InPo[3] = XYZ1[0]+10;
			}else{
				InPo[3] = MaxXYZ[0];
			}
			InPo[5] = (AA[2][2]-AA[2][0]*InPo[3])/AA[2][1];
			
		}else{

			if( !MaxMinMode ){
				InPo[5] = XYZ1[2]+10;
			}else{
				InPo[5] = MaxXYZ[2];
			}
			InPo[3] = (AA[2][2]-AA[2][1]*InPo[5])/AA[2][0];

		}

		if( fabs(AA[0][0]) > Error ){
			InPo[4] = (AA[0][2]-AA[0][1]*InPo[5])/AA[0][0];
		}else{
			InPo[4] = (AA[1][2]-AA[1][0]*InPo[3])/AA[1][1];
		}
	/*==========================================

		AA[0][0] y + A[0][1] z = A[0][2]
		AA[1][0] x + A[1][1] y = A[1][2]
		AA[2][0] x + A[2][1] z = A[2][2]

	===========================================*/
	}else if((fabs(DIR1[0]) > fabs(DIR1[2]) ) && (fabs(DIR1[0]) > fabs(DIR1[1]))){
		
		if( fabs(DIR1[2]) > fabs(DIR1[1]) ){

			if( !MaxMinMode ){
				InPo[1] = XYZ1[1];
			}else{
				InPo[1] = MinXYZ[1];
			}
			InPo[2] = (AA[0][2]-AA[0][0]*InPo[1])/AA[0][1];
			
		}else{

			if( !MaxMinMode ){
				InPo[2] = XYZ1[2];
			}else{
				InPo[2] = MinXYZ[2];
			}
			InPo[1] = (AA[0][2]-AA[0][1]*InPo[2])/AA[0][1];

		}

		if( fabs(AA[1][0]) > Error ){
			InPo[0] = (AA[1][2]-AA[1][1]*InPo[1])/AA[1][0];
		}else{
			InPo[0] = (AA[2][2]-AA[2][1]*InPo[2])/AA[2][0];
		}

		if( fabs(DIR1[2]) > fabs(DIR1[1]) ){

			if( !MaxMinMode ){
				InPo[4] = XYZ1[1]+10;
			}else{
				InPo[4] = MaxXYZ[1];
			}
			InPo[5] = (AA[0][2]-AA[0][0]*InPo[4])/AA[0][1];
			
		}else{

			if( !MaxMinMode ){
				InPo[5] = XYZ1[2]+10;
			}else{
				InPo[5] = MaxXYZ[2];
			}
			InPo[4] = (AA[0][2]-AA[0][1]*InPo[5])/AA[0][1];

		}

		if( fabs(AA[1][0]) > Error ){
			InPo[3] = (AA[1][2]-AA[1][1]*InPo[4])/AA[1][0];
		}else{
			InPo[3] = (AA[2][2]-AA[2][1]*InPo[5])/AA[2][0];
		}
	}


//str.Format("(%lf, %lf, %lf)-(%lf, %lf, %lf)", InPo[0], InPo[1], InPo[2], InPo[3], InPo[4], InPo[5]);
//AfxMessageBox(str); 

	return TRUE;

}


/*==========================================================================================//
																							
				Calculate the intersection point between a line and a plane						
				------------------------------------------------------------

  Line[2][3]	; Two points on the line	(Line[0][0], Line[0][1], Line[0][2])-
											(Line[1][0], Line[1][1], Line[1][2])

  DIR[3]		; The normal vector of the plane.
  Dd			; A parameter for the plane equation.

  *InPo			; Intersection points

				(x = *InPo,		y = *(InPo+1), z = *(InPo+2)),

  If there is no intersection point, the return value is FALSE.
																							
==========================================================================================*/

BOOL CMakeObject::InterPointLineToPlane(double Line[2][3], double DIR[3], double Dd, double *InPo) 
{
	
	double		N_1[3];
//	double		N[3];
	double		sum;
	int			i;
	double		Ang;


	sum = (double)0;

	for(i=0;i<3;i++){
		N_1[i] = Line[1][i]-Line[0][i];
		sum += DD(N_1[i]);
	}

	for(i=0;i<3;i++)	N_1[i] /= sqrt(sum);

	Ang = 0.0;
	for(i=0;i<3;i++) Ang += N_1[i]*DIR[i];
	
	Ang /= sqrt(DD(N_1[0])+DD(N_1[1])+DD(N_1[2]))*sqrt(DD(DIR[0])+DD(DIR[1])+DD(DIR[2]));

	Ang = acos(Ang);

	if( fabs(Ang-PI/2) < 0.0001 || fabs(Ang-3*PI/2) < 0.0001 ) return FALSE;

	if( Ang > PI/2 ){

		Ang = PI - Ang;

	}

	sum = DIR[0]*Line[1][0]+DIR[1]*Line[1][1]+DIR[2]*Line[1][2]-Dd;
	sum = sum/cos(Ang);

	if( fabs(sum) < Error ){

		for(i=0;i<3;i++){

			InPo[i] = Line[1][i];

		}

	}else{

		for(i=0;i<3;i++){

			InPo[i] = Line[1][i] - sum * N_1[i];

		}
	}

	return TRUE;

}

/*==========================================================================================//
																							
				Calculate the distance between a point and a plane						
				------------------------------------------------------------


  DIR[3]		; The normal vector of the plane.
  Dd			; A parameter for the plane equation.

  *InPo			; point on the plane to make the shortest distance from the point.

				(x = *InPo,		y = *(InPo+1), z = *(InPo+2)),

																							
==========================================================================================*/

double CMakeObject::DistancePointToPlane(double Point[3], double DIR[3], double Dd, double *InPo) 
{

	
//	double		N_1[3];
//	double		N[3];
	double		sum, Dis;
	int			i;
//	double		Ang;


	Dis = fabs(DIR[0]*Point[0]+DIR[1]*Point[1]+DIR[2]*Point[2]-Dd);

	InPo[0] = Point[0];
	InPo[1] = Point[1];
	InPo[2] = Point[2];

	if( fabs(Dis) < Error ){

		for(i=0;i<3;i++){

			InPo[3+i] = Point[i];

		}
	
	}else{

		for(i=0;i<3;i++){

			InPo[3+i] = Point[i] - Dis * DIR[i];

		}


		sum = fabs(DIR[0]*InPo[3]+DIR[1]*InPo[4]+DIR[2]*InPo[5]-Dd);

		if( fabs(sum) < Error ){

			return Dis;

		}

		for(i=0;i<3;i++){

			InPo[3+i] = Point[i] + Dis * DIR[i];

		}

 
	}

	return Dis;

}

/*==========================================================================================//
																							
				Calculate the distance between a point and a line						
				------------------------------------------------------------

  *InPo			; point on the plane to make the shortest distance from the point.

				(x = *InPo,		y = *(InPo+1), z = *(InPo+2)),

																							
==========================================================================================*/

double CMakeObject::DistancePointToLine(double Point[3], double LinePoint[2][3], double InPo[3], short mode) 
{
	
	double		N_1[3];
	double		N[3];
	double		sum, Dis, Dis2;
	int			i;
	double		Ang;

	// mode is to select projection plane of the measured point ....

	if( mode == 1){

		Point[2] = (double)0.0;

	}else if( mode == 2){
		
		Point[0] = (double)0.0;

	}else if( mode == 3){

		Point[1] = (double)0.0;

	}

	
	sum = (double)0;
	for(i=0;i<3;i++){

		N_1[i]	 = LinePoint[1][i] - LinePoint[0][i];
		sum		+= DD(N_1[i]);

	}

	sum = sqrt( sum );

	for(i=0;i<3;i++){


		N_1[i] /= sum;	


	}

	sum = (double)0;
	for(i=0;i<3;i++){

		N[i]	 = LinePoint[1][i] - Point[i];
		sum		+= DD(N[i]);

	}

	sum = sqrt( sum );

	for(i=0;i<3;i++){


		N[i] /= sum;	


	}

	Ang = acos( N[0]*N_1[0] + N[1]*N_1[1]+ N[2]*N_1[2]);

	if( fabs(Ang-PI) < Error || fabs(Ang) < Error ){


		InPo[0] = Point[0];
		InPo[1] = Point[1];
		InPo[2] = Point[2];

		return (double)0.0;

	}
	
	
	if( Ang > PI/2 ){


		Ang = PI - Ang;


	}

	Dis		= sum * sin(Ang);
	Dis2	= sum * cos(Ang);


	for(i=0;i<3;i++){

		InPo[i] = LinePoint[1][i] - N_1[i] * Dis2;

	}


	sum = (double)0;

	for( i=0; i<3; i++){

		sum += DD( InPo[i] - Point[i] );

	}
	sum = sqrt(sum);


	if( fabs( Dis - sum ) < 0.00001 ){

	}else{

		for(i=0;i<3;i++){

			InPo[i] = LinePoint[1][i] + N_1[i] * Dis2;

		}

	}


	return Dis;

}

/************************************
 Method:    DistancePointToCircle2d
 FullName:  CMakeObject::DistancePointToCircle2d
 Access:    public 
 Returns:   double
 Qualifier:
 Parameter: double Point1[2]
 Parameter: double cXY[2]
 Parameter: double Rad

 Calculate the distance Point to Circle
 --------------------------------------

 dis = 

************************************/
double CMakeObject::DistancePointToCircle2d(double Pnt[3], double cXY[3], double Rad)
{
	return sqrt(DD(Pnt[0] - cXY[0]) + DD(Pnt[1] - cXY[1])) - Rad;
}

/*==========================================================================================//
																							
				Calculate the distance between two lines						
				----------------------------------------

  *InPo			; point on the plane to make the shortest distance from the point.

				(x = *InPo,		y = *(InPo+1), z = *(InPo+2)),

																							
==========================================================================================*/

double CMakeObject::DistanceLineToLine(double LinePoint1[2][3], double LinePoint[2][3], double InPo[6]) 
{
	double		N_1[3];
	double		N[3];
	double		sum, Dis, Dis2;
	int			i;
	double		Ang;
	double		Point[3];

	if( !InterPointTwoLines(LinePoint1, LinePoint, InPo) )
	{
		InPo[3] = InPo[0];
		InPo[4] = InPo[1];
		InPo[5] = InPo[2];
		return (double) 0.0;
	}


	if( fabs(-10000.0-InPo[0]) < Error && fabs(-10000.0-InPo[1]) < Error && fabs(-10000.0-InPo[1]) < Error ) 
	{  
		for(i=0;i<3;i++)
		{
			Point[i] = (LinePoint1[0][i]+LinePoint1[1][i])/2;
			InPo[3+i] = Point[i];
		}
	
		sum = (double)0;
		for(i=0;i<3;i++)
		{
			N_1[i]	 = LinePoint[1][i] - LinePoint[0][i];
			sum		+= DD(N_1[i]);
		}

		sum = sqrt( sum );

		for(i=0;i<3;i++)
		{
			N_1[i] /= sum;	
		}

		sum = (double)0;
		for(i=0;i<3;i++)
		{
			N[i]	 = LinePoint[1][i] - Point[i];
			sum		+= DD(N[i]);
		}

		sum = sqrt( sum );

		for(i=0;i<3;i++)
		{
			N[i] /= sum;	
		}

		Ang = acos( N[0]*N_1[0] + N[1]*N_1[1]+ N[2]*N_1[2]);

		if( Ang > PI/2 )
		{
			Ang = PI - Ang;
		}

		Dis		= sum * sin(Ang);
		Dis2	= sum * cos(Ang);

		for(i=0;i<3;i++)
		{
			InPo[i] = LinePoint[1][i] - N_1[i] * Dis2;
		}

		sum = (double)0;

		for( i=0; i<3; i++)
		{
			sum += DD( InPo[i] - Point[i] );
		}

		sum = sqrt(sum);

		if( fabs( Dis - sum ) < 0.00001 )
		{
		}
		else
		{
			for(i=0;i<3;i++)
			{
				InPo[i] = LinePoint[1][i] + N_1[i] * Dis2;
			}
		}
	}
	else
	{
		sum = (double)0;
		for(i=0;i<3;i++)
		{
			N_1[i]	 = LinePoint[1][i] - LinePoint[0][i];
			sum		+= DD(N_1[i]);
		}

		sum = sqrt( sum );

		for(i=0;i<3;i++)
		{
			N_1[i] /= sum;	
		}

		Dis2 = N_1[0]*LinePoint1[0][0] + N_1[1]*LinePoint1[0][1] + N_1[2]*LinePoint1[0][2];
		InterPointLineToPlane(LinePoint, N_1, Dis2, Point);
		Dis = DistancePointToLine(Point, LinePoint1, InPo, 0) ;

		InPo[3] = Point[0];
		InPo[4] = Point[1];
		InPo[5] = Point[2];
	}
	if(Dis < Error)
	{
		InPo[3] = InPo[0];
		InPo[4] = InPo[1];
		InPo[5] = InPo[2];
	}

	return Dis;

}

/*==========================================================================================//
																							
				Calculate the distance between two points						
				------------------------------------------------------------

  *InPo			; point on the plane to make the shortest distance from the point.

				(x = *InPo,		y = *(InPo+1), z = *(InPo+2)),

																							
==========================================================================================*/

double CMakeObject::DistancePointToPoint(double Point1[3], double Point2[3]) 
{
	double Dis;

	Dis = sqrt( DD(Point2[0] - Point1[0])+DD(Point2[1] - Point1[1])+DD(Point2[2] - Point1[2]));
	
	return Dis;

}
double CMakeObject::DistancePointToPoint_float(double Point1[2], double Point2[2]) 
{
	double Dis;

	Dis = sqrt( DD(Point2[0] - Point1[0])+DD(Point2[1] - Point1[1]));

	return Dis;

}
/*==========================================================================================//
																							
				Calculate the distance between a point and a point						
				------------------------------------------------------------

  *InPo			; point on the plane to make the shortest distance from the point.

				(x = *InPo,		y = *(InPo+1), z = *(InPo+2)),

																							
==========================================================================================*/

double CMakeObject::DistancePointToLine(double Point1[3], double Point2[3]) 
{
	double		Dis;
	int			i;

	Dis = (double)0;
	for(i=0;i<3;i++)
	{
		Dis += DD(Point2[i] - Point1[i]);
	}

	Dis = sqrt( Dis );
	
	return Dis;

}


/*##############################################################################################
  ##############################################################################################


		THE FOLLOWING FUNCTIONS ARE APPLICATION FUNCTIONS FOR mathmatical calculation.


							M A T H M A T I C A L	T O O L S
							---------------------------------
##############################################################################################
##############################################################################################*/

/*============================================================================================

	This function is to calculate the mean value and the standard deviation of a data set
	which consists of three dimensional coordinates

=============================================================================================*/

void	CMakeObject::Mean_SD(double XYZ[][3], int Gasu, double MeanXYZ[3], double SD_XYZ[3], double MinXYZ[3], double MaxXYZ[3], short Order)
{
	int 	i, j;                

	for(i=0;i<3;i++)
	{
		MinXYZ[i] = 100000.0;
		MaxXYZ[i] = -100000.0;
	}

	for(j=0;j<Order;j++)
	{
		MeanXYZ[j] = (double)0.0;

		for(i=0;i<Gasu;i++)
		{
			MeanXYZ[j] 	+= 	XYZ[i][j];
			MaxXYZ[j] = Max(MaxXYZ[j],XYZ[i][j]);
			MinXYZ[j] = Min(MinXYZ[j],XYZ[i][j]);
		}  

		MeanXYZ[j] 	/= 	Gasu;

	}
    
	for(j=0;j<Order;j++)
	{
		SD_XYZ[j] = (double)0.0;

		for(i=0;i<Gasu;i++)
		{
			SD_XYZ[j] 	+= 	pow(XYZ[i][j]-MeanXYZ[j],2);
		}  
		SD_XYZ[j] 	/= 	(double)(Gasu-1);
		SD_XYZ[j] 	=	(double)sqrt(SD_XYZ[j]);
	}
	
}


/*=============================================================================================

	The following function is to calculate the inverse matrix of Order x Order matrix.

==============================================================================================*/

BOOL CMakeObject::InverseMatrix(int Order, double *al)
{
	int 			i, k;
	double  		eps,det;

	eps = 1.0e-6;     
	k 	 = 0;

	i = minver((double *)al, Order, eps, &det);

	if( i == 999)
	{
		return FALSE;
	}

	return TRUE;
}

int CMakeObject::minver(double a_Inv[], int RowCol, double eps, double *det)
{

	int		i, j, k, r, iw, s, t, u, v;
	double	w, wmax, pivot, api, w1;

	if(RowCol < 2 || RowCol > 20 || eps <= 0.0) return(999);
	w1 = 1.0;
	for(i = 0; i < RowCol; i++)
			mLeastM[i] = i;
	for(k = 0; k < RowCol; k++)
	{
		wmax = 0.0;
		for(i = k; i < RowCol; i++)
		{
			w = fabs(a_Inv[i*RowCol + k]);
			if(w > wmax)
			{
				wmax = w;
				r = i;
			}
		}
		pivot = a_Inv[r*RowCol + k];
		api = fabs(pivot);
		if(api <= eps)
		{
			*det = w1;
			return(1);
		}
		w1 *= pivot;
		u = k * RowCol;
		v = r * RowCol;
		if(r != k)
		{
			w1 = -w;
			iw = (int)mLeastM[k];
			mLeastM[k] = mLeastM[r];
			mLeastM[r] = iw;
			for(j = 0; j < RowCol; j++)
			{
				s = u + j;
				t = v + j;
				w = a_Inv[s];
				a_Inv[s] = a_Inv[t];
				a_Inv[t] = w;
			}
		}
		for(i = 0; i < RowCol; i++)
			a_Inv[u+i] /= pivot;
		for(i = 0; i < RowCol; i++)
		{
			if(i != k)
			{
				v = i * RowCol;
				s = v + k;
				w = a_Inv[s];
				if(w != 0.0)
				{
					for(j = 0; j < RowCol; j++)
						if(j != k) 
							a_Inv[v+j] -= w * a_Inv[u+j];
						a_Inv[s] = -w / pivot;
				}
			}
		}
		a_Inv[u+k] = 1.0 / pivot;
	}
	for(i = 0; i < RowCol; i++)
	{
		while(1)
		{
			k = (int)mLeastM[i];
			if(k == i) break;
			iw = (int)mLeastM[k];
			mLeastM[k] = mLeastM[i];
			mLeastM[i] = iw;
			for(j = 0; j < RowCol; j++)
			{
				u = j * RowCol;
				s = u + i;
				t = u + k;
				w = a_Inv[s];
				a_Inv[s] = a_Inv[t];
				a_Inv[t] = w;
			}
		}
	}
	*det = w1;
	return(0);
}    

/*=============================================================================================

			The following function is to get the Gaussian value

==============================================================================================*/


BOOL CMakeObject::Get_Gauss(int m_GaussMean, double m_GaussSDev, double Gauss[])
{
	int 		i;
	double 		temp1,temp2;
	
	temp1 = (double)sqrt(2.*PI)*m_GaussSDev*m_GaussSDev*m_GaussSDev;
	
	for(i=0; i<(2*m_GaussMean+1);i++)
	{
        temp2=-(double)( pow((double)(i-m_GaussMean),2) )/( (double)2.*pow((double)(m_GaussSDev),2) );
        Gauss[i]=(double)(m_GaussMean-i)*(double)exp(temp2)/temp1;
	}

	return TRUE;

}

/*=============================================================================================

			The following function is to get the CORRELATION between Data1[] and Data2[].

==============================================================================================*/
BOOL CMakeObject::Correlation(double m_Gasu, double Data1[], double Data2[], int m_GaussMean, double *MaxG, double *MinG, double Correl[] )
{
	int    				i, j;
	double				sum;
	double				MaxCG, MinCG;
               
	for(i=0;i<m_Gasu;i++)
	{
		Correl[i] = (double)0.0;
	}

	for(j=0;j<(m_Gasu-(2*m_GaussMean+1));j++)
	{
		sum=(double)0.;
		for(i=0;i<(2*m_GaussMean+1);i++)
		{
			sum += (double)Data1[i+j]*Data2[i];
		}

		Correl[j+m_GaussMean]=(double)fabs(sum);
	}

	MaxCG = 0.0;
	MinCG = 1000.0;

	for(i=0;i<m_Gasu;i++)
	{
	    if(MaxCG<Correl[i]) MaxCG = Correl[i];
	    if(MinCG>Correl[i]) MinCG = Correl[i];
	}

	*MaxG = MaxCG;
	*MinG = MinCG; 

	return TRUE;

}


/*=============================================================================================

				The following function is for calculate PSEUDO-INVERSE.

==============================================================================================*/

BOOL CMakeObject::PseudoInverse(int PolyOrder, int EdgeNum, double Q[], double YY[], double YTT[])
{
	int	 		i,j,k;
	double  	wl, *al,  *YT;

	al		= new double[PolyOrder*PolyOrder];
	YT		= new double[PolyOrder];
	
	CString str;

	for(i=0; i<PolyOrder; i++)
	{
		 for(j=0; j<PolyOrder; j++)
		 {
			 if ((i*PolyOrder + j) >= PolyOrder*PolyOrder)
				 break;
			 if ((i*PolyOrder + j) >= DEF_ARRAY_SIZE)
				 break;

			  wl = 0.0;
			  for(k=0;k<EdgeNum;k++)
			  {
				  if ((k*PolyOrder + i) >= DEF_ARRAY_SIZE)
					  break;
				  if ((j + k*PolyOrder) >= DEF_ARRAY_SIZE)
					  break;

				  wl += Q[k*PolyOrder+i]*Q[j+k*PolyOrder];
			  }
// 			  *(al+i*PolyOrder+j) = wl;
			  al[i*PolyOrder + j] = wl;
		 }
	 }
	 
     InverseMatrix( PolyOrder, al);

	for(j = 0; j <PolyOrder; j++)
	{
		if (j >= DEF_ARRAY_SIZE)
			break;

		wl = 0.0;
		for(k=0;k<EdgeNum;k++)
		{
			if ((j + k*PolyOrder) >= DEF_ARRAY_SIZE)
				break;
			if (k >= DEF_ARRAY_SIZE)
				break;

			wl += YY[k]*Q[j+k*PolyOrder];
		}
		YT[j] = (double)wl;
	}

	for(j=0;j<PolyOrder;j++)
	{
		if (j >= DEF_ARRAY_SIZE)
			break;

		wl = (double)0.0;
		for(k=0;k<PolyOrder;k++)
		{
			if (k >= DEF_ARRAY_SIZE)
				break;

			wl += *(al+j*PolyOrder+k)*YT[k];
		}
		YTT[j] = wl;	// coefficients of polynomial function
	}  

	delete al;
	delete YT;

	return TRUE;

} 


/*=============================================================================================

				The following function is to Sort data according to angle.

==============================================================================================*/

BOOL CMakeObject::SortWithAngle(double Point[][3],  int Gasu, double Angle[], short Mode)
{ 
																		// if Mode = 0, then neglect z axis
	double		xDir,yDir,zDir;
	double		Angle1, Temp;
	double		MeanXYZ[3];
	double		SD_XYZ[3];
	double		MinXYZ[3];
	double		MaxXYZ[3];
	int			i,j;
//	int		k;
	double		TempPoint[3];
	double		Cir;

	Mean_SD(Point, Gasu, MeanXYZ, SD_XYZ, MinXYZ, MaxXYZ, 3);

	CircleC(Point, Gasu, MeanXYZ, &Temp, &Cir);

	for(i=0; i<Gasu;i++)
	{
		xDir = Point[i][0] - MeanXYZ[0];
		yDir = Point[i][1] - MeanXYZ[1];
		zDir = Point[i][2] - MeanXYZ[2];
		
		Temp = sqrt( DD(xDir) + DD(yDir) + DD(zDir)); 
		
		xDir /= Temp;
		yDir /= Temp;
		zDir /= Temp;

		if( xDir > 0.00 && yDir > 0.00)
		{
			Angle1 = acos( xDir );
		}
		else if( xDir < 0.00 && yDir > 0.00)
		{
			Angle1  = PI - acos(fabs(xDir));
		}
		else if( xDir < 0.00 && yDir < 0.00)
		{
			Angle1  = PI + acos(fabs(xDir));
		}
		else if( xDir > 0.00 && yDir < 0.00)
		{
			Angle1  = 2*PI - acos(xDir);
		}
		else if( fabs(xDir) < Error && yDir > 0.00)
		{
			Angle1  = PI/2.0;
		}
		else if( fabs(xDir) < Error && yDir < 0.00)
		{
			Angle1  = 3*PI/2.0;
		}
		else if( fabs(yDir) < Error && xDir > 0.00)
		{
			Angle1  = 0.0;
		}
		else if( fabs(yDir) < Error && xDir < 0.00)
		{
			Angle1  = PI;
		}

		Angle[i] = Angle1;

	}

	for( i = 0; i< Gasu; i++)
	{
//		int k;
		for( j=i+1; j<Gasu;j++)
		{
			if( Angle[j] < Angle[i] )
			{
				TempPoint[0] = Point[i][0];
				TempPoint[1] = Point[i][1];
				TempPoint[2] = Point[i][2];
				
				Point[i][0] = Point[j][0];
				Point[i][1] = Point[j][1];
				Point[i][2] = Point[j][2];

				Point[j][0] = TempPoint[0];
				Point[j][1] = TempPoint[1];
				Point[j][2] = TempPoint[2];

				Temp = Angle[i];
				Angle[i] = Angle[j];
				Angle[j] = Temp;

			}
		}
	}

	return TRUE;
}

int CMakeObject::Get_roots(double a,double b,double c, double *root1,double *root2)
{
	double det;

	det=b*b-4*a*c; 

	if (det>0)
	{// 실근 2개 
		*root1=(-b+pow(det,0.5))/(2*a); 
		*root2=(-b-pow(det,0.5))/(2*a); 
		return 1;
	} 
	else if (det==0)
	{//중근 

		*root1=-b/(2*a); 
		*root2=-b/(2*a); 
		return 2;
	} 
	else
	{//복소근
		*root1=-b/(2*a);//실수부 
		*root2=pow(fabs(det),0.5)/(2*a);//허수부 
		return 0;
	} 
	return 0;
}

int CMakeObject::Get_circle_y(double x,double y,double r,double xk,double *yk)
{
	double temp=r*r-(xk-x)*(xk-x);

	if (temp<0.0)
	{
		return 0;
	}
	*yk=pow(temp,0.5)+y;
	return 1;
}

int CMakeObject::Get_circle_x(double x,double y,double r,double yk,double *xk)
{
	double temp=r*r-(yk-y)*(yk-y);

	if (temp<0.0)
	{
		return 0;
	}
	*xk=pow(temp,0.5)+x;
	return 1;
}

//기울기가 a이고 xk,yk를 지나는 직선과 원과의 교점을 구한다.
int CMakeObject::Get_circle_y2(double xo,double yo,double ro,double a,double xk,double yk,double *xp,double *yp)
{
	double b;
	double r,s,t;//2차방정식 계수.
	double root1,root2;
	int ret;


	b=a*(-xk)+yk;

	r=(a*a+1);
	s=(-2.0)*xo+2*a*(b-yo);
	t=xo*xo+(b-yo)*(b-yo)-ro*ro;

	ret=Get_roots(r,s,t,&root1,&root2);
	if (ret>0)
	{
		*xp=root1;
		*yp=a*root1+b;
	}
	return 0;
}

double CMakeObject::Distance(double x1,double y1,double x2,double y2)
{
	double dist=(x1-x2)*(x1-x2)+(y1-y2)*(y1-y2);

	dist=pow(dist,0.5);
	return dist;
}

//yo값이 같을때 
int CMakeObject::Get_point_1(double xo1,double yo1,double ro1,double xo2,double yo2,double ro2,double *xp1,double *yp1,double *xp2,double *yp2)
{
	double center_dist;
	double start_x,end_x;
// 	double a;
	double xk,yk;
	double error;
	double len1,len2;

	center_dist=Distance(xo1,yo1,xo2,yo2);
	if (center_dist>(ro1+ro2))
	{
		return 0;
	}

	if (ro1>ro2)
	{
		if ((center_dist+ro2)<ro1)
		{
			return 0;
		}
	}
	else
	{
		if ((center_dist+ro1)<ro2)
		{
			return 0;
		}
	}

	// a=-(xo1-xo2)/(yo1-yo2);

	if (xo1<xo2)
	{
		start_x=xo1-ro1;
		end_x=xo2+ro2;
	}
	else
	{
		start_x=xo2-ro2;
		end_x=xo1+ro1;
	}

	error=1;
	while(error>threshold)
	{
		xk=(start_x+end_x)/2;
		Get_circle_y(xo1,yo1,ro1,xk,&yk);
		len1=Distance(xk,yo1,xk,yk);
		Get_circle_y(xo2,yo2,ro2,xk,&yk);
		len2=Distance(xk,yo1,xk,yk);

		error=fabs(len1-len2);
		if (len1>len2)
		{
			start_x=xk;
		}
		else
		{
			end_x=xk;
		}
	}

	*xp1=xk;
	*xp2=xk;

	*yp1=yo1+len1;
	*yp2=yo1-len1;

	return 1;
}

//xo값이 같을때 
int CMakeObject::Get_point_2(double xo1,double yo1,double ro1,double xo2,double yo2,double ro2,double *xp1,double *yp1,double *xp2,double *yp2)
{
	double center_dist;
	double start_y,end_y;
// 	double a;
	double xk,yk;
	double error;
	double len1,len2;

	center_dist=Distance(xo1,yo1,xo2,yo2);
	if (center_dist>(ro1+ro2))
	{
		return 0;
	}

	if (ro1>ro2)
	{
		if ((center_dist+ro2)<ro1)
		{
			return 0;
		}
	}
	else
	{
		if ((center_dist+ro1)<ro2)
		{
			return 0;
		}
	}

	// a=-(xo1-xo2)/(yo1-yo2);

	if (yo1<yo2)
	{
		start_y=yo1-ro1;
		end_y=yo2+ro2;
	}
	else
	{
		start_y=yo2-ro2;
		end_y=yo1+ro1;
	}

	error=1;
	while(error>threshold)
	{
		yk=(start_y+end_y)/2;
		Get_circle_x(xo1,yo1,ro1,yk,&xk);
		len1=Distance(xk,yo1,xk,yk);
		Get_circle_x(xo2,yo2,ro2,yk,&xk);
		len2=Distance(xk,yo1,xk,yk);

		error=fabs(len1-len2);
		if (len1>len2)
		{
			start_y=yk;
		}
		else
		{
			end_y=yk;
		}
	}

	*yp1=yk;
	*yp2=yk;

	*xp1=xo1+len1;
	*xp2=xo1-len1;

	return 1;
}

//원의 중심이 다 다를때 
int CMakeObject::Get_point_3(double xo1,double yo1,double ro1,double xo2,double yo2,double ro2,double *xp1,double *yp1,double *xp2,double *yp2)
{

	double center_dist;
	double start_x,end_x;
	double a;
	double a1,b1;
	double xk,yk;
	double xp,yp;
	double error;
	double len1,len2;

	center_dist=Distance(xo1,yo1,xo2,yo2);
	if (center_dist>(ro1+ro2))
	{
		return 0;
	}

	if (ro1>ro2)
	{
		if ((center_dist+ro2)<ro1)
		{
			return 0;
		}
	}
	else
	{
		if ((center_dist+ro1)<ro2)
		{
			return 0;
		}
	}

	a=-(xo1-xo2)/(yo1-yo2);
	a1=-1/a;
	b1=-a1*xo1+yo1;


	if (xo1<xo2)
	{
		start_x=xo1-ro1;
		end_x=xo2+ro2;
	}
	else
	{
		start_x=xo2-ro2;
		end_x=xo1+ro1;
	}

	error=1;
	while(error>threshold)
	{
		xk=(start_x+end_x)/2;
		yk=a1*xk+b1;
		Get_circle_y2(xo1,yo1,ro1,a,xk,yk,&xp,&yp);

		len1=Distance(xk,yk,xp,yp);

		Get_circle_y2(xo2,yo2,ro2,a,xk,yk,&xp,&yp);
		len2=Distance(xk,yk,xp,yp);

		error=fabs(len1-len2);
		if (len1>len2)
		{
			start_x=xk;
		}
		else
		{
			end_x=xk;
		}
	}

	*xp1=xp;
	*yp1=yp;

	if (xk>xp)
	{
		*xp2=xk+(xk-xp);
	}
	else
	{
		*xp2=xk-(xk-xp);
	}

	if (yk>yp)
	{
		*yp2=yk+(yk-yp);
	}
	else
	{
		*yp2=yk-(yk-yp);
	}

	return 1;
}

bool CMakeObject::InterPointCircleToCircle2(double cXYZ1[3],double Rad1, double cXYZ2[3], double Rad2, double InPo[2][3], short *Gasu) 
{
	double xp1, yp1, xp2, yp2;
	int res;
	if(cXYZ1[0]==cXYZ2[0])
	{
		res = Get_point_2(cXYZ1[0],cXYZ1[1],Rad1,cXYZ2[0],cXYZ2[1],Rad2,&xp1,&yp1,&xp2,&yp2);
	}
	else if (cXYZ1[1]==cXYZ2[1])
	{
		res = Get_point_1(cXYZ1[0],cXYZ1[1],Rad1,cXYZ2[0],cXYZ2[1],Rad2,&xp1,&yp1,&xp2,&yp2);
	}
	else
	{
		res = Get_point_3(cXYZ1[0],cXYZ1[1],Rad1,cXYZ2[0],cXYZ2[1],Rad2,&xp1,&yp1,&xp2,&yp2);
	}

	if(res==1)
	{
		InPo[0][0] = xp1;	InPo[0][1] = yp1;
		InPo[1][0] = xp2;	InPo[1][1] = yp2;
		if(xp1==xp2 && yp1==yp2)	*Gasu = 1;
		else						*Gasu = 2;
		return true;
	}

	return false;
}

void CMakeObject::FittingParabolic(double* Point1, double* Point2, double* Point3, double dConst[3])
{
	double A, B, C;

	A = (Point3[1] * ((Point1[0] * Point1[0]) - (Point2[0] * Point2[0]))
		+ Point2[1] * ((Point3[0] * Point3[0]) - (Point1[0] * Point1[0]))
		+ Point1[1] * ((Point2[0] * Point2[0]) - (Point3[0] * Point3[0])))
		/ 2
		/ ((Point3[1] - Point2[1]) * Point1[0]
			+ (Point1[1] - Point3[1]) * Point2[0]
			+ (Point2[1] - Point1[1]) * Point3[0]);
	C = (Point1[1] - Point2[1]) / ((Point1[0] * Point1[0]) - (Point2[0] * Point2[0]) + 2 * A*(Point2[0] - Point1[0]));
	B = Point2[1] - C*(Point2[0] - A)*(Point2[0] - A);

	dConst[0] = C;
	dConst[1] = -2*A*C;
	dConst[2] = B+A*A*C;

	return;
}

bool CMakeObject::GetInEllipse(double cx, double cy, double xr, double yr, double edgeX, double edgeY)
{
	if(((edgeX-cx)*(edgeX-cx)/(xr*xr)) + ((edgeY-cy)*(edgeY-cy)/(yr*yr)) > 1.0)
		return false;

	return true;
}

void CMakeObject::Node(double gradient1, double y_intercept1, double gradient2, double y_intercept2, double *nodeX, double *nodeY)
{ // 가로   /    세로
	if(gradient1==0)
	{
		if(gradient2==0)
		{
			*nodeX = y_intercept2;
			*nodeY = y_intercept1;
		}
		else 
		{ 
			*nodeX = (gradient2*y_intercept1)+y_intercept2;
			*nodeY = y_intercept1;
		}
	}
	else 
	{
		if(gradient2==0) 
		{
			*nodeX = y_intercept2;
			*nodeY = gradient1*y_intercept2 + y_intercept1;
		}
		else 
		{
			*nodeX = ((-1)*((y_intercept2/gradient2)+y_intercept1) / (gradient1 - (1/gradient2)));
			*nodeY = gradient1*(*nodeX)+y_intercept1;
		}
	}
}

float CMakeObject::GetDistanceLineToDot(float gradient, float Y_value, CDPoint dot, int direction, int* x, int* y, int direction2, int bdNo, int camNo)
{	// direction	0: Left/Right   1: Top/Bottom
	// direction2	0: 점의 위치 파악
	float fGradient;
	float fYvalue;
	float xP, yP;
	float dis;
//	double store;

// 	if(direction2==0)
// 	{
// 		store = dot.y - (gradient*dot.x+Y_value);
// 		if(store>0.f)	
// 			return -1.f;
// 	}
// 	else if(direction2==1)
// 	{
// 		store = dot.y - (gradient*dot.x+Y_value);
// 		if(store<0.f)		return -1.f;
// 	}
// 	else if(direction2==2)
// 	{
// 		store = dot.x - (gradient*dot.y+Y_value);
// 		if(store<0.f)		return -1.f;
// 	}
// 	else
// 	{
// 		store = dot.x - (gradient*dot.y+Y_value);
// 		if(store>0.f)		return -1.f;
// 	}


	//	if(!g_autoFlag)
	{
		if(gradient!=0.f)
		{
			fGradient = (-1.f)/gradient;
		}
		else
			fGradient = 0.f;
		if(direction)
			fYvalue = (float)dot.y - (float)(fGradient*dot.x);
		else
			fYvalue = (float)dot.x - (float)(fGradient*dot.y);
		if(gradient!=0.f)
		{
			if(!direction)
			{
				// 				yP = (float)( ((float)fYvalue-(float)Y_value)/((float)gradient-(float)(fGradient)) );
				// 				xP = (float)(gradient*yP) + Y_value;
				// 				dis = (dot.x-xP)*(dot.x-xP) + (dot.y-yP)*(dot.y-yP);
				// 				dis = (float)(sqrt(dis));
				// 				*x = (int)xP;
				// 				*y = (int)yP;
				xP = (float)( ((float)fYvalue-(float)Y_value)/((float)gradient-(float)(fGradient)) );
				yP = (float)(gradient*xP) + Y_value;
				dis = (float)((dot.x-yP)*(dot.x-yP) + (dot.y-xP)*(dot.y-xP));
				dis = (float)(sqrt(dis));
				dis = dis;
				*x = (int)yP;
				*y = (int)xP;
			}
			else
			{
				xP = (float)( ((float)fYvalue-(float)Y_value)/((float)gradient-(float)(fGradient)) );
				yP = (float)(gradient*xP) + Y_value;
				dis = (float)((dot.x-xP)*(dot.x-xP) + (dot.y-yP)*(dot.y-yP));
				dis = (float)(sqrt(dis));
				dis = dis;
				*x = (int)xP;
				*y = (int)yP;
			}
		}
		else
		{
			if(direction)
			{
				dis = (float)fabs(Y_value-dot.y);
				dis = dis;
				*x = (int)dot.x;
				*y = (int)Y_value;
			}
			else
			{
				dis = (float)fabs(Y_value-dot.x);
				dis = dis;
				*x = (int)Y_value;
				*y = (int)dot.y;
			}
		}
	}
	// 	else
	// 	{
	// 		if(direction)
	// 			dis = (float)fabs((float)dot.y - (float)(gradient*dot.x+Y_value));
	// 		else
	// 			dis = (float)fabs((float)dot.x - (float)(gradient*dot.y+Y_value));
	// 	}

	return dis;
}

float CMakeObject::GetDistanceLineToDot_float(float gradient, float Y_value, CDPoint dot, int direction, float* x, float* y, int direction2, int bdNo, int camNo)
{	// direction	0: Left/Right   1: Top/Bottom
	// direction2	0: 점의 위치 파악
	float fGradient;
	float fYvalue;
	float xP, yP;
	float dis;
	//	double store;

	// 	if(direction2==0)
	// 	{
	// 		store = dot.y - (gradient*dot.x+Y_value);
	// 		if(store>0.f)	
	// 			return -1.f;
	// 	}
	// 	else if(direction2==1)
	// 	{
	// 		store = dot.y - (gradient*dot.x+Y_value);
	// 		if(store<0.f)		return -1.f;
	// 	}
	// 	else if(direction2==2)
	// 	{
	// 		store = dot.x - (gradient*dot.y+Y_value);
	// 		if(store<0.f)		return -1.f;
	// 	}
	// 	else
	// 	{
	// 		store = dot.x - (gradient*dot.y+Y_value);
	// 		if(store>0.f)		return -1.f;
	// 	}


	//	if(!g_autoFlag)
	{
		if(gradient!=0.f)
		{
			fGradient = (-1.f)/gradient;
		}
		else
			fGradient = 0.f;
		if(direction)
			fYvalue = (float)dot.y - (float)(fGradient*dot.x);
		else
			fYvalue = (float)dot.x - (float)(fGradient*dot.y);
		if(gradient!=0.f)
		{
			if(!direction)
			{
				// 				yP = (float)( ((float)fYvalue-(float)Y_value)/((float)gradient-(float)(fGradient)) );
				// 				xP = (float)(gradient*yP) + Y_value;
				// 				dis = (dot.x-xP)*(dot.x-xP) + (dot.y-yP)*(dot.y-yP);
				// 				dis = (float)(sqrt(dis));
				// 				*x = (int)xP;
				// 				*y = (int)yP;
				xP = (float)( ((float)fYvalue-(float)Y_value)/((float)gradient-(float)(fGradient)) );
				yP = (float)(gradient*xP) + Y_value;
				dis = (float)((dot.x-yP)*(dot.x-yP) + (dot.y-xP)*(dot.y-xP));
				dis = (float)(sqrt(dis));
				dis = dis;
				*x = yP;
				*y = xP;
			}
			else
			{
				xP = (float)( ((float)fYvalue-(float)Y_value)/((float)gradient-(float)(fGradient)) );
				yP = (float)(gradient*xP) + Y_value;
				dis = (float)((dot.x-xP)*(dot.x-xP) + (dot.y-yP)*(dot.y-yP));
				dis = (float)(sqrt(dis));
				dis = dis;
				*x = xP;
				*y = yP;
			}
		}
		else
		{
			if(direction)
			{
				dis = (float)fabs(Y_value-dot.y);
				dis = dis;
				*x = (float)dot.x;
				*y = Y_value;
			}
			else
			{
				dis = (float)fabs(Y_value-dot.x);
				dis = dis;
				*x = Y_value;
				*y = (float)dot.y;
			}
		}
	}
	// 	else
	// 	{
	// 		if(direction)
	// 			dis = (float)fabs((float)dot.y - (float)(gradient*dot.x+Y_value));
	// 		else
	// 			dis = (float)fabs((float)dot.x - (float)(gradient*dot.y+Y_value));
	// 	}

	return dis;
}
