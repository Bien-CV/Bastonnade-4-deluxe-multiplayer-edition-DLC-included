/*
 ============================================================================
 Name        : ReseauM1.c
 Author      : 
 Version     :
 Copyright   : 
 Description : 
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
#include <unistd.h>/* close */
#include <linux/types.h>
#include <netdb.h>/* gethostbyname */
#include "main.h"
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>



#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define closesocket(s) close(s)
typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
typedef struct in_addr IN_ADDR;

#define DEFAULT_PORT 5000
#define PORT_NUMBER 5000
#define TAILLE_MAX_NOM 256
#define ERROR_INT -1
#define DEBUG 1
#define BUFFER_SIZE 9999
#define MAX_CONNEXIONS 1000

typedef struct sockaddr sockaddr;
typedef struct sockaddr_in sockaddr_in;
typedef struct hostent hostent;
typedef struct servent servent;

typedef struct connexion{
	SOCKET sock;
	hostent* hostinfo;
	int port;
}connexion_t;

connexion_t connexions[MAX_CONNEXIONS];

int connexionCount=0;

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
	printf("Bienvenue dans Bastonnade 4 deluxe multiplayer edition DLC included\n");
	menu(argc, argv);
	return EXIT_SUCCESS;
}

SOCKET newSocket(){
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock == INVALID_SOCKET)
	{
		perror("socket()");
		exit(errno);
	}
	return sock;
}

void connectTo(const char* hostname){
	SOCKET sock = newSocket();
	struct hostent *hostinfo = NULL;
	SOCKADDR_IN sin = { 0 }; /* initialise la structure avec des 0 */

	hostinfo = gethostbyname(hostname); /* on récupère les informations de l'hôte auquel on veut se connecter */
	if (hostinfo == NULL) /* l'hôte n'existe pas */
	{
		fprintf (stderr, "Unknown host %s.\n", hostname);
		exit(EXIT_FAILURE);
	}

	sin.sin_addr = *(IN_ADDR *) hostinfo->h_addr; /* l'adresse se trouve dans le champ h_addr de la structure hostinfo */
	sin.sin_port = htons(DEFAULT_PORT); /* on utilise htons pour le port */
	sin.sin_family = AF_INET;

	if(connect(sock,(SOCKADDR *) &sin, sizeof(SOCKADDR)) == SOCKET_ERROR)
	{
		perror("connect()");
		exit(errno);
	}
	return;
}

void connectTo(const char* hostname, int port){
	SOCKET sock = newSocket();
	struct hostent *hostinfo = NULL;
	SOCKADDR_IN sin = { 0 }; /* initialise la structure avec des 0 */

	hostinfo = gethostbyname(hostname); /* on récupère les informations de l'hôte auquel on veut se connecter */
	if (hostinfo == NULL) /* l'hôte n'existe pas */
	{
		fprintf (stderr, "Unknown host %s.\n", hostname);
		exit(EXIT_FAILURE);
	}

	sin.sin_addr = *(IN_ADDR *) hostinfo->h_addr; /* l'adresse se trouve dans le champ h_addr de la structure hostinfo */
	sin.sin_port = htons(port); /* on utilise htons pour le port */
	sin.sin_family = AF_INET;

	if(connect(sock,(SOCKADDR *) &sin, sizeof(SOCKADDR)) == SOCKET_ERROR)
	{
		perror("connect()");
		exit(errno);
	}
	
	connexions[connexionCount].sock=sock;
	connexions[connexionCount].hostinfo=hostinfo;
	connexions[connexionCount].port=port;
	
}
void sendString(int connexionNumber, char* string){
	//TODO: interdire les messages au dessus de 1024 chars
	if(send(connexions[connexionNumber].sock, string, strlen(string), 0) < 0)
	{
		perror("send()");
		exit(errno);
	}
}

//WARNING: Attention, recv est bloquant.
void recvString(int connexionNumber, char* stringBuffer){
	char buffer[1024]
	int n = 0;

	if((n = recv(connexions[connexionNumber].sock, buffer, sizeof buffer - 1, 0)) < 0)
	{
	perror("recv()");
	exit(errno);
	}

	buffer[n] = '\0';
}
int mainClient(int argc, char **argv) {
	//int send(int s, const void *msg, size_t len, int flags);
	//int connect(int sockfd, struct sockaddr *serv_addr, socklen_t addrlen);
	connectTo("localhost");
	sendString(0,"prout");
	
	closeAllSockets();
	return EXIT_SUCCESS;
}
void closeAllSockets(){
	for ( int i=0; i<MAX_CONNEXIONS;i++){
		if(connexions[i] != NULL ){
		closesocket(connexions[i].sock);
		}
	}
}
	
void servListen(){
	SOCKADDR_IN sin = { 0 };
	SOCKET sock;
	
	// nous sommes un serveur, nous acceptons n'importe quelle adresse
	sin.sin_addr.s_addr = htonl(INADDR_ANY); 
	
	sin.sin_family = AF_INET;
	
	sin.sin_port = htons(DEFAULT_PORT);
	
	if(bind (sock, (SOCKADDR *) &sin, sizeof sin) == SOCKET_ERROR)
	{
    perror("bind()");
    exit(errno);
	}
	
	if(listen(sock, 5) == SOCKET_ERROR)
	{	
		perror("listen()");
		exit(errno);
	}
	//TODO fermeture de sock ?
	
	SOCKADDR_IN csin = { 0 };
	SOCKET csock;
	
	int sinsize = sizeof csin;
	
	csock = accept(sock, (SOCKADDR *)&csin, &sinsize);
	
	if(csock == INVALID_SOCKET)
	{
		perror("accept()");
		exit(errno);
	}
	connexion[connexionCount].sock=csock;
}

/* longueur_adresse_courante est la longueur d'adresse courante d'un client */
/* ptr_hote = les infos recuperees sur la machine hote */ 
/* ptr_service = les infos recuperees sur le service de la machine */
/* machine = nom de la machine locale */
int mainServer(int argc, char **argv) {
	//int bind(int sockfd, struct sockaddr *my_addr, socklen_t addrlen);
	//int accept(int sock, struct sockaddr *adresse, socklen_t *longueur); //return a socket
	//int recv(int s, void *buf, int len, unsigned int flags);
	
	while(true){
		//accept ou bind
		servListen();
	}
	
	char[1024]stringBuffer;
	recvString(0,stringBuffer);
	
	closeAllSockets();
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
