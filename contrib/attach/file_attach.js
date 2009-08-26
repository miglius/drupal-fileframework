// $Id$

Drupal.behaviors.file_attach = function(context) {
  var translate = Drupal.settings.file_attach;
  // Attach existing file
  $('a.file-attach-select').click(function() {
    Drupal.file_attachPopup(this.href, translate.popupTitle);
    return false;
  });
  $('#edit-attach-browse').click(function() {
    Drupal.file_attachPopup($('#edit-attach-browse-url').val(), translate.popupTitle);
    return false;
  });
};	

Drupal.file_attachPopup = function(url, title) {
  // Thickbox.js is pretty buggy; removing all its DIV elements before
  // attempting to show a thickbox overlay will ensure that the overlay
  // doesn't duplicate after the user has cancelled an earlier overlay.
  $("#TB_load").remove();
  $("#TB_overlay").remove();
  $("#TB_window").remove();
  tb_show(title || 'Add files', url, false);
};

