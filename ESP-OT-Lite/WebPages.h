
const char rootPage[] PROGMEM = R"=====(

<!DOCTYPE html>
<html lang='en'>

<head>
<title>ESP-OpenTherm</title>
<meta http-equiv='Content-Type' content='text/html; charset=UTF-8'>
<meta name="viewport" content="width=device-width, initial-scale=0.8">
<meta http-equiv='refresh' content='{rftime};URL=/'>

<style type='text/css'>
body {
 text-align:center;
 font-family:verdana
}
body h2 {
 margin:auto
}
.wrap {
 text-align:center;
 display:inline-block;
 min-width:260px;
 max-width:400px
}
div,input {
 padding:5px;
 font-size:1em;
 margin:5px 0;
 box-sizing:border-box
}
input[type="range"], input[type="submit"] {
    width: 90%;
}
</style>

<script>
function setRangeValue(ElementID) {
	value = document.getElementById(ElementID).value;
	document.getElementById(ElementID+'_new').value = value;
}
var xmlHttp = createXmlHttpObject();
function createXmlHttpObject() {
	if (window.XMLHttpRequest) {
		xmlHttp = new XMLHttpRequest();
	}
	else {
		xmlHttp = new ActiveXObject('Microsoft.XMLHTTP');
	} 
	return xmlHttp;
}
function process() {
	if (xmlHttp.readyState == 0 || xmlHttp.readyState == 4) {
		xmlHttp.open('GET','status',true);
		xmlHttp.onreadystatechange = handleServerResponse;
		xmlHttp.send(null);
	} 
	setTimeout('process()',1000);
}
function handleServerResponse() {
	if (xmlHttp.readyState == 4 && xmlHttp.status == 200) {
		var jsonResponse = JSON.parse(xmlHttp.responseText);
		document.getElementById('build_name').innerHTML = jsonResponse.build_name;
		document.getElementById('build_ver').innerHTML = jsonResponse.build_ver;
		document.getElementById('build_date').innerHTML = jsonResponse.build_date;
		document.getElementById('ot_inpin').innerHTML = jsonResponse.ot_inpin;
		document.getElementById('ot_outpin').innerHTML = jsonResponse.ot_outpin;
		document.getElementById('ot_power').innerHTML = jsonResponse.ot_power;
		ot_power_new_state = ((jsonResponse.ot_power=="OFF")? "ON":"OFF");
		document.getElementById('ot_power_href').href = "/?ot_power="+ot_power_new_state;
		document.getElementById('ot_power_href').title = "Switch "+ot_power_new_state;
		document.getElementById('ot_status').innerHTML = jsonResponse.ot_status;
		document.getElementById('flame_status').innerHTML = jsonResponse.flame_status;
		document.getElementById('ot_modLevel').innerHTML = jsonResponse.ot_modLevel;
		document.getElementById('boiler_temp').innerHTML = jsonResponse.boiler_temp;
		document.getElementById('boiler_setpoint').innerHTML = jsonResponse.boiler_setpoint;
		document.getElementById('boiler_sp_min').innerHTML = jsonResponse.boiler_sp_min;
		document.getElementById('boiler_sp_max').innerHTML = jsonResponse.boiler_sp_max;
		document.getElementById('water_temp').innerHTML = jsonResponse.water_temp;
		document.getElementById('water_setpoint').innerHTML = jsonResponse.water_setpoint;
		document.getElementById('water_sp_min').innerHTML = jsonResponse.water_sp_min;
		document.getElementById('water_sp_max').innerHTML = jsonResponse.water_sp_max;
		document.getElementById('boiler_status').innerHTML = jsonResponse.boiler_status;
		document.getElementById('boiler_mode').innerHTML = jsonResponse.boiler_mode;
		boiler_mode_new_state = ((jsonResponse.boiler_mode=="OFF")? "ON":"OFF");
		document.getElementById('boiler_mode_href').href = "/?boiler_mode="+boiler_mode_new_state;
		document.getElementById('boiler_mode_href').title = "Switch "+boiler_mode_new_state;
		document.getElementById('water_status').innerHTML = jsonResponse.water_status;
		document.getElementById('water_mode').innerHTML = jsonResponse.water_mode;
		water_mode_new_state = ((jsonResponse.water_mode=="OFF")? "ON":"OFF");
		document.getElementById('water_mode_href').href = "/?water_mode="+water_mode_new_state;
		document.getElementById('water_mode_href').title = "Switch "+water_mode_new_state;
		document.getElementById('uptime').innerHTML = jsonResponse.uptime;
	}
}
</script>
</head>

<body onload='process()'>

<div class='wrap'>
<h2>ESP-Opentherm</h2>

<div>
<table width='100%'>
<tr>
<td>Opentherm (<span id='ot_inpin'>...</span>,<span id='ot_outpin'>...</span>) status:</td>
<td><span id='ot_status'>...</span></td>
</tr>
<tr>
<td>Flame status [modLevel]:</td>
<td><span id='flame_status'>...</span> [<span id='ot_modLevel'>0</span>%]</td>
</tr>
<tr>
<td>Boiler CH temp [setpoint]:</td>
<td><span id='boiler_temp'>0.0</span> [<span id='boiler_setpoint'>60</span>°C]</td>
</tr>
<tr>
<td>Boiler DHW temp [setpoint]:</td>
<td><span id='water_temp'>0.0</span> [<span id='water_setpoint'>50</span>°C]</td>
</tr>
<tr>
<td>Boiler CH status [mode]:</td>
<td><span id='boiler_status'>...</span> [<a id='boiler_mode_href' href='/?boiler_mode=ON' title='Switch ON'><span id='boiler_mode'>OFF</span></a>]</td>
</tr>
<tr>
<td>Boiler DHW status [mode]:</td>
<td><span id='water_status'>...</span> [<a id='water_mode_href' href='/?water_mode=ON' title='Switch ON'><span id='water_mode'>OFF</span></a>]</td>
</tr>
<tr>
<td>Boiler power control:</td>
<td>[<a id='ot_power_href' href='/?ot_power=ON' title='Switch ON'><span id='ot_power'>OFF</span></a>]</td>
</tr>
</table>
</div>

<hr>

<h3>Boiler settings</h3>
<hr>

<div>
<table width='100%'>
<tr>
<td colspan='2'>Set new CH setpoint: <output id='boiler_sp_new'>{bsp}</output> [<span id='boiler_sp_min'>0</span>...<span id='boiler_sp_max'>100</span>]</td>
</tr>
<form method='GET' action='/'>
<tr>
<td><input type='range' id='boiler_sp' name='boiler_sp' oninput='setRangeValue("boiler_sp")' min='{bsp_min}' max='{bsp_max}' step='1' value='{bsp}'></td>
<td><input value='Set' type='submit'></td>
</tr>
</form>
<tr>
<td colspan='2'>Set new DHW setpoint: <output id='water_sp_new'>{wsp}</output> [<span id='water_sp_min'>0</span>...<span id='water_sp_max'>100</span>]</td>
</tr>
<form method='GET' action='/'>
<tr>
<td><input type='range' id='water_sp' name='water_sp' oninput='setRangeValue("water_sp")' min='{wsp_min}' max='{wsp_max}' step='1' value='{wsp}'></td>
<td><input value='Set' type='submit'></td>
</tr>
</form>
</table>
</div>

<hr>

<div>
<span id='build_name'>ESP-OT-...</span> ver: <span id='build_ver'>...</span> (<span id='build_date'>...</span>)
<br>System uptime: <span id='uptime'>00:00:00</span>
<br>
<a href='/restart'>Restart</a> | <a href='/update'>Update</a>
</div>

</div>
</body>
</html>

)=====";

