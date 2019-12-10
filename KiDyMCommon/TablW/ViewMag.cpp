//---------------------------------------------------------------------------
#pragma hdrstop

#include "ViewMag.h"
#include "EditList.h"
#include "ConvDW.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)

extern TEditList *EditList;

void ViewMagazine(Magazine *M,char *Zag,char *Shapka,char *Help){
//Просмотр магазина
 char *FileSh="shviewmag",*Format="%-s",*Fprn="viewmag.prn",
  **Head=(char **)calloc(1,SzV);
 int i,*MNN=(int *)calloc(2,SzI); FILE *F; MNN[0]=1;
 if(F=fopen("shviewmag","w")){ Fputs(Shapka,F); fputc('\n',F); fclose(F); }
 Head[0]=(char *)calloc(256,SzV); strcpy(Head[0],"Просмотр");
 EditList=new TEditList((void **)&M,MNN,false,NULL,false,NULL,Format,FileSh,
  Head,Fprn,NULL,NULL,NULL,0,-1,-1,-1,-1,Zag,NULL,1,Help,NULL,NULL);
 if(EditList->Diagnos()){
  Application->CreateForm(__classid(TFormEditList),&FormEditList);
  FormEditList->ShowModal();
 }
 if(Head){ if(Head[0]) free(Head[0]); free(Head); }
 if(MNN) free(MNN); if(Help) free(Help); delete EditList;
}
//---------------------------------------------------------------------------
