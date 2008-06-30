<?php
// $Id$

/**
 * @file_browser
 * Themes for file browser.
 */
//////////////////////////////////////////////////////////////////////////////
// Theme callbacks

/**
 * Renders a browser page.
 */
function theme_file_browser_page($data) {
  extract($data, EXTR_SKIP);

  $output = '<div class="file-system" id="file-system-'. $vid .'">';
  $output .= $terms;
  $output .= '</div>';
  $output .= '<script type="text/javascript">$(document).ready(function() { Drupal.file_browserInit(\'page\'); $(document).find("body").append(\''. $popupdiv .'\'); });</script>';
  $output .= '<form id="file_browser-ajax-urlholder-form"><input id="file-ajax-url" type="hidden" value="'. url('file_browser/ajax') .'" /></form>';
  return $output;
}

/**
 * Renders a term line.
 */
function theme_file_browser_term($data) {
  extract($data, EXTR_SKIP);

  $output = '<div class="'. $xhtml_class . ($hierarchy ? ' hierarchy' : '') .'" id="'. $xhtml_id .'" style="display: block;">';
  $output .= '<div class="file-cells" onClick="Drupal.file_browserFolderClick(this);">';
  $output .= '<div class="file-toggle"></div>';
  $output .= '<div class="file-icon"></div>';
  $output .= '<div class="file-title">'. $name .'</div>';
  if ($columns) {
    //$output .= '<div class="file-date">'. ($is_vocab ? '&mdash;' : $count) .'</div>';
    $output .= '<div class="file-date">'. $count .'</div>';
    //$output .= '<div class="file-size">'. ($is_vocab ? '&mdash;' : $size) .'</div>';
    $output .= '<div class="file-size">'. $size .'</div>';
  }
  $output .= '</div></div>';
  if ($expanded) {
    $output .= '<script type="text/javascript">$(document).ready(function() { $(\'#'. $xhtml_id .'\').children().each(function() { Drupal.file_browserFolderClick(this) }); });</script>';
  }
  return $output;
}

/**
 * Renders a file line.
 */
function theme_file_browser_file($data) {
  extract($data, EXTR_SKIP);

  $title_length = $columns ? 50 : 14;
  $title = drupal_strlen($title) > $title_length ? drupal_substr($title, 0, $title_length - 3) .'...' : $title;
  $link = l($title, $url, array('attributes' => array('title' => $title), 'onclick' => 'return Drupal.file_browserFileLinkClick(this);'));

  $output = '<div class="file-node" id="file-node-'. $id .'" style="display: block;">';
  $output .= '<div class="file-cells" onClick="Drupal.file_browserFileClick(this);">';
  $output .= '<div class="file-icon">'. $icon .'</div>';
  $output .= '<div class="file-title">'. $link .'</div>';
  if ($columns) {
    $output .= '<div class="file-date">'. $date .'</div>';
    $output .= '<div class="file-size">'. $size .'</div>';
  }
  $output .= '</div>';
  $output .= '</div>';
  return $output;
}

/**
 * Outputs a file preview block.
 */
function theme_file_browser_preview($data) {
  extract($data, EXTR_SKIP);

  $output = '<input id="file-preview-url" type="hidden" value="'. url('file_browser/preview') .'" />';
  $output .= '<div id="file-preview-spinner" style="display: none;"></div>';
  $output .= '<div id="file-preview">';

  if (is_object($node)) {
    $file = $node->file;
    $output .= '<div id="file-preview-info">';
    $output .= '<table>';
    $output .= '<tr><th>'. t('Name:') .'</th><td>'. l($node->title, 'node/'. $file->nid) .'</td></tr>';
    $output .= '<tr><th>'. t('Type:') .'</th><td>'. file_mime_icon_for($file->type) .'&nbsp;'. file_mime_description_for($file->type) .'</td></tr>';
    $output .= '<tr><th>'. t('Size:') .'</th><td>'. format_size($file->size) .'</td></tr>';
    $output .= '<tr><th>'. t('Author:') .'</th><td>'. l($node->name, 'user/'. $node->uid) .'</td></tr>';
    $output .= '<tr><th>'. t('Created:') .'</th><td>'. format_date($node->created, 'small') .'</td></tr>';
    $output .= '<tr><th>'. t('Modified:') .'</th><td>'. format_date($node->changed, 'small') .'</td></tr>';
    $output .= '<tr><th>'. t('Views:') .'</th><td>'. $file->views .'</td></tr>';
    $output .= '<tr><th>'. t('Downloads:') .'</th><td>'. $file->downloads .'</td></tr>';

    // Integration with og.module
    if (!empty($groups)) {
      $output .= '<tr><th>'. t('Groups:') .'</th><td>'. implode(', ', $groups) .'</td></tr>';
    }

    // Integration with location.module
    if (!empty($location)) {
      $output .= '<tr><th>'. t('Latitude:') .'</th><td>'. $location['latitude'] .'</td></tr>';
      $output .= '<tr><th>'. t('Longitude:') .'</th><td>'. $location['longitude'] .'</td></tr>';
    }
    $output .= '</table>';
    $output .= '</div>';
    $output .= $url_preview ? '<a'. drupal_attributes(array('href' => $url_preview, 'title' => check_plain($node->title), 'class' => $class, 'rel' => 'file-gallery')) .'>'. $thumbnail .'</a>' : $thumbnail;
    $output .= '<div id="file-preview-download">'. (FILE_VIEW_LINK ? l(t('view'), 'file/'. $file->nid .'/view/'. $file->vid) .' | ' : '') . l(t('download'), 'file/'. $file->nid .'/download/'. $file->vid) .'</div>';
    $output .= '<script type=\'text/javascript\'>$(document).ready(function() { $(\'#file-preview-download\').append(\' | <a href="'. base_path() .'" class="file-metadata" rel="'. base_path() .'file_metadata/'. file_get_hash($file->uri) .'/'. $file->nid .'" title="'. t('Metadata') .'">'. t('info') .'</a>\'); }); $(document).ready(function() { $(\'a.file-metadata\').cluetip({arrows: true}); });</script>';
    if ($script) {
      $output .= $script;
    }
  }

  // Backreferences
  if (!empty($nodes)) {
    $output .= '<div id="file-references">';
    $output .= '<span>'. t('This file is attached to') .':</span>';
    $output .= '<div class="item-list">';
    $output .= '<ul>';
    foreach ($nodes as $n) {
      $output .= '<li>'. l($n->title, 'node/'. $n->nid, $n->cid ? array('fragment' => 'comment-'. $n->cid) : array()) .'</li>';
    }
    $output .= '</ul>';
    $output .= '</div>';
  }
  $output .= '</div>'; // id=file-preview
  return $output;
}

/**
 * Outputs a browser block.
 */
function theme_file_browser_block($data) {
  extract($data, EXTR_SKIP);

  $output = '<div class="file-system" id="file-system-'. $vid .'">';
  $output .= $terms;
  $output .= '</div>';
  $output .= $popupdiv;
  $output .= '<div class="more-link">'. l(t('more'), 'file_browser/'. $vid, array('title' => t('View file browser'))) .'</div>';
  $output .= '<script type="text/javascript">Drupal.file_browserInit(\''. $vid .'\');</script>';
  $output .= '<form id="file-node-url-form"><input id="file-node-url" type="hidden" value="'. url('node/') .'" /></form>';
  $output .= '<form id="file_browser-ajax-urlholder-form"><input id="file-ajax-url" type="hidden" value="'. url('file_browser/ajax') .'" /></form>';
  return $output;
}

/**
 * Outputs a create new term block.
 */
function theme_file_browser_newterm($form) {
  $output = '<div id="file_browser-newterm">';
  $output .= $form;
  $output .= '</div>';
  $output .= '<iframe src="javascript:;" id="file_browser-newterm-submission-frame" name="file_browser-newterm-submission-frame" border="0" frameborder="0" width="0" height="0"></iframe>';
  return $output;
}

/**
 * Renders a file upload block
 */
function theme_file_browser_upload($form) {
  $output .= '<div id="file-upload-spinner" style="display: none;"></div>';
  $output .= '<div id="file-upload">';
  $output .= $form;
  $output .= '</div>';
  $output .= '<iframe src="javascript:;" id="file_browser-upload-submission-frame" name="file_browser-upload-submission-frame" border="0" frameborder="0" width="0" height="0"></iframe>';
  return $output;
}

