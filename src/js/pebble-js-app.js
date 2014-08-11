var destination = 0;
var l1_text = 'Home';
var l1_coords = '32.853788193502346,-97.21060723260094';
var l1_type = 'fastest | car';
var l1_trans_mode = 'car';
var l2_text = 'Work';
var l2_coords = '32.891631,-96.979631';
var l2_type = 'fastest | car';
var l2_trans_mode = 'car';
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
var km_or_mi = 'mi';
var distance_conversion = 0;

var maxTriesForSendingAppMessage = 3;
var numTriesStart = 0;

function getAllLocations(){
	var returnData = {"l1_text":l1_text,
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
	sendAppMessage(returnData, numTriesStart);
}

function fetchHereToThere(latitude, longitude){
	var response;
	var app_id='***REMOVED***';
	var app_code='***REMOVED***';
	var req = new XMLHttpRequest();
	var dest_coords ='';
	switch (destination){
		case 0:
			dest_coords = l1_coords;
			break;
		case 1:
			dest_coords = l2_coords;
			break;
		case 2:
			dest_coords = l3_coords;
			break;
		case 3:
			dest_coords = l4_coords;
			break;
		case 4:
			dest_coords = l5_coords;
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
						
						if (km_or_mi == 'mi'){
							distance_conversion = m_to_mi;
						}else{
							distance_conversion = m_to_km;
						}
						
						var summary_distance = Math.round(100*(routeInfo.summary.distance / distance_conversion))/100;
						var summary_traffic_time = Math.round(100*(routeInfo.summary.trafficTime / 60))/100;
						var summary_base_time = Math.round(100*(routeInfo.summary.baseTime / 60))/100;
						var summary_travel_time = Math.round(100*(routeInfo.summary.travelTime / 60))/100;
					
						console.log(summary_distance + " | " + 
							summary_traffic_time + " | " +
							summary_base_time + " | " +
							summary_travel_time);
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

function sendAppMessage(message, numTries, transactionId){
	if (numTries < maxTriesForSendingAppMessage){
		numTries++;
		Pebble.sendAppMessage(
			message, function(){}, function(e){
				console.log('Failed sending AppMessage for transactionId: ' + e.data.transactionId + '. Error: ' + e.data.error.message);
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
	if (retVal == "undefined" || retVal == null){retVal = '';}
}

Pebble.addEventListener("ready", function(e) {
	console.log("Ready Event");
        
    //l1_text = getLocationValue("l1_text");
    //l1_coords = getLocationValue("l1_coords");
    //l1_type = getLocationValue("l1_type");
    //l1_trans_mode = getLocationValue("l1_trans_mode");
    
    //l2_text = getLocationValue("l2_text");
    //l2_coords = getLocationValue("l2_coords");
    //l2_type = getLocationValue("l2_type");
    //l2_trans_mode = getLocationValue("l2_trans_mode");
    
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
	
	Pebble.sendAppMessage({
		"l1_text":"Getting locations..."
	});
	
	if (pullType == 'getLocations'){
		console.log("getting location");
		getAllLocations();
	}else if (pullType == '0'){
		console.log("getting location 0 stats");
		destination = 0;
		window.navigator.geolocation.getCurrentPosition(locationSuccess, locationError, locationOptions);
	}else if (pullType == '1'){
		console.log("getting location 1 stats");
		destination = 1;
		window.navigator.geolocation.getCurrentPosition(locationSuccess, locationError, locationOptions);
	}else if (pullType == '2'){
		console.log("getting location 2 stats");
		destination = 2;
		window.navigator.geolocation.getCurrentPosition(locationSuccess, locationError, locationOptions);	
	}else if (pullType == '3'){
		console.log("getting location 3 stats");
		destination = 3;
		window.navigator.geolocation.getCurrentPosition(locationSuccess, locationError, locationOptions);
	}else if (pullType == '4'){
		console.log("getting location 4 stats");
		destination = 4;
		window.navigator.geolocation.getCurrentPosition(locationSuccess, locationError, locationOptions);
	}else if (pullType == '5'){
		console.log("getting location 5 stats");
		destination = 5;
		window.navigator.geolocation.getCurrentPosition(locationSuccess, locationError, locationOptions);
	}
});
