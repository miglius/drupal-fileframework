<?php
// $Id$
/**
 * @file
 * Module admin page callbacks.
 */

//////////////////////////////////////////////////////////////////////////////
// File attach settings

/**
 * Implements the settings page.
 *
 * @return
 *   The form structure.
 */
function file_attach_admin_settings() {
  $form = array();
  
  // Attachments settings
  $form['attachments'] = array('#type' => 'fieldset', '#title' => t('Attachments settings'), '#collapsible' => TRUE, '#collapsed' => FALSE);
  $form['attachments']['file_attach_method'] = array(
    '#type'          => 'radios',
    '#title'         => t('File upload method'),
    '#options'       => array(TRUE => t('Javascript AHAH based upload'), FALSE => t('Upload on the node save')),
    '#default_value' => FILE_ATTACH_METHOD,
    '#description'   => t('Select the method to upload attachments.'),
  );
  $form['attachments']['file_attach_max_files'] = array(
    '#type'          => 'select',
    '#title'         => t('Attachments on page submit'),
    '#default_value' => FILE_ATTACH_MAX_FILES,
    '#options'       => drupal_map_assoc(array(1, 2, 3, 4, 5, 6, 7, 8, 9, 10)),
    '#description'   => t('If uploads are performed on node submit, select a number of files allowed to attach to the form. This setting does not restrict the total number of attachments. More attachments can be added on the preview page.'),
  );
  
  // Visibility settings
  $form['visibility'] = array('#type' => 'fieldset', '#title' => t('Visibility settings'), '#collapsible' => TRUE, '#collapsed' => FALSE);
  $form['visibility']['file_attach_show_attachments'] = array(
    '#type'          => 'checkbox',
    '#title'         => 'Attachments',
    '#description'   => 'Show the attached files.',
    '#default_value' => FILE_ATTACH_SHOW_ATTACHMENTS,
  );
  $form['visibility']['file_attach_show_previews'] = array(
    '#type'          => 'checkbox',
    '#title'         => 'Previews',
    '#description'   => 'Show the available generated previews for the attached files.',
    '#default_value' => FILE_ATTACH_SHOW_PREVIEWS,
  );
  $form['visibility']['file_attach_show_generated'] = array(
    '#type'          => 'checkbox',
    '#title'         => 'Generated (converted) files',
    '#description'   => 'Show a list of the avialable converted attachments in other MIME types. The list is also available as a block.',
    '#default_value' => FILE_ATTACH_SHOW_GENERATED,
  );
  
  // Vocabulary settings
  $options = array();
  foreach (taxonomy_get_vocabularies() as $vid => $vocab) {
    $options[$vid] = $vocab->name;
  }
  $form['vocabs'] = array('#type' => 'fieldset', '#title' => t('Vocabulary settings'), '#collapsible' => TRUE, '#collapsed' => FALSE);
  $form['vocabs']['file_attach_vocabularies_all'] = array(
    '#type'          => 'radios',
    '#title'         => t('Vocabularies to inherit terms from'),
    '#options'       => array(TRUE => t('All'), FALSE => t('Only bellow selected vocabularies')),
    '#default_value' => FILE_ATTACH_VOCABULARIES_ALL,
    '#description'   => t('Select the vocabularies from which file attachments will inherit the taxonomy terms of the parent node.'),
  );
  $form['vocabs']['file_attach_vocabularies'] = array(
    '#type'          => 'select',
    '#title'         => t('Choose vocabularies to inherit terms from'),
    '#default_value' => variable_get('file_attach_vocabularies', array()),
    '#options'       => $options,
    '#multiple'      => TRUE,
    '#description'   => t('If "Only bellow selected vocabularies" are selected, then choose the vocabularies.'),
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
function file_attach_admin_settings_validate($form, &$form_state) {
  $op = $form_state['clicked_button']['#value'];
  switch ($op) {
    case "Save configuration":
      break;
  }
}

/**
 * Submit hook for the settings form.
 */
function file_attach_admin_settings_submit($form, &$form_state) {
  $op = $form_state['clicked_button']['#value'];
  switch ($op) {
    case "Save configuration":
      variable_set('file_attach_method', $form_state['values']['file_attach_method']);
      variable_set('file_attach_max_files', $form_state['values']['file_attach_max_files']);
      variable_set('file_attach_show_attachments', $form_state['values']['file_attach_show_attachments']);
      variable_set('file_attach_show_previews', $form_state['values']['file_attach_show_previews']);
      variable_set('file_attach_show_generated', $form_state['values']['file_attach_show_generated']);
      variable_set('file_attach_vocabularies_all', $form_state['values']['file_attach_vocabularies_all']);
      variable_set('file_attach_vocabularies', $form_state['values']['file_attach_vocabularies']);
      drupal_set_message(t('The configuration options have been saved.'));
      break;
    case "Reset to defaults":
      variable_del('file_attach_method');
      variable_del('file_attach_max_files');
      variable_del('file_attach_show_attachments');
      variable_del('file_attach_show_previews');
      variable_del('file_attach_show_generated');
      variable_del('file_attach_vocabularies');
      variable_del('file_attach_vocabularies_all');
      drupal_set_message(t('The configuration options have been reset to their default values.'));
      break;
  }
}

