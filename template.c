#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

struct pixel
{
    unsigned int R,G,B;
};
struct detectie           //detectiile obtinute le memorez intr-un vector de tip struct, care retine valoarea corelatiei pentru detectia respectiva,
{
    double corelatie;      //coordonatele i si j ale coltului din stanga al ferestrei si valorile pixelilor
    unsigned int i,j,R,G,B;
};
void grayscale_image(char* nume_fisier_sursa,char* nume_fisier_destinatie,unsigned int*latime_img,unsigned int*inaltime_img) //transforma o imagine color in grayscale
{
   FILE *fin, *fout;
   unsigned int dim_img;
   unsigned char pRGB[3], aux;

   printf("nume_fisier_sursa = %s \n",nume_fisier_sursa);

   fin = fopen(nume_fisier_sursa, "rb");
   if(fin == NULL)
   	{
   		printf("nu am gasit imaginea sursa din care citesc");
   		return;
   	}

   fout = fopen(nume_fisier_destinatie, "wb+");

   fseek(fin, 2, SEEK_SET);
   fread(&dim_img, sizeof(unsigned int), 1, fin);
   printf("Dimensiunea imaginii in octeti: %u\n", dim_img);

   fseek(fin, 18, SEEK_SET);
   fread(latime_img, sizeof(unsigned int), 1, fin);
   fread(inaltime_img, sizeof(unsigned int), 1, fin);
   printf("Dimensiunea imaginii in pixeli (latime x inaltime): %u x %u\n",*latime_img, *inaltime_img);

   //copiaza octet cu octet imaginea initiala in cea noua
	fseek(fin,0,SEEK_SET);
	unsigned char c;
	while(fread(&c,1,1,fin)==1)
	{
		fwrite(&c,1,1,fout);
		fflush(fout);
	}
	fclose(fin);
	//calculam padding-ul pentru o linie
	int padding;
    if((*latime_img) % 4 != 0)
        padding = 4 - (3 * (*latime_img)) % 4;
    else
        padding = 0;

    printf("padding = %d \n",padding);

	fseek(fout, 54, SEEK_SET);
	int i,j;
	for(i = 0; i < (*inaltime_img); i++)
	{
		for(j = 0; j < (*latime_img); j++)
		{
			//citesc culorile pixelului
			fread(pRGB, 3, 1, fout);
			//fac conversia in pixel gri
			aux = 0.299*pRGB[2] + 0.587*pRGB[1] + 0.114*pRGB[0];
			pRGB[0] = pRGB[1] = pRGB[2] = aux;
        	fseek(fout, -3, SEEK_CUR);
        	fwrite(pRGB, 3, 1, fout);
        	fflush(fout);
		}
		fseek(fout,padding,SEEK_CUR);
	}
	fclose(fout);
}
void imag_init(char*imag,unsigned int latime_img,unsigned int inaltime_img,unsigned int***ma)  //functie care citeste o imagine dintr-un fisier binar
{                                                                                              //si o transpune sub forma unei matrice
    int i,j;
    FILE*fin=fopen(imag,"rb");
    (*ma)=(unsigned int**)malloc(inaltime_img*sizeof(unsigned int*));
    for(i=0;i<inaltime_img;i++)
        (*ma)[i]=(unsigned int*)malloc(latime_img*sizeof(unsigned int));
    if(ma==NULL)
        printf("Nu s-a putut aloca memorie");
    if(fin==NULL)
        printf("Nu s-a putut deschide fisierul");
    unsigned char c;
    fseek(fin, 54, SEEK_SET);
    for(i=0;i<inaltime_img;i++)
        for(j=0;j<latime_img;j++)
    {
        fread(&c,1,1,fin);                    //imaginea fiind grayscale, este suficient ca matricea sa retine valorea R a pixelului, caci cele 3 valori sunt egale pentru grayscale
        (*ma)[i][j]=c;
        fseek(fin,2,SEEK_CUR);                 //sarim cu fseek peste celelalte doua valori G,B
    }
    fclose(fin);
}
void imag_init_color(char*imag,unsigned int latime_img,unsigned int inaltime_img,struct pixel***ma)     //functie care citeste o imagine color dintr-un fisier binar
{                                                                                                       //si o memoreaza sub forma unei matrice
    int i,j;
    FILE*fin=fopen(imag,"rb");
    (*ma)=(struct pixel**)malloc(inaltime_img*sizeof(struct pixel*));
    for(i=0;i<inaltime_img;i++)
        (*ma)[i]=(struct pixel*)malloc(latime_img*sizeof(struct pixel));
    if(ma==NULL)
        printf("Nu s-a putut aloca memorie");
    if(fin==NULL)
        printf("Nu s-a putut deschide fisierul");
    unsigned char c;
    fseek(fin, 54, SEEK_SET);
    for(i=0;i<inaltime_img;i++)
        for(j=0;j<latime_img;j++)
    {
        fread(&c,1,1,fin);          //matricea este de tip struct pixel si retine toate cele 3 valori RGB
        (*ma)[i][j].B=c;
        fread(&c,1,1,fin);
        (*ma)[i][j].G=c;
        fread(&c,1,1,fin);
        (*ma)[i][j].R=c;
    }
    fclose(fin);
}
void fereastra(unsigned int latime_img,unsigned int inaltime_img,unsigned int latime_sablon,unsigned int inaltime_sablon,int i,int j,unsigned int**imagine_i,unsigned int***ma)
{
    (*ma)=(unsigned int**)malloc(inaltime_sablon*sizeof(unsigned int*));
    if(ma==NULL)
        printf("Nu s-a putut aloca");
    int a,b;
    for(a=0;a<inaltime_sablon;a++)
    {
        (*ma)[a]=(unsigned int*)malloc(latime_sablon*sizeof(unsigned int));
    }
    for(a=0;a<inaltime_sablon;a++)                       //functie care primeste doua coordonate i si j si extrage fereastra cu coltul din stanga sus
        for(b=0;b<latime_sablon;b++)                     //in acele coordonate
    {
        (*ma)[a][b]=imagine_i[a+i][b+j];
    }
}
double medie(unsigned int**sablon,unsigned int latime_sablon,unsigned int inaltime_sablon,unsigned int n) //functie care calculeaza media aritmetica a
{                                                                                                        //pixelilor din matricea sablon
    unsigned int suma_pix=0,i,j;
    double medie_pix;
    for(i=0;i<inaltime_sablon;i++)
        {
            for(j=0;j<latime_sablon;j++);
               suma_pix+=sablon[i][j];
        }
    medie_pix=((double)(suma_pix))/((double)(n));
    return medie_pix;
}
double deviatie(unsigned int**sablon,unsigned int latime_sablon,unsigned int inaltime_sablon,unsigned int n,double medie)
{
    double S=0;                              //functie care calculeaza deviatia standard a valorilor intensitatilor pixelilor din matricea sablon
    int i,j;
    double deviatie;
    for(i=0;i<inaltime_sablon;i++)
       {
           for(j=0;j<latime_sablon;j++)
              S=S+((sablon[i][j]-medie)*(sablon[i][j]-medie));
       }
    deviatie=sqrt((((double)(1))/((double)(n-1)))*S);
    return deviatie;
}
double calcul_corelatie(unsigned int**sablon,unsigned int**fer,unsigned int latime_sablon,unsigned int inaltime_sablon,unsigned int n,double medie_pix_sablon,double medie_pix_fereastra,double deviatie_sablon,double deviatie_fereastra)
{
    int i,j;
    double corelatie,S=0,raport;
    raport=(((double)(1))/((double)(deviatie_sablon*deviatie_fereastra)));            //functie care calculeaza corelatia dintre sablonul curent si fereastra
    for(i=0;i<inaltime_sablon;i++)                                                    //extrasa
       {
           for(j=0;j<latime_sablon;j++)
              S=S+((fer[i][j]-medie_pix_fereastra)*(sablon[i][j]-medie_pix_sablon));
       }
    S=S*raport;
    corelatie=(((double)(1))/((double)(n)))*S;
    return corelatie;
}
double corelatie(unsigned int**sablon,unsigned int**ma,unsigned int latime_sablon,unsigned int inaltime_sablon)
{
    double medie_pix_sablon,medie_pix_fereastra,deviatie_sablon,deviatie_fereastra;
    unsigned int n=latime_sablon*inaltime_sablon;
    medie_pix_sablon=medie(sablon,latime_sablon,inaltime_sablon,n);                    //calculez mediile pentru sablonul curent si fereastra, deviatiile si apoi corelatia
    deviatie_sablon=deviatie(sablon,latime_sablon,inaltime_sablon,n,medie_pix_sablon);
    medie_pix_fereastra=medie(ma,latime_sablon,inaltime_sablon,n);
    deviatie_fereastra=deviatie(ma,latime_sablon,inaltime_sablon,n,medie_pix_fereastra);
    double corelatie1;
    corelatie1=calcul_corelatie(sablon,ma,latime_sablon,inaltime_sablon,n,medie_pix_sablon,medie_pix_fereastra,deviatie_sablon,deviatie_fereastra);
    return corelatie1;
}
void colorare_detectie(struct pixel***imag,struct pixel C,unsigned int i,unsigned int j,unsigned int latime_sablon,unsigned int inaltime_sablon)
{
    unsigned int a;
    for(a=i;a<i+inaltime_sablon;a++)
    {
        (*imag)[a][j].B=C.B;            //atunci cand este gasita o detectie, este colorata
        (*imag)[a][j].G=C.G;            //acest lucru se realizeaza prin inlocuirea valorilor pixelilor din matricea obtinuta din imaginea initiala
        (*imag)[a][j].R=C.R;            //cu valorile corespunzatoare culorii dorite, trasnmise prin variabila C de tip struct pixel
        (*imag)[a][j+latime_sablon-1].B=C.B;     //in acest for se coloreaza prima si ultima coloana
        (*imag)[a][j+latime_sablon-1].G=C.G;
        (*imag)[a][j+latime_sablon-1].R=C.R;
    }
    for(a=j;a<j+latime_sablon;a++)
    {
        (*imag)[i][a].B=C.B;         //in acest for se coloreaza prima si ultima linie
        (*imag)[i][a].G=C.G;
        (*imag)[i][a].R=C.R;
        (*imag)[i+inaltime_sablon-1][a].B=C.B;
        (*imag)[i+inaltime_sablon-1][a].G=C.G;
        (*imag)[i+inaltime_sablon-1][a].R=C.R;
    }

}
void detectii(char*imag,char*im_sablon,unsigned int latime_img,unsigned int inaltime_img,unsigned int latime_sablon,unsigned int inaltime_sablon,unsigned int **imagine_i,double prag,struct pixel***imag_color,struct detectie**d,struct pixel C,unsigned int*k)
{
    unsigned int**sablon=NULL,**ma=NULL;
    imag_init(im_sablon,latime_sablon,inaltime_sablon,&sablon);
    unsigned int a,b;
    for(a=0;a<inaltime_img;a++)
        for(b=0;b<latime_img;b++)       //se parcurge matricea
    {
        if(a+inaltime_sablon<=inaltime_img && b+latime_sablon<=latime_img)
           {
        fereastra(latime_img,inaltime_img,latime_sablon,inaltime_sablon,a,b,imagine_i,&ma);      //extragem pe rand ferestre din imagine
        double corel=corelatie(sablon,ma,latime_sablon,inaltime_sablon);  //calculam corelatia pentru fereastra respectiva
        if(corel>prag)   //daca corelatia este mai mare decat pragul dat atunci este colorata detectia si se memoreaza in vectorul de tip struct detectie
    {
        colorare_detectie(imag_color,C,a,b,latime_sablon,inaltime_sablon);
        (*d)[*k].i=a;   //acesta retine coordonatele coltului din stanga sus ale detectiei, valorile culorilor si corelatia
        (*d)[*k].j=b;
        (*d)[*k].R=C.R;
        (*d)[*k].G=C.G;
        (*d)[*k].B=C.B;
        (*d)[*k].corelatie=corel;
        (*k)++;
    }
           }
    }
    free(ma);
    free(sablon);
}
void afis(char*img_i,char*img,struct pixel**ma,unsigned int inaltime_img,unsigned int latime_img) //functia care scrie dintr-o matrice intr-o imagine dintr-un fisier binar
{
    FILE*fin=fopen(img_i,"rb");
    FILE*fout=fopen(img,"wb");
    if(fin==NULL)
        printf("Nu s-a putut deschide fisierul");
    if(fout==NULL)
        printf("Nu s-a putut deschide fisierul");
    int i,j;
    unsigned char c;
    for(i=0;i<54;i++)      //copiaza headerul
    {
        fread(&c,1,1,fin);
		fwrite(&c,1,1,fout);
		fflush(fout);
    }
    for(i=0;i<inaltime_img;i++)           //copiaza imaginea, cu ordinea pixelilor B,G,R
        for(j=0;j<latime_img;j++)
    {
        fwrite(&ma[i][j].B,1,1,fout);
        fwrite(&ma[i][j].G,1,1,fout);
        fwrite(&ma[i][j].R,1,1,fout);
    }
    fclose(fout);
    fclose(fin);
}
void culori(struct pixel**C)     //intr-un vector C de tip struct pixel memorez valorile culorilor folosite la colorarea ferestrelor
{
    (*C)=(struct pixel*)malloc(10*sizeof(struct pixel));
    (*C)[0].R=255;
    (*C)[0].G=0;
    (*C)[0].B=0;
    (*C)[1].R=255;
    (*C)[1].G=255;
    (*C)[1].B=0;
    (*C)[2].R=0;
    (*C)[2].G=255;
    (*C)[2].B=0;
    (*C)[3].R=0;
    (*C)[3].G=255;
    (*C)[3].B=255;
    (*C)[4].R=255;
    (*C)[4].G=0;
    (*C)[4].B=255;
    (*C)[5].R=0;
    (*C)[5].G=0;
    (*C)[5].B=255;
    (*C)[6].R=192;
    (*C)[6].G=192;
    (*C)[6].B=192;
    (*C)[7].R=255;
    (*C)[7].G=140;
    (*C)[7].B=0;
    (*C)[8].R=128;
    (*C)[8].G=0;
    (*C)[8].B=128;
    (*C)[9].R=128;
    (*C)[9].G=0;
    (*C)[9].B=0;
}
void template_matching(char*fisier,struct detectie**d,unsigned int*k)
{
    unsigned int**imagine_i=NULL,latime_img,inaltime_img,inaltime_sablon,latime_sablon;
    struct pixel**imagine_i_color=NULL,*C=NULL;
    FILE*fin=fopen(fisier,"r");
    char*test,*test_g,*cifra,*cifra_g,*im;
    test=malloc(15*sizeof(char));
    test_g=malloc(15*sizeof(char));
    cifra=malloc(15*sizeof(char));
    cifra_g=malloc(15*sizeof(char));
    im=malloc(15*sizeof(char));
    if(fin==NULL)
        printf("Nu s-a putut deschide fisierul");
    culori(&C);
    fgets(test,15,fin);
    int n=strlen(test);             //citeste din fisier numele fisierelor binare si text care vor fi folosite pentru scrierea imaginilor sau citirea lor
    strcpy(test+n-1,test+n);
    fgets(test_g,15,fin);     //citeste linie cu linie din fisier
    n=strlen(test_g);
    strcpy(test_g+n-1,test_g+n);   //sterge \n de la finalului sirului
    grayscale_image(test,test_g,&latime_img,&inaltime_img);
    fgets(im,15,fin);
    n=strlen(im);
    strcpy(im+n-1,im+n);
    imag_init(test_g,latime_img,inaltime_img,&imagine_i);
    imag_init_color(test,latime_img,inaltime_img,&imagine_i_color);
    (*d)=(struct detectie*)malloc(30000*sizeof(struct detectie));
    if(d==NULL)
        printf("Nu s-a putut aloca");
    int i;
    for(i=0;i<=9;i++)  //fiecare sablon cu cifrele de la 0 la 9 se transforma in grayscale, dupa care se apeleaza functia detectii cu pragul de 0.5
    {
        fgets(cifra,15,fin);
        n=strlen(cifra);
        strcpy(cifra+n-1,cifra+n);
        fgets(cifra_g,15,fin);
        n=strlen(cifra_g);
        strcpy(cifra_g+n-1,cifra_g+n);
        grayscale_image(cifra,cifra_g,&latime_sablon,&inaltime_sablon);
        detectii(test_g,cifra_g,latime_img,inaltime_img,latime_sablon,inaltime_sablon,imagine_i,0.5,&imagine_i_color,d,C[i],k); //c[i] reprezinta culoarea data, cu care se va colora fereastra detectata
    }
    afis(test,im,imagine_i_color,inaltime_img,latime_img);       //se afiseaza imaginea cu detectiile
    free(imagine_i);
    free(imagine_i_color);
    free(C);
    fclose(fin);
}
int cmp(const void*a,const void*b)
{
    struct detectie*pa=(struct detectie*)a;     //functia cmp folosita la quicksort, pentru a sorta descrescator valorile corelatiilor din vectorul d de detectii
    struct detectie*pb=(struct detectie*)b;
    if(pa->corelatie<pb->corelatie)
        return 1;
    if(pa->corelatie>pb->corelatie)
        return -1;
    return 0;
}
int min(int x,int y)   //calculeaza minimul dintre 2 nr
{
    if(x<y) return x;
    else return y;
}
int max(int x,int y) //calculeaza maximul dintre 2 nr
{
    if(x>y) return x;
    else return y;
}
double overlapping(unsigned int s_i1,unsigned int s_i2,unsigned int s_j1,unsigned int s_j2,unsigned int latime_sablon,unsigned int inaltime_sablon)
{
    unsigned d_i1,d_j1,d_i2,d_j2;
    d_i1=s_i1+inaltime_sablon-1;             //functia primeste coordonatele coltului din stanga sus ale celor 2 puncte
    d_j1=s_j1+latime_sablon-1;              //calculeaza coordonatele coltului din dreapta jos pentru cele 2 puncte
    d_i2=s_i2+inaltime_sablon-1;
    d_j2=s_j2+latime_sablon-1;
    int aria1=abs(s_i1-d_i1)*abs(s_j1-d_j1);          //calculeaza aria celor 2 ferestre
    int aria2=abs(s_i2-d_i2)*abs(s_j2-d_j2);
    int aria_inter=(min(d_i1,d_i2)-max(s_i1,s_i2))*(min(d_j1,d_j2)-max(s_j1,s_j2));  //formula pentru aria intersectiei
    double overlap=(double)(aria_inter)/(double)(aria1+aria2-aria_inter);    //calculeaza suprapunerea spatiala a doua ferestre
    return overlap;
}
void eliminare(char*img,char*im2,struct detectie*d,unsigned int k,unsigned int latime_img,unsigned int inaltime_img,unsigned int latime_sablon,unsigned int inaltime_sablon)
{
    struct pixel**ma=NULL;      //eliminarea non-maximelor
    unsigned int a,b;
    imag_init_color(img,latime_img,inaltime_img,&ma);     //memoreaza imaginea sub forma unei matrici
    for(a=0;a<k-1;a++)
      {
        for(b=a+1;b<k;b++)
    {
        if(overlapping(d[a].i,d[b].i,d[a].j,d[b].j,latime_sablon,inaltime_sablon)>0.2)    //dupa ordonarea descrescatoare, calculeaza suprapunerea spatiala pentru detectii
            {
                struct pixel C;     //daca se suprapun, o pastreaza pe cea cu corelatia mai mare, adica d[a]
                C.R=d[a].R;
                C.G=d[a].G;
                C.B=d[a].B;
                colorare_detectie(&ma,C,d[a].i,d[a].j,latime_sablon,inaltime_sablon);   //se salveaza detectiile ramase
            }
      }
      }
    afis(img,im2,ma,inaltime_img,latime_img); //se afiseaza noua imagine cu detectii
}
