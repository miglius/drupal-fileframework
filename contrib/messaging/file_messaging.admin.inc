<?php
// $Id$

/**
 * @file
 * Module admin page callbacks.
 */

//////////////////////////////////////////////////////////////////////////////
// File messaging settings

/**
 * Implements the settings page.
 *
 * @return
 *   The form structure.
 */
function file_messaging_admin_settings() {
  $form = array();

  $form['messaging'] = array('#type' => 'fieldset', '#title' => t('Messaging settings'), '#collapsible' => TRUE, '#collapsed' => FALSE);
  $form['messaging']['file_messaging_max_size'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Max size of the attachments'),
    '#default_value' => FILE_MESSAGING_MAX_SIZE,
    '#size'          => 10,
    '#maxlength'     => 255,
    '#description'   => t('Total maximum size of all files attached to the notification message in MB. If the total size exeed this value no files will be attached. Zero means that all files are attached.'),
  );

  $form['submit'] = array(
    '#type'  => 'submit',
    '#value' => t('Save configuration'),
  );
  $form['reset'] = array(
    '#type'  => 'submit',
    '#value' => t('Reset to defaults'),
  );

  return $form;
}

/**
 * Validate hook for the settings form.
 */
function file_messaging_admin_settings_validate($form, &$form_state) {
  $op = $form_state['clicked_button']['#value'];
  $values = $form_state['values'];
  switch ($op) {
    case t('Save configuration'):
      if (!is_numeric($values['file_messaging_max_size']) || $values['file_messaging_max_size'] < 0) {
        form_set_error('file_messaging_max_size', t('A max size should be a positive number.'));
      }
      break;
  }
}

/**
 * Submit hook for the settings form.
 */
function file_messaging_admin_settings_submit($form, &$form_state) {
  $op = $form_state['clicked_button']['#value'];
  $values = $form_state['values'];
  switch ($op) {
    case t('Save configuration'):
      variable_set('file_messaging_max_size', $values['file_messaging_max_size']);
      drupal_set_message(t('The configuration options have been saved.'));
      break;
    case t('Reset to defaults'):
      variable_del('file_messaging_max_size');
      drupal_set_message(t('The configuration options have been reset to their default values.'));
      break;
  }
}

