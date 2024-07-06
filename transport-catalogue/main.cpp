#include "transport_catalogue.h"
#include "input_reader.h"
#include "stat_reader.h"
#include <iostream>

using namespace std;

int main() {
    TransportCatalogue catalogue;
    input_(catalogue, cin);
    output_(catalogue, cin, cout);
}