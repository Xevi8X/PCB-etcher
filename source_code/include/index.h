

const char MAIN_page[] PROGMEM = R"=====(
<HTML>
	<HEAD>
			<TITLE>Wytrawiarka</TITLE>
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
				document.getElementById("status").innerHTML = json.status;
    			}
  			};
  			xhttp.open("GET", "data", true);
  			xhttp.send();
		}	  
	</script> 
	<CENTER>
		<p>Target temperature: <span id="target">0</span> &#8451 <input type = "button" onclick = "increase()" value = "+">  <input type = "button" onclick = "decrease()" value = "-"></p> 
		<p>Actual temperature: <span id="actual">0</span> &#8451</p>
		<p>Heating power: <span id="power">0</span> %</p>
		<p>Status: <span id="status">UNKNOWN</span> </p>
		<p><button onclick="location.href='/chart'">Otw&#243rz wykresy</button></p>
	</CENTER>	
</BODY>
</HTML>
)=====";

const char chart_temp[] PROGMEM = R"=====(
<HTML>
	<HEAD>
		<TITLE>Wykresy</TITLE>
		<meta name="viewport" content="width=device-width, initial-scale=1">
  		<script src=https://code.highcharts.com/highcharts.js></script>
	</HEAD>
	<BODY>
		<center>
			<button onclick="start()">Start</button> <button onclick="stop()">Stop</button> <button onclick="location.href='/'">Powr&#243t</button>
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
  			title: { text: 'Temperatura' },
  			series: [
			{
    			showInLegend: true,
    			data: [],
			color: '#00FF00',
			name: 'aktualna'
  			},
			{
    			showInLegend: true,
    			data: [],
			color: '#0000FF',
			name: 'zadana'
  			}
			],
  			plotOptions: {
    			line: { animation: false,
     			 dataLabels: { enabled: false }
    			},
    			series: { color: '#FF0000' }
 			 },
  			xAxis: { 
				title: { text: 'Czas (s)' }
  			},
  			yAxis: {
    			title: { text: 'Temperatura (&#8451)' }
  			},
  			credits: { enabled: false }
		});


		var chartPow = new Highcharts.Chart({
  			chart:{ renderTo : 'chart-power' ,animation: false},
			credits: { enabled: false },
       		tooltip: { enabled: false },
  			title: { text: 'Aktualna moc' },
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
				title: { text: 'Czas (s)' }
  			},
  			yAxis: {
    			title: { text: 'Moc (%)' }
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