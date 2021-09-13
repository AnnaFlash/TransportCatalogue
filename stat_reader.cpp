//#include "stat_reader.h"
//void OutputBus(std::string& req, transport_catalogue::TransportCatalogue& catalogue, std::ostream& output) {
//	auto b_name = req.substr(sizeof("Bus"));
//	b_name.erase(b_name.find_last_not_of(' ') + 1);
//	auto inf = catalogue.GetBusInfo(b_name);
//	if (inf.exists == false) {
//		output << "Bus " << b_name << ": not found" << std::endl;
//	}
//	else {
//		output << "Bus " << b_name << ": " << inf.stops << " stops on route, " << inf.uniq_stops
//			<< " unique stops, " << inf.length << " route length, " << std::setprecision(6)
//			<< inf.curvature << std::defaultfloat << " curvature" << std::endl;
//	}
//}
//
//void OutputStop(std::string& req, transport_catalogue::TransportCatalogue& catalogue, std::ostream& output) {
//	auto s_name = req.substr(sizeof("Stop"));
//	s_name.erase(s_name.find_last_not_of(' ') + 1);
//	auto s_inf = catalogue.GetStopInfo(s_name);
//	if (s_inf.about.find("not found") != std::string::npos) {
//		output << "Stop " << s_name << ": not found" << std::endl;
//	}
//	else if (s_inf.about.find("no buses") != std::string::npos) {
//		output << "Stop " << s_name << ": no buses" << std::endl;
//	}
//	else {
//		output << "Stop " << s_name << ": buses";
//		for (auto s : s_inf.stop_buses_) {
//			output << " " << s;
//		}
//		output << std::endl;
//	}
//}
//
//void output::OutputInfo(transport_catalogue::TransportCatalogue& catalogue, std::istream& input, std::ostream& output)
//{
//	using namespace transport_catalogue;
//	size_t request;
//	input >> request;
//	std::string req;
//	getline(input, req);
//	for (size_t i = 0; i < request; i++) {
//		getline(input, req);
//		if (req.find("Bus ") != std::string::npos) {
//			OutputBus(req, catalogue, output);
//		}
//		else if (req.find("Stop ") != std::string::npos) {
//			OutputStop(req, catalogue, output);
//		}
//		else {
//			std::cerr << "incorrect request!";
//		}
//	}
//}
