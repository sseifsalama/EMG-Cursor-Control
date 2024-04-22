#include <util/delay.h>
#include "Adc_LED.h"
#include "Uart.h"
#include <stdlib.h>
#include "dio.h"
#include <Wire.h>

float elapsedTime, time, timePrev;        
int gyro_error=0;                         
float Gyr_rawX, Gyr_rawY, Gyr_rawZ;     
float Gyro_angle_x, Gyro_angle_y;        
float Gyro_raw_error_x, Gyro_raw_error_y;
int acc_error=0;                         
float rad_to_deg = 180/3.141592654;      
float Acc_rawX, Acc_rawY, Acc_rawZ;   
float Acc_angle_x, Acc_angle_y;  
float Acc_angle_error_x, Acc_angle_error_y; 
float Total_angle_x, Total_angle_y;

void setupGyroAndAcc() {
  Wire.begin();                          
  Wire.beginTransmission(0x68);                       
  Wire.write(0x6B);                     
  Wire.write(0x00);
  Wire.endTransmission(true);          
  Wire.beginTransmission(0x68);          
  Wire.write(0x1B);                      
  Wire.write(0x10);                       
  Wire.endTransmission(true);           
  Wire.beginTransmission(0x68);           
  Wire.write(0x1C);                      
  Wire.write(0x10);                      
  Wire.endTransmission(true); 
  
  time = millis();                        

  if(acc_error==0)
  {
    for(int a=0; a<200; a++)
    {
      Wire.beginTransmission(0x68);
      Wire.write(0x3B);                      
      Wire.endTransmission(false);
      Wire.requestFrom(0x68,6,true); 
      
      Acc_rawX=(Wire.read()<<8|Wire.read())/4096.0 ;
      Acc_rawY=(Wire.read()<<8|Wire.read())/4096.0 ;
      Acc_rawZ=(Wire.read()<<8|Wire.read())/4096.0 ;

      Acc_angle_error_x = Acc_angle_error_x + ((atan((Acc_rawY)/sqrt(pow((Acc_rawX),2) + pow((Acc_rawZ),2)))*rad_to_deg));
      Acc_angle_error_y = Acc_angle_error_y + ((atan(-1*(Acc_rawX)/sqrt(pow((Acc_rawY),2) + pow((Acc_rawZ),2)))*rad_to_deg)); 
      
      if(a==199)
      {
        Acc_angle_error_x = Acc_angle_error_x/200;
        Acc_angle_error_y = Acc_angle_error_y/200;
        acc_error=1;
      }
    }
  }

  if(gyro_error==0)
  {
    for(int i=0; i<200; i++)
    {
      Wire.beginTransmission(0x68);            
      Wire.write(0x43);                        
      Wire.endTransmission(false);
      Wire.requestFrom(0x68,4,true);          
         
      Gyr_rawX=Wire.read()<<8|Wire.read();     
      Gyr_rawY=Wire.read()<<8|Wire.read();
   
      Gyro_raw_error_x = Gyro_raw_error_x + (Gyr_rawX/32.8); 
      Gyro_raw_error_y = Gyro_raw_error_y + (Gyr_rawY/32.8);
      if(i==199)
      {
        Gyro_raw_error_x = Gyro_raw_error_x/200;
        Gyro_raw_error_y = Gyro_raw_error_y/200;
        gyro_error=1;
      }
    }
  } 
}

void readAndPrintSensorData() {
  timePrev = time;                        
  time = millis();                       
  elapsedTime = (time - timePrev) / 1000; 

  Wire.beginTransmission(0x68);          
  Wire.write(0x43);                      
  Wire.endTransmission(false);
  Wire.requestFrom(0x68,4,true);           
  Gyr_rawX=Wire.read()<<8|Wire.read();     
  Gyr_rawY=Wire.read()<<8|Wire.read();
  Gyr_rawX = (Gyr_rawX/32.8) - Gyro_raw_error_x; 
  Gyr_rawY = (Gyr_rawY/32.8) - Gyro_raw_error_y;
  Gyro_angle_x = Gyr_rawX*elapsedTime;
  Gyro_angle_y = Gyr_rawY*elapsedTime;

  Wire.beginTransmission(0x68);    
  Wire.write(0x3B);                
  Wire.endTransmission(false);     
  Wire.requestFrom(0x68,6,true);    
  Acc_rawX=(Wire.read()<<8|Wire.read())/4096.0 ;
  Acc_rawY=(Wire.read()<<8|Wire.read())/4096.0 ;
  Acc_rawZ=(Wire.read()<<8|Wire.read())/4096.0 ; 

  Acc_angle_x = (atan((Acc_rawY)/sqrt(pow((Acc_rawX),2) + pow((Acc_rawZ),2)))*rad_to_deg) - Acc_angle_error_x;
  Acc_angle_y = (atan(-1*(Acc_rawX)/sqrt(pow((Acc_rawY),2) + pow((Acc_rawZ),2)))*rad_to_deg) - Acc_angle_error_y;    
  Total_angle_x = 0.98 *(Total_angle_x + Gyro_angle_x) + 0.02*Acc_angle_x;
  Total_angle_y = 0.98 *(Total_angle_y + Gyro_angle_y) + 0.02*Acc_angle_y;
  
  Serial.print(Total_angle_x);
  Serial.print(",");
  Serial.print(Total_angle_y);
  Serial.print(",");
}

void readAndPrintADC() {
  unsigned short adc_reading;
  adc_reading = Adc_ReadChannel(0);
  
  if(adc_reading > 300 && adc_reading < 700) {
    Serial.print("1");
  }
  else if (adc_reading > 700) {
    Serial.print("2");
  }
  else {
      Serial.print("0");
  }
  Serial.println();
}

void setup() {
  Adc_Init(); 
  Uart_Init();
  Serial.begin(9600);
  setupGyroAndAcc();
}

void loop() {
  readAndPrintSensorData();
  readAndPrintADC();
  _delay_ms(100);
}
