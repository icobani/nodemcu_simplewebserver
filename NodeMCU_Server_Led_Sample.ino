#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// Replace with your network credentials
const char* ssid = "Asci";
const char* password = "qqqqqqqq";
char serveradr[] = "mail.smtp2go.com"; // The SMTP Server 

ESP8266WebServer server(80);   //instantiate server at port 80 (http port)
WiFiClient espClient;


String page = "";
int LEDPin = 13;
void setup(void){
  //the HTML of the web page
  page = "<h1>Sera Sulama Projesi Oguzkaan</h1><p><a href=\"LEDOn\"><button>AC</button></a>&nbsp;<a href=\"LEDOff\"><button>KAPAT</button></a></p>";
  //make the LED pin output and initially turned off
  pinMode(LEDPin, OUTPUT);
  digitalWrite(LEDPin, LOW);
   
  delay(1000);
  Serial.begin(74880);
  WiFi.begin(ssid, password); //begin WiFi connection
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
   
  server.on("/", [](){
    server.send(200, "text/html", page);
  });
  server.on("/LEDOn", [](){
    server.send(200, "text/html", page);
    digitalWrite(LEDPin, HIGH);
    delay(1000);
  });
  server.on("/LEDOff", [](){
    server.send(200, "text/html", page);
    digitalWrite(LEDPin, LOW);
    delay(1000); 
  });
  server.begin();
  Serial.println("Web server started!");

IPAddress broadCast = WiFi.localIP();
broadCast[3] = 255;
Serial.println(broadCast);

  byte ret = sendEmail(IpAddress2String(WiFi.localIP()));
}
 
void loop(void){
  server.handleClient();
}


String IpAddress2String(const IPAddress& ipAddress)
{
  return String(ipAddress[0]) + String(".") +\
  String(ipAddress[1]) + String(".") +\
  String(ipAddress[2]) + String(".") +\
  String(ipAddress[3])  ;
}


byte emailResp()
{
  byte responseCode;
  byte readByte;
  int loopCount = 0;

  while (!espClient.available()) 
  {
    delay(1);
    loopCount++;
    // Wait for 20 seconds and if nothing is received, stop.
    if (loopCount > 20000) 
    {
      espClient.stop();
      Serial.println(F("\r\nTimeout"));
      return 0;
    }
  }

  responseCode = espClient.peek();
  while (espClient.available())
  {
    readByte = espClient.read();
    Serial.write(readByte);
  }

  if (responseCode >= '4')
  {
    //  efail();
    return 0;
  }
  return 1;
}



byte sendEmail(String ipAddress)
{
  Serial.println(ipAddress);
  if (espClient.connect(serveradr, 2525) == 1) 
  {
    Serial.println(F("connected"));
  } 
  else 
  {
    Serial.println(F("connection failed"));
    return 0;
  }
  if (!emailResp()) 
    return 0;
  //
  Serial.println(F("Sending EHLO"));
  espClient.println("EHLO www.example.com");
  if (!emailResp()) 
    return 0;
  //
  /*Serial.println(F("Sending TTLS"));
  espClient.println("STARTTLS");
  if (!emailResp()) 
  return 0;*/
  //  
  Serial.println(F("Sending auth login"));
  espClient.println("AUTH LOGIN");
  if (!emailResp()) 
    return 0;
  //  
  Serial.println(F("Sending User"));
  // Change this to your base64, ASCII encoded username
  /*
  For example, the email address test@gmail.com would be encoded as dGVzdEBnbWFpbC5jb20=
  */
  espClient.println("YWxpQGNvYmFuaS5tZQ=="); //base64, ASCII encoded Username
  if (!emailResp()) 
    return 0;
  //
  Serial.println(F("Sending Password"));
  // change to your base64, ASCII encoded password
  /*
  For example, if your password is "testpassword" (excluding the quotes),
  it would be encoded as dGVzdHBhc3N3b3Jk
  */
  espClient.println("dGVzdHBhc3N3b3Jk");//base64, ASCII encoded Password
  if (!emailResp()) 
    return 0;
  //
  Serial.println(F("Sending From"));
  // change to sender email address
  espClient.println(F("MAIL From: ali@cobani.me"));
  if (!emailResp()) 
    return 0;
  // change to recipient address
  Serial.println(F("Sending To"));
  espClient.println(F("RCPT To: ali@cobani.me"));
  if (!emailResp()) 
    return 0;
  //
  Serial.println(F("Sending DATA"));
  espClient.println(F("DATA"));
  if (!emailResp()) 
    return 0;
  Serial.println(F("Sending email"));
  // change to recipient address
  espClient.println(F("To:  ali@cobani.me"));
  // change to your address
  espClient.println(F("From: ali@cobani.me"));
  espClient.println(F("Subject: Hey sana sevincli bir haberim var.\r\n"));
  espClient.println(F("Asagidaki adresten sisteme ulasabilirsin..\n"));
  espClient.println(F("ip adresi bak bu."));
  espClient.println(ipAddress);
  espClient.println("Bak bu adresden ulasabilirsin.");
  espClient.println("http://" + ipAddress);
  //
  espClient.println(F("."));
  if (!emailResp()) 
    return 0;
  //
  Serial.println(F("Sending QUIT"));
  espClient.println(F("QUIT"));
  if (!emailResp()) 
    return 0;
  //
  espClient.stop();
  Serial.println(F("disconnected"));
  return 1;
}
