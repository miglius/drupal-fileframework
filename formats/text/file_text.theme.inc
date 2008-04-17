<?php
// $Id$

/**
 * @file
 * Themes for text file formats.
 */
//////////////////////////////////////////////////////////////////////////////
// Theme callbacks

/**
 * Theme for text rendering.
 */
function theme_file_text_text_render($options = array()) {

  extract($options, EXTR_SKIP);
  $form['preview'] = array(
    '#type' => 'textarea',
    '#name' => 'file_preview',
    '#rows' => 15,
    '#cols' => 80,
    '#attributes' => array('disabled' => 'disabled'),
    '#value' => utf8_encode($text) . $append,
  );
  return drupal_render($form);
}

