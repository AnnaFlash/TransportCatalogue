#pragma once
#include <string>
#include <vector>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <deque>
#include <iostream>
#include "geo.h"

namespace transport_catalogue {
	struct Stop
	{
		std::string name = "";
		distance::Coordinates coordinates = { 0, 0 };
		std::unordered_map<Stop*, int> distances;
	};

	struct Bus {
		std::string B_name;
		std::deque<Stop*> b_stops;
		int length_ = 0;
		double curvature_ = 0.0;
		std::vector<Stop*> end_points_;
	};

	struct BusInfo {
		bool exists = false;
		int stops = 0;
		int uniq_stops = 0;
		int length = 0;
		double curvature = 0.0;
	};

	struct StopInfo {
		std::string about = "not found";
		std::set<std::string_view> stop_buses_;
	};
}
namespace detail {
	struct Bus_to_Add {
		std::string name;
		std::vector<std::string> stops;
		bool is_round;
		std::vector<std::string> end_points_;
	};
	struct StopDistances {
		std::string stop;
		std::unordered_map<std::string, int> distances;
	};
}
/*
 * � ���� ����� �� ������ ���������� ������/���������, ������� �������� ������ ���������� ������� (domain)
 * ������ ���������� � �� ������� �� ������������� �����������. �������� ���������� �������� � ���������.
 *
 * �� ����� ���� �� ���������� � � transport_catalogue.h, ������ ��������� �� � ���������
 * ������������ ���� ����� ��������� ��������, ����� ���� ����� �� ������������ ����� ���������:
 * ������������ ����� (map_renderer) ����� ����� ������� ����������� �� ������������� �����������.
 *
 * ���� ��������� ������ ���������� �� ��������� ��� �������, ������ �������� ���� ���� ������.
 *
 */