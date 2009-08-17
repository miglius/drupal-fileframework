<?php
// $Id$

/**
 * @file
 * User page callbacks.
 */

//////////////////////////////////////////////////////////////////////////////
// File conversion

/**
 * Re-run file conversions for a file.
 *
 * @param $node
 *   A node object as returned from node_load().
 * @param $vid
 *   A node version ID.
 */
function file_convert_rerun($node, $vid) {
  if (isset($vid) && $node->vid != $vid) {
    $node = node_load($node->nid, $vid);
  }
  if (!user_access('re-run file conversions') || $node->type != 'file' || !is_object($node->file) || (!empty($vid) && !is_numeric($vid))) {
    drupal_not_found();
    exit;
  }
  $file = $node->file;
  $file->filename = $node->title;
  $file->filemime = $file->type;
  unset($file->type);
  $file->force_delete = TRUE;
  $file->keep_blob = TRUE;
  $file->keep_rdf = TRUE;
  file_node_delete_node($file);
  file_generate_previews($file, TRUE);
  drupal_goto('node/'. $node->nid);
}

