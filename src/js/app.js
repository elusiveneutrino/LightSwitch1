var IPAddress = '192.168.192.60';
// var KEY_POWER_SET = 0;
// var KEY_POWER_STATUS = 1;


console.log('#1');

function light_switch(plugnum,onoff) {
  var req = new XMLHttpRequest();
  var url = 'http://' + IPAddress + '/sensors/' + plugnum;
  var data = 'output=' + onoff;
  req.open('POST', url, false);
  req.setRequestHeader('Content-Type','application/x-www-form-urlencoded');
  req.setRequestHeader('Content-Length',data.length);
  req.send(data);
  return req.responseText;
}
console.log('#2');

function get_light_status(plugnum) {
	var req = new XMLHttpRequest();
	var url = 'http://' + IPAddress + '/sensors/' + plugnum + '/output';
	req.open('GET', url, false);
	req.send();
	var rv = JSON.parse(req.responseText);
	if ( rv.status == "success" ) {
		console.log('SUCCESS: status = ' + rv.sensors[0].output);
		return rv.sensors[0].output; 
	} else {
		console.log('ERROR WITH RESPONSE: ' + req.responseText);
		return -1;
	}
}
console.log('#3');

Pebble.addEventListener('ready', function(e) {
  console.log('ASDFASDFASDF PebbleKit JS Ready!');

	var light_status = get_light_status(1);
	if (light_status == -1) {
		console.log('RCVD ERROR FROM get_light_status()');
	} else {

  // Construct a dictionary
  		var dict = {
    			'KEY_POWER_STATUS':light_status
  		};

  // Send a string to Pebble
  		Pebble.sendAppMessage(dict, function(e) {
    			console.log('Send successful.');
  			}, function(e) {
    				console.log('Send failed!');
  			});
	}
});
console.log('#4');

Pebble.addEventListener('appmessage', function(e) {
	console.log('Message for you, sire!');
	console.log('MSG: ' + JSON.stringify(e.payload));
	var msg = JSON.parse(JSON.stringify(e.payload));

	var rv = '';
	
	if ( msg.KEY_POWER_SET == 1) {
		console.log('TURN LIGHT ON, BRUH!');
		rv = light_switch(1,1);	
	} else if (msg.KEY_POWER_SET === 0) {
		console.log('CUT THAT SHIT OFF!');
		rv = light_switch(1,0);	
	} else if (msg.KEY_STATUS_REQUEST == 1) {
		var rv2 = get_light_status(1);
		if (rv2 == -1) {
			console.log('ERROR GETTING STATUS');
		} else {
			var dict = {
				'KEY_POWER_STATUS' : rv2
			};
			Pebble.sendAppMessage(dict, function(e) {
				console.log('Sending status succeeded');
			}, function(e) {
				console.log('Sending status failed');
			});
		}
		
	} else {
		console.log('WTF: ' + JSON.stringify(e.payload));
	}
	console.log('RV: ' + rv);

});
console.log('#5');

