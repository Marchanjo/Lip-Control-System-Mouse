/*
  LCS-LeonardoOrBTRN42.ino - Code to implement a mouse to be 
  controlled by the lower lip, use Arduino Leonardo compatible
  board (ATmega32u), have one thumb joystick to control the
  mouse cursor movement, fast left movement is the button 01, 
  fast right movement is the button 02,  fast up movement is 
  scroll-up and fast down movement is scroll-down.
  
  This code work directly with Arduino Leonardo compatible 
  board using micro USB cable (wired, low cost version) and 
  with Roving RN42 HID Bluetooth module (wireless version). 
  Both versions work with Lip Control System for people with 
  tetraplegia.
	
  License: CC BY-SA 4.0: Creative Commons Attribution-ShareAlike 
  4.0 International License. Feel free to use and abuse this code 
  however you'd like. If you find it useful please attribute, 
  and SHARE-ALIKE! 
  
  Original code created during my doctor degree since 2010 this 
  code was adapted to Arduino Leonardo in 02 Dec 2014
  by Marcelo Archnajo Jose (marcelo_archanjo@yahoo.com.br)
   
  Distributed as-is; no warranty is given.
 */


//Tested with Pro Micro (Leonardo compatible) and RN 42-HID Blutooth Module / Testado com Pro Micro (Leonardo compatível) e RN 42-HID Blutooth Module 


//Comandos seriais que configuraram o módulo Bluetooth Roving RN 42-HID (usando um FTDI Cable 5V VCC-3.3V I/O) / Serial commands that configured the Roving RN 42-HID Blutooth Module (FTDI Cable 5V VCC-3.3V I/O was used):

//$$$				(modo de comandos / commands mode3)
//ST,255			(desativa timer para poder enviar comandos por mais de 60s)
//SF,1 				(restaura configurações de fábrica)
//S-,LCS	                (muda o nome para RV42HID-MOUSE-xxxx 4 últimos dígitos do MAC address do módulo )
//SH,0220			(configura como mouse)
//S~,6				(ativa profile HID)
//SM,0				(Modo Slave, quem inicia a comunicação é o computador)
//R,1 				(Reboot)

//Pacote bluetooh transmitido no formato de dado bruto / The mouse raw bluetooth report format is:
//  0xFD
//  0x05 
//  0x02 
//  Buttons 
//  X-stop 
//  Y-stop 
//  Wheel
// more information: http://dlnmh9ip6v2uc.cloudfront.net/datasheets/Wireless/Bluetooth/RN-HID-User%20Guide-1.1r.pdf


//Este código funciona com fio (cabo micro USB na Pro Micro) ou sem fio com o módulo Bluetooth Roving RN 42-HID
//This code works wired (micro USB cable in Pro Micro board) or wireless with Roving RN 42-HID Blutooth Module
boolean bluetooth=false; /// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<< Mude aqui // Change here 

boolean leonardo=true;

//joy vcc=5v vml
const int VERT = 0; 
const int HORIZ = 1; 
const int SEL = 8; 


int vertical, horizontal;
int lastVert, lastHor, difVert, difHor;
int led = 14;//debug


int x,y;
int wheel;
boolean button1,button2,button3;
boolean mouseRefresh;

void mouseClear(void);
void mouseStep(void);
void readData(void);
int joystickSteperMouse1(int value);

unsigned long  deadZoneTimer;


//bool repairing = true;

void setup()
  {
  pinMode(SEL,INPUT);//joy
  pinMode(led, OUTPUT);
  digitalWrite(led, HIGH); 
  delay(1000);//Importante 1 segundo para deixar firmware do RN42 HID carregar // |Important: Wait 1 second to RN42 HID firmware start
  digitalWrite(led, LOW); 
 
  if(bluetooth)
    { 
    if(leonardo) Serial1.begin(115200);//tem que ser a velocidade certa Serial1 = leonardo
    else Serial.begin(115200);//tem que ser a velocidade certa
    
    if(leonardo) while (!Serial1);//necessario para Leonardo / necessary for Leonardo
     
   
    digitalWrite(led, HIGH); 
  
    
    
    //digitalWrite(greenLed, HIGH);
    if(leonardo) Serial1.print("$$$");//<<<<<<<<<< Modo de comandos
    else Serial.print("$$$");//<<<<<<<<<< Modo de comandos
    readData();
    //digitalWrite(led, HIGH); 
    
    if(leonardo) Serial1.println("C");//<<<<<<<<<<< Connect immediate to stored remote address
    else Serial.println("C");//<<<<<<<<<<< Connect immediate to stored remote address
    readData();
    }
    digitalWrite(led, LOW); //Apagou deu certo
  
 
  wheel=0;  
  button1=false;//inicia o botão1 como aberto
  button2=false;//inicia o botão2 como aberto
  button3=false;//inicia o botão3 como aberto
  
  deadZoneTimer=millis();
  }
 
 void readData(void) 
   {
   if(leonardo)
     {
     while(!Serial1.available());
     delay(200);
     while(Serial1.available()) Serial1.read();
     delay(200);
     }
   else
     {
     while(!Serial.available());
     delay(200);
     while(Serial.available()) Serial.read();
     delay(200);
     }
   } 
 
void loop()
  {
  lastVert=vertical;
  lastHor=horizontal;
  
  vertical = 1023-analogRead(VERT); //joy will be 0-1023
  horizontal = analogRead(HORIZ); //joy will be 0-1023
     
  difHor=horizontal-lastHor;
  difVert=vertical-lastVert;
  
 /* // debug
  Serial.print("H=");
  Serial.print(horizontal);
  Serial.print("  V="); 
  Serial.println(vertical);*/
   

  x=joystickSteperMouse1(horizontal); y=joystickSteperMouse1(vertical);
   
  mouseRefresh=false;
  if((x!=0)||(y!=0)){mouseRefresh=true;digitalWrite(led, HIGH);}//acende led se mover o joystisk
  else digitalWrite(led, LOW);//apagado em repouso
  
  
   //botao/button 1
  if((horizontal>600)&&(difHor>350)&&(vertical>424)&&(vertical<600)&&(button1==false))//difVert>350 
         {
         x=0;//não move o cursor quando faz movimento rapido
         y=0;//não move o cursor quando faz movimento rapido
         button1=true;
         button2=false;//Importante <<<<<<<<<<<<< 
         button3=false;//Importante <<<<<<<<<<<<<
         mouseStep();
         mouseRefresh=false;
         } 

  
  //botao/button 2 
  if((horizontal<400)&&(difHor<-350)&&(vertical>424)&&(vertical<600)&&(button2==false))//difHor<-200
         {
         x=0;//não move o cursor quando faz movimento rapido
         y=0;//não move o cursor quando faz movimento rapido
         button1=false;//Importante <<<<<<<<<<<<<
         button2=true; 
         button3=false;//Importante <<<<<<<<<<<<<
         mouseStep();
         mouseRefresh=false;
         }
 
  // Wheel Up
  if((vertical>600)&&(difVert>350)&&(horizontal>424)&&(horizontal<600))//difVert>350
    {wheel=1;
     x=0;//não move o cursor quando faz movimento rapido
     y=0;//não move o cursor quando faz movimento rapido
     mouseRefresh=true;
     delay(100);}//up
  
  // Wheel Down
  if((vertical<424)&&(difVert<-350)&&(horizontal>424)&&(horizontal<600))//difVert<-200
    {wheel=-1;
     x=0;//não move o cursor quando faz movimento rapido
     y=0;//não move o cursor quando faz movimento rapido
     mouseRefresh=true;
     delay(100);}//down
   
  //deadzone - 150ms na deadzon desativa botões 150ms in deadzone deactivate buttons
  if((horizontal>=428)&&(horizontal<=596)&&(vertical>=428)&&(vertical<=596) )
      {
      if((millis()-deadZoneTimer)>150) 
        {
        button1=false;
        button2=false;      
        
        mouseClear();//desativa clicklock do botão 1 se ficar 50ms na deadzone
        deadZoneTimer=millis();
        }
      wheel=0;
      }
  else deadZoneTimer=millis();
  
  if( (button1==true)||(button2==true)||(wheel!=0) ) {x=0;y=0;}//não se move com botão acionado - if button 1 or 2 is on do not move
  if(mouseRefresh) {mouseStep();}//digitalWrite(redLed, HIGH );digitalWrite(greenLed, HIGH);}//digitalWrite(blueLed, HIGH);}
  }
  

  
int joystickSteperMouse1(int value)//curva 4 mais suave e lenta que a 3
  {
  int retorno;
  int deadzone=45;
  value=-(value-511);
  if(value<=-480) retorno=-5;
  if((value>-480)&&(value<=-416)) retorno=-4;
  if((value>-416)&&(value<=-352)) retorno=-3;
  if((value>-352)&&(value<=-288)) retorno=-2;
  if((value>-288)&&(value<=-224)) retorno=-2;
  if((value>-224)&&(value<=-160)) retorno=-1;
  if((value>-160)&&(value<=-96)) retorno=-1;
  if((value>-96)&&(value<=-deadzone)) if(retorno==-1) retorno=0; else retorno=-1;//equivale a -0,5
  if((value>-deadzone)&&(value<=deadzone)) retorno=0;
  if((value>deadzone)&&(value<=96)) if(retorno==1) retorno=0; else retorno=1;//equivale a 0,5
  if((value>96)&&(value<=160)) retorno=1;
  if((value>160)&&(value<=224)) retorno=1;
  if((value>224)&&(value<=288)) retorno=2;
  if((value>288)&&(value<=352)) retorno=2;
  if((value>352)&&(value<=416)) retorno=3;
  if((value>416)&&(value<=480)) retorno=4;
  if(value>480) retorno=5;
  return retorno;
  }
  



void mouseClear(void)
  {
  int value=0x00;
  if(bluetooth)
    {
    if(leonardo)
      {
      Serial1.write(0xFD);
      Serial1.write(0x05);
      Serial1.write(0x02);
      Serial1.write(value);//botão
      Serial1.write(value);//(x);
      Serial1.write(value);//(y);
      Serial1.write(value);//(wheel);
      }
    else
      {
      Serial.write(0xFD);
      Serial.write(0x05);
      Serial.write(0x02);
      Serial.write(value);//botão
      Serial.write(value);//(x);
      Serial.write(value);//(y);
      Serial.write(value);//(wheel);
      }
    }
  else Mouse.release(MOUSE_ALL);// to avoid continuous click
  
  delay(20);
  }
 
 
void mouseStep(void)
  {
  int value=0x00;
  
  if(bluetooth)
    {
    if(leonardo)
      { 
      Serial1.write(0xFD);
      Serial1.write(0x05);
      Serial1.write(0x02);
      
      
      if((!button1)&&(!button2)&&(!button3)) Serial1.write(value);
      if(button3) Serial1.write(0x3);
      if(button2) Serial1.write(0x2); 
      if(button1) Serial1.write(0x1);//botão1 é prioritário
     
        
      Serial1.write(x);
      Serial1.write(y);
      Serial1.write(wheel);
      }
      
    else
      { 
      Serial.write(0xFD);
      Serial.write(0x05);
      Serial.write(0x02);
      
      
      if((!button1)&&(!button2)&&(!button3)) Serial.write(value);
      if(button3) Serial.write(0x3);
      if(button2) Serial.write(0x2); 
      if(button1) Serial.write(0x1);//botão1 é prioritário / Button 1 is most important
     
        
      Serial.write(x);
      Serial.write(y);
      Serial.write(wheel);
      }
    }
  else 
    {
    Mouse.move(x, y, wheel);
  
    if((!button1)&&(!button2)&&(!button3)) Mouse.release(MOUSE_ALL);// to avoid continuous click
    if(button3) Mouse.press(MOUSE_MIDDLE);
    if(button2) Mouse.press(MOUSE_RIGHT); 
    if(button1) Mouse.press(MOUSE_LEFT);//botão1 é prioritário / Button 1 is most important
    }
  
  delay(20);
  }   


