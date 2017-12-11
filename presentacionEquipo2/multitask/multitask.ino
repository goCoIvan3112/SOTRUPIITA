#include <Arduino_FreeRTOS.h> // incluimos la librería de FreeRTOS al programa
#include <semphr.h>  // esta librería maneja las banderas o semáforos en arduino.

// se declara un semáforo binario para el manejo de la comunicación por el puerto serial
//para que solo una tarea a la vez tenga acceso al Puerto serial.
SemaphoreHandle_t xSerialSemaphore;

// se definen las 2 tareas a ejecutar por el planificador.
void TaskDigitalRead( void *pvParameters );
void TaskAnalogRead( void *pvParameters );

void setup() {

  // se inicializa la comunicación serial a 9600 baudios:
  Serial.begin(9600);
  
  while (!Serial) {
    ; // espera a que se habrá el Puerto serial
  }

  if ( xSerialSemaphore == NULL )  // se confirma que el semáforo no haya sido creado
  {
    xSerialSemaphore = xSemaphoreCreateMutex();  // se crea el semáforo que se encargara de manejar la comunicacion del puerto serial
    if ( ( xSerialSemaphore ) != NULL )
      xSemaphoreGive( ( xSerialSemaphore ) );  // se habilita el Puerto serial para ser utilizado por la función XSemaphoregive.
  }

  // se definen las tareas a realizer por el so.
  xTaskCreate(
    TaskDigitalRead
    ,  (const portCHAR *)"DigitalRead"  // se nombra la tarea
    ,  128  // tamaño de pila 
    ,  NULL
    ,  2  //prioridad de la tarea .
    ,  NULL );

  xTaskCreate(
    TaskAnalogRead
    ,  (const portCHAR *) "AnalogRead" // se nombra la tarea
    ,  128  // tamaño de pila
    ,  NULL
    ,  1  // priordad de la tarea
    ,  NULL );
}

void loop()
{
  // vacio.
}
// programacion de las tareas

void TaskDigitalRead( void *pvParameters __attribute__((unused)) )  
{
  
  uint8_t pushButton = 2;

  pinMode(pushButton, INPUT);

  for (;;) 
  {
    int buttonState = digitalRead(pushButton);

    // lee el semáforo si puede usar el Puerto serie.
    // si no está disponible, espera 5 siclos del planificador y vuelve a revisar si esta libre.
    if ( xSemaphoreTake( xSerialSemaphore, ( TickType_t ) 5 ) == pdTRUE )
    {
    // Pudimos tomar el semáforo y ahora podemos acceder al recurso compartido. 
    // Queremos tener el puerto serie solo para nosotros, ya que lleva algo de tiempo imprimir, 
    // entonces no queremos que sea robado durante la mitad de una conversión
      Serial.println(buttonState);

      xSemaphoreGive( xSerialSemaphore ); //dejamos libre el semáforo para otras tareas.
    }

    vTaskDelay(1);  // una pequeña espera para no saturar el mcu.
  }
}

void TaskAnalogRead( void *pvParameters __attribute__((unused)) )

  for (;;)
  {
    int sensorValue = analogRead(A0);

    // lee el semáforo si puede usar el Puerto serie.
    // si no está disponible, espera 5 siclos del planificador y vuelve a revisar si esta libre.
    if ( xSemaphoreTake( xSerialSemaphore, ( TickType_t ) 5 ) == pdTRUE )
    {
      // Pudimos tomar el semáforo y ahora podemos acceder al recurso compartido. 
      // Queremos tener el puerto serie solo para nosotros, ya que lleva algo de tiempo imprimir, 
      // entonces no queremos que sea robado durante la mitad de una conversión
      Serial.println(sensorValue);

      xSemaphoreGive( xSerialSemaphore ); //dejamos libre el semáforo para otras tareas.
    }

    vTaskDelay(1); 
  }
}

