<?php
// $Id$

/**
 * @file
 * Themes for audio file formats.
 */
//////////////////////////////////////////////////////////////////////////////
// Theme callbacks

/**
 * Theme for admin getID checkboxes.
 */
function theme_file_audio_admin_settings($form) {
  $rows = array();
  foreach ($form['getid3'] as $name => $element) {
    if (preg_match('/getid3_/', $name)) {
      $rows[] = array(
        drupal_render($form['getid3'][$name][0]),
        drupal_render($form['getid3'][$name][1]),
        drupal_render($form['getid3'][$name][2])
      );
      unset($form['getid3'][$name]);
    }
  }
  $form['getid3']['getid3'] = array('#type' => 'markup', '#value' => theme('table', NULL, $rows));
  $output .= drupal_render($form);
  return $output;
}

/**
 * Renders a Flash-based MP3 player for audio previews.
 */
function theme_file_audio_mp3_render($options = array()) {
  extract($options, EXTR_SKIP);

  unset($options['src'], $options['nid'], $options['vid'], $options['title']);
  $options = array_merge((array) $options, array(
    'playlist_url' => url('file_audio/xspf/'. $src),
    'player_title' => $title,
  ));
  $player_path = url() . drupal_get_path('module', 'file') .'/vendor/xspf_player';

  $url = $player_path .'/xspf_player_slim.swf?'. drupal_query_string_encode($options);
  $output = '<object type="application/x-shockwave-flash" data="'. $url .'" width="400" height="15">';
  $output .= '<param name="movie" value="'. $url .'" />';
  $output .= '</object>';
  return $output;
}

