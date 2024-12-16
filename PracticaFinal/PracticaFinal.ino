//#include "WiFiHandler.h"
#include <WiFi.h>
#include <WiFiManager.h>
#include "MQTTHandler.h"
#include "ServoController.h"
#include "LEDHandler.h"
#include "LaserSensor.h"
#include "Autenticador.h"



// MQTT broker details
const char* MQTT_BROKER = "a5ji8fpy1x6e7-ats.iot.us-east-1.amazonaws.com";
const int MQTT_PORT = 8883;
const char* CLIENT_ID = "ESP-32";

  // Certificate and key
const char AMAZON_ROOT_CA1[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
rqXRfboQnoZsG4q5WTP468SQvvG5
-----END CERTIFICATE-----
)EOF";

const char CERTIFICATE[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----
MIIDWTCCAkGgAwIBAgIUVVccSwUjni6/DNAY8VP4CCXUpNwwDQYJKoZIhvcNAQEL
BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g
SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTI0MTAyNjIyMjMy
OFoXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0
ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAKQ95cMeALW23nZXnGOR
Rlhx5fYTrAzFpwJFV7cdyxfsQmvcejKutu+k9Lkh0m+A6aR4R754mqBv/quYaZ3M
YUWX0aseFFGdvENmDW5LoKAlTTKhNdESBBmHRZ7tIX2k8Sh7Qp8hMyzUHtz6sbRw
a8ETJeTTrDA8C/pQZsNfmNGILhbKSt5zFoaOBo2dAOEFEcXGBuZYvDJY0RiOvvy2
K9bM4YLaVE5e+k7IFgB3uCJWJSJ8yb8zd4mB3uoq7EzV7YIA6QpTvO/hGj8kv6hU
IdcHdoGMYy5d47BHPE0/zJMMBNRGA9wORVomUwiX0CoekoBAT5AW4qfb0qJq9eIr
5oECAwEAAaNgMF4wHwYDVR0jBBgwFoAUL3kARBk40cftq28srpYCKsGk6m4wHQYD
VR0OBBYEFMzxJ9F4udBX4MYbUJExEJC2zHVEMAwGA1UdEwEB/wQCMAAwDgYDVR0P
AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQCsSXt0VQZa5P1QFFEwwsaJLNb4
LVRPZh9sYWx2FTKjVeMxAoO8baDQovxTFwM2Pvfg5DwI42WHHcqbbDF90wDo9mir
835MBxqIuN2YbC9AGncXN7btDpONm5t6gdARliTL6VLWDGCAOgtfTmn/m7mhWo07
4/gjTx9+fZXPihIs4XDLaVJNd5f6MwPYtX7ofRNQ7bhbsKSDrRWUwi6qza73v2iT
U2bwcI76eKox9NXtUkFz/sReKXIy8Qb+8r0adVIoiRQ0U+MymTVTXaCKVpKCO9Na
3+rWHPiPrduxer8savmH0QIXwaoiiuWLl+18PZOoHHuB9m++Qn9ZQi0EWflt
-----END CERTIFICATE-----
)KEY";

const char PRIVATE_KEY[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----
MIIEogIBAAKCAQEApD3lwx4AtbbedlecY5FGWHHl9hOsDMWnAkVXtx3LF+xCa9x6
Mq6276T0uSHSb4DppHhHvniaoG/+q5hpncxhRZfRqx4UUZ28Q2YNbkugoCVNMqE1
0RIEGYdFnu0hfaTxKHtCnyEzLNQe3PqxtHBrwRMl5NOsMDwL+lBmw1+Y0YguFspK
3nMWho4GjZ0A4QURxcYG5li8MljRGI6+/LYr1szhgtpUTl76TsgWAHe4IlYlInzJ
vzN3iYHe6irsTNXtggDpClO87+EaPyS/qFQh1wd2gYxjLl3jsEc8TT/MkwwE1EYD
3A5FWiZTCJfQKh6SgEBPkBbip9vSomr14ivmgQIDAQABAoIBABdDmT75CfqzS3GR
2VoVItS2VW90u+MXQB/HOLR8aN8bnRDwKvLw4oKxZ+StUMRwiye7zdXB+Y0OMBGY
RiWO5JxZ8938Jb6lzBvz4aUk2zyz6+pnAVt65M/E5GythBfq/CnSw84MgtYvuatt
ayx23Bx+HNj7zqEKcFvldZjo+pl7KYsxkzzJm+5DHP5XBZo6ZPkbmwgG2GLgOO9u
XsasCDt5IrpWIXoIH2IKSvqwX6bq7uPJSwW7G1uzXBntOODbpu6H4Ppi8WpTj8S+
IG5AA+J3F/OoqWUhDlkZx5Mt99DnQ7PdAuVCmwgQNi+3Ory/tXZIsUU2zQsn3DRQ
CJNZBQECgYEA0F1FdO0pBEYzLzASVmlpRoM0NTTZPnmoKWwjstxDnUAH7SAlek7+
7Lzs9z8LdDp4sACyEHxOUHQDaf8zk7itQCVaOzZc26oNTpo0lPpTRNo58cXnWT/L
3GqfV7tPdbS+F+3MBSpB0M/+U1tPmhJeGJid0J8w+bXb4Fg992erkxECgYEAycpP
aBeZMsTAkR0Qxp3izMpfLvE7h33SxfaCiZotzzdXfqw0LlodlI5c0MuItzLGxElc
OgvuFdNtMuPQk+UxpUbXuZiexqc7H/6tc9T1Mb8yG+QVai6z6GGWjkEYRXh8n+am
X5f49PnrmPsmHP9/xif+8n4QdGCWXuJlVml4PHECgYAGuwyUU1jg13etzYq6GduZ
uIFGOrfNydZByEl67JyOhtP6t/Ad/FFFynCJKBMxpPXjEZKu76UD+ktl8CV1XyDR
kqNQiFgFjH+zi02phlCMR9RZWAA6JNJsA1G0XsuZTMZbNYZCoHPRz/YyMR/oJTG+
76chZxMI7fP1UkRmKoYjUQKBgDctGolb6of3RLBpQi0M/vcAXwZ279Acl4WJ4ie+
AMioWURJ0fraqTv1sWtmO/vb5n3FkXJN0MwnA7TmliHaibinplUZZlDraT9WZBdI
I8N2hD0cIL7oBkmEZaaAJiLYzzrp1pRM+cYCkGlEQyeqtUV5qjykO9uWjYdkYFFA
haABAoGAKluQT19vaQeVoLaxfHbqkeoTce+ciq2oucm/PfaZX03227zjpOOOkpKn
KfvPwy9m0Ehc35z2kzX3rDS34zRKMdlRZefojl3wRgpGAqo1IicLWrVDy2IW64Pt
s5AMKCOzWQyGgopaccmYAlahALv+wgm5opSW+BA/F66u3H+OBoA=
-----END RSA PRIVATE KEY-----
)KEY";

// MQTT topics
const char* UPDATE_TOPIC = "$aws/things/MyThing/shadow/update";
const char* UPDATE_DELTA_TOPIC = "$aws/things/MyThing/shadow/update/delta";

// Pin definitions
const int SERVO_PIN = 18;
const int RED_LED_PIN = 12;
const int BLUE_LED_PIN = 14;
const int LASER_SENSOR_PIN = 34;
const int INTERRUPTOR_PIN = 21;

// Global objects
WiFiManager wifiManager;
WiFiClientSecure wifiClient;
MQTTHandler mqttHandler(MQTT_BROKER, MQTT_PORT, CLIENT_ID, UPDATE_TOPIC, UPDATE_DELTA_TOPIC, wifiClient);
LEDHandler ledHandler(RED_LED_PIN, BLUE_LED_PIN);
ServoController servoController(SERVO_PIN, mqttHandler, ledHandler);
LaserSensor laserSensor(LASER_SENSOR_PIN, servoController, mqttHandler);
Autenticador autenticador(INTERRUPTOR_PIN, mqttHandler);

void callback(char* topic, byte* payload, unsigned int length) {
    String message;
    for (int i = 0; i < length; i++) message += String((char)payload[i]);
    Serial.println("Message from topic " + String(topic) + ": " + message);
    
    StaticJsonDocument<JSON_OBJECT_SIZE(64)> inputDoc;
    DeserializationError err = deserializeJson(inputDoc, payload);
    if (!err) {
        if (String(topic) == UPDATE_DELTA_TOPIC) {
            int newPosition = inputDoc["state"]["servoPosition"].as<int>();
            servoController.setPosition(newPosition);
        }
    }
}

void setup() {
    Serial.begin(115200);
    
    pinMode(INTERRUPTOR_PIN, INPUT);

    // Setup connections
    wifiManager.autoConnect("ESP32-Config");
    Serial.println("Conectado a la red Wi-Fi");
    Serial.print("Direccion IP: ");
    Serial.println(WiFi.localIP());
    Serial.print("Conectado a la red: ");
    Serial.println(WiFi.SSID());
    

    mqttHandler.setup(AMAZON_ROOT_CA1, CERTIFICATE, PRIVATE_KEY);
    mqttHandler.setCallback(callback);
    
    // Setup hardware
    servoController.setup();
}

void loop() {
    if (!mqttHandler.isConnected()) {
        mqttHandler.connect();
    }
    mqttHandler.loop();
    
    servoController.updateLEDs();
    laserSensor.checkLaser();
    autenticador.comprobarEstado();
}