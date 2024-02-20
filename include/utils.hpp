#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <regex>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <iostream>
#include <sqlite3.h>
#include <dirent.h>


#define NOTLOGGED "YOU'RE NOT LOGGED IN!"
#define CMD_UNKNOWN "UNKNOWN COMMAND!"
#define TRAIN_NO_EXISTS "The requested train does not exist!"
#define WRONG_FORMAT_LOGIN "WRONG FORMAT! login <username> <password>"
#define WRONG_FORMAT_REQUEST "WRONG FORMAT! request schedule <all|train_id>"
#define WRONG_FORMAT_REGISTER "WRONG FORMAT! register <username> <password>"
#define WRONG_FORMAT_NAME "Please use only alphanumeric characters!"
#define WRONG_LENGHT_NAME "The maximum name length is 16 characters!"
#define WRONG_PASSWORD "Wrong password!"
#define USERS_EXISTS "This username is already used!"
#define SUCCESS_REGISTER "Successfully registered!"
#define SUCCESS_LOGIN "Successfully logged in!"
#define SUCCESS_LOGOUT "Successfully logged out!"
#define USER_NOT_EXIST "This username is not registered!"
#define NOT_LOGGED "You're not logged in!"

#define ALREADY_LOGGED "You're already logged in!"
#define CANNOT_USE "You can't use this while you are logged in!"
#define DIR_NFOUND "No such file or directory!"

#define DB_NAME "test.db"
#define NAME_FORMAT std::regex("^[a-zA-Z][a-zA-Z0-9]*$")

#define RED "\33[31m"
#define GREEN "\33[32m"
#define NORMAL "\33[0m"
char logo[] = "\33[31m ███▄ ▄███▓▓██   ██▓  ██████ ██▒   █▓ ███▄    █\n ▓██▒▀█▀ ██▒ ▒██  ██▒▒██    ▒▓██░   █▒ ██ ▀█   █\n ▓██    ▓██░  ▒██ ██░░ ▓██▄   ▓██  █▒░▓██  ▀█ ██▒\n▒██    ▒██   ░ ▐██▓░  ▒   ██▒ ▒██ █░░▓██▒  ▐▌██▒\n▒██▒   ░██▒  ░ ██▒▓░▒██████▒▒  ▒▀█░  ▒██░   ▓██░v0.1\n░ ▒░   ░  ░   ██▒▒▒ ▒ ▒▓▒ ▒ ░  ░ ▐░  ░ ▒░   ▒ ▒\n ░  ░      ░ ▓██ ░▒░ ░ ░▒  ░ ░  ░ ░░  ░ ░░   ░ ▒░\n░      ░    ▒ ▒ ░░  ░  ░  ░      ░░     ░   ░ ░\n       ░    ░ ░           ░       ░           ░\n             ░ ░                  ░             \n\33[0m";

void writeM(int fd, const char* buff){
    int size = strlen(buff);
    std::cout << size << '\n';
    if (write (fd, &size, sizeof(int)) <= 0){
          perror ("[Thread]Eroare la write() catre client.\n");
          exit(-1);
    }
    if (write (fd, buff, strlen(buff)) <= 0){
          perror ("[Thread]Eroare la write() catre client.\n");
          exit(-1);
    }
}

void readM(int fd, char* &buff){
    // int size = 0;
    // if (read (fd, &size, sizeof(int)) <= 0){
    //       perror ("[Thread]Eroare la write() catre client.\n");
    //       exit(-1);
    // }
    if (read (fd, &buff, sizeof(buff)) <= 0){
          perror ("[Thread]Eroare la write() catre client.\n");
          exit(-1);
    }
}

bool createUserFolder(const char* name){
    struct stat st = {0};
    char path[56] = {0};
    sprintf(path, "repositories/%s", name);
    path[strlen(path)]='\0';
    if(stat(path, &st) == -1){
        if(mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) < 0){    
            std::cout << "eroare: " << strerror(errno) << '\n';
            return false;
        }
    } else {
        return false;
    }

    return true;
}

void sendFile(int fd, const char* filename){

    

}

bool validateFormat(const char* buffer, std::regex r){
     if(regex_match(buffer, r)){
         return true;
     }
     return false;
}

bool checkPassword(const char* name, const char* pswd){

    sqlite3* db;
    int exiterr = 0;
    exiterr = sqlite3_open(DB_NAME, &db);

    if(exiterr){
        std::cout << "eroare" << sqlite3_errmsg(db) << '\n';
        exit(-1);
    } else {
        std::cout << "s-a deschis" << '\n';
    }


    sqlite3_stmt* stmt;
    char stm[] = "SELECT * FROM users WHERE NAME = ? and PASSWORD = ?;";
    //sprintf(stm, "SELECT * FROM users WHERE NAME = 'romania';");
    //stm[strlen(stm)-1] = '\0';

     if(sqlite3_prepare_v2(db, stm, -1, &stmt, NULL) != SQLITE_OK){
         std::cout << "eroare: " << sqlite3_errmsg(db) << '\n';
         sqlite3_close(db);
         sqlite3_finalize(stmt);
     }

     int ret_code = 0;

    sqlite3_bind_text(stmt, 1, name, strlen(name), SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, pswd, strlen(pswd), SQLITE_STATIC);

    if((ret_code =  sqlite3_step(stmt)) == SQLITE_ROW){
        sqlite3_close(db);
        sqlite3_finalize(stmt);
        return true;
    }
    //  while((ret_code =  sqlite3_step(stmt)) == SQLITE_ROW){
    //      std::cout << sqlite3_column_int(stmt,0) <<" " <<sqlite3_data_count(stmt) << '\n';
        
    //  }

     if(ret_code != SQLITE_DONE){
         std::cout << sqlite3_errmsg(db) << '\n';
     }
     sqlite3_finalize(stmt);
    sqlite3_close(db);
    return false;
}

bool checkUser(const char* name){
    
    sqlite3* db;
    int exiterr = 0;
    exiterr = sqlite3_open(DB_NAME, &db);

    if(exiterr){
        std::cout << "eroare" << sqlite3_errmsg(db) << '\n';
        exit(-1);
    } else {
        std::cout << "s-a deschis" << '\n';
    }


    sqlite3_stmt* stmt;
    char stm[] = "SELECT * FROM users WHERE NAME = ?;";
    //sprintf(stm, "SELECT * FROM users WHERE NAME = 'romania';");
    //stm[strlen(stm)-1] = '\0';

     if(sqlite3_prepare_v2(db, stm, -1, &stmt, NULL) != SQLITE_OK){
         std::cout << "eroare: " << sqlite3_errmsg(db) << '\n';
         sqlite3_close(db);
         sqlite3_finalize(stmt);
     }

     int ret_code = 0;

    sqlite3_bind_text(stmt, 1, name, strlen(name), SQLITE_STATIC);

    if((ret_code =  sqlite3_step(stmt)) == SQLITE_ROW){
        sqlite3_close(db);
        sqlite3_finalize(stmt);
        return true;
    }
    //  while((ret_code =  sqlite3_step(stmt)) == SQLITE_ROW){
    //      std::cout << sqlite3_column_int(stmt,0) <<" " <<sqlite3_data_count(stmt) << '\n';
        
    //  }

     if(ret_code != SQLITE_DONE){
         std::cout << sqlite3_errmsg(db) << '\n';
     }
     sqlite3_finalize(stmt);
    sqlite3_close(db);
    return false;
}

#pragma region Database
bool addUser(const char* name, const char* pswd){

    sqlite3* db;
    char* err = 0;
    char stm[128] = "INSERT INTO users VALUES(NULL,'";
    int exiterr;

    if(checkUser(name)){
        std::cout << "userul exista" << '\n';
        return false;
    } else {
    exiterr = sqlite3_open(DB_NAME, &db);

    if(exiterr){
        std::cout << "eroare" << sqlite3_errmsg(db) << '\n';
        exit(-1);
    } else {
        std::cout << "s-a deschis" << '\n';
    }

    sprintf(stm, "INSERT INTO users VALUES(NULL, '%s', '%s');", name, pswd);
    stm[strlen(stm)-1] = '\0';

    // memcpy(stm, name, strlen(name)); memcpy(stm, "','", 3); memcpy(stm, pswd, strlen(pswd)); memcpy(stm, "');", 3);
    // stm[strlen(stm)-1] = '\0'; 
    std::cout << stm << '\n';
    //stm = (char*)malloc((strlen(name) + strlen(pswd) + 30)*sizeof(char));
    //sprintf(stm, "INSERT INTO users VALUES(NULL, '%s', '%s');", name, pswd);
    exiterr = sqlite3_exec(db, stm, NULL, 0, &err);
    }

    if(exiterr != SQLITE_OK){
        std::cerr << "eroare executare stmt: " << sqlite3_errmsg(db) <<'\n';
        sqlite3_free(err);
        sqlite3_close(db);
        return false;
    } else {
        std::cout << "s-a executat stmt" <<'\n';
        sqlite3_close(db);
        return true;
    }
    sqlite3_close(db);
    return true;
}

#pragma endregion


std::string registerUser(int& cfd, std::vector<std::string> args, int& login){
    if(args[0].size() > 16) {
        writeM(cfd, WRONG_LENGHT_NAME);
        return "";
    }
    if(validateFormat(args[0].c_str(), NAME_FORMAT) == 0){
        writeM(cfd, WRONG_FORMAT_NAME);
        return "";
    }
    if(checkUser(args[0].c_str()) == true){
        writeM(cfd, USERS_EXISTS);
        return "";
    }
    addUser(args[0].c_str(), args[1].c_str());
    login = 1;
    writeM(cfd, SUCCESS_REGISTER);
    std::cout << "valoare createfolder: " << createUserFolder(args[0].c_str()) << '\n';
    return args[0];
}

std::string login(int& cfd, int& l, std::vector<std::string> args){

    if(checkUser(args[0].c_str()) != true){
        writeM(cfd, USER_NOT_EXIST);
        return "";
    }

    if(checkPassword(args[0].c_str(), args[1].c_str()) != true){
        writeM(cfd, WRONG_PASSWORD);
        return "";
    }
    writeM(cfd,SUCCESS_LOGIN);
    l = 1;

    return args[0];
}