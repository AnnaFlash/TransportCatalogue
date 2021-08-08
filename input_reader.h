#pragma once
#include "transport_catalogue.h"
#include <sstream>
namespace input {
	void FillCatalogue(transport_catalogue::TransportCatalogue& catalogue, std::istream& input = std::cin);
}