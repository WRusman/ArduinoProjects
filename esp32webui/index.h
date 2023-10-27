const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<title>webUI</title>
<style>
.card{
    max-width: 80%;
     min-height: 250px;
     background: #5E5E5E;
     padding: 30px;
     box-sizing: border-box;
     color: #FFF;
     margin:20px;
     box-shadow: 0px 2px 18px -4px rgba(0,0,0,0.75);
}

.textValue{
     color: #F00;
     font-size: 5em;
}

</style>
<body>

<div class="card">
  <h4>WebUI</h4><br>
  <h1>text: <span class="textValue" id="textValue">0</span></h1><br>
<BR>
<BR>

</div>

<form action="/setText">
  <h4> Set the text :</h4>
  <input type="text" name="text">
  <input type="submit" value="Set text">
</form>
<BR><BR>

<div id="input">
  <button class="button" onclick="sendState('On')">On</button>
  <button class="button" onclick="sendState('Off')">Off</button>
  <button class="button" onclick="sendState('Pause')">Pause</button>  
 </div>
 <BR>
  Status is : <span id="stateText">NA</span>

<script>
function sendState(newState) {
  var xhttp = new XMLHttpRequest();
  xhttp.open("GET", "setState?state="+newState, true);
  xhttp.send();
}

setInterval(function() {
  // Call a function repetatively with half Second interval
  getState();
  getText();
}, 1000); //500mSeconds update rate

function getState() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("stateText").innerHTML =
      this.responseText;
    }
  };
  xhttp.open("GET", "getState", true);
  xhttp.send();
}

function getText() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("textValue").innerHTML =
      this.responseText;
    }
  };
  xhttp.open("GET", "getText", true);
  xhttp.send();
}
</script>
</body>
</html>
)=====";
