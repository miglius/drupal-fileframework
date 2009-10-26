// $Id$

/**
 * Conditionally show or hide settings.
 */
Drupal.behaviors.file_browser_admin = function () {
  $('div.file-browser-vocabs-file-browser-vocabularies-all-setting input.form-radio').click(function () {
    if (this.value == '0') {
      $('div.file-browser-vocabs-file-browser-vocabularies-setting').show();
    }
    else {
      $('div.file-browser-vocabs-file-browser-vocabularies-setting').hide();
    }
  });
};

