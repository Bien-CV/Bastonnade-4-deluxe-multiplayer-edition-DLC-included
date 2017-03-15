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
#define DEFAULT_MESSAGE (char*)"Connexion au serveur Bastonnade réussie !\n"
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
void showAddrList(char ** list );
void showHostent(hostent* h);
void showAliases(char ** list );
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
	int len=strlen(clientString);
	for (int i=0;i<len;i++){
		if ( clientString[i] == 'e' ) clientString[i]='3';
		if ( clientString[i] == 'E' ) clientString[i]='3';
		if ( clientString[i] == 'T' ) clientString[i]='7';
		if ( clientString[i] == 't' ) clientString[i]='7';
		if ( clientString[i] == 'L' ) clientString[i]='1';
		if ( clientString[i] == 'l' ) clientString[i]='1';
		if ( clientString[i] == 's' ) clientString[i]='5';
		if ( clientString[i] == 'S' ) clientString[i]='5';
		if ( clientString[i] == 'a' ) clientString[i]='4';
		if ( clientString[i] == 'A' ) clientString[i]='4';
		
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
	//printf("Argc = %d , argv1 = %s",argc,argv[1]);
	//printf("strcmp argv1 client:%d\n",strcmp(argv[1],"client"));
	
	if ( ( argc == 2 ) && ( 0 == strcmp(argv[1],"client")) ){
		mainClient(argc, argv);
	}else if( ( argc == 2 ) && ( 0 == strcmp(argv[1],"serveur")) ){
		if DEBUG puts("Launching server...\n");
		mainServer(argc, argv);
	}else{
		menu(argc, argv);
	}
	
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

int mainClient(int argc, char **argv) {
	puts("Usage : >telnet localhost 5000 \n Ensuite, écrivez leetspeak et appuyez  entrée.\n Pour quitter telnet, saisissez ctrl + alt-gr + ] à vide, telnet> s'affichera devant votre curseur, à cet instant, saisissez \"quit\" puis appuyez sur entrée");
	
	
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
		//puts("Allo ?");
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
                    printf("Le client %d:%s envoie : %s \n",sd,inet_ntoa(address.sin_addr), buffer);
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
