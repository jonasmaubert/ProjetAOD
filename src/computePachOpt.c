/* 
 * File:   computePachOpt.c
 * Author: Nicolas KAMOWSKI Jonas MAUBERT
 *
 * Created on 18 novembre 2015, 08:37
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*Prototypes de fonction*/
void getNbLigne(int* nbLigne, int* longest, char* nomFichier);
int B(int i, int j, int **tab,int n, int m, char* tabF1[], char* tabF2 [], char (*inst)[10000]);
int c(char *ligne1, char *ligne2 );
 //gcc -g -Wall -Wextra -std=c99 src/computePachOpt.c -o computePatchOpt
 
 /*
 *Fonction qui créé la patch de coup minimal entre deux fichiers
 * @param fichier initial
 * @param fichier final
 * @param patch
 */
int main(int argc, char** argv)
{
	if (argc != 3 ) {
		printf("./bin fichier1 fichier2");
		return 0;
	}
	int nbLigne1;
	int nbLigne2;
	int taillePlusLongueLigne = 0;
	getNbLigne(&nbLigne1, &taillePlusLongueLigne, argv[1]);
	getNbLigne(&nbLigne2, &taillePlusLongueLigne, argv[2]);
	//taillePlusLongueLigne++; ??
	FILE* fichier1 = fopen(argv[1],"r");
	FILE* fichier2 = fopen(argv[2],"r");
	/*Recupération des données*/
	/*char tab1[nbLigne1][taillePlusLongueLigne];
	char tab2[nbLigne2][taillePlusLongueLigne];*/
	char** tab1;
	char** tab2;
	tab1 = malloc(nbLigne1*sizeof(char*));
	tab2 = malloc(nbLigne2*sizeof(char*));
	for (int i =0; i<nbLigne1 ; i++ ) {
		tab1[i] = malloc(taillePlusLongueLigne*sizeof(char));
		fgets(tab1[i],taillePlusLongueLigne, fichier1);

	}
	for (int j =0; j< nbLigne2; j++) {
		tab2[j] = malloc(taillePlusLongueLigne*sizeof(char));
		fgets(tab2[j],taillePlusLongueLigne, fichier2);
	}
	fclose(fichier1);
	fclose(fichier2);
	/*Recherche du patch de cout minimal*/
	
	int** tabBellMan;//[nbLigne1 + 1][nbLigne2 + 1];
	tabBellMan = malloc((nbLigne1+1)*sizeof(int*));
	for(int i = 0; i<=nbLigne1;i++) {
		tabBellMan[i] = malloc((nbLigne2+1)*sizeof(int));
	}
	for (int k = 0;k<=nbLigne1; k++ ) {
		for (int l = 0; l<= nbLigne2; l++) {
			tabBellMan[k][l] = -1; //cette valeur correspond a une case non traité
		}
	}
	//Cette chaine de caractère contient en réalité la suite des instructions a effectuer dans le patch
	char instructions[10000] ;
	int res = B(1, 1, tabBellMan,nbLigne1, nbLigne2, tab1, tab2, &instructions);
	//On ecrit le patch sur la sortie
	printf(instructions);
	printf("LE MIN VAUT: %d", res);
	for (int i=0; i<nbLigne1; i++) {
		free(tab1[i]);
	}
	for (int j=0; j<nbLigne2; j++) {
		free(tab2[j]);
	}
	free(tab1);
	free(tab2);
	return(EXIT_SUCCESS);
}

void getNbLigne(int* nbLigne,int* longest, char* nomFichier) {
	FILE* fichier = fopen(nomFichier, "r");
	*nbLigne=0;
	char tmp[1000];
	while (fgets(tmp,1000,fichier) != NULL) {
		if ((int) strlen(tmp) > *longest) {
			*longest = strlen(tmp);
		}
		(*nbLigne)++;
		
	}
	fclose(fichier);
}

int B(int i, int j, int **tab,int n, int m, char* tabF1[], char* tabF2 [], char (*inst)[10000]) {
	int res =0;
	//ATTENTION : i et j correspondent vraiment à la leur num de ligne mais leur position dans le tableau est (i-1) et (j-1) 
	//(n et m correspondent aussi vraiment au nb de ligne) et on va chercher dans tab[i-1][j-1] si la valeur est différente de -1 sinon la valeur 0 ne servirait a rien
	//int n = sizeof(tab) - 1;
	//int m = sizeof(tab[0]) - 1;
	char instTmp[2*sizeof(tabF1[0])];
	char * test="";
	//char* tmp;
	if(tab[i-1][j-1] != -1) {
		return tab[i-1][j-1];
	}
	/* conditions d'arret*/
	//premier if pas vraiment obligatoire nan? a voir...
	if (i==n && j==m) {
		//return le cout de la dernière substitution
		sprintf(instTmp,"= %d\n",i);
		test = strcat(instTmp, tabF2[j-1]);
		test = strcat(instTmp, "\n");
		test = strcat(*inst,instTmp); // inst = inst +"= i\n Fichiers2(j)\n"
		tab[i-1][j-1] = c(tabF1[i-1],tabF2[j-1]);
		return c(tabF1[i-1],tabF2[j-1]);
	}
	if(i==n + 1) {
		//On ajoute les lignes manquantes après i.
		for (int k = m; k > j;k--) {
			sprintf(instTmp,"+ %d\n",n);
			test = strcat(instTmp, tabF2[k-1]);
			test = strcat(instTmp, "\n");
			test = strcat(*inst,instTmp); // inst = inst + "+ n\n Fichiers2(k)\n"
			res+=10+strlen(tabF2[k-1]);
		}

		return res;
	}
	if (j== m+1) {
		//On détruit les lignes en trop
		if (i==n-1) {
			sprintf(instTmp, "d %d\n", n);
			test = strcat(*inst,instTmp);
			return 10;
		} else {
			sprintf(instTmp, "D %d %d\n",i , (n-i));
			test = strcat(*inst,instTmp);
			return 15;
		}
	}
	/*cas général*/
	//ajout ATTENTION L'AJOUT SE FAIT EN I-1 (pour se mettre "avant" la ligne i)
	char instNext[10000]="";
	res = B(i,j+1, tab, n, m,tabF1, tabF2, &instNext) + 10 + strlen(tabF2[j-1]);
	sprintf(instTmp,"+ %d\n",i-1);
	test = strcat(instTmp, tabF2[j-1]);
	test = strcat(instTmp, "\n");
	test = strcat(instTmp,instNext);
	 // substitution
	memset(instNext,0,sizeof(instNext));
	int aux = B(i+1,j+1, tab, n, m, tabF1, tabF2, &instNext) + c(tabF1[i-1],tabF2[j-1]);
	if (aux < res ) {
		sprintf(instTmp,"= %d\n",i);
		test = strcat(instTmp, tabF2[j-1]);
		test = strcat(instTmp, "\n");
		test = strcat(instTmp,instNext);
		res = aux;
	}
	// destruction unique
	memset(instNext,0,sizeof(instNext));
	aux = B(i+1,j, tab, n, m, tabF1, tabF2, &instNext) +10; 
	if (aux < res) {
		sprintf(instTmp, "d %d\n", i);
		test = strcat(instTmp,instNext);
		res = aux;
	}
	//destruction multiple,on doit ester pour chaque valeur de k possible
	memset(instNext,0,sizeof(instNext));
	for (int l = 2; l <= n - i; l++ ) {
		aux = B(i + l,j, tab, n, m, tabF1, tabF2, &instNext) +15; 
		if (aux < res) {
			sprintf(instTmp,"D %d %d\n", i, i+l);
			test = strcat(instTmp,instNext);
			res = aux;
		}
	}
	tab[i-1][j-1] = res;
	/*strcpy(tmp,instTmp);
	strcat(instTmp,*inst);
	strcpy(*inst,instTmp);*/
	test = strcat(*inst, instTmp);
	return res;
}

int c(char *ligne1, char *ligne2 ) {
	if (ligne1 == ligne2) {
		return 0; // ligne identique, pas de substitution
	}
	else {
		return 10 + strlen(ligne2); //cout d'une substitution
	}
}

/*TODO: 
 * PRB de string
 */

