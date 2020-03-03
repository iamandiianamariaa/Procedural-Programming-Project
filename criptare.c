#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct pixel1
{
    unsigned char R,G,B;
};
void xorshift32(unsigned int R0, unsigned int **R, unsigned int H,unsigned int W)     //functie pentru generatorul de numere pseudo-aleatoare XORSHIFT32
{
    (*R)=(unsigned int *)malloc((2*W*H-1)*sizeof(unsigned int));
    if(R==NULL)
        printf("Nu s-a putut aloca");
     unsigned int r;
     unsigned int k;
     r=R0;
     for(k=1;k<=2*W*H-1;k++)
      {
          r=r^r<<13;
          r=r^r>>17;
          r=r^r<<5;
          (*R)[k]=r;
      }
}
void image_size(char*nume_fisier,unsigned int*H,unsigned int*W)
{
    FILE*fin=fopen(nume_fisier,"rb");
    if(fin == NULL)
   	{
   		printf("Nu am gasit imaginea sursa din care citesc");
   		return;
   	}
   fseek(fin, 18, SEEK_SET);
   fread(W, sizeof(unsigned int), 1, fin);          //functie care extrage latimea si inaltimea unei imagini din header, incepand cu pozitia 18
   fread(H, sizeof(unsigned int), 1, fin);
   fclose(fin);
}
void imagine_interna(char*nume_fisier,unsigned int H,unsigned int W,struct pixel1**L)
{
    (*L) = (struct pixel1*) malloc(W * H * sizeof(struct pixel1));
    if(L==NULL)
        printf("Nu s-a putut aloca");
    FILE*fin=fopen(nume_fisier,"rb");
    if(fin == NULL)
   	{
   		printf("Nu am gasit imaginea sursa din care citesc");
   		return;
   	}
   	struct pixel1**ma=NULL;
   	ma=(struct pixel1**)malloc(W*sizeof(struct pixel1*));
   	int i,j,k=0;
   	for(i=0;i<W;i++)
        ma[i]=(struct pixel1*)malloc(H*sizeof(struct pixel1));
   	fseek(fin, 54, SEEK_SET);
   	unsigned char c;
   	for(i=0;i<H;i++)
        for(j=0;j<W;j++)      //memoreaza o imagine sub forma unei matrice
        {
            fread(&c,1,1,fin);
            ma[i][j].B=c;
            fread(&c,1,1,fin);
            ma[i][j].G=c;
            fread(&c,1,1,fin);
            ma[i][j].R=c;
        }
   	for(i=H-1;i>=0;i--)        //trece imaginea din matricea in forma liniarizata
        for(j=0;j<W;j++)
    {
        (*L)[k].B=ma[i][j].B;
        (*L)[k].G=ma[i][j].G;
        (*L)[k].R=ma[i][j].R;
        k++;
    }
    free(ma);
    fclose(fin);
}
void imagine_externa(char*nume_fisier_intrare,char*nume_fisier_iesire,unsigned int H,unsigned int W,struct pixel1*L)
{
    FILE*fin=fopen(nume_fisier_intrare,"rb");
    if(fin == NULL)
   	{
   		printf("Nu am gasit imaginea sursa din care citesc");
   		return;
   	}
   	FILE*fout=fopen(nume_fisier_iesire,"wb");
    if(fout == NULL)
   	{
   		printf("Nu am gasit imaginea sursa din care citesc");
   		return;
   	}
   	int i,j;
   	unsigned char c;
   	for(i=0;i<=53;i++)       //copiaza headerul
    {
        fread(&c,1,1,fin);
        fwrite(&c,1,1,fout);
		fflush(fout);
    }
    fseek(fout,54,SEEK_SET);
   	for(i=H-1;i>=0;i--)
        for(j=0;j<W;j++)
    {
        fwrite(&L[i*W+j].B,1,1,fout);        //scrie o imagine din forma liniarizata in memoria externa
        fwrite(&L[i*W+j].G,1,1,fout);        //porneste de pe ultima linie, prima coloana
        fwrite(&L[i*W+j].R,1,1,fout);
    }
   	fclose(fout);
   	fclose(fin);
}
void Durstenfeld(unsigned int **p,unsigned int W,unsigned int H,unsigned int *R) //genereaza o permutarea aleatoare p folosind algoritmul lui Durstenfeld
{
    (*p)=(unsigned int *)malloc((W*H-1)*sizeof(unsigned int));
    if(p==NULL)
        printf("Nu s-a putut aloca");
    unsigned int k,r,aux;
    for(k=0;k<W*H;k++)
        (*p)[k]=k;
    for(k=W*H-1;k>=1;k--)
    {
        r=R[W*H-k]%(k+1);
        aux=(*p)[r];
        (*p)[r]=(*p)[k];
        (*p)[k]=aux;
    }
}
void citire_seed_R0(char*fisier,unsigned int*R0,unsigned int*seed)
{
    FILE*fin=fopen(fisier,"r");
    if(fin == NULL)
   	{
   		printf("Nu s-a putut deschide fisierul");
   	}
    fscanf(fin,"%d%d",R0,seed);      //citeste valorile seed si R0 din fisier
    fclose(fin);
}
unsigned int octet(unsigned int x,unsigned int i) //extrage octetii dintr-un numar
{
    unsigned char*p=&x;
    return p[i];
}
void permutare(struct pixel1**L_perm,unsigned int*p,struct pixel1*L,unsigned int W,unsigned int H)
{
    (*L_perm)=(struct pixel1*) malloc(W * H * sizeof(struct pixel1));
    if(L_perm==NULL)
        printf("Nu s-a putut aloca");
    int i;
    for(i=0;i<W*H;i++)
    {
        (*L_perm)[p[i]]=L[i];   //aplicata permutarea aleatoare generata asupra vectorului L, care retine imaginea in forma liniarizata
    }
}
void criptare(char*imag_init,char*imag_crip,char*fisier,unsigned int W,unsigned int H,struct pixel1*L,struct pixel1**C)
{
    unsigned int seed, R0;
    struct pixel1*L_perm=NULL;
    unsigned int*R=NULL,*p=NULL;
    citire_seed_R0(fisier,&R0,&seed);
    xorshift32(R0,&R,H,W);
    Durstenfeld(&p,W,H,R);
    permutare(&L_perm,p,L,W,H);
    (*C)=(struct pixel1*) malloc(W * H * sizeof(struct pixel1));
    if(C==NULL)
        printf("Nu s-a putut aloca");
    int i;
    for(i=0;i<W*H;i++)
    {
        if(i==0)    //asupra fiecarui pixel se aplica operatia de xorare dupa regula data
        {
            (*C)[i].B=octet(seed,0)^L_perm[i].B^octet(R[W*H],0);
            (*C)[i].G=octet(seed,1)^L_perm[i].G^octet(R[W*H],1);
            (*C)[i].R=octet(seed,2)^L_perm[i].R^octet(R[W*H],2);
        }
        else
        {
            (*C)[i].B=octet((*C)[i-1].B,0)^L_perm[i].B^octet(R[W*H+i],0);
            (*C)[i].G=octet((*C)[i-1].G,1)^L_perm[i].G^octet(R[W*H+i],1);
            (*C)[i].R=octet((*C)[i-1].R,2)^L_perm[i].R^octet(R[W*H+i],2);
        }
    }
    free(R);
    free(p);
    free(L_perm);
    imagine_externa(imag_init,imag_crip,H,W,*C);   //se scrie imaginea criptata
}
void inversa(unsigned int*p,unsigned int W, unsigned int H,unsigned int**p_invers)
{
    (*p_invers)=(unsigned int*)malloc(W*H*sizeof(unsigned int));
    if(p==NULL)
        printf("Nu s-a putut aloca");
    int i;
    for(i=0;i<W*H;i++)
        (*p_invers)[p[i]]=i;    //calculeaza inversa unei permutari
}
void permutare_C_inter(unsigned int W,unsigned int H,struct pixel1*C_inter,unsigned int*p_invers,struct pixel1**D)
{
    (*D)=(struct pixel1*) malloc(W * H * sizeof(struct pixel1));
    if(D==NULL)
        printf("Nu s-a putut aloca");
    int i;
    for(i=0;i<W*H;i++)
        (*D)[p_invers[i]]=C_inter[i];            //se permuta pixelii imaginii conform permutarii p_invers
}
void decriptare(char*imag_init,char*imag_crip,char*fisier,char*imag_decrip,unsigned int W,unsigned int H,struct pixel1**D,struct pixel1*C)
{
    unsigned int seed, R0;
    unsigned int*R=NULL,*p=NULL,*p_invers=NULL;
    citire_seed_R0(fisier,&R0,&seed);
    xorshift32(R0,&R,H,W);
    Durstenfeld(&p,W,H,R);
    inversa(p,W,H,&p_invers);
    struct pixel1*C_inter=NULL;
    C_inter=(struct pixel1*)malloc(W*H*sizeof(struct pixel1));
    if(C_inter==NULL)
        printf("Nu s-a putut aloca");
    int i;
    for(i=0;i<W*H;i++)
    {
        if(i==0)
        {
            C_inter[i].B=octet(seed,0)^C[i].B^octet(R[W*H],0); //se aplica operatia de xorare dupa regula data,pentru octetii 0,1,2
            C_inter[i].G=octet(seed,1)^C[i].G^octet(R[W*H],1);
            C_inter[i].R=octet(seed,2)^C[i].R^octet(R[W*H],2);
        }
        else
        {
            C_inter[i].B=octet(C[i-1].B,0)^C[i].B^octet(R[W*H+i],0);
            C_inter[i].G=octet(C[i-1].G,1)^C[i].G^octet(R[W*H+i],1);
            C_inter[i].R=octet(C[i-1].R,2)^C[i].R^octet(R[W*H+i],2);
        }
    }
    permutare_C_inter(W,H,C_inter,p_invers,D);    //se aplica permutarea
   	imagine_externa(imag_crip,imag_decrip,H,W,*D); //se scrie imaginea decriptata
   	free(C_inter);
    free(R);
    free(p);
    free(p_invers);
}
void calc_chi(char*imagine,unsigned int H,unsigned int W)
{
    double f_teoretic=(W*H)/256,chi_R=0,chi_G=0,chi_B=0;
    unsigned int*frecv_R=NULL,*frecv_G=NULL,*frecv_B=NULL;
    int i,j;
    frecv_R=(unsigned int*)calloc(256,sizeof(unsigned int));
    frecv_G=(unsigned int*)calloc(256,sizeof(unsigned int));        //se calculeaza valoarea testului chi patrat cu ajutorul a 3 vectori de frecventa, cate
    frecv_B=(unsigned int*)calloc(256,sizeof(unsigned int));        //unul pentru fiecare canal R,G,B
    struct pixel1*L=NULL;
    imagine_interna(imagine,H,W,&L);
    for(i=0;i<W*H;i++)
        for(j=0;j<=255;j++)
    {
        if(L[i].R==j) frecv_R[j]++;
        if(L[i].G==j) frecv_G[j]++;     //pentru fiecare valoare j se calculeaza frecventa ei pe fiecare canal
        if(L[i].B==j) frecv_B[j]++;
    }
    for(i=0;i<=255;i++)
    {
        chi_R+=((frecv_R[i]-f_teoretic)*(frecv_R[i]-f_teoretic))/f_teoretic;    //dupa formula data, se calculeaza chi patrat pentru fiecare canal
        chi_G+=((frecv_G[i]-f_teoretic)*(frecv_G[i]-f_teoretic))/f_teoretic;
        chi_B+=((frecv_B[i]-f_teoretic)*(frecv_B[i]-f_teoretic))/f_teoretic;
    }
    printf("Valorile testului chi pe canalele de culoare RGB sunt: %f %f %f \n",chi_R,chi_G,chi_B);
    free(frecv_R);
    free(frecv_G);
    free(frecv_B);
}
