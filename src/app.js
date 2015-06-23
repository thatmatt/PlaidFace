Pebble.addEventListener('showConfiguration', function() {
  var options = window.localStorage.getItem('options');
  Pebble.openURL('http://thatmatt.github.io/pebble.html?options='+options);
});

Pebble.addEventListener('webviewclosed', function(f) {
  var options = JSON.parse(decodeURIComponent(f.response));
  console.log(options);
  var curPlaid = encodeURIComponent(options.curPlaid);
  var curPlaidColor = encodeURIComponent(options.curPlaidColor);
  var curHandColor = encodeURIComponent(options.curHandColor);
  var watchMode = encodeURIComponent(options.watchMode);

  window.localStorage.setItem('options', f.response);
  var transactionId = Pebble.sendAppMessage( 
    { 
      'CUR_PLAID': parseInt(curPlaid,10),
      'CUR_PLAID_COLOR': parseInt(curPlaidColor,10),
      'CUR_HAND_COLOR': parseInt(curHandColor,10),
      'WATCH_MODE': parseInt(watchMode,10),
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