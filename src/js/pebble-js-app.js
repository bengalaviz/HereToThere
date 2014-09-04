var destination = 0;
var location_name = '';
var dest_coords ='';
var routing_type='';
var transport_mode='';
var l1_text = '';
var l1_coords = '';
var l1_type = 0;
var l1_trans_mode = 0;
var l2_text = '';
var l2_coords = '';
var l2_type = 0;
var l2_trans_mode = 0;
var l3_text = '';
var l3_coords = '';
var l3_type = 0;
var l3_trans_mode = 0;
var l4_text = '';
var l4_coords = '';
var l4_type = 0;
var l4_trans_mode = 0;
var l5_text = '';
var l5_coords = '';
var l5_type = 0;
var l5_trans_mode = 0;

var m_to_mi = 1609.344;
var m_to_km = 1000;
var km_or_mi = 0;
var distance_conversion = 0;
var distance_conversion_label = '';
var refresh_minutes = 0;

var maxTriesForSendingAppMessage = 3;
var numTriesStart = 0;

String.prototype.capitalize = function() {
    return this.replace(/(?:^|\s)\S/g, function(a) { return a.toUpperCase(); });
};

function getRoutingType(rt){
	if (typeof(rt) == 'string'){
		rt = parseInt(rt);
	}
	switch (rt){
		case 0:
			return 'fastest';
			break;
		case 1:
			return 'shortest';
			break;
		default:
			return 'fastest';
	}
}

function getTransportMode(tm){
	if (typeof(tm) == 'string'){
		tm = parseInt(tm);
	}
	switch (tm){
		case 0:
			return 'car';
			break;
		case 1:
			return 'pedestrian';
			break;
		case 2:
			return 'carHOV';
			break;
		case 3:
			return 'publicTransport';
			break;
		default:
			return 'car';
	}
}

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
	var app_id='';
	var app_code='';
	var req = new XMLHttpRequest();
	
	switch (destination){
		case 1:
			dest_coords = l1_coords;
			location_name = l1_text;
			routing_type= getRoutingType(l1_type);
			transport_mode=getTransportMode(l1_trans_mode);
			break;
		case 2:
			dest_coords = l2_coords;
			location_name = l2_text;
			routing_type=getRoutingType(l2_type);
			transport_mode=getTransportMode(l2_trans_mode);
			break;
		case 3:
			dest_coords = l3_coords;
			location_name = l3_text;
			routing_type=getRoutingType(l3_type);
			transport_mode=getTransportMode(l3_trans_mode);
			break;
		case 4:
			dest_coords = l4_coords;
			location_name = l4_text;
			routing_type=getRoutingType(l4_type);
			transport_mode=getTransportMode(l4_trans_mode);
			break;
		case 5:
			dest_coords = l5_coords;
			location_name = l5_text;
			routing_type=getRoutingType(l5_type);
			transport_mode=getTransportMode(l5_trans_mode);
			break;
		default:
			dest_coords = l1_coords;
			location_name = l1_text;
			routing_type=getRoutingType(l1_type);
			transport_mode=getTransportMode(l1_trans_mode);
	}
	
	if (routing_type == ''){routing_type='fastest';}
	if (transport_mode == ''){transport_mode='car';}
	
	var apicall = 'http://route.cit.api.here.com/routing/7.2/calculateroute.json?' + 
		'app_id=' + app_id + 
		'&app_code=' + app_code + 
		'&waypoint0=geo!' + latitude + ',' + longitude + 
		'&waypoint1=geo!' + dest_coords +
		'&mode=' + routing_type + ';' + transport_mode + ';traffic:enabled' + 
		'&representation=overview&routeattributes=summary';
	req.open('GET', apicall);
	req.onload = function(e){
		if (req.readyState == 4){
			if (req.status == 200){
				response = JSON.parse(req.responseText);
				if (response){
					var trafficResults = response.response.route;
					
					var routing_type;
					var transport_mode;
					var distance = 0;
					var base_time = 0;
					var travel_time = 0;
					var traffic_time = 0;
					var traffic_hours = 0;
					var traffic_minutes = 0;
					
					if (trafficResults != undefined){
						for (var i=0;i<trafficResults.length;i++){
							var routeInfo = trafficResults[i];
							location_name = "To " + location_name
							if (km_or_mi == 0){
								distance_conversion = m_to_mi;
								distance_conversion_label = "mi";
							}else{
								distance_conversion = m_to_km;
								distance_conversion_label = "km";
							}
							routing_type = routeInfo.mode.type;
							transport_mode = routeInfo.mode.transportModes[0];
							distance = Math.round(100*(routeInfo.summary.distance / distance_conversion))/100;
							base_time = Math.round(100*(routeInfo.summary.baseTime / 60))/100;
							travel_time = Math.round(100*(routeInfo.summary.travelTime / 60))/100;
							
							traffic_time = 0
							if (routeInfo.summary.trafficTime != undefined){
								traffic_time = Math.round(1*(routeInfo.summary.trafficTime / 60))/1;
							}else{
								traffic_time = travel_time;
							}
							
							traffic_hours = Math.floor(traffic_time/60);
							traffic_minutes = traffic_time % 60;
							traffic_minutes = Math.round(traffic_minutes);
							
							if (traffic_hours == 0){
								traffic_hours = "";
							}
						}
					}else{
						var error = response.subtype;
						location_name = error;
					}
					
					var returnData = {traveltime:true,
						refresh_minutes:refresh_minutes * 60000,
						location_name:location_name,
						traffic_time_minutes:traffic_minutes + "",
						traffic_time_hours:traffic_hours + "",
						location_distance:distance + "",
						distance_conversion:distance_conversion_label,
						routing_type:routing_type.capitalize(),
						transport_mode:transport_mode.capitalize()};
					numTriesStart = 0;
					sendAppMessage(returnData, numTriesStart);
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
	if (numTries < maxTriesForSendingAppMessage){
		numTries++;
		Pebble.sendAppMessage(
			message, function(){}, function(e){
				console.log('Failed sending AppMessage for transactionId: ' + e.data.transactionId + '. Error: ' + e.data.error.message);
				sendAppMessage(message, numTries);
			}
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
	console.log(errorMessage);
	var returnData = {traveltime:true,
		refresh_minutes:refresh_minutes * 60000,
		location_name:"Location Unavailable",
		traffic_time_minutes:"",
		traffic_time_hours:"",
		location_distance:"",
		distance_conversion:"",
		routing_type:routing_type.capitalize(),
		transport_mode:transport_mode.capitalize()};
		numTriesStart = 0;
	sendAppMessage(returnData, numTriesStart);
}

function failedReq(errorMessage){
	console.log(errorMessage);
	var returnData = {traveltime:true,
		refresh_minutes:refresh_minutes * 60000,
		location_name:"No Route Found",
		traffic_time_minutes:"",
		traffic_time_hours:"",
		location_distance:"",
		distance_conversion:"",
		routing_type:routing_type.capitalize(),
		transport_mode:transport_mode.capitalize()};
		numTriesStart = 0;
	sendAppMessage(returnData, numTriesStart);
}

var locationOptions = {"timeout":5000, "maximumAge": 60000 };

function getLocationValue(item, default_value){
	var retVal = localStorage.getItem(item);
	if (retVal == 'undefined'){
		retVal = default_value;
	}
	return retVal;
}

Pebble.addEventListener("ready", function() {
	console.log("Ready Event");
    
    km_or_mi = getLocationValue("km_or_mi",0);
    refresh_minutes = getLocationValue("rm");
    
    l1_text = getLocationValue("l1_text","");
    l1_coords = getLocationValue("l1_coords","");
    l1_type = getLocationValue("l1_type",0);
    l1_trans_mode = getLocationValue("l1_trans_mode",0);
    
    l2_text = getLocationValue("l2_text","");
    l2_coords = getLocationValue("l2_coords","");
    l2_type = getLocationValue("l2_type",0);
    l2_trans_mode = getLocationValue("l2_trans_mode",0);
    
    l3_text = getLocationValue("l3_text","");
    l3_coords = getLocationValue("l3_coords","");
    l3_type = getLocationValue("l3_type",0);
    l3_trans_mode = getLocationValue("l3_trans_mode",0);
    
    l4_text = getLocationValue("l4_text","");
    l4_coords = getLocationValue("l4_coords","");
    l4_type = getLocationValue("l4_type",0);
    l4_trans_mode = getLocationValue("l4_trans_mode",0);
    
    l5_text = getLocationValue("l5_text","");
    l5_coords = getLocationValue("l5_coords","");
    l5_type = getLocationValue("l5_type",0);
    l5_trans_mode = getLocationValue("l5_trans_mode",0);
});

Pebble.addEventListener("appmessage", function(e){
	pullType = e.payload.query_type;
	
	if (pullType == 0){
		getAllLocations();
	}else if (pullType == 1){
		destination = 1;
		window.navigator.geolocation.getCurrentPosition(locationSuccess, locationError, locationOptions);
	}else if (pullType == 2){
		destination = 2;
		window.navigator.geolocation.getCurrentPosition(locationSuccess, locationError, locationOptions);	
	}else if (pullType == 3){
		destination = 3;
		window.navigator.geolocation.getCurrentPosition(locationSuccess, locationError, locationOptions);
	}else if (pullType == 4){
		destination = 4;
		window.navigator.geolocation.getCurrentPosition(locationSuccess, locationError, locationOptions);
	}else if (pullType == 5){
		destination = 5;
		window.navigator.geolocation.getCurrentPosition(locationSuccess, locationError, locationOptions);
	}
});

Pebble.addEventListener("showConfiguration", function(e){
	var settingsURI = "du=" + encodeURIComponent(km_or_mi) +
		"&rm=" + encodeURIComponent(refresh_minutes);
	
	if (l1_text != ""){
		settingsURI+="&l1=" + encodeURIComponent(l1_text + "|" + l1_coords + "|" + l1_type + "|" + l1_trans_mode);
	}
	
	if (l2_text != ""){
		settingsURI+="&l2=" + encodeURIComponent(l2_text + "|" + l2_coords + "|" + l2_type + "|" + l2_trans_mode);
	}
	
	if (l3_text != ""){
		settingsURI+="&l3=" + encodeURIComponent(l3_text + "|" + l3_coords + "|" + l3_type + "|" + l3_trans_mode);
	}
	
	if (l4_text != ""){
		settingsURI+="&l4=" + encodeURIComponent(l4_text + "|" + l4_coords + "|" + l4_type + "|" + l4_trans_mode);
	}
	
	if (l5_text != ""){
		settingsURI+="&l5=" + encodeURIComponent(l5_text + "|" + l5_coords + "|" + l5_type + "|" + l5_trans_mode);
	}
	
	Pebble.openURL("http://bengalaviz.com/pebble/pebble-heretothere.php?v=0.8&" + settingsURI);
});

Pebble.addEventListener("webviewclosed", function(e){
	if (e.response != null && e.response != ''){
		var config = JSON.parse(e.response);
		km_or_mi = config["du"];
		localStorage.setItem("km_or_mi",km_or_mi);
		refresh_minutes = config["rm"];
		localStorage.setItem("rm", refresh_minutes);
		
		l1_text = config["l1_name"];
		localStorage.setItem("l1_text",l1_text);
		l1_coords = config["l1_coords"];
		localStorage.setItem("l1_coords",l1_coords);
		l1_type = config["l1_rt"];
		localStorage.setItem("l1_type", l1_type);
		l1_trans_mode = config["l1_tm"];
		localStorage.setItem("l1_trans_mode", l1_trans_mode);
		
		l2_text = config["l2_name"];
		localStorage.setItem("l2_text",l2_text);
		l2_coords = config["l2_coords"];
		localStorage.setItem("l2_coords",l2_coords);
		l2_type = config["l2_rt"];
		localStorage.setItem("l2_type", l2_type);
		l2_trans_mode = config["l2_tm"];
		localStorage.setItem("l2_trans_mode", l2_trans_mode);
		
		l3_text = config["l3_name"];
		localStorage.setItem("l3_text",l3_text);
		l3_coords = config["l3_coords"];
		localStorage.setItem("l3_coords",l3_coords);
		l3_type = config["l3_rt"];
		localStorage.setItem("l3_type", l3_type);
		l3_trans_mode = config["l3_tm"];
		localStorage.setItem("l3_trans_mode", l3_trans_mode);
		
		l4_text = config["l4_name"];
		localStorage.setItem("l4_text",l4_text);
		l4_coords = config["l4_coords"];
		localStorage.setItem("l4_coords",l4_coords);
		l4_type = config["l4_rt"];
		localStorage.setItem("l4_type", l4_type);
		l4_trans_mode = config["l4_tm"];
		localStorage.setItem("l4_trans_mode", l4_trans_mode);
		
		l5_text = config["l5_name"];
		localStorage.setItem("l5_text",l5_text);
		l5_coords = config["l5_coords"];
		localStorage.setItem("l5_coords",l5_coords);
		l5_type = config["l5_rt"];
		localStorage.setItem("l5_type", l5_type);
		l5_trans_mode = config["l5_tm"];
		localStorage.setItem("l5_trans_mode", l5_trans_mode);
	}
});
