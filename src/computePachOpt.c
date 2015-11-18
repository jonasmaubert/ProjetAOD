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
	/*
	 *
	 * 
	 * 
	 * 
	 * 
	 * 
	 */
	
	
	
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

/*TODO: 
 * Parser le fichier1 pour en récuperer toute les données (sous quelle forme? tab?)
 * Faire de même avec le fichier 2
 * (on fait ça car pour trouver le patch de cout min on va devoir lire plusieurs fois caque ligne)
 * On cerche le patch de cout minimal avec les algos vus en cours:
 * On peut commencer par chercher le cout de ce patch puis ajouter les éléments 
 * pour dégager les différentes instructions que l'on recherche
 */

