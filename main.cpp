#include "json.h"
#include "json_reader.h"
#include "request_handler.h"
#include "transport_catalogue.h"
#include "transport_router.h"
#include <fstream>
using namespace std::literals;

/*
 * Если запустить приложение с ключом --render-only,
 * приложение выведет в cout только SVG карту маршрутов
 */
void PrintUsage(std::ostream& stream = std::cerr) {
    stream << "Usage: transport_catalogue [make_base|process_requests]\n"sv;
}


int main(int argc, char* argv[]) {
    if (argc != 2) {
        PrintUsage();
        return 1;
    }
    const std::string_view mode(argv[1]);
    transport_catalogue::TransportCatalogue base_data;
    router::TransportRouter router(base_data);
    renderer::MapRenderer map_renderer;
    serialization::Serialization serializator(base_data, map_renderer, router);
    service::RequestHandler request_handler(serializator, base_data, map_renderer, router);
    reader::JsonReader json_reader(serializator, base_data, map_renderer, request_handler, router);
    std::ofstream ofs("C://TransportCatalogue//myamswer33.json");
    if (mode == "make_base"sv) {
        json_reader.ReadBase();
    }
    else if (mode == "process_requests"sv) {
        json_reader.ReadRequests();
        json_reader.Answer(ofs);
    }
    else {
        PrintUsage();
        return 1;
    }
    system("pause");
}
