// $Id$

/**
 * Conditionally show or hide settings.
 */
Drupal.behaviors.file_image_admin = function () {
  $('div.file-image-exif-file-image-exif-setting input.form-checkbox').click(function () {
    if (this.checked) {
      $('div.file-image-exif-file-image-exif-data-setting').show();
      $('div.file-image-geo-file-image-geo-setting input.form-checkbox').attr('disabled', '');
    }
    else {
      $('div.file-image-exif-file-image-exif-data-setting').hide();
      $('div.file-image-geo-file-image-geo-setting input.form-checkbox').attr('disabled', 'disabled');
    }
  });
};

