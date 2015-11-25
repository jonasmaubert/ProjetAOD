/* 
 * File:   computePachOpt.c
 * Author: Nicolas KAMOWSKI Jonas MAUBERT
 *
 * Created on 18 novembre 2015, 08:37
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/*Structures*/
struct data {
	int valeur;
	char commande[15];
	int nextI;
	int nextJ;
};
typedef struct data data;

/*Prototypes de fonction*/
void getNbLigne(int* nbLigne, int* longest, char* nomFichier);
int B(data **tab, int n, int m, char* tabF1[], char* tabF2 []);
int c(char *ligne1, char *ligne2);
int Max(int a, int b);

//gcc -pg -Wall -Wextra -std=c99 src/computePatchOpt.c -o computePatchOpt

/*
 *Fonction qui créé la patch de coup minimal entre deux fichiers
 * @param fichier initial
 * @param fichier final
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
	taillePlusLongueLigne++;
	/* parser le retour*/
	FILE* fichier1 = fopen(argv[1], "r");
	FILE* fichier2 = fopen(argv[2], "r");
	char** tab1;
	char** tab2;
	tab1 = malloc(nbLigne1 * sizeof(char*));
	tab2 = malloc(nbLigne2 * sizeof(char*));
	char* RetourLigne;
	for (int i = 0; i < nbLigne1; i++) {
		tab1[i] = malloc((taillePlusLongueLigne) * sizeof(char));
		fgets(tab1[i], taillePlusLongueLigne, fichier1);
		RetourLigne = strstr(tab1[i], "\n");
		if (RetourLigne == NULL) {
			strcat(tab1[i], "\n");
		}
	}

	for (int j = 0; j < nbLigne2; j++) {
		tab2[j] = malloc((taillePlusLongueLigne) * sizeof(char));
		fgets(tab2[j], taillePlusLongueLigne, fichier2);
		RetourLigne = strstr(tab2[j], "\n");
		if (RetourLigne == NULL) {
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
			tabBellMan[k][l].valeur = -1;
			strcpy(tabBellMan[k][l].commande, "");
			tabBellMan[k][l].nextI = nbLigne1;
			tabBellMan[k][l].nextJ = nbLigne2;
		}
	}
	int res = B(tabBellMan, nbLigne1, nbLigne2, tab1, tab2);
	fprintf(stderr, "Cout du patch : %d", res);
	//On ecrit le patch sur la sortie
	//peut etre write serait mieux : write(1,instructions); 1= sortie standar, 2= sortie d erreur.
	for (int k = 0; k <= nbLigne1; k++) {
		free(tabBellMan[k]);
	}
	free(tabBellMan);
	for (int i = 0; i < nbLigne1; i++) {
		free(tab1[i]);
	}
	for (int j = 0; j < nbLigne2; j++) {
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
	char tmp[200];
	while (fgets(tmp, 200, fichier) != NULL) {
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
	char instTmp[15] = {0};
	/*Condition d'arret */
	tab[n][m].nextI = -1;
	tab[n][m].nextJ = -1;
	tab[n][m].valeur = 0;
	/*Cas particuliers */
	res = c(tabF1[n - 1], tabF2[m - 1]);
	tab[n - 1][m - 1].valeur = res;
	if (res != 0) {
		sprintf(instTmp, "= %d\n", n);
		strcpy(tab[n - 1][m - 1].commande, instTmp);
	}
	/*cas (n+1,)*/
	/*PRB ce cas là conduit a créer des lignes trop grandes! better traiter sa commande à la fin*/
	for (int j = m - 1; j >= 0; j--) {
		res = tab[n][j + 1].valeur + 10 + strlen(tabF2[j]);
		sprintf(instTmp, "+ %d\n", n);
		strcpy(tab[n][j].commande, instTmp);
		tab[n][j].valeur = res;
		tab[n][j].nextI = n;
		tab[n][j].nextJ = j + 1;
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
			//On utilise res et instTmp pour stocker les résultats temporaires
			if (tab[i][j - 1].commande[0] == 'd') {
				//Debut de destruction multiple
				res = tab[i][j - 1].valeur + 5;
				sprintf(instTmp, "D %d 2\n", i); //elle commence à la ligne actuelle et va jusqu'a la ligne de d
				iSuivant = i + 1; //On saute la case de destruction simple pour les isntructions
				jSuivant = j - 1;
			} else if (tab[i][j - 1].commande[0] == 'D') {
				//continuité de destruction multiple
				res = tab[i][j - 1].valeur;
				char dest;
				int tmp;
				int nbD;
				//on recupère le nombre de destruction multiple
				sscanf(tab[i][j - 1].commande, "%c %d %d", &dest, &tmp, &nbD);
				sprintf(instTmp, "D %d %d\n", i, nbD + 1);
				iSuivant = i + nbD;
				jSuivant = j - 1;
			} else {
				//Destruction simple
				res = tab[i][j - 1].valeur + 10;
				sprintf(instTmp, "d %d\n", i);
				iSuivant = i;
				jSuivant = j - 1;
			}
			//Ajout
			int aux = tab[i - 1][j].valeur + 10 + strlen(tabF2[j - 1]);
			if (aux < res) {
				//L'ajout se fait avant la ligne courante
				sprintf(instTmp, "+ %d\n", i - 1);
				iSuivant = i - 1;
				jSuivant = j;
				res = aux;
			}
			//Substitution
			int valC = c(tabF1[i - 1], tabF2[j - 1]);
			aux = tab[i][j].valeur + valC;
			if (aux < res) {
				if (valC != 0) {
					sprintf(instTmp, "= %d\n", i);
				} else {
					strcpy(instTmp, "");
				}
				iSuivant = i;
				jSuivant = j;
				res = aux;
			}

			strcpy(tab[i - 1][j - 1].commande, instTmp);
			tab[i - 1][j - 1].valeur = res;
			tab[i - 1][j - 1].nextI = iSuivant;
			tab[i - 1][j - 1].nextJ = jSuivant;
		}
	}
	/*On print toute les instructions ici*/
	data cour = tab[0][0];
	int ligneF2Cour = 0;
	do {
		write(1, cour.commande, strlen(cour.commande));
		if (cour.commande[0] == '+' || cour.commande[0] == '=') {
			write(1, tabF2[ligneF2Cour], strlen(tabF2[ligneF2Cour]));
		}
		ligneF2Cour = cour.nextJ;
		cour = tab[cour.nextI][cour.nextJ];
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

int Max(int a, int b)
{
	return(a > b ? a : b);
}

/*TODO: 
 * -le temps d'exec est encore un peu long et le patch n'est pas parfait
 */

