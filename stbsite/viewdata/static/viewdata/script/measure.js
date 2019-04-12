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
            html += '<tfoot><tr>';
            for (var i = 0; i < response.footer.length; i++) {
                if (i == 0) {
                    html += '<td>' + 'Mean' + '</td>';
                } else {
                    html += '<td>' + response.footer[i] + '</td>';
                }
            }
            html += '</tr></tfoot>';
            $('#data').html(html);
      },'json');
      return false;
   });
});

// function submitForm(action) {
//     document.getElementById('board-select').action = action;
//     console.log(action)
//     document.getElementById('board-select').submit();
// };
