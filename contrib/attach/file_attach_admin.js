// $Id$

/**
 * Conditionally show or hide settings.
 */
Drupal.behaviors.file_attach_admin = function () {
  $('div.file-attach-attachments-file-attach-reuse-setting input.form-checkbox').click(function () {
    if (this.checked) {
      $('div.file-attach-attachments-file-attach-reuse-nodes-setting').show();
    }
    else {
      $('div.file-attach-attachments-file-attach-reuse-nodes-setting').hide();
    }
  });
  $('div.file-attach-vocabs-file-attach-vocabularies-all-setting input.form-radio').click(function () {
    if (this.value == '0') {
      $('div.file-attach-vocabs-file-attach-vocabularies-setting').show();
    }
    else {
      $('div.file-attach-vocabs-file-attach-vocabularies-setting').hide();
    }
  });
};

