Pebble.addEventListener('showConfiguration', function() {
  var options = window.localStorage.getItem('options7');
  console.log(options);
  console.log("Entering settings for PlaidFace");
  Pebble.openURL('http://thatmatt.github.io/pebble2.html?options='+encodeURIComponent(options));
  
  Pebble.getTimelineToken(
  function (token) {
    console.log('My timeline token is ' + token);
  },
  function (error) { 
    console.log('Error getting timeline token: ' + error);
  }
);
});

Pebble.addEventListener('webviewclosed', function(f) {
  var options = JSON.parse(decodeURIComponent(f.response));
  var curPlaid = encodeURIComponent(options.curPlaid);
  var curPlaidColor = encodeURIComponent(options.curPlaidColor);
  var curHandColor = encodeURIComponent(options.curHandColor);
  var watchMode = encodeURIComponent(options.watchMode);
  var calDate = encodeURIComponent(options.calDate);
  var blDetect = encodeURIComponent(options.blDetect);

  window.localStorage.setItem('options7', f.response);
  var transactionId = Pebble.sendAppMessage( 
    { 
      'CUR_PLAID': parseInt(curPlaid,10),
      'CUR_PLAID_COLOR': parseInt(curPlaidColor,10),
      'CUR_HAND_COLOR': parseInt(curHandColor,10),
      'WATCH_MODE': parseInt(watchMode,10),
      'CAL_DATE': parseInt(calDate,10),
      'BL_DETECT': parseInt(blDetect,10),
    },
  function(e) {
    console.log('Successfully delivered message with transactionId='
      + e.data.transactionId);
  },
  function(e) {
    console.log('Unable to deliver message with transactionId='
      + e.data.transactionId
      + ' Error is: ' + e.data.error);
  }
);
});