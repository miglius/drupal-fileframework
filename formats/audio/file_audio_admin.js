// $Id$

/**
 * Conditionally show or hide settings.
 */
Drupal.behaviors.file_audio_admin = function () {
  $('div.file-audio-getid3-file-audio-getid3-setting input.form-checkbox').click(function () {
    if (this.checked) {
      $('div.file-audio-getid3-file-audio-getid3-data-setting').show();
    }
    else {
      $('div.file-audio-getid3-file-audio-getid3-data-setting').hide();
    }
  });
};

