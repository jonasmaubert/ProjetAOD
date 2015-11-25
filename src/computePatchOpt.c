/* 
 * \file    computePachOpt.c
 * \author  Nicolas KAMOWSKI
 * \author  Jonas MAUBERT
 * \date    Created on 18 novembre 2015, 08:37
 *
 * Implemente le programme de création de patch de cout minimum.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/*Structures*/
/**
 * \struct data
 * Permet de stocker pour chaque couple de ligne du fichier 1 et 2 le 
 * minimum du cout du patch à cette étape, le type de commande pour
 * cette ligne et le nombre de destruction si la commande est une 
 * destruction multiple. Cette structure contient aussi deux int
 * représentant la localisations des prochaines instructions pour le patch.
 */
struct data {
	int valeur;
	char commande;
	int nbDest;
	int nextI;
	int nextJ;
};
typedef struct data data;

/*Prototypes de fonction*/
/**
 * Fonction permettant d'obtenir le nombre de ligne d'un fichier ainsi
 * que la taille de la ligne la plus grande.
 * @param nbLigne Nombre de ligne du fichier. 
 * @param longuest Taille de la ligne la plus grande.
 * @param nomFichier nom du fichier à parcourir.
 */
void getNbLigne(int* nbLigne, int* longest, char* nomFichier);

/**
 * Fonction implémentant les équations de bellman pour trouver le patch de 
 * cout minimal.
 * @param tab Matrice de permettant de stocker les informations sur le patch.
 * @param n Nombre de ligne du fichier 1.
 * @param m Nombre de ligne du fichier 2.
 * @param tabF1 tableau contenant toutes les lignes du fichiers 1.
 * @param tabF2 tableau contenant toutes les lignes du fichiers 2.
 * \return {cout du patch}
 */
int B(data **tab, int n, int m, char* tabF1[], char* tabF2 []);

/**
 * Fonction donnant le cout d'une substitution.
 * @param ligne1 première chaine de caractère.
 * @param ligne2 deuxième chaine de caractère.
 * \return {0 si les lignes sont égales, 10 plus la taille de ligne2 sinon}
 */
int c(char *ligne1, char *ligne2);


/*
 *Fonction qui créé la patch de coup minimal entre deux fichiers
 * @param fichier initial
 * @param fichier final
 * \return {0 si le programme a fonctionné}
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
	/*parser*/
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
			tabBellMan[k][l].commande = 'x';//caractère par defaut;
			tabBellMan[k][l].nbDest = 0; 
			tabBellMan[k][l].nextI = nbLigne1;
			tabBellMan[k][l].nextJ = nbLigne2;
		}
	}
	int res = B(tabBellMan, nbLigne1, nbLigne2, tab1, tab2);
	fprintf(stderr, "Cout du patch : %d", res);
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
	return 0;
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
		tab[n - 1][m - 1].commande = '=';
	}
	/*cas (n+1,)*/
	for (int j = m - 1; j >= 0; j--) {
		res = tab[n][j + 1].valeur + 10 + strlen(tabF2[j]);
		tab[n][j].commande = '+';
		tab[n][j].valeur = res;
		tab[n][j].nextI = n;
		tab[n][j].nextJ = j + 1;
	}
	/* cas (,m+1)*/
	for (int i = 0; i < n; i++) {
		if (i == n - 1) {//dernière ligne, une destruction simple suffit
			tab[i][m].commande = 'd';
			tab[i][m].valeur = 10;
		} else {
			tab[i][m].commande = 'D';
			tab[i][m].nbDest = n-i;
			tab[i][m].valeur = 15;
		}
	}
	/*cas général*/
	char tmp;
	for (int i = n; i > 0; i--) {
		for (int j = m; j > 0; j--) {
			//On utilise res et instTmp pour stocker les résultats temporaires
			if (tab[i][j - 1].commande == 'd') {
				//Debut de destruction multiple
				res = tab[i][j - 1].valeur + 5;
				tmp = 'D';
				tab[i-1][j-1].nbDest = 2;
				iSuivant = i + 1; 
			} else if (tab[i][j - 1].commande == 'D') {
				//continuité de destruction multiple
				res = tab[i][j - 1].valeur;
				tmp = 'D';
				tab[i-1][j-1].nbDest = tab[i][j-1].nbDest + 1;
				iSuivant = i + tab[i][j-1].nbDest;
			} else {
				//Destruction simple
				res = tab[i][j - 1].valeur + 10;
				tmp = 'd';
				iSuivant = i;
			}
			jSuivant = j-1;
			//Ajout
			int aux = tab[i - 1][j].valeur + 10 + strlen(tabF2[j - 1]);
			if (aux < res) {
				//L'ajout se fait avant la ligne courante
				tmp = '+';
				iSuivant = i - 1;
				jSuivant = j;
				res = aux;
			}
			//Substitution
			int valC = c(tabF1[i - 1], tabF2[j - 1]);
			aux = tab[i][j].valeur + valC;
			if (aux < res) {
				if (valC != 0) {
					tmp = '=';
				} else {
					tmp = 'x';
				}
				iSuivant = i;
				jSuivant = j;
				res = aux;
			}
			tab[i - 1][j - 1].commande = tmp;
			tab[i - 1][j - 1].valeur = res;
			tab[i - 1][j - 1].nextI = iSuivant;
			tab[i - 1][j - 1].nextJ = jSuivant;
		}
	}
	/*On print toutes les instructions ici*/
	data cour = tab[0][0];
	int ligneF2Cour = 0;
	int ligneF1Cour = 0;
	do {
		if (cour.commande == 'd') {
			sprintf(instTmp,"%c %d\n",cour.commande, ligneF1Cour + 1);
			write(1,instTmp,strlen(instTmp));
		}
		else if (cour.commande == '+') {
			sprintf(instTmp,"%c %d\n", cour.commande, ligneF1Cour);
			write(1,instTmp,strlen(instTmp));
			write(1, tabF2[ligneF2Cour], strlen(tabF2[ligneF2Cour]));
		}
		else if (cour.commande == '=') {
			sprintf(instTmp,"%c %d\n", cour.commande, ligneF1Cour+1);
			write(1,instTmp,strlen(instTmp));
			write(1, tabF2[ligneF2Cour], strlen(tabF2[ligneF2Cour]));
		}
		else if(cour.commande == 'D') {
			sprintf(instTmp,"%c %d %d\n", cour.commande, ligneF1Cour + 1, cour.nbDest);
			write(1,instTmp,strlen(instTmp));
		}
		ligneF2Cour = cour.nextJ;
		ligneF1Cour = cour.nextI;
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


