$(document).ready(function(){
   var $form = $('form');
   $form.submit(function(){
      $('#data').html('')
      $.get($(this).attr('action'), $(this).serialize(), function(response){
            var html = '<thead><tr>';
            for (var i = 0; i < response.header.length; i++) {
                html += '<th>' + response.header[i] + '</th>';
            }
            html += '</tr></thead>';
            for (var i = 0; i < response.results.length; i++) {
                html += '<tr>';
                for (var j = 0; j < response.results[i].length; j++) {
                    html += '<td>' + response.results[i][j] + '</td>';
                }
                html += '</tr>';
            }
            $('#data').html(html);
      },'json');
      return false;
   });
});
