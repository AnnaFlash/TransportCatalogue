#pragma once
#include  "request_handler.h"
#include "map_renderer.h"
/*
 * Здесь можно разместить код наполнения транспортного справочника данными из JSON,
 * а также код обработки запросов к базе и формирование массива ответов в формате JSON
 */
struct Renderer_settings {
    double width, height, padding, line_width, stop_radius, underlayer_width;
    int bus_label_font_size, stop_label_font_size;
    distance::Coordinates bus_label_offset, stop_label_offset;
    svg::Color underlayer_color;
    std::vector<svg::Color> color_pallete;
};
namespace input_json {
	void AddingBuses(transport_catalogue::TransportCatalogue& catalogue, std::vector<detail::Bus_to_Add>& buses);
	void AddingDistances(transport_catalogue::TransportCatalogue& catalogue, std::vector<detail::StopDistances>&& distances, std::vector<std::string>&& buses_names);
	void InputJson(std::istream& input = std::cin, std::ostream& out = std::cout);
	[[nodiscard]] inline transport_catalogue::TransportCatalogue FillCatalogue(const json::Node& n, const json::Dict& render_settings, Renderer_settings& r);
	void OutputCatalogue(transport_catalogue::TransportCatalogue& tc, const json::Node& n, const Renderer_settings& r, std::ostream& out);
	void Draw(transport_catalogue::TransportCatalogue& tc, const Renderer_settings& r, std::ostream& out);
}