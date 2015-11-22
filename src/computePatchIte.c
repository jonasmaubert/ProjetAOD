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
	char commande[100000];
};
typedef struct data data;

/*Prototypes de fonction*/
void getNbLigne(int* nbLigne, int* longest, char* nomFichier);
int B(data **tab, int n, int m, char* tabF1[], char* tabF2 [], char (*inst)[10000]);
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
	FILE* fichier1 = fopen(argv[1], "r");
	FILE* fichier2 = fopen(argv[2], "r");
	/*Recupération des données*/
	char** tab1;
	char** tab2;
	tab1 = malloc(nbLigne1 * sizeof(char*));
	tab2 = malloc(nbLigne2 * sizeof(char*));
	char* RetourLigne;
	for (int i = 0; i < nbLigne1; i++) {
		tab1[i] = malloc((taillePlusLongueLigne + 1) * sizeof(char));
		fgets(tab1[i], taillePlusLongueLigne, fichier1);
		RetourLigne = strstr(tab1[i], "\n");
		if (RetourLigne == NULL) {
			strcat(tab1[i], "\n");
		}
	}

	for (int j = 0; j < nbLigne2; j++) {
		tab2[j] = malloc((taillePlusLongueLigne + 1) * sizeof(char));
		fgets(tab2[j], taillePlusLongueLigne, fichier2);
		RetourLigne = strstr(tab2[j], "\n");
		if (RetourLigne == NULL) {
			strcat(tab2[j], "\n");
		}
	}
	fclose(fichier1);
	fclose(fichier2);
	/*Recherche du patch de cout minimal*/

	data** tabBellMan; //[nbLigne1 + 1][nbLigne2 + 1];
	tabBellMan = malloc((nbLigne1 + 1) * sizeof(data));
	for (int i = 0; i <= nbLigne1; i++) {
		tabBellMan[i] = malloc((nbLigne2 + 1) * sizeof(data));
	}
	for (int k = 0; k <= nbLigne1; k++) {
		for (int l = 0; l <= nbLigne2; l++) {
			tabBellMan[k][l].valeur = -1; //cette valeur correspond a une case non traité
			strcpy(tabBellMan[k][l].commande, "");
		}
	}
	//Cette chaine de caractère contient en réalité la suite des instructions a effectuer dans le patch
	char instructions[10000]={0};
	int res = B(tabBellMan, nbLigne1, nbLigne2, tab1, tab2, &instructions);
	//On ecrit le patch sur la sortie
	//peut etre write serait mieux : write(1,instructions); 1= sortie standar, 2= sortie d erreur.
	printf(instructions);
	for (int i = 0; i < nbLigne1; i++) {
		free(tab1[i]);
	}
	for (int j = 0; j < nbLigne2; j++) {
		free(tab2[j]);
	}
	for (int k = 0; k <= nbLigne1; k++) {
		free(tabBellMan[k]);
	}
	free(tabBellMan);
	free(tab1);
	free(tab2);
	return res;
}

void getNbLigne(int* nbLigne, int* longest, char* nomFichier)
{
	FILE* fichier = fopen(nomFichier, "r");
	*nbLigne = 0;
	char tmp[1000];
	while (fgets(tmp, 1000, fichier) != NULL) {
		if ((int) strlen(tmp) > *longest) {
			*longest = strlen(tmp);
		}
		(*nbLigne)++;

	}
	fclose(fichier);
}

int B(data **tab, int n, int m, char* tabF1[], char* tabF2 [], char (*inst)[10000])
{
	int res = 0;
	char instTmp[10000] = {0};
	/*cas (n,m)*/
	res = c(tabF1[n - 1], tabF2[m - 1]);
	tab[n-1][m-1].valeur = res;
	if (res != 0) {
		sprintf(instTmp, "= %d\n", n);
		strcat(instTmp, tabF2[m - 1]);
		//strcat(*inst, instTmp); // inst = inst +"= i\n Fichiers2(j)\n"
		strcpy(tab[n - 1][m - 1].commande, instTmp);
	}
	/*cas (n+1,)*/
	for (int j=0; j<m; j++) {
		for (int k = j; k < m; k++) {
			sprintf(instTmp, "+ %d\n", n);
			strcat(instTmp, tabF2[k]);
			strcat(tab[n][j].commande, instTmp); // inst = inst + "+ n\n Fichiers2(k)\n"
			res += 10 + strlen(tabF2[k]);
		}
		//strcpy(tab[n][j - 1].commande, *instTmp);
		tab[n][j - 1].valeur = res;
	}
	/* cas (,m+1)*/
	for(int i = 0;i<n ; i++) {
		if (i == n-1) {//dernière ligne, une destruction simple suffit
			sprintf(instTmp, "d %d\n", n);
			strcat(*inst, instTmp);
			strcpy(tab[i][m].commande, *inst);
			tab[i][m].valeur = 10;
		} else {
			sprintf(instTmp, "D %d %d\n", i+1, (n - i));
			strcat(*inst, instTmp);
			strcpy(tab[i][m].commande, *inst);
			tab[i][m].valeur = 15;
		}
	}
	/*cas général*/
	//on remplace B(i,j,tabB,tab1,tab2,inst) par t[]
	for (int i = n; i > 0; i--) {
		for (int j = m; j > 0; j--) {
			//On utilise res et instTmp pour stackoer les résultats temporaires
			//ajout
			res = tab[i-1][j].valeur+ 10 + strlen(tabF2[j - 1]);
			sprintf(instTmp, "+ %d\n", i - 1);
			strcat(instTmp, tabF2[j - 1]);
			strcat(instTmp, tab[i-1][j].commande );
			// substitution
			int aux = tab[i][j].valeur + c(tabF1[i - 1], tabF2[j - 1]);
			if (aux <= res) {
				if (c(tabF1[i - 1], tabF2[j - 1]) != 0) {
					sprintf(instTmp, "= %d\n", i);
					strcat(instTmp, tabF2[j - 1]);
				} else {
					strcpy(instTmp, "");
				}
				strcat(instTmp, tab[i][j].commande );
				res = aux;
			}
			// destruction unique
			aux = tab[i][j-1].valeur + 10;
			if (aux < res) {
				sprintf(instTmp, "d %d\n", i);
				strcat(instTmp, tab[i][j-1].commande);
				res = aux;
			}
			//destruction multiple,on doit tester pour chaque valeur de k possible
			for (int l = 2; l <= n - i; l++) {
				aux = tab[i+l-1][j-1].valeur + 15;
				if (aux < res) {
					sprintf(instTmp, "D %d %d\n", i, i + l);
					strcat(instTmp, tab[i+l-1][j-1].commande);
					res = aux;
				}
			}
			strcpy(tab[i-1][j-1].commande ,instTmp);
			tab[i-1][j-1].valeur = res;
		}
	}
	strcpy(*inst, tab[0][0].commande);
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
 * -continuer les tests
 */


