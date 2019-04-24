$(document).ready(function(){
    var $form = $('form');
    $form.submit(function(){
        $('#data').html('Measuring voltages...')
        $.get($(this).attr('action'), $(this).serialize(), function(response){
            console.log(response);
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
                    html += '<td '
                    switch(response.footer[i][1]) {
                        case 0:
                            html += "style='color:green'"
                            break;
                        case 1:
                            html += "style='color:#af9500'"
                            break;
                        case 2:
                            html += "style='color:red'"
                            break;
                        default:
                            break;
                    }
                    html += '>' + response.footer[i][0] + '</td>';
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
