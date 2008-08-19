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
function theme_file_gallery_page($content, $options = array()) {
  extract($options, EXTR_SKIP);

  $output = "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n";
  $output .= '<html xmlns="http://www.w3.org/1999/xhtml">';
  $output .= '<head>';
  $output .= ' <title>'. (drupal_get_title() ? strip_tags(drupal_get_title()) : variable_get('site_name', 'drupal')) .'</title>';
  $output .= drupal_get_html_head();
  $output .= drupal_get_css();
  $output .= drupal_get_js();
  $output .= '</head>';
  $output .= '<body style="background-color: #fff; color: #000; margin: 10px;">';
  if ($breadcrumb) {
    $output .= '<div id="wrapper"><div id="container"><div class="breadcrumb">'. implode(' > ', drupal_get_breadcrumb()) .'</div></div></div>';
  }
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
  extract($data, EXTR_SKIP);
  extract($options, EXTR_SKIP);

  $image = theme('image', drupal_get_path('module', 'file_gallery') .'/images/folder.gif', NULL, NULL, array('width' => 96, 'height' => 96), FALSE);
  $output = '<li class="file-gallery" style="width: '. ((int)$max_width) .'px; height: '. ((int)$max_height) .'px;">';
  $output .= l($image, $url, array('html' => TRUE, 'query' => $query, 'attributes' => array('class' => 'folder', 'title' => check_plain($term->name))));
  $output .= '<br /><span>'. l($term->name, $url, array('query' => $query, 'attributes' => array('title' => check_plain($term->description)))) .'</span>';
  $output .= '</li>';
  return $output;
}

/*
 * Theme for the file node.
 */
function theme_file_gallery_node($data, $options) {
  extract($data, EXTR_SKIP);
  extract($options, EXTR_SKIP);

  $output = '<li class="file-gallery" id="file-'. $file->nid .'" style="width: '. ((int)$max_width) .'px; height: '. ((int)$max_height) .'px;">';
  $output .= !empty($url_preview) ? l($thumbnail ? $thumbnail : t('No thumbnail'), $url_preview['link'], array('html' => $thumbnail ? TRUE : FALSE, 'query' => $url_preview['query'], 'attributes' => array('onclick' => $url_preview['onclick'], 'title' => $title, 'class' => $class))) : ($thumbnail ? $thumbnail : t('No thumbnail'));
  $file->name = $title_short;
  $output .= '<br />'. theme('file_render', $file, $file_links);
  $output .= '</li>';
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

