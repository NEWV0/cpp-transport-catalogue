#include "transport_catalogue.h"
#include <iostream>
#include <execution>
 
void TransportCatalogue::add_stop(Stop&& stop_) {
    stops_.push_back(std::move(stop_));
    Stop* stop_buf = &stops_.back();
    stopname_to_stop.insert(StopMap::value_type(stop_buf->name_, stop_buf));
}
 
void TransportCatalogue::add_bus(Bus&& bus_) {
    Bus* bus_buf;
    buses_.push_back(std::move(bus_)); 
    bus_buf = &buses_.back();
    busname_to_bus.insert(BusMap::value_type(bus_buf->name_, bus_buf));
 
    for (Stop* _stop : bus_buf->stops_) {
        _stop->buses_.push_back(bus_buf);
    }    
}
 
void TransportCatalogue::add_distance(const Stop* stopA, const Stop* stopB, int distance) {
    auto dist_pair = std::make_pair(stopA, stopB);
    distance_to_stop.insert(DistanceMap::value_type(dist_pair, distance));
}

 
Bus* TransportCatalogue::get_bus(std::string_view _bus_name) {   
    if(busname_to_bus.empty()){
        return nullptr;
    }
    try {
        return busname_to_bus.at(_bus_name);
    } catch(const std::out_of_range &e) {
        return nullptr;
    }
}
 
Stop* TransportCatalogue::get_stop(std::string_view _stop_name) {
    if(stopname_to_stop.empty()){
        return nullptr;
    }
    try {
        return stopname_to_stop.at(_stop_name);
    } catch(const std::out_of_range &e) {
        return nullptr;
    }
}
 
std::unordered_set<const Stop*> TransportCatalogue::get_uniq_stops(Bus* bus) {
    std::unordered_set<const Stop*> unique_stops_;
    
    unique_stops_.insert(bus->stops_.begin(), bus->stops_.end());
    
    return unique_stops_;
}
 
double TransportCatalogue::get_length(Bus* bus) {
    return transform_reduce(next(bus->stops_.begin()), 
                            bus->stops_.end(), 
                            bus->stops_.begin(),
                            0.0,
                            std::plus<>{},
                            [](const Stop* lhs, const Stop* rhs) { 
                                return ComputeDistance({(*lhs).latitude_,
                                                        (*lhs).longitude_}, {(*rhs).latitude_,
                                                                              (*rhs).longitude_});
                            });
}
 
std::unordered_set<const Bus*> TransportCatalogue::stop_get_uniq_buses(Stop* stop){    
    std::unordered_set<const Bus*> unique_stops_;    
      
    unique_stops_.insert(stop->buses_.begin(), stop->buses_.end());   
    
    return unique_stops_;
}
 
size_t TransportCatalogue::get_distance_stop(const Stop* _begin, const Stop* _finish) {
    if(distance_to_stop.empty()){
        return 0;
    }   
    try {
        auto dist_pair = std::make_pair(_begin, _finish);
        return distance_to_stop.at(dist_pair);
    } catch (const std::out_of_range &e) {
        try {
            auto dist_pair = std::make_pair(_finish, _begin);
            return distance_to_stop.at(dist_pair);
        } catch (const std::out_of_range &e) {
            return 0;    
        } 
    } 
}
 
size_t TransportCatalogue::get_distance_to_bus(Bus* _bus) {
    size_t distance = 0;
    auto stops_size = _bus->stops_.size() - 1;
    for (int i = 0; i < stops_size; i++) {
        distance += get_distance_stop(_bus->stops_[i], _bus->stops_[i+1]);
    }
    return distance;
}