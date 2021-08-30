#include "request_handler.h"

/*
 * ����� ����� ���� �� ���������� ��� ����������� �������� � ����, ����������� ������, ������� ��
 * �������� �� �������� �� � transport_catalogue, �� � json reader.
 *
 * ���� �� ������������� �������, ��� ����� ���� �� ��������� � ���� ����,
 * ������ �������� ��� ������.
 */

transport_catalogue::BusInfo RequestHandler::GetBusStat(const std::string_view& bus_name) const
{
    return transport_catalogue::BusInfo(db_.GetBusInfo(bus_name));
}

const transport_catalogue::StopInfo RequestHandler::GetBusesByStop(const std::string_view& stop_name) const
{
    return(db_.GetStopInfo(stop_name));
}
