#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "criptare.c"
#include "template.c"

int main()
{
    struct pixel1*C1=NULL,*D=NULL,*L=NULL;
   	unsigned int W,H;
   	image_size("peppers.bmp",&H,&W);          //extrage dimensiunile
    imagine_interna("peppers.bmp",H,W,&L);    //salveaza imaginea in forma liniarizata
    imagine_externa("peppers.bmp","peppers1.bmp",H,W,L);   //trece imaginea din forma liniarizata in memoria externa
    criptare("peppers.bmp","peppersc.bmp","secret_key.txt",W,H,L,&C1); //cripteaza imaginea
    decriptare("peppers.bmp","peppersc.bmp","secret_key.txt","peppersd.bmp",W,H,&D,C1); //decripteaza
    calc_chi("peppers.bmp",H,W); //chi patrat pentru imaginea initiala
    calc_chi("peppersc.bmp",H,W); //chi patrat pentru imaginea criptata
    free(L);
    free(C1);
    free(D);
    unsigned int k=0,inaltime_img,latime_img,inaltime_sablon,latime_sablon;
    struct detectie*d=NULL;
    image_size("test.bmp",&inaltime_img,&latime_img); //extrage dimensiunile imaginii
    image_size("cifra0.bmp",&inaltime_sablon,&latime_sablon); //extrage dimensiunile sablonului
    template_matching("date_template.txt",&d,&k);   //gaseste detectiile si le coloreaza
    qsort(d,k,sizeof(struct detectie),cmp);      //ordoneaza descrescator dupa corelatie
    eliminare("test.bmp","im2.bmp",d,k,latime_img,inaltime_img,latime_sablon,inaltime_sablon);   //eliminarea non-maximelor
    return 0;
}
