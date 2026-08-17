#include <SPI.h>
#include <Ethernet.h>
#include <DHT.h>

/* ================= CONFIG ================= */
#define DHTPIN 5
#define DHTTYPE DHT11
#define LED_PIN 2
#define BUZZER_PIN 3
#define TEMP_ALERT 35.0

int relayPins[5] = {30,31,32,33,34};


/* ================= LOGS ================= */
#define LOG_SIZE 15
String logs[LOG_SIZE];
int logIndex = 0;

/* ================= ETHERNET ================= */
byte mac[] = { 0xDE,0xAD,0xBE,0xEF,0xFE,0xED };
IPAddress ip(192,168,1,177);
EthernetServer server(80);

/* ================= OBJETS ================= */
DHT dht(DHTPIN, DHTTYPE);

/* ================= VARIABLES ================= */
bool relayState[5] = {false};
bool ledState = false;
bool buzzerState = false;
unsigned long lastBlink = 0;

/* ================= LOG FUNCTION ================= */
void addLog(String msg) {
  logs[logIndex] = msg;
  logIndex = (logIndex + 1) % LOG_SIZE;
}

/* ================= SETUP ================= */
void setup() {
  pinMode(53, OUTPUT);
  digitalWrite(53, HIGH);
  delay(3000);
  for (int i=0;i<5;i++){
    pinMode(relayPins[i], OUTPUT);
    digitalWrite(relayPins[i], LOW);
  }

  pinMode(10, OUTPUT);
  digitalWrite(10, HIGH);
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  dht.begin();
  Ethernet.init(10);
  Ethernet.begin(mac, ip);
  server.begin();

  addLog("Système demarre");
}


/* ================= LOOP ================= */
void loop() {
  EthernetClient client = server.available();
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();
  

  if (isnan(temp)) temp = 0.0; // Sécurité si le DHT11 rate une lecture

  /* ALERTES */
  if (temp >= TEMP_ALERT) {
    if (millis() - lastBlink > 500) {
      ledState = !ledState;
      buzzerState = !buzzerState;
      digitalWrite(LED_PIN, ledState);
      digitalWrite(BUZZER_PIN, buzzerState);
      lastBlink = millis();
    }
  } else {
    digitalWrite(LED_PIN, LOW);
    digitalWrite(BUZZER_PIN,LOW);
  }

  if (client) {
    String req = client.readStringUntil('\r');
    client.flush();

     

    /* ===== TRAITEMENT DES RELAIS ===== */  
    for (int i=0; i<5; i++) {  
      if (req.indexOf("GET /?relay" + String(i)) >= 0) {  
        relayState[i] = !relayState[i];  
        digitalWrite(relayPins[i], relayState[i] ? HIGH : LOW);  
        addLog("Switch " + String(i+1) + (relayState[i] ? " ON" : " OFF"));  
        
        // Redirection immédiate vers la racine pour nettoyer l'URL et éviter le bug au rafraîchissement manuel
        client.println("HTTP/1.1 303 See Other");  
        client.println("Location: /");  
        client.println();  
        client.stop();  
        return;  
      }  
    }  

    /* ===== API JSON POUR AJAX ===== */  
    if (req.indexOf("GET /data") >= 0) {
      client.println("HTTP/1.1 200 OK");  
      client.println("Content-Type: application/json");  
      client.println("Connection: close");  
      client.println();  
      
      client.print("{");
      client.print("\"temp\":" + String(temp, 1) + ",");
      client.print("\"hum\":" + String(hum, 1) + ",");
      client.print("\"relais\":[");
      for(int i=0; i<5; i++){
        client.print(relayState[i] ? "true" : "false");
        if(i < 4) client.print(",");
      }
      client.print("],\"logs\":[");
      int logCount = 0;
      for (int i=0; i<LOG_SIZE; i++) {  
        int idx = (logIndex + i) % LOG_SIZE;  
        if (logs[idx] != "") {  
          if(logCount > 0) client.print(",");
          client.print("\"" + logs[idx] + "\"");
          logCount++;
        }  
      }  
      client.print("]}");
      client.stop();
      return;
    }

    /* ===== PAGE PRINCIPALE ===== */  
    client.println("HTTP/1.1 200 OK");  
    client.println("Content-Type: text/html");  
    client.println();  
    
    client.println("<!DOCTYPE html><html><head><meta charset='UTF-8'>");  
    client.println("<meta name='viewport' content='width=device-width, initial-scale=1.0'>");
    client.println("<title>Smart Home</title>");
    
    // CSS Moderne, épuré et responsive
    client.println("<style>");  
    client.println("body{font-family:'Segoe UI',Arial,sans-serif;background:#f4f6f9;margin:0;padding:20px;color:#333}");  
    client.println(".container{max-width:900px;margin:0 auto}");  
    client.println("h2,h3{color:#222;border-bottom:2px solid #ddd;padding-bottom:8px}");  
    client.println(".sensors{display:grid;grid-template-columns:1fr 1fr;gap:20px;margin-bottom:25px}");  
    client.println(".box{padding:20px;border-radius:10px;background:#fff;box-shadow:0 2px 8px rgba(0,0,0,0.05);font-size:18px;font-weight:600;display:flex;justify-content:space-between;align-items:center}");  
    client.println(".grid{display:grid;grid-template-columns:repeat(auto-fit,minmax(140px,1fr));gap:15px;margin-bottom:25px}");  
    client.println(".grid a{text-decoration:none}");
    client.println("button{width:100%;padding:15px 10px;border:none;border-radius:8px;font-size:14px;font-weight:bold;cursor:pointer;transition:all 0.2s ease;box-shadow:0 2px 5px rgba(0,0,0,0.05)}");  
    client.println(".btn-off{background:#e9ecef;color:#495057}.btn-off:hover{background:#dee2e6}");  
    client.println(".btn-on{background:#28a745;color:white;box-shadow:0 4px 10px rgba(40,167,69,0.3)}");  
    client.println(".log-card{background:white;padding:20px;border-radius:10px;box-shadow:0 2px 8px rgba(0,0,0,0.05)}");  
    client.println("ul{list-style:none;padding:0;margin:0;max-height:200px;overflow-y:auto}");  
    client.println("li{padding:8px 12px;border-bottom:1px solid #eee;font-family:monospace;font-size:13px;color:#555}");  
    client.println("li:first-child{font-weight:bold;color:#000}");  
    client.println("</style></head><body>");  
    
    client.println("<div class='container'><h2>Smart Home Controller</h2>");  
    
    // Zone Capteurs
    client.println("<div class='sensors'>");  
    client.println("<div class='box' id='tempBox'>Temperature: <span id='v-temp'>--</span> °C</div>");  
    client.println("<div class='box' id='motionBox'>Humidité: <span id='v-hum'>--</span> %</div>");  
    client.println("</div>");  
    
    // Zone Relais
    client.println("<h3>Commandes Relais</h3><div class='grid'>");  
    for (int i=0; i<5; i++) {  
      client.print("<a href='/?relay" + String(i) + "'><button id='btn-" + String(i) + "' class='");  
      client.print(relayState[i] ? "btn-on" : "btn-off");  
      client.print("'>Switch " + String(i+1));  
      client.print(relayState[i] ? " ON" : " OFF");  
      client.println("</button></a>");  
    }  
    client.println("</div>");  
    
    // Zone Journaux
    client.println("<div class='log-card'><h3>Journal des événements</h3><ul id='logList'>");  
    client.println("</ul></div></div>");  
    
    // SCRIPT AJAX BAS NIVEAU (XMLHttpRequest)
    client.println("<script>");
    client.println("function updateData(){");
    client.println("  var xhr = new XMLHttpRequest();");
    client.println("  xhr.onreadystatechange = function(){");
    client.println("    if(this.readyState == 4 && this.status == 200){");
    client.println("      var data = JSON.parse(this.responseText);");
    
    // MAJ Température
    client.println("      document.getElementById('v-temp').innerText = data.temp;");
    client.println("      if(data.temp >= " + String(TEMP_ALERT) + "){");
    client.println("        document.getElementById('tempBox').style.background = '#dc3545'; document.getElementById('tempBox').style.color = 'white';");
    client.println("      }else{");
    client.println("        document.getElementById('tempBox').style.background = '#fff'; document.getElementById('tempBox').style.color = '#333';");
    client.println("      }");
    

  // MAJ Humidité
    client.println("      document.getElementById('v-hum').innerText = data.hum;");
    client.println("      if(data.temp >= " + String(TEMP_ALERT) + "){");
    client.println("        document.getElementById('motionBox').style.background = '#dc3545'; document.getElementById('motionBox').style.color = 'white';");
    client.println("      }else{");
    client.println("        document.getElementById('motionBox').style.background = '#fff'; document.getElementById('motionBox').style.color = '#333';");
    client.println("      }");


   
    // MAJ Boutons Relais (au cas où l'état change en arrière plan)
    client.println("      for(var i=0; i<10; i++){");
    client.println("        var btn = document.getElementById('btn-'+i);");
    client.println("        if(data.relais[i]){");
    client.println("          btn.className = 'btn-on'; btn.innerText = 'Switch ' + (i+1) + ' ON';");
    client.println("        }else{");
    client.println("          btn.className = 'btn-off'; btn.innerText = 'Switch ' + (i+1) + ' OFF';");
    client.println("        }");
    client.println("      }");
    
    // MAJ Logs
    client.println("      var logHtml = '';");
    client.println("      for(var i=0; i<data.logs.length; i++){ logHtml += '<li>' + data.logs[i] + '</li>'; }");
    client.println("      document.getElementById('logList').innerHTML = logHtml;");
    
    client.println("    }");
    client.println("  };");
    client.println("  xhr.open('GET', '/data', true);");
    client.println("  xhr.send();");
    client.println("}");
    client.println("updateData(); setInterval(updateData, 2000);"); // Lance toutes les 2 secondes
    client.println("</script>");
    
    client.println("</body></html>");  
    client.stop();
  }
}
