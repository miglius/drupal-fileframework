<?php
// $Id$

/**
 * @file
 * Integrates file operations with the Views module.
 */

/**
 * Field handler to present a file thumbnail.
 */
class file_views_handler_field_file_nodes_uri extends views_handler_field {
  function construct() {
    parent::construct();
    $this->additional_fields['nid'] = 'nid';
    $this->additional_fields['vid'] = 'vid';
  }

  function render($values) {
    $nid = $values->{$this->aliases['nid']};
    $vid = $values->{$this->aliases['vid']};
    $node = node_load(array('nid' => $nid, 'vid' => $vid));
    $file = $node->file;
    if (is_object($file)) {
      $file->name = drupal_strlen($node->title) > FILE_VIEWS_TITLE_LENGTH_SHORT ? drupal_substr($node->title, 0, FILE_VIEWS_TITLE_LENGTH_SHORT - 3) .'...' : $node->title;
      return theme('file_render', $file) .'<script type=\'text/javascript\'>$(\'a.file-metadata\').cluetip();</script>';
    }
    return '';
  }
}

