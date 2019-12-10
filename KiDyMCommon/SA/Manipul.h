//---------------------------------------------------------------------------

#ifndef ManipulH
#define ManipulH
//---------------------------------------------------------------------------
#include "CA.h"

void TakeCoSinFi(Parm *P);
void MultTransMatrVect(Form M[3][3],Form V[3]);
void MultMatrVect(Form M[4][4],Form V[4]);
void MultMatrVect(Form M[3][3],Form V[3]);
void AddVect(Form A[3],Form B[3],Form C[3]);
void MultMatrMatr(Form A[3][3],Form B[3][3],Form C[3][3]);
void MultMatrMatr(Form A[3][3],Form B[3][3]);
void MatrMultMatr(Form A[4][4],Form B[4][4]);
void PrintFormVect(wchar_t *File,wchar_t *Comm,wchar_t *NameVect,Form V[4]);
void PrintFormMatr(wchar_t *File,wchar_t *Comm,wchar_t *NameMatr,Form M[4][4]);
int Jacobi(int N,double **A,double **V,double *D,int Increase);
#endif
