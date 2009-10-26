// $Id$

/**
 * Conditionally show or hide settings.
 */
Drupal.behaviors.file_video_admin = function () {
  $('div.file-video-getid3-file-video-getid3-setting input.form-checkbox').click(function () {
    if (this.checked) {
      $('div.file-video-getid3-file-video-getid3-data-setting').show();
    }
    else {
      $('div.file-video-getid3-file-video-getid3-data-setting').hide();
    }
  });
};

