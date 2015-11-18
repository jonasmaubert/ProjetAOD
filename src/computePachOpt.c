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

/*
 * gcc -Wall -std=c99 src/computePachOpt.c -o computePatchOpt
 */
int main(int argc, char** argv)
{
	
	int nbLigne1;
	int nbLigne2;
	int taillePlusLongueLigne = 0;
	getNbLigne(&nbLigne1, &taillePlusLongueLigne, argv[1]);

	getNbLigne(&nbLigne2, &taillePlusLongueLigne, argv[2]);
	FILE* fichier1 = fopen(argv[1],"r");
	FILE* fichier2 = fopen(argv[2],"r");
	/*Recupération des données*/
	char tab1[nbLigne1][taillePlusLongueLigne];
	char tab2[nbLigne2][taillePlusLongueLigne];
	for (int i=0; i<nbLigne1; i++) {
		fgets(tab1[i],taillePlusLongueLigne, fichier1);
	}
	for (int j=0; j<nbLigne2; j++) {
		fgets(tab2[j],taillePlusLongueLigne, fichier2);
	}
	fclose(fichier1);
	fclose(fichier2);
	/*Recherche du patch de cout minimal*/
	
	int tabBellMan[nbLigne1][nbLigne2];
	for (int k = 0;k<nbLigne1; k++ ) {
		for (int l = 0; l< nbLigne2; l++) {
			tabBellMan[k][l] = -1; //cette valeur correspond a une case non traité
		}
	}
	
	int res = B(0,1,&tabBellMan, tab1, tab2);
	return(EXIT_SUCCESS);
}

void getNbLigne(int* nbLigne,int* longest, char* nomFichier) {
	FILE* fichier = fopen(nomFichier, "r");
	*nbLigne=0;
	char tmp[1000];
	while (fgets(tmp,1000,fichier) != NULL) {
		if (strlen(tmp) > *longest) {
			*longest = strlen(tmp);
		}
		(*nbLigne)++;
		
	}
	fclose(fichier);
}

int B(int i, int j, int* tab[][], char tabF1[][], char tabF2 [][]) {
	int res =0;
	int n = tab.length - 1;
	int m = tab[0].length - 1;
	if(tab[i][j] != -1) {
		return tab[i][j];
	}
	//TODO: que se passe t'il si des fichiers sont vide?
	if (i==n && j==m) {
		//return le cout de la dernière substitution
		return c(tabF1[i],tabF2[j]); 
	}
	if(i==n) {
		//On ajoute les lignes manquantes
		for (int k = j; k <= m;k++) {
			res+=10+strlen(tabF2[k]);
		}
		return res;
	}
	
	if (j==m) {
		//On détruit les lignes en trop
		if (i==n-1) {
			return 10;
		} else {
			return 15;
		}
	}
	/*cas général*/
	res = B(i,j-1) + 10 + strlen(tabF2[j]); //cas ajout
	int aux = B(i-1,j-1) + c(tabF1[i],tabF2[j]); //cas sub
	if (aux < res) {
		res = aux;
	}
	aux = B(i-1,j) +10; //cas dest unique
	if (aux < res) {
		res = aux;
	}
	//k?????
	int k;
	aux = B(i-k,j) +15; //cas dest mult
	if (aux < res) {
		res = aux;
	}
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
 * Parser le fichier1 pour en récuperer toute les données (sous quelle forme? tab?)
 * Faire de même avec le fichier 2
 * (on fait ça car pour trouver le patch de cout min on va devoir lire plusieurs fois caque ligne)
 * On cerche le patch de cout minimal avec les algos vus en cours:
 * On peut commencer par chercher le cout de ce patch puis ajouter les éléments 
 * pour dégager les différentes instructions que l'on recherche
 * 
 */

