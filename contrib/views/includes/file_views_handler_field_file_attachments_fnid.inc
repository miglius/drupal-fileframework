<?php
// $Id$

/**
 * @file
 * Integrates file operations with the Views module.
 */

/**
 * Field handler to present a node attachments.
 */
class file_views_handler_field_file_attachments_fnid extends views_handler_field_prerender_list {
  function construct() {
    parent::construct();
    $this->additional_fields['nid'] = array('table' => 'node', 'field' => 'nid');
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $form['only_listed'] = array(
      '#title' => t('Only show "listed" file attachments'),
      '#type' => 'checkbox',
      '#default_value' => !empty($this->options['only_listed']),
    );
  }

  function query() {
    $this->add_additional_fields();
    $this->field_alias = $this->aliases['nid'];
  }

  function pre_render($values) {
    $nids = array();
    $this->items = array();

    $data = array();
    foreach ($values as $result) {
      $nids[] = $result->{$this->aliases['nid']};
    }
    if ($nids) {
      // Support "only listed files" option.
      $where = '';
      if (!empty($this->options['only_listed'])) {
        $where = " AND fa.list <> 0";
      }
      $result = db_query("SELECT fa.nid AS pnid, fn.*, n.title AS name FROM {file_attachments} fa LEFT JOIN {file_nodes} fn ON fa.fnid = fn.nid LEFT JOIN {node} n ON fa.fnid = n.nid WHERE fa.nid IN (". implode(', ', $nids) .")". $where ." ORDER BY fa.weight");
      while ($file = db_fetch_object($result)) {
        $file->name = drupal_strlen($file->name) > FILE_VIEWS_TITLE_LENGTH_SHORT ? drupal_substr($file->name, 0, FILE_VIEWS_TITLE_LENGTH_SHORT - 3) .'...' : $file->name;
        $this->items[$file->pnid][$file->nid] = theme('file_render', $file);
      }
    }
  }
}

