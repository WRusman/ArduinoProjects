const char SETTINGS_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<title>Settings</title>
<style>
.card{
     background: #5E5E5E;
     padding: 30px;
     box-sizing: border-box;
     color: #FFF;
     margin:20px;
     box-shadow: 0px 2px 18px -4px rgba(0,0,0,0.75);
}

</style>
<body>

<form action="/saveSettings">
<br />
  <h4> Clock mode : </h4>
  <label for="clockMode"> select :</label>
  <select name="clockMode" id="clockmode">
    <option value="TZ">Countdown to the end time and stop</option>
    <option value="AO">Countdown to the end time and Show overtime</option>
  </select>
  <input type="submit" value="Save settings">
</form>
</body>
</html>
)=====";
