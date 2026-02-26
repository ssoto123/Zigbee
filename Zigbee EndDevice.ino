/* =========================================================================
   CÓDIGO DIDÁCTICO: ZIGBEE LUZ / ACTUADOR (CON RELEVADOR)
   Materia: Internet de las Cosas
   Placa: ESP32-C6
   Rol: Router (Recomendado) o End Device
   Propósito: Recibir comandos ON/OFF de un Switch Zigbee y activar un Foco.
   ========================================================================= */

// --- BLOQUE 1: LAS DEFENSAS DEL CÓDIGO (Directivas de Preprocesador) ---
// Mecanismo de seguridad a nivel de compilador. Es como un "guardia de seguridad"
// que revisa si el alumno configuró correctamente el Arduino IDE antes de compilar.
// Si olvidaron seleccionar el modo Router o End Device, lanza un error y detiene todo.
#ifndef ZIGBEE_MODE_ED
#ifndef ZIGBEE_MODE_ZCZR
#error "Zigbee device mode is not selected in Tools->Zigbee mode"
#endif
#endif

#include "Zigbee.h"

// --- BLOQUE 2: ENDPOINTS Y EL "MODELO DE OBJETOS" ZIGBEE ---
// Definimos el pin físico que controlará el relevador (módulo del foco).
const uint8_t PIN_RELEVADOR = 8; 

// El concepto de Endpoint: Si la dirección física (MAC) del chip es un "edificio", 
// el Endpoint es el "número de departamento". 
// Le decimos al código que nuestro foco vive en el departamento 10, 
// porque el Switch está programado para enviarle órdenes exactamente a ese número.
const uint8_t ENDPOINT_LUZ = 10;

// Programación Orientada a Objetos (POO): Creamos nuestro objeto 'zbLuz' 
// usando la plantilla (clase) ZigbeeLight proporcionada por Espressif.
ZigbeeLight zbLuz = ZigbeeLight(ENDPOINT_LUZ);

// --- BLOQUE 3: LA MAGIA ASÍNCRONA (La Función Callback) ---
// En lugar de preguntar constantemente en el loop "¿Ya llegó el mensaje?" (Polling),
// usamos un Callback. Es como darle tu número de teléfono a la paquetería: 
// "No me interrumpas ahora, pero cuando llegue la orden, llámame a esta función".
// El "cerebro" Zigbee ejecutará esta función automáticamente al recibir un comando.
void accionarRelevador(bool estado) {
  
  if (estado == true) {
    Serial.println("[ACTUADOR] Comando recibido: ENCENDER");
    
    // LÓGICA DEL RELEVADOR (ATENCIÓN ALUMNOS): 
    // Muchos módulos relevador optoacoplados son "Active-LOW" (encienden con 0V).
    // Si tu foco se enciende cuando debería apagarse, cambia este HIGH por LOW.
    digitalWrite(PIN_RELEVADOR, HIGH); 
  } 
  else {
    Serial.println("[ACTUADOR] Comando recibido: APAGAR");
    digitalWrite(PIN_RELEVADOR, LOW);
  }
}

// --- BLOQUE 4: EL SETUP (Registrando el Dispositivo) ---
void setup() {
  Serial.begin(115200);
  delay(1000); // Pequeña pausa para estabilizar voltajes al arrancar

  Serial.println("\n--- NODO ACTUADOR (LUZ) INICIANDO ---");

  // Configuramos el pin del relevador como SALIDA física
  pinMode(PIN_RELEVADOR, OUTPUT);
  // Por seguridad, iniciamos forzando el foco a estar apagado
  digitalWrite(PIN_RELEVADOR, LOW); 

  // Opcional: Nombre del dispositivo para que el Coordinador lo identifique
  zbLuz.setManufacturerAndModel("ClaseIng", "FocoRelevador");

  // EL CONTRATO (VINCULACIÓN): Conectamos nuestro objeto Zigbee con la función Callback.
  // Le decimos: "Cuando te ordenen cambiar de estado, ejecuta 'accionarRelevador'".
  zbLuz.onStateChange(accionarRelevador);

  // Entregamos los planos de nuestro "departamento" al administrador Zigbee
  // usando el Ampersand (&) para pasar la dirección de memoria (puntero).
  Zigbee.addEndpoint(&zbLuz);

  // Encendemos la antena de radio. Aquí inicia el proceso de "Commissioning"
  // (el chip busca unirse a la red creada por el Coordinador).
  if (!Zigbee.begin()) {
    Serial.println("[ERROR] El radio Zigbee no pudo iniciar.");
    while(1); // Bucle infinito de bloqueo si hay una falla crítica de hardware
  }

  Serial.println("[SISTEMA] Zigbee iniciado correctamente. Procesando en 2do plano.");
  Serial.println("[SISTEMA] Esperando comandos desde el Switch...");
}

// --- BLOQUE 5: EL LOOP MISTERIOSAMENTE VACÍO (RTOS) ---
void loop() {
  // ¿Por qué el programa funciona si el loop está prácticamente vacío? 
  // Porque el ESP32 usa FreeRTOS (Un Sistema Operativo en Tiempo Real).
  // El manejo del protocolo Zigbee se está ejecutando en una "Tarea" (hilo) invisible.
  
  // El delay(100) no es inútil: le dice al procesador que esta tarea principal 
  // está libre, permitiendo que le asigne el 100% de la energía a la antena de radio 
  // y evita que el "Perro Guardián" (Watchdog Timer) reinicie el chip por asfixia.
  delay(100); 
}