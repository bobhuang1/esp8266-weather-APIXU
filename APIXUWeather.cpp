#include <ESPWiFi.h>
#include <WiFiClient.h>
#include <ESPHTTPClient.h>
#include "APIXUWeather.h"

APIXUWeather::APIXUWeather() {
}

uint8_t APIXUWeather::updateWeather(APIXUWeatherCurrentData *data, APIXUWeatherForecastData *data1, String appId, String location, String language, uint8_t maxForecasts) {
	this->maxForecasts = maxForecasts;
	this->currentFinished = 0;
	return doUpdate(data, data1, "http://api.apixu.com/v1/forecast.json?key=" + appId + "&q=" + location + "&days=" + maxForecasts + "&lang=" + language);
}

uint8_t APIXUWeather::doUpdate(APIXUWeatherCurrentData *data, APIXUWeatherForecastData *data1, String url) {
	if (WiFi.status() != WL_CONNECTED) return 0;
	unsigned long lostTest = 30000UL;
	unsigned long lost_do = millis();
	this->currentForecast = 0;
	this->data = data;
	this->data1 = data1;
	JsonStreamingParser parser;
	parser.setListener(this);
	Serial.printf("Getting url: %s\n", url.c_str());
	HTTPClient http;

	http.begin(url);
	bool isBody = false;
	char c;
	int size;
	Serial.print("[HTTP] GET...\n");
	// start connection and send HTTP header
	int httpCode = http.GET();
	Serial.printf("[HTTP] GET... code: %d\n", httpCode);
	if (httpCode > 0) {
		WiFiClient * client = http.getStreamPtr();
		while (client->connected()) {
			while ((size = client->available()) > 0) {
				if ((millis() - lost_do) > lostTest) {
					Serial.println("lost in client with a timeout");
					client->stop();
					ESP.restart();
				}
				c = client->read();
				if (c == '{' || c == '[') {

					isBody = true;
				}
				if (isBody) {
					parser.parse(c);
				}
			}
		}
	}
	else
	{
		return 0;
	}
	this->data = nullptr;
	this->data1 = nullptr;
	return currentForecast;
}

void APIXUWeather::whitespace(char c) {
}

void APIXUWeather::startDocument() {
}

void APIXUWeather::key(String key) {
	currentKey = String(key);
}

void APIXUWeather::value(String value) {
	if (currentForecast >= maxForecasts) {
		return;
	}
	if (currentFinished == 0)
	{
		if (currentKey == "temp_c") {
			this->data->temp_c = value.toFloat();
		}
		if (currentKey == "temp_f") {
			this->data->temp_f = value.toFloat();
		}
		if (currentKey == "text") {
			this->data->text = value;
		}
		if (currentKey == "code") {
			this->data->code = value;
			this->data->iconMeteoCon = getMeteoconIcon(value);
		}
		if (currentKey == "wind_kph") {
			this->data->wind_kph = value.toInt();
		}
		if (currentKey == "wind_dir") {
			this->data->wind_dir = value;
		}
		if (currentKey == "humidity") {
			this->data->humidity = value.toInt();
			currentFinished = 1;
			currentForecast = 0;
		}
	}
	if (currentFinished == 1)
	{
		if (currentKey == "date") {
			data1[currentForecast].date = value;
		}
		if (currentKey == "date_epoch") {
			data1[currentForecast].date_epoch = value.toInt();
		}
		if (currentKey == "maxtemp_c") {
			data1[currentForecast].maxtemp_c = value.toFloat();
		}
		if (currentKey == "mintemp_c") {
			data1[currentForecast].mintemp_c = value.toFloat();
		}
		if (currentKey == "totalprecip_mm") {
			data1[currentForecast].totalprecip_mm = value.toFloat();
		}
		if (currentKey == "avghumidity") {
			data1[currentForecast].avghumidity = value.toInt();
		}
		if (currentKey == "text") {
			data1[currentForecast].text = value;
		}
		if (currentKey == "code") {
			data1[currentForecast].code = value;
			data1[currentForecast].iconMeteoCon = getMeteoconIcon(value);
			currentForecast++;
		}
	}
}

void APIXUWeather::endArray() {
}

void APIXUWeather::startObject() {
	currentParent = currentKey;
}

void APIXUWeather::endObject() {
}

void APIXUWeather::endDocument() {
}

void APIXUWeather::startArray() {
}


String APIXUWeather::getMeteoconIcon(String code) {

	if (code == "1000") { return "B2"; }      //   Sunnyt           ;        Clear
	if (code == "1003") { return "HI"; }      //I   Partly cloudy-day;        Partly cloudy-night. Code: 116
	if (code == "1006") { return "N5"; }      //5   Cloudy-day;        Cloudy-night. Code: 119
	if (code == "1009") { return "Y%"; }      //%   Overcast-day;        Overcast-night. Code: 122
	if (code == "1030") { return "M9"; }      //9   Mist-day;        Mist-night. Code: 143
	if (code == "1063") { return "Q7"; }      //7   Patchy rain possible-day;        Patchy rain possible-night. Code: 176
	if (code == "1066") { return "U\""; }      //"   Patchy snow possible-day;        Patchy snow possible-night. Code: 179
	if (code == "1069") { return "V\""; }      //"   Patchy sleet possible-day;        Patchy sleet possible-night. Code: 182
	if (code == "1072") { return "Q7"; }      //7   Patchy freezing drizzle possible-day;        Patchy freezing drizzle possible-night. Code: 185
	if (code == "1087") { return "P6"; }      //6   Thundery outbreaks possible-day;        Thundery outbreaks possible-night. Code: 200
	if (code == "1114") { return "U\""; }      //"   Blowing snow-day;        Blowing snow-night. Code: 227
	if (code == "1117") { return "S9"; }      //9   Blizzard-day;        Blizzard-night. Code: 230
	if (code == "1135") { return "JK"; }      //K   Fog-day;        Fog-night. Code: 248
	if (code == "1147") { return "JK"; }      //K   Freezing fog-day;        Freezing fog-night. Code: 260
	if (code == "1150") { return "Q7"; }      //7   Patchy light drizzle-day;        Patchy light drizzle-night. Code: 263
	if (code == "1153") { return "Q7"; }      //7   Light drizzle-day;        Light drizzle-night. Code: 266
	if (code == "1168") { return "Q7"; }      //7   Freezing drizzle-day;        Freezing drizzle-night. Code: 281
	if (code == "1171") { return "R8"; }      //8   Heavy freezing drizzle-day;        Heavy freezing drizzle-night. Code: 284
	if (code == "1180") { return "Q7"; }      //7   Patchy light rain-day;        Patchy light rain-night. Code: 293
	if (code == "1183") { return "Q7"; }      //7   Light rain-day;        Light rain-night. Code: 296
	if (code == "1186") { return "R8"; }      //8   Moderate rain at times-day;        Moderate rain at times-night. Code: 299
	if (code == "1189") { return "R8"; }      //8   Moderate rain-day;        Moderate rain-night. Code: 302
	if (code == "1192") { return "X$"; }      //$   Heavy rain at times-day;        Heavy rain at times-night. Code: 305
	if (code == "1195") { return "X$"; }      //$   Heavy rain-day;        Heavy rain-night. Code: 308
	if (code == "1198") { return "Q7"; }      //7   Light freezing rain-day;        Light freezing rain-night. Code: 311
	if (code == "1201") { return "R8"; }      //8   Moderate or heavy freezing rain-day;        Moderate or heavy freezing rain-night. Code: 314
	if (code == "1204") { return "Q7"; }      //7   Light sleet-day;        Light sleet-night. Code: 317
	if (code == "1207") { return "R8"; }      //8   Moderate or heavy sleet-day;        Moderate or heavy sleet-night. Code: 320
	if (code == "1210") { return "U\""; }      //"   Patchy light snow-day;        Patchy light snow-night. Code: 323
	if (code == "1213") { return "U\""; }      //"   Light snow-day;        Light snow-night. Code: 326
	if (code == "1216") { return "W#"; }      //#   Patchy moderate snow-day;        Patchy moderate snow-night. Code: 329
	if (code == "1219") { return "W#"; }      //#   Moderate snow-day;        Moderate snow-night. Code: 332
	if (code == "1222") { return "W#"; }      //#   Patchy heavy snow-day;        Patchy heavy snow-night. Code: 335
	if (code == "1225") { return "W#"; }      //#   Heavy snow-day;        Heavy snow-night. Code: 338
	if (code == "1237") { return "GG"; }      //G   Ice pellets-day;        Ice pellets-night. Code: 350
	if (code == "1240") { return "Q7"; }      //7   Light rain shower-day;        Light rain shower-night. Code: 353
	if (code == "1243") { return "R8"; }      //8   Moderate or heavy rain shower-day;        Moderate or heavy rain shower-night. Code: 356
	if (code == "1246") { return "Q7"; }      //7   Torrential rain shower-day;        Torrential rain shower-night. Code: 359
	if (code == "1249") { return "Q7"; }      //7   Light sleet showers-day;        Light sleet showers-night. Code: 362
	if (code == "1252") { return "R8"; }      //8   Moderate or heavy sleet showers-day;        Moderate or heavy sleet showers-night. Code: 365
	if (code == "1255") { return "U\""; }      //"   Light snow showers-day;        Light snow showers-night. Code: 368
	if (code == "1258") { return "W#"; }      //#   Moderate or heavy snow showers-day;        Moderate or heavy snow showers-night. Code: 371
	if (code == "1261") { return "GG"; }      //G   Light showers of ice pellets-day;        Light showers of ice pellets-night. Code: 374
	if (code == "1264") { return "GG"; }      //G   Moderate or heavy showers of ice pellets-day;        Moderate or heavy showers of ice pellets-night. Code: 377
	if (code == "1273") { return "Z&"; }      //&   Patchy light rain with thunder-day;        Patchy light rain with thunder-night. Code: 386
	if (code == "1276") { return "Z&"; }      //&   Moderate or heavy rain with thunder-day;        Moderate or heavy rain with thunder-night. Code: 389
	if (code == "1279") { return "U\""; }      //"   Patchy light snow with thunder-day;        Patchy light snow with thunder-night. Code: 392
	if (code == "1282") { return "W#"; }      //#   Moderate or heavy snow with thunder-day;        Moderate or heavy snow with thunder-night. Code: 395
	return "))";   // Nothing matched: N/A
}
