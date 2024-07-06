#pragma once
#include "transport_catalogue.h"
#include <iostream>

void query_(TransportCatalogue& catalogue, std::string_view str, std::ostream& output = std::cout);
void output_(TransportCatalogue& catalogue, std::istream& input = std::cin, std::ostream& output = std::cout);
