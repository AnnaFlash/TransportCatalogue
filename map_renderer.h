#pragma once
#include "domain.h"
#include "svg.h"

#include <cassert>
#include <map>

    namespace renderer {
        using namespace transport_catalogue;
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
            MapView(RenderSettings render_settings, std::vector<BusPtr> buses);

            void Draw(svg::ObjectContainer& container) const override;

        private:
            void DrawRoutes(svg::ObjectContainer& container) const;
            void DrawRoutesNames(svg::ObjectContainer& container) const;
            void DrawStopCitcles(svg::ObjectContainer& container) const;
            void DrawStopsNames(svg::ObjectContainer& container) const;
            const svg::Color& GetBusLineColor(size_t index) const;

            struct SortedByName {
                bool operator()(StopPtr lhs, StopPtr rhs) const {
                    assert(lhs);
                    assert(rhs);
                    return lhs->name < rhs->name;
                }
            };
            RenderSettings render_settings_;
            std::vector<BusPtr> buses_;
            std::map<StopPtr, svg::Point, SortedByName> stops_coords_;
        };

        class MapRenderer {
        public:
            MapRenderer() = default;
            MapRenderer(RenderSettings settings);

            MapView RenderMap(const std::vector<BusPtr> b) const {
                std::vector<BusPtr> buses = b;
                return { render_settings_, buses };
            }
            void SetRenderSettings(const RenderSettings& settings);
            RenderSettings GetRenderSettings() const;

        private:
            RenderSettings render_settings_;
        };

    }  // namespace renderer
