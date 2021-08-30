#include "json_reader.h"

/*
 * Здесь можно разместить код наполнения транспортного справочника данными из JSON,
 * а также код обработки запросов к базе и формирование массива ответов в формате JSON
 */
void FillRenderSettings(Renderer_settings& r, const json::Dict& render_settings) {
	r.bus_label_font_size = render_settings.at("bus_label_font_size").AsInt();
	r.bus_label_offset = distance::Coordinates({ render_settings.at("bus_label_offset").AsArray()[0].AsDouble(),
		render_settings.at("bus_label_offset").AsArray()[1].AsDouble() });

	for (auto& col : render_settings.at("color_palette").AsArray()) {
		if (col.IsString()) {
			r.color_pallete.emplace_back(col.AsString());
		}
		else if (col.IsArray() && col.AsArray().size() == 3) {
			r.color_pallete.emplace_back(svg::Rgb(col.AsArray()[0].AsInt(),
				col.AsArray()[1].AsInt(),
				col.AsArray()[2].AsInt()));
		}
		else {
			r.color_pallete.emplace_back(svg::Rgba(col.AsArray()[0].AsInt(),
				col.AsArray()[1].AsInt(),
				col.AsArray()[2].AsInt(), col.AsArray()[3].AsDouble()));
		}
	}
	r.height = render_settings.at("height").AsDouble();
	r.line_width = render_settings.at("line_width").AsDouble();
	r.padding = render_settings.at("padding").AsDouble();
	r.stop_label_font_size = render_settings.at("stop_label_font_size").AsInt();
	r.stop_label_offset = distance::Coordinates({ render_settings.at("stop_label_offset").AsArray()[0].AsDouble(),
		render_settings.at("stop_label_offset").AsArray()[1].AsDouble() });

	r.stop_radius = render_settings.at("stop_radius").AsDouble();
	if (render_settings.at("underlayer_color").IsString()) {
		r.underlayer_color = render_settings.at("underlayer_color").AsString();
	}
	else if (render_settings.at("underlayer_color").IsArray() && render_settings.at("underlayer_color").AsArray().size() == 3) {
		r.underlayer_color = svg::Rgb(render_settings.at("underlayer_color").AsArray()[0].AsInt(),
			render_settings.at("underlayer_color").AsArray()[1].AsInt(),
			render_settings.at("underlayer_color").AsArray()[2].AsInt());
	}
	else {
		r.underlayer_color = svg::Rgba(render_settings.at("underlayer_color").AsArray()[0].AsInt(),
			render_settings.at("underlayer_color").AsArray()[1].AsInt(),
			render_settings.at("underlayer_color").AsArray()[2].AsInt(), render_settings.at("underlayer_color").AsArray()[3].AsDouble());
	}
	r.underlayer_width = render_settings.at("underlayer_width").AsDouble();;
	r.width = render_settings.at("width").AsDouble();
}

void input_json::AddingBuses(transport_catalogue::TransportCatalogue& catalogue, std::vector<detail::Bus_to_Add>& buses) {
	using namespace transport_catalogue;
	for (auto& bus : buses) {
		std::vector<std::string_view> stops(begin(bus.stops), end(bus.stops));
		catalogue.AddBus(std::move(bus.name), std::move(stops), bus.is_round);
	}
}

void input_json::AddingDistances(transport_catalogue::TransportCatalogue& catalogue, std::vector<detail::StopDistances>&& distances, std::vector<std::string>&& buses_names) {
	for (auto& stop_distance : distances) {
		for (auto& [s_name, distance] : stop_distance.distances) {
			catalogue.AddDistances(stop_distance.stop, s_name, distance);
		}
	}
	for (auto& name : buses_names) {
		catalogue.AddLenth(name);
	}
}
void input_json::InputJson(std::istream& input, std::ostream& out)
{
	using namespace json;
	Document doc = Load(input);
	Renderer_settings r;
	transport_catalogue::TransportCatalogue tc = FillCatalogue(doc.GetRoot().AsMap().at("base_requests"), doc.GetRoot().AsMap().at("render_settings").AsMap(), r);
	OutputCatalogue(tc, doc.GetRoot().AsMap().at("stat_requests"), r, out);
}

transport_catalogue::TransportCatalogue input_json::FillCatalogue(const json::Node& n, const json::Dict& render_settings, Renderer_settings& r)
{
	using namespace transport_catalogue;
	TransportCatalogue tc;
	std::vector<detail::Bus_to_Add> buses;
	std::vector<detail::StopDistances> dist;
	std::vector<std::string> buses_names;
	auto req_array = n.AsArray();
	for (auto& req : req_array) {
		auto req_map = req.AsMap();
		if (req_map.at("type").AsString() == "Bus") {
			buses_names.push_back(req_map.at("name").AsString());
			detail::Bus_to_Add b = { std::move(req_map.at("name").AsString()) , {} , false };
			for (auto stop : req_map.at("stops").AsArray()) {
				b.stops.emplace_back(std::move(stop.AsString()));
			}
			if (req_map.at("is_roundtrip").AsBool() == false) {
				b.stops.reserve(b.stops.size() * 2 - 1);
				for (auto it = ++std::rbegin(b.stops); it != std::rend(b.stops); ++it) {
					b.stops.emplace_back(*it);
				}
			}
			b.is_round = req_map.at("is_roundtrip").AsBool();
			buses.emplace_back(std::move(b));
		}
		else if (req_map.at("type").AsString() == "Stop") {
			tc.AddStop(req_map.at("name").AsString(), req_map.at("latitude").AsDouble(), req_map.at("longitude").AsDouble());
			detail::StopDistances distances_ = { std::move(req_map.at("name").AsString()), {} };
			for (auto& [stop, dist] : req_map.at("road_distances").AsMap()) {
				distances_.distances[stop] = dist.AsInt();
			}
			dist.emplace_back(std::move(distances_));
		}
		else {
			std::cerr << "Incorrect input";
			break;
		}
	}
	FillRenderSettings(r, render_settings);
	AddingBuses(tc, (buses));
	AddingDistances(tc, std::move(dist), std::move(buses_names));
	return tc;
}

void input_json::OutputCatalogue(transport_catalogue::TransportCatalogue& tc, const json::Node& n, const Renderer_settings& r, std::ostream& out)
{
	using namespace transport_catalogue;
	RequestHandler handler(tc);
	json::Array answers;
	std::string render_map;
	std::stringstream render_out(render_map);
	for (auto& req : n.AsArray()) {
		if (req.AsMap().at("type").AsString() == "Bus") {
			json::Dict dict;
			BusInfo b_inf = handler.GetBusStat(req.AsMap().at("name").AsString());
			if (b_inf.exists == false) {
				dict["request_id"] = json::Node(req.AsMap().at("id").AsInt());
				dict["error_message"] = json::Node(std::string("not found"));
			}
			else {
				dict["curvature"] = json::Node(b_inf.curvature);
				dict["request_id"] = json::Node(req.AsMap().at("id").AsInt());
				dict["route_length"] = json::Node(b_inf.length);
				dict["stop_count"] = json::Node(b_inf.stops);
				dict["unique_stop_count"] = json::Node(b_inf.uniq_stops);
			}
			answers.emplace_back(std::move(json::Node(dict)));
		}
		else if (req.AsMap().at("type").AsString() == "Stop") {
			json::Dict dict_stop;
			StopInfo buses = handler.GetBusesByStop(req.AsMap().at("name").AsString());
			if (buses.about == "not found") {
				dict_stop["request_id"] = json::Node(req.AsMap().at("id").AsInt());
				dict_stop["error_message"] = json::Node(std::string("not found"));
			}
			else {
				json::Array ar;
				for (auto& bus : buses.stop_buses_) {
					ar.push_back(json::Node(std::string(bus)));
				}
				dict_stop["buses"] = json::Node(ar);
				dict_stop["request_id"] = json::Node(req.AsMap().at("id").AsInt());
			}
			answers.emplace_back(std::move(json::Node(dict_stop)));
		}
		else if (req.AsMap().at("type").AsString() == "Map") {
			json::Dict dict;
			Draw(tc, r, render_out);
			render_map = render_out.str();
			dict["request_id"] = json::Node(req.AsMap().at("id").AsInt());
			dict["map"] = json::Node(render_map);
			answers.emplace_back(std::move(json::Node(dict)));
		}
		else {
			std::cerr << "Incorrect input";
			break;
		}
	}
	json::Node node = json::Node(answers);
	json::Document doc(node);
	json::Print(doc, out);
}

void input_json::Draw(transport_catalogue::TransportCatalogue& tc, const Renderer_settings& r, std::ostream& out)
{
	using namespace transport_catalogue;
	using namespace svg;
	Document doc;
	std::vector<Bus> buses;
	std::vector<Stop> all_stops;
	std::vector<Stop> stops;
	std::vector<Polyline> routes;
	std::vector<std::pair<std::string, Text>> routes_names;
	std::vector<std::pair<std::string, Text>> routes_names_unders;

	std::vector<std::pair<std::string, Circle>> stops_circles;
	std::vector<std::pair<std::string, Text>> stops_names_unders;
	std::vector<std::pair<std::string, Text>> stops_names;
	std::vector<distance::Coordinates> minlon_maxlat;
	tc.GetBuses(buses);
	tc.GetStops(all_stops);
	for (auto& stop : all_stops) {
		if (tc.GetStopInfo(stop.name).stop_buses_.size() != 0) {
			stops.emplace_back(stop);
		}
	}
	for (auto& stop : stops) {
		minlon_maxlat.push_back(stop.coordinates);
	}

	std::sort( buses.begin(), buses.end(), [](const Bus& b1, const Bus& b2) {return b1.B_name < b2.B_name; });
	SphereProjector s(minlon_maxlat.begin(), minlon_maxlat.end(), r.width, r.height, r.padding);
	int i = 0, j = 0;
	for (auto& st : stops) {
		if (tc.GetStopInfo(st.name).stop_buses_.size() != 0) {
			stops_circles.push_back({ st.name, Circle().SetCenter(s(distance::Coordinates(st.coordinates))).SetRadius(r.stop_radius).SetFillColor("white") });
			stops_names.push_back({ st.name,Text().SetPosition(s(distance::Coordinates(st.coordinates))).SetData(st.name).SetOffset(Point({ r.stop_label_offset.lat,  r.stop_label_offset.lng }))
				.SetFontSize(r.stop_label_font_size).SetFontFamily("Verdana")
				.SetFillColor("black") });
			stops_names_unders.push_back({ st.name, (Text().SetPosition(s(distance::Coordinates(st.coordinates))).SetData(st.name).SetOffset(Point({ r.stop_label_offset.lat,  r.stop_label_offset.lng }))
				.SetFontSize(r.stop_label_font_size).SetFontFamily("Verdana")
				.SetFillColor(r.underlayer_color).SetStrokeColor(r.underlayer_color)
				.SetStrokeWidth(r.underlayer_width).SetStrokeLineCap(StrokeLineCap::ROUND)
				.SetStrokeLineJoin(StrokeLineJoin::ROUND)) });
		}
	}
	for (auto& bus : buses) {
		Polyline poly;
		if (bus.b_stops.size() == 0) {
			break;
		}
		for (const Stop& st : bus.b_stops) {
			poly.AddPoint(s(distance::Coordinates(st.coordinates)));
		}
		poly.SetStrokeWidth(r.line_width).SetStrokeColor(r.color_pallete[i])
			.SetStrokeLineCap(StrokeLineCap::ROUND).SetStrokeLineJoin(StrokeLineJoin::ROUND).SetFillColor("none");
		routes.emplace_back(std::move(poly));
		j = (bus.round == false) ? 2 : 1;
		if (j == 2) {
			routes_names.push_back({ bus.B_name, Text().SetPosition(s(bus.b_stops[0].coordinates)).SetData(bus.B_name)
				.SetFillColor(r.color_pallete[i]).SetOffset(Point({ r.bus_label_offset.lat,  r.bus_label_offset.lng })).SetFontSize(r.bus_label_font_size)
				.SetFontFamily("Verdana").SetFontWeight("bold") });

			routes_names.push_back({ bus.B_name, Text().SetPosition(s(bus.b_stops[bus.b_stops.size() / 2].coordinates)).SetData(bus.B_name)
				.SetFillColor(r.color_pallete[i]).SetOffset(Point({ r.bus_label_offset.lat,  r.bus_label_offset.lng })).SetFontSize(r.bus_label_font_size)
				.SetFontFamily("Verdana").SetFontWeight("bold") });

			routes_names_unders.push_back({ bus.B_name, Text().SetPosition(s(bus.b_stops[0].coordinates)).SetData(bus.B_name)
				.SetFillColor(r.underlayer_color).SetOffset(Point({ r.bus_label_offset.lat,  r.bus_label_offset.lng })).SetFontSize(r.bus_label_font_size)
				.SetFontFamily("Verdana").SetFontWeight("bold").SetStrokeColor(r.underlayer_color).SetStrokeWidth(r.underlayer_width)
				.SetStrokeLineCap(StrokeLineCap::ROUND).SetStrokeLineJoin(StrokeLineJoin::ROUND) });

			routes_names_unders.push_back({ bus.B_name, Text().SetPosition(s(bus.b_stops[bus.b_stops.size() / 2].coordinates)).SetData(bus.B_name)
				.SetFillColor(r.underlayer_color).SetOffset(Point({ r.bus_label_offset.lat,  r.bus_label_offset.lng })).SetFontSize(r.bus_label_font_size)
				.SetFontFamily("Verdana").SetFontWeight("bold").SetStrokeColor(r.underlayer_color).SetStrokeWidth(r.underlayer_width)
				.SetStrokeLineCap(StrokeLineCap::ROUND).SetStrokeLineJoin(StrokeLineJoin::ROUND) });
		}
		else {
			routes_names.push_back({ bus.B_name, Text().SetPosition(s(bus.b_stops[0].coordinates)).SetData(bus.B_name)
				.SetFillColor(r.color_pallete[i]).SetOffset(Point({ r.bus_label_offset.lat,  r.bus_label_offset.lng })).SetFontSize(r.bus_label_font_size)
				.SetFontFamily("Verdana").SetFontWeight("bold") });
			routes_names_unders.push_back({ bus.B_name, Text().SetPosition(s(bus.b_stops[0].coordinates)).SetData(bus.B_name)
				.SetFillColor(r.underlayer_color).SetOffset(Point({ r.bus_label_offset.lat,  r.bus_label_offset.lng })).SetFontSize(r.bus_label_font_size)
				.SetFontFamily("Verdana").SetFontWeight("bold").SetStrokeColor(r.underlayer_color).SetStrokeWidth(r.underlayer_width)
				.SetStrokeLineCap(StrokeLineCap::ROUND).SetStrokeLineJoin(StrokeLineJoin::ROUND) });
		}
		i = (i + 1) % static_cast<int>(r.color_pallete.size());
	}
	std::sort( stops_circles.begin(), stops_circles.end(), [](const std::pair<std::string, Circle>& pair1, const std::pair<std::string, Circle>& pair2)
		{return pair1.first < pair2.first; });
	std::sort( stops_names.begin(), stops_names.end(), [](const std::pair<std::string, Text>& pair1, const std::pair<std::string, Text>& pair2) {return pair1.first < pair2.first; });
	std::sort( stops_names_unders.begin(), stops_names_unders.end(), [](const std::pair<std::string, Text>& pair1, const std::pair<std::string, Text>& pair2) {return pair1.first < pair2.first; });
	std::sort(routes_names.begin(), routes_names.end(),[](const std::pair<std::string, Text>& pair1, const std::pair<std::string, Text>& pair2) {return pair1.first < pair2.first; });
	std::sort(routes_names_unders.begin(), routes_names_unders.end(), [](const std::pair<std::string, Text>& pair1, const std::pair<std::string, Text>& pair2) {return pair1.first < pair2.first; });
	
	for (auto& p : routes) {
		doc.Add(p);
	}
	for (size_t i = 0; i < routes_names.size(); i++) {
		doc.Add(routes_names_unders[i].second);
		doc.Add(routes_names[i].second);
	}

	for (auto& circle : stops_circles) {
		doc.Add(circle.second);
	}
	for (size_t i = 0; i < stops_names.size(); i++) {
		doc.Add(stops_names_unders[i].second);
		doc.Add(stops_names[i].second);
	}
	doc.Render(out);
}	//[](const Bus& b1, const Bus& b2) {return b1.B_name < b2.B_name; });

	/*minlon_maxlat.push_back((*std::min_element(stops.begin(), stops.end(), [](const Stop& st1, const Stop& st2) {
		return st1.coordinates.lng < st2.coordinates.lng; })).coordinates);
	minlon_maxlat.push_back((*std::max_element(stops.begin(), stops.end(), [](const Stop& st1, const Stop& st2) {
		return st1.coordinates.lat < st2.coordinates.lat; })).coordinates);
	minlon_maxlat.push_back((*std::min_element(stops.begin(), stops.end(), [](const Stop& st1, const Stop& st2) {
		return st1.coordinates.lat < st2.coordinates.lat; })).coordinates);
	minlon_maxlat.push_back((*std::max_element(stops.begin(), stops.end(), [](const Stop& st1, const Stop& st2) {
		return st1.coordinates.lng < st2.coordinates.lng; })).coordinates);*/