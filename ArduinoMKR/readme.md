# GL Green Tech - Air Data Challenge
# Green Lab - Future Education Modena 2022

## Hardware utilizzato
- Arduino MKR WiFi 1010
- MKR ENV SHIELD

## Librerie
- MKRENV (tutti)
- DallasTemperature (Terra)
- OneWire (Terra)
- SdsDustSensor (aria)
- Adafruit_CCS811 (aria)
- ArduinoJson (tutti)

## Sensori
- Capacitive Soil Moisture Sensor v1.2
- CJMCU-811
- MH-Z19B ndir CO2
- SDS011 Nova PM Sensor

## Collegamenti
- Capacitive Soil Moisture Sensor v1.2
> red -> 5V
> black -> GND
> yellow -> A1

- CJMCU-811
> VCC -> 3.3V
> GND -> GND
> SCL -> 12SCL
> SDA -> 11SDA
> WAK -> GND
> ADD -> GND

- MH-Z19B ndir CO2
> Vin -> 5V
> GND -> GND
> Vo -> A0

- SDS011 Nova PM Sensor
> 5V -> 5V
> GND -> GND
> TDX -> 13RX
> RDX -> 14TX