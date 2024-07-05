#pragma once
#include <deque>
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>

#include <iomanip>
#include <iostream>

#include "geo.h"

struct Stop {    
    std::string name_;
    double latitude_;
    double longitude_;
    
    auto tuple() const {
        return tie(name_, latitude_, longitude_);
    }

    bool operator==(const Stop& other) const {
        return tuple() == other.tuple();
    }
    
    size_t hash() const {
        std::hash<double> hasher;
        auto hasher_latitude = hasher(latitude_);
        auto hasher_longitude = hasher(longitude_);
        return size_t(hasher_latitude * 17 + hasher_longitude);
    }  
};

struct Bus { 
    std::string name_;
    std::deque<const Stop*> stops_;
};

struct Bus_hash {
    size_t operator()(const Stop& stop) const {
        return size_t(stop.hash());
    }
};

struct Bus__hash {
    size_t operator()(const Stop* stop) const {
        return size_t((*stop).hash());
    }
};

class TransportCatalogue {
public:   
    void add_bus(Bus&& bus);
    void add_stop(Stop&& stop);

    const Bus* get_bus(std::string_view stop);
    const Stop* get_stop(std::string_view stop);
    
    int get_uniq_stops(const Bus* bus);
    double get_length(const Bus* bus);
    std::vector<std::string> get_buses_by_stop(std::string_view stop_name); // новый метод

private:
    std::deque<Bus> buses_;
    std::unordered_set<Stop, Bus_hash> stops_to_buses_;    
    std::unordered_map<std::string_view, std::vector<std::string>> stop_to_buses_map_; // новый индекс
};
