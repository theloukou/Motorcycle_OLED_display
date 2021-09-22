const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body { text-align:center; }
    .vert { margin-bottom: 10%; }
    .hori{ margin-bottom: 0%; }
  </style>
</head>
<body>
  <div id="container">
    <h2>Duck Entertainment System</h2>
    <p>
      <button onclick="syncRTC();">Sync RTC</button>
    </p>
  </div>
</body>
<script>
  function syncRTC() {
    var now = new Date();
    var json = {
         "Y": now.getFullYear(),
         "M": now.getMonth()+1,
         "D": now.getDate(),
         "h": now.getHours(),
         "m": now.getMinutes(),
         "s": now.getSeconds()
        };
        
    var xhr = new XMLHttpRequest();
    xhr.open('POST', "/rtc");
    xhr.setRequestHeader('Content-Type', 'application/json');
    xhr.send(JSON.stringify(json));
    
    xhr.onload = function() {
      if (xhr.status == 200) {
        alert("RTC set!");
        location.reload();
      }
    };
  }
</script>
</html>
)rawliteral";
