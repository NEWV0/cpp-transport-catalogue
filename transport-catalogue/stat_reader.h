#pragma once
#include "transport_catalogue.h"
#include <algorithm>
#include <vector>
#include <iostream>

void query_stop(TransportCatalogue& catalogue, std::string_view stop_name);
void query_(TransportCatalogue& catalogue, std::string_view str);
void output_(TransportCatalogue& catalogue);