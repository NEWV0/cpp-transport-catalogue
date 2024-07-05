#include "stat_reader.h"

void query_(TransportCatalogue& catalogue, std::string_view str) {
    if (str.substr(0, 3) == "Bus") {
        auto entry = 4;
        str = str.substr(entry);
        
        const Bus* bus = catalogue.get_bus(str);
        if (bus != nullptr) {
            std::cout << "Bus " << bus->name_ << ": "
                      << bus->stops_.size() << " stops on route, "
                      << catalogue.get_uniq_stops(bus) << " unique stops, "
                      << std::setprecision(6) << catalogue.get_length(bus) << " route length" << std::endl;
        } else {      
            std::cout << "Bus " << str << ": not found" << std::endl;
        }
    } else if (str.substr(0, 4) == "Stop") {
        auto entry = 5;
        str = str.substr(entry);

        const Stop* stop = catalogue.get_stop(str);
        if (stop == nullptr) {
            std::cout << "Stop " << str << ": not found" << std::endl;
        } else {
            auto buses = catalogue.get_buses_by_stop(str);
            if (buses.empty()) {
                std::cout << "Stop " << str << ": no buses" << std::endl;
            } else {
                std::cout << "Stop " << str << ": buses";
                for (const auto& bus : buses) {
                    std::cout << " " << bus;
                }
                std::cout << std::endl;
            }
        }
    }
}

void output_(TransportCatalogue& catalogue) {
    std::string count;
    std::getline(std::cin, count);
    
    std::string str;
    std::vector<std::string> query;
    auto amount = stoi(count);
    
    for (int i = 0; i < amount; ++i) {
        std::getline(std::cin, str);
        query.push_back(str);
    }
    
    for (auto& strr : query) {
        query_(catalogue, strr);
    }
}
