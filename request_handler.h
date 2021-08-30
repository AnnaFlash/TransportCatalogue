#pragma once
#include "transport_catalogue.h"
#include "svg.h"
#include "json.h"
#include <optional>

/*
 * Здесь можно было бы разместить код обработчика запросов к базе, содержащего логику, которую не
 * хотелось бы помещать ни в transport_catalogue, ни в json reader.
 *
 * В качестве источника для идей предлагаем взглянуть на нашу версию обработчика запросов.
 * Вы можете реализовать обработку запросов способом, который удобнее вам.
 *
 * Если вы затрудняетесь выбрать, что можно было бы поместить в этот файл,
 * можете оставить его пустым.
 */

 // Класс RequestHandler играет роль Фасада, упрощающего взаимодействие JSON reader-а
 // с другими подсистемами приложения.
 // См. паттерн проектирования Фасад: https://ru.wikipedia.org/wiki/Фасад_(шаблон_проектирования)
namespace renderer {
    /*class MapRenderer {
        MapRenderer() = default;
    private:
        double width, height;
        double padding;
        double line_width;
        double stop_radius;
        int bus_label_font_size;
        distance::Coordinates bus_label_offset;
        int stop_label_font_size;
        distance::Coordinates stop_label_offset;
        svg::Color underlayer_color;
        double underlayer_width;
        std::vector<svg::Color> color_pallete;
    };*/
}

 class RequestHandler{
 public:
     RequestHandler(transport_catalogue::TransportCatalogue& db) :db_(db) {}
     // MapRenderer понадобится в следующей части итогового проекта
     //RequestHandler(const transport_catalogue::TransportCatalogue& db, const renderer::MapRenderer& renderer);

     // Возвращает информацию о маршруте (запрос Bus)
     transport_catalogue::BusInfo GetBusStat(const std::string_view& bus_name) const;

     // Возвращает маршруты, проходящие через
     const transport_catalogue::StopInfo GetBusesByStop(const std::string_view& stop_name) const;

     // Этот метод будет нужен в следующей части итогового проекта
     //svg::Document RenderMap() const;

 private:
     // RequestHandler использует агрегацию объектов "Транспортный Справочник" и "Визуализатор Карты"
     const transport_catalogue::TransportCatalogue& db_;
    // const renderer::MapRenderer& renderer_;
 };
 