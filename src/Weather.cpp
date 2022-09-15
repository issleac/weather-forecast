//
// Created by ByteDance on 2022/9/14.
//

#include <fstream>
#include <iostream>
#include "Weather.h"

Weather::Weather(std::string m_date,std::string m_week,std::string m_phenomenon,std::string m_air_temperature,std::string m_wind_direction,std::string m_wind_power):
m_date(std::move(m_date)),m_week(std::move(m_week)),m_phenomenon(std::move(m_phenomenon)),m_air_temperature(std::move(m_air_temperature)),m_wind_direction(std::move(m_wind_direction)),m_wind_power(std::move(m_wind_power)){}


Weather::~Weather() {}

void Weather::to_execl(std::ofstream& of) {
    of << m_date << "\t" << m_week << "\t" << m_phenomenon << "\t" << m_air_temperature << "\t" << m_wind_direction
       << "\t" << m_wind_power << "\n";
}
