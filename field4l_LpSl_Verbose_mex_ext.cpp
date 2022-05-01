// AUTHOR:  Filipe M. Ferreira (filipef@ieee.org)
//mex -lgsl -lgslcblas field4l_LpSl_Verbose_mex_ext.cpp fieldLP.cpp dispersionLP.cpp DmatrixLp.cpp SmatrixLP.cpp FO_dmd.cpp gen_deltan_prof.cpp gen_rho.cpp modeSolverLP.cpp refIndProfile.cpp sellmeierEquation.cpp zeroFinderF.cpp zeroFinderGE.cpp zeroFinderMode.cpp
#include "matrix.h"
#include "mex.h"
#include "matrix.h"
#include <iostream>
#include <stdio.h>
#include <math.h>
#include <cmath>
#include <time.h>
#include "Header.h"
#include <gsl/gsl_sf_bessel.h>
using namespace std;

#define maxLwArb 10
#define maxLrho 20000

void DmatrixLP(double epsi,double r,double n,int v,double k0,double u, double* D, double* invD);
double SmatrixLP(double *eVector, double *rVector, double *nVector, int v, double k0, int nopN, int nopR, double *S);
double zeroFinderF(double w0,double nCl,double *x0,int n);
double zeroFinderGE(double w0,double nCo,double *x0,int n);
void gen_deltan_prof(double w0,int profType,pPar profPar,double *rho,double *deltan,int lRho);
void refIndProfile(double w0,double *wArb,int lw,double *rho,double *deltan,int lRho,double n[maxLrho][maxLwArb],double *xGe,double *xF);
void sellmeierEquation(double *w,int wl,double xGe,double xF,double *n);
void gen_rho(int profType, pPar profPar, double incMax,double *rho,int &lRho);
void modeSolverLP(int profType,pPar profPar,double dr,double lambda,modes *modesLP,int &tnom);
double zeroFinderMode(double *eVector, double *rVector, int v, double k0, int nopR, double *S,double *x0,int n);
void FO_dmd(int profType,pPar profPar,double dr,double *lArb,int lArbL,double dl,double &FO,int &tnom,modes *modesLP,double dmd[20][10]);
void dispersionLP(int profType,pPar profPar,double dr,double lambda,double dl,double *ng,double *vg,double *dmd,int &tnom,modes *modesLP1);
void fieldLP(int profType, pPar profPar, double dr, double lambda, int V, int U, double pc, double rho[maxLrho], double nRho[maxLrho], double Ex[maxLrho], int &lRho);

void mexFunction(int nlhs, mxArray *plhs[],int nrhs, const mxArray *prhs[]) /* Input variables */
{
    //nhls - Number of output variables
    //nrhs - Number of input variables (incMax,w1,w2,w3,sI1,sI2,sI3,a1,a2,a3,w0,radius,lambda)
    //plhs - Array of mxArray pointers to the output variables
    //prhs - Array of mxArray pointers to the input variables
    
    double incMax,w1,w2,w3,sI1,sI2,sI3,eI1,a1,a2,a3,w0,radius,lambda,dl,dw;
    double e0,u0,c0,nCo,nCl,eCo,eCl,k0;
    double pi = 3.141592653589793;
    double w;
    modes modesLP[100];
    int nopN=40,k1,tnom;
    double *pointerEx, *pointerEz, *pointerRho, *pointerRhoL, *pointerNrho;
    double ng[100],vg[100],dmd[100],pc,nRho[maxLrho],rho[maxLrho],Ex[maxLrho],Ez[maxLrho];
    int V,U,rhoL;
    
    incMax = mxGetScalar(prhs[0]);
    w1 = mxGetScalar(prhs[1]);
    w2 = mxGetScalar(prhs[2]);
    w3 = mxGetScalar(prhs[3]);
    sI1 = mxGetScalar(prhs[4]);
    sI2 = mxGetScalar(prhs[5]);
    sI3 = mxGetScalar(prhs[6]);
    eI1 = mxGetScalar(prhs[7]);
    a1 = mxGetScalar(prhs[8]);
    a2 = mxGetScalar(prhs[9]);
    a3 = mxGetScalar(prhs[10]);
    w0 = mxGetScalar(prhs[11]);
    radius = mxGetScalar(prhs[12]);
    lambda = mxGetScalar(prhs[13]);
    V = mxGetScalar(prhs[14]);
    U = mxGetScalar(prhs[15]);
    pc = mxGetScalar(prhs[16]);
       
	// Physical constans
	e0      = 8.854187817e-12;
	u0      = 1.25663706e-6;
	c0      = 1/sqrt(e0*u0);

	// Source parameters
	w       = 2*pi*c0/lambda;

    // Profile Parameters
    pPar profPar;
    profPar.w1  = w1; profPar.w2  = w2; profPar.w3 = w3;
    profPar.sI1 = sI1; profPar.sI2 = sI2; profPar.sI3 = sI3;
    profPar.eI1 = eI1;
    profPar.a1  = a1; profPar.a2  = a2; profPar.a3 = a3;
    profPar.radius = radius;
    profPar.w0 = w0;

    // Field
    fieldLP(1,profPar,incMax,lambda,V,U,pc,rho,nRho,Ex,rhoL);

    plhs[0] = mxCreateNumericMatrix(10000, 1, mxDOUBLE_CLASS, mxREAL);
    plhs[1] = mxCreateNumericMatrix(10000, 1, mxDOUBLE_CLASS, mxREAL);
    plhs[2] = mxCreateNumericMatrix(10000, 1, mxDOUBLE_CLASS, mxREAL);
    plhs[3] = mxCreateNumericMatrix(1, 1, mxDOUBLE_CLASS, mxREAL);

    pointerEx = mxGetPr(plhs[0]);
    pointerRho = mxGetPr(plhs[1]);
    pointerNrho = mxGetPr(plhs[2]);
    pointerRhoL = mxGetPr(plhs[3]);
    
    
    for(k1=0;k1<rhoL;k1++){
        pointerEx[k1] = Ex[k1];
        pointerNrho[k1] = nRho[k1];
        pointerRho[k1] = rho[k1];
        
    }
    *pointerRhoL = rhoL;

    return;
}
