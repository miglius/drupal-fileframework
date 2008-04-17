<?php
// $Id$
/**
 * @file
 * Module admin page callbacks.
 */

//////////////////////////////////////////////////////////////////////////////
// File image settings

/**
 * Implements the settings page.
 *
 * @return
 *   The form structure.
 */
function file_image_admin_settings() {
  $form = array();

  // Preview settings
  $form['preview'] = array('#type' => 'fieldset', '#title' => t('Preview settings'), '#collapsible' => TRUE, '#collapsed' => FALSE);
  $form['preview']['file_image_preview_resolution'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Maximum resolution for embedded image previews'),
    '#default_value' => FILE_IMAGE_PREVIEW_RESOLUTION,
    '#size'          => 10,
    '#maxlength'     => 255,
    '#description'   => t('Specify the maximum pixel width and height to use when displaying previews of images e.g. in node teasers and bodies. The default is %default pixels.', array('%default' => '640x640')),
  );
  $form['preview']['file_image_thumbnail_resolution'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Maximum resolution for embedded image thumbnails'),
    '#default_value' => FILE_IMAGE_THUMBNAIL_RESOLUTION,
    '#size'          => 10,
    '#maxlength'     => 255,
    '#description'   => t('Specify the maximum pixel width and height to use when displaying thumbnails of images e.g. in file galleries. The default is %default pixels.', array('%default' => '120x120')),
  );

  // Exif settings
  $form['exif'] = array('#type' => 'fieldset', '#title' => t('EXIF settings'), '#collapsible' => TRUE, '#collapsed' => FALSE);
  $form['exif']['file_image_exif'] = array(
    '#type'          => 'checkbox',
    '#title'         => t('Enable Extraction of the image\'s <a href="http://en.wikipedia.org/wiki/Exif" title="Exchangeable image file format">EXIF</a> metadata'),
    '#default_value' => FILE_IMAGE_EXIF,
    '#description'   => t('Check this box if you want to extract any of the following metadata.'),
  );
  $exif = _file_image_exif();
  foreach ($exif as $name => $data) {
    $exif_data_default[$name] = $data['default'];
  }
  $exif_data = variable_get('file_image_exif_data', $exif_data_default);
  $i = $j = 0;
  foreach ($exif as $name => $data) {
    $form['exif']['exif_'. $i][$j]['file_image_exif_'. $name] = array(
      '#type'          => 'checkbox',
      '#title'         => $data['name'],
      '#default_value' => $exif_data[$name],
    );
    $j++;
    if ($j == 3) { $i++; $j = 0; }
  }

  // Geo settings
  $form['geo'] = array('#type' => 'fieldset', '#title' => t('Geo location settings'), '#collapsible' => TRUE, '#collapsed' => FALSE);
  $form['geo']['file_image_geo'] = array(
    '#type'          => 'checkbox',
    '#title'         => t('Save image\'s geo location to the <a href="@rdf" title="Resource Description Framework">RDF</a>', array('@rdf' => url('admin/settings/rdf'))),
    '#default_value' => FILE_IMAGE_GEO,
    '#description'   => t('EXIF extraction should be enabled under the "EXIF settings" for the geo location saving to take effect.'),
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
function file_image_admin_settings_validate($form, &$form_state) {
  $op = $form_state['clicked_button']['#value'];
  switch ($op) {
    case "Save configuration":
      $preview_resolution = $form_state['values']['file_image_preview_resolution'];
      if (empty($preview_resolution) || !preg_match('/^[0-9]+x[0-9]+$/', $preview_resolution)) {
        form_set_error('file_image_preview_resolution', t('Invalid value %value specified for embedded image preview resolution.', array('%value' => $preview_resolution)));
      }
      $thumbnail_resolution = $form_state['values']['file_image_thumbnail_resolution'];
      if (empty($thumbnail_resolution) || !preg_match('/^[0-9]+x[0-9]+$/', $thumbnail_resolution)) {
        form_set_error('file_image_thumbnail_resolution', t('Invalid value %value specified for embedded image thumbnail resolution.', array('%value' => $thumbnail_resolution)));
      }
      break;
  }
}

/**
 * Submit hook for the settings form.
 */
function file_image_admin_settings_submit($form, &$form_state) {
  $op = $form_state['clicked_button']['#value'];
  switch ($op) {
    case "Save configuration":
      variable_set('file_image_preview_resolution', $form_state['values']['file_image_preview_resolution']);
      variable_set('file_image_thumbnail_resolution', $form_state['values']['file_image_thumbnail_resolution']);
      variable_set('file_image_exif', $form_state['values']['file_image_exif']);
      $exif = _file_image_exif();
      $exif_data = array();
      foreach ($exif as $name => $data) {
        $exif_data[$name] = $form_state['values']['file_image_exif_'. $name];
      }
      variable_set('file_image_exif_data', $exif_data);
      variable_set('file_image_geo', $form_state['values']['file_image_geo']);
      drupal_set_message(t('The configuration options have been saved.'));
      break;
    case "Reset to defaults":
      variable_del('file_image_preview_resolution');
      variable_del('file_image_thumbnail_resolution');
      variable_del('file_image_exif');
      variable_del('file_image_exif_data');
      variable_del('file_image_geo');
      drupal_set_message(t('The configuration options have been reset to their default values.'));
      break;
  }
}

