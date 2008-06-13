<?php
// $Id$

/**
 * @file
 * Themes for file convert.
 */
//////////////////////////////////////////////////////////////////////////////
// Theme callbacks

/**
 * Theme for file convert admin converters settings.
 */
function theme_file_convert_admin_converter_settings($form) {
  $rows = array();
  foreach ($form['convert'] as $from => $tos) {
    if (preg_match('/^convert_/', $from)) {
      foreach ($tos as $to => $pipeline) {
        $name = 'file_'. preg_replace('/\./', '\\', $from) .'_'. preg_replace('/\./', '\\', $to);
        if (!preg_match('/^#/', $to)) {
          $row = array(
            preg_replace('/^convert_/', '', $from) .'<br />&nbsp;&nbsp;'. $to,
            drupal_render($pipeline[$name .'_pipeline']),
            $pipeline['#check'],
            array('data' => drupal_render($pipeline[$name .'_enabled']), 'class' => 'checkbox'),
            $pipeline['#module'],
          );
          $rows[] = array('data' => $row, 'class' => !empty($pipeline[$name .'_enabled']['#default_value']) ? ' menu-enabled' : ' menu-disabled');
        }
        unset($form['convert'][$from][$to]);
      }
    }
  }
  $header = array(t('From/To'), t('Conversion utilities'), t('Installed'), t('Enabled'), t('Module'));
  $form['convert']['convert'] = array('#type' => 'markup', '#value' => theme('table', $header, $rows));
  $output .= drupal_render($form);
  return $output;
}

