
const char head_1[] PROGMEM =    // cabeceras
  "<!DOCTYPE html>"
  "<html lang=\"es\">"
    "<head>"
      "<meta charset=\"UTF-8\">"
      "<title>Control Rover</title>"
      "<link rel=\"icon\" href=\"data:,\">"
      "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
      "<style>"
        "body {font-family:Arial, sans-serif; text-align: center; }"
        "button {padding:10px 20px; font-size: 16px; margin: 10px; }"
        "body{margin-top:50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n"
        ".button {display:inline-block;width: 80px;background-color: #3498db;border: none;color: white;padding: 13px 30px;"
        "text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n"
        ".button-on {background-color: #3498db;}"
        ".button-on:active {background-color: #2980b9;}"
        ".button-off {background-color: #34495e;}"
        ".button-off:active {background-color: #2c3e50;}"
        "p {font-size: 14px;color: #888;margin-bottom: 10px;}"
        ".indtemp {display:flex; justify-content: space-around; margin:10px 0;}"
        ".indicator {text-align: center;}"
        ".car {width: auto;height: auto;max-width: 60px;max-height: 60px;transition: transform 0.3s ease;"
        "transform-origin: center center;object-fit: contain;}"
        ".carminus {width: auto;height: auto;max-width: 40px;max-height: 40px;transition: transform 0.3s ease;"
        "transform-origin: center center;object-fit: contain;}"
        ".bar {width: 150px; height: 20px; background-color: #e0e0e0; border-radius: 10px; overflow: hidden; position: relative; margin: 10px auto; }"
        ".barfill {height: 100%; background-color: #3498db; width: var(--percent, 0%); }"
        ".bar::after {content: attr(data-value); font-size: 14px; position: absolute; top: 50%; left: 50%; transform: translate(-50%, -50%); color: #444444; }"
        // Media Queries para dispositivos pequeños 
        "@media (max-width: 300px) { button { font-size: 14px; padding: 8px 16px;}"
          ".car {width: 60px; height: 80px;}"
        ".bar {width: 80px;} barfill { height: 10px;}"
        "#streamImg {width: 240px;height: 180px;}"
        "h1 {font-size:24px;}"
        "p {font-size:12px;}"
        ".inline-buttons {flex-direction: column; gap: 5px;}}"

      "</style>"
    "</head>"
    "<body>"
    "<button onmousedown=\"mvD('camup')\" onmouseup=\"stopD()\" ontouchstart=\"mvD('camup')\" ontouchend=\"stopD()\">&#9650;</button><br>"
    "<div style=\"display: flex; align-items: center; justify-content: center; gap: 5px;\">"
      "<div><button onmousedown=\"mvD('caml')\" onmouseup=\"stopD()\" ontouchstart=\"mvD('caml')\" ontouchend=\"stopD()\">&#9664;</button>\n</div>"
      "<div><img id=\"streamImg\" src=\"http://";

  // aquí se inserta la variable other_ip  "192.168.11.92"

const char head_2[] PROGMEM =    // cabeceras
      ":81/stream\" alt=\"cam\" width=\"320\" height=\"240\" ></div>"
      "<div><button onmousedown=\"mvD('camr')\" onmouseup=\"stopD()\" ontouchstart=\"mvD('camr')\" ontouchend=\"stopD()\">&#9654;</button></div>"
    "</div>"
    "<button onmousedown=\"mvD('camdn')\" onmouseup=\"stopD()\" ontouchstart=\"mvD('camdn')\" ontouchend=\"stopD()\">&#9660;</button><br>\n"
    //"<p><span id=\"dUS1\">--</span>cm</p>"
    "<p>angulo: <span id=\"ang\">--</span> º --- distancia: <span id=\"dis\">--</span> cm</p>"

    "<canvas id=\"radarCanvas\" width=\"340\" height=\"180\"></canvas><br>"
    "<button onmousedown=\"mvD('fl')\" onmouseup=\"stopD('stop')\" ontouchstart=\"mvD('fl')\" ontouchend=\"stopD()\">Giro Izda</button>\n"
    "<button onmousedown=\"mvD('f')\" onmouseup=\"stopD()\" ontouchstart=\"mvD('f')\" ontouchend=\"stopD()\">Adelante</button>\n"
    "<button onmousedown=\"mvD('fr')\" onmouseup=\"stopD()\" ontouchstart=\"mvD('fr')\" ontouchend=\"stopD()\">Giro Dcha</button><br>\n"
    "<div style=\"display: flex; align-items: center; justify-content: center; gap: 5px;\">"
      "<div><button id=controlButton8 onmousedown=\"mvD('latl')\" onmouseup=\"stopD()\" ontouchstart=\"mvD('latl')\" ontouchend=\"stopD()\">Lat Izda</button></div>\n"
      "<div class=\"indicator\">"
        "<img src=\"/carback\" id=\"carelev\" class=\"carminus\">"
        "<p>Ala: <span id=\"e\">--</span>°</p>"
      "</div>"
      "<div class=\"indicator\">"
      "</div>"
      "<div class=\"indicator\">"
        "<img src=\"/carup\" id=\"carazim\" class=\"car\">"
        "<p><span id=\"a\">--</span>°</p>"
      "</div>"
      "<div class=\"indicator\">"
      "</div>"
      "<div class=\"indicator\">"
        "<img src=\"/carside\" id=\"cargiro\" class=\"car\">"
        "<p>Cab: <span id=\"g\">--</span>°</p>"
      "</div>"
      "<div><button onmousedown=\"mvD('latr')\" onmouseup=\"stopD()\" ontouchstart=\"mvD('latr')\" ontouchend=\"stopD()\">Lat Dcha</button>\n</div>"
    "</div>"
  "<button onmousedown=\"mvD('rl')\" onmouseup=\"stopD()\" ontouchstart=\"mvD('rl')\" ontouchend=\"stopD()\">Atras Izda</button>\n"
  "<button onmousedown=\"mvD('r')\" onmouseup=\"stopD()\" ontouchstart=\"mvD('r')\" ontouchend=\"stopD()\"> Atras  </button>\n"
  "<button onmousedown=\"stopD()\" onmouseup=\"stopD()\" ontouchstart=\"stopD()\" ontouchend=\"stopD()\"> STOP  </button>\n"
  "<button onmousedown=\"mvD('rr')\" onmouseup=\"stopD()\" ontouchstart=\"mvD('rr')\" ontouchend=\"stopD()\">Atras Der</button><br>\n"
  "<button onmousedown=\"mvD('rotl')\" onmouseup=\"stopD()\" ontouchstart=\"mvD('rotl')\" ontouchend=\"stopD()\">Rot Izda</button>\n"
  "<button onmousedown=\"mvD('rotr')\" onmouseup=\"stopD()\" ontouchstart=\"mvD('rotr')\" ontouchend=\"stopD()\">Rot Dcha</button><br>\n"
  // Botones de velocidad en la misma línea -->
  "<div style=\"display: flex; justify-content: center; align-items: center; gap: 10px; margin-top: 20px;\">"
    "<button onclick=\"sCom('speedmenos')\">---</button>\n"
    "<span id=\"sp_needle\">1000</span>"
    "<button onclick=\"sCom('speedmas')\">+++</button><br>"
   "</div>"   
   "<button onclick=\"sCom('luces')\">Luces</button>"
   "<button onclick=\"sCom('flash')\">Flash</button>"
   "<a href=\"/setup\"><button>Config</button></a>"
  "<div style=\"display: flex; justify-content: center; align-items: center; flex-direction: column; margin: 20px;\"></div>"
  "<div id=\"wsStatus\" style=\"font-size: 18px; margin-top: 20px; color: green;\"></div>"
    "<span id=\"t\" style=\"display: none;\">--</span>"
    "<span id=\"h\" style=\"display: none;\">--</span>"
    "<div class=\"indtemp\">"
      "<div class=\"indicator\">Temp"
        "<div id=\"tempBar\" class=\"bar\" data-value=\"0%\">"
          "<div class=\"barfill\" style=\"--percent: 0%;\"></div>"
        "</div>"
      "</div>"
      "<div class=\"indicator\">Hum"
        "<div id=\"humBar\" class=\"bar\" data-value=\"0%\">"
          "<div class=\"barfill\" style=\"--percent: 0%;\"></div>"
        "</div>"
      "</div>"
    "</div>"
    "RUEDAS<br>"
    "<button onmousedown=\"mvD('DIF')\" onmouseup=\"stopD('stop')\" ontouchstart=\"mvD('DIF')\" ontouchend=\"stopD()\">DI-F</button>\n"
    "<button onmousedown=\"mvD('DDF')\" onmouseup=\"stopD('stop')\" ontouchstart=\"mvD('DDF')\" ontouchend=\"stopD()\">DD-F</button>\n<br>"
    "<button onmousedown=\"mvD('DIR')\" onmouseup=\"stopD('stop')\" ontouchstart=\"mvD('DIR')\" ontouchend=\"stopD()\">DI-R</button>\n"
    "<button onmousedown=\"mvD('DDR')\" onmouseup=\"stopD('stop')\" ontouchstart=\"mvD('DDR')\" ontouchend=\"stopD()\">DD-R</button>\n<br><br>"
    "<button onmousedown=\"mvD('TIF')\" onmouseup=\"stopD('stop')\" ontouchstart=\"mvD('TIF')\" ontouchend=\"stopD()\">TI-F</button>\n"
    "<button onmousedown=\"mvD('TDF')\" onmouseup=\"stopD('stop')\" ontouchstart=\"mvD('TDF')\" ontouchend=\"stopD()\">TD-F</button>\n<br>"
    "<button onmousedown=\"mvD('TIR')\" onmouseup=\"stopD('stop')\" ontouchstart=\"mvD('TIR')\" ontouchend=\"stopD()\">TI-R</button>\n"
    "<button onmousedown=\"mvD('TDR')\" onmouseup=\"stopD('stop')\" ontouchstart=\"mvD('TDR')\" ontouchend=\"stopD()\">TD-R</button>\n";

const char script_01[] PROGMEM =
  "<script>"
    "let ws;"
    "let r_sp=1000;" // Velocidad inicial

    // Conectar WebSocket
    "function connectWS() {ws=new WebSocket(`ws://${location.hostname}:81/`);"
    "ws.onopen=()=>{"
      "console.log(\"WS On\");};"
      "document.getElementById(\"wsStatus\").innerText=\"ON\";"
      "document.getElementById(\"wsStatus\").style.color=\"green\";" // Cambia el color a verde

    "ws.onmessage=(event)=> "
      "{"
      //"console.log(\"Mensaje rec. WebSocket:\", event.data);"
      "try"
        "{"
        "let data=JSON.parse(event.data);"

        "if (data.type===\"sensor\")"
          "{"
          "upText(\"t\",data.t);"
          "upText(\"h\",data.h);"
          "upIndDht(data.t,data.h);"   // Actualizar indicadores gráficos
          "}"
          
        "else if (data.type===\"giros\")"
          "{"
          "upText(\"a\",data.a);"
          "upText(\"e\",data.e);"
          "upText(\"g\",data.g);"
          "upIndGiros(data.a,data.e,data.g,data.dUS1);" // Actualizar indicadores gráficos
          "}"
        "else if (data.type===\"radar\")"
          "{"
          "upText(\"ang\",data.ang);"
          "upText(\"dis\",data.dis);"
          "upIndRadar(data.ang,data.dis);" // Actualizar indicadores gráficos
          "}"
        "}"
        "catch(e) {console.error(\"Error datos:\",e);"
        "}"
      "};"  

    "ws.onclose = () => {"
      "console.log(\"WS Off\");"
      "document.getElementById(\"wsStatus\").innerText = \"OFF\";"
      "document.getElementById(\"wsStatus\").style.color = \"red\";" // Cambia el color a rojo
      "setTimeout(connectWS, 2000);};" // Reconectar después de 3 segundos
      "ws.onerror = (error) => {"
      "console.error(\"Error WS:\", error);};}"

      "function upText(id, value) {"
        "document.getElementById(id).innerText = value.toFixed(1);}"

      "let moveInterval;"

      "function sCom(command) {"
          "if (ws && ws.readyState === WebSocket.OPEN) {"
            "if (command === 'stop') {"
              "clearInterval(moveInterval);" // Si el comando es 'stop', detén el intervalo y envía el comando de parada
              "ws.send(command);}"
            "else"
              "{ws.send(command);}" // Para otros comandos, envíalos directamente
            "}"
          "else {"
            "console.error(\"WS Off\");}"
          "}"

    // Iniciar conexión WebSocket
    "connectWS();"

    // mover cámara
    "let mvInt;\n"

    "function mvD(command,stopCmd='stop') {"
      "sCom(command);"
      "mvInt=setInterval(()=>{ sCom(command);},50);}"
    "function stopD(stopCmd='stop') {"
      "clearInterval(mvInt);"
      "sCom(stopCmd);}"

    "function upIndDht(t,h) {"
      // Actualizar gráficos de temperatura y humedad
      "const tBFill=document.querySelector(\"#tempBar .barfill\");"
      "const hBFill=document.querySelector(\"#humBar .barfill\");"
      "tBFill.style.width=`${(t/50)*100}%`;"
      "tBFill.parentElement.setAttribute(\"data-value\",`${t.toFixed(1)}°C`);"
      "hBFill.style.width=`${h}%`;"
      "hBFill.parentElement.setAttribute(\"data-value\",`${h.toFixed(1)}%`);}"

    "function upIndGiros(a,e,g) {"
      "document.getElementById(\"carazim\").style.transform=`rotate(${a}deg)`;"
      "document.getElementById(\"carelev\").style.transform=`rotate(${e}deg)`;"
      "document.getElementById(\"cargiro\").style.transform=`rotate(${g}deg)`;"
      "document.getElementById(\"a\").innerText=a.toFixed(1);"
      "document.getElementById(\"e\").innerText=e.toFixed(1);"
      "document.getElementById(\"g\").innerText=g.toFixed(1);}"
  "let detections= new Array(181).fill(null);" // Almacena distancias por ángulo (-90 a 90)
  "let lastAng=-90;" // Último ángulo detectado

  "function upIndRadar(ang,dis) {"
    // Redondear y mostrar valores de ángulo y distancia
    "document.getElementById(\"ang\").innerText=ang.toFixed(1);"
    "document.getElementById(\"dis\").innerText=(dis/10).toFixed(1);"

    // Guardar la distancia en el array de detecciones (ajustando índice de -90 a 90)
    "let index=ang+90;"
    "detections[index]=dis;"
    "lastAng=ang;" // Guardar el ángulo actual para el barrido

    // Obtener el canvas y su contexto
    "let canvas=document.getElementById(\"radarCanvas\");"
    "let ctx=canvas.getContext(\"2d\");"
    "ctx.fillStyle=\"#006400\";" // Color verde oscuro
    "ctx.fillRect(0,0,canvas.width,canvas.height);"
    // Definir el centro del radar
    "let centerX=canvas.width / 2;"
    "let centerY=canvas.height;"

      // Añadir las circunferencias concéntricas con sus etiquetas de distancia\n" +
    "let distances=[30,60,90,120,150];" // Distancias para las circunferencias
    "ctx.lineWidth=1;" // Ajustar el grosor de las líneas
    "ctx.strokeStyle=\"#32CD32\";" // Color de las circunferencias\n" +
    "ctx.fillStyle=\"black\";" // Color de las etiquetas de distancia\n" +
    "distances.forEach(function(dist) {"
      "ctx.beginPath();"
      "ctx.arc(centerX,centerY,dist,Math.PI,2*Math.PI);"
      "ctx.stroke();"
        "ctx.fillText(dist+\" cm\",centerX+5,centerY-dist);" // Etiqueta de distancia
        "});"

    // Dibujar los radios para los ángulos\n" +
    "let angleSteps=[0,30,60,90,120,150,180];" 
    "ctx.lineWidth=1;"
    "ctx.strokeStyle=\"#90EE90\";" 
    "angleSteps.forEach(function(angle) {" 
      "let angRad=(angle*Math.PI)/180;" // Convertir el ángulo a radianes
      "let sweepX=centerX+Math.cos(angRad)*150;"
      "let sweepY=centerY-Math.sin(angRad)*150;"
      "ctx.beginPath();"
      "ctx.moveTo(centerX,centerY);"
      "ctx.lineTo(sweepX,sweepY);"
      "ctx.stroke();"
      "});"

    // Dibujar todos los puntos detectados en el radar
    "for (let i=0;i<detections.length;i++) {"
      "if (detections[i]!==null) {"
        "let angRad=((i-90)*Math.PI)/180;" // Convertir índice a ángulo real
        "let maxDist=300;" // Distancia máxima representada en píxeles
        "let x=centerX+Math.cos(angRad)*(detections[i]/2750)*maxDist;"  // 2750 es experimental
        "let y=centerY-Math.sin(angRad)*(detections[i]/2750)*maxDist;"
        "ctx.beginPath();"
        "ctx.arc(x,y,2,0,2*Math.PI);"
        "ctx.fillStyle=\"#90EE90\";"
        "ctx.fill();"
      "}"
    "}"

    // Dibujar el barrido de radar (línea verde en el ángulo actual)
    "let angRad=(lastAng*Math.PI)/180;"
    "let sweepX=centerX+Math.cos(angRad)*150;"
    "let sweepY=centerY-Math.sin(angRad)*150;"

    "ctx.beginPath();"
    "ctx.moveTo(centerX, centerY);"
    "ctx.lineTo(sweepX, sweepY);"
    "ctx.strokeStyle = \"limegreen\";"
    "ctx.lineWidth = 2;"
    "ctx.stroke();"
  "}"
    "</script>"
    "</body>"
  "</html>";

//////////////////////////////////////////////////
// página setup
const char setup_page_1[] PROGMEM =    // cabeceras
  "<!DOCTYPE html>"
  "<html lang=\"es\">"
  "<head>"
    "<meta charset=\"UTF-8\">"
    "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
    "<title>Configuración</title>"
    "<style>"
      "body "
        "{font-family: Arial, sans-serif;text-align: center;padding: 20px;}"
        "input, button {padding: 10px;margin: 10px;font-size: 16px;border-radius: 5px;"
            "border:1px solid #ccc;}"
        "button {background-color:#3498db;color:white;cursor:pointer;}"
        "button:hover {background-color:#2980b9;}"
        ".ssid-list {margin-top:20px;padding:10px;"
            "border: 1px solid #ccc;max-height: 150px;overflow-y:auto;}"
        ".ssid-list div {padding:5px;margin:5px;background-color:#f1f1f1;border-radius: 4px;}"
        ".ssid-list div:hover {background-color:#ddd;}"
    "</style>"
  "</head>"

  "<body>"
    "<h2>Configuración</h2>";

const char setup_page_2[] PROGMEM =    // cabeceras
    "<div><label for=\"modoinic\">Modo inicio:</label>"
    "<input type=text id=\"modoinic\" placeholder=\"0,1,2\"></div>"
    "<div><label for=\"ssid\">SSID:</label><input type=text id=\"ssid\" placeholder=\"SSID\"></div>"
    "<div><label for=\"password\">Contraseña:</label><input type=password id=password placeholder=\"Contraseña\"></div>"
    "<div><button id=\"searchSSIDButton\" onclick=\"searchSSIDs()\">Buscar SSIDs</button>"
    "<button id=\"saveButton\" onclick=\"saveData()\">Guardar</button></div>"
    "<div class=\"ssid-list\" id=\"ssidList\">"
      //<!-- Lista de SSIDs disponibles se mostrará aquí -->
    "</div>"

  "<script>"
    // Función para buscar SSIDs disponibles
    "function searchSSIDs() {"
      "let ws=new WebSocket(`ws://${location.hostname}:81/`);"
      "ws.onopen=()=>{ws.send('getssids');};"

    "ws.onmessage=(event) => {"
      "const ssids=JSON.parse(event.data);"  // Suponiendo que el ESP32 responde con una lista de SSIDs
        //"console.log(\"Tipo de ssids:\", typeof ssids);" // Verifica el tipo de dato (debe ser 'object' o 'array').
        //"console.log(\"Lista de SSIDs:\", ssids);" // Imprime los datos parseados.
        //Verifica si ssids es realmente un array.
      "if (!Array.isArray(ssids)) {throw new Error(\"El dato recibido no es un array válido.\");}"
      "const ssidList = document.getElementById(\"ssidList\");"
      "ssidList.innerHTML='';"
      "ssids.forEach(ssid=>{"
        "const ssidDiv=document.createElement(\"div\");"
        "ssidDiv.textContent=ssid;ssidDiv.onclick=()=>{"
        "document.getElementById(\"ssid\").value=ssid;};"
        "ssidList.appendChild(ssidDiv);});"
      "};}"

    "function saveData() {"
        //"console.log(\"Guardando datos\");" 
        "const modoinic=document.getElementById('modoinic').value;"
        "const ssid=document.getElementById('ssid').value;"
        "const password=document.getElementById('password').value;"

        "const data={modoinic: modoinic,ssid: ssid,password: password};"

        "fetch('http://'+location.hostname+'/save_config',{"
            "method:'POST',headers:{'Content-Type':'application/json'},"
            "body:JSON.stringify(data)})"
        ".then(response=>response.json())"
        ".then(data=>{alert('Configuración guardada');})"
        ".catch((error)=>{console.error('Error:',error);});"
    "}"

    "function loadConfig() {"
      "fetch('http://'+location.hostname+'/get_config')"
        ".then(response=>response.json())"
        ".then(data=>{"
          "document.getElementById('modoinic').value=data.modoinic || \"\";"
          "document.getElementById('ssid').value=data.ssid || \"\";"
          "document.getElementById('password').value=data.password || \"\";"
          "})"
        ".catch(error=>console.error('Error al cargar la configuración:',error));}"

    "window.onload = loadConfig;"

    "</script>"
    "</body>"
  "</html>";
