#pragma once
#include "transport_catalogue.h"
#include "svg.h"
#include "json.h"
#include <optional>

/*
 * ����� ����� ���� �� ���������� ��� ����������� �������� � ����, ����������� ������, ������� ��
 * �������� �� �������� �� � transport_catalogue, �� � json reader.
 *
 * � �������� ��������� ��� ���� ���������� ��������� �� ���� ������ ����������� ��������.
 * �� ������ ����������� ��������� �������� ��������, ������� ������� ���.
 *
 * ���� �� ������������� �������, ��� ����� ���� �� ��������� � ���� ����,
 * ������ �������� ��� ������.
 */

 // ����� RequestHandler ������ ���� ������, ����������� �������������� JSON reader-�
 // � ������� ������������ ����������.
 // ��. ������� �������������� �����: https://ru.wikipedia.org/wiki/�����_(������_��������������)
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
     // MapRenderer ����������� � ��������� ����� ��������� �������
     //RequestHandler(const transport_catalogue::TransportCatalogue& db, const renderer::MapRenderer& renderer);

     // ���������� ���������� � �������� (������ Bus)
     transport_catalogue::BusInfo GetBusStat(const std::string_view& bus_name) const;

     // ���������� ��������, ���������� �����
     const transport_catalogue::StopInfo GetBusesByStop(const std::string_view& stop_name) const;

     // ���� ����� ����� ����� � ��������� ����� ��������� �������
     //svg::Document RenderMap() const;

 private:
     // RequestHandler ���������� ��������� �������� "������������ ����������" � "������������ �����"
     const transport_catalogue::TransportCatalogue& db_;
    // const renderer::MapRenderer& renderer_;
 };
 