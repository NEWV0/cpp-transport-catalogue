#pragma once
#include "transport_catalogue.h"
#include <iostream>

Stop split_stop(std::string_view str);
std::vector<Distance> split_distance(std::string str, TransportCatalogue& catalogue);
Bus split_bus(TransportCatalogue& catalogue, std::string_view str);
void input_(TransportCatalogue& catalogue, std::istream& input);