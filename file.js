// $Id$

Drupal.behaviors.file = function() {
  $('input[@type=file]').change(function() {
    if (this.value != '' && $('#edit-title').val() == '') {
      var title = this.value.match(/([^\/\\]+)$/)[1];
      $('#edit-title').val(title);
    }
  });

  $('span.file.with-menu').unbind('click').click(function(event) {
    $(this).toggleClass('active');
    $(this).find('ul').toggle();
  });

  $(document).click(function(event) {
    $('span.file.with-menu').each(function() {
      if ($(event.target).parents('span.file')[0] != this) {
        $(this).removeClass('active');
        $(this).find('ul').hide();
      }
    });
  });
};

