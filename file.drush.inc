<?php
// $Id$

/**
 * @file
 *   FileFramework drush command.
 */

//////////////////////////////////////////////////////////////////////////////

define('FILE_DRUSH_CONSOLE_LENGTH', 80);

//////////////////////////////////////////////////////////////////////////////

/**
 * Implementation of hook_drush_command().
 */
function file_drush_command() {
  return array(
    'file cron' => array(
      'callback' => 'drush_file_cron',
      'description' => "Run file's cron.",
      'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_FULL,
    ),
    'file convert' => array(
      'callback' => 'drush_file_convert',
      'description' => "Run file conversions.",
      'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_FULL,
      'options' => array(
        '-a' => 'Process all files.',
        '-d' => 'Delete existing file derivatives first.',
        '-f' => 'Run the conversion even if prefious conversion for the file failed.',
        '--nid=<NID>' => 'Only process file with the specified node ID.',
        '--type=<MIME type>' => 'Only process files with the specified MIME type.',
        '--vid=<VID>' => 'Only process file with the specified node revision ID.',
      ),
      'examples' => array(
        'drush file convert -a' => 'Create all missing derivatives.',
        'drush file convert -type=image/jpeg' => 'Create all missing derivatives for the files of the image/jpeg MIME type.',
        'drush file convert -d -nid=1234' => 'Recreate all derivatives for the file node with the NID=1234 and it\'s revisions.',
      ),
    ),
    'file change type' => array(
      'callback' => 'drush_file_change_type',
      'description' => "Change MIME type of the file.",
      'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_FULL,
      'arguments' => array(
        'MIME type' => 'A new MIME type',
        'NID' => 'A node ID of the file node a MIME type should be changed for.',
        'VID' => 'A revision ID of the file node a MIME type should be changed for (optional).',
      ),
      'examples' => array(
        'drush file change type image/jpeg 1234' => 'Change MIME type to "image/jpeg" for a node file and it\'s revisions with a node ID=1234',
        'drush file change type image/jpeg 1234 2345' => 'Change MIME type to "image/jpeg" for a node file with a node ID=1234 and revision ID=2345',
      ),
    ),
    'file failed clear' => array(
      'callback' => 'drush_file_failed_clear',
      'description' => "Clear data about failed conversions and metadata extractions.",
      'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_FULL,
    ),
    'file rdf flush' => array(
      'callback' => 'drush_file_rdf_flush',
      'description' => "Flush files metadata RDF storage.",
      'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_FULL,
    ),
  );
}

/**
 * Implementation of hook_drush_help().
 */
function file_drush_help($section) {
  switch ($section) {
    case 'drush:file cron':
      return dt('Usual cron runs only limited time and only small number of files are inspected in a sliding window manner. This command runs file\'s cron unlimitted time until all files are inspected.');
    case 'drush:file convert':
      return dt('Run file conversion utilities to create missing derivatives.');
    case 'error:file convert':
      return dt('aaaaRun file conversion utilities to create missing derivatives.');
    case 'drush:file change type':
      return dt('Change MIME type for the particular file. This is useful if the correct MIME type was not detected. All existing derived files will be deleted and new ones reconverted.');
    case 'drush:file failed clear':
      return dt('When a file conversion or metadata extraction fails, it is marked as such to make sure that the same conversion or metadata extraction is not performed on the same file again from the cron. Use this command to delete all data about failures. This is useful if a conversion utility is fixed or additional format module is enabled.');
    case 'drush:file rdf flush':
      return dt('Deletes all files\' metadata and derived files mapping from the RDF storage. The derived files will be recreated and their metadata will be extracted from the cron.');
  }
}

//////////////////////////////////////////////////////////////////////////////
// Callbacks

/**
 * A callback function.
 */
function drush_file_cron() {
  set_time_limit(0);
  file_cron();
  drush_print(dt('A cron run has been completed.'));
}

/**
 * A callback function.
 */
function drush_file_convert() {
  set_time_limit(0);

  $time = time();
  $files = $converted = $errors = 0;
  $converts = $errors = $byte_counter = 0;

  $sql = "SELECT f.nid, f.vid, f.uri, f.size, f.type AS filemime, n.title AS filename FROM {file_nodes} f INNER JOIN {node} n ON f.nid = n.nid";
  $sql_count = "SELECT COUNT(f.vid) FROM {file_nodes} f";
  $where = $args = array();

  if (!drush_get_option('a')) {
    if (($type = drush_get_option('type')) && is_string($type) && !empty($type)) {
      $where[] = "f.type = '%s'";
      $args[] = $type;
    }
    if (($nid = drush_get_option('nid')) && is_numeric($nid)) {
      $where[] = "f.nid = %d";
      $args[] = $nid;
    }
    if (($vid = drush_get_option('vid')) && is_numeric($vid)) {
      $where[] = "f.vid = %d";
      $args[] = $vid;
    }

    if (!empty($where)) {
      $sql .= ' WHERE '. implode(' AND ', $where);
      $sql_count .= ' WHERE '. implode(' AND ', $where);
    }
    else {
      drush_set_error(dt('Please specify valid options.'));
      return;
    }
  }

  $total = db_result(db_query($sql_count, $args));
  $result = db_query($sql, $args);
  $current = 0;
  while ($file = db_fetch_object($result)) {
    if (drush_get_option('d')) {
      $file->keep_blob = TRUE;
      $file->keep_rdf = TRUE;
      file_node_delete_node($file);
    }
    else if (drush_get_option('f')) {
      rdf_delete($file->uri, 'wordnet:failed', NULL, array('repository' => FILE_RDF_REPOSITORY));
    }
    file_generate_previews($file, TRUE);
    $converts += isset($file->converts) ? $file->converts : 0;
    $byte_counter = isset($file->converts) ? $byte_counter + $file->size : $byte_counter;
    $errors += isset($file->errors) ? $file->errors : 0;
    _file_drush_indicator($total, $current);
    $current++;
  }

  drush_print(dt('Summary:'));
  $rows = array(
    array(dt('Files inspected'), $total),
    array(dt('Conversions performed'), $converts),
    array(dt('Errors'), $errors),
    array(dt('Total size'), format_size($byte_counter)),
    array(dt('Time elapsed'), dt('@time sec', array('@time' => time() - $time))),
  );
  drush_print_table($rows, 2);
}

/**
 * A callback function.
 */
function drush_file_change_type($type = NULL, $nid = NULL, $vid = NULL) {
  set_time_limit(0);
  if (!isset($type) || !preg_match('|^\w+/\w+$|', $type) || !isset($nid) || !is_numeric($nid) || (isset($vid) && !is_numeric($vid))) {
    drush_set_error(dt('Incorrect arguments.'));
    return;
  }
  if (!($node = node_load($nid, $vid))) {
    drush_set_error(dt('Node does not exists.'));
    return;
  }
  if ($node->type != 'file' || !isset($node->file)) {
    drush_set_error(dt('The node is not a file node.'));
    return;
  }
  $file = $node->file;
  $file->filemime = $file->type;
  unset($file->type);
  $file->filename = $node->title;
  if ($file->filemime == $type) {
    drush_set_error(dt('A new MIME type should differ from the current one.'));
    return;
  }

  $file->keep_blob = TRUE;
  file_node_delete_node($file);
  db_query("UPDATE {file_nodes} f SET f.type = '%s' WHERE f.nid = %d". (isset($vid) ? ' AND f.vid = %d' : ''), array_merge(array($type, $nid), isset($vid) ? array($vid) : array()));
  $file->filemime = $type;
  $file->delete_rdf = TRUE;
  file_generate_previews($file, TRUE);
}

/**
 * A callback function.
 */
function drush_file_failed_clear() {
  if (!rdf_delete(NULL, 'wordnet:failed', NULL, array('repository' => FILE_RDF_REPOSITORY)))
    drush_set_error('The RDF data about failed conversions and metadata extraction was not deleted.');
  else
    drush_print(dt('The data about failed conversions and metadata extraction have been cleared.'));
}

/**
 * A callback function.
 */
function drush_file_rdf_flush() {
  db_query('DELETE FROM {rdf_data_'. FILE_RDF_REPOSITORY .'}');
  drush_print(dt('All files\' metadata and derived files mapping have been deleted from the RDF storage.'));
}

//////////////////////////////////////////////////////////////////////////////
// Misc

function _file_drush_indicator($total, $current) {
  $position = (int)($current*(FILE_DRUSH_CONSOLE_LENGTH-2)/$total);
  print '['. str_repeat('x', $position) . str_repeat(' ', FILE_DRUSH_CONSOLE_LENGTH-2-$position) .']'."\r";

  if ($current + 1 == $total)
    print '['. str_repeat('x', FILE_DRUSH_CONSOLE_LENGTH-2) .']'."\n\n";
}

