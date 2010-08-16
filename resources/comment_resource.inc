<?php
// $Id$

function _comment_resource_definition() {
  if (!module_exists('comment')) {
    return array();
  }

  return array(
    'comment' => array(

      'file' => array('type' => 'inc', 'module' => 'services', 'name' => 'resources/comment_resource'),

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
          'help'   => t('This method returns the number of new comments on a given node since a given timestamp.'),
          'access callback' => '_node_resource_access',
          'access arguments' => array('view'),
          'access arguments append' => TRUE,
          'callback' => '_comment_resource_load_node_comments',
          'args'     => array(
            array(
              'name'         => 'nid',
              'type'         => 'int',
              'description'  => t('The node id to load comments for.'),
              'source'       => array('path' => '0'),
              'optional'     => FALSE,
            ),
            array(
              'name'         => 'count',
              'type'         => 'int',
              'description'  => t('Number of comments to load.'),
              'source' => array('path' => '1'),
              'optional' => TRUE,
            ),
            array(
              'name'         => 'start',
              'type'         => 'int',
              'description'  => t('If count is set to non-zero value, you can pass also non-zero value for start. For example to get comments from 5 to 15, pass count=10 and start=5.'),
              'source'       => array('path' => '2'),
              'optional'     => TRUE,
            ),
          ),
        ),
        'countAll' => array(
          'help'   => t('This method returns the number of comments on a given node.'),
          'access callback' => '_node_resource_access',
          'access arguments' => array('view'),
          'access arguments append' => TRUE,
          'callback' => '_comment_resource_count_all',
          'args'     => array(
            array(
              'name'         => 'nid',
              'type'         => 'int',
              'description'  => t('The node id to count all comments.'),
              'source'       => array('path' => '0'),
              'optional'     => FALSE,
            ),
          ),
        ),
        'countNew' => array(
          'help'   => t('This method returns the number of new comments on a given node since a given timestamp.'),
          'access callback' => '_node_resource_access',
          'access arguments' => array('view'),
          'access arguments append' => TRUE,
          'callback' => '_comment_resource_count_new',
          'args'     => array(
            array(
              'name'          => 'nid',
              'type'          => 'int',
              'description'   => t('The node id to load comments for.'),
              'source'        => array('path' => '0'),
              'optional'      => FALSE,
            ),
            array(
              'name'         => 'since',
              'type'         => 'int',
              'optional'     => TRUE,
              'description'  => t('Timestamp to count from (defaults to time of last user acces to node).'),
              'source'        => array('path' => '1'),
              'optional'      => FALSE,
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
  $comment = (object)$comment;
  $cid = NULL;

  // Setup form_state
  $form_state = array();
  $form_state['values'] = (array)$comment;
  $form_state['values']['op'] = t('Save');
  $ret = drupal_execute('comment_form', $form_state, $comment);

  // Fetch $nid out of $form_state
  $cid = $form_state['cid'];

  if ($errors = form_get_errors()) {
    return services_error(implode("\n", $errors), 406);
  }

  return (object)array(
    'cid' => $cid,
    'uri' => services_resource_uri(array('comment', $nid)),
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
 * @param $comment
 *   An object as would be returned from comment_load().
 * @return
 *   Unique identifier for the comment (cid) or FALSE if there was a problem.
 */
function _comment_resource_update($comment) {
  $old_comment = _comment_load($comment['cid']);
  if ($old_comment['cid']) {
    // Setup form_state.
    $form_state = array();
    $form_state['values'] = $comment;
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

  return $comment['cid'];
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
