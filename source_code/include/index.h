#include "heating.h"

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
				document.getElementById("state").innerHTML = json.state;
    			}
  			};
  			xhttp.open("GET", "data", true);
  			xhttp.send();
		}	  
	</script> 
	<CENTER>
		<p>Target temperature: <span id="target">0</span> &#8451 <input type = "button" onclick = "increase()" value = "+">  <input type = "button" onclick = "decrease()" value = "-"></p> 
		<p>Actual temperature: <span id="actual">0</span> &#8451</p>
		<p>Heating: <span id="state">UNKNOWN</span> </p>
	</CENTER>	
</BODY>
</HTML>
)=====";

inline const char* heatingToString(Heating state)
{
    switch(state)
    {
        case(Heating::ON):
            return "ON";
        case(Heating::OFF):
            return "OFF";
        case(Heating::ERROR):
            return "ERROR";
    }
	return "UNKNOWN";
}