// $Id$

Drupal.behaviors.file = function() {
  $('input[@type=file]').change(function() {
    if (this.value != '' && $('#edit-title').val() == '') {
      var title = this.value.match(/([^\/\\]+)$/)[1];
      $('#edit-title').val(title);
    }
  });

  $('span.file.with-menu span.label .highlight').unbind('click').click(function(event) {
    $(this).parent('.label').toggleClass('active');
    $(this).parent('.label').find('ul').toggle('fast');
  });

  $(document).click(function(event) {
    $('span.file.with-menu .label').each(function() {
      if ($(event.target).parents('.label')[0] != this) {
        $(this).removeClass('active');
        $(this).find('ul').hide();
      }
    });
  });
};

