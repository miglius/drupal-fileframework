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
  $output = implode('.....', array_filter($data, create_function('$a', 'return !empty($a);')));
  return !empty($output) ? '<div class="file-show">'. $output .'</div>'."\n" : '';
}

/**
 * Theme for previews.
 */
function theme_file_previews($data) {
  extract($data, EXTR_SKIP);

  $prevs = array();
  foreach ($previews as $preview) {
    if (!isset($handler_first)) {
      $handler_first = $preview['handler'];
    }
    $prevs[] = '<span class="file-preview-toggle" id="'. $preview['handler'] .'-'. $id .'">'. l($preview['name'], !empty($nid) ? 'node/'. $nid .'/file_preview/'. $preview['handler'] : '') .'</span>';
  }
  if (!empty($prevs)) {
    $output = '<div class="file-generated" id="file-preview-'. $id .'">';
    $output .= t('File') .' '. ($name ? '"'. $name .'" ': '') . t('preview') .': '. implode(' | ', $prevs);
    $output .= !isset($nid) ? '<div id="file-preview-notice-'. $id .'">'. t(' (works only with a Javascript enabled)') .'</div>' : '';
    $output .= '</div>';

    $output .= '<div id="file-preview-container-'. $id .'"></div>';
    if (!isset($nid)) {
      drupal_add_js('$(document).ready(function() { $(\'#file-preview-notice-'. $id .'\').hide(); });', 'inline');
    }
    drupal_add_js('$(document).ready(function() { $(\'#file-preview-'. $id .'\').append(\' | <a href="'. base_path() .'" id="file-preview-hide-'. $id .'" >'. t('hide') .'</a>\'); });', 'inline');
    drupal_add_js('$(document).ready(function() { $(\'#file-preview-hide-'. $id .'\').click(function() { $(\'#file-preview-container-'. $id .'\').html(\'\'); return false; }); });', 'inline');
    if (variable_get('file_handler_autoload', FILE_HANDLER_AUTOLOAD)) {
      drupal_add_js('$(document).ready(function() { $(\'#'. $handler_first .'-'. $id .'\').trigger(\'mousedown\'); });', 'inline');
      /*
      if (count($prevs) == 1) {
        drupal_add_js('$(document).ready(function() { $(\'#file-preview-'. $id .'\').hide(); });', 'inline');
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
  extract($data, EXTR_SKIP);

  $entries = array();
  $handlers = file_get_mime_handlers();
  foreach ($formats as $gid => $format) {
    $description = preg_match('/^file_image_/', $format['handler']) ? $handlers[$format['handler']]['name'] : '';
    $entries[] = '<div id="file-formats-'. $id .'-'. $gid .'">&nbsp;&nbsp;'. file_mime_icon_for($format['type']) .' <small><span class="file-description">'. $format['description'] .'</span> (<span class="file-size">'. format_size($format['size']) .'</span>)'. ($description ? ' [<span class="file-size">'. $description .'</span>]' : '') . (FILE_VIEW_LINK ? ' | '. l(t('view'), $format['uri'], array('query' => array('vid' => $vid, 'disposition' => 'inline'))) : '') .' | '. l(t('download'), $format['uri'], array('query' => array('vid' => $vid))) .'</small></div>';
    if (FILE_INFO_LINK) {
      drupal_add_js('$(document).ready(function() { $(\'#file-formats-'. $id .'-'. $gid .'\').children().append(\' | <a href="'. base_path() .'" class="file-metadata" rel="'. base_path() .'file_metadata/'. $format['hash'] .'/'. $nid .'" title="'. t('Metadata') .'">'. t('info') .'</a>\'); });', 'inline');
    }
  }
  if (!empty($entries)) {
    $output = '<div class="file-formats" id="file-formats-'. $id .'">';
    $output .= ($name ? t('File') .' '. ($name ? '"'. $name .'" ': '') . t('other available formats') : t('Other available formats')) .':<br />';
    $output .= implode('', $entries);
    $output .= '</div>';
  }
  if (FILE_INFO_LINK) {
    drupal_add_css(drupal_get_path('module', 'file') .'/jquery/cluetip/jquery.cluetip.css', 'module');
    drupal_add_js(drupal_get_path('module', 'file') .'/jquery/cluetip/jquery.dimensions.js', 'module');
    drupal_add_js(drupal_get_path('module', 'file') .'/jquery/cluetip/jquery.cluetip.js', 'module');

    //drupal_add_js('$(document).ready(function() { $(\'a.file-metadata\').cluetip({activation: \'click\', arrows: true}); });', 'inline');
    drupal_add_js('$(document).ready(function() { $(\'a.file-metadata\').cluetip({arrows: true}); });', 'inline');
  }
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
  extract($data, EXTR_SKIP);

  $entries = array();
  foreach ($formats as $id => $format) {
    $entries[] = '<div id="block-file-formats-'. $id .'">'. file_mime_icon_for($format['type'], $format['description']) .' <span class="file-size">'. format_size($format['size']) .'</span>'. (FILE_VIEW_LINK ? ' | '. l(t('view'), $format['uri'], array('query' => array('vid' => $vid, 'disposition' => 'inline'))) : '') .' | '. l(t('download'), $format['uri'], array('query' => array('vid' => $vid))) .'</div>';
    if (FILE_INFO_LINK) {
      drupal_add_js('$(document).ready(function() { $(\'#block-file-formats-'. $id .'\').append(\' | <a href="'. base_path() .'" class="file-metadata" rel="'. base_path() .'file_metadata/'. $format['hash'] .'/'. $nid .'" title="'. t('Metadata') .'">'. t('info') .'</a>\'); });', 'inline');
    }
  }
  if (!empty($entries)) {
    $output = '<div id="file-block-formats">';
    $output .= implode('', $entries);
    $output .= '</div>';
    if (FILE_INFO_LINK) {
      drupal_add_css(drupal_get_path('module', 'file') .'/jquery/cluetip/jquery.cluetip.css', 'module');
      drupal_add_js(drupal_get_path('module', 'file') .'/jquery/cluetip/jquery.dimensions.js', 'module');
      drupal_add_js(drupal_get_path('module', 'file') .'/jquery/cluetip/jquery.cluetip.js', 'module');

      //drupal_add_js('$(document).ready(function() { $(\'a.file-metadata\').cluetip({activation: \'click\', arrows: true}); });', 'inline');
      drupal_add_js('$(document).ready(function() { $(\'a.file-metadata\').cluetip({arrows: true}); });', 'inline');
    }
    return $output;
  }
}

