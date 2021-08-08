#include "transport_catalogue.h"
using namespace transport_catalogue;
void TransportCatalogue::AddStop(std::string_view stop, const double lat, const double longt)
{
	Stop s = { std::string(stop), {lat, longt}, {} };
	stops_.emplace(stop, std::move(s));
    auto correct_name = stops_.extract(std::string(stop));
    correct_name.key() = correct_name.mapped().name;
	stops_.insert(std::move(correct_name)).position->second;
	stop_to_buses_[stops_.at(stop).name];
}

void TransportCatalogue::AddBus(std::string_view bus, std::vector<std::string_view> stops)
{
	Bus b = { std::string(bus), {}};
	for (auto& s : stops) {
		b.b_stops.push_back(FindStop(s)); 
	}
	buses_.emplace(bus, std::move(b));
	auto correct_name = buses_.extract(std::string(bus));
	correct_name.key() = correct_name.mapped().B_name;
	buses_.insert(std::move(correct_name)).position->second;
	for (auto s : stops) {
		stop_to_buses_[FindStop(s).name].insert(FindBus(bus).B_name);
	}
}

void TransportCatalogue::AddDistances(const std::string_view stop1, const std::string_view stop2, const int distance)
{
	FindStop(stop1).distances.emplace(&FindStop(stop2), distance);
}

[[nodiscard]] inline int TransportCatalogue::GetDistances(const std::string_view stop1, const std::string_view stop2) const
{
	auto& stop1_S = FindStop(stop1);
	auto& stop2_S = FindStop(stop2);

	if (stop1_S.distances.count(&stop2_S)) {
		return stop1_S.distances.at(&stop2_S);
	}
	return stop2_S.distances.at(&stop1_S);
}

[[nodiscard]] inline Bus& TransportCatalogue::FindBus(const std::string_view bus) const
{
	if (buses_.count(bus)) {
		return const_cast<Bus&>(buses_.at(bus));
	}
	else {
		throw std::invalid_argument("bus does not exists!");
	}
}

[[nodiscard]] inline Stop& TransportCatalogue::FindStop(const std::string_view stop) const
{
	if (stops_.count(stop)) {
		return const_cast<Stop&>(stops_.at(stop));
	}
	else {
		throw std::invalid_argument("stop does not exists!");
	}
}

[[nodiscard]] BInfo TransportCatalogue::BusInfo(const std::string_view bus) const noexcept
{
	if (buses_.count(bus) == 0) {
		return BInfo();
	}
	BInfo i = { true, buses_.at(bus).b_stops.size(), 0, 0, 0.0 };
	std::unordered_set<std::string_view> uniq;
	for (auto& st : buses_.at(bus).b_stops) {
		uniq.emplace(st.name);
	}
	i.uniq_stops = uniq.size();
	const Bus& b = buses_.at(bus);
	auto get_geographical_distance = [](const Stop stop2, const Stop stop1) {
		return distance::ComputeDistance(stop1.coordinates, stop2.coordinates);
	};

	double geographical_length = std::transform_reduce(++std::begin(b.b_stops), std::end(b.b_stops),
		std::begin(b.b_stops), 0.0, std::plus<>{}, get_geographical_distance
	);

	auto get_length = [this](const Stop stop2, const Stop stop1) {
		return GetDistances(stop1.name, stop2.name);
	};

	i.length = std::transform_reduce(std::execution::par, ++std::begin(b.b_stops), std::end(b.b_stops),
		std::begin(b.b_stops), 0, std::plus<>{}, get_length
	);

	i.curvature = i.length / geographical_length;
	return i;
}

SInfo TransportCatalogue::StopInfo(const std::string_view stop) const noexcept
{
	if (stop_to_buses_.count(stop) == 0) {
		return SInfo();
	}
	SInfo si;
	if (stop_to_buses_.at(stop).size() == 0) {
		si.about = "no buses";
	}
	else {
		si.about = "Ok";
		for (auto bus : stop_to_buses_.at(stop)) {
			si.stopbuses_.push_back(bus);
		}
		sort(si.stopbuses_.begin(), si.stopbuses_.end());
	}
	return SInfo(si);
}

