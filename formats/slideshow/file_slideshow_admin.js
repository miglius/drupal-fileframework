// $Id$

/**
 * Conditionally show or hide settings.
 */
Drupal.behaviors.file_slideshow_admin = function () {
  $('div.file-slideshow-pdf-file-slideshow-pdf-setting input.form-checkbox').click(function () {
    if (this.checked) {
      $('div.file-slideshow-pdf-file-slideshow-pdf-data-setting').show();
    }
    else {
      $('div.file-slideshow-pdf-file-slideshow-pdf-data-setting').hide();
    }
  });
};

