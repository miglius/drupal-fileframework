<?php
// $Id$

/**
 * @file
 * Themes for file formats.
 */
//////////////////////////////////////////////////////////////////////////////
// Theme callbacks

/**
 * Theme for file handlers.
 */
function theme_file_admin_handler_settings($form) {
  drupal_add_tabledrag('file-table', 'order', 'sibling', 'handler-weight');
  
  $header[] = '';
  $header[] = t('Handler');
  $header[] = t('MIME types');
  if (function_exists('file_get_mime_converters')) {
    $header[] = t('Converters');
  }
  $header[] = t('Module');
  $header[] = array('data' => t('Enabled'), 'class' => 'checkbox');
  $header[] = t('Weight');

  $rows = array();
  foreach ($form['handler'] as $handler => $element) {
    if (preg_match('/^file_/', $handler)) {
      $row = array('');
      $row[] = drupal_render($element['title']);
      $row[] = drupal_render($element['mime']);
      if (function_exists('file_get_mime_converters')) {
        $row[] = drupal_render($element['convert']);
      }
      $row[] = drupal_render($element['module']);
      $element['weight']['#attributes']['class'] = "handler-weight";
      $row[] = array('data' => drupal_render($element['enabled']), 'class' => 'checkbox');
      $row[] = drupal_render($element['weight']);
      $rows[] = array('data' => $row, 'class' => 'draggable'. ($element['enabled']['#default_value'] ? ' menu-enabled' : ' menu-disabled'));
      unset($form['handler'][$handler]);
    }
  }

  if ($rows) {
    $form['handler']['table'] = array(
      '#value' => theme('table', $header, $rows, array('id' => 'file-table')),
    );
  }
  $output = drupal_render($form);
  return $output;
}

/**
 * Theme for showing previews and generated files.
 */
function theme_file_show($data) {
  return !empty($data) ? '<div class="file-show">'. implode('.....', $data) .'</div>'."\n" : '';
}

/**
 * Theme for previews.
 */
function theme_file_previews($data) {
  $previews = array();
  foreach ($data['previews'] as $preview) {
    if (!isset($handler_first)) { $handler_first = $preview['handler']; }
    $previews[] = '<span class="file_preview_toggle" id="'. $preview['handler'] .'_'. $data['id'] .'">'. l($preview['name'], !empty($data['nid']) ? 'node/'. $data['nid'] .'/file_preview/'. $preview['handler'] : '') .'</span>';
  }
  if (!empty($previews)) {
    $output = '<div class="file-generated" id="file_preview_'. $data['id'] .'">';
    $output .= t('File') .' '. ($data['name'] ? '"'. check_plain($data['name']) .'" ': '') . t('preview') .': '. implode(' | ', $previews);
    $output .= !isset($data['nid']) ? '<div id="file_preview_notice_'. $data['id'] .'">'. t(' (works only with a Javascript enabled)') .'</div>' : '';
    $output .= '</div>';
 
    $output .= '<div id="file_preview_container_'. $data['id'] .'"></div>';
    if (!isset($data['nid'])) {
      drupal_add_js('$(document).ready(function() { $(\'#file_preview_notice_'. $data['id'] .'\').hide(); });', 'inline');
    }
    drupal_add_js('$(document).ready(function() { $(\'#file_preview_'. $data['id'] .'\').append(\' | <a href="'. base_path() .'" id="file_preview_hide_'. $data['id'] .'" >'. t('hide') .'</a>\'); });', 'inline');
    drupal_add_js('$(document).ready(function() { $(\'#file_preview_hide_'. $data['id'] .'\').click(function() { $(\'#file_preview_container_'. $data['id'] .'\').html(\'\'); return false; }); });', 'inline');
    if (variable_get('file_handler_autoload', FILE_HANDLER_AUTOLOAD)) {
      drupal_add_js('$(document).ready(function() { $(\'#'. $handler_first .'_'. $data['id'] .'\').trigger(\'mousedown\'); });', 'inline');
      /*
      if (count($previews) == 1) {
        drupal_add_js('$(document).ready(function() { $(\'#file_preview_'. $data['id'] .'\').hide(); });', 'inline');
      }
      */
    }
  }
  return $output;
}

/**
 * Theme for generated formats.
 */
function theme_file_generated($data) {
  drupal_add_css(drupal_get_path('module', 'file') .'/jquery/cluetip/jquery.cluetip.css', 'module');
  drupal_add_js(drupal_get_path('module', 'file') .'/jquery/cluetip/jquery.dimensions.js', 'module');
  drupal_add_js(drupal_get_path('module', 'file') .'/jquery/cluetip/jquery.cluetip.js', 'module');
  $formats = array();
  foreach ($data['formats'] as $id => $format) {
    if (preg_match('/^file_image_/', $format['handler'])) {
      $handlers = file_get_mime_handlers();
      $description = $handlers[$format['handler']]['name'];
    }
    $formats[] = '<div id="file_formats_'. $data['id'] .'_'. $id .'">&nbsp;&nbsp;'. file_mime_icon_for($format['type']) .' <small><span class="file-description">'. $format['description'] .'</span> (<span class="file-size">'. format_size($format['size']) .'</span>)'. ($description ? ' [<span class="file-size">'. $description .'</span>]' : '') .' | '. l(t('view'), $format['uri'], array('query' => array('vid' => $data['vid'], 'disposition' => 'inline'))) .' | '. l(t('download'), $format['uri'], array('query' => array('vid' => $data['vid']))) .'</small></div>';
    drupal_add_js('$(document).ready(function() { $(\'#file_formats_'. $data['id'] .'_'. $id .'\').children().append(\' | <a href="'. base_path() .'" class="file-metadata" rel="'. base_path() .'file_metadata/'. $format['hash'] .'/'. $data['nid'] .'" title="'. t('Metadata') .'">'. t('info') .'</a>\'); });', 'inline');
  }
  if (!empty($formats)) {
    $output = '<div class="file-formats" id="file_formats_'. $data['id'] .'">';
    $output .= ($data['name'] ? t('File') .' "'. $data['name'] .'" '. t('other available formats') : t('Other available formats')) .':<br />';
    $output .= implode('', $formats);
    $output .= '</div>';
  }
  //drupal_add_js('$(document).ready(function() { $(\'a.file-metadata\').cluetip({activation: \'click\', arrows: true}); });', 'inline');
  drupal_add_js('$(document).ready(function() { $(\'a.file-metadata\').cluetip({arrows: true}); });', 'inline');
  return $output;
}

/**
 * Theme for ajax metadata.
 */
function theme_file_metadata($data) {
  if (!empty($data)) {
    foreach ($data as $d) {
      $output[] = '<strong>'. $d[0] .':</strong><br />&nbsp;&nbsp;'. $d[1];
    }
  }
  print implode('<br />', $output);
  exit;
}

/**
 * Renders a block containing file's metadata.
 */
function theme_file_block_properties($metadata) {
  if (!empty($metadata)) {
    foreach ($metadata as $data) {
      $fields[] = '<span class="metadata-name"><strong>'. $data['name'] .':</strong></span><br />&nbsp;&nbsp;<span class="metadata-value">'. $data['value'] .'</span>';
    }
    //return theme('table', NULL, $rows, array('id' => 'file-block-properties'));
    if (!empty($fields)) {
      return '<div id="file-block-properties">'. implode('<br />', $fields) .'</div>';
    }
  }
}

/**
 * Renders a block containing file's derivatives.
 */
function theme_file_block_formats($data) {
  $formats = array();
  foreach ($data['formats'] as $format) {
    $formats[] = file_mime_icon_for($format['type'], $format['description']) .' <span class="file-size">'. format_size($format['size']) .'</span> | '. l(t('view'), $format['uri'], array('query' => array('vid' => $data['vid'], 'disposition' => 'inline'))) .' | '. l(t('download'), $format['uri'], array('query' => array('vid' => $data['vid']))) .'</small>';
  }
  if (!empty($formats)) {
    $output = '<div id="file-block-formats">';
    $output .= implode('<br />', $formats);
    $output .= '</div>';
    return $output;
  }
}
