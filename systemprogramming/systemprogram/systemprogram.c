#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "fields.h"
#include "jval.h"
#include "jrb.h"


#define sinir 1001 //fields.h kutuphanesindeki max uzunluk 1001 oldugu icin siniri 1001 aldik.


int sayimi(const char *sayi)
{
		while(*sayi)
		{
			if (isdigit(*sayi++) == 0 ) return 0;			//keyin sayimi olup olmadigini kontrol eden program.
		}

		return 1;
}



int main(int argc, char const *argv[])
{
	IS is;													//IS is ile fields yapisina ait elemanim oluyor.
	FILE *dosya;											//Dosya belirtiyorum ileride kullanicam.	
	int n=0;												//n'i sayac olarak kullandik	
	char *kontrol;											//kelimeleri parcalara ayirirken kontrol amacli kullaniyoruz.	
	const char *sondort;									//txt dosyasinin input olarak alirken txt dosyasimi degilmi kontrolu icin ayrildi.	
	
	char deger[sinir];										//input icin kullanilacak dizi	

	JRB liste,yedek,gecici;									//agac tanimlamalarimiz.	


	char *addsatir[5];

	is = new_inputstruct(NULL);								//Terminalden inputumuz basliyor.	
	liste = make_jrb();										//liste agacini aktif ettik.	

	fprintf(stderr, ">");									//Odevde input baslangici bole oldugu icin terminalin basina bunu koyuyoruz.

	while(get_line(is) >= 0)								//Satirimiz kalmayana kadar islem yapiyoruz burada
	{
		
		
		kontrol= strdup(strtok(is->text1,"|"));
		
		if(strcmp(kontrol, "add") == 0)
		{
			while(kontrol !=NULL)							//input olarak girilen kisim add ile baslayip devam ediyorsa buraya gelicek her |arasinda kalan kismi alicak| ve addsatir dizisine	
		{													//ekleyecek eger giriste 5 kisim yoksa yanlis oldugu icin uyari vericek eger 5 kisim varsa keyin tamamiyla sayi olup olmadigina bakicak			
			addsatir[n] = kontrol;									
			n++;
			kontrol = strtok(NULL,"|");						//eger o da sayiysa dizidekileri birlestirip bir string halinde agaca aticak.
		}
		if(n==5)
		{
			n=0;
			if(sayimi(addsatir[1]))
			{
			strcpy(deger,addsatir[1]);
			strcat(deger,", ");
			strcat(deger,addsatir[2]);
			strcat(deger,", ");
			strcat(deger,addsatir[3]);
			strcat(deger,", ");
			strcat(deger,addsatir[4]);	
			jrb_insert_int(liste,atoi(addsatir[1]),new_jval_s(strdup(deger)));
		}
			else
			{
				printf("%s\n","Bicim uygun degil");
			}
		}
		else{
			printf("%s\n","Bicim uygun degil");
			}


		}

		else if(strcmp(kontrol,"search") == 0)
		{
			while(kontrol !=NULL)
		{
			addsatir[n]= kontrol;
			n++;
			kontrol = strtok(NULL,"|");
		}
			
		if(n == 2)														
		{												//search yazildiginda gene | arasinda kalanlari ayiracak.| eger iki giris varsa son elemanin uzunlugunu bir azalatcak				
			char *kf;									//cunku kelimeleri ayirirken sona belirsiz bir karakter koyuyor.Onu da aldiktan sonra key sayimi degilmi ona bakicak eger				
			kf = addsatir[1];							//sayiysa agacin icine girip aricak bulursa yazicak bulamassa Aranan kayit bulunamadi yazicak.
			kf[strlen(kf)-1]=0;
			addsatir[1]=kf;

			n=0;

		if(sayimi(addsatir[1]))
		{
			gecici=jrb_find_int(liste,atoi(addsatir[1]));
			if(gecici == NULL)
				printf("%s\n","Aranan kayit bulunamadi.");
			else
				printf("%s",gecici->val.s);
		}

		else 
		{
			printf("%s\n","Bicim uygun degil");
		}

		}

		}

		else if(strcmp(kontrol,"write") == 0)
		{

			while(kontrol !=NULL)
		{
			addsatir[n] = kontrol;
			kontrol = strtok(NULL,"|");
			n++;
		}
		if(n == 2)										//write da ayni sekilde |arada kalanlari ayiricak| ve 2 giris varsa girisler dogruysa dosya acicak dosyadina .txt uzantisini			
		{												//ekleyip kucukten buyuge kendi yazicak aksi halde Bicim uygun degil yazisi cikicak.			
		n=0;
		char *lf;
		lf = addsatir[1];
		lf[strlen(lf)-1] = 0;
		strcat(lf,".txt");
		dosya=fopen(lf,"w");

		jrb_traverse(yedek,liste){
			fprintf(dosya, "%s",jval_s(yedek->val));
		}
		fclose(dosya);
		}
		else
		{
			printf("%s\n","Bicim uygun degil");
		}

		}

		else if (is->NF == 0)									//Bos girilmesini engelliyoruz.
		{
			printf("%s\n","Lutfen bir komut giriniz.");
		}

		else if (strcmp(is->fields[0],"print") == 0)
		{
			jrb_traverse(yedek,liste)							// listeyi yazdiriyor.	
			{
				printf("%s",jval_s(yedek->val));
			}
		}
		
		else if (strcmp(is->fields[0],"quit") == 0)
		{
			jettison_inputstruct(is);
			jrb_free_tree(liste);								//is ve listeyi bosaltarak programdan cikiyor.	
			exit(0);

		}
		
		else if (strcmp(is->fields[0],"pro") == 0)
		{
			if(strcmp(is->fields[1],"<") == 0)
			{
				char *yakala = is->fields[2];			
				sondort = &yakala[strlen(yakala)-4];
				if(strcmp(sondort,".txt") == 0)
				{
					
					FILE *ac= fopen(is->fields[2],"r");									//add kismi ile ayni sey yapiliyor fakat burda dosya acilip satir satir okuluyor ve					
					if( ac !=NULL)														//dosyanin txt uzantisina sahip olup olmadigi garanti ediliyor.					
					{
						char satir [128]; //Uygun maximum satir sayisi		

						while(fgets (satir,sizeof satir,ac) !=NULL)
						{
							
							yakala = strdup(strtok(satir,"|"));
							if(strcmp(yakala,"add") == 0)
							{
								while(yakala !=NULL)
								{
									addsatir[n] = yakala;
									n++;
									yakala = strtok(NULL,"|");
								}
								if(n==5)
								{
									n=0;
									if(sayimi(addsatir[1]))
									{
										strcpy(deger,addsatir[1]);
										strcat(deger,", ");
										strcat(deger,addsatir[2]);
										strcat(deger,", ");
										strcat(deger,addsatir[3]);
										strcat(deger,", ");
										strcat(deger,addsatir[4]);
										jrb_insert_int(liste,atoi(addsatir[1]),new_jval_s(strdup(deger)));	
									}
								}
							}



						}
						fclose(ac);

					}
					else
					{
						perror(is->fields[2]);
					}

				}
			}	
		}



		else
		{
			printf("%s\n","Bicim uygun degil");
		}
		
		fprintf(stderr,">");								//input karakteri devam ediyor.
		
	}


	return 0;
}