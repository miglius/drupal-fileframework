<?php
// $Id$
/**
 * @file
 * User page callbacks.
 */

//////////////////////////////////////////////////////////////////////////////
// Xspf playlist

/**
 * Outputs a xspf playlist xml.
 *
 * @param $node
 *   A node object.
 * @param $vid
 *   A revision ID.
 * @param $hash
 *   A bitstram hash.
 */
function file_audio_xspf($node, $vid, $hash) {
  $node = node_load($node->nid, $vid);
  $file = $node->file;
  $uri = 'bitcache://'. $hash;

  // Check if the requested uri is a derivatie of the original one.
  $item = rdf_normalize(rdf_query($uri, rdf_qname_to_uri('dc:source'), $file->uri)); 

  if ($file->uri != $uri && empty($item)) {
    // The URI is not the node iteslf, nor the derivative.
    drupal_access_denied();
    exit;
  }

  $item = rdf_normalize(rdf_query($uri, NULL, NULL));

  $output = '<?xml version="1.0" encoding="UTF-8"?>'."\n";
  $output .= '<playlist version="1" xmlns="http://xspf.org/ns/0/">'."\n";
  $output .= '  <trackList>'."\n";
  $output .= '    <track>'."\n";
  $output .= '      <location>'. url('bitcache/'. $hash, array('query' => array('vid' => $vid), 'absolute' => true)) .'</location>'."\n";
  if ($artist = $item[$uri][rdf_qname_to_uri('foaf:made')][0]->value) {
    $output .= '      <creator>'. $artist .'</creator>'."\n";
  }
  if ($album = $item[$uri][rdf_qname_to_uri('wordnet:album')][0]->value) {
    $output .= '      <album>'. $album .'</album>'."\n";
  }
  if ($title = $item[$uri][rdf_qname_to_uri('dc:title')][0]->value) {
    $output .= '      <title>'. $title .'</title>'."\n";
  }
  if ($length = $item[$uri][rdf_qname_to_uri('wordnet:length')][0]->value) {
    list($m, $s) = split(':', $length);
    $length = (int)(($m * 60 + $s) * 1000);
    $output .= '      <duration>'. $length .'</duration>'."\n";
  }
  $output .= '      <annotation>'. $node->title .'</annotation>'."\n";
  $output .= '    </track>'."\n";
  $output .= '  </trackList>'."\n";
  $output .= '</playlist>'."\n";

  drupal_set_header('Content-Type: application/xspf+xml; charset=utf-8');
  print($output);  
  exit;
}

