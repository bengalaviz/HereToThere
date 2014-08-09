var l1_text = '';
var l1_coords = '';

var maxTriesForSendingAppMessage = 3;
var numTriesStart = 0;

function getAllLocations(){
	var returnData = {"l1_text":l1_text,
		"l1_coords":l1_coords};
	sendAppMessage(returnData, numTriesStart);
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

Pebble.addEventListener("ready", function(e) {
        console.log("Ready Event");
        
        l1_text = localStorage.getItem("l1_text");
        if (l1_text == "undefined" || l1_text == null){l1_text = '';}
        
        l1_coords = localStorage.getItem("l1_coords");
        if (l1_coords == "undefined" || l1_coords == null){l1_coords = '';}
        
        
});

Pebble.addEventListener("appmessage", function(e){
	pullType = e.payload.location_id;
	
	Pebble.sendAppMessage({
		"location_text":"Getting locations..."
	});
	
	if (pullType == 'Locations'){
		getAllLocations();
	}
});
