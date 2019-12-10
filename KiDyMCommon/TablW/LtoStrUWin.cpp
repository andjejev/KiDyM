#include <vcl.h>
#pragma hdrstop

#include <string.h>
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>

//---------------------------------------------------------------------------

#pragma package(smart_init)

char *MlrdU (unsigned long Val, char *S) { strcat(S," мiль€рд");
 if(Val%10==1&&Val%100!=11) return S;
 switch((int)(Val%10)) {
  case 2: case 3: case 4:
   if(Val%100<12||Val%100>14) { strcat(S,"а"); break; }
  default: strcat(S,"iв");
 } return S;
}

char *MlnU (unsigned long Val, char *S) { strcat(S," мiльйон");
 if(Val%10==1&&Val%100!=11) return S;
 switch((int)(Val%10)) {
  case 2: case 3: case 4:
   if(Val%100<12||Val%100>14) { strcat(S,"а"); break; }
  default: strcat(S,"iв");
 } return S;
}

char *TysU (unsigned long Val, char *S) { strcat(S," тис€ч");
 if(Val%10>4||!(Val%10)) return S;
 if(Val%100>10&&Val%100<20) return S;
 if(Val%10==1) { strcat(S,"а"); return S; }
 strcat(S,"i"); return S;
}

char *LtoStrU (int Pol, unsigned long Val, char *S) { char s[256];
// if(Val==0L) { strcat(S," нуль"); return S; }
 if(Val>=1000000000l) {
  LtoStrU(2,Val/1000000000l,S); MlrdU(Val/1000000000l,S);
  return LtoStrU(Pol,Val%1000000000l,S);
 }
 if(Val>=1000000l) {
  LtoStrU(2,Val/1000000l,S); MlnU(Val/1000000l,S);
  return LtoStrU(Pol,Val%1000000l,S);
 }
 if(Val>=1000) {
  LtoStrU(0,Val/1000,S); TysU(Val/1000,S); return LtoStrU(Pol,Val%1000,S);
 }
 if(Val>=100) {
  switch((int)(Val/100)) {
   case 1: strcat(S," сто");       break;
   case 2: strcat(S," двiстi");    break;
   case 3: strcat(S," триста");    break;
   case 4: strcat(S," чотириста"); break;
   case 5: strcat(S," п'€тсот");   break;
   case 6: strcat(S," шiстсот");  break;
   case 7: strcat(S," сiмсот");   break;
   case 8: strcat(S," вiсiмсот"); break;
   case 9: strcat(S," дев'€тсот");
  } return LtoStrU(Pol,Val%100,S);
 }
 if(Val>=20) {
  switch((int)(Val/10)) {
   case 2: strcat(S," двадц€ть");    break;
   case 3: strcat(S," тридц€ть");    break;
   case 4: strcat(S," сорок");       break;
   case 5: strcat(S," п'€тдес€т");   break;
   case 6: strcat(S," шiстдес€т");   break;
   case 7: strcat(S," сiмдес€т");    break;
   case 8: strcat(S," вiсiмдес€т");  break;
   case 9: strcat(S," дев'€носто");
  } return LtoStrU(Pol,Val%10,S);
 }
 switch((int)Val) {
//  case  0: strcat(S," нуль"); break;
  case  1: strcat(S,Pol==2 ? " один" : Pol==1 ? " одно" : " одна"); break;
  case  2: strcat(S,Pol ? " два" : " двi");                         break;
  case  3: strcat(S," три");                                        break;
  case  4: strcat(S," чотири");                                     break;
  case  5: strcat(S," п'€ть");                                      break;
  case  6: strcat(S," шiсть");                                      break;
  case  7: strcat(S," сiм");                                        break;
  case  8: strcat(S," вiсiм");                                      break;
  case  9: strcat(S," дев'€ть");                                    break;
  case 10: strcat(S," дес€ть");                                     break;
  case 11: strcat(S," одинадц€ть");                                 break;
  case 12: strcat(S," дванадц€ть");                                 break;
  case 13: strcat(S," тринадц€ть");                                 break;
  case 14: strcat(S," чотирнадц€ть");                               break;
  case 15: strcat(S," п€тнадц€ть");                                 break;
  case 16: strcat(S," шiстнадц€ть");                                break;
  case 17: strcat(S," сiмнадц€ть");                                 break;
  case 18: strcat(S," вiсiмнадц€ть");                               break;
  case 19: strcat(S," дев'€тнадц€ть");
 } return S;
}
