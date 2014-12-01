//Testado com Pro Micro (Leonardo compatível) e RN 42-HID 

//O mouse do pellegrini funciona assim:
//botão 1 para a esquerda
//click (movimento rápido) realease (dead zone), mas enquando isso não move o cursor (se não voltar o joystick não ocorre o release)
tudo implementado, veja se faz o click e release no botão 2

//Comandos que configuraram o módulo:
//$$$				(modo de comandos)
//ST,255			(desativa timer para poder enviar comandos por mais de 60s)
//SF,1 				(restaura configurações de fábrica)
//S-,RN42HID-Mouse	(muda o nome para RV42HID-MOUSE-xxxx 4 últimos dígitos do MAC address do módulo )
//SH,0220			(configura como mouse)
//S~,6				(ativa profile HID)
//SM,0				(Modo Slave, quem inicia a comunicação é o computador)
//R,1 				(Reboot)


//0x80 128 Teclado Cursor up
//0x82 130 Teclado Cursor down

//0x88 136  Teclado Page up
//0x89 137  Teclado Page down

//0x8A	138	Mouse buttons up
//0x8B	139	Mouse up (10px)
//0x8C	140	Mouse right (10px)
//0x8D	141	Mouse down (10px)
//0x8E	142	Mouse left (10px)
//0x8F	143	Mouse button 1 (first clear with 138)
//0x90	144	Mouse button 2 (first clear with 138)
//0x91	145	Mouse button 3 (first clear with 138)

boolean galileo=false;
boolean leonardo=true;

//joy vcc=5v vml
const int VERT = 1; //joy analog amr
const int HORIZ = 0; //joy analog vrd
const int SEL = 8; //joy digital azl


int vertical, horizontal;//, select;//joy
int lastVert, lastHor, difVert, difHor;//teste a - incluir click 1, 2 e wheel
int led = 14;
//test single joy//int vertical2, horizontal2, select2;//joy2

int x,y;
int wheel;
boolean button1,button2,button3;
boolean button1ClickLock;
boolean deadZonePass;

boolean mouseRefresh;
void mouseClear(void);
void mouseStep(void);
void mouseStep2(void);
void readData(void);
int joystickSteperMouse0(int value);
int joystickSteperMouse1(int value);
int joystickSteperMouse2(int value);
int curva=1;//Curva 4 a mais lenta e melhor
int ledPin0 = 9;//debug01
unsigned long  deadZoneTimer;


//bool repairing = true;

void setup()
  {
  pinMode(led, OUTPUT);
  digitalWrite(led, HIGH); 
  delay(1000);//Necessário 1 segundo para deixar firmware do RN42 HID carregar
  digitalWrite(led, LOW); 
 
  
  //delay(60000);//arduino mega com 60s funcionou, 70s ficou instavel e 80s não vai de jeito nenhum
  //digitalWrite(led, HIGH);
  
  pinMode(ledPin0, OUTPUT);//debug01
  pinMode(SEL,INPUT);//joy
  
  //test single joy//pinMode(SEL2,INPUT);//joy
  

 
  if(galileo||leonardo) Serial1.begin(115200);//tem que ser a velocidade certa Serial1 = galileo||leonardo
  else Serial.begin(115200);//tem que ser a velocidade certa
  
  if(leonardo) while (!Serial1);
 
   
  digitalWrite(led, HIGH); 

  
  
  //digitalWrite(greenLed, HIGH);
  if(galileo||leonardo) Serial1.print("$$$");//<<<<<<<<<< Modo de comandos
  else Serial.print("$$$");//<<<<<<<<<< Modo de comandos
  readData();
  //digitalWrite(led, HIGH); 
  
  if(galileo||leonardo) Serial1.println("C");//<<<<<<<<<<< Connect immediate to stored remote address
  else Serial.println("C");//<<<<<<<<<<< Connect immediate to stored remote address
  readData();
  digitalWrite(led, LOW); //Apagou deu certo
  
 
  wheel=0;  
  button1=false;//inicia o botão1 como aberto
  button2=false;//inicia o botão2 como aberto
  button3=false;//inicia o botão3 como aberto
  button1ClickLock=false;
  deadZonePass=false;
  deadZoneTimer=millis();
  }
 
 void readData(void) 
   {
   if(galileo||leonardo)
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
  lastVert=vertical;//teste a
  lastHor=horizontal;//teste a
  
  vertical = analogRead(VERT); //joy will be 0-1023
  if(galileo)  vertical += 86;//marretado para dar 511 na galileo
  
  analogWrite(ledPin0, vertical/4);//1024/4=256   //debug01
  
  horizontal = 1023-analogRead(HORIZ); //joy will be 0-1023
  if(galileo) horizontal += 92;//marretado para dar 511 na galileo
   
  difHor=horizontal-lastHor;//teste a
  difVert=vertical-lastVert;//teste a
  
 /* Serial.print("H=");
  Serial.print(horizontal);
  Serial.print("  V="); 
  Serial.println(vertical);*/
   

  x=joystickSteperMouse1(horizontal); y=joystickSteperMouse1(vertical);//digitalWrite(redLed, LOW );digitalWrite(greenLed, HIGH);
   
  mouseRefresh=false;
  if((x!=0)||(y!=0)){mouseRefresh=true;digitalWrite(led, HIGH);}//apaga led se mover o joystisk
  else digitalWrite(led, LOW);//aceso em repouso
  
  
   //botao 1
  if((horizontal>600)&&(difHor>350)&&(vertical>424)&&(vertical<600)&&(button1==false))//difVert>350 
  //if((horizontal<400)&&(difHor<-350)&&(vertical>424)&&(vertical<600)&&(button1==false))//difHor<-200
        {//mouseClear();//desativa tudo, pois o Modulo trava se transitar do botão 1 para o 2 sem antes desativá-lo
         x=0;//não move o cursor quando faz movimento rapido
         y=0;//não move o cursor quando faz movimento rapido
         button1=true;
         button2=false;//Importante <<<<<<<<<<<<< 
         button3=false;//Importante <<<<<<<<<<<<<
         //mouseRefresh=true;
         mouseStep();
        // button1=false;
        // button2=false;
        // button3=false;
       //  mouseClear();
         mouseRefresh=false;
         } 
  else if((horizontal>600)&&(difHor>350)&&(vertical>424)&&(vertical<600)&&(button1==true))//difVert>350     
  //else if((horizontal<400)&&(difHor<-350)&&(vertical>424)&&(vertical<600)&&(button1==true))//difHor<-200
          {//mouseClear();//desativa tudo, pois o Modulo trava se transitar do botão 1 para o 2 sem antes desativá-lo
           x=0;//não move o cursor quando faz movimento rapido
           y=0;//não move o cursor quando faz movimento rapido
           button1=false;
           button2=false;
           button3=false;
           mouseClear();
           mouseRefresh=false;
           } 
  
  //botao 2 
  if((horizontal<400)&&(difHor<-350)&&(vertical>424)&&(vertical<600)&&(button2==false))//difHor<-200
  //if((horizontal>600)&&(difHor>350)&&(vertical>424)&&(vertical<600)&&(button2==false))//difVert>350
        {mouseClear();//desativa tudo, pois o Modulo trava se transitar do botão 1 para o 2 sem antes desativá-lo
         x=0;//não move o cursor quando faz movimento rapido
         y=0;//não move o cursor quando faz movimento rapido
         button1=false;//Importante <<<<<<<<<<<<<
         button2=true; 
         button3=false;//Importante <<<<<<<<<<<<<
         //mouseRefresh=true;
         mouseStep();
         button1=false;
         button2=false;
         button3=false;
         mouseClear();
         mouseRefresh=false;
         }
         
  
  // Wheel Up
  if((vertical>600)&&(difVert>350)&&(horizontal>424)&&(horizontal<600))//difVert>350
    {wheel=1;
     x=0;//não move o cursor quando faz movimento rapido
     y=0;//não move o cursor quando faz movimento rapido
     mouseRefresh=true;
     delay(100);}//up
  
  // Wheel Up
  if((vertical<424)&&(difVert<-350)&&(horizontal>424)&&(horizontal<600))//difVert<-200
    {wheel=-1;
     x=0;//não move o cursor quando faz movimento rapido
     y=0;//não move o cursor quando faz movimento rapido
     mouseRefresh=true;
     delay(100);}//down
   
  //deadzone  
  if((horizontal>=428)&&(horizontal<=596)&&(vertical>=428)&&(vertical<=596) )
      {
      if((millis()-deadZoneTimer)>150) 
        {
        button1=false;
      
        
        mouseClear();//desativa clicklock do botão 1 se ficar 50ms na deadzone
        deadZoneTimer=millis();
        }
      wheel=0;
      }
  else deadZoneTimer=millis();
  
  if(button1==true) {x=0;y=0;}
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
  if((value>-deadzone)&&(value<=deadzone)) retorno=0;//dead zone maior que nas outras curvas aqui é 38 e nas outras é 32
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
  if(galileo||leonardo)
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
  delay(20);
  }
  

void mouseStep(void)
  {
  int value=0x00;
  
  if(galileo||leonardo)
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
    if(button1) Serial.write(0x1);//botão1 é prioritário
   
      
    Serial.write(x);
    Serial.write(y);
    Serial.write(wheel);
    }
    
  delay(20);
  }   


