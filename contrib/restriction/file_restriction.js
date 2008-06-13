// $Id$

/**
 * Attach handlers to evaluate if the file extension is supported by the web server.
 */
Drupal.behaviors.file_extension = function(context) {
  var translate = Drupal.settings.file_restriction;
  $("input[@type=file]", context).each(function() {

    $(this).parent().after('<div class="file-extension error" style="display: block;">'+ translate.description +'<br />'+ translate.extension_allowed +'</div>');
    $("div.file-extension").hide();

    $(this).change(function() {

      var file = $(this).val();
      var ext = '';
      if(file.length > 0) {
        var dot = file.lastIndexOf(".");
	if(dot != -1) {
	  ext = file.substr(dot+1,file.length);
	}
      }
      var extensions = translate.extension_allowed.split(" ");
      var found = 0;
      if(ext.length > 0) {
        for(i=0;i<extensions.length;i++) {
	  if(extensions[i] == ext.toLowerCase()) { found = 1; }
        }
      }
      if(found == 1) {
        $("div.file-extension").hide();
      }
      else {
        $("div.file-extension").show();
      }
    }); 
  });
};

