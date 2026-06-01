# Test corriente instantánea (Casa 1)

Firmware de prueba: WiFi + medición SCT-013 en **GPIO0** (ADC1-0) con **EmonLib**.

## Endpoints

| Método | Ruta | Respuesta |
|--------|------|-----------|
| GET | `/` | `true` |
| GET | `/api/health` | `true` |
| GET | `/actual/1` | `{"casa":1,"i_rms_a":…,"p_w":…}` |

La corriente se actualiza cada **1 s** en segundo plano; `/actual/1` devuelve el último valor calculado.

Las respuestas incluyen cabeceras **CORS** (`Access-Control-Allow-Origin: *`) para el monitor HTML en `tools/poll-actual.html`.

## Calibración

Ajustar `CAL_CASA1` en `test-actual.ino` (valor inicial **30**, como Savjee).

## Placa

ESP32C3 Dev Module, USB CDC On Boot: Enabled.
