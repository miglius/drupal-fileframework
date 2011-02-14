// $Id$

/**
 * Conditionally show or hide settings.
 */
Drupal.behaviors.file_convert_admin = function () {
  $('div.file-convert-performance-file-convert-restrictions-enabled-setting input.form-checkbox').click(function () {
    if (this.checked) {
      $('div.file-convert-performance-file-convert-processes-commandline-setting').show();
      $('div.file-convert-performance-file-convert-restrictions-setting').show();
    }
    else {
      $('div.file-convert-performance-file-convert-processes-commandline-setting').hide();
      $('div.file-convert-performance-file-convert-restrictions-setting').hide();
    }
  });
};

