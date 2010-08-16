<?php
// $Id$

/**
 * THERE SHOULD BE NO UPDATE!!!
 * Drupal doesn't allow updating or replacing a file in the files table.
 * If you need to, create a new file and remove the old file.
 */
function _file_resource_definition() {
  return array(
    'file' => array(

      'file' => array('type' => 'inc', 'module' => 'services', 'name' => 'resources/file_resource'),

      'create' => array(
        'help' => 'Creates a file',
        'callback' => '_file_resource_create',
        'access callback' => '_file_resource_access',
        'access arguments' => array('create'),
        'access arguments append' => TRUE,
        'args' => array(
          array(
            'name' => 'file',
            'type' => 'struct',
            'description'    => t('An object representing a file.'),
            'source' => 'data',
            'optional' => FALSE,
          ),
        ),
      ),

      'retrieve' => array(
        'help' => 'Retrieves a file',
        'callback' => '_file_resource_retrieve',
        'access callback' => '_file_resource_access',
        'access arguments' => array('view'),
        'access arguments append' => TRUE,
        'args' => array(
          array(
            'name' => 'fid',
            'type' => 'int',
            'description' => 'The fid of the file to retrieve.',
            'source' => array('path' => '0'),
            'optional' => FALSE,
          ),
        ),
      ),

      'delete' => array(
        'help' => 'Deletes a file',
        'callback' => '_file_resource_delete',
        'access callback' => '_file_resource_access',
        'access arguments' => array('delete'),
        'access arguments append' => TRUE,
        'args' => array(
          array(
            'name' => 'cid',
            'type' => 'int',
            'description' => 'The id of the file to delete',
            'source' => array('path' => '0'),
            'optional' => FALSE,
          ),
        ),
      ),
      'actions' => array(
        'nodeFiles' => array(
          'help'   => t('This method returns the files on a given node.'),
          'access callback' => '_file_resource_node_access',
          'access arguments' => array('view'),
          'access arguments append' => TRUE,
          'callback' => '_file_resource_load_node_files',
          'args'     => array(
            array(
              'name'         => 'nid',
              'type'         => 'int',
              'description'  => t('The node id to load files for.'),
              'source'       => array('path' => '0'),
              'optional'     => FALSE,
            ),
          ),
        ),
      ),
    ),
  );
}

/**
 * Adds a new file and returns the fid.
 *
 * @param $file
 *   An object as representing the file.
 * @return
 *   Unique identifier for the file (fid) or errors if there was a problem.
 */
function _file_resource_create($file) {
  global $user;
  $file = (object) $file;

  // If the file data is empty then bail
  if (!isset($file->file)) {
    return FALSE;
  }

  // If the submitted file is an update, then set the update parameter for
  // drupal_write_record(), indicating such. Otherwise we can just pass the
  // object in and it will be treated as an insert.
  $update = array();
  if (!empty($file->fid)) {
    $update = 'fid';
  }

  // Build the list of non-munged extensions.
  // @todo: this should not be here. we need to figure out the right place.
  // @todo: also isn't that repeated variable get a waste? I mean, I guess it
  //        is cached but still it is pretty ugly.
  $extensions = '';
  foreach ($user->roles as $rid => $name) {
    $extensions .= ' '. variable_get("upload_extensions_$rid",
    variable_get('upload_extensions_default', 'jpg jpeg gif png txt html doc xls pdf ppt pps odt ods odp'));
  }

  // Get the directory name for the location of the file:
  $dir = dirname($file->filepath);
  // Build the destination folder tree if it doesn't already exists.
  if (!file_check_directory($dir, FILE_CREATE_DIRECTORY)) {
    return services_error("Could not create destination directory for file.");
  }

  // Update file object as necessary
  $file->filepath = file_destination(file_create_path($file->filepath), FILE_EXISTS_RENAME);
  $file->filename = file_munge_filename(trim(basename($file->filepath), '.'), $extensions, TRUE);
  $file->filemime = file_get_mimetype($file->filename);

  // Rename potentially executable files, to help prevent exploits.
  if (preg_match('/\.(php|pl|py|cgi|asp|js)$/i', $file->filename) && (substr($file->filename, -4) != '.txt')) {
    $file->filemime = 'text/plain';
    $file->filepath .= '.txt';
    $file->filename .= '.txt';
  }

  // If the destination is not provided, or is not writable, error our
  if (empty($file->filepath) || file_check_path($file->filepath) === FALSE) {
    return services_error("Destintion directory does not exist or is not writeable.");
  }

  //The filepath that ends up in the node must contain the filename
  $file->filepath .= '/'. $file->filename;

  // Write the file
  if (!file_save_data(base64_decode($file->file), $file->filepath)) {
    return services_error("Could not write file to destination");
  }

  // If we made it this far it's safe to record this file in the database.
  drupal_write_record('files', $file, $update);

  // hook_file_insert() requires an object
  if (empty($update)) {
    foreach (module_implements('file_insert') as $module) {
      $function =  $module .'_file_insert';
      $function($file);
    }
  }

  return (object)array(
    'cid' => $file->fid,
    'uri' => services_resource_uri(array('file', $fid)),
  );
}

/**
 * Get a given file
 *
 * @param $fid
 *   Number. File ID
 * @return
 *   The file
 */
function _file_resource_retrieve($fid) {
  if ($file = db_fetch_array(db_query('SELECT * FROM {files} WHERE fid = %d', $fid))) {
    $filepath = file_create_path($file['filepath']);
    $binaryfile = fopen($filepath, 'rb');
    $file['file'] = base64_encode(fread($binaryfile, filesize($filepath)));
    fclose($binaryfile);
    return $file;
  }
  else {
    return services_error(t('There is no file with the given ID.'));
  }
}

/**
 * Generates an array of base64 encoded files attached to a node
 *
 * @param $nid
 *   Number. Node ID
 * @return
 *   Array. A list of all files from the given node
 */
function _file_resource_load_node_files($nid) {
  $node = node_load($nid);
  if (isset($node->files)) {
    $files = array();
    foreach ($node->files as $file) {
      // Rebuild the files array so it only contains files we know we're allowed to list
      if ($file->list) {
        $files[] = $file;
      }
    }
    if (count($files) > 0) {
      $send = array();
      foreach ($files as $file) {
        $file = array_shift($files);
        $filepath = file_create_path($file->filepath);
        $binaryfile = fopen($filepath, 'rb');
        $send[$file->fid] = array(
          'file'      => base64_encode(fread($binaryfile, filesize($filepath))),
          'filename'  => $file->filename,
          'uid'       => $file->uid,
          'filemime'  => $file->filemime,
          'filesize'  => $file->filesize,
          'status'    => $file->status,
          'timestamp' => $file->timestamp
        );
        fclose($binaryfile);
      }
    }
    return $send;
  }
  else {
    return services_error(t('There are no files on given node.'));
  }
}

/**
 * Delete a file.
 *
 * @param $fid
 *   Unique identifier of the file to delete.
 * @return bool
 *   Whether or not the delete was successful.
 */
function _file_resource_delete($fid) {
  $file = db_fetch_array(db_query('SELECT * FROM {files} WHERE fid = %d', $fid));

  return file_delete($file['filepath']);
}
