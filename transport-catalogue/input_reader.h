#pragma once
#include "transport_catalogue.h"
#include <iostream>

Stop split_stop(std::string_view str);
Bus split_bus(TransportCatalogue& catalogue, std::string_view str);
void input_(TransportCatalogue& catalogue, std::istream& input = std::cin);
