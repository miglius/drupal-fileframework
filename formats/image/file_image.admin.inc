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
  drupal_add_js(drupal_get_path('module', 'file_image') .'/file_image_admin.js');
  $form = array();

  // Processing settings.
  $form['process'] = array('#type' => 'fieldset', '#title' => t('Processing settings'), '#description' => _file_command_exists('convert') ? t('Image Magic "convert" utility is installed in the system.') : t('Image Magic "convert" utility is not installed in the system.'), '#collapsible' => TRUE, '#collapsed' => FALSE);
  $form['process']['file_image_use_image_magic'] = array(
    '#type'          => 'checkbox',
    '#title'         => t('Use Image Magic'),
    '#default_value' => _file_command_exists('convert') ? FILE_IMAGE_USE_IMAGE_MAGIC : 0,
    '#description'   => t('Use command line Image Magic program "convert" to scale images instead of php extensions. This might be slower, but will not consume php memory.'),
    '#disabled'      => _file_command_exists('convert') ? FALSE : TRUE,
  );
  // Preview settings.
  $form['preview'] = array('#type' => 'fieldset', '#title' => t('Preview settings'), '#collapsible' => TRUE, '#collapsed' => FALSE);
  $form['preview']['file_image_preview_resolution'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Maximum resolution for embedded image previews'),
    '#default_value' => FILE_IMAGE_PREVIEW_RESOLUTION,
    '#size'          => 10,
    '#maxlength'     => 255,
    '#description'   => t('Specify the maximum pixel width and height to use when displaying previews of images e.g. in node teasers and bodies. The default is %default pixels.', array('%default' => '640x640')),
  );
  $form['preview']['file_image_medium_resolution'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Maximum resolution for embedded image medium previews'),
    '#default_value' => FILE_IMAGE_MEDIUM_RESOLUTION,
    '#size'          => 10,
    '#maxlength'     => 255,
    '#description'   => t('Specify the maximum pixel width and height to use when displaying images at medium resolution e.g. in file galleries. The default is %default pixels.', array('%default' => '300x300')),
  );
  $form['preview']['file_image_thumbnail_resolution'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Maximum resolution for embedded image thumbnails'),
    '#default_value' => FILE_IMAGE_THUMBNAIL_RESOLUTION,
    '#size'          => 10,
    '#maxlength'     => 255,
    '#description'   => t('Specify the maximum pixel width and height to use when displaying thumbnails of images e.g. in file galleries. The default is %default pixels.', array('%default' => '120x120')),
  );
  $form['preview']['file_image_custom_previews'] = array(
    '#type'          => 'textarea',
    '#title'         => t('Maximum resolution for embedded image custom previews'),
    '#default_value' => variable_get('file_image_custom_previews', ''),
    '#rows'          => 3,
    '#description'   => t('Specify the maximum pixel width and height to use followed by the computer readable short version of the preview name (composed from lowecase letters and underscore) followed by the name of the preview, like %default. One preview definition per line.', array('%default' => '200x200 custom_size Custom size image')),
  );

  // Exif settings.
  $form['exif'] = array('#type' => 'fieldset', '#title' => t('EXIF settings'), '#description' => function_exists('exif_read_data') ? t('Exif extensions are installed in the system.') : t('Exif extensions are not installed in the system.'), '#collapsible' => TRUE, '#collapsed' => FALSE);
  $form['exif']['file_image_exif'] = array(
    '#type'          => 'checkbox',
    '#title'         => t('Enable Extraction of the image\'s <a href="http://en.wikipedia.org/wiki/Exif" title="Exchangeable image file format">EXIF</a> metadata'),
    '#default_value' => function_exists('exif_read_data') ? FILE_IMAGE_EXIF : 0,
    '#description'   => t('Check this box if you want to extract any of the exif metadata.'),
    '#disabled'      => function_exists('exif_read_data') ? FALSE : TRUE,
    '#prefix' => '<div class="file-image-exif-file-image-exif-setting">',
    '#suffix' => '</div>',

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
    if ($j == 3) {
      $i++;
      $j = 0;
    }
  }

  // Geo settings.
  $form['geo'] = array('#type' => 'fieldset', '#title' => t('Geo location settings'), '#collapsible' => TRUE, '#collapsed' => FALSE);
  $form['geo']['file_image_geo'] = array(
    '#type'          => 'checkbox',
    '#title'         => t('Save image\'s geo location to the <a href="@rdf" title="Resource Description Framework">RDF</a>', array('@rdf' => url('admin/settings/rdf'))),
    '#default_value' => function_exists('exif_read_data') && FILE_IMAGE_EXIF ? FILE_IMAGE_GEO : 0,
    '#description'   => t('EXIF extraction should be enabled under the "EXIF settings" for the geo location saving to take effect.'),
    '#disabled'      => function_exists('exif_read_data') && FILE_IMAGE_EXIF ? FALSE : TRUE,
    '#prefix' => '<div class="file-image-geo-file-image-geo-setting">',
    '#suffix' => '</div>',
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
  $values = $form_state['values'];
  switch ($op) {
    case t('Save configuration'):
      if ($values['file_image_use_image_magic'] && !_file_command_exists('convert')) {
        form_set_error('file_image_use_image_magic', t('Image Magic utility "convert" is not installed.'));
      }
      $preview_resolution = $values['file_image_preview_resolution'];
      if (empty($preview_resolution) || !preg_match('/^[0-9]+x[0-9]+$/', $preview_resolution)) {
        form_set_error('file_image_preview_resolution', t('Invalid value %value specified for embedded image preview resolution.', array('%value' => $preview_resolution)));
      }
      $medium_resolution = $values['file_image_medium_resolution'];
      if (empty($medium_resolution) || !preg_match('/^[0-9]+x[0-9]+$/', $medium_resolution)) {
        form_set_error('file_image_medium_resolution', t('Invalid value %value specified for embedded image medium resolution.', array('%value' => $medium_resolution)));
      }
      $thumbnail_resolution = $values['file_image_thumbnail_resolution'];
      if (empty($thumbnail_resolution) || !preg_match('/^[0-9]+x[0-9]+$/', $thumbnail_resolution)) {
        form_set_error('file_image_thumbnail_resolution', t('Invalid value %value specified for embedded image thumbnail resolution.', array('%value' => $thumbnail_resolution)));
      }
      foreach (explode("\n", $values['file_image_custom_previews']) as $line) {
        $line = trim(preg_replace('/\s+/', ' ', $line));
        if (!empty($line) && !preg_match('/^#/', $line)) {
          list($resolution, $handler, $name) = explode(' ', $line, 3);
          if (empty($resolution) || !preg_match('/^[0-9]+x[0-9]+$/', $resolution)) {
            form_set_error('file_image_custom_previews', t('Invalid value %value specified for embedded image custom preview resolution.', array('%value' => $resolution)));
          }
          if (empty($handler) || !preg_match('/^[a-z_]+$/', $handler)) {
            form_set_error('file_image_custom_previews', t('Invalid value %value specified for embedded image custom preview short name.', array('%value' => $handler)));
          }
          if (in_array($handler, array('preview', 'medium', 'thumbnail'))) {
            form_set_error('file_image_custom_previews', t('The value %value is reserved. Please use a different name.', array('%value' => $handler)));
          }
          if (empty($name)) {
            form_set_error('file_image_custom_previews', t('Embedded image custom preview name cannot be empty.'));
          }
        }
      }
      break;
  }
}

/**
 * Submit hook for the settings form.
 */
function file_image_admin_settings_submit($form, &$form_state) {
  $op = $form_state['clicked_button']['#value'];
  $values = $form_state['values'];
  switch ($op) {
    case t('Save configuration'):
      variable_set('file_image_use_image_magic', $values['file_image_use_image_magic']);
      variable_set('file_image_preview_resolution', $values['file_image_preview_resolution']);
      variable_set('file_image_medium_resolution', $values['file_image_medium_resolution']);
      variable_set('file_image_thumbnail_resolution', $values['file_image_thumbnail_resolution']);
      variable_set('file_image_custom_previews', $values['file_image_custom_previews']);
      variable_set('file_image_exif', $values['file_image_exif']);
      $exif = _file_image_exif();
      $exif_data = array();
      foreach ($exif as $name => $data) {
        $exif_data[$name] = $values['file_image_exif_'. $name];
      }
      variable_set('file_image_exif_data', $exif_data);
      variable_set('file_image_geo', $values['file_image_geo']);
      drupal_set_message(t('The configuration options have been saved.'));
      break;
    case t('Reset to defaults'):
      variable_del('file_image_use_image_magic');
      variable_del('file_image_preview_resolution');
      variable_del('file_image_medium_resolution');
      variable_del('file_image_thumbnail_resolution');
      variable_del('file_image_custom_previews');
      variable_del('file_image_exif');
      variable_del('file_image_exif_data');
      variable_del('file_image_geo');
      drupal_set_message(t('The configuration options have been reset to their default values.'));
      break;
  }
}

