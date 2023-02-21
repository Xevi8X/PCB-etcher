

const char MAIN_page[] PROGMEM = R"=====(
<HTML>
	<HEAD>
			<TITLE>Etcher</TITLE>
	</HEAD>
<BODY>
	<script type = "text/javascript">  
		function increase() {   
			fetch("/target/increase", {
  			method: "POST"
			});
        }

		function decrease() {   
			fetch("/target/decrease", {
  			method: "POST"
			});
        }

		setInterval(function() {
  			getData();
		}, 200); //ms

		function getData() {
  			var xhttp = new XMLHttpRequest();
  			xhttp.onreadystatechange = function() {
    			if (this.readyState == 4 && this.status == 200) {
				var json = JSON.parse(this.responseText);
      			document.getElementById("target").innerHTML = json.target;
				document.getElementById("actual").innerHTML = json.actual;
				document.getElementById("power").innerHTML = json.power;
				document.getElementById("controller").innerHTML = json.controller;
				document.getElementById("status").innerHTML = json.status;
    			}
  			};
  			xhttp.open("GET", "data", true);
  			xhttp.send();
		}	  
	</script> 
	<CENTER>
		<p><h2>Info:</h2></p>
		<p>Target temperature: <span id="target">0</span> &#8451 <input type = "button" onclick = "increase()" value = "+">  <input type = "button" onclick = "decrease()" value = "-"></p> 
		<p>Actual temperature: <span id="actual">0</span> &#8451</p>
		<p>Heating power: <span id="power">0</span> %</p>
		<p>Controller: <span id="controller">UNKNOWN</span> </p>
		<p>Status: <span id="status">UNKNOWN</span> </p>
		<p><button onclick="location.href='/chart'">Charts</button> <button onclick="location.href='/controller'"> Controller </button></p>
	</CENTER>	
</BODY>
</HTML>
)=====";

const char chart_page[] PROGMEM = R"=====(
<HTML>
	<HEAD>
		<TITLE>Charts</TITLE>
		<meta name="viewport" content="width=device-width, initial-scale=1">
  		<script src=https://code.highcharts.com/highcharts.js></script>
	</HEAD>
	<BODY>
		<center>
			<button onclick="start()">Start</button> <button onclick="stop()">Stop</button> <button onclick="location.href='/'">Back</button>
		</center>
		<div id="chart-temperature" class="container"></div>
		<div id="chart-power" class="container"></div>
	</BODY>
	<script>  
		var isRunning = true;
  
		var chartT = new Highcharts.Chart({
  			chart:{ renderTo : 'chart-temperature' ,animation: false},
			credits: { enabled: false },
       		tooltip: { enabled: false },
  			title: { text: 'Temperature' },
  			series: [
			{
    			showInLegend: true,
    			data: [],
			color: '#00FF00',
			name: 'Actual'
  			},
			{
    			showInLegend: true,
    			data: [],
			color: '#0000FF',
			name: 'Target'
  			}
			],
  			plotOptions: {
    			line: { animation: false,
     			 dataLabels: { enabled: false }
    			},
    			series: { color: '#FF0000' }
 			 },
  			xAxis: { 
				title: { text: 'Time (s)' }
  			},
  			yAxis: {
    			title: { text: 'Temperature (&#8451)' }
  			},
  			credits: { enabled: false }
		});


		var chartPow = new Highcharts.Chart({
  			chart:{ renderTo : 'chart-power' ,animation: false},
			credits: { enabled: false },
       		tooltip: { enabled: false },
  			title: { text: 'Actual power' },
  			series: [{
    			showInLegend: false,
    			data: []
  			}],
  			plotOptions: {
    			line: { animation: false,
     			 dataLabels: { enabled: false }
    			},
    			series: { color: '#FF0000' }
 			 },
  			xAxis: { 
				title: { text: 'Time (s)' }
  			},
  			yAxis: {
    			title: { text: 'Power (%)' }
  			},
  			credits: { enabled: false }
		});

		setInterval(function() {
			if(isRunning)
			{
				getData();
			}
		}, 500); //ms

		function getData() {
  			var xhttp = new XMLHttpRequest();
  			xhttp.onreadystatechange = function() {
    			if (this.readyState == 4 && this.status == 200) {
					var json = JSON.parse(this.responseText);
					var t = document.timeline.currentTime/1000.0,
          			actual = json.actual,
					target = json.target,
					power = json.power;
      				if(chartT.series[0].data.length > 120) {
        				chartT.series[0].addPoint([t, actual], true, true, true);
						chartT.series[1].addPoint([t, target], true, true, true);
						chartPow.series[0].addPoint([t, power], true, true, true);
      				} else {
        				chartT.series[0].addPoint([t, actual], true, false, true);
						chartT.series[1].addPoint([t, target], true, false, true);
						chartPow.series[0].addPoint([t, power], true, false, true);
      				}
    			}
  			};
  			xhttp.open("GET", "data", true);
  			xhttp.send();
		}

		function start() { isRunning = true; }
		function stop() { isRunning = false; }  
	</script> 
</HTML>
)=====";

const char controller_page[] PROGMEM = R"=====(
<HTML>
	<HEAD>
			<TITLE>Controller</TITLE>
		<style>
			input {width: 70px}
			td {width: 100px}
			table, th, td {
  				border:1px solid black;
			}
		</style>
	</HEAD>
<BODY>
	<center>
		<button onclick="location.href='/'">Back</button>
		<p><h2>Controller:</h2></p>
		<p>Controller type: <span id="controller">UNKNOWN</span> </p>
		<p><button onclick="changeController(1)">PID</button> <button onclick="changeController(2)">Bang-bang</button> <button onclick="changeController(3)">None</button></p>
		<p><h2>PID:</h2></p>
		<table>
			<tr>
				<td>Kp: </td>
				<td align="right"><span id="kp">0</span></td>
				<td align="center"><input type="number" step="0.01" id="kp_input" name="kp_input"> </td>
			</tr>
			<tr>
				<td>Ki: </td>
				<td align="right"><span id="ki">0</span></td>
				<td align="center"><input type="number" step="0.01" id="ki_input" name="ki_input"> </td>
			</tr>
			<tr>
				<td>Kd: </td>
				<td align="right"><span id="kd">0</span></td>
				<td align="center"><input type="number" step="0.01" id="kd_input" name="kd_input"> </td>
			</tr>
			<tr>
				<td>Anty-windup: </td>
				<td align="right"><span id="antywindup">false</span></td>
				<td align="center"><button onclick="toggleAntywindup()">Toggle</button> </td>
			</tr>
		</table>
		
		
		
		<p><h2>Bang-bang:</h2></p>
		<table>
			<tr>
				<td>Hysteresis: </td>
				<td align="right"><span id="hysteresis">0</span> &#8451</td>
				<td align="center"><input type="number" step="0.1" id="hysteresis_input" name="hysteresis_input"></td>
			</tr>
		</table>

		<p><h2>None (manual):</h2></p>
		<table>
			<tr>
				<td>Power: </td>
				<td align="right"><span id="power">0</span> %</td>
				<td align="center"><input type="number" step="1" min="0" max="100" id="power_input" name="power_input"></td>
			</tr>
		</table>
		<button margin-top: 25px; onclick="update()">Update</button>
	</center>
</BODY>
<script type = "text/javascript">  
	function changeController(type_no) {
		let formData = new FormData();
		formData.append('type', type_no);
		fetch("controller/setType", {
  			method: "POST",
			body: formData
			});
	}

	function update() {
		let formData = new FormData();

		if(document.getElementById('kp_input').value != "") formData.append('kp', document.getElementById('kp_input').value);
		if(document.getElementById('ki_input').value != "") formData.append('ki', document.getElementById('ki_input').value);
		if(document.getElementById('kd_input').value != "") formData.append('kd', document.getElementById('kd_input').value);
		if(document.getElementById('hysteresis_input').value != "") formData.append('hysteresis', document.getElementById('hysteresis_input').value);
		if(document.getElementById('power_input').value != "") formData.append('power', document.getElementById('power_input').value);

		fetch("controller/setParams", {
  			method: "POST",
			body: formData
			});
	}

	function toggleAntywindup() {
		let formData = new FormData();
		formData.append('antywindup', 1);
		fetch("controller/setParams", {
  			method: "POST",
			body: formData
			});
	}	

	setInterval(function() {
		getData();
	}, 500); //ms

	function getData() {
		var xhttp = new XMLHttpRequest();
		xhttp.onreadystatechange = function() {
			if (this.readyState == 4 && this.status == 200) {
			var json = JSON.parse(this.responseText);
			document.getElementById("controller").innerHTML = json.controller;

			document.getElementById("kp").innerHTML = parseFloat(json.kp).toFixed( 2 );
			document.getElementById("ki").innerHTML = parseFloat(json.ki).toFixed( 2 );
			document.getElementById("kd").innerHTML = parseFloat(json.kd).toFixed( 2 );
			document.getElementById("antywindup").innerHTML = json.antywindup;

			document.getElementById("hysteresis").innerHTML = parseFloat(json.hysteresis).toFixed( 1 );

			document.getElementById("power").innerHTML = json.power;
			}
		};
		xhttp.open("GET", "controller/data", true);
		xhttp.send();
	}	  
</script> 
</HTML>
)=====";