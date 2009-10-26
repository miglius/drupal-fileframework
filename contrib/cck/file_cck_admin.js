// $Id$

/**
 * Conditionally show or hide settings.
 */
Drupal.behaviors.file_cck_admin = function () {
  $('div.file-cck-vocabs-file-cck-vocabularies-all-setting input.form-radio').click(function () {
    if (this.value == '0') {
      $('div.file-cck-vocabs-file-cck-vocabularies-setting').show();
    }
    else {
      $('div.file-cck-vocabs-file-cck-vocabularies-setting').hide();
    }
  });
};

