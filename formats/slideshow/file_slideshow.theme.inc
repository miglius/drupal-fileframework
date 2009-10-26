<?php
// $Id$

/**
 * @file
 * Themes for slideshow file formats.
 */

//////////////////////////////////////////////////////////////////////////////
// Theme callbacks

/**
 * Theme for admin PDF checkboxes.
 */
function theme_file_slideshow_admin_settings($form) {
  $rows = array();
  foreach ($form['pdf'] as $name => $element) {
    if (preg_match('/pdf_/', $name)) {
      $rows[] = array(
        drupal_render($form['pdf'][$name][0]),
        drupal_render($form['pdf'][$name][1]),
        drupal_render($form['pdf'][$name][2])
      );
      unset($form['pdf'][$name]);
    }
  }
  $form['pdf']['pdf'] = array(
    '#type' => 'markup',
    '#value' => theme('table', NULL, $rows),
    '#prefix' => '<div class="file-slideshow-pdf-file-slideshow-pdf-data-setting'. (_file_command_exists('pdfinfo') && FILE_SLIDESHOW_PDF ? '' : ' js-hide') .'">',
    '#suffix' => '</div>',
    );
  return drupal_render($form);
}

