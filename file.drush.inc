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
        '-e' => 'Delete existing file derivatives first.',
        '-f' => 'Run the conversion even if previous conversion for the file failed.',
        '-d' => 'Print debug information about failed conversions.',
        '--type=<MIME type>' => 'Only process files with the specified MIME type.',
        '--nid=<NID>' => 'Only process file with the specified node ID.',
        '--vid=<VID>' => 'Only process file with the specified node revision ID.',
        '--exclude=<NID1,NID2,NID3>' => 'Exclude files with the specified node IDs.',
      ),
      'examples' => array(
        'drush file convert -a --exclude=1234,1235' => 'Create missing derivatives for all files except files with the NID=1234 and NID=1235.',
        'drush file convert --type=image/jpeg' => 'Create all missing derivatives for the files of the image/jpeg MIME type.',
        'drush file convert -e --nid=1234' => 'Recreate all derivatives for the file node with the NID=1234 and it\'s revisions.',
      ),
    ),
    'file change type' => array(
      'callback' => 'drush_file_change_type',
      'description' => "Change MIME type of the file.",
      'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_FULL,
      'options' => array(
        '--mime-old=<MIME type>' => 'Only process files with the specified MIME type. Default MIME type is \'application/octet-stream\'.',
        '--mime=<MIME type>' => 'A new MIME type. If omitted, it will be autodetected.',
        '--nid=<NID>' => 'Only process file with the specified node ID.',
        '--vid=<VID>' => 'Only process file with the specified node revision ID.',
      ),
      'examples' => array(
        'drush file change type --mime-old=image/gif --mime=application/pdf' => 'Change MIME type for files which currently have an \'image/gif\' MIME type to a \'application/pdf\' type.',
        'drush file change type --nid=1234 --vid=2345' => 'Detect MIME type for a file node with the NID=1234 and VID=2345 which currently has a MIME type \'application/octet-stream\'.',
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
    case 'drush:file change type':
      return dt('Change MIME type for the files. This is useful if the correct MIME type was not detected. All existing derived files will be deleted and new ones reconverted.');
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

  $exclude = drush_get_option('exclude');
  $exclude = isset($exclude) ? explode(',', $exclude) : array();
  $exclude = array_filter($exclude, create_function('$a', 'return is_numeric($a);'));

  $total = db_result(db_query($sql_count, $args));
  $result = db_query($sql, $args);
  $current = 0;
  while ($file = db_fetch_object($result)) {
    if (in_array($file->nid, $exclude)) {
      $total--;
      continue;
    }

    if (drush_get_option('e')) {
      $file->force_delete = TRUE;
      $file->keep_blob = TRUE;
      $file->keep_rdf = TRUE;
      file_node_delete_node($file);
    }
    else if (drush_get_option('f')) {
      rdf_delete($file->uri, 'wordnet:failed', NULL, array('repository' => FILE_RDF_REPOSITORY));
    }
    if (drush_get_option('d')) {
      drush_print(str_repeat('-', FILE_DRUSH_CONSOLE_LENGTH));
      $debug = array('nid', 'vid', 'uri', 'size', 'filemime', 'filename');
      $rows = array();
      foreach ($debug as $var)
        $rows[] = array($var, $file->$var);
      drush_print_table($rows, 2);
      drush_print();
    }
    file_generate_previews($file, TRUE);
    $converts += isset($file->converts) ? $file->converts : 0;
    $byte_counter = isset($file->converts) ? $byte_counter + $file->size : $byte_counter;
    $errors += isset($file->errors) ? $file->errors : 0;
    if (drush_get_option('d') && $file->debug) {
      foreach ($file->debug as $entry) {
        foreach ($entry as $key => $val) {
          drush_print('-- '. $key .' --');
          $val = trim($val);
          if (!empty($val))
            drush_print(trim($val));
        }
        drush_print();
      }
    }
    if (drush_get_option('d')) {
      drush_print(($current + 1) .'/'. $total);
      drush_print();
    }
    else {
      _file_drush_indicator($total, $current);
    }
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
function drush_file_change_type() {
  set_time_limit(0);

  $time = time();
  $files = $converted = $errors = 0;
  $converts = $errors = $byte_counter = 0;
  $detected = 0;

  $sql = "SELECT f.nid, f.vid, f.uri, f.size, f.type AS filemime, n.title AS filename FROM {file_nodes} f INNER JOIN {node} n ON f.nid = n.nid";
  $sql_count = "SELECT COUNT(f.vid) FROM {file_nodes} f";
  $where = $args = array();

  if ($nid = drush_get_option('nid')) {
    if (is_numeric($nid)) {
      $where[] = "f.nid = %d";
      $args[] = $nid;
    }
    else {
      drush_set_error(dt('A node ID should be numeric.'));
      return;
    }
  }
  if ($vid = drush_get_option('vid')) {
    if (is_numeric($vid)) {
      $where[] = "f.vid = %d";
      $args[] = $vid;
    }
    else {
      drush_set_error(dt('A revision ID should be numeric.'));
      return;
    }
  }
  if (($mime = drush_get_option('mime')) && !preg_match('|^\w+/\w+$|', $mime)) {
    drush_set_error(dt('Invalid MIME type.'));
    return;
  }
  if (($mime_old = drush_get_option('mime-old')) && !preg_match('|^\w+/\w+$|', $mime_old)) {
    drush_set_error(dt('Invalid old MIME type.'));
    return;
  }
  $mime_old = isset($mime_old) ? $mime_old : 'application/octet-stream';
  $where[] = "f.type = '%s'";
  $args[] = $mime_old;
  $sql .= ' WHERE '. implode(' AND ', $where);
  $sql_count .= ' WHERE '. implode(' AND ', $where);

  $total = db_result(db_query($sql_count, $args));
  $result = db_query($sql, $args);
  $current = 0;
  while ($file = db_fetch_object($result)) {

    // Copy file to the temporal file.
    $file->filepath = tempnam($options['tmpdir'], 'drupal_drush_file_detect_');
    $input = bitcache_get_stream(file_get_hash($file->uri));
    $output = fopen($file->filepath, 'wb');
    stream_copy_to_stream($input, $output);
    fclose($input);
    fclose($output);
    $filemime_old = $file->filemime;
    if (!isset($mime)) {
      file_mime_detect($file, TRUE);
      $file->filemime = !empty($file->filemime) ? $file->filemime : 'application/octet-stream';
      $detected++;
    }
    else {
      $file->filemime = $mime;
    }
    if ($file->filemime != $filemime_old) {
      $file->force_delete = TRUE;
      $file->keep_blob = TRUE;
      file_node_delete_node($file);

      unset($file->uri);
      db_query("UPDATE {file_nodes} f SET type = '%s' WHERE f.nid = %d AND f.vid = %d", $file->filemime, $file->nid, $file->vid);

      file_data_save($file);
      file_generate_previews($file, TRUE);
    }
    file_delete($file->filepath);

    $converts += isset($file->converts) ? $file->converts : 0;
    $byte_counter = isset($file->converts) ? $byte_counter + $file->size : $byte_counter;
    $errors += isset($file->errors) ? $file->errors : 0;
    _file_drush_indicator($total, $current);
    $current++;
  }

  drush_print(dt('Summary:'));
  $rows = array(
    array(dt('Files inspected'), $total),
    array(dt('Files detected'), $detected),
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

