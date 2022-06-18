# windguru-simple-api-esp8266
Función simple para publicar en windguru.cz utilizando su API

Esta función recibe el ID de su estación en windguru y la contraseña, además de las variables a subir como temperatura, humedad, velocidad del viento, etc, con el formato:

    putToWindguru(String stationId, String stationPassword, float temperature, float humidity, int windAverage, int windMax)
    


Se encarga de generar la URL para la petición HTTP además del hash MD5 (ver WindGuru Upload API Documentatión: https://stations.windguru.cz/upload_api.php )

To-Do:

- Verificar compatibilidad con 2.4.2
- Verificar compatibilidad con 3.0.2
- Agregar ejemplo con BMP280
- Agregar esquemático en ejemplo con DHT

