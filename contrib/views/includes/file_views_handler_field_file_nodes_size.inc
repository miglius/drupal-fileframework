<?php
// $Id$

/**
 * @file
 * Integrates file operations with the Views module.
 */

/**
 * Field handler to present a file size.
 */
class file_views_handler_field_file_nodes_size extends views_handler_field {
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $form['show_bytes'] = array(
      '#title' => t('Show unformated value in bytes'),
      '#type' => 'checkbox',
      '#default_value' => !empty($this->options['show_bytes']),
      '#description' => t('If checked, the size in bytes will be displayed.'),
    );
  }

  function render($values) {
    $size = $values->{$this->field_alias};
    return isset($size) ? (empty($this->options['show_bytes']) ? format_size($size) : $size) : '';
  }
}

