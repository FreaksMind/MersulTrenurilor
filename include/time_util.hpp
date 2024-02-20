#include <ctime>
#include <string>
#include <iostream>

//return 1: daca t1 este mai mare;
bool compareTimeTm(struct tm t1, struct tm t2){
    
    struct tm* t1c = (struct tm*) malloc(sizeof(struct tm));
    struct tm* t2c = (struct tm*) malloc(sizeof(struct tm));

    memcpy(t1c, &t1, sizeof(struct tm));
    memcpy(t2c, &t2, sizeof(struct tm));

    //std::cout << "test: " << t1c->tm_hour << '\n';
    //std::cout << "test: " << t2c->tm_hour << '\n';

    time_t tt1 = mktime(t1c);
    time_t tt2 = mktime(t2c);
    int diff = difftime(tt1,tt2);
    //std::cout << ctime(&tt1) << " " << ctime(&tt2) << '\n';
    //std::cout << "diff: " << diff << '\n';
    if(diff > 0){
        return 1;
    }
    return 0;
}

struct tm convertStringToTm(std::string ceas){
    time_t curr_time;
	curr_time = time(NULL);
    struct tm* time = localtime(&curr_time);
    //std::cout << asctime(time) << '\n';
    int i = ceas.find(":");
    std::string temp = ceas;
    std::string ora;
    std::string minute;
    if(i != std::string::npos){
        ora = ceas.substr(0, i);
        minute = temp.substr(i+1, temp.size()-i);
    }
    time->tm_min = atoi(minute.c_str());
    time->tm_hour = atoi(ora.c_str());
    time->tm_sec = 0;

    //std::cout << ora << "x " << minute << "x" <<'\n';
    //std::cout << asctime(time) << '\n';

    return *time;
}

std::string getCurrentHM(){
    std::string response;
    time_t curr_time;
    curr_time = time(NULL);
    struct tm* str = localtime(&curr_time);
    std::string ora = std::to_string(str->tm_hour);
    std::string minute = std::to_string(str->tm_min);
    response += ora + ":" + minute;
    //std::cout << "respose: " << response << '\n';   
    //delete str;
    return response;
}

std::string t_addDelay(int delay, std::string ora){
    //std::cout << ora << " + " << delay << '\n';
    struct tm time = convertStringToTm(ora);
    if(time.tm_min+delay > 60){
        time.tm_hour += 1;
        delay -= (60-time.tm_min);
        time.tm_min = 0;
        time.tm_min += delay;
        time.tm_sec = 0;
    } else if (time.tm_min+delay < 60){
        time.tm_min += delay;
        time.tm_sec = 0;
    } else {
        time.tm_hour += 1;
        time.tm_min = 0;
        time.tm_sec = 0;
    }
    //std::cout << asctime(time) << '\n';
}