#include "transport_catalogue.h"
#include "input_reader.h"
#include "stat_reader.h"
 
using namespace std;
 
int main() {     
    TransportCatalogue catalogue;
    input_(catalogue);
    output_(catalogue);
}