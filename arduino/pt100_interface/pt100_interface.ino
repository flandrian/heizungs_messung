#include <Adafruit_MAX31865.h>

static const int NUM_SENSORS = 2;

Adafruit_MAX31865 sensor_0 = Adafruit_MAX31865(2, 10, 11, 12);
Adafruit_MAX31865 sensor_1 = Adafruit_MAX31865(3, 10, 11, 12);

Adafruit_MAX31865 *sensors[NUM_SENSORS] = {&sensor_0, &sensor_1};

// The value of the Rref resistor. Use 430.0!
static const int RREF = 430;
static const size_t FIRST_NODE = 10;

static const size_t MEASUREMENT_PERIOD_MS = 600000;
#define FLOAT_BUFFER_SIZE 10
char float_buffer[FLOAT_BUFFER_SIZE];

void readSensor(Adafruit_MAX31865* sensor, size_t node_index);

void setup() {
  Serial.begin(115200);
  Serial.println("Adafruit MAX31865 PT100 Sensor Test!");

  sensor_0.begin(MAX31865_2WIRE);
  sensor_1.begin(MAX31865_4WIRE);
}

void loop() {
  // put your main code here, to run repeatedly:
  size_t node_index = FIRST_NODE;
  for (auto sensor : sensors)
  {
    readSensor(sensor, node_index);
    node_index++;
  }
  delay(MEASUREMENT_PERIOD_MS);
}

int ftoa(char *a, float f)  //translates floating point readings into strings to send over the air
{
  int length;
  int left = int(f);
  float decimal = f - left;
  int right = decimal * 100; //2 decimal points
  if (right > 10) {  //if the decimal has two places already. Otherwise
    length = sprintf(a, "%d.%d", left, right);
  } else {
    length = sprintf(a, "%d.0%d", left, right); //pad with a leading 0
  }
  return length;
}

void readSensor(Adafruit_MAX31865* sensor, size_t node_index)
{
  uint16_t rtd = sensor->readRTD();
  float temperature = sensor->temperature(100, RREF);
  Serial.print("{\"node\":");
  Serial.print(node_index);
  Serial.print(",");

  Serial.print("\"temperature\":");
  ftoa(float_buffer, temperature);
  Serial.print(float_buffer);

  Serial.println("}");
  // Check and print any faults
//  uint8_t fault = sensor->readFault();
//  if (fault) {
//    sensor->clearFault();
//  }
}


