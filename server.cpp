#include "utils.hpp"
#include "include/Command.h"
#include "include/TrainObject.h"
#include "include/time_util.hpp"
/* portul folosit */
#define PORT 6666

/* codul de eroare returnat de anumite apeluri */
extern int errno;

typedef struct thData{
	int idThread; //id-ul thread-ului tinut in evidenta de acest program
	int cl; //descriptorul intors de accept
}thData;
std::map<std::string, int> trainMap;
std::vector<TrainObject> trains;
pthread_mutex_t mutex;

static void *treat(void *); /* functia executata de fiecare thread ce realizeaza comunicarea cu clientii */
void raspunde(void *, char*);

using namespace std;

int main ()
{
  pthread_mutex_init(&mutex, NULL);
  struct sockaddr_in server;	// structura folosita de server
  struct sockaddr_in from;	
  int nr;		//mesajul primit de trimis la client 
  int sd;		//descriptorul de socket 
  int pid;
  pthread_t th[100];    //Identificatorii thread-urilor care se vor crea
	int i=0;
  std::cout << "Loaded: " << TrainObject::loadTrainData(trains, trainMap, "tren.xml") << " trains!" << '\n';
  /* crearea unui socket */
  if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
      perror ("[server]Eroare la socket().\n");
      return errno;
    }
  /* utilizarea optiunii SO_REUSEADDR */
  int on=1;
  setsockopt(sd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));
  
  /* pregatirea structurilor de date */
  bzero (&server, sizeof (server));
  bzero (&from, sizeof (from));
  
  /* umplem structura folosita de server */
  /* stabilirea familiei de socket-uri */
    server.sin_family = AF_INET;	
  /* acceptam orice adresa */
    server.sin_addr.s_addr = htons(INADDR_ANY);
  /* utilizam un port utilizator */
    server.sin_port = htons (PORT);
  
  /* atasam socketul */
  if (bind (sd, (struct sockaddr *) &server, sizeof (struct sockaddr)) == -1)
    {
      perror ("[server]Eroare la bind().\n");
      return errno;
    }

  /* punem serverul sa asculte daca vin clienti sa se conecteze */
  if (listen (sd, 2) == -1)
    {
      perror ("[server]Eroare la listen().\n");
      return errno;
    }
  /* servim in mod concurent clientii...folosind thread-uri */
  while (1)
    {
      int client;
      thData * td; //parametru functia executata de thread     
      socklen_t length = sizeof (from);

      printf ("[server]Asteptam la portul %d...\n",PORT);
      fflush (stdout);

      // client= malloc(sizeof(int));
      /* acceptam un client (stare blocanta pina la realizarea conexiunii) */
      if ( (client = accept (sd, (struct sockaddr *) &from, &length)) < 0)
	{
	  perror ("[server]Eroare la accept().\n");
	  continue;
	}
  //printf("%s:%s", inet_ntop(server.sin_addr), inet_ntop(from.sin_port));
        /* s-a realizat conexiunea, se astepta mesajul */
    
	// int idThread; //id-ul threaduluieck_fd(sd))
	// int cl; //descriptorul intors de accept

	td=(struct thData*)malloc(sizeof(struct thData));	
	td->idThread=i++;
	td->cl=client;

	pthread_create(&th[i], NULL, &treat, td);	      
				
	}//while
  pthread_mutex_destroy(&mutex);    
};				
static void *treat(void * arg)
{		
    std::string name;
		int logged = 1;
    struct thData tdL; 
		tdL= *((struct thData*)arg);	
		printf ("[thread]- %d - Asteptam mesajul...\n", tdL.idThread);
		fflush (stdout);		 
		pthread_detach(pthread_self());
    char buf[256] = {0};
    memset(buf, 0, sizeof(buf));
    while(read (tdL.cl, &buf, sizeof(buf)) > 0){
    Command* c = new Command(buf);
    switch(c->getCommandCode()){
      case LOGIN_COMMAND:{ //login
        if(c->argsCount() != 2){
          writeM(tdL.cl, WRONG_FORMAT_LOGIN);
        } else {
          if(logged == 0){
            name = login(tdL.cl, logged, c->getCommandArgs());
          } else {
            writeM(tdL.cl, ALREADY_LOGGED);
          }
        }
      break;
      }
      case REGISTER_COMMAND:{ //register
        if(c->argsCount() != 2){
          writeM(tdL.cl, WRONG_FORMAT_REGISTER);
        } else {
          if(logged == 0){
          name = registerUser(tdL.cl, c->getCommandArgs(), logged);
        } else {
          writeM(tdL.cl, CANNOT_USE);
          }
        }
      break;
      }
      case LOGOUT_COMMAND:{ //logout
      //if(c->argsCount != 0) {} ma mai gandesc daca accept logout bla blababal
        if(logged == 1){
          logged = 0;
          name = "";
          writeM(tdL.cl,SUCCESS_LOGOUT);
        } else {
          writeM(tdL.cl, NOT_LOGGED);
        }
          break;
        }
      case REQUEST_COMMAND   :{ //logout
        std::vector<std::string> args = c->getCommandArgs();
        if(args.size() != 2) {
          writeM(tdL.cl,WRONG_FORMAT_REQUEST);
          break;
        }
        if(logged == 1){
          if(args[0] == "schedule"){
            if(args[1] == "all"){
              const char* rep;
              std::string response;
              for(int i = 0; i < trains.size(); ++i){
                  response += "Train nr: " + std::to_string(i) + " - " + trains[i].getTrainName() + '\n';
                  for(int j = 0; j < trains[i].getTrainStations().size(); ++j){
                      auto statie = trains[i].getTrainStations()[j];
                      struct tm ora1 = convertStringToTm(getCurrentHM());
                      struct tm ora2 = convertStringToTm(std::get<1>(statie));
                      bool test = compareTimeTm(ora1,ora2);
                      if(test){
                          response += + "\33[32m[✓] \33[0m";
                      } else {
                          response += "\33[31m[✗] \33[0m";
                      }
                      response += "  ↳Statie: " + std::get<0>(statie) + " - Ora sosire: " + std::get<1>(statie) + ", Ora plecare: " + std::get<2>(statie) + '\n';
                  }

              }
              rep = response.c_str();                            
              writeM(tdL.cl, rep);
            } else {
              int index = TrainObject::getTrainID(trainMap,args[1]);
              if(index != -1){
                const char* rep;
                std::string response;
                    response += "Train nr: " + std::to_string(index) + " - " + trains[index].getTrainName() + '\n';
                    for(int j = 0; j < trains[index].getTrainStations().size(); ++j){
                        auto statie = trains[index].getTrainStations()[j];
                        struct tm ora1 = convertStringToTm(getCurrentHM());
                        struct tm ora2 = convertStringToTm(std::get<1>(statie));
                        bool test = compareTimeTm(ora1,ora2);
                        if(test){
                            response += + "\33[32m[✓] \33[0m";
                        } else {
                            response += "\33[31m[✗] \33[0m";
                        }
                        response += "  ↳Statie: " + std::get<0>(statie) + " - Ora sosire: " + std::get<1>(statie) + ", Ora plecare: " + std::get<2>(statie) + '\n';
                    }
                rep = response.c_str();  
                writeM(tdL.cl, rep);
              } else {
                writeM(tdL.cl, TRAIN_NO_EXISTS);
              }
            }
          } else {
            writeM(tdL.cl, CMD_UNKNOWN);
          }
        } else {
          writeM(tdL.cl, NOT_LOGGED);
        }
          break;
      }
      default:{ //nu exista
        std::cout << "unknown" <<'\n';
        writeM(tdL.cl, CMD_UNKNOWN);
      break;
      }
    };

      // if(logged == 1){
      //   raspunde((struct thData*)arg, buf);
      //   memset(buf, 0, sizeof(buf));
      // } else {
      //   login(logged);
      //   if (write (tdL.cl, NOTLOGGED, strlen(NOTLOGGED)) <= 0){
      //     printf("[Thread %d] ",tdL.idThread);
      //     perror ("[Thread]Eroare la write() catre client.\n");
      //     }
      //  }

       //endwhile
       delete c;
      memset(buf, 0, sizeof(buf));
    }
		close ((intptr_t)arg);
		return(NULL);	
  		
};
    
void raspunde(void *arg, char* buf)
{
  char test[] = "test";
        int i=0;
        int quit = 0;
	struct thData tdL; 
	tdL= *((struct thData*)arg);
        
                /*pregatim mesajul de raspuns */    
        printf("[Thread %d]Trimitem mesajul inapoi...\n",tdL.idThread);
                
                
                /* returnam mesajul clientului */
        if (write (tdL.cl, buf, strlen(buf)) <= 0)
            {
            printf("[Thread %d] ",tdL.idThread);
            perror ("[Thread]Eroare la write() catre client.\n");
            }
        else
            printf ("[Thread %d]Mesajul a fost trasmis cu succes.\n",tdL.idThread);
            memset(buf, 0, sizeof(buf));
}
