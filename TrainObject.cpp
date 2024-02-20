#include "include/TrainObject.h"
#include <iostream>

//#define ARRAY_LENGTH (x) std::extent(decltype x)::value


TrainObject::TrainObject(){

}


TrainObject::~TrainObject(){
    
}

int TrainObject::loadTrainData(std::vector<TrainObject>& trains, std::map<std::string, int>& trainMap, const char* xmlFile){
    int trainsNumber = 0;
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(xmlFile);
    for (pugi::xml_node trainX : doc.child("Trains")) {
        TrainObject train = TrainObject();
        std::vector<std::tuple<std::string, std::string, std::string>> stations;
        for(pugi::xml_node station : trainX.child("Stations")){
            std::tuple<std::string, std::string, std::string> stationT = std::make_tuple(station.attribute("Statie").value(), station.attribute("Ora_sosire").value(), station.attribute("Ora_plecare").value());
            stations.push_back(stationT);
        }
        train.setTrainName(trainX.attribute("name").value()).addStations(stations);
        trains.push_back(train);
        trainMap.insert({trainX.attribute("name").value(), trainsNumber++});
    }
    return trainsNumber;
}

std::string TrainObject::getTrainName() const {

    return this->name;

}

std::vector<std::tuple<std::string, std::string, std::string>> TrainObject::getTrainStations() const{
    
    return this->stations;

}

TrainObject& TrainObject::setTrainName(std::string name){

    this->name = name;

    return *(this);
}
TrainObject& TrainObject::addStations(std::vector<std::tuple<std::string, std::string, std::string>>stations){
    this->stations = stations;

    return *(this);
}

int TrainObject::getTrainID(std::map<std::string, int>& trainMap, std::string trName){
    auto it = trainMap.find(trName);
    if(it != trainMap.end()){
        return trainMap.at(trName);
    }
    return -1;
}

std::string TrainObject::getTrainIDStations(std::vector<TrainObject>& trains, std::map<std::string, int> trainMap, std::string trName) {
    std::string response;
    int trainID;
    if((trainID = getTrainID(trainMap, trName)) >= 0){
        for(int j = 0; j < trains[trainID].getTrainStations().size(); ++j){
            auto statie = trains[trainID].getTrainStations()[j];
            response += "  ↳Statie: " + std::get<0>(statie) + " - Ora sosire: " + std::get<1>(statie) +", Ora plecare: " + std::get<2>(statie) + '\n';
        }
    } else {
        response += "The requested train does not exist!";
    }

    return response;
}

int TrainObject::getDelay() const{

    return this->delay;

}
TrainObject& TrainObject::addDelay(int delay){

    this->delay = delay;
    return *(this);
}