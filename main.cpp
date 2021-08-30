#include "request_handler.h"
#include "json_reader.h"
#include "svg.h"
#include "json.h"
#include <iostream>
#include <sstream>
#include <fstream>
using namespace std::literals;
using namespace svg;

int main() {
    transport_catalogue::TransportCatalogue tc;
    std::ifstream in("C:\\Users\\Anna\\source\\repos\\TransportCatalogue\\test.txt");
    std::ofstream out("testing.svg");
    input_json::InputJson(in, std::cout);
    //int i = 0;
    //for (int j = 1; j < 50; j++) {
    //    std::cout << i << " " << j << std::endl;
    //    i = (i + 1) % 2;
    //}
    return 0;
}