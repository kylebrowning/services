<?php

/**
 * @file
 * File resource.
 */

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
          array(
            'name'         => 'file_contents',
            'type'         => 'int',
            'description'  => t('To return file contents or not.'),
            'source'       => 'data',
            'optional'     => FALSE,
            'default value' => TRUE,
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

  // Make sure we create new file.
  $file->fid = NULL;

  // Get the directory name for the location of the file:
  if (isset($file->filename) && !isset($file->filepath)) {
    $file->filepath = file_default_scheme() . '://' . $file->filename;
  }
  $dir = file_default_scheme() . '://';
  // Build the destination folder tree if it doesn't already exists.
  if (!file_prepare_directory($dir, FILE_CREATE_DIRECTORY)) {
    return services_error("Could not create destination directory for file.", 500);
  }

  $file->filemime = file_get_mimetype($file->filename);

  // Rename potentially executable files, to help prevent exploits.
  if (preg_match('/\.(php|pl|py|cgi|asp|js)$/i', $file->filename) && (drupal_substr($file->filename, -4) != '.txt')) {
    $file->filemime = 'text/plain';
    $file->filepath .= '.txt';
    $file->filename .= '.txt';
  }

  // Write the file
  if (!$file = file_save_data(base64_decode($file->file), $file->filepath)) {
    return services_error("Could not write file to destination", 500);
  }

  // Required to be able to reference this file.
  file_usage_add($file, 'services', 'files', $file->fid);

  return array(
    'fid' => $file->fid,
    'uri' => services_resource_uri(array('file', $file->fid)),
  );
}

/**
 * Get a given file
 *
 * @param $fid
 *   Number. File ID
 * @param $include_file_contents
 *   Bool Whether or not to include the base64_encoded version of the file.
 * @return
 *   The file
 */
function _file_resource_retrieve($fid, $include_file_contents = TRUE) {
  if ($file = file_load($fid)) {
    $filepath = $file->uri;
    if ($include_file_contents) {
      $file->file = base64_encode(file_get_contents($filepath));
    }
    return $file;
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
  if ($file = file_load($fid)) {
    file_usage_delete($file, 'services');
    return file_delete($file);
  }
  return FALSE;
}

/**
 * Access check callback for file controllers.
 */
function _file_resource_access($op = 'view', $args = array()) {
  global $user;

  if ($op=='create') {
    $file = (object)$args[0];
  }
  else {
    $file = file_load($args[0]);
  }
  if (empty($file)) {
    return services_error('There is no file with the given ID: ' . $args[0], 406);
  }
  switch ($op) {
    case 'view':
      if (user_access('get any binary files')) {
        return TRUE;
      }
      return $file->uid == $user->uid && user_access('get own binary files');
      break;
    case 'create':
    case 'delete':
      return $file->uid == $user->uid && user_access('save file information');
      break;
  }

  return FALSE;
}

function _file_resource_node_access($op = 'view', $args = array()) {
  global $user;
  if (user_access('get any binary files')) {
    return TRUE;
  }
  elseif ($node = node_load($args[0])) {
    return $node->uid == $user->uid && user_access('get own binary files');
  }
  return FALSE;
}

