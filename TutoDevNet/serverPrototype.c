#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>/* timeval */
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/types.h>
#include <unistd.h> /* close */
#include <netdb.h> /* gethostbyname */
#include <math.h> /* random */ 
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define closesocket(s) close(s)
typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
typedef struct in_addr IN_ADDR;


//TODO Un système de logging de l'activité serveur

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>
#define AFFICHAGE_EXCENTRIQUE 1
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
#define MAX_NUMBER_OF_ROOMS 100
#define MAX_WELCOME_LEN 128
#define EXIT_ACTIVATED (false)
#define TRUE   1
#define FALSE  0

#define max(x,y) ((x) > (y) ? (x) : (y))

typedef struct connexion{
	SOCKET sock;
	hostent* hostinfo;
	int port;
}connexion_t;

typedef struct room{
	int p1; //Points de vie du joueur 1
	int p2;//Points de vie du joueur 2
	SOCKET idPlayer1;//Socket descriptor du joueur 1
	SOCKET idPlayer2;//Socket descriptor du joueur 2
	SOCKET currentPlayer;//Joueur dont c'est le tour de jouer
}room_t;

typedef room_t* lobby;

void initRoom(room_t* room, int maxHP){
	room->p1=maxHP;
	room->p2=maxHP;
	room->idPlayer1=0;
	room->idPlayer2=0;
	room->currentPlayer=0;	
}

void printRoom(room_t* room){
	printf("Print room : ");
	printf("p1=%d p2=%d ",room->p1,room->p2);
	printf("id1=%d id2=%d ",room->idPlayer1,room->idPlayer2);
}

void printRoomNL(room_t* room){
	printRoom(room);
	printf("\n");
}

bool isRoomEmpty(room_t* room){
	return ( (room->idPlayer1==0 ) && (room->idPlayer2==0) );
}
 
void clearRoom(room_t* room){
	room->p1=0;
	room->p2=0;
	room->idPlayer1=0;
	room->idPlayer2=0;
}



void sendTo(SOCKET client,const char* msg){
	send(client , msg , strlen(msg) , 0 );
}

void victoryMessage(SOCKET winner){
	sendTo(winner , "Vous avez gagné!");
}

char** str_split(char* a_str, const char a_delim)
{
    char** result    = 0;
    size_t count     = 0;
    char* tmp        = a_str;
    char* last_comma = 0;
    char delim[2];
    delim[0] = a_delim;
    delim[1] = 0;

    /* Count how many elements will be extracted. */
    while (*tmp)
    {
        if (a_delim == *tmp)
        {
            count++;
            last_comma = tmp;
        }
        tmp++;
    }

    /* Add space for trailing token. */
    count += last_comma < (a_str + strlen(a_str) - 1);

    /* Add space for terminating null string so caller
       knows where the list of returned strings ends. */
    count++;

    result = (char **)malloc(sizeof(char*) * count);

    if (result)
    {
        size_t idx  = 0;
        char* token = strtok(a_str, delim);

        while (token)
        {
            assert(idx < count);
            *(result + idx++) = strdup(token);
            token = strtok(0, delim);
        }
        assert(idx == count - 1);
        *(result + idx) = 0;
    }

    return result;
}

//On itère sur toutes rooms quand un joueur part afin de vérifier dans quelles rooms il était présent.
//Sous-optimal, devrait être fait régulièrement mais pas à très haute-fréquence
void playerLeft(SOCKET id, lobby lobby){
	int i;
	for ( i=0;i<MAX_NUMBER_OF_ROOMS;i++){
		if ( lobby[i].idPlayer1 == id ){
			if ( lobby[i].idPlayer2 == 0 ){
				printf("Room %d cleared.",i);
				clearRoom(&lobby[i]);
				return;
			}else{
				printf("Player %d surrendered over player %d.",lobby[i].idPlayer1, lobby[i].idPlayer2);
				victoryMessage(lobby[i].idPlayer2);
				printf("Room %d cleared.",i);
				clearRoom(&lobby[i]);
				return;
			}
		}else if ( lobby[i].idPlayer2 == id ){
			if ( lobby[i].idPlayer1 == 0 ){
				printf("Room %d cleared.",i);
				clearRoom(&lobby[i]);
				return;
			}else{
				printf("Player %d surrendered over player %d.",lobby[i].idPlayer2, lobby[i].idPlayer1);
				victoryMessage(lobby[i].idPlayer1);
				printf("Room %d cleared.",i);
				clearRoom(&lobby[i]);
				return;
			}
		}
	}
	
	return;

}


void initLobby(lobby lobby,int maxHP){
	int i;
	for ( i=0;i<MAX_NUMBER_OF_ROOMS;i++){
		initRoom(&(lobby[i]),maxHP);
	}
}
void printLobby(lobby lobby){
	int i;
	printf(" __________________________________________________________ \n");
	printf("|~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ LOBBY ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ |\n");
	printf("| Room number                 Room details                 |\n");
	for ( i=0;i<MAX_NUMBER_OF_ROOMS;i++){
		printf("|");
		printf("%11d | ",i);
		printRoom(&(lobby[i]));
		printf("         |");
		printf("\n");
	}
	printf("|__________________________________________________________|\n");
}
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
void leetspeaker(char* clientString){
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
SOCKET getOtherPlayer(SOCKET sd,int roomNumber,lobby lobby){
	if ( (lobby[roomNumber].idPlayer1) == sd ) {
		return lobby[roomNumber].idPlayer2;
	}
	if ( (lobby[roomNumber].idPlayer2) == sd ) {
		return lobby[roomNumber].idPlayer1;
	}
	return sd;
}
void giveRoomInfo(SOCKET sd,int roomNumber,lobby lobby){
	
	//sendTo("Print room : ");
	//sendTo("p1=%d p2=%d ",room->p1,room->p2);
	//sendTo("id1=%d id2=%d ",room->idPlayer1,room->idPlayer2);
}
void attaqueNormale(SOCKET sd,int roomNumber,lobby lobby){
	//appliquer dégats
	giveRoomInfo(sd,roomNumber,lobby);
	return;
}
void attaqueRisquee(SOCKET sd,int roomNumber,lobby lobby){
	return;
}
void attaqueSuicide(SOCKET sd,int roomNumber,lobby lobby){
	return;
}

void joinRoom(SOCKET sd, int roomNumber,lobby lobby){
	//si player1 vide alors sd devient player1
	//return 
	//si player2 vide alors sd devient player2
	
	
	return;
}
void processClientString(SOCKET sd, char* s,lobby lobby){
	char** instructions;
	int roomNumber;
	char* originalMessage;
	strcpy(originalMessage,s);
	instructions = str_split(s, ' ');
	if (instructions!=NULL)
    {
		if (strcmp(instructions[0],"R")==0){
				roomNumber=atoi(instructions[1]);
				if ( strcmp(instructions[2],"normale") == 0 ){
					attaqueNormale(sd,roomNumber,lobby);
				}else if ( strcmp(instructions[2],"risquée") == 0 ){
					attaqueRisquee(sd,roomNumber,lobby);
				}else if ( strcmp(instructions[2],"suicide") == 0 ){
					attaqueSuicide(sd,roomNumber,lobby);
				}
				
		}else if (strcmp(instructions[0],"/R")==0){
				roomNumber=atoi(instructions[1]);
				sendTo(getOtherPlayer(sd,roomNumber,lobby),originalMessage);
									
		}else if (strcmp(instructions[0],"join")==0){
				roomNumber=atoi(instructions[1]);
				joinRoom(sd,roomNumber,lobby);
		}
		
		
		
        int i;
        for (i = 0; instructions[i]; i++)
        {
            
            free(instructions[i]);
        }
    free(instructions);
    }
	return;
}

void print_image(FILE *fptr){
		char read_string[MAX_WELCOME_LEN];
	 
		while(fgets(read_string,sizeof(read_string),fptr) != NULL)
			printf("%s",read_string);
}

void printMenu(int choice){
	if ( choice == 0 ){
		printf("-Menu-\n");
		printf("1 - Jouer\n");
		printf("2 - Serveur\n");
		printf("0 - Quitter\n");
	}else if ( choice == 1 ){
		char *filename = (char *)"menu1.txt";
		FILE *fptr = NULL;
	 
		if((fptr = fopen(filename,"r")) == NULL)
		{
			fprintf(stderr,"error opening %s\n",filename);
			return;
		}
	 
		print_image(fptr);
	 
		fclose(fptr);
	}else if ( choice == 2 ){
		char *filename = (char *)"menu2.txt";
		FILE *fptr = NULL;
	 
		if((fptr = fopen(filename,"r")) == NULL)
		{
			fprintf(stderr,"error opening %s\n",filename);
			return;
		}
	 
		print_image(fptr);
	 
		fclose(fptr);
	
	}
	return;
 }
 
 void printEasterEgg(){
	
		char *filename = (char *)"easteregg";
		FILE *fptr = NULL;
	 
		if((fptr = fopen(filename,"r")) == NULL)
		{
			fprintf(stderr,"error opening %s\n",filename);
			return;
		}
	 
		print_image(fptr);
	 
		fclose(fptr);
	
	
	return;
 }
 
          
void menu(int argc, char **argv){
	printMenu(AFFICHAGE_EXCENTRIQUE);
	int intInput=getInt();
	if ( intInput == 2 ){
		//call server
        if DEBUG puts("Launching server...\n");
		mainServer(argc, argv);
	}else if( intInput == 1 ){
		//call client
		mainClient(argc, argv);
	}else if( intInput == 0 ){
		//call exit
		return;
	}else if( intInput == 3 ){
		//call easter egg
		printEasterEgg();
		return;
	}else{
		printf("Erreur de saisie menu.");
	}
}
 
 
 void printWelcomeMessage(int choice){
	if ( choice == 0 ){
		printf("Bienvenue dans Bastonnade 4 deluxe multiplayer edition DLC included\n");
	}else if ( choice == 1 ){
		char *filename = (char *)"welcome.txt";
		FILE *fptr = NULL;
	 
		if((fptr = fopen(filename,"r")) == NULL)
		{
			fprintf(stderr,"error opening %s\n",filename);
			return;
		}
	 
		print_image(fptr);
	 
		fclose(fptr);
	}else if ( choice == 2 ){
		char *filename = (char *)"welcome2.txt";
		FILE *fptr = NULL;
	 
		if((fptr = fopen(filename,"r")) == NULL)
		{
			fprintf(stderr,"error opening %s\n",filename);
			return;
		}
	 
		print_image(fptr);
	 
		fclose(fptr);
	
	}
	return;
 }
 
  
 
 
int main(int argc, char **argv){
	
	printWelcomeMessage(AFFICHAGE_EXCENTRIQUE);
	
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
	puts("\n\n\n\n\n\n\n\n\n\n");
	puts("\nPour jouer, utilisez la commande suivante :\n\ntelnet localhost 5000 \n\nlocalhost doit être l'adresse d'un serveur Bastonnade. \n\n\n ");
	puts("Ensuite, écrivez une commande et appuyez  entrée.\n\n ");
	puts("Pour rejoindre la room 1234 : join 1234\n ");
	puts("Pour executer une attaque normale dans la room 1234 : R 1234 normale\n ");
	puts("Pour executer une attaque risquée dans la room 1234 : R 1234 risquée\n ");
	puts("Pour executer une attaque suicide dans la room 1234 : R 1234 suicide\n ");
	puts("Pour parler dans la room 1234 : /R 1234 un message\n ");
	puts("Pour quitter telnet, saisissez ctrl + alt-gr + ] à vide, telnet> s'affichera devant votre curseur, à cet instant, saisissez \"quit\" puis appuyez sur entrée");
	
	
	return EXIT_SUCCESS;
}


int mainServer(int argc , char *argv[])
{    
    room_t lobby[MAX_NUMBER_OF_ROOMS];
    initLobby(lobby,0);
    printLobby(lobby);
    
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
    
    
    puts("En attente de connexions ...");
     
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
            printf("Nouvelle connexion, le fd du socket est %d , l'ip est : %s , le port est : %d \n" , new_socket , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
        
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
          
        //Si ce n'est pas une nouvelle connexion sur le socket principal alors c'est une opération d'entrée ou sortie sur un socket quelqconque
        for (i = 0; i < max_clients; i++) 
        {
            sd = client_socket[i];
              
            if (FD_ISSET( sd , &readfds)) 
            {
                //Vérifie si c'était une fermeture de connexion et lit le message entrant
                if ((valread = read( sd , buffer, READ_BUFFER_SIZE )) == 0)
                {
                    //Quelqu'un s'est déconnecté, affiche les détails
                    getpeername(sd , (struct sockaddr*)&address , (socklen_t*)&addrlen);
                    printf("Hote déconnecté , ip %s , port %d \n" , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
                    playerLeft(sd,lobby);
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
                    processClientString(sd,buffer,lobby);                    
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

/***********************
 *
 * Partie jeu
 *
 ***********************/

// Fonction pour changer le tour des joueurs
void change_player(room_t* room){
	if(room->currentPlayer==room->idPlayer1){
		room->currentPlayer = room->idPlayer2;
	} else {
		room->currentPlayer = room->idPlayer1;
	}
}

// Clean la chaine de caractères entrée par l'utilisateur
int clean_stdin() {
    while (getchar()!='\n');
    return 1;
}

// Fonction pour afficher les choix du joueur lors de son tour de jeu
void play(room_t* room){
	int r;
	// Affichage des choix
	// printf("0 - Passer son tour\n");
	// Dégats ingligés par les attaques à définir
	// Inflige des dégats entre 1 et 5
	printf("\n1 - Attaque normale\n");
	// Inflige des dégats entre 1 et 10
	printf("2 - Attaque risquée\n");
	// Inflige des dégats à l'adversaire et à soi même
	printf("3 - Attaque suicide\n");
	
	// Vérification de l'entrée utilisateur, si le choix fait parti de ceux proposés
	int choice =0;  
	char c;
	do {  
		printf("Choisir une action : ");
	} while (((scanf("%d%c", &choice, &c)!=2 || c!='\n') && clean_stdin()) || choice<1 || choice>3);
	
	// Tour de jeu du joueur 1
	if(room->currentPlayer==room->idPlayer1){
		if(choice==1) {
			r = rand()%6;	
			room->p2 = room->p2 - r;
		} else if(choice==2) {
			r = rand()%11;	
			room->p2 = room->p2 - r;
			room->p1 = room->p1 - rand()%6;
		} else if(choice==3) {
			r = rand()%16;	
			room->p2 = room->p2 - r;
			room->p1 = room->p1 - rand()%11;
		}			
	}

	// Tour de jeu du joueur 2 
	if(room->currentPlayer==room->idPlayer2){
		if(choice==1) {
			r = rand()%6;	
			room->p1 = room->p1 - r;
		}
		if(choice==2) {
			r = rand()%11;	
			room->p1 = room->p1 - r;
			room->p2 = room->p2 - rand()%6;
		}
		if(choice==3) {
			r = rand()%16;	
			room->p1 = room->p1 - r;
			room->p2 = room->p2 - rand()%10;
		}			
	}

	// Affichage des dégats
	if(r==0){
		printf("\nAttaque ratée !");
	} else {
		printf("\nDégats ingligés : -");
		printf("%d", r);
	}

	// Un des joueurs n'a plus de vie	
	if(room->p1<0){
		room->p1=0;
	}

	if(room->p2<0){
		room->p2=0;
	}

	printf("\nVie du joueur 1: ");
	printf("%d",room->p1);
	printf("\nVie du joueur 2: ");
	printf("%d",room->p2);
	printf("\n");

	// Fonction pour changer de joueur
	change_player(room);
}

int game(room_t* room){
	// Tant que les 2 joueurs ont des points de vie
	while(room->p1 > 0 || room->p2 > 0){
		//printf("\nAu tour du joueur ");
		//printf("%d",room->currentPlayer);		
		play(room);

		/*if(room->p1<=0){
			printf("\nLe gagnant est le joueur 2 !");
		} else if (room->p2<=0){
			printf("\nLe gagnant est le joueur 1 !");
		}*/

		// Relancer une partie ou quitter
		/*char answer;
		char c;
		printf("\nVoulez-vous-rejouer ? (o/n)");
		do {  
			printf("Veuillez entrer o ou n");
		} while (((scanf("%c", &answer, &c)!=2 || c!='\n') && clean_stdin()) || answer=="o" || answer=="n");
		if(answer=="o") {
			endGame=!endGame;
			p1=10;
			p2=10;
			player=1;
		}*/
	}
}
