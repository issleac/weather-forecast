#ifndef WEATHER_FORECAST_WEATHER_H
#define WEATHER_FORECAST_WEATHER_H

#include <string>
#include <utility>

class Weather {
private:
    std::string m_date;
    std::string m_week;
    std::string m_phenomenon;
    std::string m_air_temperature;
    std::string m_wind_direction;
    std::string m_wind_power;
public:
    Weather(std::string m_date,std::string m_week,std::string m_phenomenon,std::string m_air_temperature,std::string m_wind_direction,std::string m_wind_power);
    ~Weather();
    void to_execl(std::ofstream& of);
};


#endif //WEATHER_FORECAST_WEATHER_H
