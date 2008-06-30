<?php
// $Id$
/**
 * @file
 * Module admin page callbacks.
 */

//////////////////////////////////////////////////////////////////////////////
// File taxonomy settings

/**
 * Implements the settings page.
 *
 * @return
 *   The form structure.
 */
function file_browser_admin_settings() {
  $form = array();

  // Vocabulary settings
  $form['vocabs'] = array('#type' => 'fieldset', '#title' => t('Vocabulary settings'), '#collapsible' => TRUE, '#collapsed' => FALSE);
  $options = array();
  foreach (taxonomy_get_vocabularies('file') as $vid => $vocabulary) {
    $options[$vid] = $vocabulary->name;
  }
  $form['vocabs']['file_browser_vocabularies'] = array(
    '#type'          => 'select',
    '#title'         => t('Include cross-cutting vocabularies'),
    '#default_value' => array_filter(variable_get('file_browser_vocabularies', array()), 'is_string'),
    '#options'       => $options,
    '#multiple'      => TRUE,
    '#description'   => t('Only the selected vocabularies will be shown in the file browser.'),
  );

  // Display settings
  $form['display'] = array('#type' => 'fieldset', '#title' => t('Display settings'), '#collapsible' => TRUE, '#collapsed' => FALSE);
  $form['display']['file_browser_file_link'] = array(
    '#type'          => 'select',
    '#title'         => t('Link file names to'),
    '#default_value' => FILE_BROWSER_FILE_LINK,
    '#options'       => array('file' => t('File download'), 'node' => t('File node')),
    '#description'   => t('Whether to link to the file download, or to the containing node.'),
  );
  $form['display']['file_browser_hide_empty'] = array(
    '#type'          => 'select',
    '#title'         => t('Hide empty folders'),
    '#default_value' => FILE_BROWSER_HIDE_EMPTY,
    '#options'       => array(FALSE => t('Disabled'), TRUE => t('Enabled')),
    '#description'   => t('Whether taxonomy terms that have no sub-terms or file nodes associated with them should be hidden. This can be useful to reduce load time in situations where the vocabulary is very large (e.g. for free-tagging vocabularies).'),
  );
  $form['display']['file_browser_embed_previews'] = array(
    '#type'          => 'select',
    '#title'         => t('Embed thumbnail in file preview block'),
    '#default_value' => FILE_BROWSER_EMBED_PREVIEWS,
    '#options'       => array(FALSE => t('Disabled'), TRUE => t('Enabled')),
    '#description'   => t('Whether the file preview block should include an embedded thumbnail of the file\'s content, for those file formats which provide thumbnail capabilities.'),
  );
  $form['display']['file_browser_folder_properties'] = array(
    '#type'          => 'select',
    '#title'         => t('Show term\'s files count and size'),
    '#default_value' => FILE_BROWSER_FOLDER_PROPERTIES,
    '#options'       => array(FALSE => t('Disabled'), TRUE => t('Enabled')),
    '#description'   => t('Whether the number and the total count of the size of all files below the term should be displayed along with the term name in the browser.'),
  );

  // Organic groups integration
  if (module_exists('og')) {
    $form['og'] = array('#type' => 'fieldset', '#title' => t('Organic groups integration'), '#collapsible' => TRUE, '#collapsed' => FALSE);
    if (module_exists('og_vocab')) {
      $form['og']['file_browser_og_vocabularies'] = array(
        '#type'          => 'checkbox',
        '#title'         => t('Include group vocabularies'),
        '#default_value' => FILE_BROWSER_OG_VOCABULARIES,
        '#description'   => t('Vocabularies defined in the group\'s scope by og_vocab module will be included in the browser automatically even if they are not explicitly selected in "Include cross-cutting vocabularies".'),
      );
    }
    $form['og']['file_browser_og_upload'] = array(
      '#type'          => 'select',
      '#title'         => t('Enable selection of audience on file upload'),
      '#default_value' => FILE_BROWSER_OG_UPLOAD,
      '#options'       => array(FALSE => t('Disabled'), TRUE => t('Enabled')),
      '#description'   => t('Whether the file upload block should allow selection of the audience/groups for the files to be uploaded.'),
    );
    $form['og']['file_browser_og_preview'] = array(
      '#type'          => 'select',
      '#title'         => t('Show groups in file preview block'),
      '#default_value' => FILE_BROWSER_OG_PREVIEW,
      '#options'       => array(FALSE => t('Disabled'), TRUE => t('Enabled')),
      '#description'   => t('Whether the file preview block should include information about the audience/groups the currently selected file is targeted at.'),
    );
    if (module_exists('og_vocab')) {
      $form['og']['file_browser_og_unfiled'] = array(
        '#type'          => 'textfield',
        '#title'         => t('Associate non-categorized file nodes with group folder'),
        '#default_value' => FILE_BROWSER_OG_UNFILED,
        '#size'          => 20,
        '#maxlength'     => 255,
        '#description'   => t('Input a folder name here if you wish to enforce that all unfiled file nodes are automatically associated with a category of that name in the og_vocab.module vocabularies for the groups the file belongs to. Leave empty to disable this functionality.'),
      );
    }
  }

  // Location module integration
  if (module_exists('location')) {
    $form['location'] = array('#type' => 'fieldset', '#title' => t('Location module integration'), '#collapsible' => TRUE, '#collapsed' => FALSE);
    $form['location']['file_browser_location_preview'] = array(
      '#type'          => 'select',
      '#title'         => t('Show geolocation information in file preview block'),
      '#default_value' => FILE_BROWSER_LOCATION_PREVIEW,
      '#options'       => array(FALSE => t('Disabled'), TRUE => t('Enabled')),
      '#description'   => t('Whether to display latitude and longitude information for spatially-enabled nodes. <em>Requires location.module.</em>'),
    );
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
function file_browser_admin_settings_validate($form, &$form_state) {
  $op = $form_state['clicked_button']['#value'];
  switch ($op) {
    case "Save configuration":
      break;
  }
}

/**
 * Submit hook for the settings form.
 */
function file_browser_admin_settings_submit($form, &$form_state) {
  $op = $form_state['clicked_button']['#value'];
  switch ($op) {
    case "Save configuration":
      variable_set('file_browser_vocabularies', $form_state['values']['file_browser_vocabularies']);
      variable_set('file_browser_file_link', $form_state['values']['file_browser_file_link']);
      variable_set('file_browser_hide_empty', $form_state['values']['file_browser_hide_empty']);
      variable_set('file_browser_embed_previews', $form_state['values']['file_browser_embed_previews']);
      variable_set('file_browser_folder_properties', $form_state['values']['file_browser_folder_properties']);
      variable_set('file_browser_og_vocabularies', $form_state['values']['file_browser_og_vocabularies']);
      variable_set('file_browser_og_upload', $form_state['values']['file_browser_og_upload']);
      variable_set('file_browser_og_preview', $form_state['values']['file_browser_og_preview']);
      variable_set('file_browser_og_unfiled', $form_state['values']['file_browser_og_unfiled']);
      variable_set('file_browser_location_preview', $form_state['values']['file_browser_location_preview']);
      drupal_set_message(t('The configuration options have been saved.'));
      break;
    case "Reset to defaults":
      variable_del('file_browser_vocabularies');
      variable_del('file_browser_file_link');
      variable_del('file_browser_hide_empty');
      variable_del('file_browser_embed_previews');
      variable_del('file_browser_folder_properties');
      variable_del('file_browser_og_vocabularies');
      variable_del('file_browser_og_upload');
      variable_del('file_browser_og_preview');
      variable_del('file_browser_og_unfiled');
      variable_del('file_browser_location_preview');
      drupal_set_message(t('The configuration options have been reset to their default values.'));
      break;
  }
}

