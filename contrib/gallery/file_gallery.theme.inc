<?php
// $Id$

/**
 * @file_gallery
 * Themes for file gallery.
 */
//////////////////////////////////////////////////////////////////////////////
// Theme callbacks

/*
 * Theme for the entire page.
 */
function theme_file_gallery_page($content) {
  $output = "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n";
  $output .= '<html xmlns="http://www.w3.org/1999/xhtml">';
  $output .= '<head>';
  $output .= ' <title>'. (drupal_get_title() ? strip_tags(drupal_get_title()) : variable_get('site_name', 'drupal')) .'</title>';
  $output .= drupal_get_html_head();
  $output .= '</head>';
  $output .= '<body style="background-color: #fff; color: #000; margin: 10px;">';
  $output .= "\n<!-- begin content -->\n";
  $output .= $content;
  $output .= "\n<!-- end content -->\n";
  $output .= "</body>\n</html>";
  return $output;
}

/*
 * Theme for the taxonomy term.
 */
function theme_file_gallery_term($data, $options) {
  extract($options, EXTR_SKIP);

  $output = '<li class="file-gallery" style="width: '. ((int)$max_width) .'px; height: '. ((int)$max_height) .'px;">';
  $output .= '<a'. drupal_attributes(array('href' => url($data['url'], array('query' => $data['query'])), 'title' => $data['term']->name, 'class' => 'folder')) .'>';
  $output .= theme('image', drupal_get_path('module', 'file_gallery') .'/images/folder.gif', NULL, NULL, array('width' => 96, 'height' => 96), FALSE);  
  $output .= '</a>';
  $output .= '<span>'. l($data['term']->name, $data['url'], array('title' => check_plain($data['term']->description)), $data['query']) .'</span>';
  $output .= '</li>';
  return $output;
}

/*
 * Theme for the file node.
 */
function theme_file_gallery_node($data, $options) {
  extract($options, EXTR_SKIP);

  $output = '<li class="file-gallery" id="file_'. $data['file']->nid .'" style="width: '. ((int)$max_width) .'px; height: '. ((int)$max_height) .'px;">';
  $output .= $data['url_preview'] ? '<a'. drupal_attributes(array('href' => $data['url_preview'], 'title' => $data['title'], 'class' => $data['class'], 'rel' => 'file-gallery')) .'>'. $data['thumbnail'] .'</a>' : $data['thumbnail'];
  $output .= '<span>'. l($data['title_short'], $data['url_open']) .'</span>';
  if (FILE_GALLERY_SHOW_TYPE) {
    $output .= '<span>'. file_mime_icon_for($data['file']->type) .' '. file_mime_description_for($data['file']->type) .'</span>';
  }
  $output .= '</li>';

  drupal_add_js('$(document).ready(function() { $(\'#file_'. $data['file']->nid .' > span > a\').parent().append(\' | <a href="'. base_path() .'" class="file-metadata" rel="'. base_path() .'file_metadata/'. file_get_hash($data['file']->uri) .'/'. $data['file']->nid .'" title="'. t('Metadata') .'">'. t('info') .'</a>\'); });', 'inline');
  return $output;
}

/*
 * Theme for the empty page.
 */
function theme_file_gallery_empty($filter = NULL) {
  $texts = array('folder' => t('subfolders'), 'image' => t('images'), 'audio' => t('audio files'), 'video' => t('video files'));
  $output = '<span class="file-gallery-empty">';
  $output .= $filter ? t('There are no %files in this folder.', array('%files' => $texts[$filter])) : t('There are no subfolders or files in this folder.');
  $output .= '</span>';
  return $output;
}

