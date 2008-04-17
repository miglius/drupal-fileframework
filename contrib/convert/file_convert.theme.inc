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
        if (!preg_match('/^#/', $to)) {
          $rows[] = array(
            preg_replace('/^convert_/', '', $from) .'<br />&nbsp;&nbsp;'. $to,
            drupal_render($pipeline),
            $pipeline['#check'],
            $pipeline['#module'],
          );
        }
        unset($form['convert'][$from][$to]);
      }
    }
  }
  $header = array(t('From/To'), t('Conversion utilities'), t('Installed'), t('Module'));
  $form['convert']['convert'] = array('#type' => 'markup', '#value' => theme('table', $header, $rows));
  $output .= drupal_render($form);
  return $output;
}

