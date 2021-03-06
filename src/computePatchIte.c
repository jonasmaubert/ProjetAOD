/* 
 * File:   computePachOpt.c
 * Author: Nicolas KAMOWSKI Jonas MAUBERT
 *
 * Created on 18 novembre 2015, 08:37
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*Structures*/
struct data {
	int valeur;
	char commande[100];
	int nextI;
	int nextJ;
};
typedef struct data data;

/*Prototypes de fonction*/
void getNbLigne(int* nbLigne, int* longest, char* nomFichier);
int B(data **tab, int n, int m, char* tabF1[], char* tabF2 []);
int c(char *ligne1, char *ligne2);

//gcc -g -Wall -Wextra -std=c99 src/computePatchIte.c -o computePatchIte

/*
 *Fonction qui créé la patch de coup minimal entre deux fichiers
 * @param fichier initial
 * @param fichier final
 * @param patch
 */
int main(int argc, char** argv)
{
	if (argc != 3) {
		printf("./bin fichier1 fichier2");
		return 0;
	}
	int nbLigne1;
	int nbLigne2;
	int taillePlusLongueLigne = 0;
	getNbLigne(&nbLigne1, &taillePlusLongueLigne, argv[1]);
	getNbLigne(&nbLigne2, &taillePlusLongueLigne, argv[2]);
	/* parser le retour*/
	FILE* fichier1 = fopen(argv[1],"r");
	FILE* fichier2 = fopen(argv[2],"r");
	char** tab1;
	char** tab2;
	tab1 = malloc(nbLigne1*sizeof(char*));
	tab2 = malloc(nbLigne2*sizeof(char*));
	char* RetourLigne;
	for (int i =0; i<nbLigne1 ; i++ ) {
		tab1[i] = malloc((taillePlusLongueLigne+1)*sizeof(char));
		fgets(tab1[i],taillePlusLongueLigne, fichier1);
		RetourLigne = strstr(tab1[i], "\n");
		if (RetourLigne == NULL)
		  {
		    strcat(tab1[i], "\n");
		  }
	}
	
	for (int j =0; j< nbLigne2; j++) {
		tab2[j] = malloc((taillePlusLongueLigne+1)*sizeof(char));
		fgets(tab2[j],taillePlusLongueLigne, fichier2);
		RetourLigne = strstr(tab2[j], "\n");
		if (RetourLigne == NULL)
		  {
		    strcat(tab2[j], "\n");
		  }
	}
	fclose(fichier1);
	fclose(fichier2);
	/*Recherche du patch de cout minimal*/
	data** tabBellMan; 
	tabBellMan = malloc((nbLigne1 + 1) * sizeof(data));
	for (int i = 0; i <= nbLigne1; i++) {
		tabBellMan[i] = malloc((nbLigne2 + 1) * sizeof(data));
	}
	for (int k = 0; k <= nbLigne1; k++) {
		for (int l = 0; l <= nbLigne2; l++) {
			tabBellMan[k][l].valeur = -1; //cette valeur correspond a une case non traité
			strcpy(tabBellMan[k][l].commande, "");
			tabBellMan[k][l].nextI = -1;
			tabBellMan[k][l].nextJ = -1;
		}
	}
	int res = B(tabBellMan, nbLigne1, nbLigne2, tab1, tab2);
	//On ecrit le patch sur la sortie
	//peut etre write serait mieux : write(1,instructions); 1= sortie standar, 2= sortie d erreur.
	for (int k = 0; k <= nbLigne1; k++) {
		free(tabBellMan[k]);
	}
	free(tabBellMan);
	for (int i=0; i<nbLigne1; i++) {
		free(tab1[i]);
	}
	for (int j=0; j<nbLigne2; j++) {
		free(tab2[j]);
	}
	free(tab1);
	free(tab2);
	return res;
}

void getNbLigne(int* nbLigne, int* longest, char* nomFichier)
{
	FILE* fichier = fopen(nomFichier, "r");
	*nbLigne = 0;
	char tmp[100];
	while (fgets(tmp, 100, fichier) != NULL) {
		if ((int) strlen(tmp) > *longest) {
			*longest = strlen(tmp);
		}
		(*nbLigne)++;

	}
	fclose(fichier);
}

int B(data **tab, int n, int m, char* tabF1[], char* tabF2 [])
{
	int res = 0;
	int iSuivant = -1;
	int jSuivant = -1;
	char instTmp[10000] = {0};
	res = c(tabF1[n - 1], tabF2[m - 1]);//tabF1[n - 1], tabF2[m - 1]);
	tab[n - 1][m - 1].valeur = res;
	if (res != 0) {
		sprintf(instTmp, "= %d\n", n);
		strcat(instTmp, tabF2[m - 1]); //tabF2[m - 1]);
		strcpy(tab[n - 1][m - 1].commande, instTmp);
	}
	/*cas (n+1,)*/
	/*PRB ce cas là conduit a créer des lignes trop grandes! better traiter sa commande à la fin*/
	for (int j = 0; j < m; j++) {
		for (int k = j; k < m; k++) {

			res += 10 + strlen(tabF2[k]);//tabF2[k]);
		}
		tab[n][j].valeur = res;
	}
	/* cas (,m+1)*/
	for (int i = 0; i < n; i++) {
		if (i == n - 1) {//dernière ligne, une destruction simple suffit
			sprintf(instTmp, "d %d\n", n);
			strcpy(tab[i][m].commande, instTmp);
			tab[i][m].valeur = 10;
		} else {
			sprintf(instTmp, "D %d %d\n", i + 1, (n - i));
			strcpy(tab[i][m].commande, instTmp);
			tab[i][m].valeur = 15;
		}
	}
	/*cas général*/
	for (int i = n; i > 0; i--) {
		for (int j = m; j > 0; j--) {
			//On utilise res et instTmp pour stackoer les résultats temporaires
			//ajout
			res = tab[i - 1][j].valeur + 10 + strlen(tabF2[j - 1]);//tabF2[j - 1]);
			sprintf(instTmp, "+ %d\n", i - 1);
			strcat(instTmp,tabF2[j - 1]);
			iSuivant = i-1;
			jSuivant = j;
			// substitution
			int valC = c(tabF1[i-1], tabF2[j-1]);
			int aux = tab[i][j].valeur + valC;
			if (aux <= res) {
				if (valC != 0) {
					sprintf(instTmp, "= %d\n", i);
					strcat(instTmp, tabF2[j-1]);
				} else {
					strcpy(instTmp, "");
				}
				iSuivant = i;
				jSuivant = j;
				res = aux;
			}
			// destruction unique
			aux = tab[i][j - 1].valeur + 10;
			if (aux < res) {
				sprintf(instTmp, "d %d\n", i);
				iSuivant = i;
				jSuivant = j-1;
				res = aux;
			}
			//destruction multiple,on doit tester pour chaque valeur de k possible
			for (int l = 2; l <= n - i; l++) {
				aux = tab[i + l - 1][j - 1].valeur + 15;
				if (aux < res) {
					sprintf(instTmp, "D %d %d\n", i, l);
					iSuivant = i+l-1;
					jSuivant = j-1;
					res = aux;
				}
			}
			strcpy(tab[i - 1][j - 1].commande, instTmp);
			tab[i - 1][j - 1].valeur = res;
			tab[i - 1][j - 1].nextI = iSuivant;
			tab[i - 1][j - 1].nextJ = jSuivant;
		}
	}
	/*On print toute les instructions ici*/
	data cour = tab[0][0];
	do {
		printf(cour.commande);
		if(cour.nextI == n) {//on add ce qu'il reste
			for(int j = cour.nextJ; j < m;j++) {
			printf( "+ %d\n", n);
			//getLine(&lineF2, f2, j+1, lenghtMax);
			printf(tabF2[j]);
			}
			//On a fini
			return tab[0][0].valeur;
		} else {
		cour = tab[cour.nextI][cour.nextJ];
		}
	} while (cour.nextI != -1 && cour.nextJ != -1);
	return tab[0][0].valeur;
}

int c(char *ligne1, char *ligne2)
{
	if (strcmp(ligne1, ligne2) == 0) {
		return 0; // ligne identique, pas de substitution
	} else {
		return 10 + strlen(ligne2); //cout d'une substitution
	}
}

/*TODO: 
 * -le temps d'exec est encore un peu long et le patch n'est pas parfait
 */


