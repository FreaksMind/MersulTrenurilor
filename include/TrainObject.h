#include <string>
#include <string.h>
#include <tuple>
#include <vector>
#include <map>
#include "pugixml.hpp"
//#include "time_util.hpp"

class TrainObject {

    std::string name;
    std::vector<std::tuple<std::string, std::string, std::string>> stations;
    int delay = 0;

    public:
    TrainObject();
    ~TrainObject();
    static int loadTrainData(std::vector<TrainObject>& trains, std::map<std::string, int>& trainMap, const char* xmlFile);
    std::string getTrainName() const;
    std::vector<std::tuple<std::string, std::string, std::string>> getTrainStations() const;
    int getDelay() const;
    TrainObject& setTrainName(std::string);
    TrainObject& addDelay(int delay);
    TrainObject& addStations(std::vector<std::tuple<std::string, std::string, std::string>>);
    static std::string getTrainIDStations(std::vector<TrainObject>&, std::map<std::string, int>, std::string);
    static int getTrainID(std::map<std::string, int>&, std::string);

};

