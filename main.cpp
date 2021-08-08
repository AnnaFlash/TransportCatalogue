#include "transport_catalogue.h"
#include "input_reader.h"
#include "stat_reader.h"
#include <iostream>
#include <sstream>
#include <fstream>

int main() {
    transport_catalogue::TransportCatalogue tc;
    std::ifstream in("C:\\Users\\Anna\\Source\\Repos\\TransportCatalogue\\test.txt");
    std::ofstream out("test.txt");
    input::FillCatalogue(tc, in);
    output::OutpuInfo(tc, in, std::cout);
    return 0;
}