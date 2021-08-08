#include "transport_catalogue.h"
#include "input_reader.h"
#include "stat_reader.h"
#include <iostream>
#include <sstream>
#include <fstream>

int main() {
    TransportCatalogue tc;
    ifstream in("C:\\Users\\Anna\\source\\repos\\Transport\\Transport\\text.txt");
    ofstream out("test.txt");
    FillCatalogue(tc, in);
    OutpuInfo(tc, in, cout);
    return 0;
}