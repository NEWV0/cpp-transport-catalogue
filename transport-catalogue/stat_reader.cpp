#include "stat_reader.h"

void query_(TransportCatalogue& catalogue, std::string_view str, std::ostream& output) {
    if (str.substr(0, 3) == "Bus") {
        auto entry = 4;
        str = str.substr(entry);
        
        const Bus* bus = catalogue.get_bus(str);
        if (bus != nullptr) {
            BusStatistics stats = catalogue.get_bus_statistics(bus);
            output << "Bus " << bus->name_ << ": "
                   << stats.stops_count << " stops on route, "
                   << stats.unique_stops_count << " unique stops, "
                   << std::setprecision(6) << stats.route_length << " route length" << std::endl;
        } else {
            output << "Bus " << str << ": not found" << std::endl;
        }
    } else if (str.substr(0, 4) == "Stop") {
        auto entry = 5;
        str = str.substr(entry);

        const Stop* stop = catalogue.get_stop(str);
        if (stop == nullptr) {
            output << "Stop " << str << ": not found" << std::endl;
        } else {
            const auto& buses = catalogue.get_buses_by_stop(str);
            if (buses.empty()) {
                output << "Stop " << str << ": no buses" << std::endl;
            } else {
                output << "Stop " << str << ": buses";
                for (const auto& bus : buses) {
                    output << " " << bus;
                }
                output << std::endl;
            }
        }
    }
}

void output_(TransportCatalogue& catalogue, std::istream& input, std::ostream& output) {
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
        query_(catalogue, strr, output);
    }
}