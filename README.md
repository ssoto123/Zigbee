# 🌐 Proyecto Didáctico: Red Zigbee Básico con ESP32-C6 (Interruptor y Relevador)

Este repositorio contiene el código fuente y las instrucciones para implementar una red inalámbrica Zigbee básica utilizando microcontroladores ESP32-C6. El proyecto está diseñado con fines educativos para comprender la arquitectura de nodos, la configuración de roles (Coordinador y Enrutador) y la interacción de hardware en aplicaciones de Internet de las Cosas (IoT).

---

## 👨‍🏫 Datos del Docente / Autor
* **Autor:** MGTI. Saul Isai Soto Ortiz
---

## 🛠️ Requisitos del Sistema (Hardware y Software)

### Hardware Necesario
* 2x Tarjetas de desarrollo **ESP32-C6** (Una actuará como Interruptor/Coordinador y otra como Foco/Actuador).
* 1x Módulo Relevador (Relay) Optoacoplado de 1 Canal (5V o 3.3V).
* Cables jumper (Dupont).
* Foco y socket para pruebas de corriente alterna (Con precaución y bajo supervisión).

### Software Necesario
* **Arduino IDE** (Versión 2.x recomendada).
* **ESP32 Core de Espressif**: Versión **3.3.7**. *(Versiones diferentes pueden tener incompatibilidades con las librerías Zigbee).*

---

## ⚙️ Configuración del Arduino IDE (¡Importante!)

El ESP32-C6 requiere configuraciones específicas de memoria y particiones para que el radio Zigbee funcione correctamente. Sigue estas instrucciones al pie de la letra antes de compilar.


### 1. Configuración para el Nodo "Interruptor" (Coordinador)
El Coordinador es el "Jefe" de la red. Es el encargado de crear la red Zigbee y permitir que otros dispositivos se unan.

Ve al menú **Herramientas (Tools)** y verifica lo siguiente:
* **Board:** ESP32C6 Dev Module
* **Zigbee mode:** `Zigbee ZCZR (Coordinator/Router)`
* **Partition Scheme:** `Zigbee 4MB with spiffs`
* **Erase All Flash Before Sketch Upload:** `Enabled` *(Crucial para limpiar configuraciones de memoria previas).*

### 2. Configuración para el Nodo "Foco/Relevador" (Router / End Device)
Este nodo se unirá a la red creada por el Coordinador y esperará órdenes.

Ve al menú **Herramientas (Tools)** y verifica lo siguiente:
* **Board:** ESP32C6 Dev Module
* **Zigbee mode:** `Zigbee Router` (Recomendado) o `Zigbee ED`
* **Partition Scheme:** `Zigbee 4MB with spiffs`
* **Erase All Flash Before Sketch Upload:** `Enabled`

---

## 🔌 Conexiones de Hardware

### En el Nodo Actuador (Foco/Relevador)
Usaremos el **GPIO 8** para enviar la señal de control.
* **VCC / DC+** del Relay ➡️ Pin **5V** (o VIN) del ESP32-C6.
* **GND / DC-** del Relay ➡️ Pin **GND** del ESP32-C6.
* **IN / S** del Relay ➡️ Pin **GPIO 8** del ESP32-C6.

### En el Nodo Coordinador (Interruptor)
No se requiere cableado extra. Utilizaremos el botón físico integrado en la placa (marcado como **BOOT** o **USER**), el cual está mapeado internamente al **GPIO 9**.

---

## 📡 Interacción entre los Códigos: ¿Cómo funciona?

El sistema utiliza un modelo de comunicación asíncrona basado en "Endpoints" (Puntos finales), que actúan como "números de departamento" dentro de cada microcontrolador.


### 1. El Emisor (Coordinador / Switch)
Cuando presionas el botón BOOT, el código filtra el ruido eléctrico (Debounce) y ejecuta el siguiente comando:

```cpp
zbSwitch.lightToggle();
```
Este comando estándar de Zigbee se transmite por el aire. Significa: *"Cualquier luz (Endpoint 10) que esté en mi red, invierta su estado actual"*. Al Coordinador no le importa cuántos focos hay; él solo emite la orden.

### 2. El Receptor (Router / Relevador)
El ESP32-C6 receptor ejecuta un Sistema Operativo en Tiempo Real (FreeRTOS) que escucha la radio en segundo plano. Al arrancar, vinculamos una función (Callback) usando:

```cpp
zbLuz.onStateChange(accionarRelevador);
```

Cuando la orden `Toggle` viaja por el aire y llega al nodo receptor, el "cerebro" Zigbee interrumpe lo que está haciendo y llama automáticamente a la función `accionarRelevador(bool estado)`. 
Si el estado recibido es `true` (encender), el ESP32 envía una señal HIGH al GPIO 8, cerrando el circuito del relevador y encendiendo el foco real.

---

## 🚀 Pasos para la Demostración en Laboratorio

1. **Flashea** el código del *Coordinador* en la placa 1.
2. **Flashea** el código del *Actuador* en la placa 2.
3. Abre **dos ventanas** del Monitor Serial (a 115200 baudios) para observar ambas placas simultáneamente.
4. Conecta a la corriente ambas placas. Observa en el Coordinador cómo se "Crea la red" y en el Actuador cómo se completa el proceso de "Commissioning" (Unión a la red).
5. Presiona el botón BOOT en la placa 1.
6. Observa el Monitor Serial y escucha el *¡Clic!* del relevador en la placa 2.

> **⚠️ Advertencia de Seguridad:** Si manipulas corriente alterna (110V/220V) a través del módulo relevador, asegúrate de realizar todas las conexiones con el circuito desenergizado y bajo la supervisión del docente.
