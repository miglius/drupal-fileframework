<?php
// $Id$

/**
 * @file
 * Themes for image file formats.
 */
//////////////////////////////////////////////////////////////////////////////
// Theme callbacks

/**
 * Theme for admin EXIF checkboxes.
 */
function theme_file_image_admin_settings($form) {
  $rows = array();
  foreach ($form['exif'] as $name => $element) {
    if (preg_match('/exif_/', $name)) {
      $rows[] = array(
        drupal_render($form['exif'][$name][0]),
        drupal_render($form['exif'][$name][1]),
        drupal_render($form['exif'][$name][2])
      );
      unset($form['exif'][$name]);
    }
  }
  $form['exif']['exif'] = array('#type' => 'markup', '#value' => theme('table', NULL, $rows));
  $output .= drupal_render($form);
  return $output;
}

/**
 * Theme for image rendering.
 */
function theme_file_image_image_render($options = array()) {

  extract($options, EXTR_SKIP);
  if (is_null($container) || !empty($container)) {
    $output = '<span class="'. $class .'" style="display: block; margin: 0px; padding: 0px;';
    $output .= $width ? ' width: '. min($width, $max_width) .'px;' : '';
    $output .= $height ? ' height: '. min($height, $max_height) .'px;' : '';
    $output .= $width || $height ? ' overflow: hidden;' : '';
    $output .= '">';
  }
  $output .= '<img ';

  $allowed_keys = _file_image_allowed_attributes();
  foreach ($options as $key => $value) {
    if (in_array($key, $allowed_keys) && !empty($value)) {
      // Restrict HTML attribute names to safe values
      $key = preg_replace('/[^a-zA-Z0-9\-\._:]+/', '', strip_tags($key));
      $output .= $key .'="'. check_plain($value) .'" ';
    }
  }

  $output .= '/>';
  if (is_null($container) || !empty($container)) {
    $output .= '</span>';
  }
  return $output;
}

/**
 * Theme for image's thumbnail.
 */
function theme_image_thumbnail_render($width, $height, $options = array()) {
  return theme('render_image', $file, array_merge($options, array('max_width' => $width, 'max_height' => $height)));
}

