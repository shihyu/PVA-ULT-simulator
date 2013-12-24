#include "PlaneDetector.h"
//#define VECTOR_SOLVER
#define ANGLE_SOLVER
//#define VECTOR2_SOLVER
//#define ANGLE2_SOLVER
class DetectionData
{
public:
	Vector vC;
	Vector vB;
	Vector vL;
	double fC;
	double fR;
public:
	DetectionData(Vector vListener, Vector vB, double fR)
	{
		this->vB = vB;
		this->vL = vListener;
		this->vC = vListener - vB;
		this->fC = - vC.getSqrSize() + pow(fR, 2);
		this->fR = fR;
	}
	DetectionData(){}
	~DetectionData(){}


	// vN should be a unit vector. |vN| = 1
	double GetF(Vector vN, double d)
	{
		double fCN = vC & vN;
		double ftSqrtPart = pow(fCN, 2) + fC;
		if (ftSqrtPart < 0) 
		{
			printf("PlaneDetector.cpp : DetectionData.GetF, negative value in ftSqrtPart = %f\n", ftSqrtPart);
			getchar();
			exit(90);
		}
		double ft = fCN + sqrt(ftSqrtPart);
		double ff = (vB & vN) + (ft/2.0) + d;
//		return fabs(ff);
		return ff;
	}
	
	// vN should be a unit vector. |vN| = 1
	Vector GetDF(Vector vN)
	{
		double fCN = vC & vN;
		double ftSqrtPart = pow(fCN, 2) + fC;

		if (ftSqrtPart < 0) 
		{
			printf("PlaneDetector.cpp : DetectionData.GetDF, negative value in ftSqrtPart = %f\n", ftSqrtPart);
			getchar();
			exit(91);
		}

		double fdtPart = 1 + (fCN / sqrt(ftSqrtPart));
		Vector vdt = vC * (fdtPart / 2.0);

		Vector vdf = vB + vdt;

		return vdf;
	}

	double GetF(double theta, double pi, double d)
	{
		Vector vN = Vector(sin(pi)*cos(theta), sin(pi)*sin(theta), cos(pi));
//		return GetF(vN, d);
		double fCN = vC & vN;
		double fTSqrtPart = pow(fCN, 2) + fC;
		double fT = fCN + sqrt(fTSqrtPart);
		double fD = - ((vN & vL) + d);
		double fF = (vB & vN) + (fT / 2.0) + fD;

		return fF;
	}

	Vector GetDF(double theta, double pi, double d)
	{
		Vector vN = Vector(sin(pi)*cos(theta), sin(pi)*sin(theta), cos(pi));
		double fCN = vC & vN;
		double fTSqrtPart = pow(fCN, 2) + fC;
		double fTSqrtInv = (1.0 / sqrt(fTSqrtPart));
		Vector vNdTheta = Vector(-sin(pi) * sin(theta), sin(pi) * cos(theta), 0);
		Vector vNdPi = Vector(cos(pi) * cos(theta), cos(pi) * sin(theta), -sin(pi));

		double fBNdTheta = vB & vNdTheta;			
		double fCNdTheta = vC & vNdTheta;
		double fTdTheta = fCNdTheta + fTSqrtInv * fCN *  fCNdTheta;
		double fDdTheta = - (vL & vNdTheta);
		double fFdTheta = fBNdTheta + (fTdTheta / 2.0) + fDdTheta;

		double fBNdPi = vB & vNdPi;
		double fCNdPi = vC & vNdPi;
		double fTdPi = fCNdPi + fTSqrtInv * fCN * fCNdPi;
		double fDdPi = - (vL & vNdPi);
		double fFdPi = fBNdPi + (fTdPi / 2.0) + fDdPi;

		return Vector(fFdTheta, fFdPi, -1);
	}
	double GetF(Vector vN, Vector vX1)
	{
		return GetF(vN, -(vN&vX1));
	}
	Vector GetDF(Vector vN, Vector vX1)
	{
		double fCN = vC & vN;
		double ftSqrtPart = pow(fCN, 2) + fC;

		if (ftSqrtPart < 0) 
		{
			printf("PlaneDetector.cpp : DetectionData.GetDF, negative value in ftSqrtPart = %f\n", ftSqrtPart);
			getchar();
			exit(91);
		}

		double fdtPart = 1 + (fCN / sqrt(ftSqrtPart));
		Vector vdt = vC * (fdtPart / 2.0);
		Vector vdd = -vX1;

		Vector vdf = vB + vdt + vdd;

		return vdf;
	}

	double GetF(double theta, double pi, Vector vX1)
	{		
		Vector vN = Vector(sin(pi)*cos(theta), sin(pi)*sin(theta), cos(pi));
		double fCN = vC & vN;
		double fTSqrtPart = pow(fCN, 2) + fC;
		double fT = fCN + sqrt(fTSqrtPart);
		double fF = (vB & vN) + (fT / 2.0) + (-(vN&vX1));

		return fF;
	}

	Vector GetDF(double theta, double pi, Vector vX1)
	{
		Vector vN = Vector(sin(pi)*cos(theta), sin(pi)*sin(theta), cos(pi));
		double fCN = vC & vN;
		double fTSqrtPart = pow(fCN, 2) + fC;
		double fTSqrtInv = (1.0 / sqrt(fTSqrtPart));

		Vector vNdTheta = Vector(-sin(pi) * sin(theta), sin(pi) * cos(theta), 0);
		Vector vNdPi = Vector(cos(pi) * cos(theta), cos(pi) * sin(theta), -sin(pi));

		double fBNdTheta = vB & vNdTheta;
		double fCNdTheta = vC & vNdTheta;
		double fTdTheta = fCNdTheta + fTSqrtInv * fCN *  fCNdTheta;
		double fDdTheta = - (vX1 & vNdTheta);
		double fFdTheta = fBNdTheta + (fTdTheta / 2.0) + fDdTheta;

		double fBNdPi = vB & vNdPi;
		double fCNdPi = vC & vNdPi;
		double fTdPi = fCNdPi + fTSqrtInv * fCN * fCNdPi;
		double fDdPi = - (vX1 & vNdPi);
		double fFdPi = fBNdPi + (fTdPi / 2.0) + fDdPi;

		return Vector(fFdTheta, fFdPi, 1);
	}


	Vector GetP(Vector vN)
	{
		double fCN = vC & vN;
		double ftSqrtPart = pow(fCN, 2) + fC;
		double ft = fCN + sqrt(ftSqrtPart);

		return vB + (vN * (ft / 2.0));

	}
	void Print()
	{
		printf("vC = ");
		vC.Print();
		printf("\tvB = ");
		vB.Print();
		printf("\tvL = ");
		vL.Print();
		printf("\fC = %f", fC);

		printf("\n");
	}



};

PlaneDetector::PlaneDetector()
{
	fpA = fopen("data.txt", "w");
	fpD = fopen("PD_distance.txt", "w");

}

PlaneDetector::~PlaneDetector()
{
	fclose(fpA);
	fclose(fpD);
}

// plane normal fitting
#ifdef VECTOR_SOLVER
int pd_func_f(const gsl_vector *x, void *ptrData, gsl_vector *f)
{
	std::vector<DetectionData>* data = (std::vector<DetectionData>*) ptrData;
	Vector vN = Vector(
			gsl_vector_get(x, 0),
			gsl_vector_get(x, 1),
			gsl_vector_get(x, 2)).getUnitVector();
	double fd = gsl_vector_get(x, 3);

	for (size_t i = 0; i < data->size(); i++)
	{
		gsl_vector_set(f, i, (*data)[i].GetF(vN, fd));
	}
	return GSL_SUCCESS;
}
int pd_func_df(const gsl_vector *x, void *ptrData, gsl_matrix *J)
{
	std::vector<DetectionData>* data = (std::vector<DetectionData>*) ptrData;
	Vector vN = Vector(
			gsl_vector_get(x, 0),
			gsl_vector_get(x, 1),
			gsl_vector_get(x, 2)).getUnitVector();

	for (size_t i = 0; i < data->size(); i++)
	{
		Vector vJi = (*data)[i].GetDF(vN);
		gsl_matrix_set(J, i, 0, vJi.x);
		gsl_matrix_set(J, i, 1, vJi.y);
		gsl_matrix_set(J, i, 2, vJi.z);
		gsl_matrix_set(J, i, 3, 1.0);
	}

	return GSL_SUCCESS;
}
#endif
#ifdef ANGLE_SOLVER 
int pd_func_f(const gsl_vector *x, void *ptrData, gsl_vector *f)
{
	std::vector<DetectionData>* data = (std::vector<DetectionData>*) ptrData;
	double theta = gsl_vector_get(x, 0);
	double pi = gsl_vector_get(x, 1);
	double d = gsl_vector_get(x, 2);

	for (size_t i = 0; i < data->size(); i++)
	{
		gsl_vector_set(f, i, (*data)[i].GetF(theta, pi, d));
	}
	return GSL_SUCCESS;
}
int pd_func_df(const gsl_vector *x, void *ptrData, gsl_matrix *J)
{
	std::vector<DetectionData>* data = (std::vector<DetectionData>*) ptrData;
	double theta = gsl_vector_get(x, 0);
	double pi = gsl_vector_get(x, 1);
	double d = gsl_vector_get(x, 2);
	
	for (size_t i = 0; i < data->size(); i++)
	{
		Vector vJi = (*data)[i].GetDF(theta, pi, d);
		gsl_matrix_set(J, i, 0, vJi.x);
		gsl_matrix_set(J, i, 1, vJi.y);
		gsl_matrix_set(J, i, 2, vJi.z);
	}

	return GSL_SUCCESS;
}
#endif
#ifdef VECTOR2_SOLVER
int pd_func_f(const gsl_vector *x, void *ptrData, gsl_vector *f)
{
	std::vector<DetectionData>* data = (std::vector<DetectionData>*) ptrData;
	Vector vN = Vector(
			gsl_vector_get(x, 0),
			gsl_vector_get(x, 1),
			gsl_vector_get(x, 2)).getUnitVector();
	Vector vX1 = Vector(
			gsl_vector_get(x, 3),
			gsl_vector_get(x, 4),
			gsl_vector_get(x, 5));

	for (size_t i = 0; i < data->size(); i++)
	{
		gsl_vector_set(f, i, (*data)[i].GetF(vN, vX1));
	}
	return GSL_SUCCESS;
}
int pd_func_df(const gsl_vector *x, void *ptrData, gsl_matrix *J)
{
	std::vector<DetectionData>* data = (std::vector<DetectionData>*) ptrData;
	Vector vN = Vector(
			gsl_vector_get(x, 0),
			gsl_vector_get(x, 1),
			gsl_vector_get(x, 2)).getUnitVector();
	Vector vX1 = Vector(
			gsl_vector_get(x, 3),
			gsl_vector_get(x, 4),
			gsl_vector_get(x, 5));

	for (size_t i = 0; i < data->size(); i++)
	{
		Vector vJi = (*data)[i].GetDF(vN, vX1);
		gsl_matrix_set(J, i, 0, vJi.x);
		gsl_matrix_set(J, i, 1, vJi.y);
		gsl_matrix_set(J, i, 2, vJi.z);
		gsl_matrix_set(J, i, 3, -vN.x);
		gsl_matrix_set(J, i, 4, -vN.y);
		gsl_matrix_set(J, i, 5, -vN.z);
	}

	return GSL_SUCCESS;
}
#endif
#ifdef ANGLE2_SOLVER
int pd_func_f(const gsl_vector *x, void *ptrData, gsl_vector *f)
{
	std::vector<DetectionData>* data = (std::vector<DetectionData>*) ptrData;
	double theta = gsl_vector_get(x, 0);
	double pi = gsl_vector_get(x, 1);
	Vector vX1 = Vector(
			gsl_vector_get(x, 2),
			gsl_vector_get(x, 3),
			gsl_vector_get(x, 4));

	for (size_t i = 0; i < data->size(); i++)
	{
		gsl_vector_set(f, i, (*data)[i].GetF(theta, pi, vX1));
	}
	return GSL_SUCCESS;
}
int pd_func_df(const gsl_vector *x, void *ptrData, gsl_matrix *J)
{
	std::vector<DetectionData>* data = (std::vector<DetectionData>*) ptrData;
	double theta = gsl_vector_get(x, 0);
	double pi = gsl_vector_get(x, 1);
	Vector vX1 = Vector(
			gsl_vector_get(x, 2),
			gsl_vector_get(x, 3),
			gsl_vector_get(x, 4));
	
	for (size_t i = 0; i < data->size(); i++)
	{
		Vector vJi = (*data)[i].GetDF(theta, pi, vX1);
		gsl_matrix_set(J, i, 0, -vJi.x);
		gsl_matrix_set(J, i, 1, -vJi.y);
		gsl_matrix_set(J, i, 2, -(sin(pi) * cos(theta)));
		gsl_matrix_set(J, i, 3, -(sin(pi) * sin(theta)));
		gsl_matrix_set(J, i, 4, -(cos(pi)));		
	}

	return GSL_SUCCESS;
}
#endif
int pd_func_fdf(const gsl_vector *x, void *data, gsl_vector *f, gsl_matrix *J)
{
	pd_func_f(x, data, f);
	pd_func_df(x, data, J);
	return GSL_SUCCESS;
}
/*
// plane offset fitting
int pof_func_f(const gsl_vector *x, void *ptrData, gsl_vector *f)
{
	// the first element of ptrData is vN
	std::vector<Vector> *data = (std::vector<Vector>*) ptrData;
	Vector vN = (*data)[0];
	Vector vP0 = Vector(
			gsl_vector_get(x, 0),
			gsl_vector_get(x, 1),
			gsl_vector_get(x, 2));

	for (size_t i = 0; i < data->size()-1; i++)
	{
		gsl_vector_set(f, i, vN & ((*data)[i+1] - vP0));
	}
	return GSL_SUCCESS;
}

int pof_func_df(const gsl_vector *x, void *ptrData, gsl_matrix *J)
{
	// the first element of ptrData is vN
	std::vector<Vector> *data = (std::vector<Vector>*) ptrData;
	Vector vN = (*data)[0];

	for (size_t i = 0; i < data->size()-1; i++)
	{
		gsl_matrix_set(J, i, 0, -vN.x);
		gsl_matrix_set(J, i, 1, -vN.y);
		gsl_matrix_set(J, i, 2, -vN.z);
	}
	return GSL_SUCCESS;
}
int pof_func_fdf(const gsl_vector *x, void *data, gsl_vector *f, gsl_matrix *J)
{
	pof_func_f(x, data, f);
	pof_func_df(x, data, J);
	return GSL_SUCCESS;
}
*/

Vector PlaneDetector::getPlaneNormal(HeardList& heardList, Vector vListener)
{
	// prepare calculation data
	std::vector<DetectionData> data;

	DetectionData baseData;

	for (size_t i = 0; i < heardList.heardInfo.size(); i++)
	{
		DetectionData curData = 
				DetectionData(vListener, heardList.heardInfo[i].location, heardList.heardInfo[i].distance);
		data.push_back(curData);		
	}

	// prepare gsl variables
	const gsl_multifit_fdfsolver_type *T;
	gsl_multifit_fdfsolver *s;

	int n = (int)data.size();
#ifdef VECTOR_SOLVER
	const int p = 4;
	double x_init[p] = {0.0, 0.0, 1.0, 0.0};
#endif
#ifdef ANGLE_SOLVER
	const int p = 3;
//	double x_init[p] = {0.0, 0.0, 0.0};
	double x_init[p] = {0.0, 90.0 / 180.0 * M_PI, 0.0};
#endif
#ifdef VECTOR2_SOLVER
	const int p = 6;
//	double x_init[p] = {1.0, 0.0, 0.0, 500.0, 0.0, 0.0};
	double x_init[p] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
#endif
#ifdef ANGLE2_SOLVER
	const int p = 5;
	double x_init[p] = {0.0, M_PI / 2.0 + 0.1, 000.0, 0.0, 0.0};
#endif

	gsl_multifit_function_fdf f;
	f.f = &pd_func_f;
	f.df = &pd_func_df;
	f.fdf = &pd_func_fdf;
	f.n = n;
	f.p = p;
	f.params = &data;
	gsl_vector_view x = gsl_vector_view_array(x_init, p);

	T = gsl_multifit_fdfsolver_lmsder;
	s = gsl_multifit_fdfsolver_alloc(T, n, p);
	gsl_multifit_fdfsolver_set(s, &f, &x.vector);

	gsl_vector *gradt = gsl_vector_alloc(p);

	int iter = 0;
	int status;
	do
	{
		iter ++;
		status = gsl_multifit_fdfsolver_iterate(s);
		if (status)
		{
			printf ("error: %s\n", gsl_strerror (status));
			break;
		}
		gsl_multifit_gradient(s->J, s->f, gradt);
		status = gsl_multifit_test_gradient(gradt, 1e-6);
	} while (status == GSL_CONTINUE && iter < 1000000);

#ifdef VECTOR_SOLVER
	vN = Vector(
			gsl_vector_get(s->x, 0),
			gsl_vector_get(s->x, 1),
			gsl_vector_get(s->x, 2)).getUnitVector();
	fD = gsl_vector_get(s->x, 3);
#endif
#ifdef ANGLE_SOLVER
	double theta = gsl_vector_get(s->x, 0);
	double pi = gsl_vector_get(s->x, 1);
	vN = Vector(sin(pi)*cos(theta), sin(pi)*sin(theta), cos(pi));
	double d = gsl_vector_get(s->x, 2);
	fD = - ((vN & vListener) + d);
#endif
#ifdef VECTOR2_SOLVER
	vN = Vector(
			gsl_vector_get(s->x, 0),
			gsl_vector_get(s->x, 1),
			gsl_vector_get(s->x, 2)).getUnitVector();
	Vector vX1 = Vector(
			gsl_vector_get(s->x, 3),
			gsl_vector_get(s->x, 4),
			gsl_vector_get(s->x, 5));
	fD = -(vN & vX1);
#endif
#ifdef ANGLE2_SOLVER
	double theta = gsl_vector_get(s->x, 0);
	double pi = gsl_vector_get(s->x, 1);
	vN = Vector(sin(pi)*cos(theta), sin(pi)*sin(theta), cos(pi));
	Vector vX1 = Vector(
			gsl_vector_get(s->x, 2),
			gsl_vector_get(s->x, 3),
			gsl_vector_get(s->x, 4));
	fD = -(vN & vX1);
#endif

#if 0
	for (double  pp = 0; pp < M_PI; pp += 0.1)
	{
		for (double tt = 0; tt < M_PI; tt += 0.1)
		{
			Vector vNN = Vector(sin(pp)*cos(tt), sin(pp)*sin(tt), cos(pp));
			printf("vN = ");
			vNN.Print();
			double sum = 0;
			for (size_t i = 0; i < data.size(); i++)
			{
				sum += pow(data[i].GetF(tt, pp, fD), 2);
			}
			printf(" , sum = %f\n", sum);
		}
	}
#endif


#if 0
	double sqrsum = 0;
	for (size_t i = 0; i < data.size(); i++)
	{
#ifdef VECTOR_SOLVER
		sqrsum += pow(data[i].GetF(vN, fD), 2);
#endif
#ifdef ANGLE_SOLVER
		sqrsum += pow(data[i].GetF(theta, pi, fD), 2);
#endif
#ifdef VECTOR2_SOLVER
		sqrsum += pow(data[i].GetF(vN, fD), 2);
#endif
//		printf ("%f/", sqrsum);
	}
	vN.Print();
	printf(" %f %f\n", fD, sqrt(sqrsum));	
#endif





//	for (size_t i = 0; i < data.size(); i++)
//		printf ("F_%d = %f\n", i, data[i].GetF(vN));


	gsl_vector_free(gradt);
	gsl_multifit_fdfsolver_free(s);

	vP.clear();
	for (size_t i = 0; i < data.size(); i++)
	{
		vP.push_back(data[i].GetP(vN));
	}

	return vN;
}
/*
Vector PlaneDetector::getPlaneOffset()
{
	// plane offset fitting
	const gsl_multifit_fdfsolver_type *T;
	gsl_multifit_fdfsolver *s;

	int n = (int)vP.size() - 1;
	int p = 3;

	gsl_multifit_function_fdf f;
	f.f = &pof_func_f;
	f.df = &pof_func_df;
	f.fdf = &pof_func_fdf;
	f.n = n;
	f.p = p;
	f.params = &vP;

	double x_init[3] = {0.0, 0.0, 0.0};
	gsl_vector_view x = gsl_vector_view_array(x_init, 3);

	T = gsl_multifit_fdfsolver_lmsder;
	s = gsl_multifit_fdfsolver_alloc(T, n, p);
	gsl_multifit_fdfsolver_set(s, &f, &x.vector);

	gsl_vector *gradt = gsl_vector_alloc(p);

	int iter = 0;
	int status;
	do
	{
		iter ++;
		status = gsl_multifit_fdfsolver_iterate(s);
		if (status) break;
		gsl_multifit_gradient(s->J, s->f, gradt);
		status = gsl_multifit_test_gradient(gradt, 1e-6);
	} while (status == GSL_CONTINUE && iter < 50000);

	vP0 = Vector(
			gsl_vector_get(s->x, 0),
			gsl_vector_get(s->x, 1),
			gsl_vector_get(s->x, 2));

	gsl_vector_free(gradt);
	gsl_multifit_fdfsolver_free(s);

	return vP0;
}
*/
Vector PlaneDetector::GetReflectedPoint(Vector vPoint)
{
	/* this part is as same as GetReflectedPoint function in Plane class*/
//	Vector propNormal = vN * ((vP0 -  vPoint) & vN);		
////	Vector propNormal = vN * ((vP[1] -  vPoint) & vN);			
	double dist = -(fD + (vPoint&vN));
	Vector vRef = vPoint + (vN * (2 * dist));
	return vRef;

//	return vPoint + (propNormal * 2);
}

double PlaneDetector::GetAngleError(Plane &realPlane)
{	
	double ret = acos(realPlane.vNormal & vN) * 180 / M_PI;
	fprintf(fpA, "%f\t", ret);
	return ret;
}

double PlaneDetector::GetDistanceError(HeardList &heardList, Plane &realPlane)
{
	int nValidBeacon = 0;
	double distErrorSum = 0;

	for (size_t i = 0; i < heardList.heardInfo.size(); i++)
	{
		Vector vBeacon = heardList.heardInfo[i].location;
		Vector vRealRefBeacon = realPlane.GetReflectedPoint(vBeacon);
		Vector vEstRefBeacon = GetReflectedPoint(vBeacon);

		nValidBeacon++;
		double distError = vRealRefBeacon.getDistance(vEstRefBeacon);
		distErrorSum += distError;
	}

	double ret = distErrorSum / nValidBeacon;
	fprintf(fpA, "%f\n", ret);
	return ret;
}


void PlaneDetector::GetPlane(HeardList &heardList, Vector vListener)
{
	fprintf(fpA, "%f\t%f\t%f\t", vListener.x, vListener.y, vListener.z);
	fprintf(fpD, "%f\t%f\t%f\t", vListener.x, vListener.y, vListener.z);
	getPlaneNormal(heardList, vListener);
/*
	getPlaneOffset();
	for (size_t i = 1; i < vP.size(); i++)
	{
//		vP[i].Print();
		vP[i] = vP[i] + (vN * ((vP0 - vP[i]) & vN));
//		printf(" -> ");
//		vP[i].Println();
	}
*/
}

void PlaneDetector::Fail()
{
	fprintf(fpA, "-1 -1\n");
}

void PlaneDetector::Fail(Vector vListener)
{
	fprintf(fpA, "%f\t%f\t%f\t", vListener.x, vListener.y, vListener.z);
	fprintf(fpA, "-1 -1\n");
}