<?php
// $Id$

/**
 * @file
 * Integrates file operations with the Views module.
 */

/**
 * Field handler to present a file URI.
 */
class file_views_handler_field_file_nodes_uri extends views_handler_field {
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $form['show_hash'] = array(
      '#title' => t('Show file hash'),
      '#type' => 'checkbox',
      '#default_value' => !empty($this->options['show_hash']),
      '#description' => t('If checked, the hash of the file will be displayed.'),
    );
  }

  function render($values) {
    $uri = $values->{$this->field_alias};
    return isset($uri) ? (!empty($this->options['show_hash']) ? file_get_hash($uri) : $uri) : '';
  }
}

