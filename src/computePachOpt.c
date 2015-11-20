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
	char commande[10000];
};
typedef struct data data;

/*Prototypes de fonction*/
void getNbLigne(int* nbLigne, int* longest, char* nomFichier);
int B(int i, int j, data **tab,int n, int m, char* tabF1[], char* tabF2 [], char (*inst)[10000]);
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
	FILE* fichier1 = fopen(argv[1],"r");
	FILE* fichier2 = fopen(argv[2],"r");
	/*Recupération des données*/
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
	
	data** tabBellMan;//[nbLigne1 + 1][nbLigne2 + 1];
	tabBellMan = malloc((nbLigne1+1)*sizeof(data));
	for(int i = 0; i<=nbLigne1;i++) {
		tabBellMan[i] = malloc((nbLigne2+1)*sizeof(data));
	}
	for (int k = 0;k<=nbLigne1; k++ ) {
		for (int l = 0; l<= nbLigne2; l++) {
			tabBellMan[k][l].valeur = -1; //cette valeur correspond a une case non traité
			strcpy(tabBellMan[k][l].commande,"");
		}
	}
	//Cette chaine de caractère contient en réalité la suite des instructions a effectuer dans le patch
	char instructions[10000];
	int res = B(1, 1, tabBellMan,nbLigne1, nbLigne2, tab1, tab2, &instructions);
	//On ecrit le patch sur la sortie
	printf(instructions);
	for (int i=0; i<nbLigne1; i++) {
		free(tab1[i]);
	}
	for (int j=0; j<nbLigne2; j++) {
		free(tab2[j]);
	}
	for(int k=0; k<=nbLigne1;k++) {
		free(tabBellMan[k]);
	}
	free(tabBellMan);
	free(tab1);
	free(tab2);
	return res;
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

int B(int i, int j, data **tab,int n, int m, char* tabF1[], char* tabF2 [], char (*inst)[10000]) {
	int res =0;
	char *instTmp=malloc(10000*sizeof(char));
	if(tab[i-1][j-1].valeur != -1) {
		strcpy(*inst, tab[i-1][j-1].commande);
		return tab[i-1][j-1].valeur;
	}
	/* conditions d'arret*/
	//premier if pas vraiment obligatoire nan? a voir...
	if (i==n && j==m) {
		//return le cout de la dernière substitution
		res = c(tabF1[i-1],tabF2[j-1]);
		if (res !=0 ) {
			sprintf(instTmp,"= %d\n",i);
			strcat(instTmp, tabF2[j-1]);
			strcat(*inst,instTmp); // inst = inst +"= i\n Fichiers2(j)\n"
			strcpy(tab[i-1][j-1].commande, *inst);
		}
		tab[i-1][j-1].valeur = res;
		return res;
	}
	if(i==n + 1) {
		//On ajoute les lignes manquantes après i.
		for (int k = j; k <= m;k++) {
			sprintf(instTmp,"+ %d\n",n);
			strcat(instTmp, tabF2[k-1]);
			strcat(*inst,instTmp); // inst = inst + "+ n\n Fichiers2(k)\n"
			res+=10+strlen(tabF2[k-1]);
		}
		strcpy(tab[i-1][j-1].commande, *inst);
		tab[i-1][j-1].valeur = res;
		return res;
	}
	if (j== m+1) {
		//On détruit les lignes en trop
		if (i==n) {
			sprintf(instTmp, "d %d\n", n);
			strcat(*inst,instTmp);
			strcpy(tab[i-1][j-1].commande, *inst);
			tab[i-1][j-1].valeur = 10;
			return 10;
		} else {
			sprintf(instTmp, "D %d %d\n",i , (n-i+1));
			strcat(*inst,instTmp);
			strcpy(tab[i-1][j-1].commande, *inst);
			tab[i-1][j-1].valeur = 15;
			return 15;
		}
	}
	/*cas général*/
	//ajout ATTENTION L'AJOUT SE FAIT EN I-1 (pour se mettre "avant" la ligne i)
	char (*instNext)[10000]=malloc(10000*sizeof(char));
	strcpy(*instNext,"");
	
	res = B(i,j+1, tab, n, m,tabF1, tabF2, instNext) + 10 + strlen(tabF2[j-1]);
	sprintf(instTmp,"+ %d\n",i-1);
	strcat(instTmp, tabF2[j-1]);
	strcat(instTmp,*instNext);
	
	 // substitution
	strcpy(*instNext,"");
	int aux = B(i+1,j+1, tab, n, m, tabF1, tabF2, instNext) + c(tabF1[i-1],tabF2[j-1]);
	if (aux <= res ) {
		if (c(tabF1[i-1],tabF2[j-1]) != 0) {
			sprintf(instTmp,"= %d\n",i);
			strcat(instTmp, tabF2[j-1]);
			strcat(instTmp,*instNext);
		}
		res = aux;
	}

	// destruction unique
	strcpy(*instNext,"");
	aux = B(i+1,j, tab, n, m, tabF1, tabF2, instNext) +10; 
	if (aux < res) {
		sprintf(instTmp, "d %d\n", i);
		strcat(instTmp,*instNext);
		res = aux;
	}

	//destruction multiple,on doit ester pour chaque valeur de k possible
	strcpy(*instNext,"");
	for (int l = 2; l <= n - i; l++ ) {
		aux = B(i + l,j, tab, n, m, tabF1, tabF2, instNext) +15;
		if (aux < res) {
			sprintf(instTmp,"D %d %d\n", i, i+l);
			strcat(instTmp,*instNext);
			res = aux;
		}
	}
	
	strcpy(*inst, instTmp);
	strcpy(tab[i-1][j-1].commande, *inst);
	tab[i-1][j-1].valeur = res;
	free(instNext);
	free(instTmp);
	return res;
}

int c(char *ligne1, char *ligne2 ) {
	if (strcmp(ligne1,ligne2) == 0) {
		return 0; // ligne identique, pas de substitution
	}
	else {
		return 10 + strlen(ligne2); //cout d'une substitution
	}
}

/*TODO: 
 * -continuer les tests
 */

