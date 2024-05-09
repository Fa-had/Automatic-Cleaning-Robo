#include <WiFi.h>
#include <Arduino.h>

int vac = 18;
int pump = 3;
int ir_sen1 = 19;
int ir_sen2 = 21; 

// Motor A
int motor1Pin1 = 2;
int motor1Pin2 = 4;
int enable1Pin = 15; 

// Motor B
int motor2Pin1 = 16;
int motor2Pin2 = 17;
int enable2Pin = 5;

// Setting PWM properties
const int freq = 30000;
const int pwmChannel1 = 0;
const int pwmChannel2 = 1;

const int resolution = 8;
// int dutyCycle1 = 200;
// int dutyCycle2 = 200;

const char *ssid = "i97";
const char *password = "1234567890";

WiFiServer server(8080);

void Vacu()
{
    digitalWrite(vac, HIGH);
}


void move(float speed, int turn, int time)
{
    // Move the DC motor forward at maximum speed
    speed *= 100;
    turn *= 70;
    float leftSpeed = speed - turn;
    float rightSpeed = speed + turn;
    //Front Right
    if (leftSpeed > 100)
    {
        leftSpeed = 100;
    }
    else if (leftSpeed < -100)
    {
        leftSpeed = -100;
    }
    if (rightSpeed > 100)
    {
        rightSpeed = 100;
    }
    else if (rightSpeed < -100)
    {
        rightSpeed = -100;
    }

    ledcWrite(pwmChannel1, leftSpeed);
    ledcWrite(pwmChannel2, rightSpeed);
  
    //Front Left
    // if (leftSpeed > 0)
    // {
    //     digitalWrite(motor1Pin1, HIGH);
    //     digitalWrite(motor1Pin2, LOW);
    // }
    // else
    // {
    //     digitalWrite(motor1Pin1, LOW);
    //     digitalWrite(motor1Pin2, HIGH);
    // }

    // if (rightSpeed > 0)
    // {
    //     digitalWrite(motor2Pin1, HIGH);
    //     digitalWrite(motor2Pin2, LOW);
    // }
    // else
    // {
    //     digitalWrite(motor2Pin1, HIGH);
    //     digitalWrite(motor2Pin2, LOW);
    // }
    if (rightSpeed > 0)
    {
        digitalWrite(motor1Pin1, LOW);
        digitalWrite(motor1Pin2, HIGH);
    }
    else
    {
        digitalWrite(motor1Pin1, HIGH);
        digitalWrite(motor1Pin2, LOW);
    }

    if (leftSpeed > 0)
    {
        digitalWrite(motor2Pin1, LOW);
        digitalWrite(motor2Pin2, HIGH);
    }
    else
    {
        digitalWrite(motor2Pin1, HIGH);
        digitalWrite(motor2Pin2, LOW);
    }
    delay(time);
}
void stop(int t){

    ledcWrite(pwmChannel1, 0);
    ledcWrite(pwmChannel2, 0);
    delay(t);

}
void setup()
{
    Serial.begin(115200);
    delay(100);

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.print("Connected to WiFi: ");
    server.begin();
    Serial.println(WiFi.localIP());
    pinMode(ir_sen1, INPUT);
    pinMode(ir_sen2,  INPUT);
    pinMode(vac, OUTPUT);
    pinMode(motor1Pin1, OUTPUT);
    pinMode(motor1Pin2, OUTPUT);
    pinMode(enable1Pin, OUTPUT);
    pinMode(motor1Pin1, OUTPUT);
    pinMode(motor1Pin2, OUTPUT);
    pinMode(enable1Pin, OUTPUT);
    // configure LED PWM functionalitites
    ledcSetup(pwmChannel1, freq, resolution);
    ledcSetup(pwmChannel2, freq, resolution);
    // attach the channel to the GPIO to be controlled
    ledcAttachPin(enable1Pin, pwmChannel1);
    ledcAttachPin(enable2Pin, pwmChannel2);
}

void loop()
{
    WiFiClient client = server.available();
    if (client)
    {
        Serial.println("New client connected");
        while (client.connected())
        {
            if (client.available())
            {
                if(ir_sen1 && ir_sen2)
                {
                    String cmd = client.readStringUntil('\r');
            
                    float curveVal = cmd.toFloat();
                    
                    //Serial.println("Received command: " + curveVal);
                    float sen = 1.3;    // SENSITIVITY
                    float maxVAl = 0.3; // MAX SPEED
                    if (curveVal > maxVAl)
                    {
                        curveVal = maxVAl;
                    }
                    if (curveVal < -maxVAl)
                    {
                        curveVal = -maxVAl;
                    }
                    Serial.println(curveVal);
                    if (curveVal > 0)
                        sen = 1.7;
                        if (curveVal < 0.05)
                        {
                            curveVal = 0;
                        }
                    else
                    {
                        if (curveVal > -0.08)
                        {
                            curveVal = 0;
                        }
                    }
                    move(0.20, -curveVal * sen, 50);              // For demonstration, let's just echo the command back to the client

                    Vacu();
                }else{
                  stop(50);
                }
                
            }
        }
        Serial.println("Client disconnected");
        //VACUM OFF
        digitalWrite(vac, LOW);
        client.stop();
    }
}