const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<title>CountDownClock</title>
<style>
body {
  background: #e7e7e7;  
  min-width: 1200px;
  font-family: 'Courier New', monospace;
}

.card{
  width: 1190px;
  min-width: 1190px;
  max-width: 1190px;  
  min-height: 250px;
  background: #5E5E5E;
  padding: 10px;
  box-sizing: border-box;
  color: #000;
  margin:10px;
  box-shadow: 0px 2px 18px -4px rgba(0,0,0,0.75);
}

.inputDiv{
  position: relative;
  margin: auto;
  width: 1190px;
  min-width: 1190px;
  max-width: 1190px;
}

.inputFormText{
  display: inline-block;
  width: 120px;
}

.timeValue{
  font-size: 2em;
}

.clockValue{
  font-size: 5em;
  font-weight: bold;
}

button, input[type=submit] {
  background-color: #a7a7a7; 
  border: none;
  padding: 10px 16px;
  color: #000;
  text-align: center;
  text-decoration: none;
  display: inline-block;
  font-size: 16px;
  border-radius:6px;
  margin: 10px;
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

<div class="card">
  <div class="timeValue">Event time: <span id="endTimeValue">0</span></div>  
  <div class="timeValue">Local time: <span id="localTimeValue">0</span></div>
<br /><br />
  <div class="timeValue">Time remaining: <br /><span class="clockValue" id="ClockValue">0</span></div>
</div>
<br /><br />


<div class="inputDiv" id="inputDiv">
  <form action="/setTargetTime">
    <div><span class="inputFormText"> Event : </span><input id="message" type="text" name="message" size="57" maxlength="57"/></div>
    <div><span class="inputFormText"> Event date : </span><input id="targetTime" type="datetime-local" name="targetTime"/></div>
    <div align="center"><input type="submit" value="Store event"></div>
  </form>
 </div>


</body>
</html>
)=====";
