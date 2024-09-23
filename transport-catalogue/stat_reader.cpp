#include "stat_reader.h"
#include <iostream>
#include <unordered_set>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>



void query_bus(TransportCatalogue& catalogue, std::string_view str, std::ostream& output) {
    auto entry = 4;
    str = str.substr(entry);
    
    BusStats stats = catalogue.get_bus_stats(str); 
    if (!stats.name.empty()) {
        output << "Bus " << stats.name << ": "
               << stats.stops_count << " stops on route, "
               << stats.unique_stops_count << " unique stops, "
               << stats.route_length << " route length, " 
               << std::setprecision(6) << stats.curvature
               << " curvature" << std::endl;
    } else {      
        output << "Bus " << str << ": not found" << std::endl;
    }  
}


void query_stop(TransportCatalogue& catalogue, std::string_view stop_name, std::ostream& output) {
    auto entry = 5;
    stop_name = stop_name.substr(entry);
    std::unordered_set<const Bus*> unique_buses;      
    std::vector<std::string> bus_name_v;
    
    Stop* stop = catalogue.get_stop(stop_name);
    
    if (stop != nullptr) {
        unique_buses = catalogue.get_unique_buses_for_stop(stop);
        
        if(unique_buses.empty()){
            output << "Stop " << stop_name << ": no buses" << std::endl;
        } else {
            output << "Stop " << stop_name << ": buses ";
 
            for (const Bus* _bus : unique_buses) {
                bus_name_v.push_back(_bus->name_);
            }
 
            std::sort(bus_name_v.begin(), bus_name_v.end());         
 
            for (const std::string& _bus_name : bus_name_v) {
                output << _bus_name << " ";
            }
            output << std::endl;
        }        
    } else {      
        output << "Stop " << stop_name << ": not found" << std::endl;
    }
} 



void query_(TransportCatalogue& catalogue, std::string_view str) {
    if (str.substr(0, 3) == "Bus") {
        query_bus(catalogue, str, std::cout);
    } else if (str.substr(0, 4) == "Stop") {
        query_stop(catalogue, str, std::cout); 
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
