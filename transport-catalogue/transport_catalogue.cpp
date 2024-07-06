#include "transport_catalogue.h"
#include <algorithm>
#include <execution>

void TransportCatalogue::add_stop(Stop&& stop) {
    stops_.push_back(std::move(stop));
    stop_name_to_stop_[stops_.back().name_] = &stops_.back();
}

void TransportCatalogue::add_bus(Bus&& bus) {
    for (const Stop* stop : bus.stops_) {
        stop_to_buses_map_[stop->name_].push_back(bus.name_);
    }
    buses_.push_back(std::move(bus));
    bus_name_to_bus_[buses_.back().name_] = &buses_.back();
}

const Bus* TransportCatalogue::get_bus(std::string_view bus_name) {
    if (bus_name_to_bus_.count(bus_name) == 0) {
        return nullptr;
    }
    return bus_name_to_bus_.at(bus_name);
}

const Stop* TransportCatalogue::get_stop(std::string_view stop_name) {
    if (stop_name_to_stop_.count(stop_name) == 0) {
        return nullptr;
    }
    return stop_name_to_stop_.at(stop_name);
}

BusStatistics TransportCatalogue::get_bus_statistics(const Bus* bus) {
    int unique_stops = std::unordered_set<const Stop*>(bus->stops_.begin(), bus->stops_.end()).size();
    double route_length = transform_reduce(next(bus->stops_.begin()), 
                                           bus->stops_.end(), 
                                           bus->stops_.begin(),
                                           0.0,
                                           std::plus<>{},
                                           [](const Stop* lhs, const Stop* rhs) { 
                                               return ComputeDistance({lhs->latitude_, lhs->longitude_}, {rhs->latitude_, rhs->longitude_});});
    return {static_cast<int>(bus->stops_.size()), unique_stops, route_length};
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
