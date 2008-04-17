// $Id$

Drupal.behaviors.file_attach = function() {
  var translate = Drupal.settings.file_attach;
  var max = translate.max;

  // For some reason after ahah calls on the same page the behaviours are 
  // bind to the events again. To avoid that have to do some checks.
  if ($('#attachments-list').length == 0) {
    
    // Remove additional file upload elements.
    for(var i=1; i<max; i++) {
      $('#edit-upload-'+i+'-wrapper').html('');
    }

    $('#attachments-new').after('<div id="attachments-list" style="border:1px solid black;padding:5px;background:#fff;font-size:x-small;"><strong>'+translate.files+' ('+translate.maximum+' '+max+'):</strong></div>');
  
    $('#edit-upload-0').change(function() {
      Drupal.fileAttachDoIt(this, max);
    });
  }
};	

Drupal.fileAttachDoIt = function(obj, m) {
  var n = Drupal.fileAttachFindNext(m);

  $(obj).hide();
  Drupal.fileAttachAddFile(m, n);
  
  var v = obj.value;
  if(v != '') {
    $("div#attachments-list").append('<div><input type="button" class="remove" value="Delete" /> '+v+'</div>').find("input").click(function() {
      $(this).parent().remove();
      if ($('#edit-upload--1').length == 1) {
        $('#edit-upload--1').remove();
        $(obj).remove();
        n = Drupal.fileAttachFindNext(m);
        Drupal.fileAttachAddFile(m, n);
      }
      return true;
    });
  }
};

Drupal.fileAttachFindNext = function(m) {
  var n = -1;
  for(var i=0; i<m; i++) {
    if ($('#edit-upload-'+i).length == 0 ) {
      n = i;
      break;
    }
  }
  return n;
};

Drupal.fileAttachAddFile = function(m, n) {
  $('#edit-upload-0-wrapper').append('<input type="file" name="files[upload_'+n+']" class="form-file" id="edit-upload-'+n+'" size="40"'+(n == -1 ? ' disabled=""' : '')+' />').find("input").change(function() {
    Drupal.fileAttachDoIt(this, m);
  });
};

