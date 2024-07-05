#include "transport_catalogue.h"

#include <execution>
#include <algorithm>

void TransportCatalogue::add_stop(Stop&& stop) {
    stops_to_buses_.insert(std::move(stop)); 
}

void TransportCatalogue::add_bus(Bus&& bus) {
    for (const Stop* stop : bus.stops_) {
        stop_to_buses_map_[stop->name_].push_back(bus.name_);
    }
    buses_.push_back(std::move(bus)); 
}

const Bus* TransportCatalogue::get_bus(std::string_view bus) {   
    for (const auto& bus_ : buses_) {
        if (bus == bus_.name_) {
            return &bus_; 
        }
    }
    
    return {};
}

const Stop* TransportCatalogue::get_stop(std::string_view stop) {
    for (const auto& stop_ : stops_to_buses_) {
        if (stop == stop_.name_) {
            return &stop_;
        }
    }
    
    return {};
}

int TransportCatalogue::get_uniq_stops(const Bus* bus) {
    std::unordered_set<const Stop*, Bus__hash> unique_stops_;
    
    unique_stops_.insert(bus->stops_.begin(), 
                         bus->stops_.end());
    
    return int(unique_stops_.size());
}

double TransportCatalogue::get_length(const Bus* bus) {
    return transform_reduce(next(bus->stops_.begin()), 
                            bus->stops_.end(), 
                            bus->stops_.begin(),
                            0.0,
                            std::plus<>{},
                            [](const Stop* lhs, const Stop* rhs) { 
                                return ComputeDistance({(*lhs).latitude_, (*lhs).longitude_}, {(*rhs).latitude_, (*rhs).longitude_});});
}

std::vector<std::string> TransportCatalogue::get_buses_by_stop(std::string_view stop_name) {
    if (stop_to_buses_map_.count(stop_name) == 0) {
        return {};
    }
    auto& buses = stop_to_buses_map_[stop_name];
    std::sort(buses.begin(), buses.end());
    buses.erase(std::unique(buses.begin(), buses.end()), buses.end());
    return buses;
}
