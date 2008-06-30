<?php
// $Id$
/**
 * @file
 * Implements an extended procedural File API for file management.
 */

//////////////////////////////////////////////////////////////////////////////
// File node API

/**
 * Saves file blob in the bitchache and saves file uri to the db.
 *
 * @param $node
 *   A populated node object.
 * @param $file
 *   A popolated file object.
 *
 * @return
 *   SAVED_NEW or SAVED_UPDATED on success or FALSE on failure.
 */
function file_node_save(&$node, $file) {

  if (!is_object($file)) {
    watchdog('file', 'The file object is not passed to a file_node_save() function for the node with nid=%nid.', array('%nid' => $node->nid), WATCHDOG_ERROR, l(t('edit'), 'node/'. $node->nid));
    return FALSE;
  }

  if (!$file->uri) {
    // Note that at this point, we have a transient file object with a path,
    // MIME type and file size. The file is still located in the temporary
    // directory.

    // If we didn't get a proper MIME type from the client, attempt to guess
    // it from the uploaded file name
    if (empty($file->filemime) || $file->filemime == 'application/octet-stream') {
      $file->filemime = file_mime_guess($file->filepath);
    }

    // If specified by the administrator, we'll always try and autodetect the
    // MIME type on the server-side for any uploaded files
    if (FILE_MIME_AUTODETECTION > 0 && ($mime = file_mime_detect($file))) {
      $file->filemime = $mime;
    }

    // Assign nid and vid to the file object for the logging.
    if (isset($node->nid)) {
      $file->nid = $node->nid;
      $file->vid = $node->vid;
    }

    // Move the file to permanent storage, deleting the temporary file.
    // Save metadata information to the RDF.
    if (!file_data_save($file)) {
      return FALSE;
    }

    // Remove the entry from the {files} table about the temporary file, which was
    // added by the file_save_upload().
    db_query('DELETE FROM {files} WHERE fid = %d', $file->fid);

    // Generate and save the preview of the file.
    if (!file_generate_previews($file)) {
      watchdog('file', 'The preview generation faild for the file %name.', array('%name' => $file->filename), WATCHDOG_ERROR, $node->nid ? l(t('edit'), 'node/'. $node->nid) : '');
    }
  }

  // Create a file object for the node and saving it to the db.
  $node->file = (object)array('nid' => $node->nid, 'vid' => $node->vid, 'uri' => $file->parent ? $file->parent : $file->uri, 'size' => $file->filesize ? $file->filesize : $file->size, 'type' => $file->filemime ? $file->filemime : $file->type, 'name' => $file->filename ? $file->filename : $file->name);
  if ($node->nid && !$node->nosave) {
    return drupal_write_record('file_nodes', $node->file, empty($node->is_new) && empty($node->revision) ? array('nid', 'vid') : array());
  }
  else {
    global $user;
    $file_tmp = (object)array('uid' => $user->uid, 'uri' => $file->parent ? $file->parent : $file->uri, 'created' => time());
    return drupal_write_record('file_tmp', $file_tmp);
    return TRUE;
  }
}

/**
 * Saves file blob in the bitchache and metadata to RDF repository.
 *
 * @param $file
 *   A popolated file object.
 *
 * @return
 *   Bitcache resource uri.
 */
function file_data_save(&$file) {
  // Check the file size.
  if (!filesize($file->filepath)) {
    if (!isset($file->converted)) {
      unlink($file->filepath);
    }
    watchdog('file', 'The file converted from uri=%uri, name=%name to %mime was not saved because it was empty.', array('%uri' => $file->uri, '%name' => $file->filename, '%mime' => $file->filemime), WATCHDOG_ERROR);
    return FALSE;
  }

  // Hook with a parent file.
  $parent = isset($file->parent) || isset($file->uri) ? array(
    'dc:source' => array(isset($file->parent) ? $file->parent : $file->uri),
  ) : array();

  // Move the file to permanent storage, deleting the temporary file.
  $hash = bitcache_put_file(NULL, $file->filepath, $file->converted ? FALSE : TRUE);
  if (!$hash) {
    watchdog('file', 'The bitcache_put_file() function failed for the file converted from uri=%uri, name=%name to %mime.', array('%uri' => $file->uri, '%name' => $file->filename, '%mime' => $file->filemime), WATCHDOG_ERROR);
    return FALSE;
  }
  $file->uri = bitcache_uri($hash);

  // Save metadata information to the RDF.
  if (!rdf_exists($file->uri)) {
    $metadata = isset($file->metadata) ? array($file->uri => array_merge($parent, $file->metadata)) : array_merge_recursive(array($file->uri => $parent), file_get_metadata($file));
    if (!rdf_insert_all(rdf_denormalize($metadata), array('repository' => 'file'))) {
      watchdog('file', 'The RDF data was not saved for the file converted from uri=%uri, name=%name to %mime.', array('%uri' => $file->uri, '%name' => $file->filename, '%mime' => $file->filemime), WATCHDOG_ERROR);
      return FALSE;
    }
  }
  return TRUE;
}

/**
 * Programmatically creates a new file container node.
 *
 * @param $node
 *   A pre-populated node array.
 *
 * @return
 *   A fully populated node object.
 */
function file_node_create($node = array()) {
  global $user;
  $node = (object)$node;

  if (empty($node->file)) {
    return FALSE;
  }

  // Set default attributes
  $node->type = 'file';
  $node_type_default = variable_get('node_options_'. $node->type, array('status', 'promote'));
  $node->title = isset($node->title) ? $node->title : ($node->file->name ? $node->file->name : $node->file->filename);
  $node->uid = isset($node->uid) ? $node->uid : $user->uid;
  $node->status = isset($node->status) ? $node->status : in_array('status', $node_type_default);
  $node->comment = isset($node->comment) ? $node->comment : variable_get('comment_'. $node->type, 2);
  $node->promote = isset($node->promote) ? $node->promote : in_array('promote', $node_type_default);
  $node->moderate = isset($node->moderate) ? $node->moderate : in_array('moderate', $node_type_default);
  $node->revision = isset($node->revision) ? $node->revision : in_array('revision', $node_type_default);
  $node->sticky = isset($node->sticky) ? $node->sticky : in_array('sticky', $node_type_default);
  $node->format = FILTER_FORMAT_DEFAULT;
  $node->body = '';

  // Save and load the node to get a proper $node->file.
  node_save($node);
  return node_load($node->nid);
}

/**
 * Deletes a file and all previews.
 *
 * @param $node
 *   A pre-populated node array.
 *
 */
function file_node_delete_node($file) {
  if (!empty($file->uri)) {
    $count = db_result(db_query("SELECT COUNT(f.nid) FROM {file_nodes} f WHERE f.uri = '%s'", $file->uri));

    // File object passed from cron has a created property.
    if ((!$file->created && $count == 1) || ($file->created && $count == 0)) {
      // The last reference to the blob.
      // Search for a previews.
      $derived = rdf_normalize(rdf_query(NULL, NULL, $file->uri));
      if ($derived) {
        // Delete all previews.
        foreach ($derived as $key => $data) {
          // Check if derived is not referenced in the table.
          if (db_result(db_query("SELECT COUNT(f.nid) FROM {file_nodes} f WHERE f.uri = '%s'", $key)) == 0) {
            if (!rdf_delete($key, NULL, NULL, array('repository' => 'file'))) {
              watchdog('file', 'The RDF data was not deleted for the subject s=%sub.', array('%sub' => $key), WATCHDOG_ERROR);
            }
            // Delete from a bitcache.
            if (!bitcache_delete(file_get_hash($key))) {
              watchdog('file', 'The %uri blob was not deleted from the bitcache.', array('%uri' => $key), WATCHDOG_ERROR);
            }
          }
        }
      }
      if (!rdf_delete($file->uri, NULL, NULL, array('repository' => 'file'))) {
        watchdog('file', 'The RDF data was not deleted for the subject s=%sub.', array('%sub' => $file->uri), WATCHDOG_ERROR);
      }
      // Delete from a bitcache.
      if (!bitcache_delete(file_get_hash($file))) {
        watchdog('file', 'The %uri blob was not deleted from the bitcache.', array('%uri' => $file->uri), WATCHDOG_ERROR);
      }
    }
  }
}

//////////////////////////////////////////////////////////////////////////////
// File MIME type detection

/**
 * Searches for a MIME registry file in a predefined locations.
 *
 * @param $file
 *   Filename of the MIME registry file.
 *
 * @return
 *   A full path to the registry file if it is found or NULL otherwise.
 */
function file_mime_registry_path($file = 'mime.types') {
  static $path = NULL;
  if (!$path) {
    $locations = FILE_MIME_REGISTRY_PATH ? array(FILE_MIME_REGISTRY_PATH) : array(
      dirname(__FILE__),         // Drupal module directory
      '/opt/local/apache2/conf', // Apache 2.x on OS X w/ MacPorts
      '/etc/apache2',            // Apache 2.x
      '/etc/apache',             // Apache 1.x
      '/etc/httpd',              // Apache 1.x
      '/etc',                    // Debian/Ubuntu/etc
    );
    foreach ($locations as $location) {
      $filename = $location .'/'. $file;
      if (file_exists($filename) && is_readable($filename)) {
        return $path = $filename;
      }
    }
  }
  return $path;
}

/**
 * This function executes all hook_mime_types() hooks in the format modules
 * and creates an array of all supported MIME types pointing tho the array of
 * module name, description and icon file.
 *
 * @return
 *   A structured array of the MIME types pointing to the arary of the MIME details.
 */
function file_get_mime_types() {
  static $handlers = array();
  if (!$handlers) {
    foreach (module_implements('mime_types') as $module) {
      if (($types = module_invoke($module, 'mime_types')) && is_array($types)) {
        foreach ($types as $mime => $data) {
          $types[$mime]['module'] = $module;
          if (!is_array($data['handlers'])) $types[$mime]['handlers'] = array();
          if (!is_array($data['extensions'])) $types[$mime]['extensions'] = array();
        }
        $handlers = array_merge_recursive($handlers, $types);
      }
    }
  }
  return $handlers;
}

/**
 * Implementation of hook_mime_types().
 */
function file_mime_types() {
  return array(
    'application/octet-stream' => array(
      'name' => t('Unrecognized file type'),
      'icon' => 'binary.gif',
    ),
  );
}

/**
 * Returns a description of the MIME type. If a particular format module is not enabled
 * the default description is returned.
 *
 * @param $type
 *   A MIME type.
 * @param $default
 *   Default description for the MIME type to be return if no modules are implementing the MIME type.
 *
 * @return
 *   The description for the MIME type.
 */
function file_mime_description_for($type, $default = NULL) {
  $handlers = file_get_mime_types();
  $description = is_array($handlers[$type]['name']) ? reset($handlers[$type]['name']) : $handlers[$type]['name'];
  $default_mime_types = file_default_mime_types();
  return isset($description) ? $description :
    // If the exact mime type is not defined, we give a default description.
    (in_array(preg_replace('/([^\/]+).*/', '$1/*', $type), array_keys($default_mime_types)) ? $default_mime_types[preg_replace('/([^\/]+).*/', '$1/*', $type)]['name'] : $default);
}

/**
 * Returns an image HTML block with the MIME type icon.
 *
 * @param $type
 *   A MIME type.
 *
 * @return
 *   The HTML image block for the MIME type.
 */
function file_mime_icon_for($type, $description = NULL) {
  $handlers = file_get_mime_types();
  $default_mime_types = file_default_mime_types();
  $icon = is_array($handlers[$type]['icon']) ? reset($handlers[$type]['icon']) : $handlers[$type]['icon'];
  $icon = isset($icon) ? $icon : (in_array(preg_replace('/([^\/]+).*/', '$1/*', $type), array_keys($default_mime_types)) ? $default_mime_types[preg_replace('/([^\/]+).*/', '$1/*', $type)]['icon'] : '');
  if (!empty($icon) && file_exists(drupal_get_path('module', 'file') .'/icons/'. $icon)) {
    return theme('image', drupal_get_path('module', 'file') .'/icons/'. $icon, $description, $description, NULL, TRUE);
  }
  return NULL;
}

/**
 * Returns a MIME extensions registered with the web server.
 *
 * @return
 *   A structured array of the MIME types pointing to the array of extensions.
 */
function file_mime_extensions() {
  static $registry = NULL;
  if (!$registry) {
    $registry = array();
    if (($typemap = file_mime_registry_path())) {
      if (($file = fopen($typemap, 'r'))) {
        while (!feof($file)) {
          if (($line = trim(preg_replace('/\s+/', ' ', fgets($file, 1024)))) && $line[0] != '#') {
            $extensions = explode(' ', $line);
            $type = array_shift($extensions);
            if (count($extensions) > 0)
              $registry[$type] = $extensions;
          }
        }
        fclose($file);
      }
    }
    foreach (file_get_mime_types() as $type => $data) {
      if (!empty($data['extensions'])) {
        $registry[$type] = is_array($registry[$type]) ? array_unique(array_merge($data['extensions'], $registry[$type])) : $data['extensions'];
      }
    }
  }
  return $registry;
}

/**
 * Returns supported extensions for the MIME type.
 *
 * @param $type
 *   A MIME type.
 *
 * @return
 *   An array of the extensions.
 */
function file_mime_extensions_for($type) {
  $extensions = file_mime_extensions();
  return is_array($extensions[$type]) ? $extensions[$type] : array();
}

/**
 * Finds the first matching MIME content type based on a file's extension.
 *
 * @param $filename
 *   Full filename of the file.
 *
 * @return
 *   A MIME type of the file.
 */
function file_mime_guess($filename) {
  $registry = file_mime_extensions();
  $pathinfo = pathinfo($filename);
  $extension = $pathinfo['extension'];
  foreach ($registry as $type => $extensions) {
    if (in_array($extension, $extensions)) {
      return $type;
    }
  }

  // Default MIME type if the file extension is not registerd.
  return 'application/octet-stream';
}

/**
 * Tries to detect the file MIME type using other utilities.
 *
 * @param $filename
 *   Full filename of the file.
 *
 * @return
 *   A MIME type of the file.
 */
function file_mime_detect($file) {
  if (FILE_MIME_AUTODETECTION == 2) {
    $mimes = $exts = array();
    foreach (explode("\n", FILE_MIME_AUTODETECTION_CONDITIONS) as $line) {
      $line = trim($line);
      if (!empty($line)) {
        if (preg_match('/\//', $line)) {
          $mimes[] = $line;
        }
        else {
          $exts[] = $line;
        }
      }
    }
    $ext = preg_replace('/^.*\.([^\.]+)$/', '$1', drupal_strtolower($file->filename));
    if (!in_array($ext, $exts) && !in_array($file->filemime, $mimes)) {
      return FALSE;
    }
  }

  if (extension_loaded('fileinfo') && ($finfo = finfo_open(FILEINFO_MIME))) {
    // Use 'fileinfo' php extension
    $type = finfo_file($finfo, $file->filepath);
    finfo_close($finfo);
    return $type;
  }
  else if (file_exists($file->filepath)) {
    // Attempts to detect a file's MIME type using the Unix `file' utility.
    // MIME content type format defined in http://www.ietf.org/rfc/rfc1521.txt
    $filename = realpath($file->filepath);
    $output = exec('file -bi '. escapeshellarg($file->filepath), $ignore, $status);
    if ($status == 0 && preg_match('!([\w-]+/[\w\d_+-]+)!', $output, $matches))
      return $matches[1];
  }

  // Default MIME type if the file extension is not registerd.
  return $file->filemime;
}

/**
 * Default MIME types and their descriptions.
 * This is needed if corresponding format modules are not enabled.
 * object.
 *
 * @return
 *   Structured array of default MIME types.
 */
function file_default_mime_types() {
  return array(
    'text/*' => array(
      'name' => t('Text file'),
      'icon' => 'text.gif',
    ),
    'image/*' => array(
      'name' => t('Image file'),
      'icon' => 'image.gif',
    ),
    'audio/*' => array(
      'name' => t('Audio file'),
      'icon' => 'audio.gif',
    ),
    'video/*' => array(
      'name' => t('Video file'),
      'icon' => 'video.gif',
    ),
    'application/*' => array(
      'name' => t('Unrecognized file type'),
      'icon' => 'binary.gif',
    ),
  );
}

/**
 * This function executes all hook_mime_handlers() hooks in the format modules
 * and creates an array of all supported MIME handlers pointing tho the array of
 * handler details.
 *
 * @return
 *   A structured array of the MIME type handlers pointing to the handler detailsi.
 */
function file_get_mime_handlers() {
  static $mime_handlers = array();
  if (!$mime_handlers) {
    $handlers = array();
    foreach (module_implements('mime_handlers') as $module) {
      if (($types = module_invoke($module, 'mime_handlers')) && is_array($types)) {
        foreach ($types as $handler => $data) {
          $types[$handler]['module'] = $module;
          if (!isset($data['weight'])) $types[$handler]['weight'] = 0;
        }
        $handlers = array_merge_recursive($handlers, $types);
      }
    }
    foreach ($handlers as $handler => $data) {
      $handlers_sort[$handler] = $data['weight'];
    }
    $default_handlers = variable_get('file_handlers', array());
    $default_handlers_sort = array();
    if (is_array($default_handlers)) {
      foreach ($default_handlers as $handler => $data) {
        $default_handlers_sort[$handler] = $data['weight'];
      }
    }
    $handlers_sorted = array_merge($handlers_sort, $default_handlers_sort);
    asort($handlers_sorted);
    foreach ($handlers_sorted as $handler => $weight) {
      if ($handlers[$handler]) {
        $mime_handlers[$handler] = array(
          'name' => $handlers[$handler]['name'],
          'dimensions' => $handlers[$handler]['dimensions'] ? $handlers[$handler]['dimensions'] : '0x0',
          'weight' => $weight,
          'enabled' => isset($default_handlers[$handler]['enabled']) ? $default_handlers[$handler]['enabled'] : (isset($handlers[$handler]['enabled']) ? $handlers[$handler]['enabled'] : 1),
          'module' => $handlers[$handler]['module'],
        );
      }
    }
  }
  return $mime_handlers;
}

//////////////////////////////////////////////////////////////////////////////
// File metadata extraction

/**
 * This function executes all hook_metadata_info() hooks in the format modules
 * and creates an array of the metadata info.
 *
 * @return
 *   A structured array of the metadata information.
 */
function file_get_metadata_info() {
  static $result = array();
  if (!$result) {
    $result['dc:format'] = t('MIME type');
    $result['dc:extent'] = t('Size in Bytes');
    foreach (module_implements('metadata_info') as $module) {
      $info = module_invoke($module, 'metadata_info');
      if (is_array($info)) {
        $result = array_merge($result, $info);
      }
    }
  }
  return $result;
}

/**
 * This function executes all hook_metadata_parse() hooks in the format modules
 * and creates an array of the file's metadata.
 *
 * @param $filename
 *   A name of the file.
 * @param $mimetype
 *   A file's MIME type.
 *
 * @return
 *   A structured array of the metadata information.
 */
function file_metadata_extract($filename, $mimetype) {
  $result = array(
    'dc:format' => array($mimetype),
    'dc:extent' => array(filesize($filename)),
  );
  foreach (module_implements('metadata_parse') as $module) {
    $metadata = module_invoke($module, 'metadata_parse', $filename, $mimetype);
    if (is_array($metadata)) {
      $result = array_merge($result, $metadata);
    }
  }
  return $result;
}

/**
 * Extracts file's metadata information.
 *
 * @param $file
 *   A file object as returned from file_node_load().
 *
 * @return
 *   Structured array pointing to the structored array of metadata information.
 */
function file_get_metadata($file) {
  $metadata = file_metadata_extract(bitcache_get(file_get_hash($file))->path, $file->filemime);
  return array($file->uri => $metadata);
}

/**
 * Generates and saves the preview for the file based on the MIME type.
 *
 * @param $file
 *   A file object as returned from file_node_load().
 * @param $cron
 *   A flag to show that function is called from the cron.
 *
 * @return
 *   TRUE if the preview was generated, FALSE otherwise.
 */
function file_generate_previews($file, $cron = NULL) {
  $result = TRUE;

  $previews = array();

  if (!empty($file->uri)) {
    // Check if file exists in the bitcache.
    if (!bitcache_exists(file_get_hash($file->uri))) {
      watchdog('file', 'The blob was not found in the bitcache for the uri=%uri.', array('%uri' => $file->uri), WATCHDOG_ERROR);
      return FALSE;
    }

    //Extract metadata if it is missing and save to RDF.
    if (!rdf_exists($file->uri)) {
      if (!rdf_insert_all(rdf_denormalize(file_get_metadata($file)), array('repository' => 'file'))) {
        watchdog('file', 'The RDF data was not saved for the node with nid=%nid, vid=%vid.', array('%nid' => $file->nid, '%vid' => $file->vid), WATCHDOG_ERROR, l(t('edit'), 'node/'. $file->nid));
        return FALSE;
      }
    }

    // Find generated previews for the file.
    if ($generated = rdf_normalize(rdf_query(NULL, rdf_qname_to_uri('dc:source'), $file->uri))) {
      foreach ($generated as $uri => $data) {
        $item = rdf_normalize(rdf_query($uri, NULL, NULL));
        if (!bitcache_exists(file_get_hash($uri)) || !$item[$uri][rdf_qname_to_uri('dc:extent')][0]->value) {
          // The blob does not exist in the bitcache or the size is zero.
          // We delete the generated entry reference in the RDF
          // and the file will be converted again.
          if (!rdf_delete($uri, NULL, NULL, array('repository' => 'file'))) {
            watchdog('file', 'The RDF data was not deleted for the uri=%uri.', array('%uri' => $uri), WATCHDOG_ERROR);
          }
        }
        if ($mime = $item[$uri][rdf_qname_to_uri('dc:format')][0]) {
          // Preview is already generated.
          // All derived files should have a MIME type set.
          $previews[$mime][] = $item[$uri][rdf_qname_to_uri('dc:creator')][0];
        }
      }
    }
  }

  // Create array of enabled handlers.
  $handlers_enabled = array();
  foreach (file_get_mime_handlers() as $handler => $data) {
    if ($data['enabled'] != 0) {
      $handlers_enabled[] = $handler;
    }
  }

  // Find all preview generation handlers for the MIME type and check if the preview is already generated.
  $mime_types = file_get_mime_types();
  if (is_array($mime_types[$file->filemime]['handlers'])) {
    foreach (array_diff($mime_types[$file->filemime]['handlers'], is_array($previews[$file->filemime]) ? $previews[$file->filemime] : array()) as $handler) {
      if (in_array($handler, $handlers_enabled) && function_exists($full_handler = $handler .'_generate')) {
        $result = $full_handler($file) && $result;
      }
    }
  }

  // Perform file conversions and generate previews.
  if (module_exists('file_convert')) {
    // Check size limit for conversion.
    // There is no restrictions for the cron run.
    if (!isset($cron) && FILE_CONVERT_LIMIT_SIZE > 0 && $file->filesize > FILE_CONVERT_LIMIT_SIZE * 1024 * 1024) {
      return $result;
    }

    $mime_converters = file_get_mime_converters();
    if (is_array($mime_converters[$file->filemime])) {
      foreach ($mime_converters[$file->filemime] as $to => $converter) {
        if ($converter['handlers']) {
          // Check if we already have a preview generated and the handler is enabled.
          $converter['handlers'] = array_intersect($handlers_enabled, array_diff($converter['handlers'], is_array($previews[$to]) ? $previews[$to] : array()));
        }
        if ($converter['enabled']) {
          // Need to generate a preview if there is no converted
          // file of the targer MIME or if we have not used MIME
          // handler of the target MIME.
          if ((!empty($converter['handlers']) || !is_array($previews[$to])) && $converted = file_convert($file, $converter['pipeline'])) {
            $file_converted = (object)array('uri' => $file->uri, 'filepath' => $converted, 'filename' => $file->filename, 'filemime' => $to, 'uri' => $file->uri, 'converted' => TRUE);
            if (!empty($converter['handlers'])) {
              // Apply generate handlers.
              foreach ($converter['handlers'] as $handler) {
                if (function_exists($full_handler = $handler .'_generate')) {
                  $result = $full_handler($file_converted) && $result;
                }
              }
            }
            else {
              // Save the file to the bitcache and RDF.
              $result = file_data_save($file_converted) && $result;
            }
            unlink($converted);
            // Flag the wile that conversion took place.
            // Needed for a cron to calculate conversions.
            $file->convert = $file->convert ? $file->convert + 1 : 1;
          }
        }
      }
    }
  }
  return $result;
}

/**
 * Converts and saves a secondary file derivative.
 *
 * @param
 *   Module name, used for the watchdog.
 * @param
 *   A file object.
 * @param
 *   A mime tipe to convert to.
 * @param
 *   A file generate handler.
 */
function file_generate_secondary_previews($module, $file, $mime_to, $handler) {
  $handler_generate = $handler .'_generate';
  $mime_handlers = file_get_mime_handlers();
  if ($mime_handlers[$handler]['enabled'] == 1 && module_exists('file_convert') && function_exists($handler_generate)) {
    $mime_converters = file_get_mime_converters();
    $converter = $mime_converters[$file->filemime][$mime_to];
    if (is_array($converter)) {
      if ($converter['enabled'] && in_array($handler, $converter['handlers'])) {
        // Convert the file.
        if (!$converted = file_convert($file, $converter['pipeline'])) {
          watchdog($module, 'A preview of type %mime was not created for the file %file, %uri. File conversion failed.', array('%mime' => $mime_to, '%file' => $file->filename, '%uri' => $file->uri), WATCHDOG_ERROR);
        }
        else {
          $file->filepath = $converted;
          $file->filemime = $mime_to;

          if (!$handler_generate($file)) {
            watchdog($module, 'A preview of type %mime was not created for the file %file, %uri.', array('%mime' => $mime_to, '%file' => $file->filename, '%uri' => $file->uri), WATCHDOG_ERROR);
          }

          // Remove temporal file
          unlink($converted);
        }
      }
    }
  }
}

//////////////////////////////////////////////////////////////////////////////
// File rendering & theming

/**
 * Generates an array of all  preview handlers for the file pointing.
 * to the previews' bitcache URIs.
 *
 * @param $file
 *   A file object as returned from file_node_load().
 *
 * @return
 *   An array of the preview handlers pointing to the bitcache URIs.
 */
function file_handlers_for($file) {
  $mime_types = file_get_mime_types();
  $result = array();

  // Find generated previews for the file.
  $previews = array();
  if (!empty($file->uri)) {
    if ($generated = rdf_normalize(rdf_query(NULL, NULL, $file->uri))) {
      foreach ($generated as $uri => $data) {
        $item = rdf_normalize(rdf_query($uri, NULL, NULL));
        if ($handler = $item[$uri][rdf_qname_to_uri('dc:creator')][0]) {
          $result[$handler] = array(
            'uri' => $uri,
            'type' => $item[$uri][rdf_qname_to_uri('dc:format')][0],
            'size' => $item[$uri][rdf_qname_to_uri('dc:extent')][0]->value,
          );
        }
      }
    }
  }

  // Display a file when preview is absent.
  if (is_array($mime_types[$file->type]['handlers'])) {
    foreach (array_diff($mime_types[$file->type]['handlers'], array_keys($result)) as $handler) {
      $result[$handler] = array(
        'uri' => $file->uri,
        'type' => $file->type,
        'size' => $file->size,
      );
    }
  }

  return $result;
}

/**
 * Generates an array of all file's generated derivatives URIs pointing
 * to the data of that derivative.
 *
 * @param $file
 *   A file object as returned from file_node_load().
 *
 * @return
 *   An array of the generated derivatives URIs pointing to derivatives' data.
 */
function file_generated_for($file) {
  $result = array();
  // Find generated previews for the file.
  if (!empty($file->uri)) {
    if ($generated = rdf_normalize(rdf_query(NULL, NULL, $file->uri))) {
      foreach ($generated as $uri => $data) {
        $item = rdf_normalize(rdf_query($uri, NULL, NULL));
        $result[$uri] = array(
          'type' => $item[$uri][rdf_qname_to_uri('dc:format')][0],
          'size' => $item[$uri][rdf_qname_to_uri('dc:extent')][0]->value,
          'handler' => $item[$uri][rdf_qname_to_uri('dc:creator')][0],
        );
      }
    }
  }

  return $result;
}

/**
 * Returns the other file formats section.
 *
 * @param $file
 *   A file object as returned from file_node_load().
 * @param $id
 *   ID of the preview in case there are several previews on the same page.
 *
 * @return
 *   The HTML section of the file formats.
 */
function file_render_generated($file, $id = 0) {
  $formats = array();
  $file_uris = file_generated_for($file);
  foreach ($file_uris as $uri => $data) {
    if ($file->uri != $uri) {
      $data['uri'] = bitcache_resolve_uri($uri, array('absolute' => TRUE));
      $data['hash'] = file_get_hash($uri);
      $data['description'] = file_mime_description_for($data['type']);
      $formats[] = $data;
    }
  }

  return theme('file_generated', array('formats' => $formats, 'nid' => $file->nid, 'vid' => $file->vid, 'name' => check_plain($file->name), 'id' => $id));
}

/**
 * Returns the controls section of the all file previews. The actual
 * preview is rendered later via the AHAH call.
 *
 * @param $file
 *   A file object as returned from file_node_load().
 * @param $id
 *   ID of the preview in case there are several previews on the same page.
 *
 * @return
 *   The HTML section of the file preview controls.
 */
function file_render_previews($file, $id = 0) {
  $previews = array();
  $file_handlers = file_handlers_for($file);
  foreach (file_get_mime_handlers() as $handler => $data) {
    if (array_key_exists($handler, $file_handlers) && $data['enabled'] == 1 && function_exists($handler .'_render')) {
      $form = file_render_preview_for($file, $handler, $id);
      $previews[] = array('handler' => $handler, 'name' => $data['name']);
    }
  }

  return theme('file_previews', array('previews' => $previews, 'nid' => $file->nid, 'name' => check_plain($file->name), 'id' => $id));
}

/**
 * Returns the form element which triggers the AHAH call to download
 * the file preview for particular handler.
 * This function ensures that AHAH extentions are added to the page.
 *
 * @param $file
 *   A file object as returned from file_node_load().
 * @param $handler
 *   The file preview handler.
 * @param $id
 *   ID of the preview in case there are several previews on the same page.
 *
 * @return
 *   The form element.
 */
function file_render_preview_for($file, $handler, $id) {
  $form['preview'] = array(
    '#type' => 'button',
    '#id' => $handler .'_'. $id,
    '#ahah' => array(
      'path' => 'file_preview/'. $handler .'/'. file_get_hash($file->uri) .'/'. $file->vid,
      'wrapper' => 'file_preview_container_'. $id,
      'method' => 'replace',
      'effect' => 'slide',
    ),
  );

  form_expand_ahah($form['preview']);
  return $form;
}

/**
 * Returns the HTML or JSON data of the rendered HTML preview of the file.
 * This functions is called from the file view page via AHAH call.
 *
 * @param $handler
 *   A preview handler associated with the file.
 * @param $hash
 *   A blob hash.
 * @param $node
 *   A populated node object.
 * @param $type
 *   A return type. 'html' will output the html page, other values will result in javascript.
 *
 * @return
 *   A  or HTML or JSON data of the rendered HTML of the file preview.
 */
function file_wrapper($handler, $uri, $node = NULL, $type = NULL) {
  if (array_key_exists($handler, file_get_mime_handlers()) && function_exists($handler .'_render')) {
    $handler_full = $handler .'_render';
    $output = $handler_full($uri, is_object($node) ? array('nid' => $node->nid, 'vid' => $node->vid) : array());
  }
  if ($type == 'html') {
    return $output;
  }
  drupal_json(array('status' => TRUE, 'data' => theme('status_messages') . $output));
}

/**
 * Returns the HTML data of the rendered HTML preview of the file.
 *
 * @param $node
 *   A node object.
 *
 * @return
 *   A HTML data of the rendered HTML of the file preview.
 */
function file_wrapper_html($node, $handler) {
  // Access control is done at the menu layer.
  $handlers = file_handlers_for($node->file);
  return file_wrapper($handler, $handlers[$handler]['uri'], $node, 'html');
}

/**
 * Returns the Javascript data of the rendered HTML preview of the file.
 *
 * @param $handler
 *   A preview handler associated with the file.
 * @param $hash
 *   A blob hash.
 *
 * @return
 *   A JAvascript data of the rendered HTML of the file preview.
 */
function file_wrapper_ahah($handler, $hash, $vid) {
  // Access control is not done at menu layer.
  if ($node = file_bitcache_access($hash, $vid)) {
    $handlers = file_handlers_for((object)array('uri' => 'bitcache://'. $hash));
    if (empty($handlers[$handler])) {
      $handlers[$handler]['uri'] = 'bitcache://'. $hash;
    }
    return file_wrapper($handler, $handlers[$handler]['uri'], is_object($node) ? $node : '', 'js');
  }
  //header("HTTP/1.1 403 Forbidden");
  drupal_access_denied();
}

//////////////////////////////////////////////////////////////////////////////
// BITCACHE

/**
 * Implementation of hook_bitcache().
 */
function file_bitcache($op, $id, $stream = NULL) {
  $node = file_bitcache_access($id, $_GET['vid']);

  switch ($op) {
    case 'access':
      return is_object($node) ? TRUE : FALSE;
    case 'download':
      // Determine additional HTTP headers for the bitstream download
      $disposition = $_GET['disposition'] ? $_GET['disposition'] : 'attachment';
      if ($node->nid) {
        $counter = $disposition == 'inline' ? 'views' : 'downloads';
        db_query("UPDATE {file_nodes} f SET $counter = $counter + 1 WHERE f.vid = %d", $node->vid);
      }
      foreach (file_mime_extensions_for(rdf_value('bitcache://'. $id, rdf_qname_to_uri('dc:format'), NULL)) as $extension) {
        if (preg_match('/\.'. $extension .'$/', drupal_strtolower($node->title))) {
          $extension_correct = TRUE;
          break;
        }
      }
      $ext = !empty($extensions) && !$extension_correct ? '.'. reset($extensions) : '';
      $title = $node->title ? $node->title : 'file';
      return array(
        'Content-Disposition' => $disposition .'; filename="'. $title . $ext .'"',
        'Last-Modified' => gmdate('D, d M Y H:i:s', is_object($node) ? $node->changed : time()) .' GMT',
      );
      break;
  }
}

/**
 * Returns the hash of the file stored in the bitcache.
 *
 * @param $file
 *   A file object as returned from file_node_load().
 *
 * @return
 *   A file hash.
 */
function file_get_hash(&$file) {
  return is_object($file) ? drupal_substr($file->uri, -FILE_HASH_SIZE) : drupal_substr($file, -FILE_HASH_SIZE);
}

/**
 * Checks if the user has rights to access bitsream.
 *
 * @param $id
 *   Bitstream hash.
 * @param $vid
 *   A node version ID.
 *
 * @return
 *   A node object or FALSE if access is not granted.
 */
function file_bitcache_access($id, $vid) {
  $uri = 'bitcache://'. $id;
  $parent_uri = rdf_value('bitcache://'. $id, rdf_qname_to_uri('dc:source'));

  if (isset($vid)) {
    $row = db_fetch_object(db_query("SELECT f.nid, f.vid FROM {file_nodes} f WHERE f.uri = '%s' AND f.vid = %d", $uri, $vid));
    if (!$row) {
      $row = db_fetch_object(db_query("SELECT f.nid, f.vid FROM {file_nodes} f WHERE f.uri = '%s' AND f.vid = %d", $parent_uri, $vid));
    }
    if ($row) {
      $node = node_load($row->nid, $row->vid);
      return node_access('view', $node) ? $node : FALSE;
    }
  }

  $files = array_merge(
    is_object($_SESSION['file_preview_file']) ? array($_SESSION['file_preview_file']) : array(),
    is_array($_SESSION['file_attach_files']) ? $_SESSION['file_attach_files'] : array(),
    is_array($_SESSION['file_cck_files']) ? $_SESSION['file_cck_files'] : array()
  );

  foreach ($files as $file) {
    if (is_object($file) && $file->uri == $parent_uri || $file->uri == $uri) {
      return (object)array('title' => $file->name);
    }
  }

  return FALSE;
}

//////////////////////////////////////////////////////////////////////////////
// FILE UPLOAD VALIDATORS

/**
 * Gets all file upload validators.
 *
 * @return
 *   An array of validate functions.
 */
function file_get_validators() {
  static $validators = array();
  if (!$validators) {
    foreach (module_implements('file_validate') as $module) {
      $validators[$module .'_file_validate'] = array();
    }
    $validators = is_array($validators) ? $validators : array();
  }
  return $validators;
}

//////////////////////////////////////////////////////////////////////////////
// FILE QUOTAS

/**
 * Calculates total size of all user's files.
 *
 * @param
 *   A user ID.
 * @param
 *   A flag which shows if converted files should be included in the calculations.
 *
 * @return
 *   Size in bytes.
 */
function file_get_files_size($uid, $converted = 1) {
  $size = 0;
  $result = db_query('SELECT fn.size FROM {file_nodes} fn INNER JOIN {node} n ON fn.nid = n.nid WHERE n.uid = %d', $uid);
  while ($row = db_fetch_object($result)) {
    $size += $row->size;

    if ($converted && $generated = rdf_normalize(rdf_query(NULL, rdf_qname_to_uri('dc:source'), $file->uri))) {
      foreach ($generated as $uri => $data) {
        $size += rdf_value($uri, rdf_qname_to_uri('dc:extent'))->value;
      }
    }
  }
  return $size;
}

//////////////////////////////////////////////////////////////////////////////
// AUXILIARY FUNCTIONS

/**
 * Compares data with the template array and gets the matching entry.
 * Is needed by format modules.
 *
 * @param $data
 *   Data array.
 * @param $template
 *   Template array.
 *
 * @return
 *   A data which was matched in the template.
 */
function file_get_recursive($data, $template) {
  while (is_array($data) && is_array($template)) {
    $key = key($template);
    $template = $template[$key];
    $data = $data[$key];
  }
  return isset($data) && !is_array($data) ? $data : NULL;
}

/**
 * Finds an image of a given resolution of the file if it is available.
 *
 * @param $file
 *   A file object.
 * @param $handler
 *   An image handler.
 * @param $dimensions
 *   An array with max width and height of the image.
 *
 * @return
 *   A HTML output of the image or NULL if it is not available.
 */
function file_get_image($file, $handler, $dimensions) {
  $image = NULL;
  if ($generated = rdf_normalize(rdf_query(NULL, rdf_qname_to_uri('dc:source'), $file->uri))) {
    foreach ($generated as $uri => $data) {
      if (rdf_value($uri, rdf_qname_to_uri('dc:creator')) == $handler) {
        // We gave a generated image.
        $image = file_get_hash($uri);
        $width = rdf_value($uri, rdf_qname_to_uri('exif:width'))->value;
        $height = rdf_value($uri, rdf_qname_to_uri('exif:height'))->value;
        break;
      }
    }
  }
  if (!$image && preg_match('/^image\//', $file->type)) {
    $width = rdf_value($file->uri, rdf_qname_to_uri('exif:width'))->value;
    $height = rdf_value($file->uri, rdf_qname_to_uri('exif:height'))->value;
    if ($width < $dimensions[0] && $height < $dimensions[1]) {
      $image = file_get_hash($file->uri);
    }
  }
  return $image ? '<img width="'. $width .'" height="'. $height .'" src="'. url('bitcache/'. $image, array('query' => array('vid' => $file->vid, 'disposition' => 'inline'))) .'">' : NULL;
}

