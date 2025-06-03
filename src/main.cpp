#include <Arduino.h>
#include <secrets.h>
#include <Logger.h>
#include <WiFi.h>
#include <ElegantOTA.h>
#include <LittleFS.h>
#include <time.h>
#include "weather.h"

byte valve_pins[] = {0, 1}; // pin headers for valve relays

#define NUM_VALVES sizeof(valve_pins)
#define LOG_BUFFER_SIZE 256

const char *ssid = WIFI_SSID;
const char *password = WIFI_PASS;
const char *ntpServer = "pool.ntp.org";
String device_name = "GardenIrrigation"; // NO SPACES

int frequency[NUM_VALVES] = {1, 3};                    // Days between irrigation
float irrigation_rate[NUM_VALVES] = {4.0, 600};        // litres/hr
float irrigation_area[NUM_VALVES] = {0.1256637060, 6}; // m2 for in-ground
float Kc[NUM_VALVES] = {0.5, 1};                       // crop coefficients

unsigned long update = 0;

AsyncWebServer server(80);

struct IrrigationTask
{
  int valve;
  unsigned long duration_ms;
  time_t scheduled_time;
};

std::vector<IrrigationTask> irrigationQueue;

unsigned long getTime()
{
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo))
  {
    Log.println("Failed to obtain time");
    return (0);
  }
  time(&now);
  return now;
}

String historyFilePath(int i)
{
  return "/history" + String(i) + ".txt";
}

void saveCompletedTask(int valve, const IrrigationTask &task)
{
  File f = LittleFS.open(historyFilePath(valve), "w");
  if (!f)
  {
    Log.printf("[Task %d] Failed to write file\n", valve);
    return;
  }
  // Save valve, duration_ms, scheduled_time as CSV: valve,duration_ms,scheduled_time
  f.printf("%lu,%lu\n", task.duration_ms, (unsigned long)task.scheduled_time);
  f.close();
}

void clearHistory(int valve)
{
  IrrigationTask task;
  task.valve = valve;
  task.duration_ms = 0;
  task.scheduled_time = 10000;
  saveCompletedTask(valve, task);
  Log.printf("Valve %d history cleared!", valve);
}

void clearAllHistory()
{
  Log.println("Clearing history...");
  IrrigationTask task;
  for (int i = 0; i < NUM_VALVES; ++i)
  {
    task.valve = i;
    task.duration_ms = 0;
    task.scheduled_time = 10000;
    saveCompletedTask(i, task);
  }
  Log.println("History Cleared!");
}

IrrigationTask loadHistory(int valve)
{
  IrrigationTask task;
  task.valve = valve; // Always set the valve ID

  File f = LittleFS.open(historyFilePath(valve), "r");
  if (!f)
  {
    Log.printf("[Task %d] File not found, initializing file...\n", valve);
    clearHistory(valve);
    loadHistory(valve);
  }

  String line = f.readStringUntil('\n');
  f.close();

  int comma = line.indexOf(',');
  if (comma == -1)
  {
    Log.printf("[Task %d] Malformed task line: %s\n", valve, line.c_str());
    task.duration_ms = 0;
    task.scheduled_time = 0;
    return task;
  }

  // Parse the values: duration, time
  task.duration_ms = line.substring(0, comma).toInt();
  task.scheduled_time = (time_t)line.substring(comma + 1).toInt();

  return task;
}

void scheduleIrrigation(int valve, float mm_needed, time_t when)
{
  float litres_needed = mm_needed * irrigation_area[valve];
  float time_hr = litres_needed / irrigation_rate[valve];
  unsigned long duration_ms = (unsigned long)(time_hr * 3600000);

  irrigationQueue.push_back({valve, duration_ms, when});

  char timeStr[20];
  strftime(timeStr, sizeof(timeStr), "%m/%d/%Y, %H:%M:%S", localtime(&when));

  Log.printf("[Valve %d] Queued %.2f L @ %s for (%lu ms)\n",
             valve, litres_needed, timeStr, duration_ms);
}

void executeIrrigationTasks()
{
  static bool running = false;
  static unsigned long end_time = 0;
  static int current_valve = -1;

  if (!running && !irrigationQueue.empty())
  {
    time_t now = getTime();
    for (size_t i = 0; i < irrigationQueue.size(); i++)
    {
      if (irrigationQueue[i].scheduled_time <= now)
      {
        current_valve = irrigationQueue[i].valve;
        digitalWrite(valve_pins[current_valve], HIGH);
        end_time = millis() + irrigationQueue[i].duration_ms;
        float litres = (irrigation_rate[current_valve] * (end_time - millis()) / 3600000.0);
        Log.printf("[Valve %d] Started for %lu s, giving %.2f L\n", current_valve, irrigationQueue[i].duration_ms / 1000, litres);
        saveCompletedTask(current_valve, irrigationQueue[i]); // saving as completed before it is but hey ho
        irrigationQueue.erase(irrigationQueue.begin() + i);
        running = true;
        break;
      }
    }
  }

  if (running && millis() >= end_time)
  {
    digitalWrite(valve_pins[current_valve], LOW);
    Log.printf("[Valve %d] Finished\n", current_valve);
    running = false;
  }
}

void evaluateIrrigationSchedule(const WeatherData &data)
{
  time_t now = getTime();

  for (int i = 0; i < NUM_VALVES; i++)
  {
    Log.printf("Evaluating irrigation for valve %d...\n", i);
    IrrigationTask last = loadHistory(i);
    if (now - (last.scheduled_time - 3600) > (frequency[i] * 24 * 60 * 60))
    { // if more than the irrigation frequency has passed since last irrigation - 1hr (sunset change forgiveness)
      int data_size = data.sunrise.size();
      float et0_sum = 0;
      float precipitation_sum = 0;
      for (size_t j = 7 - frequency[i]; j < data_size - 1; ++j)
      {
        et0_sum += data.evapotranspiration[j];
        precipitation_sum += data.precipitation[j];
      }
      float water_needed = (et0_sum * Kc[i]) - precipitation_sum - data.precipitation.back(); // mm
      Log.printf("frequency: %d, et0_sum: %.2f, Kc: %.2f, precipitation_sum: %.2f, water_needed: %.2f\n", frequency[i], et0_sum, Kc[i], precipitation_sum, water_needed);
      scheduleIrrigation(i, water_needed, data.sunrise.back());
    }
  }
}

void serialCommands(uint8_t *data, size_t len)
{
  Log.println("Received Data...");
  String command = "";
  for (size_t i = 0; i < len; i++)
  {
    command += char(data[i]);
  }
  if (command == "reset")
  {
    clearAllHistory();
  }
  else if (command == "test")
  {
    time_t now = getTime();
    scheduleIrrigation(0, 0.25, now);
    scheduleIrrigation(1, 1, now);
  }
  else if (command == "queue")
  {
    if (irrigationQueue.empty())
    {
      Log.println("Irrigation queue is empty.");
    }
    else
    {
      Log.printf("Irrigation queue has %d item(s):\n", irrigationQueue.size());
      for (size_t i = 0; i < irrigationQueue.size(); ++i)
      {
        const IrrigationTask &task = irrigationQueue[i];
        char timeStr[20];
        strftime(timeStr, sizeof(timeStr), "%m/%d/%Y, %H:%M:%S", localtime(&task.scheduled_time));
        Log.printf("  [%d] Valve: %d | Duration: %lu ms | Scheduled: %s\n",
                   (int)i, task.valve, task.duration_ms, timeStr);
      }
    }
  }
  else if (command == "update")
  {
    char timeStr[20];
    strftime(timeStr, sizeof(timeStr), "%m/%d/%Y, %H:%M:%S", localtime((time_t *)&update));
    Log.println(timeStr);
  }
  else
  {
    Log.printf("Unknown command: %s", command);
  }
}

void setup(void)
{
  delay(2000);
  Serial.begin(115200);
  WiFi.hostname(device_name.c_str());
  Serial.println("Attempting wifi connection...");
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    Serial.println("WiFi Failed!");
    return;
  }
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.print("MAC: ");
  Serial.println(WiFi.macAddress());
  configTime(0, 0, ntpServer);
  WebSerial.begin(&server);
  WebSerial.onMessage(serialCommands);
  ElegantOTA.begin(&server);
  server.begin();

  LittleFS.begin(true);
  for (int i = 0; i < NUM_VALVES; i++)
    pinMode(i, OUTPUT);

  // clearAllHistory();
}

void loop(void)
{
  time_t now = getTime();
  ElegantOTA.loop();
  if (now >= update)
  {

    WeatherData weather;
    if (getWeather(weather))
    {
      evaluateIrrigationSchedule(weather);
      update = weather.sunrise.back() + (24 * 60 * 60);
    }
    else
    {
      Log.println("Weather fetch failed");
    }
  }
  executeIrrigationTasks();
}
