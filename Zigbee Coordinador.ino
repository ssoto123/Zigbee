/* =========================================================================
   CÓDIGO DIDÁCTICO: ZIGBEE SWITCH / COORDINADOR
   Materia: Internet de las Cosas (6to Semestre)
   Placa: ESP32-C6
   Rol: Coordinador (El "Jefe" que crea y administra la red Zigbee)
   Propósito: Leer un botón físico y enviar un comando "Toggle" al Relevador.
   ========================================================================= */

// --- BLOQUE 1: LAS DEFENSAS DEL CÓDIGO (Preprocesador) ---
// El "Guardia de Seguridad" del compilador. Si el alumno olvida poner el IDE
// en modo "Zigbee ZCZR" (Coordinator/Router), esto bloquea la subida del código.
// Es vital porque un Coordinador requiere una partición de memoria distinta (zb_storage).
#ifndef ZIGBEE_MODE_ZCZR
#error "¡Alumnos! Seleccionen 'Zigbee ZCZR' en el menú Tools -> Zigbee mode"
#endif

#include "Zigbee.h"

// --- BLOQUE 2: DEFINICIÓN DE HARDWARE Y OBJETOS ZIGBEE ---
// Usaremos el botón físico que ya viene soldado en la placa ESP32-C6 (marcado BOOT).
const uint8_t PIN_BOTON = 9; 

// Endpoint: El "Número de departamento" de nuestro apagador.
const uint8_t ENDPOINT_SWITCH = 1; 

// POO: Creamos el objeto 'zbSwitch'. Este objeto hereda toda la complejidad
// del protocolo Zigbee, permitiéndonos usar comandos sencillos más adelante.
ZigbeeSwitch zbSwitch = ZigbeeSwitch(ENDPOINT_SWITCH);

// --- BLOQUE 3: VARIABLES PARA EL "ANTIRREBOTE" (DEBOUNCE) ---
// CONCEPTO PARA LA CLASE: Cuando presionas un botón mecánico, los contactos de metal
// chocan y rebotan a nivel microscópico, creando ruido eléctrico ("falsos clics").
// 
// Usaremos estas variables para filtrar ese ruido usando tiempo (millis) en lugar de delays.
bool estadoAnteriorBoton = HIGH; // HIGH porque usaremos resistencia PULL-UP
unsigned long ultimoTiempoRebote = 0;
unsigned long retardoRebote = 50; // Esperaremos 50ms a que el botón "se calme"

// --- BLOQUE 4: CONFIGURACIÓN PRINCIPAL (SETUP) ---
void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("\n--- NODO SWITCH (COORDINADOR) INICIANDO ---");

  // Configuramos el pin del botón. Al usar INPUT_PULLUP, activamos una resistencia
  // interna en el chip. Esto significa que cuando NO presionamos el botón leeremos '1' (HIGH),
  // y cuando lo presionamos se conecta a tierra y leeremos '0' (LOW).
  pinMode(PIN_BOTON, INPUT_PULLUP);

  // --- CONFIGURACIÓN ZIGBEE ---
  zbSwitch.setManufacturerAndModel("ClaseIng", "ApagadorMaestro");

  // Inscribimos el objeto Apagador en el directorio interno del stack Zigbee.
  Zigbee.addEndpoint(&zbSwitch);

  // INICIO CRÍTICO: Arrancamos el radio y le decimos que ÉL ES EL COORDINADOR.
  // Solo puede haber UN Coordinador por red. Su trabajo es elegir el canal (frecuencia),
  // crear la red (PAN ID) y permitir que otros (como el foco) se unan.
  // 

[Image of Zigbee star topology]

  Serial.println("[SISTEMA] Creando la red Zigbee...");
  if (!Zigbee.begin(ZIGBEE_COORDINATOR)) {
    Serial.println("[ERROR] Zigbee no inició. Revisa la memoria y particiones.");
    while (1); // Bloqueo de seguridad
  }

  Serial.println("[SISTEMA] Red Zigbee creada correctamente.");
  Serial.println("[SISTEMA] Presiona el botón BOOT para enviar el comando al foco.");
}

// --- BLOQUE 5: BUCLE PRINCIPAL (Lectura Asíncrona) ---
void loop() {
  // Leemos el estado eléctrico instantáneo del pin
  bool lecturaBoton = digitalRead(PIN_BOTON);

  // Lógica de Antirrebote (Software Debouncing)
  // 1. Si el estado cambió (alguien tocó el botón o hubo ruido), reiniciamos el cronómetro.
  if (lecturaBoton != estadoAnteriorBoton) {
    ultimoTiempoRebote = millis();
  }

  // 2. Si ha pasado suficiente tiempo (50ms) sin que el estado cambie, 
  // asumimos que es una pulsación real y no ruido.
  if ((millis() - ultimoTiempoRebote) > retardoRebote) {
    
    // 3. Verificamos si la pulsación real fue "hacia abajo" (LOW = Presionado)
    if (lecturaBoton == LOW) {
      Serial.println("\n[ACCIÓN] ¡Botón presionado!");
      Serial.println("[ZIGBEE] Enviando comando 'TOGGLE' (Alternar)...");
      
      // EL COMANDO ZIGBEE:
      // 'lightToggle()' es un comando estándar (Cluster On/Off). 
      // Manda un mensaje a la red diciendo: "Quien me esté escuchando, invierta su estado".
      // Si el relevador estaba encendido, se apaga. Si estaba apagado, se enciende.
      zbSwitch.lightToggle(); 
      
      // Bloqueamos ligeramente (medio segundo) para que el alumno no ametralle
      // la red enviando 20 comandos seguidos si deja el dedo puesto en el botón.
      delay(500); 
    }
  }

  // Guardamos el estado actual para compararlo en el siguiente ciclo del loop
  estadoAnteriorBoton = lecturaBoton;
}