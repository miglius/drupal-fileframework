// $Id$

/**
 * Conditionally show or hide settings.
 */
Drupal.behaviors.file_admin = function () {
  $('div.file-maintenance-file-cron-setting input.form-radio').click(function () {
    if (this.value == '1') {
      $('div.file-maintenance-file-cron-limit-size-setting').show();
      $('div.file-maintenance-file-cron-limit-total-setting').show();
    }
    else {
      $('div.file-maintenance-file-cron-limit-size-setting').hide();
      $('div.file-maintenance-file-cron-limit-total-setting').hide();
    }
  });
  $('div.file-autodetection-file-mime-autodetection-setting input.form-radio').click(function () {
    if (this.value == '2') {
      $('div.file-autodetection-file-mime-autodetection-conditions-setting').show();
    }
    else {
      $('div.file-autodetection-file-mime-autodetection-conditions-setting').hide();
    }
  });
};

