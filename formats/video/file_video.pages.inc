<?php
// $Id$

/**
 * @file
 * User page callbacks.
 */

//////////////////////////////////////////////////////////////////////////////
// Select cover frame

/**
 * Generates the cover frame image.
 *
 * @param $node
 *   A node object.
 * @param $frame
 *   A frame offset in seconds.
 */
function file_video_select_frame($node, $frame) {
  $file = $node->file;
  if (!isset($file)) {
    drupal_access_denied();
    exit;
  }

  $has_flash_preview = FALSE;
  $file_uris = file_generated_for($file);
  foreach ($file_uris as $uri => $data) {
    if ($data['handler'] == 'file_video_flash_flv')
      $has_flash_preview = TRUE;
  }
  if (!$has_flash_preview) {
    drupal_access_denied();
    exit;
  }

  rdf_delete($file->uri, rdf_qname_to_uri('wordnet:video_frame'), NULL, array('repository' => FILE_RDF_REPOSITORY));
  rdf_insert($file->uri, 'wordnet:video_frame', (float)$frame, array('repository' => FILE_RDF_REPOSITORY));
  foreach ($file_uris as $uri => $data) {
    if ($data['type'] == 'image/jpeg') {
      $sources = rdf_normalize(rdf_query($uri, rdf_qname_to_uri('dc:source'), NULL, array('repository' => FILE_RDF_REPOSITORY)));
      if (db_result(db_query("SELECT nid FROM {file_nodes} WHERE uri = '%s'", $uri)) || count($sources[$uri][rdf_qname_to_uri('dc:source')]) > 1)
        rdf_delete($uri, rdf_qname_to_uri('dc:source'), $file->uri, array('repository' => FILE_RDF_REPOSITORY));
      else
        rdf_delete($uri, NULL, NULL, array('repository' => FILE_RDF_REPOSITORY));
    }
  }

  $file->filemime = $file->type;
  $file->fillename = $node->title;

  file_generate_previews($file, TRUE);

  drupal_goto('node/'. $node->nid);
}

