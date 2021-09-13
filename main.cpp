//#include "json.h"
#include "json_reader.h"
#include "request_handler.h"
#include "transport_catalogue.h"
#include <fstream>
using namespace std::literals;

/*
 * Если запустить приложение с ключом --render-only,
 * приложение выведет в cout только SVG карту маршрутов
 */

int main(int argc, char* argv[]) {
    using namespace transport_catalogue;
    std::ifstream in("C:\\test6.txt");
    std::ofstream out("ans6.txt");
    try {
        transport_catalogue::TransportCatalogue db;
        const auto json = json::Load(in).GetRoot().AsMap();

        json_reader::ReadTransportCatalogue(db, json.at("base_requests"s).AsArray());

        // В 1 части итогового проекта во входных данных отсутствует ключ render_settings
        // В этом случае используем настройки по умолчанию
        renderer::RenderSettings render_settings;
        if(json.count("render_settings"s) != 0){
            json_reader::ReadRenderSettings(render_settings, json.at("render_settings"s).AsMap());
        }
        renderer::MapRenderer renderer{ std::move(render_settings) };

        service::RequestHandler handler(db, renderer);

        json::Array responses
            = json_reader::HandleRequests(json.at("stat_requests"s).AsArray(), handler);

        if (argc == 2 && argv[1] == "--render-only"sv) {
            handler.RenderMap().Render(out);
        }
        else {
            json::Print(json::Document(std::move(responses)), out);
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error: "sv << e.what() << std::endl;
    }
}