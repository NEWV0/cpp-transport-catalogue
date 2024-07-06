#pragma once
#include <deque>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
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
};

struct Bus {
    std::string name_;
    std::deque<const Stop*> stops_;
};

struct BusStatistics {
    int stops_count;
    int unique_stops_count;
    double route_length;
};

class TransportCatalogue {
public:
    void add_bus(Bus&& bus);
    void add_stop(Stop&& stop);

    const Bus* get_bus(std::string_view bus_name);
    const Stop* get_stop(std::string_view stop_name);

    BusStatistics get_bus_statistics(const Bus* bus);
    std::vector<std::string> get_buses_by_stop(std::string_view stop_name);

private:
    std::deque<Bus> buses_;
    std::deque<Stop> stops_;
    std::unordered_map<std::string_view, const Stop*> stop_name_to_stop_;
    std::unordered_map<std::string_view, const Bus*> bus_name_to_bus_;
    std::unordered_map<std::string_view, std::vector<std::string>> stop_to_buses_map_;
};
