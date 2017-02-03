/*
 ============================================================================
 Name        : ReseauM1.c
 Author      : 
 Version     :
 Copyright   : 
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

/*----------------------------------------------
 Serveur à lancer avant le client
 * 
 * TODO : Utiliser select, qui a de meilleures performances pour les grands nombres de clients.
 * 
 ------------------------------------------------*/
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <linux/types.h>
#include <netdb.h>
#include "main.h"

#define PORT_NUMBER 5000
#define TAILLE_MAX_NOM 256
#define ERROR_INT -1
#define DEBUG 1
#define BUFFER_SIZE 9999

typedef struct sockaddr sockaddr;
typedef struct sockaddr_in sockaddr_in;
typedef struct hostent hostent;
typedef struct servent servent;

void renvoi(int sock) {
	char buffer[BUFFER_SIZE];
	int longueur;
	if ((longueur = read(sock, buffer, sizeof(buffer))) <= 0)
		return;
	printf("message lu : %s \n", buffer);
	printf("renvoi du message traite.\n");
	/* mise en attente du programme pour simuler un delai de transmission */
	sleep(3);
	write(sock, buffer, strlen(buffer) + 1);
	printf("message envoye. \n");
	return;
}

int getInt(void){
	int tmpInt;
	if ( scanf ("%d",&tmpInt) == 1 ){
	return tmpInt;
	}else{
		printf("Plus d'une chaine a été saisie.");
		return ERROR_INT;
	}
}

void menu(int argc, char **argv){
	printf("-Menu-\n");
	printf("0 - Quitter\n");
	printf("1 - Serveur\n");
	printf("2 - Client\n");
	int intInput=getInt();
	if ( intInput == 1 ){
		//call server
		mainServer(argc, argv);
	}else if( intInput == 2 ){
		//call client
		mainClient(argc, argv);
	}else if( intInput == 0 ){
		//call exit
		return;
	}else{
		printf("Erreur de saisie menu.");
	}
}

int main(int argc, char **argv){
	printf("Start of main\n");
	menu(argc, argv);
	return EXIT_SUCCESS;
}

int mainClient(int argc, char **argv) {
	int socket_descriptor,
	/* descripteur de socket */
	longueur;
	/* longueur d'un buffer utilisé */
	sockaddr_in adresse_locale;
	/* adresse de socket local */
	hostent * ptr_host;
	/* info sur une machine hote */
	servent * ptr_service;
	/* info sur service */
	char buffer[BUFFER_SIZE];
	char * prog;
	/* nom du programme */
	char * host;
	/* nom de la machine distante */
	char * mesg;
	/* message envoyé */
	if (argc != 3) {
		perror("usage : client <adresse-serveur> <message-a-transmettre>");
		exit(1);
	}
	prog = argv[0];
	host = argv[1];
	mesg = argv[2];
	printf("nom de l'executable : %s \n", prog);
	printf("adresse du serveur  : %s \n", host);
	printf("message envoye      : %s \n", mesg);
	if ((ptr_host = gethostbyname(host)) == NULL) {
		perror(
				"erreur : impossible de trouver le serveur a partir de son adresse.");
		exit(1);
	}
	/* copie caractere par caractere des infos de ptr_host vers adresse_locale */
	bcopy((char*) ptr_host->h_addr, (char*)&adresse_locale.sin_addr, ptr_host->h_length);
	adresse_locale.sin_family = AF_INET; /* ou ptr_host->h_addrtype; */
	adresse_locale.sin_port = htons(PORT_NUMBER);
	
	printf("numero de port pour la connexion au serveur : %d \n",
			ntohs(adresse_locale.sin_port));
	/* creation de la socket */
	if ((socket_descriptor = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror(
				"erreur : impossible de creer la socket de connexion avec le serveur.");
		exit(1);
	}
	/* tentative de connexion au serveur dont les infos sont dans adresse_locale */
	if ((connect(socket_descriptor, (sockaddr*) (&adresse_locale),
			sizeof(adresse_locale))) < 0) {
		perror("erreur : impossible de se connecter au serveur.");
		exit(1);
	}
	printf("connexion etablie avec le serveur. \n");
	printf("envoi d'un message au serveur. \n");
	/* envoi du message vers le serveur */
	if ((write(socket_descriptor, mesg, strlen(mesg))) < 0) {
		perror("erreur : impossible d'ecrire le message destine au serveur.");
		exit(1);
	}
	/* mise en attente du prgramme pour simuler un delai de transmission */
	sleep(3);
	printf("message envoye au serveur. \n");
	/* lecture de la reponse en provenance du serveur */
	while ((longueur = read(socket_descriptor, buffer, sizeof(buffer))) > 0) {
		printf("reponse du serveur : \n");
		write(1, buffer, longueur);
	}
	printf("\nfin de la reception.\n");
	close(socket_descriptor);
	printf("connexion avec le serveur fermee, fin du programme.\n");
	exit(0);
}

/* longueur_adresse_courante est la longueur d'adresse courante d'un client */
/* ptr_hote = les infos recuperees sur la machine hote */
/* ptr_service = les infos recuperees sur le service de la machine */
/* machine = nom de la machine locale */
int mainServer(int argc, char **argv) {
	
	
	int socket_descriptor,nouv_socket_descriptor;
	unsigned int longueur_adresse_courante;
	sockaddr_in adresse_locale,adresse_client_courant;
	hostent* ptr_hote;
	servent* ptr_service;
	char machine[TAILLE_MAX_NOM + 1];
	
	
	gethostname(machine, TAILLE_MAX_NOM);
	if ((ptr_hote = gethostbyname(machine)) == NULL) {
		perror(
				"erreur : impossible de trouver le serveur a partir de son nom.");
		exit(1);
	}

	/* copie de ptr_hote vers adresse_locale */
	bcopy((char*) ptr_hote->h_addr, (char*)&adresse_locale.sin_addr, ptr_hote->h_length);
	/* initialisation de la structure adresse_locale avec les infos recuperees */
	adresse_locale.sin_family = ptr_hote->h_addrtype;
	/* ou AF_INET */
	adresse_locale.sin_addr.s_addr = INADDR_ANY;
	/* ou AF_INET */

	 adresse_locale.sin_port = htons(PORT_NUMBER);

	printf("numero de port pour la connexion au serveur : %d \n",
			ntohs(adresse_locale.sin_port) /*ntohs(ptr_service->s_port)*/);
	/* creation de la socket */
	if ((socket_descriptor = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror(
				"erreur : impossible de creer la socket de connexion avec le client.");
		exit(1);
	}
	/* association du socket socket_descriptor à la structure d'adresse adresse_locale */
	if ((bind(socket_descriptor, (sockaddr*) (&adresse_locale),
			sizeof(adresse_locale))) < 0) {
		perror(
				"erreur : impossible de lier la socket a l'adresse de connexion.");
		exit(1);
	}
	/* initialisation de la file d'ecoute */
	listen(socket_descriptor, 5);
	/* attente des connexions et traitement des donnees recues */
	for (;;) {
		longueur_adresse_courante = sizeof(adresse_client_courant);
		/* adresse_client_courant sera renseignée par accept via les infos du connect */
		if ((nouv_socket_descriptor = accept(socket_descriptor,
				(sockaddr*) (&adresse_client_courant),
				&longueur_adresse_courante)) < 0) {
			perror(
					"erreur : impossible d'accepter la connexion avec le client.");
			exit(1);
		}
		/* traitement du message */
		printf("reception d'un message.\n");
		renvoi(nouv_socket_descriptor);
		close(nouv_socket_descriptor);
		showHostent(ptr_hote);
	}
	
	
	
	
	return EXIT_SUCCESS;
}

void showAliases(char ** list ){
	int compteur;
	if ( list[0]==0 || list[0][0]== 0 || list[0][0]== '\0') printf("N'a pas d'alias.\n");
	for (compteur=0;list[compteur]!=0;compteur++){
		printf("Alias %i:",compteur);
		printf("%s\n",list[compteur]);
	}
}

void showAddrList(char ** list ){
	int compteur;
	if ( list[0]==0 || list[0][0]== 0 || list[0][0]== '\0') printf("N'a pas d'adresses.\n");
	for (compteur=0;list[compteur]!=0;compteur++){
		printf("Addresse %i:",compteur);
		printf("%s\n",list[compteur]);
	}
}

void showHostent(hostent* h){
	//char    *h_name;       Nom officiel de l'hôte.   
	//char   **h_aliases;    Liste d'alias.            
	//int      h_addrtype;   Type d'adresse de l'hôte. 
	//int      h_length;     Longueur de l'adresse.    
	//char   **h_addr_list;  Liste d'adresses.        
	printf("-Affichage hostent-\n");
	printf("h_name:%s\n",h->h_name);
	showAliases(h->h_aliases);
	printf("h_addrtype:%d\n",h->h_addrtype);
	printf("h_length:%d\n",h->h_length);
	showAddrList(h->h_addr_list);
	
}
