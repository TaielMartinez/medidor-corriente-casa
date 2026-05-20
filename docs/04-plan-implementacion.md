# Plan de implementación

Orden inspirado en Savjee: **breadboard → protoboard → campo**, con **EmonLib** y dos canales en una sola placa.

---

## Fases

| Fase | Duración | Entregable |
|------|----------|------------|
| 0 | 1–2 días | BOM, jacks, pinout C3 |
| 1 | 2–4 días | 1 canal + EmonLib en Serial |
| 2 | 2–3 días | 2 canales en una placa |
| 3 | 3–4 días | WiFi + kWh + LittleFS |
| 4 | 2–3 días | Servidor HTTP + API |
| 5 | 1–2 días | Instalación SCT en ambas casas |

---

## Fase 0 — Compras y preparación

- [ ] ESP32-C3 SuperMini + **zócalos hembra**
- [ ] 2× SCT-013 100 A (con plug 3,5 mm)
- [ ] 2× jack 3,5 mm, burden 22 Ω, 2× C 10 µF, 4× R (100 kΩ)
- [ ] LED rojo + 220 Ω → GPIO2
- [ ] PlatformIO + lib **EmonLib**
- [ ] IP fija en router para el ESP

---

## Fase 1 — Un canal (mesa)

Réplica mínima del tutorial Savjee:

- [ ] Circuito burden + R1/R2 + C en breadboard
- [ ] `emon1.current(GPIO0, 30)` y `calcIrms(1480)` cada 1 s por Serial
- [ ] Ignorar primeros 10 s tras `setup`
- [ ] Calibrar con carga conocida

**Salida:** I_rms estable (error < 10 % vs referencia).

---

## Fase 2 — Dos casas, una placa

- [ ] Duplicar circuito → jack **J2** → GPIO1
- [ ] `emon2.current(GPIO1, CAL_CASA2)`
- [ ] En `loop`: medir casa1 y casa2 cada 1 s (sin `delay`)
**Salida:** dos lecturas independientes en Serial (depuración).

---

## Fase 3 — Energía y persistencia

- [ ] `P = I × V_RED` por casa
- [ ] Acumular Wh; NTP; cierre de día
- [ ] LittleFS + rotación 30 días + snapshot cada 5 min

---

## Fase 4 — Servidor HTTP y LED

- [ ] `WebServer` + `handleClient()` en cada `loop`
- [ ] `/api/status`, `/api/today`, `/api/daily`
- [ ] LED: parpadeo si falla red/internet; fijo si sin corriente; apagado si OK
- [ ] Probar todos los endpoints con `curl`

---

## Fase 5 — Instalación

- [ ] SCT en fase principal **Casa 1** → jack J1
- [ ] SCT en fase principal **Casa 2** → jack J2
- [ ] Calibración fina `CAL_CASA1` / `CAL_CASA2`
- [ ] Verificar `/api/daily` al día siguiente

---

## Criterios de aceptación

1. **Una placa** mide **dos casas** por separado.
2. Medición cada **~1 s**; kWh del día coherente.
3. **30 días** de histórico por casa en flash.
4. API HTTP en LAN sin servicios cloud.
5. LED rojo: **parpadeo** sin red/internet (prioridad); **fijo** sin corriente; **apagado** en operación normal.
6. Montaje no invasivo (pinza SCT, sin cortar cables).

---

## Riesgos

| Riesgo | Mitigación |
|--------|------------|
| `calcIrms` lento con 2 canales | Período 1 s; no añadir más canales en v1 |
| Calibración distinta por casa | `CAL_CASA1` ≠ `CAL_CASA2` en NVS |
| Secundario SCT abierto | Burden siempre montada |
| SCT-013 100 A vs 030 del tutorial | Recalibrar; burden 22 Ω según OEM |

---

## Extensiones (post v1)

- MQTT batch (estilo Savjee: 30 muestras cada 30 s).
- Home Energy Monitor V2 / Home Assistant.
- Sensor de tensión para potencia real.
