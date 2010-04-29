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

  return '<div style="overflow:auto">'. nl2br(check_plain($text) . (isset($append) ? check_plain($append) : '')) .'</div>';
}

/**
 * Theme for code text rendering.
 */
function theme_file_text_code_render($options = array()) {
  extract($options, EXTR_SKIP);

  $src = bitcache_resolve_uri($uri, array('absolute' => FALSE, 'query' => array_merge(!empty($vid) ? array('vid' => $vid) : array(), array('disposition' => 'inline'))));
  return '<iframe src="'. $src .'" width="580px" height="350px"></iframe>';
}

