var socket =  io() ;

function drive(){
  var delay = 1000;

  var drive = 'Drive Mode Engaged';
  socket.emit('drive', drive);
  console.log('drive mode');

  $(document).keypress(function(e) {

    if(e.which == 97) { // Left
      $("#left").css('color','red');
      $("#up").css('color','black');
      $("#right").css('color','black');
      $("#down").css('color','black');
      console.log('left');
      var left = 'left';
      socket.emit('left', left);

  }

    if(e.which == 100) {                //Right
      $("#left").css('color','black');
      $("#up").css('color','black');
      $("#right").css('color','red');
      $("#down").css('color','black');
       console.log('right');
       var right = 'right';
       socket.emit('right', right);

      }
    if(e.which == 119) {                //Up
      $("#left").css('color','black');
      $("#up").css('color','red');
      $("#right").css('color','black');
      $("#down").css('color','black');
        console.log('up');
        var up = 'up';
        socket.emit('up', up);

       }
    if(e.which == 115) {                //Down
      $("#left").css('color','black');
      $("#up").css('color','black');
      $("#right").css('color','black');
      $("#down").css('color','red');
         console.log('down');
         var down = 'down';
         socket.emit('down', down);

        }

   });
  $("#left").css('color','black');
}

function auto(){
  var auto = 'auto';
  socket.emit('auto', auto);
  console.log('Auto mode engaged');
}
