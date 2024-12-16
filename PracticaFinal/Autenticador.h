#ifndef AUTENTICADOR_H
#define AUTENTICADOR_H

#include "MQTTHandler.h"

class Autenticador {
private:
    int pin;                    // Pin del interruptor magnético
    unsigned long lastTime;      // Última vez que se envió el mensaje
    const unsigned long interval = 500;  // Intervalo de 0.5 segundos (500 milisegundos)
    MQTTHandler& mqttHandler;    // Referencia a la instancia de MQTTHandler

public:
    // Constructor
    Autenticador(int pin, MQTTHandler& mqtt)
        : pin(pin), lastTime(0), mqttHandler(mqtt) {
    }

    // Método para comprobar el estado del interruptor
    void comprobarEstado() {
        bool estadoActual = digitalRead(pin);  // Leer el estado actual del interruptor
        
        // Si el interruptor está activo y ha pasado el tiempo necesario (0.5 segundos)
        if (estadoActual == HIGH && (millis() - lastTime >= interval)) {
            // Publicar mensaje MQTT solo cuando el interruptor se activa y ha pasado el intervalo
            String jsonPayload = "{\"state\": {\"reported\": {\"Autenticado\": 1}}}";
            mqttHandler.publish("$aws/things/MyThing/shadow/update", jsonPayload.c_str());
            Serial.println("Interruptor magnético activado. Mensaje publicado.");
            
            // Actualizar la última vez que se envió el mensaje
            lastTime = millis();
        }
    }
};

#endif
