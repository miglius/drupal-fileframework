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

  $form['clear'] = array(
    '#type' => 'fieldset',
    '#title' => t('Clear data about failed conversions'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#description' => t('When a file conversion fails, it is marked as such to make sure that this conversion is not performed on the same file again from the cron. Click "Clear" to delete all data about failed conversions. This is usefull if a conversion utility is fixed, then the files will be converted from the cron.'),
  );
  $form['clear']['clear'] = array(
    '#type'  => 'submit',
    '#value' => t('Clear'),
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
  $values = $form_state['values'];
  switch ($op) {
    case t('Save configuration'):
      if (!ctype_digit($values['file_convert_limit_size'])) {
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
  $values = $form_state['values'];
  switch ($op) {
    case t('Save configuration'):
      variable_set('file_convert_limit_size', $values['file_convert_limit_size']);
      drupal_set_message(t('The configuration options have been saved.'));
      break;
    case t('Reset to defaults'):
      variable_del('file_convert_limit_size');
      drupal_set_message(t('The configuration options have been reset to their default values.'));
      break;
    case t('Clear'):
      if (!rdf_delete(NULL, 'wordnet:failed', NULL, array('repository' => FILE_RDF_REPOSITORY))) {
        watchdog('file_convert', 'The RDF data about failed conversions was not deleted.', WATCHDOG_ERROR);
      }
      drupal_set_message(t('The data about failed conversions have been cleared.'));
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
  $rows = array();
  $mime_converters = file_get_mime_converters();
  ksort($mime_converters);
  foreach ($mime_converters as $from => $targets) {
    ksort($targets);
    foreach ($targets as $to => $converter) {
      $module = $converter['module'];
      $name = $module .'__'. preg_replace('/\./', '\\', $from) .'_'. preg_replace('/\./', '\\', $to);
      $form['convert'][$module .'__'. $from][$to][$name .'_pipeline'] = array(
        '#type' => 'textfield',
        '#name' => $name .'_pipeline',
        '#default_value' => $converter['pipeline'],
        '#size' => 40,
        '#maxlength' => 1024,
      );
      $form['convert'][$module .'__'. $from][$to]['#check'] = $converter['installed'] ? t('Yes') : t('No');
      $form['convert'][$module .'__'. $from][$to][$name .'_enabled'] = array(
        '#type' => 'checkbox',
        '#name' => $name .'_enabled',
        '#default_value' => $converter['enabled'],
      );
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
    case t('Save configuration'):
      break;
  }
}

/**
 * Submit hook for the settings form.
 */
function file_convert_admin_converter_settings_submit($form, &$form_state) {
  $op = $form_state['clicked_button']['#value'];
  $values = $form_state['values'];
  $pipelines = array();
  foreach ($values as $key => $val) {
    if (preg_match('/^file_.*_pipeline$/', $key)) {
      list($module, $from_to) = explode('__', preg_replace('/_pipeline$/', '', $key));
      list($from, $to) = explode('_', $from_to);
      $enabled = !empty($values[$module .'__'. $from .'_'. $to .'_enabled']) ? 1 : 0;
      $pipelines[$module][preg_replace('/\\\\/', '.', $from)][preg_replace('/\\\\/', '.', $to)] = array('pipeline' => $val, 'enabled' => $enabled);
    }
  }

  switch ($op) {
    case t('Save configuration'):
      foreach ($pipelines as $module => $module_pipelines) {
        variable_set($module .'_convert_pipelines', $module_pipelines);
      }
      drupal_set_message(t('The configuration options have been saved.'));
      break;
    case t('Reset to defaults'):
      foreach ($pipelines as $module => $module_pipelines) {
        variable_del($module .'_convert_pipelines');
      }
      drupal_set_message(t('The configuration options have been reset to their default values.'));
      break;
  }
}

