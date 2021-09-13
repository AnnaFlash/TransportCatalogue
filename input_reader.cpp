//#include "input_reader.h"
//
//void TrimString(std::string& string_to_trim) {
//    string_to_trim.erase(string_to_trim.find_last_not_of(' ') + 1);
//    string_to_trim.erase(0, string_to_trim.find_first_not_of(' '));
//}
//
//void AddingBuses(transport_catalogue::TransportCatalogue& catalogue,std::vector<detail::Bus_to_Add>& buses) {
//    using namespace transport_catalogue;
//    for (auto& bus : buses) {
//        std::vector<std::string_view> stops(begin(bus.stops), end(bus.stops));
//        //catalogue.AddBus(std::move(bus.name), std::move(stops)); //for sprint 9 without end_points
//    }
//}
//
//void AddingDistances(transport_catalogue::TransportCatalogue& catalogue, std::vector<detail::StopDistances>&& distances, std::vector<std::string>&& buses_names) {
//    for (auto& stop_distance : distances) {
//        for (auto& [s_name, distance] : stop_distance.distances) {
//            catalogue.AddDistances(stop_distance.stop, s_name, distance);
//        }
//    }
//    for (auto& name : buses_names) {
//        catalogue.AddLenth(name);
//    }
//}
//void ParsingBus(std::istringstream& line_stream, std::vector<detail::Bus_to_Add>& buses, std::vector<std::string>& buses_names) {
//    std::string b_name;
//    getline(line_stream, b_name, ':');
//    TrimString(b_name);
//    buses_names.push_back(b_name);
//    detail::Bus_to_Add b = { move(b_name), {} };
//    std::string stops;
//    getline(line_stream, stops);
//    //ring < , dash - 
//    char dash_or_ring = (stops.find('>') != std::string::npos) ? '>' : '-';
//    auto start = stops.find(' ');
//    while (true) {
//        auto end = stops.find(dash_or_ring, start);
//        auto stop = stops.substr(start, end - start);
//        TrimString(stop);
//        b.stops.emplace_back(move(stop));
//        if (end == std::string::npos) {
//            break;
//        }
//        start = end + 1;
//    }
//    if (dash_or_ring == '-') {
//        b.stops.reserve(b.stops.size() * 2 - 1);
//        for (auto it = ++std::rbegin(b.stops); it != std::rend(b.stops); ++it) {
//            b.stops.emplace_back(*it);
//        }
//    }
//    buses.emplace_back(std::move(b));
//}
//
//void ParsingStops(std::istringstream& line_stream, transport_catalogue::TransportCatalogue& catalogue, std::vector<detail::StopDistances>& dist) {
//    std::string s_name, comma;
//    getline(line_stream, s_name, ':');
//    TrimString(s_name);
//    double lat, longt;
//    line_stream >> lat >> comma >> longt;
//    catalogue.AddStop(s_name, lat, longt);
//    line_stream.ignore(std::numeric_limits<std::streamsize>::max(), ',');
//    if (!line_stream.eof()) {
//        detail::StopDistances distances = { std::move(s_name), {} };
//        while (!line_stream.eof()) {
//            int distance;
//            line_stream >> distance;
//            {
//                std::string unused;
//                line_stream >> unused;
//                line_stream >> unused;
//            }
//            std::string s_name2;
//            getline(line_stream, s_name2, ',');
//            TrimString(s_name2);
//            distances.distances.emplace(std::move(s_name2), distance);
//        }
//        dist.emplace_back(std::move(distances));
//    }
//}
//void input::FillCatalogue(transport_catalogue::TransportCatalogue& catalogue, std::istream& input)
//{
//    using namespace transport_catalogue;
//    int request = 0;
//    input >> request;
//    std::vector<detail::Bus_to_Add> buses;
//    std::vector<detail::StopDistances> dist;
//    std::vector<std::string> buses_names;
//    for (size_t i = 0; i < request; i++) {
//        std::string req;
//        input >> req;
//        std::string line;
//        std::getline(input, line);
//        std::istringstream line_stream(std::move(line));
//        if (req == "Bus") {
//            ParsingBus(line_stream, buses, buses_names);
//        }
//        else if (req == "Stop") {
//            ParsingStops(line_stream, catalogue, dist);
//        }
//        else {
//            throw std::invalid_argument("Invalid query");
//        }
//    }  
//    AddingBuses(catalogue, std::move(buses));
//    AddingDistances(catalogue, std::move(dist), std::move(buses_names));
//}
