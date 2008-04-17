<?php
// $Id$
/**
 * @file
 * Module admin page callbacks.
 */

//////////////////////////////////////////////////////////////////////////////
// File convert settings

/**
 * Implements the settings page.
 *
 * @return
 *   The form structure.
 */
function file_convert_admin_settings() {
  $form = array();
  $form['settings'] = array('#type' => 'fieldset', '#title' => t('File Conversion settings'), '#collapsible' => TRUE, '#collapsed' => FALSE);
  $form['settings']['file_convert_limit_size'] = array(
    '#type'          => 'textfield',
    '#title'         => t('File size limit for conversion on upload'),
    '#default_value' => FILE_CONVERT_LIMIT_SIZE,
    '#size'          => 10,
    '#description'   => t('The size is in MB. If uploaded file is bigger than this value, then the conversion will be performed on cron run instead. This should allow avoiding page timeouts on the file uploads. Zero means that all files will be converted on the upload.'),
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
function file_convert_admin_settings_validate($form, &$form_state) {
  $op = $form_state['clicked_button']['#value'];
  switch ($op) {
    case "Save configuration":
      if (!ctype_digit($form_state['values']['file_convert_limit_size'])) {
        form_set_error('file_convert_limit_size', t('A file size should be an integer value.'));
      }
      break;
  }
}

/**
 * Submit hook for the settings form.
 */
function file_convert_admin_settings_submit($form, &$form_state) {
  $op = $form_state['clicked_button']['#value'];
  switch ($op) {
    case "Save configuration":
      variable_set('file_convert_limit_size', $form_state['values']['file_convert_limit_size']);
      drupal_set_message(t('The configuration options have been saved.'));
      break;
    case "Reset to defaults":
      variable_del('file_convert_limit_size');
      drupal_set_message(t('The configuration options have been reset to their default values.'));
      break;
  }
}

/**
 * Implements the converter settings page.
 *
 * @return
 *   The form structure.
 */
function file_convert_admin_converter_settings() {
  $form = array();
  // MIME type converters
  $form['convert']['description'] = array(
    '#value' => t('Command line or php functions as utilities to convert between MIME types and create file previews of other MIME type. Empty field means that no conversion will take place. \'||\' separates utilities, which will be executed in sequence. Shell commands or php functios should be enclosed in {} brackets. The system will check if those functions are available or commands are executable. If there is a string and digits separated by the colon in the brackets like {localhost:8100} the system will check if the port on the host is opened. Other files which should be readable should be enclosed in double {{}} brackets. Brackets {}[] in command line can be escaped with \{\}\[\]. \'No\' in the "Installed" column means that not all commands and functions are found or network connection cannot be opened and the conversion will not take place.'),
  );
  $rows = array();
  $mime_converters = file_get_mime_converters();
  ksort($mime_converters);
  foreach ($mime_converters as $from => $targets) {
    ksort($targets);
    foreach ($targets as $to => $converter) {
      $form['convert']['convert_'. $from][$to]['file_convert_'. preg_replace('/\./', '\\', $from) .'_'. preg_replace('/\./', '\\', $to)] = array(
        '#type' => 'textfield',
        '#default_value' => $converter['pipeline'],
        '#size' => 40,
        '#maxlength' => 1024,
      );
      $form['convert']['convert_'. $from][$to]['#check'] = $converter['installed'] ? t('Yes') : t('No');
      $form['convert']['convert_'. $from][$to]['#module'] = $converter['module'];
    }
  }

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
function file_convert_admin_converter_settings_validate($form, &$form_state) {
  $op = $form_state['clicked_button']['#value'];
  switch ($op) {
    case "Save configuration":
      break;
  }
}

/**
 * Submit hook for the settings form.
 */
function file_convert_admin_converter_settings_submit($form, &$form_state) {
  $op = $form_state['clicked_button']['#value'];
  switch ($op) {
    case "Save configuration":
      $pipelines = array();
      foreach ($form_state['values'] as $key => $val) {
        if (preg_match('/^file_convert_/', $key)) {
          list($from, $to) = explode('_', preg_replace('/^file_convert_/', '', $key));
          $pipelines[preg_replace('/\\\\/', '.', $from)][preg_replace('/\\\\/', '.', $to)] = $val;
        }
      }
      variable_set('file_convert_pipelines', $pipelines);
      drupal_set_message(t('The configuration options have been saved.'));
      break;
    case "Reset to defaults":
      variable_del('file_convert_pipelines');
      drupal_set_message(t('The configuration options have been reset to their default values.'));
      break;
  }
}

