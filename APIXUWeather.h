#pragma once
#include <JsonListener.h>
#include <JsonStreamingParser.h>
#include <time.h>

typedef struct APIXUWeatherCurrentData {
	// The following are under current branch
	float temp_c;
	float temp_f;
	// under current/condition
	String text; // description
	String code; // code for icon
	uint8_t wind_kph; // wind speed km per hour
	String wind_dir; // wind direction
	uint8_t humidity;
	// We will get MeteoCon from code
	String iconMeteoCon;
} APIXUWeatherCurrentData;

typedef struct APIXUWeatherForecastData {
	// All under forecast branch
	// 2018-07-27
	String date;
	uint32_t date_epoch;
	float maxtemp_c; // max temp
	float mintemp_c; // min temp
	float totalprecip_mm; 
	uint8_t avghumidity;
	String text; // description
	String code; // code for icon
	// We will get MeteoCon from code
	String iconMeteoCon;
} APIXUWeatherForecastData;

class APIXUWeather : public JsonListener {
private:
	String currentKey;
	String currentParent;
	APIXUWeatherCurrentData *data;
	APIXUWeatherForecastData *data1;
	uint8_t currentForecast;
	uint8_t maxForecasts;
	uint8_t currentFinished = 0;
	uint8_t doUpdate(APIXUWeatherCurrentData *data, APIXUWeatherForecastData *data1, String url);

public:
	APIXUWeather();
	uint8_t updateWeather(APIXUWeatherCurrentData *data, APIXUWeatherForecastData *data1, String appId, String location, String language, uint8_t maxForecasts);

	String getMeteoconIcon(String icon);

	virtual void whitespace(char c);

	virtual void startDocument();

	virtual void key(String key);

	virtual void value(String value);

	virtual void endArray();

	virtual void endObject();

	virtual void endDocument();

	virtual void startArray();

	virtual void startObject();
};
