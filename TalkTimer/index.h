const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<title>TalkTimer</title>
<style>
body {
  background: #e7e7e7;  
}

.card{
  width: calc(100% - 100px);
  min-height: 250px;
  background: #5E5E5E;
  padding: 10px;
  box-sizing: border-box;
  color: #000;
  margin:20px;
  box-shadow: 0px 2px 18px -4px rgba(0,0,0,0.75);
}

.clockStateDiv{
  width: 100%;
  text-align: center;
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
  font-size: 7em;
}

button, input[type=submit] {
  background-color: #a7a7a7; 
  border: none;
  padding: 15px 32px;
  color: #000;
  text-align: center;
  text-decoration: none;
  display: inline-block;
  font-size: 20px;
  border-radius:8px;
}

button:hover,input[type=submit]:hover  {
  box-shadow: 0 12px 16px 0 rgba(0,0,0,0.24), 0 17px 50px 0 rgba(0,0,0,0.19);
}

.buttonOnPause {
  width: calc(50% - 80px);
  margin:20px;
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

function sendState(newState) {
  var xhttp = new XMLHttpRequest();
  xhttp.open("GET", "setClockState?state="+newState, true);
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
      document.getElementById("ClockValue").style.color = jsonObj.clockcolor;
      document.getElementById("endTimeValue").innerHTML = jsonObj.endtime;
      document.getElementById("clockStateText").innerHTML = jsonObj.clockstate;
    }
  };
  xhttp.open("GET", "readData", true);
  xhttp.send();
}
</script>

<body>

<div class="settingsDiv" id="settingsDiv"> 
   <button class="buttonSettings" onclick="settings()">System Settings</button>
</div>

<div class="card">
  <h3>Local time: <span class="localTimeValue" id="localTimeValue">0</span></h3>
  <h3>End time: <span class="endTimeValue" id="endTimeValue">0</span></h3><br>  
  <h1>Time remaining: <span class="ClockValue" id="ClockValue">0</span></h1><br>
</div>
<br /><br />

<div class="clockStateDiv" id="clockStateDiv">
  <h4>Clock status is : </h4><h3><span id="clockStateText">NA</span></h3>
</div>
<br />
  <button class="buttonOnPause" onclick="sendState('On')">On</button>
  <button class="buttonOnPause" onclick="sendState('Pause')">Pause</button>  
<br /><br />

<div class="inputDiv" id="inputDiv">
<form action="/setTalkEnd">
  <h3> Set the talk ending time (HH:MM) :
  <input type="text" name="talkEnd" maxlength="5">
  <input type="submit" value="Set endtime">
  </h3>
</form>
 <br /><br /><br />


</body>
</html>
)=====";
