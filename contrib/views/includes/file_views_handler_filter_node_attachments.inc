<?php
// $Id$

/**
 * @file
 * Integrates file operations with the Views module.
 */

/**
 * Filter handler to filter file nodes which are attachments.
 */
class file_views_handler_filter_node_attachments extends views_handler_filter {
  function value_form(&$form, &$form_state) {
    $default_value = is_numeric($this->value) ? $this->value : 0;
    $form['value'] = array(
      '#title' => t('List only file nodes which'),
      '#type' => 'radios',
      '#options' => array(t('are not attachments'), t('are attachments')),
      '#default_value' => $default_value,
    );

    if (!empty($form_state['exposed']) && !isset($form_state['input'][$this->options['expose']['identifier']])) {
      $form_state['input'][$this->options['expose']['identifier']] = $default_value;
    }
  }

  function accept_exposed_input($input) {
    $rc = parent::accept_exposed_input($input);

    if ($rc) {
      // If we have previously validated input, override.
      if (isset($this->validated_exposed_input)) {
        $this->value = $this->validated_exposed_input;
      }
    }

    return $rc;
  }

  function exposed_validate(&$form, &$form_state) {
    if (empty($this->options['exposed'])) {
      return;
    }

    if (empty($this->options['expose']['identifier'])) {
      return;
    }
    $identifier = $this->options['expose']['identifier'];
    $this->validated_exposed_input = $form_state['values'][$identifier];
  }

  function admin_summary() {
    if (!empty($this->options['exposed'])) {
      return t('exposed');
    }

    if (!isset($this->options['value'])) {
      return;
    }

    return $this->options['value'] == 0 ? t('Hide attachments') : t('List attachments');
  }

  function query() {
    $this->ensure_my_table();
    $condition = $this->value == 0 ? '=' : '<>';
    $this->query->add_where($this->options['group'], "(SELECT COUNT(*) FROM {file_attachments} fa WHERE fa.fnid = $this->table_alias.nid) $condition 0");
  }
}

