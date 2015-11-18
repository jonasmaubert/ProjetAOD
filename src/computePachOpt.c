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


 //gcc -Wall -Wextra -std=c99 src/computePachOpt.c -o computePatchOpt
 
 /*
 *Fonction qui créé la patch de coup minimal entre deux fichiers
 * @param fichier initial
 * @param fichier final
 * @param patch
 */
int main(int argc, char** argv)
{
	if (argc != 4 ) {
		printf("./bin fichiers1 fichiers2 patch")
	}
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
	
	int tabBellMan[nbLigne1 + 1][nbLigne2 + 1];
	for (int k = 0;k<=nbLigne1; k++ ) {
		for (int l = 0; l<= nbLigne2; l++) {
			tabBellMan[k][l] = -1; //cette valeur correspond a une case non traité
		}
	}
	//Cette chaine de caractère contient en réalité la suite des instructions a effectuer dans le patch
	char *instructions = "";
	int res = B(1, 1, &tabBellMan, tab1, tab2, instructions);
	//On peut maintenant écrire le patch (argv[3])
	writePatch(instructions,argv[3]);
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

int B(int i, int j, int* tab[][], char tabF1[][], char tabF2 [][], char * inst) {
	int res =0;
	int n = tab.length - 1;
	int m = tab[0].length - 1;
	char * instTmp;
	if(tab[i][j] != -1) {
		return tab[i][j];
	}
	//TODO: que se passe t'il si des fichiers sont vide?
	//premier if pas vraiment obligatoire nan? a voir...
	if (i==n && j==m) {
		//return le cout de la dernière substitution
		sprintf(instTmp,"= %d\n",i);
		strcat(instTmp, tabF2[j]);
		strcat(instTmp, "\n");
		strcat(inst,instTmp); // inst = inst +"= i\n Fichiers2(j)\n"
		return c(tabF1[i],tabF2[j]);
	}
	if(i==n + 1) {
		//On ajoute les lignes manquantes après i.
		for (int k = m; k >= j;k--) {
			sprintf(instTmp,"+ %d\n",n);
			strcat(instTmp, tabF2[k]);
			strcat(instTmp, "\n");
			strcat(inst,instTmp); // inst = inst + "+ n\n Fichiers2(k)\n"
			res+=10+strlen(tabF2[k]);
		}

		return res;
	}
	if (j== m+1) {
		//On détruit les lignes en trop
		if (i==n-1) {
			strcat(inst, "d %d\n", n);
			return 10;
		} else {
			strcat(inst, "D %d %d\n",i , (n-i));
			return 15;
		}
	}
	/*cas général*/
	//ajout ATTENTION L'AJOUT SE FAIT EN I-1 (pour se mettre "avant" la ligne i)
	res = B(i,j+1, tabF1, tabF2, inst) + 10 + strlen(tabF2[j]); 
	sprintf(instTmp,"+ %d\n",i-1);
	strcat(instTmp, tabF2[j]);
	strcat(instTmp, "\n");
	 // substitution
	int aux = B(i+1,j+1, tabF1, tabF2, inst) + c(tabF1[i],tabF2[j]);
	if (aux < res) {
		sprintf(instTmp,"= %d\n",i);
		strcat(instTmp, tabF2[j]);
		strcat(instTmp, "\n");
		res = aux;
	}
	// destruction unique
	aux = B(i+1,j, tabF1, tabF2, inst) +10; 
	if (aux < res) {
		sprintf(instTmp, "d %d\n", i);
		res = aux;
	}
	//destruction multiple,on doit ester pour chaque valeur de k possible
	int k;
	for (int l = 2; l <= n - i; k++ ) {
		aux = B(i + l,j, tabF1, tabF2, inst) +15; 
		if (auxDestMult < res) {
			sprintf(instTmp,"D %d %d\n", i, i+l);
			res = aux;
		}
	}
	tab[i][j] = res;
	strcat(inst,instTmp);
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

void writePatch(char *inst, char* nomFichier) {
	FILE* patch = fopen(nomFichier,"w");
	fprintf(patch, inst);
	fclose(patch);
}
/*TODO: 
 * - Gerer la destruction multiple : DONE
 * - Ajouter la génération des lignes de commandes dans le patch, 
 *	pour cela on va remplir un char* sous la forme suivante: "instr1 \n instr2 \n ..." : DONE
 * - generer le patch :DONE
 * - Maintenant c'est phase de débug de porc <3<3<3 (doit y avoir des fautes de merde de partout)
 */

