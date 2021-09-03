#pragma once
#include "domain.h"
#include "svg.h"

#include <cassert>
#include <map>
namespace transport_catalogue {

    namespace renderer {

        struct RenderSettings {
            std::vector<svg::Color> palette;
            double max_width;
            double max_height;
            double padding;
            double line_width;
            double stop_radius;
            svg::Color underlayer_color{ std::string("white") };
            double underlayer_width;
            svg::Point stop_label_offset;
            uint32_t stop_label_font_size;
            svg::Color stop_label_color{ std::string("black") };
            std::string stop_label_font_family{ "Verdana" };
            svg::Point bus_label_offset;
            uint32_t bus_label_font_size;
            std::string bus_label_font_family{ "Verdana" };
        };

        class MapView : public svg::Drawable {
        public:
            MapView(RenderSettings render_settings, std::vector<Bus*> buses);

            void Draw(svg::ObjectContainer& container) const override;

        private:
            void DrawRoutes(svg::ObjectContainer& container) const;
            void DrawRoutesNames(svg::ObjectContainer& container) const;
            void DrawStopCitcles(svg::ObjectContainer& container) const;
            void DrawStopsNames(svg::ObjectContainer& container) const;
            const svg::Color& GetBusLineColor(size_t index) const;

            struct SortedByName {
                bool operator()(const Stop* lhs, const Stop* rhs) const {
                    assert(lhs);
                    assert(rhs);
                    return lhs->name < rhs->name;
                }
            };
            RenderSettings render_settings_;
            std::vector<Bus*> buses_;
            std::map<const Stop*, svg::Point, SortedByName> stops_coords_;
        };

        class MapRenderer {
        public:
            MapRenderer() = default;
            MapRenderer(RenderSettings settings);

            MapView RenderMap(const std::vector<Bus*> b) const {
                std::vector<Bus*> buses = b;

                return { render_settings_, std::move(buses) };
            }
        private:
            RenderSettings render_settings_;
        };

    }  // namespace renderer

}  // namespace transport_catalogue