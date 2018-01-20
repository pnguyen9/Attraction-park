#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <pthread.h>
#include <semaphore.h>

#define MAX_VISITORS 10
#define MAX_WILL 20 // D�termine la volont� du visiteur � rester au parc
#define AMOUNT_CONTROLLED_ATTRACTIONS 4

// Variables utilis�es pour la gestion des attractions contr�l�es (s�maphores et dur�e des attractions)
sem_t controlledAttractions[AMOUNT_CONTROLLED_ATTRACTIONS];
int controlledAttractionsDuration[AMOUNT_CONTROLLED_ATTRACTIONS];

// D�claration des fonctions
void initControlledAttractions();

void destroyControlledAttractions();

void *newVisitor(void *c);

void visit(char *visitorName);

void walk(char *visitorName);

void freeAttraction(char *visitorName);

void controledAttraction(char *visitorName);


// Main
int main() {
	pthread_t visitor[MAX_VISITORS]; // Tableau de threads, chaque thread �tant un visiteur
	char name[MAX_VISITORS][50]; // Tableau de noms des visiteurs
	int i;

	srandom(time(NULL));
	initControlledAttractions(); // Initialisation des s�maphores

	printf("Le parc d'attraction ouvre ses portes\n");

	// Cr�ation des visiteurs
	for (i = 0 ; i < MAX_VISITORS ; ++i) {
		strcpy(name[i], "");
		sprintf(name[i], "Visiteur #%d", i);
		if (pthread_create(&visitor[i], NULL, newVisitor, name[i]) != 0) {
			printf("Erreur cr�ation visiteur\n");
			exit(1);
		}
	}

	// Attente de fin des threads
	for (i = 0 ; i < MAX_VISITORS ; ++i) {
		pthread_join(visitor[i], NULL);
	}

	destroyControlledAttractions(); // Destruction des s�maphores

	printf("Tous les visiteurs sont partis\n");
	
	return 0;
}

// D�finition des fonctions
// Fonction initialisant les s�maphores et les dur�es des attractions contr�l�es
void initControlledAttractions() {
	int i;
	for (i = 0 ; i < AMOUNT_CONTROLLED_ATTRACTIONS ; ++i) {
		// Initialisation d'un s�maphore pour chaque attraction contr�l�e
		// G�n�ration d'un entier al�atoire pour d�finir le nombre de places dans l'attraction
		sem_init(&controlledAttractions[i], 0, (rand() % (MAX_VISITORS/3) ) + 2);
		// G�n�ration d'un entier al�atoire pour la dur�e de chaque attraction control�e
		controlledAttractionsDuration[i] = rand() % 11 + 5;
	}
}

// Fonction d�truisant les s�maphores
void destroyControlledAttractions() {
	int i;
	for (i = 0 ; i < AMOUNT_CONTROLLED_ATTRACTIONS ; ++i) {
		sem_destroy(&controlledAttractions[i]);
	}
}

// Fonction appel�e lors de la cr�ation d'un thread visiteur
void *newVisitor(void *c) {
	char *visitorName = (char *) c;
	printf("%s rentre dans le parc d'attraction\n", visitorName);
	visit(visitorName);
	printf("%s sort du parc\n", visitorName);
	pthread_exit(NULL);
}

// Fonction simulant la visite du parc par un visiteur
void visit(char *visitorName) {
	// Certains visiteurs se fatiguent plus vite que d'autres
	const int TIRED = rand() % MAX_WILL;
	// Certains visiteurs se fatiguent en entrant dans le parc
	int tiredMeter = rand() % (MAX_WILL/5);
	int attractionVisited = 0;
	int attractionTypeToVisit; // 0 = attraction libre ; 1 = attraction contr�l�e

	while (tiredMeter < TIRED) {
		walk(visitorName);

		attractionTypeToVisit = rand() % 2;

		if (attractionTypeToVisit == 0) {
			freeAttraction(visitorName);
		} else {
			controledAttraction(visitorName);
		}

		// Mise � jour du niveau de fatigue
		++attractionVisited;
		tiredMeter = (rand() % MAX_WILL) + attractionVisited;
	}
}

void walk(char *visitorName) {
	printf("%s se promene dans les allees du parc\n", visitorName);
	sleep((rand() % 10) + 1);
}

void freeAttraction(char *visitorName) {
	printf("%s rentre dans une attraction libre\n", visitorName);
	sleep((rand() % 5) + 1);
	printf("%s sort d'une attraction libre\n", visitorName);
}

void controledAttraction(char *visitorName) {
	// Le visiteur choisit l'attraction contr�l�e � faire
	int attractionIndex = rand() % AMOUNT_CONTROLLED_ATTRACTIONS;
	printf("%s rentre dans la file d'attente de l'attraction controlee #%d\n", visitorName, attractionIndex);

	// P(SEM)
	sem_wait(&controlledAttractions[attractionIndex]);

	printf("%s rentre dans l'attraction controlee #%d\n", visitorName, attractionIndex);
	sleep(controlledAttractionsDuration[attractionIndex]);

	// V(SEM)
	sem_post(&controlledAttractions[attractionIndex]);
	printf("%s sort de l'attraction controlee #%d\n", visitorName, attractionIndex);
}
