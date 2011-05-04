<?php

function _comment_resource_definition() {
  if (!module_exists('comment')) {
    return array();
  }

  return array(
    'comment' => array(
      'create' => array(
        'help' => 'Creates a comment',
        'file' => array('type' => 'inc', 'module' => 'services', 'name' => 'resources/comment_resource'),
        'callback' => '_comment_resource_create',
        'access callback' => '_comment_resource_access',
        'access arguments' => array('create'),
        'access arguments append' => TRUE,
        'args' => array(
          array(
            'name' => 'comment',
            'type' => 'array',
            'description' => 'The comment object',
            'source' => 'data',
            'optional' => FALSE,
          ),
        ),
      ),

      'retrieve' => array(
        'help' => 'Retrieves a comment',
        'file' => array('type' => 'inc', 'module' => 'services', 'name' => 'resources/comment_resource'),
        'callback' => '_comment_resource_retrieve',
        'access callback' => '_comment_resource_access',
        'access arguments' => array('view'),
        'access arguments append' => TRUE,
        'args' => array(
          array(
            'name' => 'cid',
            'type' => 'int',
            'description' => 'The cid of the comment to retrieve.',
            'source' => array('path' => '0'),
            'optional' => FALSE,
          ),
        ),
      ),

      'update' => array(
        'help' => 'Updates a comment',
        'file' => array('type' => 'inc', 'module' => 'services', 'name' => 'resources/comment_resource'),
        'callback' => '_comment_resource_update',
        'access callback' => '_comment_resource_access',
        'access arguments' => array('edit'),
        'access arguments append' => TRUE,
        'args' => array(
          array(
            'name' => 'cid',
            'optional' => FALSE,
            'source' => array('path' => 0),
            'type' => 'int',
            'description' => 'The unique identifier for this comment.',
          ),
          array(
            'name' => 'data',
            'type' => 'array',
            'description' => 'The comment array with updated information',
            'source' => 'data',
            'optional' => FALSE,
          ),
        ),
      ),

      'delete' => array(
        'help' => 'Deletes a comment',
        'file' => array('type' => 'inc', 'module' => 'services', 'name' => 'resources/comment_resource'),
        'callback' => '_comment_resource_delete',
        'access callback' => '_comment_resource_access',
        'access arguments' => array('edit'),
        'access arguments append' => TRUE,
        'args' => array(
          array(
            'name' => 'cid',
            'type' => 'int',
            'description' => 'The id of the comment to delete',
            'source' => array('path' => '0'),
            'optional' => FALSE,
          ),
        ),
      ),
      'actions' => array(
        'loadNodeComments' => array(
          'file' => array('type' => 'inc', 'module' => 'services', 'name' => 'resources/comment_resource'),
          'help'   => t('This method returns the number of new comments on a given node since a given timestamp.'),
          'access callback' => '_comment_resource_access',
          'access arguments' => array('view'),
          'callback' => '_comment_resource_load_node_comments',
          'args'     => array(
            array(
              'name'         => 'nid',
              'type'         => 'int',
              'description'  => t('The node id to load comments for.'),
              'source'       => 'data',
              'optional'     => FALSE,
            ),
            array(
              'name'         => 'count',
              'type'         => 'int',
              'description'  => t('Number of comments to load.'),
              'source'       => 'data',
              'optional'     => TRUE,
              'default value'=> 10,
            ),
            array(
              'name'         => 'start',
              'type'         => 'int',
              'description'  => t('If count is set to non-zero value, you can pass also non-zero value for start. For example to get comments from 5 to 15, pass count=10 and start=5.'),
              'source'       => 'data',
              'optional'     => TRUE,
              'default value'=> 0,
            ),
          ),
        ),
        'countAll' => array(
          'file' => array('type' => 'inc', 'module' => 'services', 'name' => 'resources/comment_resource'),
          'help'   => t('This method returns the number of comments on a given node.'),
          'access callback' => '_comment_resource_access',
          'access arguments' => array('view'),
          'callback' => '_comment_resource_count_all',
          'args'     => array(
            array(
              'name'         => 'nid',
              'type'         => 'int',
              'description'  => t('The node id to count all comments.'),
              'source'       => 'data',
              'optional'     => FALSE,
            ),
          ),
        ),
        'countNew' => array(
          'file' => array('type' => 'inc', 'module' => 'services', 'name' => 'resources/comment_resource'),
          'help'   => t('This method returns the number of new comments on a given node since a given timestamp.'),
          'access callback' => '_comment_resource_access',
          'access arguments' => array('view'),
          'callback' => '_comment_resource_count_new',
          'args'     => array(
            array(
              'name'          => 'nid',
              'type'          => 'int',
              'description'   => t('The node id to load comments for.'),
              'source'        => 'data',
              'optional'      => FALSE,
            ),
            array(
              'name'         => 'since',
              'type'         => 'int',
              'optional'     => TRUE,
              'description'  => t('Timestamp to count from (defaults to time of last user acces to node).'),
              'source'        => 'data',
              'optional'      => TRUE,
              'default value' => 0,
            ),
          ),
        ),
      ),
    ),
  );
}

/**
 * Adds a new comment to a node and returns the cid.
 *
 * @param $comment
 *   An object as would be returned from comment_load().
 * @return
 *   Unique identifier for the comment (cid) or errors if there was a problem.
 */

function _comment_resource_create($comment) {

  // Setup form_state
  $form_state = array();
  $form_state['values'] = $comment;
  $form_state['values']['op'] = t('Save');

  $comment_empty = array(
    'nid' => $comment['nid'],
    'cid' => NULL,
  );

  $ret = drupal_execute('comment_form', $form_state, $comment_empty);

  if ($errors = form_get_errors()) {
    return services_error(implode(" ", $errors), 406, array('form_errors' => $errors));
  }

  // Load latest created comment.
  $recent_comments = comment_get_recent(1);
  $cid = $recent_comments[0]->cid;

  return array(
    'cid' => $cid,
    'uri' => services_resource_uri(array('comment', $cid)),
  );
}

/**
 * Returns a specified comment
 *
 * @param $cid
 *   Unique identifier for the specified comment
 * @return
 *   The comment object
 */
function _comment_resource_retrieve($cid) {
  return _comment_load($cid);
}

/**
 * Updates a comment and returns the cid.
 *
 * @param $cid
 *   Unique identifier for this comment.
 * @param $comment
 *   An object as would be returned from comment_load().
 * @return
 *   Unique identifier for the comment (cid) or FALSE if there was a problem.
 */
function _comment_resource_update($cid, $comment) {
  $comment['cid'] = $cid;

  $old_comment = (array) _comment_load($cid);
  if (empty($old_comment)) {
    return services_error(t('Comment @cid not found', array('@cid' => $comment['cid'])), 404);
  }

  // Setup form_state.
  $form_state = array();
  $form_state['values'] = $comment;
  $form_state['values']['op'] = t('Save');
  $form_state['comment'] = $old_comment;

  drupal_execute('comment_form', $form_state, $old_comment);

  if ($errors = form_get_errors()) {
    return services_error(implode(" ", $errors), 406, array('form_errors' => $errors));
  }

  return $cid;
}

/**
 * Delete a comment.
 *
 * @param $cid
 *   Unique identifier of the comment to delete.
 * @return
 *   True.
 */
function _comment_resource_delete($cid) {
  // Load in the required includes for comment_delete.
  module_load_include('inc', 'comment', 'comment.admin');

  // The following is from comment_confirm_delete_submit in comment.admin.inc
  $comment = _comment_load($cid);
  if (empty($comment)) {
    return services_error(t('There is no comment found with id @cid', array('@cid' => $cid)), 404);
  }

  // Delete comment and its replies.
  _comment_delete_thread($comment);

  _comment_update_node_statistics($comment->nid);

  // Clear the cache so an anonymous user sees that his comment was deleted.
  cache_clear_all();
  return TRUE;
}

/**
 * Returns the comments of a specified node.
 *
 * @param $nid
 *   Unique identifier for the node.
 * @param $count
 *   Number of comments to return.
 * @param $start
 *   Which comment to start with. if present, $start and $count are used together
 *   to create a LIMIT clause for selecting comments. This could be used to do paging.
 * @return
 *   An array of comment objects.
 */
function _comment_resource_load_node_comments($nid, $count, $start) {
  $comments = array();
  $limit = ((int)$count > 0 ? ' LIMIT ' . (int)$start . ', ' . (int)$count . ' ' : '');

  $result = db_query("SELECT cid FROM {comments} WHERE nid = %d AND status = %d ORDER BY thread DESC" . $limit, $nid, COMMENT_PUBLISHED);
  while ($comment = db_fetch_array($result)) {
    $comments[] = _comment_load($comment['cid']);
  }

  return $comments;
}

/**
 * Returns the number of comments on a given node id.
 *
 * @param $nid
 *   Unique identifier for the specified node.
 * @return
 *   Number of comments that node has.
 */
function _comment_resource_count_all($nid) {
  return comment_num_all($nid);
}

/**
 * Returns the number of new comments on a given node id since timestamp.
 *
 * @param $nid
 *   Unique identifier for the specified node.
 * @param $since
 *   Timestamp to indicate what nodes are new. Defaults to time of last user acces to node.
 * @return
 *   Number of comments that node has.
 */
function _comment_resource_count_new($nid, $since = 0) {
  return comment_num_new($nid, $since);
}

/**
 * Access check callback for comment controllers.
 */
function _comment_resource_access($op = 'view', $args = array()) {
  if ($op == 'view' && !isset($args[0])) {
    return user_access('access comments');
  }

  if ($op == 'create') {
    $comment = (object)$args[0];
  }
  else {
    $comment = _comment_load($args[0]);
  }

  // If the submitted comment does not contain a nid, then return an error.
  if(isset($comment->nid)) {
    $node = node_load($comment->nid);
    if($op == 'create' && !$node->nid) {
      return services_error(t('Node nid: @nid does not exist.', array('@nid' => $comment->nid)), 406);
    }
  }
  if (user_access('administer comments')) {
    return TRUE;
  }
  switch ($op) {
    case 'view':
      // Check if the user has access to comments
      // and that the node has comments enabled.
      return ($comment->status == COMMENT_PUBLISHED) && user_access('access comments');
    case 'edit':
    case 'delete':
      // Check if the user may edit the comment, and has access to the input format
      return comment_access('edit', $comment);
    case 'create':
      // Check if the user may post comments, and has access to the used format and
      // check if the node has comments enabled, and that the user has access to the node
      return user_access('post comments');
  }
}
