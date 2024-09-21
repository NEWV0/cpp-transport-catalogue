#pragma once
#include <deque>
#include <string>
#include <vector>
#include <iomanip>
#include <iostream>
#include <unordered_set>
#include <unordered_map>
#include "geo.h"
 
struct Bus;
    
struct Stop {    
    std::string name_;
    double latitude_;
    double longitude_;
    
    std::vector<Bus*> buses_;
};
 
struct Bus { 
    std::string name_;
    std::vector<Stop*> stops_;
};
 
struct Distance {    
    const Stop* stopA;
    const Stop* stopB;
    int distance;
};
 
class DistanceHasher {
public:
    std::size_t operator()(const std::pair<const Stop*, const Stop*> pair_stops) const noexcept {
        auto hash_1 = static_cast<const void*>(pair_stops.first);
        auto hash_2 = static_cast<const void*>(pair_stops.second);
        return hasher_(hash_1) * 17 + hasher_(hash_2);
    }
private:
    std::hash<const void*> hasher_;
};
 
typedef  std::unordered_map<std::string_view , Stop*> StopMap;
typedef  std::unordered_map<std::string_view , Bus*> BusMap;
typedef  std::unordered_map<std::pair<const Stop*, const Stop*>, int, DistanceHasher> DistanceMap;
 
class TransportCatalogue {
public:   
    void add_bus(Bus&& bus);
    void add_stop(Stop&& stop);
    void add_distance(const Stop* stopA, const Stop* stopB, int distance);
    Bus* get_bus(std::string_view _bus_name);
    Stop* get_stop(std::string_view _stop_name);
 
    std::unordered_set<const Bus*> stop_get_uniq_buses(Stop* stop);    
    std::unordered_set<const Stop*> get_uniq_stops(Bus* bus);
    double get_length(Bus* bus);
    
    size_t get_distance_stop(const Stop* _start, const Stop* _finish);
    size_t get_distance_to_bus(Bus* _bus);
private:
    std::deque<Stop> stops_;
    StopMap stopname_to_stop;
    
    std::deque<Bus> buses_;
    BusMap busname_to_bus;
    
    DistanceMap distance_to_stop;
};