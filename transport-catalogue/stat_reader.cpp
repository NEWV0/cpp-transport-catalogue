#include "stat_reader.h"
#include <iostream>
#include <unordered_set>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>

struct BusStats {
    std::string name;
    size_t stops_count;
    size_t unique_stops_count;
    double route_length;
    double curvature;
};

BusStats get_bus_stats(TransportCatalogue& catalogue, std::string_view bus_name) {
    Bus* bus = catalogue.get_bus(bus_name);
    if (bus != nullptr) {
        return {
            bus->name_,
            bus->stops_.size(),
            catalogue.get_uniq_stops(bus).size(),
            static_cast<double>(catalogue.get_distance_to_bus(bus)), // Явное преобразование
            static_cast<double>(catalogue.get_distance_to_bus(bus)) / catalogue.get_length(bus) // Явное преобразование
        };
    }
    return {}; // Возвращаем пустую структуру, если автобус не найден
}


void query_bus(TransportCatalogue& catalogue, std::string_view str) {
    auto entry = 4;
    str = str.substr(entry);
    
    BusStats stats = get_bus_stats(catalogue, str);
    if (!stats.name.empty()) {
        std::cout << "Bus " << stats.name << ": "
                  << stats.stops_count << " stops on route, "
                  << stats.unique_stops_count << " unique stops, "
                  << stats.route_length << " route length, " 
                  << std::setprecision(6) << stats.curvature
                  << " curvature" << std::endl;
    } else {      
        std::cout << "Bus " << str << ": not found" << std::endl;
    }  
}

void query_stop(TransportCatalogue& catalogue, std::string_view stop_name) {
    auto entry = 5;
    stop_name = stop_name.substr(entry);
    std::unordered_set<const Bus*> unique_buses;      
    std::vector<std::string> bus_name_v;
    
    Stop* stop = catalogue.get_stop(stop_name);
    
    if (stop != nullptr) {
        unique_buses = catalogue.stop_get_uniq_buses(stop);
        
        if(unique_buses.empty()){
            std::cout << "Stop " << stop_name << ": no buses" << std::endl;
        } else {
            std::cout << "Stop " << stop_name << ": buses ";
 
            for (const Bus* _bus : unique_buses) {
                bus_name_v.push_back(_bus->name_);
            }
 
            std::sort(bus_name_v.begin(), bus_name_v.end());         
 
            for (const std::string& _bus_name : bus_name_v) {
                std::cout << _bus_name << " ";
            }
            std::cout << std::endl;
        }        
    } else {      
        std::cout << "Stop " << stop_name << ": not found" << std::endl;
    }
}

void query_(TransportCatalogue& catalogue, std::string_view str) {
    if (str.substr(0, 3) == "Bus") {
        query_bus(catalogue, str);
    } else if (str.substr(0, 4) == "Stop") {
        query_stop(catalogue, str);
    } else {
        std::cout << "Error query" << std::endl;
    }
}

void output_(TransportCatalogue& catalogue, std::istream& input) {
    std::string count;
    std::getline(input, count);
    std::string str;
    std::vector<std::string> query;
    auto amount = stoi(count);
    
    for (int i = 0; i < amount; ++i) {
        std::getline(input, str);
        query.push_back(str);
    }
    
    for (auto& strr : query) {
        query_(catalogue, strr);
    }
}
