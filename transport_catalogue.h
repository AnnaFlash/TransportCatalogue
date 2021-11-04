#pragma once
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <deque>
#include <iostream>
#include <optional>
#include <vector>
#include "domain.h"
#include "geo.h"

namespace transport_catalogue {
	using StopsDistance = std::unordered_map<std::pair<StopPtr, StopPtr>, size_t, HashPairStopPtr>;
	class TransportCatalogue
	{
	public:
		void AddStop(const std::string_view stop, double lat, double longt);
		void AddBus(const std::string_view bus, std::vector<std::string_view> stops, std::vector<std::string> end_points);
		void AddDistances(const std::string_view stop1, const std::string_view stop2, const int distance);
		void AddLenth(const std::string_view bus);
		[[nodiscard]] inline int GetDistances(const std::string_view stop1, const std::string_view stop2) const;
		[[nodiscard]] size_t GetDistanceBetweenStops(StopPtr from, StopPtr to) const;
		[[nodiscard]] BusPtr FindBus(const std::string_view bus) const;
		[[nodiscard]] StopPtr FindStop(const std::string_view stop) const;
		[[nodiscard]] std::vector<Stop> GetStops() const noexcept;
		const std::set<BusPtr>* GetBusesByStop(const std::string_view& stop_name) const noexcept;
		[[nodiscard]] BusInfo GetBusInfo(const std::string_view bus) const noexcept;
		[[nodiscard]] StopInfo GetStopInfo(const std::string_view stop) const noexcept;
		const std::vector<BusPtr> GetBuses() const noexcept
		{
			return deq_buses_;
		}
	private:
		std::vector<BusPtr> deq_buses_;
		std::unordered_map<std::string_view, Stop> stops_;
		std::unordered_map<std::string_view, Bus> buses_;
		std::unordered_map<std::string_view, std::set<std::string_view>> stop_to_buses_;
		std::unordered_map<StopPtr, std::set<BusPtr>> bus_by_stop_;
		StopsDistance distance_between_stops_;
		std::set<BusPtr> dummy_;
	};
}
