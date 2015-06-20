Pebble.addEventListener('showConfiguration', function() {
  Pebble.openURL('http://192.168.29.111/pebble.html');
});

Pebble.addEventListener('webviewclosed', function(f) {
  var options = JSON.parse(decodeURIComponent(f.response));
  console.log(options);
  var curPlaid = encodeURIComponent(options.curPlaid);
  var curPlaidColor = encodeURIComponent(options.curPlaidColor);
  var curHandColor = encodeURIComponent(options.curHandColor);
  //window.localStorage.setItem('curPlaid', curPlaid);
  var transactionId = Pebble.sendAppMessage( { 'CUR_PLAID': parseInt(curPlaid,10),'CUR_PLAID_COLOR': parseInt(curPlaidColor,10), 'CUR_HAND_COLOR': parseInt(curHandColor,10)  },
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