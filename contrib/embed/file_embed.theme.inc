<?php
// $Id$

/**
 * @file_embed
 * Themes for file embed.
 */
//////////////////////////////////////////////////////////////////////////////
// Theme callbacks

/**
 * Outputs textarea links.
 */
function theme_file_embed_textarea_link($data) {
  extract($data, EXTR_SKIP);
  
  $output = '<div class="file-embed-actions" id="file-embed-'. $id .'">';
  if (isset($url_attach)) {
    $output .= l($link != 'image' ? t('embed an attachment') : theme('image', drupal_get_path('module', 'file_embed') .'/images/add-image.jpg', t('embed an attachment'), t('Click here to add files from your attachments'), array('border' => 0)), $url_attach['url'], array('html' => TRUE, 'query' => $url_attach['query'], 'attributes' => array('class' => 'file-embed-upload')));
    
    //$output .= '<a class="file-embed-upload id="file-embed-upload-'. $id .'" title="'. t('Click here to add files from your attachments') .'" href="' . $url_attach .'">';
    //$output .= t('embed an attachment');
    //$output .= '</a>';
    $output .= '&nbsp;&nbsp;&nbsp;';
  }
  $output .= l($link != 'image' ? t('embed an existing file') : theme('image', drupal_get_path('module', 'file_embed') .'/images/add-image.jpg', t('embed an existing file'), t('Click here to add files from the database'), array('border' => 0)), $url_gallery['url'], array('html' => TRUE, 'query' => $url_gallery['query'], 'attributes' => array('class' => 'file-embed-select')));
  $output .= '</div>';
  return $output;
}

/**
 * Creates an attachments page.
 */
function theme_file_embed_attachments($data) {
  extract($data, EXTR_SKIP);
  
  $output = '<div id="file-gallery-head">';
  $output .= theme('breadcrumb', array('Your attachments'));
  $output .= '</div>';
  $output .= '<div id="file-gallery-body">';
  $output .= '<ul class="file-gallery">';
  $output .= $list;
  $output .= '</ul>';
  $output .= '</div>';
  return $output;
}

/**
 * Creates an embed option page.
 */
function theme_file_embed_options($data) {
  extract($data, EXTR_SKIP);
  
  $output = $script;
  $output .= '<div id="file-gallery-head">';
  $output .= theme('breadcrumb', array('Embed options'));
  $output .= '</div>';
  $output .= '<div id="file-gallery-body">';
  $output .= '<div style="text-align: center; padding-bottom: 10px;"><h4>'. $title .'</h4></div>';
  $output .= '<div style="float: right;">'. ($preview ? $preview : '<br /><span class="no-thumbnail">'. t('No preview') .'</span><br />') .'</div>';
  $output .= $form;
  $output .= '</div>'; 
  return $output;
}

/**
 * Outputs embeded file.
 */
function theme_file_embed_file($data) {
  extract($data, EXTR_SKIP);
  
  $output = '<div class="embed'. (isset($align) ? '-'. $align : '') .'" style="'. ($width ? 'width: '. $width .'px;' : '') .'">';
  $output .= '<div class="embed-file">'. $file_html .'</div>';
  $output .= $caption ? '<div class="embed-caption">'. $caption .'</div>' : '';
  $output .= '</div>';
  return $output;
}

