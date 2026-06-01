# Test WiFi (ESP32-C3 SuperMini)

Sketch mínimo para verificar red y servidor HTTP.

## Arduino IDE

1. Instalar el core **esp32** (Espressif) desde el Gestor de tarjetas.
2. Abrir `test-wifi.ino`.
3. Configurar la placa:

| Opción | Valor |
|--------|--------|
| Board | **ESP32C3 Dev Module** |
| USB CDC On Boot | **Enabled** |
| Flash Size | Según tu módulo (típ. 4 MB) |

4. Copiar credenciales si hace falta: `secrets.h.example` → `secrets.h`.
5. Subir y abrir el Monitor Serie a **115200 baud**.

## Probar la API

Con la IP que imprime el Serial (ej. `192.168.1.50`):

```bash
curl http://192.168.1.50/
curl http://192.168.1.50/api/health
```

Respuesta esperada: `true` (JSON).
