const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<title>CountDownClock</title>
<style>
body {
  background: #e7e7e7;  
}

.card{
  width: calc(100% - 10px);
  min-height: 250px;
  background: #5E5E5E;
  padding: 10px;
  box-sizing: border-box;
  color: #000;
  margin:10px;
  box-shadow: 0px 2px 18px -4px rgba(0,0,0,0.75);
}


.settingsDiv{
  position: absolute;
  top: 40px;
  right: 140px;
  z-index:2;
}

.inputDiv{
  position: relative;
  margin: auto;
  width: 800px;
}

.ClockValue{
  font-size: 5em;
}

button, input[type=submit] {
  background-color: #a7a7a7; 
  border: none;
  padding: 10px 16px;
  color: #000;
  text-align: center;
  text-decoration: none;
  display: inline-block;
  font-size: 12px;
  border-radius:6px;
}

button:hover,input[type=submit]:hover  {
  box-shadow: 0 12px 16px 0 rgba(0,0,0,0.24), 0 17px 50px 0 rgba(0,0,0,0.19);
}

.buttonSettings {
  padding: 10px 16px;
  font-size: 10px;
  border-radius:6px;
}
</style>

<script>
function settings() {
var link = document.createElement("a");
  link.href = "/settings";
  link.target = "_blank";
  link.click();
}

function getCurrent(){
  let dateInput = document.getElementById("targetTime");
  dateInput.min = new Date().toISOString().slice(0,new Date().toISOString().lastIndexOf(":"));

  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      var jsonObj= JSON.parse(this.responseText);
      document.getElementById("message").value = jsonObj.message;
    }
  };
  xhttp.open("GET", "readData", true);
  xhttp.send();
}

setInterval(function() {
  // Call a function repetatively with half Second interval
  readData();
}, 500); //500mSeconds update rate

function readData() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      var jsonObj= JSON.parse(this.responseText);
      document.getElementById("localTimeValue").innerHTML = jsonObj.localtime;
      document.getElementById("ClockValue").innerHTML = jsonObj.timeremaining;
      document.getElementById("endTimeValue").innerHTML = jsonObj.endtime;
    }
  };
  xhttp.open("GET", "readData", true);
  xhttp.send();
}
</script>

<body onLoad="getCurrent()">

<div class="settingsDiv" id="settingsDiv"> 
   <button class="buttonSettings" onclick="settings()">System Settings</button>
</div>

<div class="card">
  <h3>Local time: <span class="localTimeValue" id="localTimeValue">0</span></h3>
  <h3>End time: <span class="endTimeValue" id="endTimeValue">0</span></h3><br>  
  <h1>Time remaining: <br /><span class="ClockValue" id="ClockValue">0</span></h1><br>
</div>
<br /><br />


<div class="inputDiv" id="inputDiv">
<form action="/setTargetTime">
  <div><h3> Target time : <input id="targetTime" type="datetime-local" name="targetTime"/></div>
  <div><h3> countdown to : <input id="message" type="text" name="message" size="57" maxlength="57"/></div>
  <div><input type="submit" value="Set target date and time"></div>
  </h3>
</form>
 <br /><br /><br />


</body>
</html>
)=====";
