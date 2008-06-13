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

/**
 * Theme for code text rendering.
 */
function theme_file_text_code_render($options = array()) {
  extract($options, EXTR_SKIP);
 
  //return '<div class="code" style="width:580px; height:350px; overflow:auto;">'. utf8_encode($text) .'</div>';
  return '<iframe src="'. url('bitcache/'. $hash, array('query' => array('vid' => $vid, 'disposition' => 'inline'))) .'" width="580px" height="350px"><i/iframe>';
}

