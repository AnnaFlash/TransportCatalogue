#include "request_handler.h"

/*
 * Здесь можно было бы разместить код обработчика запросов к базе, содержащего логику, которую не
 * хотелось бы помещать ни в transport_catalogue, ни в json reader.
 *
 * Если вы затрудняетесь выбрать, что можно было бы поместить в этот файл,
 * можете оставить его пустым.
 */

transport_catalogue::BusInfo RequestHandler::GetBusStat(const std::string_view& bus_name) const
{
    return transport_catalogue::BusInfo(db_.GetBusInfo(bus_name));
}

const transport_catalogue::StopInfo RequestHandler::GetBusesByStop(const std::string_view& stop_name) const
{
    return(db_.GetStopInfo(stop_name));
}
