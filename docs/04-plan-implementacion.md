# Plan de implementación

## Fases y duración estimada

| Fase | Duración | Entregable |
|------|----------|------------|
| 0. Diseño y compras | 1–3 días | BOM confirmado, pinout C3 verificado |
| 1. Banco de pruebas | 3–5 días | 1 canal RMS estable en Serial |
| 2. Dos canales + energía | 3–5 días | kWh acumulado en Serial |
| 3. WiFi + NTP + LittleFS | 3–5 días | Archivos diarios y rotación |
| 4. Servidor HTTP + API | 2–4 días | Endpoints documentados funcionando |
| 5. UI web + calibración | 2–3 días | Dashboard LAN |
| 6. Instalación en campo | 1–2 días | Montaje en tableros, calibración final |

**Total orientativo:** 3–4 semanas a tiempo parcial.

---

## Fase 0 — Preparación

- [ ] Confirmar instalación **monofásica** por casa (220 V, una fase medida).
- [ ] Comprar ESP32-C3 SuperMini, 2× SCT-013, resistencias y condensadores.
- [ ] Instalar PlatformIO o Arduino IDE + soporte ESP32.
- [ ] Reservar IP fija en router (DHCP reservation) para el medidor.

---

## Fase 1 — Un canal en mesa

- [ ] Armar circuito burden + bias para un SCT.
- [ ] Firmware: leer ADC, imprimir RMS por Serial.
- [ ] Probar con carga conocida (foco incandescente / resistencia).

**Criterio de salida:** error de corriente < 10 % vs referencia.

---

## Fase 2 — Dos canales y energía

- [ ] Duplicar front-end; GPIO0 y GPIO1.
- [ ] Módulo `energy_meter`: integrar Wh con `V_NOMINAL` y `POWER_FACTOR`.
- [ ] Log cada minuto: `casa1_i, casa2_i, kwh_hoy`.

---

## Fase 3 — Persistencia

- [ ] Partición LittleFS en PlatformIO.
- [ ] `storage.cpp`: escribir/leer JSON diario.
- [ ] NTP y cierre de día (o simulación cambiando hora).
- [ ] Rotación a 30 archivos por casa.
- [ ] Snapshot `casaN_current.json` cada 5 min.

---

## Fase 4 — Servidor HTTP

- [ ] Conectar WiFi (STA); imprimir IP en Serial.
- [ ] Implementar `/api/status`, `/api/today`, `/api/daily`.
- [ ] Probar con `curl` desde PC en la misma red.

---

## Fase 5 — Dashboard y calibración

- [ ] Página HTML embebida o en LittleFS.
- [ ] Gráfico últimos 7 días (Chart.js ligero o SVG simple).
- [ ] Guardar `cal_gain` / `cal_offset` en NVS.
- [ ] Documentar procedimiento de calibración en README.

---

## Fase 6 — Instalación

- [ ] Instalar SCT en tablero Casa 1 (fase única).
- [ ] Instalar SCT en tablero Casa 2.
- [ ] Fijar caja del ESP; alimentación USB permanente.
- [ ] Calibración con medidor de referencia 24 h.
- [ ] Verificar histórico al día siguiente vía `/api/daily`.

---

## Riesgos y mitigaciones

| Riesgo | Impacto | Mitigación |
|--------|---------|------------|
| ADC ruidoso | Medición inestable | Capacitor 100 nF en ADC; promedio móvil |
| Secundario SCT abierto | Tensión peligrosa | Burden siempre conectada |
| Flash desgaste | Fallo a largo plazo | Pocos writes/día; JSON pequeños |
| Sin NTP (sin internet) | Fechas incorrectas | RTC DS3231 o entrada manual una vez |

---

## Criterios de aceptación del proyecto

1. Dos casas medidas de forma **independiente**.
2. Servidor HTTP en LAN con **estado actual** y **histórico diario**.
3. **30 días** de datos persistidos por casa sin intervención manual.
4. Consumo diario en **kWh** con error razonable (< 15 % sin sensor de tensión).
5. Recuperación tras corte de luz sin corrupción total del día en curso.

---

## Extensiones futuras (fuera de v1)

- Medición de tensión AC (ZMPT101B) para potencia real más precisa.
- Envío MQTT a Home Assistant.
- Alertas por consumo anómalo (push o Telegram).
- OTA (actualización de firmware por WiFi).
