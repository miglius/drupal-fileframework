<?php
// $Id$

/**
 * @file
 * Integrates file operations with the Views module.
 */

//////////////////////////////////////////////////////////////////////////////
// DEPENDANCIES

$views_path = drupal_get_path('module', 'views') .'/modules';
require_once $views_path .'/node.views.inc';

//////////////////////////////////////////////////////////////////////////////
// VIEWS API HOOKS

/**
 * Implementation of hook_views_data().
 */
function file_views_views_data() {
  $data = array();

  // ----------------------------------------------------------------------
  // file_nodes table

  $data['file_nodes']['table']['group']  = t('Files');

  $data['file_nodes']['table']['join'] = array(
    'node' => array(
      'left_field' => 'nid',
      'field' => 'nid',
    ),
    'node_revisions' => array(
      'left_field' => 'vid',
      'field' => 'vid',
    ),
  );

  $data['file_nodes']['size'] = array(
    'title' => t('Size'),
    'help' => t('The size of the file.'),
    'field' => array(
      'handler' => 'views_handler_field_file_nodes_size',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
  );

  $data['file_nodes']['type'] = array(
    'title' => t('Type'),
    'help' => t('The MIME type of the file.'),
    'field' => array(
      'handler' => 'views_handler_field_file_nodes_type',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
  );

  $data['file_nodes']['views'] = array(
    'title' => t('Viewed'),
    'help' => t('Display the number of times the file has been viewed.'),
    'field' => array(
      'handler' => 'views_handler_field_file_nodes_numeric',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
  );

  $data['file_nodes']['downloads'] = array(
    'title' => t('Downloaded'),
    'help' => t('Display the number of times the file has been downloaded.'),
    'field' => array(
      'handler' => 'views_handler_field_file_nodes_numeric',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
  );

  $data['file_nodes']['download'] = array(
    'field' => array(
      'title' => t('Download link'),
      'help' => t('Provide a simple link to download file.'),
      'handler' => 'views_handler_field_file_nodes_download',
    ),
  );

  $data['file_nodes']['view'] = array(
    'field' => array(
      'title' => t('View link'),
      'help' => t('Provide a simple link to view file.'),
      'handler' => 'views_handler_field_file_nodes_view',
    ),
  );

  $data['file_nodes']['uri'] = array(
    'field' => array(
      'title' => t('Thumbnail'),
      'help' => t('Provide a thumbnail of the file.'),
      'handler' => 'views_handler_field_file_nodes_uri',
    ),
  );

  // ----------------------------------------------------------------------
  // file_attachments table

  $data['file_attachments']['table']['group']  = t('Files');

  $data['file_attachments']['table']['name']  = 'file_attachments';
  $data['file_attachments']['table']['join'] = array(
    'node' => array(
      'left_field' => 'nid',
      'field' => 'nid',
    ),
    'node_revisions' => array(
      'left_field' => 'vid',
      'field' => 'vid',
    ),
  );

  $data['file_attachments']['fnid'] = array(
    'title' => t('Attached files'),
    'help' => t('All files attached to a node.'),
    'field' => array(
      'handler' => 'views_handler_field_file_attachments_fnid',
    ),
    'relationship' => array(
      'title' => t('Attached files'),
      'help' => t('Add a relationship to show only nodes which have attachments.'),
      'base' => 'node',
      'field' => 'nid',
      'handler' => 'views_handler_relationship',
      'label' => t('Attachments'),
    ),
  );

  $data['file_attached_to']['table']['group']  = t('Files');

  $data['file_attached_to']['table']['name']  = 'file_attachments';
  $data['file_attached_to']['table']['join'] = array(
    'node' => array(
      'left_field' => 'nid',
      'field' => 'nid',
    ),
    'node_revisions' => array(
      'left_field' => 'vid',
      'field' => 'vid',
    ),
  );

  $data['file_attached_to']['nid'] = array(
    'title' => t('Attached to'),
    'help' => t('All nodes the file is attached to.'),
    'field' => array(
      'handler' => 'views_handler_field_file_attachments_nid',
    ),
  );

  return $data;
}

/**
 * Field handler to present a numeric field.
 */
class views_handler_field_file_nodes_numeric extends views_handler_field {
  function render($values) {
    $num = $values->{$this->field_alias};
    return isset($num) ? $num : '';
  }
}

/**
 * Field handler to present a file size.
 */
class views_handler_field_file_nodes_size extends views_handler_field {
  function render($values) {
    $size = $values->{$this->field_alias};
    return isset($size) ? format_size($size) : '';
  }
}

/**
 * Field handler to present a file type.
 */
class views_handler_field_file_nodes_type extends views_handler_field {
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $form['show_name'] = array(
      '#title' => t('Use type name'),
      '#type' => 'checkbox',
      '#default_value' => !empty($this->options['show_name']),
    );
    $form['show_icon'] = array(
      '#title' => t('Show type icon'),
      '#type' => 'checkbox',
      '#default_value' => !empty($this->options['show_icon']),
    );
  }

  function render($values) {
    $type = $values->{$this->field_alias};
    return (!empty($this->options['show_icon']) ? file_mime_icon_for($type) .'&nbsp;': '') . (!empty($this->options['show_name']) ? file_mime_description_for($type) : $type);
  }
}

/**
 * Field handler to present a file download link.
 */
class views_handler_field_file_nodes_download extends views_handler_field_node_link {
  function construct() {
    parent::construct();
    $this->additional_fields['vid'] = 'vid';
    $this->additional_fields['type'] = array('table' => 'node', 'field' => 'type');
  }

  function render($values) {
    $nid = $values->{$this->aliases['nid']};
    $vid = $values->{$this->aliases['vid']};
    $type = $values->{$this->aliases['type']};
    $text = !empty($this->options['text']) ? $this->options['text'] : t('download');
    return $type == 'file' ? l($text, 'file/'. $nid .'/download/'. $vid) : '';
  }
}

/**
 * Field handler to present a file view link.
 */
class views_handler_field_file_nodes_view extends views_handler_field_node_link {
  function construct() {
    parent::construct();
    $this->additional_fields['vid'] = 'vid';
    $this->additional_fields['type'] = array('table' => 'node', 'field' => 'type');
  }

  function render($values) {
    $nid = $values->{$this->aliases['nid']};
    $vid = $values->{$this->aliases['vid']};
    $type = $values->{$this->aliases['type']};
    $text = !empty($this->options['text']) ? $this->options['text'] : t('view');
    return $type == 'file' ? l($text, 'file/'. $nid .'/view/'. $vid) : '';
  }
}

/**
 * Field handler to present a file thumbnail.
 */
class views_handler_field_file_nodes_uri extends views_handler_field {
  function construct() {
    parent::construct();
    $this->additional_fields['nid'] = 'nid';
    $this->additional_fields['vid'] = 'vid';
    $this->additional_fields['type'] = 'type';
    $this->additional_fields['uri'] = 'uri';
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $form['link_to'] = array(
      '#title' => t('Link this field'),
      '#type' => 'select',
      '#default_value' => $this->options['link_to'],
      '#options' => array(t('No link'), t('Node'), t('Download')),
    );
  }

  function render($values) {
    $nid = $values->{$this->aliases['nid']};
    $vid = $values->{$this->aliases['vid']};
    $type = $values->{$this->aliases['type']};
    $uri = $values->{$this->aliases['uri']};
    if (!empty($uri)) {
      $file = (object)compact('nid', 'vid', 'type', 'uri');
      $thumbnail = file_get_image($file, 'file_image_thumbnail', explode('x', FILE_IMAGE_THUMBNAIL_RESOLUTION));
      $thumbnail = $thumbnail ? $thumbnail : t('no thumbnail');
      return !empty($this->options['link_to']) ? l($thumbnail, $this->options['link_to'] == 1 ? 'node/'. $nid : 'file/'. $nid .'/download', array('html' => TRUE)) : $thumbnail;
    }
    return '';
  }
}

/**
 * Field handler to present a node attachments.
 */
class views_handler_field_file_attachments_fnid extends views_handler_field_prerender_list {
  function construct() {
    parent::construct();
    $this->additional_fields['nid'] = array('table' => 'node', 'field' => 'nid');
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $form['link_to_file'] = array(
      '#title' => t('Link this field to file download'),
      '#type' => 'checkbox',
      '#default_value' => !empty($this->options['link_to_file']),
    );
    $form['only_listed'] = array(
      '#title' => t('Only show "listed" file attachments'),
      '#type' => 'checkbox',
      '#default_value' => !empty($this->options['only_listed']),
    );
    $form['show_icon'] = array(
      '#title' => t('Show type icon'),
      '#type' => 'checkbox',
      '#default_value' => !empty($this->options['show_icon']),
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
      $result = db_query("SELECT fa.fnid, fa.nid, fn.type, n.title FROM {file_attachments} fa LEFT JOIN {file_nodes} fn ON fa.fnid = fn.nid LEFT JOIN {node} n ON fa.fnid = n.nid WHERE fa.nid IN (". implode(', ', $nids) .")". $where ." ORDER BY fa.weight");
      while ($row = db_fetch_object($result)) {
        $this->items[$row->nid][$row->fnid] = $this->render_link(check_plain($row->title), $row->fnid, $row->type);
      }
    }
  }

  /**
   * Render title as a link to the node or file.
   *
   * Data should be made XSS safe prior to calling this function.
   */
  function render_link($title, $nid, $type) {
    $icon = !empty($this->options['show_icon']) ? file_mime_icon_for($type) .'&nbsp;' : '';
    if (!empty($this->options['link_to_file'])) {
      return $icon . l($title, 'file/'. $nid .'/download');
    }
    else {
      return $icon . l($title, 'node/'. $nid);
    }
  }
}

/**
 * Field handler to present a nodes the file is attached to.
 */
class views_handler_field_file_attachments_nid extends views_handler_field_prerender_list {
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
      $result = db_query("SELECT fa.fnid, fa.nid, n.title FROM {file_attachments} fa LEFT JOIN {node} n ON fa.nid = n.nid WHERE fa.nid IN (". implode(', ', $nids) .")". $where ." ORDER BY fa.weight");
      while ($row = db_fetch_object($result)) {
        $this->items[$row->fnid][$row->nid] = $this->render_link(check_plain($row->title), $row->nid);
      }
    }
  }

  /**
   * Render title as a link to the node or file.
   *
   * Data should be made XSS safe prior to calling this function.
   */
  function render_link($title, $nid) {
    return l($title, 'node/'. $nid);
  }
}

