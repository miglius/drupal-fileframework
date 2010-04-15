<?php
// $Id$

/**
 * @file
 * Integrates file operations with the Views module.
 */

/**
 * Field handler to present a file type.
 */
class file_views_handler_field_file_nodes_type extends views_handler_field {
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $form['display'] = array(
      '#title' => t('Display'),
      '#type' => 'radios',
      '#default_value' => isset($this->options['display']) ? $this->options['display'] : 0,
      '#options' => array(0 => t('Both icon and type'), 1 => t('Only icon'), 2 => t('Only type')),
    );
    $form['show_name'] = array(
      '#title' => t('Use type name'),
      '#type' => 'checkbox',
      '#default_value' => !empty($this->options['show_name']),
    );
  }

  function render($values) {
    $type = $values->{$this->field_alias};
    switch ($this->options['display']) {
      case 1:
        return file_mime_icon_for($type, $type);
      case 2:
        return !empty($this->options['show_name']) ? file_mime_description_for($type) : $type;
      default:
        return file_mime_icon_for($type) .'&nbsp;'. (!empty($this->options['show_name']) ? file_mime_description_for($type) : $type);
    }
  }
}

