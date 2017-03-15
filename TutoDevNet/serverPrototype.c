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

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>/* timeval */
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/types.h>
#include <unistd.h> /* close */
#include <netdb.h> /* gethostbyname */
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define closesocket(s) close(s)
typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
typedef struct in_addr IN_ADDR;


#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#define DEFAULT_MESSAGE (char*)"Hello world"
#define MAX_PENDING_CONNEXIONS 10
#define DEFAULT_PORT 5000
#define PORT_NUMBER 5000
#define TAILLE_MAX_NOM 256
#define ERROR_INT -1
#define DEBUG (true)
#define BUFFER_SIZE 1024
#define READ_BUFFER_SIZE 1024
#define MAX_CONNEXIONS 1000
#define EXIT_ACTIVATED (false)
#define TRUE   1
#define FALSE  0

#define max(x,y) ((x) > (y) ? (x) : (y))

typedef struct connexion{
	SOCKET sock;
	hostent* hostinfo;
	int port;
}connexion_t;

int mainClient(int argc, char **argv);
int mainServer(int argc, char **argv);
int getInt(void);
void menu(int argc, char **argv);
void renvoi(int sock);
void showAddrList(char ** list );
void showHostent(hostent* h);
void showAliases(char ** list );
void servListen();
//void closeAllSockets();
void recvString(int connexionNumber, char* stringBuffer);
//void sendString(int connexionNumber, char* string);
void connectToSpecificPort(const char* hostname, int port);
void connectTo(const char* hostname);
SOCKET newSocket();

int getInt(void){
	int tmpInt;
	if ( scanf ("%d",&tmpInt) == 1 ){
	return tmpInt;
	}else{
		printf("Plus d'une chaine a été saisie.");
		return ERROR_INT;
	}
}
void processClientString(char* clientString){
	//EMPTY
	return;
}
                    
void clearClientString(char* clientString){
	//NOT TESTED
	int len=strlen(clientString);
	for (int i=0;i<len;i++){
		clientString[0];
	}
	
	return;
}
                    
void menu(int argc, char **argv){
	printf("-Menu-\n");
	printf("0 - Quitter\n");
	printf("1 - Serveur\n");
	printf("2 - Client\n");
	int intInput=getInt();
	if ( intInput == 1 ){
		//call server
        if DEBUG puts("Launching server...\n");
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
	if DEBUG showHostent(hostinfo);
	if (hostinfo == NULL) /* l'hôte n'existe pas */
	{
		fprintf (stderr, "Unknown host %s.\n", hostname);
		exit(EXIT_FAILURE);
	}

	sin.sin_addr = *(IN_ADDR *) hostinfo->h_addr; /* l'adresse se trouve dans le champ h_addr de la structure hostinfo */
	sin.sin_port = htons(DEFAULT_PORT); /* on utilise htons pour le port */
	sin.sin_family = AF_INET;
	//if DEBUG printf("sock : %d \n&sin : %d size SOCKADDR :  %u\n",sock, &sin, sizeof(SOCKADDR) );
	
	if(connect(sock,(SOCKADDR *) &sin, sizeof(SOCKADDR)) == SOCKET_ERROR)
	{
		perror("connect()");
		exit(errno);
	}
	return;
}

void connectToSpecificPort(const char* hostname, int port){
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
	
	//connexions[connexionCount].sock=sock;
	//connexions[connexionCount].hostinfo=hostinfo;
	//connexions[connexionCount].port=port;
	
}
void sendString(connexion target, char* string){
	//TODO: interdire les messages au dessus de 1024 chars
	
	if(send(target.sock, string, strlen(string), 0) < 0)
	{
		perror("send()");
		exit(errno);
	}
}

//WARNING: Attention, recv est bloquant.
void recvString(connexion co, char* stringBuffer){
	char buffer[1024];
	int n = 0;

	if((n = recv(co.sock, buffer, sizeof buffer - 1, 0)) < 0)
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
	
    char sentString[] = {'p','r','o','u','t'};
	//sendString(serverConnexion,sentString);
	
	
	return EXIT_SUCCESS;
}


/*
void closeAllSockets(){
	for ( int i=0; i<MAX_CONNEXIONS;i++){
		if(connexions[i].port != 0 ){
		closesocket(connexions[i].sock);
		}
	}
}
*/

void servListen(){
	if DEBUG printf("Listening...\n");
	SOCKADDR_IN sin = { 0 };
	SOCKET sock=0;
	
	// nous sommes un serveur, nous acceptons n'importe quelle adresse
	sin.sin_addr.s_addr = htonl(INADDR_ANY); 
	
	sin.sin_family = AF_INET;
	
	sin.sin_port = htons(DEFAULT_PORT);
	
	if(bind (sock, (SOCKADDR *) &sin, sizeof sin) == SOCKET_ERROR)
	{
    if EXIT_ACTIVATED perror("bind()");
    if EXIT_ACTIVATED exit(errno);
    return;
	}
	
	if(listen(sock, 5) == SOCKET_ERROR)
	{	
		if EXIT_ACTIVATED perror("listen()");
		if EXIT_ACTIVATED exit(errno);
		return;
	}
	//TODO fermeture de sock ?
	
	SOCKADDR_IN csin = { 0 };
	SOCKET csock;
	
	unsigned int sinsize = sizeof csin;
	
	csock = accept(sock, (SOCKADDR *)&csin, &sinsize);
	
	if(csock == INVALID_SOCKET)
	{
		if EXIT_ACTIVATED perror("accept()");
		if EXIT_ACTIVATED exit(errno);
		return;
	}
	//connexions[connexionCount].sock=csock;
}


//NOT USED
/* longueur_adresse_courante est la longueur d'adresse courante d'un client */
/* ptr_hote = les infos recuperees sur la machine hote */ 
/* ptr_service = les infos recuperees sur le service de la machine */
/* machine = nom de la machine locale */
int mainServer2(int argc, char **argv) {
	//int bind(int sockfd, struct sockaddr *my_addr, socklen_t addrlen);
	//int accept(int sock, struct sockaddr *adresse, socklen_t *longueur); //return a socket
	//int recv(int s, void *buf, int len, unsigned int flags);
	//int select(int n, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout); 
	if DEBUG puts("Entering server main");
	int selectReturnValue; // Si pas d'erreur, est le nombre total de fd encore présents.
	
	SOCKET sock=newSocket();
	SOCKET largestSock= sock;
    fd_set readfs;// ensemble des sockets surveillés pour lire des données
    

    
	while(true){
		FD_ZERO(&readfs);//reset l'ensemble readfs car select() modifie les fd à chaque appel
        FD_SET(sock, &readfs);//Ajoute sock à l'ensemble readfs
		if((selectReturnValue = select(largestSock + 1, &readfs, NULL, NULL, NULL)) < 0)//select ici
		{
		perror("select()");
		//peut renvoyer -1 avec errno positionné à EINTR ou EAGAIN (EWOULDBLOCK) ce qui ne relève pas d'une erreur. 
		//exit(errno);
		}
		
		if(FD_ISSET(sock, &readfs))
		{
		/* des données sont disponibles sur le socket */
		/* traitement des données */
		}
		
		//servListen();
	}
	
	//char stringBuffer [1024] ;
	//recvString(0, stringBuffer );
	
	//closeAllSockets();
	return EXIT_SUCCESS;
}

int mainServer(int argc , char *argv[])
{
    
    int sd; //socket descriptor
    int max_sd; //Plus grand socket descriptor
    int socketPrincipal;
    int addrlen;// tampon pour longueur d'adresse
    int new_socket; //tampon pour nouveau socket
    int client_socket[MAX_CONNEXIONS]; //Tableau contenant les sockets clients, si une valeur vaut 0 alors elle est libre.
    int max_clients = MAX_CONNEXIONS; // Nombre maximal de clients
    int activity;//Contient normalement le socket descripteur où de l'activité a été détectée
    int i;//compteur
    int valread;//Sortie du read()
    int opt = TRUE; // utilisé par setsockopt
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( DEFAULT_PORT );
    
    char buffer[BUFFER_SIZE];
      
    //Ensemble des sockets surveillés pour lire des données
    fd_set readfds;
      
    //message de bienvenue
    char *message = DEFAULT_MESSAGE;
  
    //initialisation de client_socket[]
    for (i = 0; i < MAX_CONNEXIONS; i++) 
    {
        client_socket[i] = 0;
    }
    
    socketPrincipal = newSocket();
  
    //Précise que le socket principal accepte les connexions multiples
    //Bonne pratique, mais pas nécessaire
    if( setsockopt(socketPrincipal, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0 )
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
  

    //bind à &adress sur le port par défaut
    if (bind(socketPrincipal, (struct sockaddr *)&address, sizeof(address))<0) 
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    printf("Ecoute en cours sur le port %d \n", DEFAULT_PORT);
     
    if (listen(socketPrincipal, MAX_PENDING_CONNEXIONS) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
      
    addrlen = sizeof(address);
    
    
    puts("En attente de connections ...");
     
    while(TRUE) 
    {
        //reset l'ensemble readfs car select() modifie les file descriptors ( fd ) à chaque appel
        FD_ZERO(&readfds);
  
        //On ajoute le socket principal à l'ensemble des fd destinés à être lu
        FD_SET(socketPrincipal, &readfds);
        max_sd = socketPrincipal;
         
        //Ajout des sockets auxilliaires
        for ( i = 0 ; i < max_clients ; i++) 
        {
            //socket descriptor
            sd = client_socket[i];
             
            //Si valide alors on l'ajoute à la liste des fd à lire
            if(sd > 0)
                FD_SET( sd , &readfds);
             
            //on redéfinit le plus grand fd, car il y en a besoin pour le select, c'est un paramètre à fournir.
            if(sd > max_sd)
                max_sd = sd;
        }
  
        //on attend un signe d'activité l'un des sockets , sans timeout
        activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);
    
        if ((activity < 0) && (errno!=EINTR)) 
        {
            printf("select error : activity<0");
        }
          
        //Si quelque chose s'est passé sur le socket principal, alors c'est une connexion entrante.
        if (FD_ISSET(socketPrincipal, &readfds)) 
        {
            if ((new_socket = accept(socketPrincipal, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
            {
                perror("accept : l.412 socket principal");
                exit(EXIT_FAILURE);
            }
          
            //inform user of socket number - used in send and receive commands
            printf("Nouvelle connection, le fd du socket est %d , l'ip est : %s , le port est : %d \n" , new_socket , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
        
            //Envoi du message de bienvenue
            if( send(new_socket, message, strlen(message), 0) != strlen(message) ) 
            {
                perror("send");
            }
              
            //puts("Message envoyé");
              
            //add new socket to array of sockets
            for (i = 0; i < max_clients; i++) 
            {
                //if position is empty
                if( client_socket[i] == 0 )
                {
                    client_socket[i] = new_socket;
                    printf("Client ajouté à la liste des sockets en position %d\n" , i);
                     
                    break;
                }
            }
        }
          
        //Si ce n'est pas une nouvelle connection sur le socket principal alors c'est une opération d'entrée ou sortie sur un socket quelqconque
        for (i = 0; i < max_clients; i++) 
        {
            sd = client_socket[i];
              
            if (FD_ISSET( sd , &readfds)) 
            {
                //Vérifie si c'était une fermeture de connection et lit le message entrant
                if ((valread = read( sd , buffer, READ_BUFFER_SIZE )) == 0)
                {
                    //Quelqu'un s'est déconnecté, affiche les détails
                    getpeername(sd , (struct sockaddr*)&address , (socklen_t*)&addrlen);
                    printf("Hote déconnecté , ip %s , port %d \n" , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
                      
                    //Ferme le socket et le marque comme étant libre ( 0 )
                    close( sd );
                    client_socket[i] = 0;
                }
                  
                //Opération sur la chaîne reçue
                else
                {
                    //assure que la chaîne est bien terminée
                    buffer[valread] = '\0';
                    
                    
                    //buffer contient maintenant la chaîne envoyée par le client
                    //On peut donc l'utiliser.
                    processClientString(buffer);
                    clearClientString(buffer);
                    
                    send(sd , buffer , strlen(buffer) , 0 );
                }
            }
        }
    }
      
    return 0;
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
