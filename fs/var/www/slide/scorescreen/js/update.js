var node = "soaringeagle.islidephoto.nl"
var slide_id = "1";
var update_interval = 30000;
function getAPIData() {
    var jsonstring = "https://" + node + "/api/get_media/?slide_id=" + slide_id
    $.getJSON(jsonstring, function(data) {
        updateslideScreen(data);
        setInterval(getAPIData(),30000);
    });
    
};

function updateslideScreen(data) {
    data.reverse();
    var slider = data[0];
        // document.getElementById("timeleft").innerHTML = Math.round((sliders[0].getElementsByTagName("duration")[0].childNodes[0].nodeValue / 1000) * 100) / 100 + "sec";
        // document.getElementById("speedleft").innerHTML = sliders[0].getElementsByTagName("speed")[0].childNodes[0].nodeValue + "km/h";
        // document.getElementById("scoreleft").innerHTML = sliders[0].getElementsByTagName("score")[0].childNodes[0].nodeValue + "pt";

        // document.getElementById("whiteleft").innerHTML = sliders[0].getElementsByTagName("white")[0].childNodes[0].nodeValue;
        // document.getElementById("greenleft").innerHTML = sliders[0].getElementsByTagName("green")[0].childNodes[0].nodeValue;
        // document.getElementById("blueleft").innerHTML = sliders[0].getElementsByTagName("blue")[0].childNodes[0].nodeValue;
        // document.getElementById("redleft").innerHTML = sliders[0].getElementsByTagName("red")[0].childNodes[0].nodeValue;
		if(typeof(slider) !== 'undefined')
		{
            
            document.getElementById("idphotoslide").src = "https://" + node + slider.photo_url;
		}
		else
		{
			document.getElementById("idphotoslide").src = "img/default.png";
		}
        
        // document.getElementById("highscore").innerHTML = sliders[2].getElementsByTagName("score")[0].childNodes[0].nodeValue + "pt";
        // document.getElementById("hightime").innerHTML =  Math.round((sliders[2].getElementsByTagName("duration")[0].childNodes[0].nodeValue / 1000) * 100) / 100 + "sec";
        // document.getElementById("highspeed").innerHTML = sliders[2].getElementsByTagName("speed")[0].childNodes[0].nodeValue + "km/h";
};