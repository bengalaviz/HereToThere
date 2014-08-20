var destination = 0;
var location_name = '';

var l1_text = '';
var l1_coords = '';
var l1_type = '';
var l1_trans_mode = '';
var l2_text = '';
var l2_coords = '';
var l2_type = '';
var l2_trans_mode = '';
var l3_text = '';
var l3_coords = '';
var l3_type = '';
var l3_trans_mode = '';
var l4_text = '';
var l4_coords = '';
var l4_type = '';
var l4_trans_mode = '';
var l5_text = '';
var l5_coords = '';
var l5_type = '';
var l5_trans_mode = '';

var m_to_mi = 1609.344;
var m_to_km = 1000;
var km_or_mi = 0;
var distance_conversion = 0;
var distance_conversion_label = '';

var maxTriesForSendingAppMessage = 3;
var numTriesStart = 0;

String.prototype.capitalize = function() {
    return this.replace(/(?:^|\s)\S/g, function(a) { return a.toUpperCase(); });
};

function getAllLocations(){
	var returnData = { "locations":true,
		"l1_text":l1_text,
		"l1_coords":l1_coords,
		"l1_type":l1_type,
		"l1_trans_mode":l1_trans_mode,
		"l2_text":l2_text,
		"l2_coords":l2_coords,
		"l2_type":l2_type,
		"l2_trans_mode":l2_trans_mode,
		"l3_text":l3_text,
		"l3_coords":l3_coords,
		"l3_type":l3_type,
		"l3_trans_mode":l3_trans_mode,
		"l4_text":l4_text,
		"l4_coords":l4_coords,
		"l4_type":l4_type,
		"l4_trans_mode":l4_trans_mode,
		"l5_text":l5_text,
		"l5_coords":l5_coords,
		"l5_type":l5_type,
		"l5_trans_mode":l5_trans_mode};
	numTriesStart = 0;
	sendAppMessage(returnData, numTriesStart);
}

function fetchHereToThere(latitude, longitude){
	var response;
	var app_id='***REMOVED***';
	var app_code='***REMOVED***';
	var req = new XMLHttpRequest();
	var dest_coords ='';
	switch (destination){
		case 1:
			dest_coords = l1_coords;
			location_name = l1_text;
			break;
		case 2:
			dest_coords = l2_coords;
			location_name = l2_text;
			break;
		case 3:
			dest_coords = l3_coords;
			location_name = l3_text;
			break;
		case 4:
			dest_coords = l4_coords;
			location_name = l4_text;
			break;
		case 5:
			dest_coords = l5_coords;
			location_name = l5_text;
			break;
		default:
			dest_coords = l1_coords;
	}
	var apicall = 'http://route.cit.api.here.com/routing/7.2/calculateroute.json?' + 
		'app_id=' + app_id + 
		'&app_code=' + app_code + 
		'&waypoint0=geo!' + latitude + ',' + longitude + 
		'&waypoint1=geo!' + dest_coords +
		'&mode=fastest;car;traffic:enabled' + 
		'&representation=overview&routeattributes=summary'
	console.log(apicall);
	req.open('GET', apicall);
	req.onload = function(e){
		if (req.readyState == 4){
			if (req.status == 200){
				response = JSON.parse(req.responseText);
				if (response){
					var trafficResults = response.response.route;
					
					for (var i=0;i<trafficResults.length;i++){
						var routeInfo = trafficResults[i];
						
						if (km_or_mi == 0){
							distance_conversion = m_to_mi;
							distance_conversion_label = "mi";
						}else{
							distance_conversion = m_to_km;
							distance_conversion_label = "km";
						}
						var routing_type = routeInfo.mode.type;
						var transport_mode = routeInfo.mode.transportModes[0];
						var distance = Math.round(100*(routeInfo.summary.distance / distance_conversion))/100;
						var traffic_time = Math.round(1*(routeInfo.summary.trafficTime / 60))/1;
						var base_time = Math.round(100*(routeInfo.summary.baseTime / 60))/100;
						var travel_time = Math.round(100*(routeInfo.summary.travelTime / 60))/100;
						
						var traffic_hours = Math.floor(traffic_time/60);
						var traffic_minutes = traffic_time % 60;
					
						console.log(distance + " | " + 
							traffic_hours + " | " +
							traffic_minutes + " | " +
							base_time + " | " +
							travel_time);
						
						if (traffic_hours == 0){
							traffic_hours = "";
						}
						var returnData = {traveltime:true,
							location_name:"To " + location_name + " w/ Traffic",
							traffic_time_minutes:traffic_minutes + "",
							traffic_time_hours:traffic_hours + "",
							location_distance:distance + "",
							distance_conversion:distance_conversion_label,
							routing_type:routing_type.capitalize(),
							transport_mode:transport_mode.capitalize()};
						console.log(JSON.stringify(returnData));
						numTriesStart = 0;
						sendAppMessage(returnData, numTriesStart);
					}
				}
			}else{
				failedReq("Status Error : " + req.status);
			}
		}
	}
	req.ontimeout = function(){
		failedReq('Request timed out');
	}
	req.onerror = function(){
		failedReq('Failed to connect');
	}
	req.send(null);
}

function sendAppMessage(message, numTries){
	console.log('sendAppMessage. Tries : ' + numTries + ' of ' + maxTriesForSendingAppMessage);
	if (numTries < maxTriesForSendingAppMessage){
		numTries++;
		Pebble.sendAppMessage(
			message, function(){}, function(e){
				console.log('Failed sending AppMessage for transactionId: ' + e.data.transactionId + '. Error: ' + e.data.error.message);
				sendAppMessage(message, numTries);
			}
		//Pebble.sendAppMessage(message);
		);
	}else{
		console.log('Failed sending AppMessage for transactionId: ' + transactionId + '. Bail. ' + JSON.stringify(message));
	}
}

function locationSuccess(pos){
	var coordinates = pos.coords;
	fetchHereToThere(coordinates.latitude, coordinates.longitude);
}

function locationError(err){
	Pebble.sendAppMessage({
		"l1_text":"Location Unavailable",
		"l1_coords":"N/A",
		"l1_type":"",
		"l1_trans_mode":""
	});
}

function failedReq(errorMessage){
	Pebble.sendAppMessage({
		"l1_text":errorMessage,
		"l1_coords":"N/A",
		"l1_type":"",
		"l1_trans_mode":""
	});
}

var locationOptions = {"timeout":5000, "maximumAge": 60000 };

function getLocationValue(item){
	var retVal = localStorage.getItem(item);
	if (retVal == undefined){retVal = '';}
	return retVal;
}

Pebble.addEventListener("ready", function() {
	console.log("Ready Event");
        
    l1_text = getLocationValue("l1_text");
    l1_coords = getLocationValue("l1_coords");
    l1_type = getLocationValue("l1_type");
    l1_trans_mode = getLocationValue("l1_trans_mode");
    
    l2_text = getLocationValue("l2_text");
    l2_coords = getLocationValue("l2_coords");
    l2_type = getLocationValue("l2_type");
    l2_trans_mode = getLocationValue("l2_trans_mode");
    
    l3_text = getLocationValue("l3_text");
    l3_coords = getLocationValue("l3_coords");
    l3_type = getLocationValue("l3_type");
    l3_trans_mode = getLocationValue("l3_trans_mode");
    
    l4_text = getLocationValue("l4_text");
    l4_coords = getLocationValue("l4_storage");
    l4_type = getLocationValue("l4_type");
    l4_trans_mode = getLocationValue("l4_trans_mode");
    
    l5_text = getLocationValue("l5_text");
    l5_coords = getLocationValue("l5_coords");
    l5_type = getLocationValue("l5_type");
    l5_trans_mode = getLocationValue("l5_trans_mode");
});

Pebble.addEventListener("appmessage", function(e){
	pullType = e.payload.query_type;
	
	console.log("Pull Type : " + pullType);
	
	if (pullType == 0){
		console.log("getting location");
		getAllLocations();
	}else if (pullType == 1){
		console.log("getting location 1 stats");
		destination = 1;
		window.navigator.geolocation.getCurrentPosition(locationSuccess, locationError, locationOptions);
	}else if (pullType == 2){
		console.log("getting location 2 stats");
		destination = 2;
		window.navigator.geolocation.getCurrentPosition(locationSuccess, locationError, locationOptions);	
	}else if (pullType == 3){
		console.log("getting location 3 stats");
		destination = 3;
		window.navigator.geolocation.getCurrentPosition(locationSuccess, locationError, locationOptions);
	}else if (pullType == 4){
		console.log("getting location 4 stats");
		destination = 4;
		window.navigator.geolocation.getCurrentPosition(locationSuccess, locationError, locationOptions);
	}else if (pullType == 5){
		console.log("getting location 5 stats");
		destination = 5;
		window.navigator.geolocation.getCurrentPosition(locationSuccess, locationError, locationOptions);
	}
});

Pebble.addEventListener("showConfiguration", function(e){
	var settingsURI = "du=" + km_or_mi +
		"&l1=" + encodeURIComponent(l1_text + "|" + l1_coords) +
		"&l2=" + encodeURIComponent(l2_text + "|" + l2_coords) +
		"&l3=" + encodeURIComponent(l3_text + "|" + l3_coords) +
		"&l4=" + encodeURIComponent(l4_text + "|" + l4_coords) +
		"&l5=" + encodeURIComponent(l5_text + "|" + l5_coords);
	console.log(settingsURI);
	Pebble.openURL("http://bengalaviz.com/pebble/pebble-heretothere.php?" + settingsURI);
});

Pebble.addEventListener("webviewclosed", function(e){
	if (e.response != null){
		var config = JSON.parse(e.response);
		
		km_or_mi = config["du"];
		localStorage.setItem("km_or_mi",km_or_mi);
		l1_text = config["l1_name"];
		localStorage.setItem("l1_text",l1_text);
		l1_coords = config["l1_coords"];
		localStorage.setItem("l1_coords",l1_coords);
		l2_text = config["l2_name"];
		localStorage.setItem("l2_text",l2_text);
		l2_coords = config["l2_coords"];
		localStorage.setItem("l2_coords",l2_coords);
		l3_text = config["l3_name"];
		localStorage.setItem("l3_text",l3_text);
		l3_coords = config["l3_coords"];
		localStorage.setItem("l3_coords",l3_coords);
		l4_text = config["l4_name"];
		localStorage.setItem("l4_text",l4_text);
		l4_coords = config["l4_coords"];
		localStorage.setItem("l4_coords",l4_coords);
		l5_text = config["l5_name"];
		localStorage.setItem("l5_text",l5_text);
		l5_coords = config["l5_coords"];
		localStorage.setItem("l5_coords",l5_coords);
	}
});