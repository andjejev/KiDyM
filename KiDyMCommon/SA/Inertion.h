//---------------------------------------------------------------------------

#ifndef InertionH
#define InertionH
//---------------------------------------------------------------------------
#include "CA.h"

class InertElem{
 public:
  InertElem *Sled;
  char *Clas,*Typ,*Numb,*V,*xo,*yo,*zo,*JyoOzo,*JzoOxo,*Jxoyo,
   *JYoOZo,*JZoOXo,*JXoOYo,*Prim;
  InertElem(char *S,Magazine **Mc,Magazine **Mt);
  long Save(FILE *F);
 ~InertElem();
};

class TInertBD{
 public:
  InertElem *R; char File[256]; Magazine *MC,*MT; bool CHANGEBD;
  TInertBD(){ R=NULL; MC=MT=NULL; CHANGEBD=false; };
  void Load();
  void Edit();
  void Save();
  InertElem *Find(char *N);
 ~TInertBD();
};

class TPunkt{
 public:
  Form Fr[3]; double r[3];
};

class TTetraedr{
 public:
  TPunkt P[4]; Form FM; double m;
  void Fill();
  void TransCord(Parm *P);
};

#endif

