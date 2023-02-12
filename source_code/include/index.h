#include "heating.h"

const char MAIN_page[] PROGMEM = R"=====(
<HTML>
	<HEAD>
			<TITLE>Wytrawiarka</TITLE>
			<meta http-equiv="refresh" content="3">
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
	</script> 
	<CENTER>
		<p>Target temperature: %.2f %cC <input type = "button" onclick = "increase()" value = "+">  <input type = "button" onclick = "decrease()" value = "-"></p> 
		<p>Actual temperature: %.2f %cC</p>
		<p>Heating: %s </p>
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

inline String mainPage(float targetTemp,float actualTemp, Heating heating)
{
	char tmp[800];
	sprintf(tmp,MAIN_page,targetTemp,176, actualTemp,176, heatingToString(heating));
	return String(tmp);
}