<?php
// $Id$

function _comment_resource_definition() {
  if (!module_exists('comment')) {
    return array();
  }

  return array(
    'comment' => array(
      'create' => array(
        'help' => 'Creates a comment',
        'callback' => '_comment_resource_create',
        'access callback' => '_comment_resource_access',
        'access arguments' => array('create'),
        'access arguments append' => TRUE,
        'args' => array(
          array(
            'name' => 'comment',
            'type' => 'struct',
            'description' => 'The comment object',
            'source' => 'data',
            'optional' => FALSE,
          ),
        ),
      ),

      'retrieve' => array(
        'help' => 'Retrieves a comment',
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
            'type' => 'struct',
            'description' => 'The comment object with updated information',
            'source' => 'data',
            'optional' => FALSE,
          ),
        ),
      ),

      'delete' => array(
        'help' => 'Deletes a comment',
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
          'access callback' => '_comment_resource_node_access',
          'access arguments' => array('view'),
          'access arguments append' => TRUE,
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
            ),
            array(
              'name'         => 'start',
              'type'         => 'int',
              'description'  => t('If count is set to non-zero value, you can pass also non-zero value for start. For example to get comments from 5 to 15, pass count=10 and start=5.'),
              'source'       => 'data',
              'optional'     => TRUE,
            ),
          ),
        ),
        'countAll' => array(
          'file' => array('type' => 'inc', 'module' => 'services', 'name' => 'resources/comment_resource'),
          'help'   => t('This method returns the number of comments on a given node.'),
          'access callback' => '_comment_resource_node_access',
          'access arguments' => array('view'),
          'access arguments append' => TRUE,
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
          'access callback' => '_comment_resource_node_access',
          'access arguments' => array('view'),
          'access arguments append' => TRUE,
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
  if (!is_object($comment)) {
    $comment = (object) $comment;
  }

  // If the submitted comment does not contain a uid, set it to anonymous.
  if (!isset($comment->uid)) {
    $comment->uid = 0;
  }

  // If the submitted comment does not contain a nid, then return an error.
  if (!isset($comment->nid)) {
    return services_error(t("No node specified."));
  }

  // If a cid is present then the user is trying to edit an existing comment.
  if (isset($comment->cid) && ($comment->cid != 0)) {
    $initial_comment = _comment_load($comment->cid);
    $admin = user_access("administer comments");

    if ($initial_comment->uid == 0 && !$admin) {
      return services_error(t("Anonymous comments can't be edited"));
    }

    if (($initial_comment->uid != $comment->uid || comment_num_replies($comment->cid) != 0) && !$admin) {
      return services_error(t("User does not have permission to edit this comment"));
    }
  }

  // Can I just make a note here about how stupid it is that comment_load returns
  // an object but comment_save expects an array?
  $cid = comment_save((array) $comment);

  return (object)array(
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
  $comment->cid = $cid;

  $old_comment = (array) _comment_load($comment->cid);
  if ($old_comment['cid']) {
    // Setup form_state.
    $form_state = array();
    $form_state['values'] = (array) $comment;
    $form_state['values']['op'] = t('Save');
    $form_state['comment'] = $old_comment;

    drupal_execute('comment_form', $form_state, $old_comment);

    if ($errors = form_get_errors()) {
      return services_error(implode("\n", $errors), 406);
    }
  }
  else {
    return services_error(t('Comment not found'), 404);
  }

  return $comment->cid;
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
function _comment_resource_load_node_comments($nid, $count = 0, $start = 0) {
  $comments = array();
  $limit = ((int)$count > 0 ? ' LIMIT '. (int)$start .', '. (int)$count .' ' : '');

  $result = db_query("SELECT cid FROM {comments} WHERE nid = %d ORDER BY thread DESC". $limit, $nid);
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
  if (user_access('administer comments')) {
    return TRUE;
  }

  if ($op=='create') {
    $comment = (object)$args[0];
  }
  else {
    $comment = _comment_load($args[0]);
  }

  switch ($op) {
    case 'view':
      // Check if the user has access to comments and that the node has comments enabled
      return $comment->status && user_access('access comments') && _comment_resource_node_access($comment->nid);
    case 'edit':
      // Check if the user may edit the comment, and has access to the input format
      return comment_access('edit', $comment) && filter_access($comment->format);
    case 'create':
      // Check if the user may post comments, and has access to the used format and
      // check if the node has comments enabled, and that the user has access to the node
      return user_access('post comments') && filter_access($comment->format) &&
        _comment_resource_node_access($comment->nid, COMMENT_NODE_READ_WRITE);
    case 'delete':
      // Check if the user may edit the comment
      return comment_access('edit', $comment);
  }
}

/**
 * Passthrough function to _node_resource_access().
 *
 * @see _node_resource_access()
 **/
function _comment_resource_node_access($op = 'view', $args = array()) {
  module_load_include('inc', 'services', 'resources/node_resource');
  
  return _node_resource_access($op, $args);
}
