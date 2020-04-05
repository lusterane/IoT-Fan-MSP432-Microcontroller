#ifndef __CC3200R1M1RGC__
// Do not include SPI for CC3200 LaunchPad
#include <SPI.h>
#endif
#include <WiFi.h>
#include <Sensitive.h> // CUSTOM LINKING PATH. REMOVE IF YOU ARE NOT ME

// constants
const int pwm_out = P3_2; // pwm pin
const int temp_in = A14; // read temp

// your network name also called SSID
char ssid[] = sSSID; // replace sSSID with ssid of network
// your network password
char password[] = sPASSWORD; // replace sPASSWORD with password of network
// your network key Index number (needed only for WEP)
int keyIndex = 0;

// fan data
int fanSpeed = 0;
float perc_fs = 0.0; // fan speed percentage
float temp = 0.0;

WiFiServer server(80);

void setup() {
  Serial.begin(115200);      // initialize serial communication

  
  pinMode(BLUE_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);      // set the LED pin mode
  pinMode(pwm_out, OUTPUT);  // set PWM pin mode

  // attempt to connect to Wifi network:
  Serial.print("Attempting to connect to Network named: ");
  // print the network name (SSID);
  Serial.println(ssid); 
  // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
  WiFi.begin(ssid, password);
  while ( WiFi.status() != WL_CONNECTED) {
    // print dots while we wait to connect
    Serial.print(".");
    delay(300);
  }
  
  Serial.println("\nYou're connected to the network");
  Serial.println("Waiting for an ip address");
  
  while (WiFi.localIP() == INADDR_NONE) {
    // print dots while we wait for an ip addresss
    Serial.print(".");
    delay(300);
  }

  Serial.println("\nIP Address obtained");
  
  // you're connected now, so print out the status  
  printWifiStatus();
  
  Serial.println("Starting webserver on port 80");
  server.begin();                           // start the web server on port 80
  Serial.println("Webserver started!");

  // initialize Fan Speed to 0% duty cycle
  analogWrite(pwm_out, 0);
}

void loop() {
  readTemperature();
  int i = 0;
  WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    Serial.println("New client");           // print a message out the serial port
    char buffer[150] = {0};                 // make a buffer to hold incoming data
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (strlen(buffer) == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // the content of the HTTP response follows the header:
            /*
            client.println("<html><head><title>Energia CC3100 WiFi Web Server</title></head><body align=center>");
            client.println("<h1 align=center><font color=\"red\">CC3100 WiFi Web Server</font></h1>");
            client.print("FAN SPEED <button onclick=\"location.href='/DECREASE_FAN'\">DECREASE</button>");
            client.println(" <button onclick=\"location.href='/INCREASE_FAN'\">INCREASE</button><br>");
            client.print("PWM Duty Cycle: ");
            client.print(fanSpeed);
            client.print("/255 (");
            client.print(100.0*fanSpeed/255, 4);
            client.println("%)");
            */
            
            perc_fs = 100.0*fanSpeed/255;
            
            client.print("<html lang=\"en\"> <head> <link rel=\"stylesheet\" href=\"https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/css/bootstrap.min.css\" integrity=\"sha384-ggOyR0iXCbMQv3Xipma34MD+dH/1fQ784/j6cY/iJTQUOhcWr7x9JvoRxT2MZw1T\" crossorigin=\"anonymous\"> <link rel=\"stylesheet\" href=\"https://codepen.io/lusterane/pen/OJVdNaR.css\"> <meta charset=\"UTF-8\"> <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"> <title>Toby's IoT Home</title> </head> ");
            client.print("<body class=\"center\"> <h1>Toby's IoT Home</h1> <div class=\"fan-container\"> <h3>FAN SPEED</h3> <button onclick=\"location.href='/DECREASE_FAN'\" class=\"btn btn-primary\">DECREASE</button> <button onclick=\"location.href='/INCREASE_FAN'\" class=\"btn btn-primary\">INCREASE</button> <div class=\"progress\"> <div class=\"progress-bar\" role=\"progressbar\" style=\"width: ");
            client.print(perc_fs, 4);
            client.print("%\"");
            client.print(" aria-valuenow=\"75\" aria-valuemin=\"0\" aria-valuemax=\"100\"></div> </div> </div>");
            client.print("PWM Duty Cycle: ");
            client.print(fanSpeed);
            client.print("/255 (");
            client.print(perc_fs, 4);
            client.println("%)");

            client.print("<script src=\"https://code.jquery.com/jquery-3.3.1.slim.min.js\" integrity=\"sha384-q8i/X+965DzO0rT7abK41JStQIAqVgRVzpbzo5smXKp4YfRvH+8abtTE1Pi6jizo\" crossorigin=\"anonymous\"></script> <script src=\"https://cdnjs.cloudflare.com/ajax/libs/popper.js/1.14.7/umd/popper.min.js\" integrity=\"sha384-UO2eT0CpHqdSJQ6hJty5KVphtPhzWj9WO1clHTMGa3JDZwrnQq4sF86dIHNDz0W1\" crossorigin=\"anonymous\"></script> <script src=\"https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/js/bootstrap.min.js\" integrity=\"sha384-JjSmVgyd0p3pXB1rRibZUAYoIIy6OrQ6VrjIEaFf/nJGzIxFDsf4x0xIM+B07jRM\" crossorigin=\"anonymous\"></script> </body> </html>");
            
            // break out of the while loop:
            break;
          }
          else {      // if you got a newline, then clear the buffer:
            memset(buffer, 0, 150);
            i = 0;
          }
        }
        else if (c != '\r') {    // if you got anything else but a carriage return character,
          buffer[i++] = c;      // add it to the end of the currentLine
        }

        // Check to see if the client request was "GET /INCREASE_FAN" or "GET /DECREASE_FAN":
        if (endsWith(buffer, "GET /INCREASE_FAN") && fanSpeed < 255) {
          fanSpeed += 17;
          digitalWrite(BLUE_LED, HIGH);
          delay(15);
          digitalWrite(BLUE_LED, LOW);
        }
        else if (endsWith(buffer, "GET /DECREASE_FAN") && fanSpeed > 0) {
          fanSpeed -= 17;
          digitalWrite(RED_LED, HIGH);
          delay(15);
          digitalWrite(RED_LED, LOW);
        }
        analogWrite(pwm_out, fanSpeed); // 0-255
      }
    }
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
}

//
//a way to check if one array ends with another array
//
boolean endsWith(char* inString, const char* compString) {
  int compLength = strlen(compString);
  int strLength = strlen(inString);
  
  //compare the last "compLength" values of the inString
  int i;
  for (i = 0; i < compLength; i++) {
    char a = inString[(strLength - 1) - i];
    char b = compString[(compLength - 1) - i];
    if (a != b) {
      return false;
    }
  }
  return true;
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
  // print where to go in a browser:
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
}

void readTemperature(){
  temp = analogRead(temp_in);

  // converting that reading to voltage, for 3.3v arduino use 3.3
   float voltage = temp * 5.0;
   voltage /= 1024.0; 

   // now print out the temperature
   float temperatureC = (voltage - 0.5) * 100 ;  //converting from 10 mv per degree wit 500 mV offset
                                                 //to degrees ((voltage - 500mV) times 100)
                                                 /*
   Serial.print(temperatureC); 
   Serial.println(" degrees C");        
   */                           
   // now convert to Fahrenheit
   float temperatureF = (temperatureC * 9.0)/ 5.0 + 32.0;
   Serial.print(temperatureF);
   Serial.println(" degrees F");
}
