#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm> // For std::transform

// Base class representing general weather data
class WeatherData {
public:
    virtual void displayWeather() const = 0; // Pure virtual function
};

// Derived class for specific city weather data
class CityWeather : public WeatherData {
private:
    std::string cityName;
    double temperature;
    double pressure;

public:
    CityWeather(const std::string& city, double temp, double pres)
        : cityName(city), temperature(temp), pressure(pres) {}

    // Override the displayWeather method
    void displayWeather() const override {
        std::cout << "Temperature in " << cityName << ": " << temperature << "°C" << std::endl;
        std::cout << "Pressure in " << cityName << ": " << pressure << " hPa" << std::endl;
    }
};

class Weather {
public:
    Weather(const std::string& apiKey) : apiKey(apiKey) {}

    void getWeather(const std::string& city) {
        std::string response = readWeatherData();
        if (!response.empty()) {
            parseWeatherData(response, city); // Parse the weather data
        } else {
            std::cerr << "Failed to get weather data for " << city << std::endl;
        }
    }

private:
    std::string apiKey;

    std::string readWeatherData() {
        std::ifstream file("weather_data.json");
        if (!file) {
            std::cerr << "Unable to open file weather_data.json" << std::endl;
            return "";
        }

        std::stringstream buffer;
        buffer << file.rdbuf();   // Read the entire file into the buffer
        return buffer.str(); // Return the contents of the file
    }

    void parseWeatherData(const std::string& response, const std::string& city) {
        size_t weatherArrayPos = response.find("\"weather\":");
        if (weatherArrayPos != std::string::npos) {
            size_t mainPos = response.find("\"main\":", weatherArrayPos);
            size_t cityPos = response.find("\"name\":", mainPos);

            while (cityPos != std::string::npos) {
                size_t nameStart = response.find("\"", cityPos + 7) + 1; // Move past the colon
                size_t nameEnd = response.find("\"", nameStart);
                std::string foundCity = response.substr(nameStart, nameEnd - nameStart);

                // Convert both names to lowercase for comparison
                std::string cityLower = city;
                std::transform(cityLower.begin(), cityLower.end(), cityLower.begin(), ::tolower);
                std::string foundCityLower = foundCity;
                std::transform(foundCityLower.begin(), foundCityLower.end(), foundCityLower.begin(), ::tolower);

                if (foundCityLower == cityLower) {
                    size_t tempPos = response.find("\"temp\":", mainPos);
                    size_t tempEnd = response.find(",", tempPos);
                    double temperature = std::stod(response.substr(tempPos + 7, tempEnd - tempPos - 7));

                    size_t pressurePos = response.find("\"pressure\":", mainPos);
                    size_t pressureEnd = response.find(",", pressurePos);
                    double pressure = std::stod(response.substr(pressurePos + 12, pressureEnd - pressurePos - 12));

                    CityWeather cityWeather(foundCity, temperature, pressure); // Create city weather object
                    cityWeather.displayWeather(); // Display the weather using polymorphism
                    return; // Exit after finding the city
                }

                mainPos = response.find("\"main\":", cityPos);
                cityPos = response.find("\"name\":", mainPos);
            }
        }

        std::cerr << "City not found: " << city << std::endl;
    }
};

int main() {
    std::string apiKey = "43bbf88beaf107a1f5e67bca52351d51"; // Your API key
    Weather weather(apiKey); // Create a Weather object with the API key
    
    std::string city;
    std::cout << "Enter city name: ";
    std::getline(std::cin, city); // Get city name from user

    weather.getWeather(city); // Fetch and display the weather data

    return 0;
}
