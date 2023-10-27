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
  <h4> Your location's time difference to UTC : </h4>
  <label for="timeZone"> select :</label>
  <select name="timeZone" id="timezone">
    <option value="-720">-12:00</option><option value="-660">-11:00</option><option value="-600">-10:00</option><option value="-570">-9:30</option><option value="-540">-9:00</option><option value="-480">-8:00</option>
    <option value="-420">-7:00</option><option value="-360">-6:00</option><option value="-300">-5:00</option><option value="-240">-4:00</option><option value="-210">-3:30</option><option value="-180">-3:00</option>
    <option value="-130">-2.5:00</option><option value="-120">-2:00</option><option value="-60">-1:00</option><option value="0">0:00</option><option value="60">+1:00</option><option value="120">+2:00</option>
    <option value="180">+3:00</option><option value="210">+3:30</option><option value="240">+4:00</option><option value="270">+4:30</option><option value="300">+5:00</option><option value="330">+5:30</option>
    <option value="345">+5:45</option><option value="360">+6:00</option><option value="390">+6:30</option><option value="420">+7:00</option><option value="480">+8:00</option><option value="525">+8:45</option>
    <option value="540">+9:00</option><option value="570">+9:30</option><option value="600">+10:00</option><option value="630">+10:30</option><option value="660">+11:00</option><option value="720">+12:00</option>
    <option value="765">+12:45</option><option value="780">+13:00</option><option value="840">+14:00</option>
  </select>
<br />
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
