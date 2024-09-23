#include "input_reader.h"
#include "stat_reader.h"
#include <algorithm>
#include <iostream>
    
Stop split_stop(std::string str) {
    auto twopoint_pos = str.find(':');
    auto comma_pos = str.find(',');
    auto entry_length = 5;
    auto distance = 2;
    Stop _stop;
 
    _stop.name_ = str.substr(entry_length, 
                                  twopoint_pos - entry_length);
    _stop.latitude_ = stod(str.substr(twopoint_pos + distance, 
                                      comma_pos - twopoint_pos - distance));
    _stop.longitude_ = stod(str.substr(comma_pos + distance));
    
    return _stop;
}
 
std::vector<Distance> split_distance(std::string str, TransportCatalogue& catalogue) {
    
    std::vector<Distance> distances_;
    auto entry_length = 5;
    auto twopoint_pos = str.find(':');
    auto space = 2;
    
    std::string name_ = str.substr(entry_length, 
                                   twopoint_pos - entry_length);
    str = str.substr(str.find(',') + 1);
    str = str.substr(str.find(',') + space);
    
    while(str.find(',') != std::string::npos){
        
        
        int distance = stoi(str.substr(0, str.find('m')));
        std::string stop_dist_name = str.substr(str.find('m') + entry_length);
        stop_dist_name = stop_dist_name.substr(0, stop_dist_name.find(','));
        
        distances_.push_back({catalogue.get_stop(name_), catalogue.get_stop(stop_dist_name), distance});
        
        str = str.substr(str.find(',') + space);
    }
    std::string last_name = str.substr(str.find('m') + entry_length);
    int distance = stoi(str.substr(0, str.find('m')));
    
    distances_.push_back({catalogue.get_stop(name_), catalogue.get_stop(last_name), distance});
    return distances_;
}
 
Bus split_bus(TransportCatalogue& catalogue, std::string_view str) {
    auto entry_length = 4;
    auto distance = 2;
    auto twopoint_pos = str.find(':');
    Bus bus;
    bus.name_ = str.substr(entry_length, 
                           twopoint_pos - entry_length);
    
    str = str.substr(twopoint_pos + distance);
    
    auto more_pos = str.find('>');
    if (more_pos == std::string_view::npos) {
        auto tire_pos = str.find('-');
        
        while (tire_pos != std::string_view::npos) {
            bus.stops_.push_back(catalogue.get_stop(str.substr(0, tire_pos - 1)));
            
            str = str.substr(tire_pos + distance);
            tire_pos = str.find('-');
        }
        
        bus.stops_.push_back(catalogue.get_stop(str.substr(0, tire_pos - 1)));
        size_t size_ = bus.stops_.size() - 1;
        
        for (size_t i = size_; i > 0; i--) {
            bus.stops_.push_back(bus.stops_[i-1]);
        }
        
    } else {
        while (more_pos != std::string_view::npos) {
            bus.stops_.push_back(catalogue.get_stop(str.substr(0, more_pos - 1)));
            
            str = str.substr(more_pos + distance);
            more_pos = str.find('>');
        }
        
        bus.stops_.push_back(catalogue.get_stop(str.substr(0, more_pos - 1)));
    }
    return bus;
}
 
void input_(TransportCatalogue& catalogue, std::istream& input) {
    std::string count;
    std::getline(input, count);
    
    if (count != "") {
        std::string str;
        std::vector<std::string> buses;
        std::vector<std::string> stops;
        int amount = stoi(count);
        auto bus_distance = 3;
        
        for (int i = 0; i < amount; ++i) {
            std::getline(input, str);
            
            if (str != "") {
                auto space_pos = str.find_first_not_of(' ');
                str = str.substr(space_pos);

                if (str.substr(0, bus_distance) != "Bus") {
                    stops.push_back(str);
                } else {
                    buses.push_back(str);
                }
            }
        }

        for (auto stop : stops) {
            catalogue.add_stop(split_stop(stop));
        }

        for (auto distance_str : stops) {  
            for (const auto& distance_info : split_distance(distance_str, catalogue)) {
                catalogue.add_distance(distance_info.stop_a, distance_info.stop_b, distance_info.distance);
            }
        }

        for (auto bus : buses) {
            catalogue.add_bus(split_bus(catalogue, bus));
        }
    }
}

